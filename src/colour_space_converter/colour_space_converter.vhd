--------------------------------------------------------------------------------
--
--	Dolphin Colour Space Converter Block.
--
--	ECE 492; Group #6
--
--	Stefan Martynkiw, Stephen Just, Mason Strong
--	February 5, 2016
--
--
--------------------------------------------------------------------------------


-- Design Documentation:
-- https://docs.google.com/document/d/1hGzqokSSvFV6MEvv3N6uqPJHyXQNLz6tmUEMOjyIg2M/edit?usp=sharing


LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;
USE ieee.numeric_std.all;

-- Library containing FIFO components
LIBRARY altera_mf;
USE altera_mf.altera_mf_components.all;


-- http://jjackson.eng.ua.edu/courses/ece680/lectures/qsys_components.pdf 
-- Page 9 for prefixes.

entity colour_space_converter is 

	generic(
		BUFFER_START_ADDRESS : std_logic_vector(31 downto 0) := (others => '0');
		BITS_PER_PIXEL       : integer                       := 8;
		FRAME_WIDTH          : integer                       := 640;
		FRAME_HEIGHT         : integer                       := 480;


        -- Width of Avalon Streaming Source to VGA Output.
        VGA_OUTPUT_STREAM_WIDTH       : integer                       := 16;
        VGA_INPUT_STREAM_WIDTH       : integer                       := 16

	);
    port (

        ------------------------------------------------------------------------
        -- Clock interface (to CPU) 
        clk                             : in std_logic;
		-- reset interface (Magic avalon reset)
		reset_n		                    : in std_logic;
        ------------------------------------------------------------------------
        

        ------------------------------------------------------------------------
        -- Avalon Memory-Mapped Master 
        -- (for interfacing with SRAM)
        ----- Interface Prefix: avm
        avm_sraminterf_read_n	        : out    std_logic;
        avm_sraminterf_readdata         : in   std_logic_vector (31 downto 0);
        avm_sraminterf_address          : out    std_logic_vector (3 downto 0);
        --byteenable?
        avm_sraminterf_write_n          : out    std_logic;
        avm_sraminterf_writedata        : out    std_logic_vector (31 downto 0);
        --avm_sraminterf_reset_n          : in    std_logic;
        ------------------------------------------------------------------------
        -- Avalon Streaming Sink                                        --------
        -- (FIFO plugs into this)                                       --------
        ----- Interface Prefix: asi                                     --------
		asi_fifoin_ready         : out    std_logic;
		asi_fifoin_data          : in   std_logic_vector(VGA_INPUT_STREAM_WIDTH-1 downto 0);
		asi_fifoin_startofpacket : in   std_logic;
		asi_fifoin_endofpacket   : in   std_logic;
		asi_fifoin_valid         : in   std_logic;        
        --asi_fifoin_reset_n         : in   std_logic;
        ------------------------------------------------------------------------
        -- Avalon Streaming Source                                      --------
        -- (Outputs into VGA signal generator)                          --------
        ----- Interface Prefix: aso                                     --------
		aso_vgaout_ready         : in    std_logic;
		aso_vgaout_data          : out   std_logic_vector(VGA_OUTPUT_STREAM_WIDTH-1 downto 0);
		aso_vgaout_startofpacket : out   std_logic;
		aso_vgaout_endofpacket   : out   std_logic;
		aso_vgaout_valid         : out   std_logic;
        --aso_vgaout_reset_n         : in    std_logic;
        ------------------------------------------------------------------------
        -- Avalon Memory-mapped slave                                   --------
        -- (For accessing read/write registers)                         --------
        ----- Interface Prefix: avs                                     --------
        avs_registers_read_n	        : in    std_logic;
        avs_registers_readdata          : out    std_logic_vector (31 downto 0);
        avs_registers_address           : in    std_logic_vector (3 downto 0);
        --byteenable?
        avs_registers_write_n           : in    std_logic;
        avs_registers_writedata         : in   std_logic_vector (31 downto 0)
        --avs_registers_reset_n             : in    std_logic
        --Response Code
        -- Described on page 18: 
        --      https://www.altera.com/content/dam/altera-www/global/en_US/pdfs/
        --      literature/manual/mnl_avalon_spec.pdf
        -- We will use this to respond a 00:Okay if the palette switch was
        -- Successful.
        -- If the palette switch (involving reading a palette from SRAM) is
        -- unsucessful, then return a 10:SlaveError.
        --avs_registers_response          : out   std_logic_vector (1 downto 0)
        ------------------------------------------------------------------------    
    );
end colour_space_converter;


architecture avalon of colour_space_converter is 
    -- Store the integer locally
    signal pre_count_val    :   integer;
begin

    --Actually need something in here, can't just have a blank section.
    

    avs_registers_readdata <= (others => '0');

end avalon;

