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
	avm_readdata     : in     std_logic_vector(7 downto 0);
	avm_readdatavalid: in     std_logic;
	avm_write        : buffer std_logic;
	avm_writedata    : buffer std_logic_vector(7 downto 0);
	avm_burstcount   : out    std_logic_vector(7 downto 0) := x"01";
	avm_address      : buffer std_logic_vector(31 downto 0);
	avm_waitrequest  : in     std_logic
);
END ENTITY avalon_copy_sequential;

ARCHITECTURE arch OF avalon_copy_sequential IS

	TYPE state IS (IDLE, RUNNING);

	SIGNAL count : integer := 0;
	SIGNAL read_remaining : std_logic_vector(31 downto 0);
	SIGNAL fifo_empty, fifo_full : std_logic;
	SIGNAL current_state : state;
	SIGNAL is_reading : boolean;
	SIGNAL skipped_byte : std_logic := '0';
	SIGNAL chunk_offset : std_logic_vector(31 downto 0);
	SIGNAL do_write : std_logic;
	SIGNAL fifo_read, fifo_write, fifo_reset : std_logic;
	SIGNAL fifo_usedw : std_logic_vector(5 downto 0);

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
				do_write <= '0';
				IF start = '1' THEN
					current_state <= RUNNING;
					read_remaining <= s_address_end - s_address_start + '1';
					avm_read <= '1';
					avm_address <= s_address_start;
					count <= 0;
					chunk_offset <= (others => '0');
					is_reading <= true;
					avm_burstcount <= min(conv_std_logic_vector(64, 32), (s_address_end - s_address_start + '1'))(7 downto 0);
				ELSE
					current_state <= IDLE;
				END IF;
			ELSIF current_state = RUNNING THEN
				address_next := avm_address;

				IF is_reading THEN
					-- For read bursts, de-assert read and the burstcount
					-- after the transfer is acknowledged.
					IF avm_waitrequest = '0' THEN
						avm_read <= '0';
						avm_burstcount <= x"01";
					END IF;
					IF (count + 1) >= read_remaining OR (count + 1) = 64 THEN
						-- Done reading chunk. Start writing.
						is_reading <= false;
						address_next := d_address_start + chunk_offset;
						chunk_offset <= chunk_offset + conv_std_logic_vector(count+1, 32);
						avm_read <= '0';
						read_remaining <= read_remaining - conv_std_logic_vector(count+1, 32);
						do_write <= '1';
						avm_burstcount <= x"01";
						count <= count + 1;
					ELSIF avm_readdatavalid = '1' THEN
						address_next := avm_address + x"1";
						count <= count + 1;
					END IF;
				ELSE
					IF avm_waitrequest = '0' OR skipped_byte = '1' THEN
						avm_burstcount <= x"01";
						IF count = 1 THEN
							do_write <= '0';
							IF read_remaining = 0 THEN
								done <= '1';
								current_state <= IDLE;
							ELSE
								avm_burstcount <= min(conv_std_logic_vector(64, 32), read_remaining)(7 downto 0);
								address_next := s_address_start + chunk_offset;
								avm_read <= '1';
								is_reading <= true;
								count <= 0;
							END IF;
						ELSE
							count <= count - 1;
							address_next := avm_address + x"1";
						END IF;
						
					END IF;
				END IF;
				avm_address <= address_next;
			END IF;
		END IF;
	END PROCESS control;

	avm_write <= '1' WHEN current_state = RUNNING
		AND do_write = '1'
		AND ((avm_writedata /= skip_byte_value AND skip_byte_en = '1') OR skip_byte_en = '0') ELSE '0';
	skipped_byte <= '1' WHEN do_write = '1' AND skip_byte_en = '1' AND skip_byte_value = avm_writedata ELSE '0';

	fifo_write <= avm_readdatavalid and not fifo_full;
	fifo_read <= ((avm_write and not avm_waitrequest) or skipped_byte) and not fifo_empty;
	fifo_reset <= reset or start;

	-- Instantiate an altera-provided single-clock FIFO
	f0 : scfifo
	GENERIC MAP(
		lpm_width              => 8,
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
		aclr        => fifo_reset,

		wrreq       => fifo_write,
		data        => avm_readdata,

		rdreq       => fifo_read,
		q           => avm_writedata,

		usedw       => fifo_usedw,
		full        => fifo_full,
		empty       => fifo_empty
	);
END ARCHITECTURE arch;