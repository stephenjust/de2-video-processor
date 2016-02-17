-- ---------------------------------------------------------------------------
-- Video output DMA Manager
--
-- This component handles moving pixels between SDRAM, SRAM and FIFO. It
-- coordinates the flow of pixels and ensures that video data is always being
-- provided to the FIFO from the correct source, without tearing between
-- frames.
--
-- Assert swap_next_frame for one clock cycle to indicate to the DMA manager
-- that the data in SDRAM is ready to be copied to SRAM. The DMA manager will
-- then copy data from SDRAM to SRAM as soon as possible without causing a
-- tear in the video output.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;
USE ieee.numeric_std.all;

ENTITY video_fb_dma_manager IS 
	GENERIC(
		SRAM_BUF_START_ADDRESS   : std_logic_vector(31 downto 0) := (others => '0');
		SDRAM_BUF_START_ADDRESS  : std_logic_vector(31 downto 0) := (others => '0');
		FRAME_WIDTH              : integer                       := 640;
		FRAME_HEIGHT             : integer                       := 480
	);

	PORT (
		clk                  : in     std_logic;
		reset                : in     std_logic;

		-- Control Signals
		swap_trigger         : in     std_logic;
		swap_done            : buffer std_logic;

		-- FIFO source
		fifo_startofpacket   : buffer std_logic;
		fifo_endofpacket     : buffer std_logic;
		fifo_pixdata         : buffer std_logic_vector(15 downto 0);
		fifo_write           : buffer std_logic;
		fifo_ready           : in     std_logic;

		-- DMA Master 0 (for SRAM, Read-write)
		dma0_readdata        : in     std_logic_vector(15 downto 0);
		dma0_read            : buffer std_logic;
		dma0_writedata       : buffer std_logic_vector(15 downto 0);
		dma0_write           : buffer std_logic;
		dma0_readdatavalid   : in     std_logic;
		dma0_waitrequest     : in     std_logic;
		dma0_address         : buffer std_logic_vector(31 downto 0);	
		dma0_burstcount      : buffer std_logic_vector(7 downto 0);

		-- DMA Master 1 (for SDRAM, Read only)
		dma1_readdata        : in     std_logic_vector(15 downto 0);
		dma1_read            : buffer std_logic;
		dma1_readdatavalid   : in     std_logic;
		dma1_waitrequest     : in     std_logic;
		dma1_address         : buffer std_logic_vector(31 downto 0);
		dma1_burstcount      : buffer std_logic_vector(7 downto 0)
	);

END video_fb_dma_manager;

ARCHITECTURE Behaviour OF video_fb_dma_manager IS

	-- Component Definitions
	COMPONENT video_fb_sdram_reader IS 
		GENERIC(
			READ_BURST_SIZE : integer;
			DATA_SIZE       : integer;
			READY_THRESHOLD : integer;
			SDRAM_BUF_START_ADDRESS : std_logic_vector(31 downto 0)
		);
		PORT (
			clk                 : in     std_logic;
			reset               : in     std_logic;

			-- Interface to read SDRAM over Avalon Interface
			dma_readdata        : in     std_logic_vector(15 downto 0);
			dma_read            : buffer std_logic;
			dma_readdatavalid   : in     std_logic;
			dma_waitrequest     : in     std_logic;
			dma_address         : buffer std_logic_vector(31 downto 0);
			dma_burstcount      : buffer std_logic_vector(7 downto 0);

			-- Interface to output data as it is read
			out_ready           : buffer std_logic;
			out_data            : buffer std_logic_vector(15 downto 0);
			out_read            : in     std_logic;
			out_usedw           : buffer std_logic_vector(6 downto 0);

			-- Control signals
			copy_trigger        : in     std_logic;
			copy_done           : buffer std_logic
		);
	END COMPONENT video_fb_sdram_reader;

	-- Type definitions
	TYPE state IS (IDLE, READ_SRAM, WRITE_SRAM);

	-- Constants
	CONSTANT sram_burst_size : std_logic_vector(7 downto 0) := x"20"; -- 32
	CONSTANT sdram_burst_size : std_logic_vector(7 downto 0) := x"40"; -- 64
	CONSTANT frame_size : std_logic_vector(31 downto 0) := std_logic_vector(to_unsigned(FRAME_WIDTH * FRAME_HEIGHT, 32));

	-- Signals
	SIGNAL sram_read_offset : std_logic_vector(31 downto 0) := (others => '0');
	SIGNAL sram_read_count  : std_logic_vector(7 downto 0) := (others => '0');
	SIGNAL sram_write_offset : std_logic_vector(31 downto 0) := (others => '0');
	SIGNAL sram_write_count : std_logic_vector(7 downto 0) := (others => '0');

	SIGNAL swap_sdram_done    : std_logic;
	SIGNAL swap_read_count    : std_logic_vector(7 downto 0) := (others => '0');
	SIGNAL swap_copy_ready    : std_logic;
	SIGNAL swap_copy_read     : std_logic := '0';
	SIGNAL swap_copy_data     : std_logic_vector(15 downto 0);

	SIGNAL copy_on_next_frame : std_logic := '0';

	SIGNAL current_state : state := IDLE;
BEGIN

	-- Process to handle all communication with the SRAM
	sram_dma : PROCESS (clk)
		VARIABLE next_state : state;
	BEGIN
		IF rising_edge(clk) THEN
			IF reset = '1' THEN
				sram_read_offset  <= (others => '0');
				sram_read_count   <= (others => '0');
				sram_write_offset <= (others => '0');
				sram_write_count  <= (others => '0');
				dma0_read         <= '0';
				dma0_write        <= '0';
				dma0_burstcount   <= (others => '0');
				swap_read_count   <= (others => '0');
				copy_on_next_frame<= '0';
				current_state     <= IDLE;
			ELSE
				IF swap_trigger = '1' THEN
					copy_on_next_frame <= '1';
				END IF;

				-- TODO: Make sure we don't get tearing
				CASE current_state IS
					WHEN IDLE =>
						swap_done <= '0';
						IF swap_copy_ready = '1' AND fifo_ready <= '0' THEN
							-- Start the process to write to SRAM
							sram_write_count  <= (others => '0');
							dma0_address      <= SRAM_BUF_START_ADDRESS + sram_write_offset;
							dma0_burstcount   <= sram_burst_size;
							dma0_write        <= '1';
							next_state        := WRITE_SRAM;
						ELSIF fifo_ready = '1' THEN
							-- Start the process to read from SRAM
							sram_read_count <= (others => '0');
							dma0_address    <= SRAM_BUF_START_ADDRESS + sram_read_offset;
							dma0_burstcount <= sram_burst_size;
							dma0_read       <= '1';
							next_state      := READ_SRAM;
						ELSE
							dma0_read      <= '0';
							dma0_write     <= '0';
							next_state     := IDLE;
						END IF;

					WHEN WRITE_SRAM =>
						-- We need to hold the address and burstcount until waitrequest is de-asserted
						IF dma0_waitrequest = '0' THEN
							dma0_address    <= (others => '0');
							dma0_burstcount <= (others => '0');
						END IF;

						-- Write next word
						IF dma0_waitrequest = '0' THEN
							IF sram_write_count + '1' < sram_burst_size THEN
								sram_write_count <= sram_write_count + '1';
								next_state       := WRITE_SRAM;
							ELSE
								sram_write_count  <= (others => '0');
								dma0_write        <= '0';
								next_state        := IDLE;
								IF sram_write_offset + (sram_burst_size & '0') >= frame_size THEN
									sram_write_offset <= (others => '0');
									copy_on_next_frame <= '0';
									swap_done <= '1';
								ELSE
									sram_write_offset <= sram_write_offset + (sram_burst_size & '0');
								END IF;
							END IF;
						END IF;
							
					WHEN READ_SRAM =>
						-- We need to hold the address, burstcount and read signal until waitrequest is de-asserted
						IF dma0_waitrequest = '0' THEN
							dma0_read       <= '0';
							dma0_address    <= (others => '0');
							dma0_burstcount <= (others => '0');
						END IF;

						-- Handle the completion of a burst. The strange burst size math is a sloppy attempt
						-- to account for byte-wise addressing when we receive two bytes at a time.
						IF sram_read_count >= sram_burst_size THEN
							next_state := IDLE;
							-- Our pixel count is a multiple of the burst size, so we can get away with only
							-- checking that we have reached the end of a frame when the burst is over.
							IF (sram_read_offset + (sram_burst_size & '0') < frame_size) THEN
								sram_read_offset <= sram_read_offset + (sram_burst_size & '0');
							ELSE
								sram_read_offset <= (others => '0');
							END IF;
						ELSE
							next_state := READ_SRAM;
						END IF;
						-- Count the incoming data packets. Combinational logic below will actually
						-- pass the data to the FIFO.
						IF dma0_readdatavalid = '1' THEN
							sram_read_count <= sram_read_count + '1';
						END IF;

					WHEN OTHERS =>
						next_state := IDLE;
				END CASE;
				current_state <= next_state;
			END IF;
		END IF;
	END PROCESS sram_dma;

	-- Output of SRAM read is always input to FIFO so we can glue them
	-- directly together without any additional logic.
	fifo_pixdata       <= dma0_readdata;
	fifo_write         <= dma0_readdatavalid;
	fifo_startofpacket <= '1' WHEN OR_REDUCE(sram_read_offset + sram_read_count) = '0' ELSE '0';
	fifo_endofpacket   <= '1' WHEN sram_read_offset + sram_read_count >= (frame_size - x"2") ELSE '0';

	-- Combinational logic for SRAM write
	swap_copy_read     <= '1' WHEN current_state = WRITE_SRAM AND dma0_waitrequest = '0' ELSE '0';
	dma0_writedata     <= swap_copy_data;

	-- Initialize a DRAM reader
	r0 : video_fb_sdram_reader
	GENERIC MAP(
		READ_BURST_SIZE => 64,
		DATA_SIZE       => FRAME_WIDTH * FRAME_HEIGHT / 2,
		READY_THRESHOLD => 32,
		SDRAM_BUF_START_ADDRESS => SDRAM_BUF_START_ADDRESS
	)
	PORT MAP(
		clk                   => clk,
		reset                 => reset,

		dma_readdata          => dma1_readdata,
		dma_read              => dma1_read,
		dma_readdatavalid     => dma1_readdatavalid,
		dma_waitrequest       => dma1_waitrequest,
		dma_address           => dma1_address,
		dma_burstcount        => dma1_burstcount,

		out_ready             => swap_copy_ready,
		out_data              => swap_copy_data,
		out_read              => swap_copy_read,
		out_usedw             => open,

		copy_trigger          => swap_trigger,
		copy_done             => swap_sdram_done
	);

END Behaviour;
