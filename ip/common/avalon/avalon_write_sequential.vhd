-- ---------------------------------------------------------------------------
-- Avalon Sequential Data Writer
--
-- This component writes a byte to a sequential address space using an
-- Avalon-MM master interface.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;

ENTITY avalon_write_sequential IS
PORT(
	clk              : in     std_logic;
	reset            : in     std_logic;

	-- Control Signals
	data             : in     std_logic_vector(7 downto 0);
	address_start    : in     std_logic_vector(31 downto 0);
	address_end      : in     std_logic_vector(31 downto 0);
	start            : in     std_logic;
	done             : out    std_logic;

	-- Avalon-MM Master
	avm_write        : out    std_logic;
	avm_writedata    : out    std_logic_vector(15 downto 0);
	avm_byteenable   : out    std_logic_vector(1 downto 0);
	avm_burstcount   : out    std_logic_vector(7 downto 0) := x"01";
	avm_address      : buffer std_logic_vector(31 downto 0);
	avm_waitrequest  : in     std_logic
);
END ENTITY avalon_write_sequential;

ARCHITECTURE arch OF avalon_write_sequential IS

	TYPE state IS (IDLE, RUNNING);

	FUNCTION byteenable( current_address, start_address, end_address : std_logic_vector(31 downto 0))
	RETURN std_logic_vector IS
	BEGIN
		IF current_address(0) = '1' THEN
			RETURN B"10";
		ELSIF current_address(31 downto 1) = end_address(31 downto 1) and end_address(0) = '0' THEN
			RETURN B"01";
		ELSE
			RETURN B"11";
		END IF;
	END FUNCTION;

	SIGNAL current_state : state;

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
				avm_write <= '0';
				IF start = '1' THEN
					current_state <= RUNNING;
					avm_write <= '1';
					avm_address <= (address_start(31 downto 1) & '0');
					avm_byteenable <= byteenable(address_start, address_start, address_end);
					avm_writedata <= (data & data);
					--avm_burstcount <= std_logic_vector(address_end - (address_start(31 downto 1) & '0') + x"1")(8 downto 1);
				ELSE
					current_state <= IDLE;
				END IF;
			ELSIF current_state = RUNNING THEN
				address_next := avm_address;

				-- Wait until write succeeds to advance to next chunk
				IF avm_waitrequest = '0' THEN
					IF avm_address(31 downto 1) >= address_end(31 downto 1) THEN
						avm_write <= '0';
						current_state <= IDLE;
						done <= '1';
					ELSE
						address_next := avm_address + B"10";
						--avm_burstcount <= std_logic_vector(address_end - address_next + x"1")(8 downto 1);
						avm_byteenable <= byteenable(address_next, address_start, address_end);
						avm_address <= address_next;
					END IF;
				END IF;
			END IF;
		END IF;
	END PROCESS control;

END ARCHITECTURE arch;