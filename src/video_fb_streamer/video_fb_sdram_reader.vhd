-- ---------------------------------------------------------------------------
-- SDRAM Pixel Buffer Reader
--
-- This component loads pixel data from the SDRAM in bursts and outputs it via
-- a FIFO to other components. There are also signals to indicate when the
-- frame has been completely copied, and when there is more than a certain
-- threshold of data available in the FIFO.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.numeric_std.all;

-- Library containing FIFO components
LIBRARY altera_mf;
USE altera_mf.altera_mf_components.all;

ENTITY video_fb_sdram_reader IS 
	GENERIC(
		READ_BURST_SIZE : integer := 64;           -- Read from SDRAM in bursts of 128
		DATA_SIZE       : integer := 640 * 480 / 2; -- Read this number of 16-bit words
		READY_THRESHOLD : integer := 32;            -- Assert a flag when the FIFO has this much data
		SDRAM_BUF_START_ADDRESS : std_logic_vector(31 downto 0) := (others => '0')
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

END video_fb_sdram_reader;

ARCHITECTURE Behaviour OF video_fb_sdram_reader IS

	TYPE state IS (IDLE, READ_SDRAM, WAIT_FIFO);

	CONSTANT dram_burst_size : std_logic_vector(7 downto 0) := std_logic_vector(to_unsigned(READ_BURST_SIZE, 8));

	SIGNAL current_state, next_state : state := IDLE;
	SIGNAL dram_read_count   : std_logic_vector(7 downto 0);
	SIGNAL dram_read_offset  : std_logic_vector(31 downto 0);
	SIGNAL full              : std_logic;

BEGIN

	PROCESS (clk)
		VARIABLE next_state : state;
	BEGIN
		IF rising_edge(clk) THEN
			IF reset = '1' THEN
				dma_address    <= (others => '0');
				dma_burstcount <= (others => '0');
				dma_read       <= '0';
				current_state  <= IDLE;
			ELSE
				CASE current_state IS
					WHEN IDLE =>
						IF copy_trigger = '1' THEN
							dram_read_count <= (others => '0');
							dram_read_offset <= (others => '0');
							dma_address <= SDRAM_BUF_START_ADDRESS;
							dma_burstcount <= dram_burst_size;
							dma_read <= '1';
							next_state := READ_SDRAM;
						ELSE
							copy_done <= '0';
							next_state := IDLE;
						END IF;

					WHEN READ_SDRAM =>
						-- Deassert control signals once waitrequest goes low
						IF dma_waitrequest = '0' THEN
							dma_read <= '0';
							dma_address <= (others => '0');
							dma_burstcount <= (others => '0');
						END IF;
						-- Handle the completion of a burst
						IF dram_read_count >= dram_burst_size THEN
							IF (dram_read_offset + dram_burst_size) < std_logic_vector(to_unsigned(DATA_SIZE, 32)) THEN
								dram_read_offset <= dram_read_offset + dram_burst_size;
								next_state := WAIT_FIFO;
							ELSE
								dram_read_offset <= (others => '0');
								next_state := IDLE;
								copy_done <= '1';
							END IF;
						ELSE
							-- Count the incoming data. Combinational logic connects SDRAM to FIFO.
							IF dma_readdatavalid = '1' THEN
								dram_read_count <= dram_read_count + '1';
							END IF;
							next_state := READ_SDRAM;
						END IF;

					WHEN WAIT_FIFO =>
						IF out_ready = '1' OR full = '1' THEN
							next_state := WAIT_FIFO;
						ELSE
							dram_read_count <= (others => '0');
							dma_address <= SDRAM_BUF_START_ADDRESS + dram_read_offset;
							dma_burstcount <= dram_burst_size;
							dma_read <= '1';
							next_state := READ_SDRAM;
						END IF;

					WHEN OTHERS =>
						next_state := IDLE;
				END CASE;
				current_state <= next_state;
			END IF;
		END IF;
	END PROCESS;
	
	-- Instantiate an altera-provided single-clock FIFO
	f0 : scfifo
	GENERIC MAP(
		lpm_width              => 16,
		lpm_widthu             => 7,
		lpm_numwords           => 128,
		lpm_showahead          => "ON",
		lpm_type               => "SCFIFO",
		overflow_checking      => "OFF",
		underflow_checking     => "OFF",
		almost_full_value      => READY_THRESHOLD,
		intended_device_family => "Cyclone II"
	)
	PORT MAP(
		clock       => clk,
		aclr        => reset,

		wrreq       => dma_readdatavalid,
		data        => dma_readdata,

		rdreq       => out_read,
		q           => out_data,

		usedw       => out_usedw,
		full        => full,
		almost_full => out_ready
	);

END Behaviour;
