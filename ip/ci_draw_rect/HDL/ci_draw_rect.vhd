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

USE work.avalon.all;
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
	SIGNAL point1, point2, current_point : point_t := ((others => '0'), (others => '0'));
	SIGNAL color : std_logic_vector(7 downto 0) := (others => '0');

	-- Avalon Writer Signals
	SIGNAL writer_start, writer_done : std_logic;
	SIGNAL writer_address_start, writer_address_end : std_logic_vector(31 downto 0);

	SIGNAL current_state : op_state := IDLE;

	FUNCTION point_address ( base_address : std_logic_vector; point : point_t )
	RETURN std_logic_vector IS
	BEGIN
		RETURN std_logic_vector(unsigned(base_address) + unsigned(point.y * FRAME_WIDTH + point.x));
	END FUNCTION;

BEGIN

	-- Do operation
	op : PROCESS(ncs_ci_clk)
		variable next_point : point_t;
	BEGIN
		IF rising_edge(ncs_ci_clk) THEN
			IF reset = '1' OR ncs_ci_reset = '1' THEN
				current_state <= IDLE;
			ELSIF current_state = IDLE THEN
				ncs_ci_done <= '0';
				IF ncs_ci_start = '1' THEN
					current_state <= RUNNING;
					current_point <= point1;
					writer_address_start <= point_address(buf_addr, point1);
					writer_address_end <= std_logic_vector(unsigned(point_address(buf_addr, point1)) + unsigned(point2.x - point1.x));
					writer_start <= '1';
				ELSE
					current_state <= IDLE;
				END IF;
			ELSIF current_state = RUNNING THEN
				next_point := current_point;
				writer_start <= '0';
				IF writer_done = '1' THEN
					IF current_point.y >= point2.y THEN
						-- Write completed
						current_state <= IDLE;
						ncs_ci_done <= '1';
					ELSE
						next_point.y := current_point.y + to_signed(1, 16);
						writer_address_start <= point_address(buf_addr, next_point);
						writer_address_end <= std_logic_vector(unsigned(point_address(buf_addr, next_point)) + unsigned(point2.x - point1.x));
						writer_start <= '1';
					END IF;
				END IF;
				current_point <= next_point;
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
				point1 <= ((others => '0'), (others => '0'));
				point2 <= ((others => '0'), (others => '0'));
				color <= (others => '0');
			ELSIF avs_s0_write = '1' THEN
				CASE avs_s0_address IS
					WHEN REG_BUF_ADDR =>
						buf_addr <= avs_s0_writedata;
					WHEN REG_X1 =>
						point1.x <= clip(signed(avs_s0_writedata(15 downto 0)), to_signed(0, 16), FRAME_WIDTH - to_signed(1, 16));
					WHEN REG_Y1 =>
						point1.y <= clip(signed(avs_s0_writedata(15 downto 0)), to_signed(0, 16), FRAME_HEIGHT - to_signed(1, 16));
					WHEN REG_X2 =>
						point2.x <= clip(signed(avs_s0_writedata(15 downto 0)), to_signed(0, 16), FRAME_WIDTH - to_signed(1, 16));
					WHEN REG_Y2 =>
						point2.y <= clip(signed(avs_s0_writedata(15 downto 0)), to_signed(0, 16), FRAME_HEIGHT - to_signed(1, 16));
					WHEN REG_COLOR =>
						color <= avs_s0_writedata(7 downto 0);
					WHEN OTHERS =>
						-- Do nothing
				END CASE;
			END IF;
		END IF;
	END PROCESS update_cfg;

	writer: avalon_write_sequential
	PORT MAP (
		clk              => ncs_ci_clk,
		reset            => ncs_ci_reset,

		-- Control Signals
		data             => color,
		address_start    => writer_address_start,
		address_end      => writer_address_end,
		start            => writer_start,
		done             => writer_done,

		-- Avalon-MM Master
		avm_write        => avm_m0_write,
		avm_writedata    => avm_m0_writedata,
		avm_byteenable   => avm_m0_byteenable,
		avm_burstcount   => avm_m0_burstcount,
		avm_address      => avm_m0_address,
		avm_waitrequest  => avm_m0_waitrequest
	);
END ARCHITECTURE;
