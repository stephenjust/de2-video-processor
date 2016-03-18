-- ---------------------------------------------------------------------------
-- Avalon Sequential Data Copier
--
-- This component copies bytes from one sequential area of memory to another
-- using an Avalon-MM master.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_arith.all;

-- Library containing FIFO components
LIBRARY altera_mf;
USE altera_mf.altera_mf_components.all;

ENTITY avalon_copy_sequential IS
GENERIC(
	DATA_WIDTH       : integer := 8
);
PORT(
	clk              : in     std_logic;
	reset            : in     std_logic;

	-- Control Signals
	s_address_start  : in     std_logic_vector(31 downto 0);
	s_address_end    : in     std_logic_vector(31 downto 0);
	d_address_start  : in     std_logic_vector(31 downto 0);
	skip_byte_en     : in     std_logic;
	skip_byte_value  : in     std_logic_vector(7 downto 0);
	start            : in     std_logic;
	done             : out    std_logic;

	-- Avalon-MM Master
	avm_read         : out    std_logic;
	avm_readdata     : in     std_logic_vector(DATA_WIDTH-1 downto 0);
	avm_readdatavalid: in     std_logic;
	avm_write        : buffer std_logic;
	avm_writedata    : buffer std_logic_vector(DATA_WIDTH-1 downto 0);
	avm_burstcount   : out    std_logic_vector(7 downto 0) := x"01";
	avm_address      : buffer std_logic_vector(31 downto 0);
	avm_waitrequest  : in     std_logic
);
END ENTITY avalon_copy_sequential;

ARCHITECTURE arch OF avalon_copy_sequential IS

	TYPE state IS (IDLE, RUNNING);

	SIGNAL count : integer;
	SIGNAL read_remaining : std_logic_vector(31 downto 0);
	SIGNAL fifo_empty : std_logic;
	SIGNAL current_state : state;
	SIGNAL is_reading : boolean;
	SIGNAL skipped_byte : std_logic := '0';

	FUNCTION min (a, b : std_logic_vector) RETURN std_logic_vector IS
	BEGIN
		IF a < b THEN
			RETURN a;
		ELSE
			RETURN b;
		END IF;
	END FUNCTION;

BEGIN

	-- Control logic
	control : PROCESS(clk)
		VARIABLE address_next : std_logic_vector(31 downto 0);
	BEGIN
		IF rising_edge(clk) THEN
			IF reset = '1' THEN
				current_state <= IDLE;
			ELSIF current_state = IDLE THEN
				done <= '0';
				avm_read <= '0';
				IF start = '1' THEN
					current_state <= RUNNING;
					read_remaining <= s_address_end - s_address_start;
					avm_read <= '1';
					avm_address <= s_address_start;
					--avm_burstcount <= min(read_remaining, x"20");
					count <= 1;
					is_reading <= true;
				ELSE
					current_state <= IDLE;
				END IF;
			ELSIF current_state = RUNNING THEN
				address_next := avm_address;

				IF is_reading THEN
					IF avm_readdatavalid = '1' THEN
						--avm_burstcount <= x"00";
						IF (DATA_WIDTH = 8 AND count >= read_remaining) OR (DATA_WIDTH = 16 AND 2*count >= read_remaining) OR count = 32 THEN
							is_reading <= false;
							address_next := s_address_end - read_remaining - conv_std_logic_vector(count, 32) + d_address_start - s_address_start;
							avm_read <= '0';
							avm_burstcount <= conv_std_logic_vector(count, 8);
							IF DATA_WIDTH = 8 THEN
								read_remaining <= read_remaining - conv_std_logic_vector(count, 32);
							ELSIF DATA_WIDTH = 16 THEN
								read_remaining <= read_remaining - conv_std_logic_vector(2*count, 32);
							END IF;
						ELSE
							IF DATA_WIDTH = 8 THEN
								address_next := avm_address + x"1";
							ELSIF DATA_WIDTH = 16 THEN
								address_next := avm_address + x"2";
							END IF;
							count <= count + 1;
						END IF;
					END IF;
				ELSE
					IF avm_waitrequest = '0' OR skipped_byte = '1' THEN
						--avm_burstcount <= x"00";
						IF count = 0 THEN
							IF read_remaining = 0 THEN
								current_state <= IDLE;
							ELSE
								address_next := s_address_end - read_remaining - conv_std_logic_vector(count, 32);
								avm_read <= '1';
								is_reading <= true;
								count <= 1;
							END IF;
						ELSE
							count <= count - 1;
							IF DATA_WIDTH = 8 THEN
								address_next := avm_address + x"1";
							ELSIF DATA_WIDTH = 16 THEN
								address_next := avm_address + x"2";
							END IF;
						END IF;
						
					END IF;
				END IF;
			END IF;
		END IF;
	END PROCESS control;

	avm_write <= '1' WHEN current_state = RUNNING
		AND is_reading = false
		AND ((avm_writedata = skip_byte_value AND skip_byte_en = '1') OR skip_byte_en = '0') ELSE '0';
	skipped_byte <= '1' WHEN skip_byte_en = '1' AND skip_byte_value = avm_writedata ELSE '0';

	-- Instantiate an altera-provided single-clock FIFO
	f0 : scfifo
	GENERIC MAP(
		lpm_width              => DATA_WIDTH,
		lpm_widthu             => 6,
		lpm_numwords           => 64,
		lpm_showahead          => "ON",
		lpm_type               => "SCFIFO",
		overflow_checking      => "OFF",
		underflow_checking     => "OFF",
		intended_device_family => "Cyclone II"
	)
	PORT MAP(
		clock       => clk,
		aclr        => reset,

		wrreq       => avm_readdatavalid,
		data        => avm_readdata,

		rdreq       => avm_write and not avm_waitrequest,
		q           => avm_writedata,

		empty       => fifo_empty
	);
END ARCHITECTURE arch;