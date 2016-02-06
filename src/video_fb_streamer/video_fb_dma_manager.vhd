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

END video_fb_dma_manager;

ARCHITECTURE Behaviour OF video_fb_dma_manager IS

BEGIN

END Behaviour;
