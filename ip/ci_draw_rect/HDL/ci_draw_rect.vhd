-- ---------------------------------------------------------------------------
-- Custom Instruction: Draw Rectangle
--
-- This custom instruction draws a rectangle with a solid colour.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.numeric_std.all;
USE ieee.std_logic_1164.all;

USE work.geometry.all;

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
		avm_m0_byteenable  : buffer std_logic_vector(1 downto 0);
		avm_m0_address     : buffer std_logic_vector(31 downto 0);
		avm_m0_waitrequest : in     std_logic
	);
END ENTITY;

ARCHITECTURE Behavioral OF ci_draw_rect IS

	TYPE op_state IS (IDLE, RUNNING);

	CONSTANT FRAME_WIDTH : signed(15 downto 0) := to_signed(640, 16);
	CONSTANT FRAME_HEIGHT : signed(15 downto 0) := to_signed(480, 16);

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

	SIGNAL current_state : op_state := IDLE;
	SIGNAL current_x : std_logic_vector(15 downto 0);
	SIGNAL current_y : std_logic_vector(15 downto 0);

BEGIN

	-- Do operation
	op : PROCESS(ncs_ci_clk)
		VARIABLE next_x : std_logic_vector(15 downto 0);
		VARIABLE next_y : std_logic_vector(15 downto 0);
	BEGIN
		IF rising_edge(ncs_ci_clk) THEN
			IF reset = '1' OR ncs_ci_reset = '1' THEN
				current_state <= IDLE;
			ELSIF current_state = IDLE THEN
				ncs_ci_done <= '0';
				avm_m0_write <= '0';
				IF ncs_ci_start = '1' THEN
					current_state <= RUNNING;
					current_x <= x1;
					current_y <= y1;
				ELSE
					current_state <= IDLE;
				END IF;
			ELSIF current_state = RUNNING THEN
				avm_m0_burstcount <= x"01";
				avm_m0_writedata <= (color & color); -- Use byteenable to mask even/odd bytes
				avm_m0_write <= '1';
				next_x := current_x;
				next_y := current_y;

				-- Wait until write succeeds to advance to next pixel
				IF avm_m0_waitrequest = '0' THEN
					IF current_y = y2 and current_x(15 downto 1) = x2(15 downto 1) THEN
						-- Write completed
						current_state <= IDLE;
						avm_m0_write <= '0';
						ncs_ci_done <= '1';
					ELSIF current_x(15 downto 1) = x2(15 downto 1) THEN
						-- Advance to next row
						next_y := std_logic_vector(signed(current_y) + to_signed(1, 16));
						next_x := x1;
					ELSE
						-- Advance to next pixel
						IF current_x(0) = '1' THEN
							next_x := std_logic_vector(signed(current_x) + to_signed(1, 16));
						ELSE
							next_x := std_logic_vector(signed(current_x) + to_signed(2, 16));
						END IF;
					END IF;
				END IF;

				-- Check byte alignment to set byteenable
				-- If we are writing an odd X pixel (will only occur in the first column),
				-- then only write the most significant pixel. If we are at the end of a
				-- line, and the pixel being drawn is of even index, then we only write the
				-- least significant pixel. Otherwise, write both pixels.
				IF next_x(0) = '1' THEN
					avm_m0_byteenable <= b"10";
				ELSIF next_x(15 downto 1) = x2(15 downto 1) and x2(0) = '0' THEN
					avm_m0_byteenable <= b"01";
				ELSE
					avm_m0_byteenable <= b"11";
				END IF;

				avm_m0_address <= std_logic_vector(unsigned(buf_addr) + unsigned(next_x) + (unsigned(FRAME_WIDTH) * unsigned(next_y)));
				current_x <= next_x;
				current_y <= next_y;
			END IF;
		END IF;
	END PROCESS op;

	-- Update configuration registers
	-- In the case of drawing a rectangle, we can do some bounds-checking on
	-- the inputs now. In other cases (such as drawing a line) we might want
	-- to allow coordinates outside of the frame window, which would be clipped
	-- during the memory-writing stage of command execution.
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
						x1 <= std_logic_vector(clip(signed(avs_s0_writedata(15 downto 0)), to_signed(0, 16), FRAME_WIDTH - to_signed(1, 16)));
					WHEN REG_Y1 =>
						y1 <= std_logic_vector(clip(signed(avs_s0_writedata(15 downto 0)), to_signed(0, 16), FRAME_HEIGHT - to_signed(1, 16)));
					WHEN REG_X2 =>
						x2 <= std_logic_vector(clip(signed(avs_s0_writedata(15 downto 0)), to_signed(0, 16), FRAME_WIDTH - to_signed(1, 16)));
					WHEN REG_Y2 =>
						y2 <= std_logic_vector(clip(signed(avs_s0_writedata(15 downto 0)), to_signed(0, 16), FRAME_HEIGHT - to_signed(1, 16)));
					WHEN REG_COLOR =>
						color <= avs_s0_writedata(7 downto 0);
					WHEN OTHERS =>
						-- Do nothing
				END CASE;
			END IF;
		END IF;
	END PROCESS update_cfg;

END ARCHITECTURE;
