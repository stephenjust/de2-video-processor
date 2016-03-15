-- Top Level for de2_video_processor
library ieee;

use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity de2_video_processor is

	port
	(
		-- Clocks
		CLOCK_50     : in       std_logic;
		CLOCK_27     : in       std_logic;

		-- SDRAM on board
		DRAM_ADDR    : out      std_logic_vector (11 downto 0);
		DRAM_BA_0    : out      std_logic;
		DRAM_BA_1    : out      std_logic;
		DRAM_CAS_N   : out      std_logic;
		DRAM_CKE     : out      std_logic;
		DRAM_CLK     : out      std_logic;
		DRAM_CS_N    : out      std_logic;
		DRAM_DQ      : inout    std_logic_vector (15 downto 0);
		DRAM_LDQM    : out      std_logic;
		DRAM_UDQM    : out      std_logic;
		DRAM_RAS_N   : out      std_logic;
		DRAM_WE_N    : out      std_logic;

		-- SRAM on board
		SRAM_ADDR    : out      std_logic_vector (17 downto 0);
		SRAM_DQ      : inout    std_logic_vector (15 downto 0);
		SRAM_WE_N    : out      std_logic;
		SRAM_OE_N    : out      std_logic;
		SRAM_UB_N    : out      std_logic;
		SRAM_LB_N    : out      std_logic;
		SRAM_CE_N    : out      std_logic;

		-- Flash memory
		FL_ADDR     :  out   std_logic_vector (21 downto 0);
		FL_CE_N     :  out   std_logic_vector (0 downto 0);
		FL_OE_N     :  out   std_logic_vector (0 downto 0);
		FL_DQ       :  inout std_logic_vector (7 downto 0);
		FL_RST_N    :  out   std_logic_vector (0 downto 0) := (others => '0');
		FL_WE_N     :  out   std_logic_vector (0 downto 0);

		-- SD Card Interface
		SD_CLK      : out   std_logic;
		SD_CMD      : inout std_logic;
		SD_DAT      : inout std_logic;
		SD_DAT3     : inout std_logic;

		-- VGA output
		VGA_R        : out      std_logic_vector (9 downto 0);
		VGA_G        : out      std_logic_vector (9 downto 0);
		VGA_B        : out      std_logic_vector (9 downto 0);
		VGA_CLK      : out      std_logic;
		VGA_BLANK    : out      std_logic;
		VGA_HS       : out      std_logic;
		VGA_VS       : buffer      std_logic;
		VGA_SYNC     : out      std_logic;

		-- Input switches and buttons
		SW           : in       std_logic_vector (17 downto 0);
		KEY          : in       std_logic_vector (3 downto 0);

		-- 7-Segment LEDs
		HEX0         : out      std_logic_vector (6 downto 0) := (others => '1');
		HEX1         : out      std_logic_vector (6 downto 0) := (others => '1');
		HEX2         : out      std_logic_vector (6 downto 0) := (others => '1');
		HEX3         : out      std_logic_vector (6 downto 0) := (others => '1');
		HEX4         : out      std_logic_vector (6 downto 0) := (others => '1');
		HEX5         : out      std_logic_vector (6 downto 0) := (others => '1');
		HEX6         : out      std_logic_vector (6 downto 0) := (others => '1');
		HEX7         : out      std_logic_vector (6 downto 0) := (others => '1');
		
		-- GPIO Port 0 
		GPIO_0 : inout std_logic_vector (35 downto 0) := (others => 'Z');
		-- GPIO Port 1
		GPIO_1 : inout std_logic_vector (35 downto 0) := (others => 'Z')
	);
end de2_video_processor;

architecture structure of de2_video_processor is
	component de2_video_processor_system is
	port (
		clk_clk                                         : in    std_logic                     := 'X';             -- 50M clock
		reset_reset_n                                   : in    std_logic                     := 'X';             -- 50M clock reset
		altpll_0_c0_clk                                 : out   std_logic;                                        -- SDRAM clock
		clk_0_clk                                       : in    std_logic                     := 'X';             -- 27M clock
		reset_0_reset_n                                 : in    std_logic;                                        -- 27M clock reset
		sdram_0_wire_addr                               : out   std_logic_vector(11 downto 0);                    -- addr
		sdram_0_wire_ba                                 : out   std_logic_vector(1 downto 0);                     -- ba
		sdram_0_wire_cas_n                              : out   std_logic;                                        -- cas_n
		sdram_0_wire_cke                                : out   std_logic;                                        -- cke
		sdram_0_wire_cs_n                               : out   std_logic;                                        -- cs_n
		sdram_0_wire_dq                                 : inout std_logic_vector(15 downto 0) := (others => 'X'); -- dq
		sdram_0_wire_dqm                                : out   std_logic_vector(1 downto 0);                     -- dqm
		sdram_0_wire_ras_n                              : out   std_logic;                                        -- ras_n
		sdram_0_wire_we_n                               : out   std_logic;                                        -- we_n
		sram_0_external_interface_DQ                    : inout std_logic_vector(15 downto 0) := (others => 'X'); -- DQ
		sram_0_external_interface_ADDR                  : out   std_logic_vector(17 downto 0);                    -- ADDR
		sram_0_external_interface_LB_N                  : out   std_logic;                                        -- LB_N
		sram_0_external_interface_UB_N                  : out   std_logic;                                        -- UB_N
		sram_0_external_interface_CE_N                  : out   std_logic;                                        -- CE_N
		sram_0_external_interface_OE_N                  : out   std_logic;                                        -- OE_N
		sram_0_external_interface_WE_N                  : out   std_logic;                                        -- WE_N
		video_vga_controller_0_external_interface_CLK   : out   std_logic;                                        -- CLK
		video_vga_controller_0_external_interface_HS    : out   std_logic;                                        -- HS
		video_vga_controller_0_external_interface_VS    : out   std_logic;                                        -- VS
		video_vga_controller_0_external_interface_BLANK : out   std_logic;                                        -- BLANK
		video_vga_controller_0_external_interface_SYNC  : out   std_logic;                                        -- SYNC
		video_vga_controller_0_external_interface_R     : out   std_logic_vector(9 downto 0);                     -- R
		video_vga_controller_0_external_interface_G     : out   std_logic_vector(9 downto 0);                     -- G
		video_vga_controller_0_external_interface_B     : out   std_logic_vector(9 downto 0);                     -- B
		genesis_0_conduit_end_vsync                     : in    std_logic                     := 'X';             -- vsync
		genesis_0_conduit_end_dpad_up_input1            : in    std_logic                     := 'X';             -- dpad_up_input1
		genesis_0_conduit_end_dpad_down_input1          : in    std_logic                     := 'X';             -- dpad_down_input1
		genesis_0_conduit_end_dpad_left_input1          : in    std_logic                     := 'X';             -- dpad_left_input1
		genesis_0_conduit_end_dpad_right_input1         : in    std_logic                     := 'X';             -- dpad_right_input1
		genesis_0_conduit_end_select_input1             : out   std_logic;                                        -- select_input1
		genesis_0_conduit_end_start_c_input1            : in    std_logic                     := 'X';             -- start_c_input1
		genesis_0_conduit_end_ab_input1                 : in    std_logic                     := 'X';             -- ab_input1
		genesis_0_conduit_end_dpad_up_input2            : in    std_logic                     := 'X';             -- dpad_up_input2
		genesis_0_conduit_end_dpad_down_input2          : in    std_logic                     := 'X';             -- dpad_down_input2
		genesis_0_conduit_end_dpad_left_input2          : in    std_logic                     := 'X';             -- dpad_left_input2
		genesis_0_conduit_end_dpad_right_input2         : in    std_logic                     := 'X';             -- dpad_right_input2
		genesis_0_conduit_end_select_input2             : out   std_logic;                                        -- select_input2
		genesis_0_conduit_end_start_c_input2            : in    std_logic                     := 'X';             -- start_c_input2
		genesis_0_conduit_end_ab_input2                 : in    std_logic                     := 'X';             -- ab_input2
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_read_n_out       : out   std_logic_vector(0 downto 0);
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_data_out         : inout std_logic_vector(7 downto 0) := (others => 'X');
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_chipselect_n_out : out   std_logic_vector(0 downto 0);
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_write_n_out      : out   std_logic_vector(0 downto 0);
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_address_out      : out   std_logic_vector(21 downto 0);
		spi_0_external_MISO                                                              : in    std_logic                     := 'X';             -- MISO
		spi_0_external_MOSI                                                              : out   std_logic;                                        -- MOSI
		spi_0_external_SCLK                                                              : out   std_logic;                                        -- SCLK
		spi_0_external_SS_n                                                              : out   std_logic                                         -- SS_n
	);
end component de2_video_processor_system;

	-- Signals to interface with DRAM
	signal BA	: std_logic_vector (1 downto 0);
	signal DQM	:	std_logic_vector (1 downto 0);
	
begin
	DRAM_BA_1 <= BA(1);
	DRAM_BA_0 <= BA(0);
	
	DRAM_UDQM <= DQM(1);
	DRAM_LDQM <= DQM(0);

	u0 : component de2_video_processor_system
		port map (
		clk_clk                                         => CLOCK_50,              --                                       clk.clk
		reset_reset_n                                   => KEY(0),                --                                     reset.reset_n
		altpll_0_c0_clk                                 => DRAM_CLK,
		clk_0_clk                                       => CLOCK_27,
		reset_0_reset_n                                 => KEY(0),
		sdram_0_wire_addr                               => DRAM_ADDR,             --                              sdram_0_wire.addr
		sdram_0_wire_ba                                 => BA,                    --                                          .ba
		sdram_0_wire_cas_n                              => DRAM_CAS_N,            --                                          .cas_n
		sdram_0_wire_cke                                => DRAM_CKE,              --                                          .cke
		sdram_0_wire_cs_n                               => DRAM_CS_N,             --                                          .cs_n
		sdram_0_wire_dq                                 => DRAM_DQ,               --                                          .dq
		sdram_0_wire_dqm                                => DQM,                   --                                          .dqm
		sdram_0_wire_ras_n                              => DRAM_RAS_N,            --                                          .ras_n
		sdram_0_wire_we_n                               => DRAM_WE_N,             --                                          .we_n
		sram_0_external_interface_DQ                    => SRAM_DQ,                    --                 sram_0_external_interface.DQ
		sram_0_external_interface_ADDR                  => SRAM_ADDR,             --                                          .ADDR
		sram_0_external_interface_LB_N                  => SRAM_LB_N,             --                                          .LB_N
		sram_0_external_interface_UB_N                  => SRAM_UB_N,             --                                          .UB_N
		sram_0_external_interface_CE_N                  => SRAM_CE_N,             --                                          .CE_N
		sram_0_external_interface_OE_N                  => SRAM_OE_N,             --                                          .OE_N
		sram_0_external_interface_WE_N                  => SRAM_WE_N,             --                                          .WE_N
		video_vga_controller_0_external_interface_CLK   => VGA_CLK,   -- video_vga_controller_0_external_interface.CLK
		video_vga_controller_0_external_interface_HS    => VGA_HS,    --                                          .HS
		video_vga_controller_0_external_interface_VS    => VGA_VS,    --                                          .VS
		video_vga_controller_0_external_interface_BLANK => VGA_BLANK, --                                          .BLANK
		video_vga_controller_0_external_interface_SYNC  => VGA_SYNC,  --                                          .SYNC
		video_vga_controller_0_external_interface_R     => VGA_R,     --                                          .R
		video_vga_controller_0_external_interface_G     => VGA_G,     --                                          .G
		video_vga_controller_0_external_interface_B     => VGA_B,     --                                          .B
		genesis_0_conduit_end_vsync                     => VGA_VS,                -- Might want to do VGA_CLK instead, 
		genesis_0_conduit_end_dpad_up_input1            => GPIO_1(9),            --                                          .dpad_up_input1
		genesis_0_conduit_end_dpad_down_input1          => GPIO_1(13),            --                                          .dpad_down_input1
		genesis_0_conduit_end_dpad_left_input1          => GPIO_1(17),            --                                          .dpad_left_input1
		genesis_0_conduit_end_dpad_right_input1         => GPIO_1(19),            --                                          .dpad_right_input1
		genesis_0_conduit_end_select_input1             => GPIO_1(15),            --                                          .select_input1
		genesis_0_conduit_end_start_c_input1            => GPIO_1(21),            --                                          .start_c_input1
		genesis_0_conduit_end_ab_input1                 => GPIO_1(11),            --                                          .ab_input1
		genesis_0_conduit_end_dpad_up_input2            => GPIO_1(23),            --                                          .dpad_up_input2
		genesis_0_conduit_end_dpad_down_input2          => GPIO_1(27),            --                                          .dpad_down_input2
		genesis_0_conduit_end_dpad_left_input2          => GPIO_1(31),            --  This might not be available..(36)           .dpad_left_input2
		genesis_0_conduit_end_dpad_right_input2         => GPIO_1(33),            --  This might not be available..(38)           .dpad_right_input2
		genesis_0_conduit_end_select_input2             => GPIO_1(29),            --                                          .select_input2
		genesis_0_conduit_end_start_c_input2            => GPIO_1(35),            --  This might not be available..(40)           .start_c_input2
		genesis_0_conduit_end_ab_input2                 => GPIO_1(25),            --
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_read_n_out       => FL_OE_N,
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_data_out         => FL_DQ,
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_chipselect_n_out => FL_CE_N,
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_write_n_out      => FL_WE_N,
		tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_address_out      => FL_ADDR,
		spi_0_external_MISO                                                              => SD_DAT,            --                            spi_0_external.MISO
		spi_0_external_MOSI                                                              => SD_CMD,            --                                          .MOSI
		spi_0_external_SCLK                                                              => SD_CLK,            --                                          .SCLK
		spi_0_external_SS_n                                                              => SD_DAT3            --                                          .SS_n
	);

end structure;
