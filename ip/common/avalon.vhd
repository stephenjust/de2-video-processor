-- ---------------------------------------------------------------------------
-- Avalon Package
--
-- The Avalon package contains common components to simplify some operations
-- on Avalon buses.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;

PACKAGE avalon IS

	COMPONENT avalon_write_sequential IS
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
		avm_burstcount   : out    std_logic_vector(7 downto 0);
		avm_address      : out    std_logic_vector(31 downto 0);
		avm_waitrequest  : in     std_logic
	);
	END COMPONENT avalon_write_sequential;

	COMPONENT avalon_copy_sequential IS
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
		avm_write        : out    std_logic;
		avm_writedata    : out    std_logic_vector(DATA_WIDTH-1 downto 0);
		avm_burstcount   : out    std_logic_vector(7 downto 0);
		avm_address      : buffer std_logic_vector(31 downto 0);
		avm_waitrequest  : in     std_logic
	);
	END COMPONENT avalon_copy_sequential;

END PACKAGE;
