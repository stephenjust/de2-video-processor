-- ---------------------------------------------------------------------------
-- Custom Instruction: Copy Rectangular Buffer
--
-- This custom instruction copies a (potentially discontinuous) rectangular
-- buffer to a continuous rectangular buffer.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.numeric_std.all;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_misc.all;

USE work.avalon.all;
USE work.geometry.all;

ENTITY ci_copy_rect_16 IS
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
		avs_s0_address   : in     std_logic_vector(3 downto 0);

		-- Memory-mapped master for DMA read/write
		avm_m0_read        : buffer std_logic;
		avm_m0_readdata    : in     std_logic_vector(15 downto 0);
		avm_m0_readdatavalid:in     std_logic;
		avm_m0_write       : buffer std_logic;
		avm_m0_writedata   : buffer std_logic_vector(15 downto 0);
		avm_m0_burstcount  : buffer std_logic_vector(7 downto 0);
		avm_m0_address     : buffer std_logic_vector(31 downto 0);
		avm_m0_byteenable  : buffer std_logic_vector(1 downto 0);
		avm_m0_waitrequest : in     std_logic
	);
END ENTITY;

ARCHITECTURE Behavioral OF ci_copy_rect_16 IS

	TYPE op_state IS (IDLE, RUNNING);

	CONSTANT REG_SRC_BUF_ADDR  : std_logic_vector(3 downto 0) := B"0000";
	CONSTANT REG_SRC_W         : std_logic_vector(3 downto 0) := B"0001";
	CONSTANT REG_SRC_H         : std_logic_vector(3 downto 0) := B"0010";
	CONSTANT REG_DEST_BUF_ADDR : std_logic_vector(3 downto 0) := B"0011";
	CONSTANT REG_DEST_W        : std_logic_vector(3 downto 0) := B"0100";
	CONSTANT REG_DEST_H        : std_logic_vector(3 downto 0) := B"0101";
	CONSTANT REG_SRC_RECT_X1   : std_logic_vector(3 downto 0) := B"0110";
	CONSTANT REG_SRC_RECT_Y1   : std_logic_vector(3 downto 0) := B"0111";
	CONSTANT REG_SRC_RECT_X2   : std_logic_vector(3 downto 0) := B"1000";
	CONSTANT REG_SRC_RECT_Y2   : std_logic_vector(3 downto 0) := B"1001";
	CONSTANT REG_DEST_RECT_X1  : std_logic_vector(3 downto 0) := B"1010";
	CONSTANT REG_DEST_RECT_Y1  : std_logic_vector(3 downto 0) := B"1011";

	SIGNAL current_state : op_state := IDLE;
	SIGNAL current_point, current_dest_point : point_t;

	-- Copy Control Signals
	SIGNAL copy_src_address_start, copy_src_address_end, copy_dest_address_start : std_logic_vector (31 downto 0);
	SIGNAL copy_start, copy_done : std_logic;

	-- Configuration registers
	SIGNAL source_buffer : pixbuf_t;
	SIGNAL dest_buffer : pixbuf_t;
	SIGNAL source_rect : rect_t;
	SIGNAL dest_rect : rect_t;

BEGIN

	-- Do operation
	op : PROCESS(ncs_ci_clk)
		variable next_point, next_dest_point : point_t;
		variable source_rect_width : unsigned(15 downto 0);
	BEGIN
		IF rising_edge(ncs_ci_clk) THEN
			IF reset = '1' OR ncs_ci_reset = '1' THEN
				current_state <= IDLE;
			ELSIF current_state = IDLE THEN
				ncs_ci_done <= '0';
				IF ncs_ci_start = '1' THEN
					source_rect_width := rect_width(source_rect);
					current_state <= RUNNING;
					current_point <= source_rect.p1;
					current_dest_point <= dest_rect.p1;
					copy_src_address_start <= std_logic_vector(pixbuf_pixel_address(source_buffer, source_rect.p1));
					copy_src_address_end <= std_logic_vector(pixbuf_pixel_address(source_buffer, source_rect.p1) + source_rect_width - to_unsigned(1, 16));
					copy_dest_address_start <= std_logic_vector(pixbuf_pixel_address(dest_buffer, dest_rect.p1));
					copy_start <= '1';
				ELSE
					current_state <= IDLE;
				END IF;
			ELSIF current_state = RUNNING THEN
				next_point := current_point;
				next_dest_point := current_dest_point;
				copy_start <= '0';
				IF copy_done = '1' THEN
					IF current_point.y >= source_rect.p2.y THEN
						-- Write completed
						current_state <= IDLE;
						ncs_ci_done <= '1';
					ELSE
						source_rect_width := rect_width(source_rect);
						next_point.y := current_point.y + to_signed(1, 16);
						next_dest_point.y := current_dest_point.y + to_signed(1, 16);
						copy_src_address_start <= std_logic_vector(pixbuf_pixel_address(source_buffer, next_point));
						copy_src_address_end <= std_logic_vector(pixbuf_pixel_address(source_buffer, next_point) + source_rect_width - to_unsigned(1, 16));
						copy_dest_address_start <= std_logic_vector(pixbuf_pixel_address(dest_buffer, next_dest_point));
						copy_start <= '1';
					END IF;
				END IF;
				current_point <= next_point;
				current_dest_point <= next_dest_point;
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
				source_buffer.address <= (others => '0');
				dest_buffer.address <= (others => '0');
			ELSIF avs_s0_write = '1' THEN
				CASE avs_s0_address IS
					WHEN REG_SRC_BUF_ADDR =>
						source_buffer.address <= unsigned(avs_s0_writedata);
					WHEN REG_SRC_W =>
						source_buffer.rect.p1.x <= to_signed(0, 16);
						source_buffer.rect.p2.x <= signed(avs_s0_writedata(15 downto 0)) - to_signed(1, 16);
					WHEN REG_SRC_H =>
						source_buffer.rect.p1.y <= to_signed(0, 16);
						source_buffer.rect.p2.y <= signed(avs_s0_writedata(15 downto 0)) - to_signed(1, 16);
					WHEN REG_DEST_BUF_ADDR =>
						dest_buffer.address <= unsigned(avs_s0_writedata);
					WHEN REG_DEST_W =>
						dest_buffer.rect.p1.x <= to_signed(0, 16);
						dest_buffer.rect.p2.x <= signed(avs_s0_writedata(15 downto 0)) - to_signed(1, 16);
					WHEN REG_DEST_H =>
						dest_buffer.rect.p1.y <= to_signed(0, 16);
						dest_buffer.rect.p2.y <= signed(avs_s0_writedata(15 downto 0)) - to_signed(1, 16);
					WHEN REG_SRC_RECT_X1 =>
						source_rect.p1.x <= signed(avs_s0_writedata(15 downto 0));
					WHEN REG_SRC_RECT_Y1 =>
						source_rect.p1.y <= signed(avs_s0_writedata(15 downto 0));
					WHEN REG_SRC_RECT_X2 =>
						source_rect.p2.x <= signed(avs_s0_writedata(15 downto 0));
					WHEN REG_SRC_RECT_Y2 =>
						source_rect.p2.y <= signed(avs_s0_writedata(15 downto 0));
					WHEN REG_DEST_RECT_X1 =>
						dest_rect.p1.x <= signed(avs_s0_writedata(15 downto 0));
					WHEN REG_DEST_RECT_Y1 =>
						dest_rect.p1.y <= signed(avs_s0_writedata(15 downto 0));
					WHEN OTHERS =>
						-- Do nothing
				END CASE;
			END IF;
		END IF;
	END PROCESS update_cfg;

	writer: avalon_copy_sequential_16
	PORT MAP (
		clk              => ncs_ci_clk,
		reset            => ncs_ci_reset,

		-- Control Signals
		s_address_start  => copy_src_address_start,
		s_address_end    => copy_src_address_end,
		d_address_start  => copy_dest_address_start,
		start            => copy_start,
		done             => copy_done,

		-- Avalon-MM Master
		avm_read         => avm_m0_read,
		avm_readdata     => avm_m0_readdata,
		avm_readdatavalid=> avm_m0_readdatavalid,
		avm_write        => avm_m0_write,
		avm_writedata    => avm_m0_writedata,
		avm_burstcount   => avm_m0_burstcount,
		avm_address      => avm_m0_address,
		avm_byteenable   => avm_m0_byteenable,
		avm_waitrequest  => avm_m0_waitrequest
	);
END ARCHITECTURE;
