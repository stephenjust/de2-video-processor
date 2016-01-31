LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;
USE ieee.numeric_std.all;

ENTITY video_fb_streamer IS 
	GENERIC(
		BUFFER_START_ADDRESS : std_logic_vector(31 downto 0) := (others => '0');
		BITS_PER_PIXEL       : integer                       := 8;
		FRAME_WIDTH          : integer                       := 640;
		FRAME_HEIGHT         : integer                       := 480
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

		-- DMA Master
		avm_dma0_readdata        : in    std_logic_vector(15 downto 0);
		avm_dma0_readdatavalid	 : in    std_logic;
		avm_dma0_waitrequest     : in    std_logic;
		avm_dma0_address         : out   std_logic_vector(31 downto 0);	
		avm_dma0_burstcount      : out   std_logic_vector(7 downto 0) := x"01";
		avm_dma0_read            : out   std_logic
	);

END video_fb_streamer;

ARCHITECTURE Behaviour OF video_fb_streamer IS
	-- Internal Wires
	SIGNAL pixel                       : std_logic_vector (7 downto 0);

	-- Internal Registers
	SIGNAL x                         : std_logic_vector (9 downto 0);
	SIGNAL y                         : std_logic_vector (8 downto 0);
BEGIN

	-- Internal Registers
	PROCESS (pix_clk)
	BEGIN
		IF rising_edge(pix_clk) THEN
			IF (reset = '1') THEN
				x <= (OTHERS => '0');
			ELSIF (aso_source_ready = '1') THEN
				IF (x = (FRAME_WIDTH - 1)) THEN
					x <= (OTHERS => '0');
				ELSE
					x <= x + '1';
				END IF;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (pix_clk)
	BEGIN
		IF rising_edge(pix_clk) THEN
			IF (reset = '1') THEN
				y <= (OTHERS => '0');
			ELSIF ((aso_source_ready = '1') AND (x = (FRAME_WIDTH - 1))) THEN
				IF (y = (FRAME_HEIGHT - 1)) THEN
					y <= (OTHERS => '0');
				ELSE
					y <= y + '1';
				END IF;
			END IF;
		END IF;
	END PROCESS;


	-- Output Assignments
	aso_source_data            <= pixel;
	aso_source_startofpacket   <= '1' WHEN ((x = 0) AND (y = 0)) ELSE '0';
	aso_source_endofpacket     <= '1' WHEN ((x = (FRAME_WIDTH - 1)) AND (y = (FRAME_HEIGHT - 1))) ELSE '0';
	aso_source_valid           <= '1';

	-- Internal Assignments
	pixel        <= x"AA";


END Behaviour;
