LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;
USE ieee.numeric_std.all;

-- Library containing FIFO components
LIBRARY altera_mf;
USE altera_mf.altera_mf_components.all;

ENTITY video_fb_streamer IS 
	GENERIC(
		SRAM_BUF_START_ADDRESS   : std_logic_vector(31 downto 0) := (others => '0');
		SDRAM_BUF_START_ADDRESS  : std_logic_vector(31 downto 0) := (others => '0');
		BITS_PER_PIXEL           : integer                       := 8;
		FRAME_WIDTH              : integer                       := 640;
		FRAME_HEIGHT             : integer                       := 480
	);

	PORT (
		clk                      : in    std_logic;
		pix_clk                  : in    std_logic;
		reset                    : in    std_logic;

		coe_swap_frame           : in    std_logic;

		-- Avalon-ST Interface
		aso_source_ready         : in    std_logic;
		aso_source_data          : out   std_logic_vector(BITS_PER_PIXEL-1 downto 0);
		aso_source_startofpacket : out   std_logic;
		aso_source_endofpacket   : out   std_logic;
		aso_source_valid         : out   std_logic;

		-- DMA Master 0 (for SRAM, Read-write)
		avm_dma0_readdata        : in     std_logic_vector(15 downto 0);
		avm_dma0_read            : buffer std_logic;
		avm_dma0_writedata       : buffer std_logic_vector(15 downto 0);
		avm_dma0_write           : buffer std_logic;
		avm_dma0_readdatavalid	 : in     std_logic;
		avm_dma0_waitrequest     : in     std_logic;
		avm_dma0_address         : buffer std_logic_vector(31 downto 0);	
		avm_dma0_burstcount      : buffer std_logic_vector(7 downto 0);

		-- DMA Master 1 (for SDRAM, Read only)
		avm_dma1_readdata        : in     std_logic_vector(15 downto 0);
		avm_dma1_read            : buffer std_logic;
		avm_dma1_readdatavalid   : in     std_logic;
		avm_dma1_waitrequest     : in     std_logic;
		avm_dma1_address         : buffer std_logic_vector(31 downto 0);
		avm_dma1_burstcount      : buffer std_logic_vector(7 downto 0)
	);

END video_fb_streamer;

ARCHITECTURE Behaviour OF video_fb_streamer IS
	-- Type definitions
	TYPE state IS (INIT, INIT_SRAM, SRAM_TO_FIFO, IDLE);

	-- Component definitions
	COMPONENT video_fb_fifo IS 
		PORT (
			in_clk                   : in    std_logic;
			out_clk                  : in    std_logic;
			reset                    : in    std_logic;

			in_startofpacket         : in    std_logic;
			in_endofpacket           : in    std_logic;
			in_pixdata               : in    std_logic_vector (15 downto 0);

			out_startofpacket        : out   std_logic;
			out_endofpacket          : out   std_logic;
			out_pixdata              : out   std_logic_vector (7 downto 0);

			in_req                   : in    std_logic;
			out_req                  : in    std_logic;

			full                     : out   std_logic;
			empty                    : out   std_logic;
			almost_empty             : out   std_logic
		);
	END COMPONENT video_fb_fifo;

	COMPONENT video_fb_dma_burst_reader IS 
		PORT (
			clk                 : in     std_logic;
			reset               : in     std_logic;

			ctl_start           : in     std_logic;
			ctl_burstcount      : in     std_logic_vector(7 downto 0);
			ctl_busy            : buffer std_logic := '0';

			dma_read            : buffer std_logic;
			dma_readdatavalid	  : in     std_logic;
			dma_waitrequest     : in     std_logic;
			dma_address         : buffer std_logic_vector(31 downto 0);
			dma_burstcount      : buffer std_logic_vector(7 downto 0)
		);
	END COMPONENT video_fb_dma_burst_reader;

	-- Constants
	CONSTANT FRAME_ADDR_COUNT        : integer := (FRAME_WIDTH * FRAME_HEIGHT / 2);

	-- Internal Registers
	SIGNAL pix                       : integer := 0;

	SIGNAL dma_read_startaddress     : std_logic_vector (31 downto 0);
	SIGNAL dma_read_burstcount       : std_logic_vector (7 downto 0);
	SIGNAL dma_read_start            : std_logic := '0';
	SIGNAL dma_read_busy             : std_logic;

	SIGNAL fifo_input_data           : std_logic_vector (15 downto 0);
	SIGNAL fifo_input_startofpacket  : std_logic;
	SIGNAL fifo_input_endofpacket    : std_logic;
	SIGNAL fifo_write_next           : std_logic;
	SIGNAL fifo_write_full           : std_logic;

	SIGNAL fifo_output_data          : std_logic_vector (7 downto 0);
	SIGNAL fifo_output_empty         : std_logic;
	
	SIGNAL current_state             : state := INIT;
	SIGNAL next_state                : state;

	SIGNAL sram_written              : integer := 0;
BEGIN

	PROCESS (clk)
	BEGIN
		IF rising_edge(clk) THEN
			IF (reset = '1') THEN
				fifo_input_data <= (others => '0');
				fifo_write_next <= '0';
				pix <= 0;
			ELSIF (fifo_write_full = '0') THEN
				IF (pix = FRAME_ADDR_COUNT - 1) THEN
					pix <= 0;
				ELSE
					pix <= pix + 1;
				END IF;
				fifo_input_data <= std_logic_vector(to_unsigned(pix, fifo_input_data'length));
				fifo_write_next <= '1';
			ELSE
				fifo_input_data <= (others => '0');
				fifo_write_next <= '0';
			END IF;
		END IF;
	END PROCESS;

	fifo_input_startofpacket   <= '1' WHEN pix = 0 ELSE '0';
	fifo_input_endofpacket     <= '1' WHEN pix = FRAME_ADDR_COUNT - 1 ELSE '0';
	aso_source_valid           <= not fifo_output_empty;

	-- Instantiate Components
	f0 : video_fb_fifo
	PORT MAP(
		in_clk            => clk,
		out_clk           => pix_clk,
		reset             => reset,

		in_startofpacket  => fifo_input_startofpacket,
		in_endofpacket    => fifo_input_endofpacket,
		in_pixdata        => fifo_input_data,

		out_startofpacket => aso_source_startofpacket,
		out_endofpacket   => aso_source_endofpacket,
		out_pixdata       => aso_source_data,

		in_req            => fifo_write_next,
		out_req           => aso_source_ready,

		full              => fifo_write_full,
		empty             => fifo_output_empty,
		almost_empty      => open
	);

	d0 : video_fb_dma_burst_reader
	PORT MAP(
		clk                => clk,
		reset              => reset,

		ctl_start          => dma_read_start,
		ctl_burstcount     => dma_read_burstcount,
		ctl_busy           => dma_read_busy,

		dma_read           => avm_dma0_read,
		dma_readdatavalid	 => avm_dma0_readdatavalid,
		dma_waitrequest    => avm_dma0_waitrequest,
		dma_address        => avm_dma0_address,
		dma_burstcount     => avm_dma0_burstcount
	);

END Behaviour;
