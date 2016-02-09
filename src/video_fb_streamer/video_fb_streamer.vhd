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

	COMPONENT video_fb_dma_manager IS 
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
			swap_next_frame      : in     std_logic;

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
	END COMPONENT video_fb_dma_manager;

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
	SIGNAL fifo_almost_empty         : std_logic;
	
	SIGNAL current_state             : state := INIT;
	SIGNAL next_state                : state;

	SIGNAL sram_written              : integer := 0;
BEGIN

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
		in_req            => fifo_write_next,

		out_startofpacket => aso_source_startofpacket,
		out_endofpacket   => aso_source_endofpacket,
		out_pixdata       => aso_source_data,
		out_req           => aso_source_ready,

		full              => fifo_write_full,
		empty             => fifo_output_empty,
		almost_empty      => fifo_almost_empty
	);

	d0 : video_fb_dma_manager 
	GENERIC MAP (
		SRAM_BUF_START_ADDRESS   => SRAM_BUF_START_ADDRESS,
		SDRAM_BUF_START_ADDRESS  => SDRAM_BUF_START_ADDRESS,
		FRAME_WIDTH              => FRAME_WIDTH,
		FRAME_HEIGHT             => FRAME_HEIGHT
	)
	PORT MAP (
		clk                  => clk,
		reset                => reset,

		-- Control Signals
		swap_next_frame      => '0',

		-- FIFO source
		fifo_startofpacket   => fifo_input_startofpacket,
		fifo_endofpacket     => fifo_input_endofpacket,
		fifo_pixdata         => fifo_input_data,
		fifo_write           => fifo_write_next,
		fifo_ready           => fifo_almost_empty,

		-- DMA Master 0 (for SRAM, Read-write)
		dma0_readdata        => avm_dma0_readdata,
		dma0_read            => avm_dma0_read,
		dma0_writedata       => avm_dma0_writedata,
		dma0_write           => avm_dma0_write,
		dma0_readdatavalid   => avm_dma0_readdatavalid,
		dma0_waitrequest     => avm_dma0_waitrequest,
		dma0_address         => avm_dma0_address,
		dma0_burstcount      => avm_dma0_burstcount,

		-- DMA Master 1 (for SDRAM, Read only)
		dma1_readdata        => avm_dma1_readdata,
		dma1_read            => avm_dma1_read,
		dma1_readdatavalid   => avm_dma1_readdatavalid,
		dma1_waitrequest     => avm_dma1_waitrequest,
		dma1_address         => avm_dma1_address,
		dma1_burstcount      => avm_dma1_burstcount
	);

END Behaviour;
