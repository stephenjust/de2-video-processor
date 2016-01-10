-- Top Level for de2_video_processor
library ieee;

use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity de2_video_processor is

	port
	(
		-- 50 MHz Clock
		CLOCK_50     : in       std_logic;

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

		-- VGA output
		VGA_R        : out      std_logic_vector (9 downto 0);
		VGA_G        : out      std_logic_vector (9 downto 0);
		VGA_B        : out      std_logic_vector (9 downto 0);
		VGA_CLK      : out      std_logic;
		VGA_BLANK    : out      std_logic;
		VGA_HS       : out      std_logic;
		VGA_VS       : out      std_logic;
		VGA_SYNC     : out      std_logic;

		-- Input switches and buttons
		SW           : in       std_logic_vector (17 downto 0);
		KEY          : in       std_logic_vector (3 downto 0)
	);
end de2_video_processor;

architecture structure of de2_video_processor is
begin

end structure;
