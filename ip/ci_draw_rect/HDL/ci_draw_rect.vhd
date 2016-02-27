-- ---------------------------------------------------------------------------
-- Custom Instruction: Draw Rectangle
--
-- This custom instruction draws a rectangle with a solid colour.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY ci_draw_rect IS
	PORT (
		clk              : in     std_logic; -- Only keeping QSYS happy - use ncs_ci_clk
		reset            : in     std_logic; -- Only keeping QSYS happy - use ncs_ci_reset

		-- Custom instruction ports
		ncs_ci_clk       : in     std_logic;
		ncs_ci_clk_en    : in     std_logic;
		ncs_ci_reset     : in     std_logic;
		ncs_ci_start     : in     std_logic;
		ncs_ci_done      : buffer std_logic;

		-- Memory-mapped slave for config registers
		avs_s0_write     : in     std_logic;
		avs_s0_writedata : in     std_logic_vector(31 downto 0);
		avs_s0_address   : in     std_logic_vector(2 downto 0);

		-- Memory-mapped master for DMA write
		avm_m0_write       : buffer std_logic;
		avm_m0_writedata   : buffer std_logic_vector(15 downto 0);
		avm_m0_burstcount  : buffer std_logic_vector(7 downto 0);
		avm_m0_address     : buffer std_logic_vector(31 downto 0);
		avm_m0_waitrequest : in     std_logic
	);
END ENTITY;

ARCHITECTURE Behavioral OF ci_draw_rect IS

	CONSTANT REG_BUF_ADDR : std_logic_vector(2 downto 0) := B"000";
	CONSTANT REG_X1 : std_logic_vector(2 downto 0) := B"001";
	CONSTANT REG_Y1 : std_logic_vector(2 downto 0) := B"010";
	CONSTANT REG_X2 : std_logic_vector(2 downto 0) := B"011";
	CONSTANT REG_Y2 : std_logic_vector(2 downto 0) := B"100";
	CONSTANT REG_COLOR : std_logic_vector(2 downto 0) := B"101";

	-- Configuration registers
	SIGNAL buf_addr : std_logic_vector(31 downto 0) := (others => '0');
	SIGNAL x1 : std_logic_vector(15 downto 0) := (others => '0');
	SIGNAL y1 : std_logic_vector(15 downto 0) := (others => '0');
	SIGNAL x2 : std_logic_vector(15 downto 0) := (others => '0');
	SIGNAL y2 : std_logic_vector(15 downto 0) := (others => '0');
	SIGNAL color : std_logic_vector(7 downto 0) := (others => '0');
	
BEGIN

	-- Do operation
	op : PROCESS(ncs_ci_clk)
	BEGIN
		IF rising_edge(ncs_ci_clk) THEN
			ncs_ci_done <= ncs_ci_start;
		END IF;
	END PROCESS op;

	-- Update configuration registers
	update_cfg : PROCESS(ncs_ci_clk)
	BEGIN
		IF rising_edge(ncs_ci_clk) THEN
			IF ncs_ci_reset = '1' THEN
				buf_addr <= (others => '0');
				x1 <= (others => '0');
				y1 <= (others => '0');
				x2 <= (others => '0');
				y2 <= (others => '0');
				color <= (others => '0');
			ELSIF avs_s0_write = '1' THEN
				CASE avs_s0_address IS
					WHEN REG_BUF_ADDR =>
						buf_addr <= avs_s0_writedata;
					WHEN REG_X1 =>
						x1 <= avs_s0_writedata(15 downto 0);
					WHEN REG_Y1 =>
						x2 <= avs_s0_writedata(15 downto 0);
					WHEN REG_X2 =>
						y1 <= avs_s0_writedata(15 downto 0);
					WHEN REG_Y2 =>
						y2 <= avs_s0_writedata(15 downto 0);
					WHEN REG_COLOR =>
						color <= avs_s0_writedata(7 downto 0);
					WHEN OTHERS =>
						-- Do nothing
				END CASE;
			END IF;
		END IF;
	END PROCESS update_cfg;

END ARCHITECTURE;
