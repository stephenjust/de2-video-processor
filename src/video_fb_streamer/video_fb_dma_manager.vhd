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

			-- Control signals
			copy_trigger        : in     std_logic;
			copy_done           : buffer std_logic
		);
	END COMPONENT video_fb_sdram_reader;

	-- Constants
	CONSTANT sram_burst_size : std_logic_vector(7 downto 0) := x"20"; -- 32
	CONSTANT sdram_burst_size : std_logic_vector(7 downto 0) := x"40"; -- 64
	CONSTANT frame_size : std_logic_vector(31 downto 0) := std_logic_vector(to_unsigned(FRAME_WIDTH * FRAME_HEIGHT, 32));

	-- Signals
	SIGNAL sram_read_offset : std_logic_vector(31 downto 0) := (others => '0');
	SIGNAL sram_read_count  : std_logic_vector(7 downto 0) := (others => '0');
	SIGNAL sram_busy        : std_logic := '0';
	SIGNAL sram_ack         : std_logic := '0';

	SIGNAL sdram_read_waiting : std_logic := '0';
	SIGNAL sdram_read_active  : std_logic := '0';
	SIGNAL sdram_read_offset  : std_logic_vector(31 downto 0) := (others => '0');
	SIGNAL sdram_read_count   : std_logic_vector(7 downto 0) := (others => '0');

BEGIN

	-- Process to handle the SDRAM to SRAM copy
	sdram_dma : PROCESS (clk)
	BEGIN
		IF rising_edge(clk) THEN
			IF swap_trigger = '1' THEN
				sdram_read_waiting <= '1';
			END IF;
			-- Placeholder for now
			IF sdram_read_waiting = '1' AND OR_REDUCE(sram_read_offset) = '0' THEN
				sdram_read_waiting <= '0';
				sdram_read_active <= '1';
				sdram_read_offset <= (others => '0');
				sdram_read_count <= sdram_burst_size;
			ELSIF sdram_read_active <= '1' AND sdram_read_offset = frame_size THEN
				sdram_read_active <= '0';
				swap_done <= '1';
			ELSE
				swap_done <= '0';
			END IF;
		END IF;
	END PROCESS sdram_dma;

	-- Process to handle all communication with the SRAM
	sram_dma : PROCESS (clk)
	BEGIN
		IF rising_edge(clk) THEN
			-- TODO: Frame swap logic
			IF reset = '1' THEN
				sram_ack <= '0';
				sram_busy <= '0';
				sram_read_offset <= (others => '0');
				sram_read_count <= (others => '0');
				dma0_read <= '0';
			ELSIF sram_busy = '0' AND fifo_ready = '1' THEN
				-- Start the process to read from SRAM
				sram_busy <= '1';
				sram_ack <= '0';
				sram_read_count <= (others => '0');
				dma0_address <= SRAM_BUF_START_ADDRESS + sram_read_offset;
				dma0_burstcount <= sram_burst_size;
				dma0_read <= '1';
			ELSIF sram_busy = '1' THEN
				-- We need to hold our read request until the SRAM de-asserts waitrequest
				IF sram_ack = '0' AND dma0_waitrequest = '0' THEN
					dma0_read <= '0';
					dma0_address <= (others => '0');
					dma0_burstcount <= (others => '0');
					sram_ack <= '1';
				END IF;
				-- Handle the completion of a burst. The strange burst size math is a sloppy attempt
				-- to account for byte-wise addressing when we receive two bytes at a time.
				IF sram_read_count >= sram_burst_size THEN
					sram_busy <= '0';
					-- Our pixel count is a multiple of the burst size, so we can get away with only
					-- checking that we have reached the end of a frame when the burst is over.
					IF (sram_read_offset + (sram_burst_size & '0') < frame_size) THEN
						sram_read_offset <= sram_read_offset + (sram_burst_size & '0');
					ELSE
						sram_read_offset <= (others => '0');
					END IF;
				END IF;
				-- Count the incoming data packets. Combinational logic below will actually
				-- pass the data to the FIFO.
				IF dma0_readdatavalid = '1' THEN
					sram_read_count <= sram_read_count + '1';
				END IF;
			END IF;
		END IF;
	END PROCESS sram_dma;

	-- Output of SRAM read is always input to FIFO so we can glue them
	-- directly together without any additional logic.
	fifo_pixdata       <= dma0_readdata;
	fifo_write         <= dma0_readdatavalid;
	fifo_startofpacket <= '1' WHEN OR_REDUCE(sram_read_offset + sram_read_count) = '0' ELSE '0';
	fifo_endofpacket   <= '1' WHEN sram_read_offset + sram_read_count >= (frame_size - x"2") ELSE '0';

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

		out_ready             => open,
		out_data              => open,
		out_read              => '0',

		copy_trigger          => '0',
		copy_done             => open
	);

END Behaviour;
