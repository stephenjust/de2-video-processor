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

-- Library containing altera components
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
        VGA_INPUT_STREAM_WIDTH       : integer               := 8;
        VGA_OUTPUT_STREAM_WIDTH       : integer              := 16


	);
    port (
        ------------------------------------------------------------------------
        -- Clock interface (to CPU) 
        clk_cpu                             : in std_logic;
        -- Clock interface (to video clock)
        clk_video                           : in std_logic;
		-- reset interface (Magic avalon reset)
		reset_n		                    : in std_logic;
        ------------------------------------------------------------------------
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
		aso_vgaout_valid         : buffer   std_logic;
        --aso_vgaout_reset_n         : in    std_logic;
        ------------------------------------------------------------------------
        -- Avalon Memory-mapped slave                                   --------
        -- (For accessing palette ram)                                  --------
        ----- Interface Prefix: avs                                     --------
        avs_paletteram_read_n	         : in    std_logic;
        avs_paletteram_readdata          : out    std_logic_vector (31 downto 0);
        avs_paletteram_address           : in    std_logic_vector (3 downto 0);
        --byteenable?
        avs_paletteram_write_n           : in    std_logic;
        avs_paletteram_writedata         : in   std_logic_vector (31 downto 0)
        --avs_registers_reset_n             : in    std_logic
        --Response Code
        -- Described on page 18: 
        --      https://www.altera.com/content/dam/altera-www/global/en_US/pdfs/literature/manual/mnl_avalon_spec.pdf
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
-- http://quartushelp.altera.com/14.1/mergedProjects/hdl/mega/mega_file_altsynch_ram.htm
component altsyncram
        generic (
                address_aclr_a          :       string := "UNUSED";
                address_aclr_b          :       string := "NONE";
                address_reg_b           :       string := "CLOCK1";
                byte_size               :       natural := 8;
                byteena_aclr_a          :       string := "UNUSED";
                byteena_aclr_b          :       string := "NONE";
                byteena_reg_b           :       string := "CLOCK1";
                clock_enable_core_a     :       string := "USE_INPUT_CLKEN";
                clock_enable_core_b     :       string := "USE_INPUT_CLKEN";
                clock_enable_input_a    :       string := "NORMAL";
                clock_enable_input_b    :       string := "NORMAL";
                clock_enable_output_a   :       string := "NORMAL";
                clock_enable_output_b   :       string := "NORMAL";
                intended_device_family  :       string := "unused";
                enable_ecc              :       string := "FALSE";
                implement_in_les        :       string := "OFF";
                indata_aclr_a           :       string := "UNUSED";
                indata_aclr_b           :       string := "NONE";
                indata_reg_b            :       string := "CLOCK1";
                init_file               :       string := "UNUSED";
                init_file_layout        :       string := "PORT_A";
                maximum_depth           :       natural := 0;
                numwords_a              :       natural := 0;
                numwords_b              :       natural := 0;
                operation_mode          :       string := "BIDIR_DUAL_PORT";
                outdata_aclr_a          :       string := "NONE";
                outdata_aclr_b          :       string := "NONE";
                outdata_reg_a           :       string := "UNREGISTERED";
                outdata_reg_b           :       string := "UNREGISTERED";
                power_up_uninitialized  :       string := "FALSE";
                ram_block_type          :       string := "AUTO";
                rdcontrol_aclr_b        :       string := "NONE";
                rdcontrol_reg_b         :       string := "CLOCK1";
                read_during_write_mode_mixed_ports      :       string := "DONT_CARE";
                read_during_write_mode_port_a           :       string := "NEW_DATA_NO_NBE_READ";
                read_during_write_mode_port_b           :       string := "NEW_DATA_NO_NBE_READ";
                width_a                 :       natural;
                width_b                 :       natural := 1;
                width_byteena_a         :       natural := 1;
                width_byteena_b         :       natural := 1;
                widthad_a               :       natural;
                widthad_b               :       natural := 1;
                wrcontrol_aclr_a        :       string := "UNUSED";
                wrcontrol_aclr_b        :       string := "NONE";
                wrcontrol_wraddress_reg_b    :       string := "CLOCK1";
                lpm_hint                :       string := "UNUSED";
                lpm_type                :       string := "altsyncram"
        );

        port(
                aclr0                   :       in std_logic := '0';
                aclr1                   :       in std_logic := '0';
                address_a               :       in std_logic_vector(widthad_a-1 downto 0);
                address_b               :       in std_logic_vector(widthad_b-1 downto 0) := (others => '1');
                addressstall_a          :       in std_logic := '0';
                addressstall_b          :       in std_logic := '0';
                byteena_a               :       in std_logic_vector(width_byteena_a-1 downto 0) := (others => '1');
                byteena_b               :       in std_logic_vector(width_byteena_b-1 downto 0) := (others => '1');
                clock0                  :       in std_logic := '1';
                clock1                  :       in std_logic := '1';
                clocken0                :       in std_logic := '1';
                clocken1                :       in std_logic := '1';
                clocken2                :       in std_logic := '1';
                clocken3                :       in std_logic := '1';
                data_a                  :       in std_logic_vector(width_a-1 downto 0) := (others => '1');
                data_b                  :       in std_logic_vector(width_b-1 downto 0) := (others => '1');
                eccstatus               :       out std_logic_vector(2 downto 0);
                q_a                     :       out std_logic_vector(width_a-1 downto 0);
                q_b                     :       out std_logic_vector(width_b-1 downto 0);
                rden_a                  :       in std_logic := '1';
                rden_b                  :       in std_logic := '1';
                wren_a                  :       in std_logic := '0';
                wren_b                  :       in std_logic := '0'
        );
end component;

    -- Signals are needed here to wire everything together. 

    signal sram_palette_store_portA_address :   std_logic_vector(8-1 downto 0) := (others => 'Z');
    signal sram_palette_store_portA_datain  :   std_logic_vector(16-1 downto 0) := (others => 'Z');
    signal sram_palette_store_portA_dataout  :   std_logic_vector(16-1 downto 0);

    signal sram_palette_store_portB_address :   std_logic_vector(8-1 downto 0) := (others => 'Z');
    signal sram_palette_store_portB_datain  :   std_logic_vector(16-1 downto 0) := (others => 'Z');
    signal sram_palette_store_portB_dataout  :   std_logic_vector(16-1 downto 0);

    -- Colour Conversion Signals
    signal colour_index : std_logic_vector(8 downto 0); --Number from sram
    -- Converted colour from the SRAM. 
    signal colour_converted : std_logic_vector(16 downto 0); 

begin
    -- This ram stores the palette data needed for the conversion.
    -- This ram is set up as a dual port ram so that it can be written
    -- to and read from at the same time. (This can cause some race conditions
    -- and invalid colour palette data during a frame draw, if current palette
    -- is being updated the same time it's being used to draw, but it should
    -- be fixed by the time the next frame is drawn. The programmer can work
    -- around this.)

    -- Port A: To signals that hold lines for Avalon bus.
    -- Port B: To video output / colour conversion logic. 


	process(clk_cpu, 
            avs_paletteram_read_n, 
            avs_paletteram_address, 
            avs_paletteram_write_n) is
	begin
		if (avs_paletteram_read_n = '0' ) then 
            --Reading all the things
            sram_palette_store_portA_address <= "00000000" or avs_paletteram_address;
            avs_paletteram_readdata <= x"00000000" or sram_palette_store_portA_datain;
        elsif (avs_paletteram_write_n = '0') then
            --Writing all the things
            sram_palette_store_portA_address <= "00000000" or avs_paletteram_address;
            sram_palette_store_portA_datain  <= sram_palette_store_portA_datain;
		else
            -- Put everything to high impedance.
            sram_palette_store_portA_address <= (others => 'Z');
            sram_palette_store_portA_datain <= (others => 'Z');
            sram_palette_store_portA_dataout <= (others => 'Z');
		end if;
	end process;


    -- Syntax for generic maps on instantiating a component.
    -- http://www.ics.uci.edu/~jmoorkan/vhdlref/compinst.html

    palette_ram: altsyncram 
            generic map (

 		        --address_aclr_a          =>,
                --address_aclr_b          =>,
                --address_reg_b           =>,

                --byte_size               => 8, -- Sane Default.
                --byteena_aclr_a          =>,
                --byteena_aclr_b          =>,
                --byteena_reg_b           =>,

                --
                -- Docs say these are typically available for Stratix Devices
                -- only.
                --
                --clock_enable_core_a     =>,
                --clock_enable_core_b     =>,
                --clock_enable_input_a    => "BYPASS",
                --clock_enable_input_b    =>,
                --clock_enable_output_a   => "BYPASS",
                --clock_enable_output_b   =>,


                intended_device_family  => "Cyclone II",  

                ----------------------------------------------------------------
                --              Error Checking and Correcting                 --
                enable_ecc              => "FALSE", -- Don't need ECC.
                --                                                            --
                ----------------------------------------------------------------
                -- Where to build (whether as on-chip ram or logical blocks).
                implement_in_les        => "OFF",
                ----------------------------------------------------------------

                -- If set do default, this is run from clock 1.
                --indata_aclr_a           =>,
                --indata_aclr_b           =>,
                --indata_reg_b            =>,

                ----------------------------------------------------------------
                -- Maximum Depth of the ram. Used for segmentation.
                -- The deeper the ram, the more dynamic power used.
                -- It does relate to Logical elements, which is not what 
                -- we're using with the M4K ram. Thus, we should omit this
                -- parameter. If it doesn't need to be omitted, it should be
                -- set to 256, which is the ideal segment size.
                --
                -- maximum_depth           => 256,
                ----------------------------------------------------------------
                -- Unused. Don't need to make the ram smaller than 
                -- it should be. 
                --
                --numwords_a              =>,
                --numwords_b              =>,
                ----------------------------------------------------------------
                --                      Operation Mode                        --
                operation_mode          => "BIDIR_DUAL_PORT",                 --
                ----------------------------------------------------------------

                ----------------------------------------------------------------
                --                  Clocks for output ports                   --
                --                                                            --
                outdata_aclr_a          => "NONE",   --Async clear clock.     --
                outdata_aclr_b          => "NONE",   --Async clear clock.     --
                outdata_reg_a           => "CLOCK0", --Output data clock.     --
                outdata_reg_b           => "CLOCK1", --Output data clock.     --
                ----------------------------------------------------------------

                ----------------------------------------------------------------
                --                      Ram Block Type                        --
                --                                                            --
                ram_block_type          => "M4K", 
                --                       -- Only valid choice on Cyclone II   --
                ----------------------------------------------------------------

                ----------------------------------------------------------------
                --                  Reads During Writes                       --
                --                                                            --
                --                                                            --
                -- https://www.altera.com/content/dam/altera-www/             --
                --             global/en_US/pdfs/literature/ug/ug_ram_rom.pdf --
                -- Page 21.                                                   --
                read_during_write_mode_mixed_ports      => "OLD_DATA",        --
                -- WITH_NBE_READ -> means you get the old data if you're      --
                -- contending. May not be supported on older cyclones.        --
                -- Feb 13: Can confirm WITH_NBE_READ not supported on Cyclone --
                -- II.                                                        --
                -- NO_NBE_READ -> means you get an X if you're contending     --
                read_during_write_mode_port_b       => "NEW_DATA_NO_NBE_READ",
                read_during_write_mode_port_a       => "NEW_DATA_NO_NBE_READ",
                --                                                            --
                ----------------------------------------------------------------

                ----------------------------------------------------------------
                --                         Bus Widths                         --
                --                                                            --
                width_a                 => 16, -- Must match width_b          --
                width_b                 => 16, -- Width of RGB565             --
                --  width_byteena_a       =>,  -- Optional. Sane defaults.    --
                --  width_byteena_b       =>,  -- Optional. Sane defaults.    --
                widthad_a               => 8,  -- Address bus width           --
                widthad_b               => 8,  -- Should be 8, since sram has 8s
                ----------------------------------------------------------------


                ----------------------------------------------------------------
                --                  Asyncronous Write Control                 --
                --                      Leave at defaults.                    --
                --wrcontrol_aclr_a              =>,                           --
                --wrcontrol_aclr_b              =>,                           --
                --wrcontrol_wraddress_reg_b     =>,                           --
                --                                                            --
                ----------------------------------------------------------------

                ----------------------------------------------------------------
                -- No idea what this is, as Altera doesn't document it.
                -- Currently set to default values.
                -- used in video_test_pattern......?
                lpm_hint                => "ENABLE_RUNTIME_MOD=NO",     
                lpm_type                => "altsyncram", 
                ----------------------------------------------------------------


                -- Ram initialization stuff. 
                init_file               => "default_palette.mif", 
                init_file_layout        => "PORT_B",
                power_up_uninitialized  => "FALSE" -- According to altera docs, can 
                                            -- power up to constant. Put in an 
                                            -- EGA palette in here. 
                ----------------------------------------------------------------
            )
            port map (
                -- Inputs
                clock0			=> clk_cpu, -- PortA, talks to CPU.
                clock1          => clk_video, -- Port B, timed to video.

		        address_a		=> sram_palette_store_portA_address,     -- Address bus for port A.
	            address_b		=> sram_palette_store_portB_address,     -- Address bus for port B.

                data_a          => sram_palette_store_portA_datain,     --Data input for port A.
                data_b          => sram_palette_store_portB_datain,     --Data input for port B.
        		-- Outputs
                q_a				=> sram_palette_store_portA_dataout,     --Data output port from memory
                q_b             => sram_palette_store_portB_dataout     --Data output port from memory

            )
    ;
  
    -- Avalon Streaming Example Code
--https://github.com/jterweeme/mediacenter/blob/master/ip/University_Program/Audio_Video/Video/altera_up_avalon_video_rgb_resampler/hdl/altera_up_avalon_video_rgb_resampler.vhd


-- Clk_video is for pixels. every tick is a VBI.
-- Clk_cpu is for cpu access.

--The ready signal will go low during blanking space so that the 
--pipeline doesn't shove new pixels in, as there's no place to store it.


-- Need to tell the FIFO when we're ready to stream, so this needs to be outside
-- all the processes.
asi_fifoin_ready    <= aso_vgaout_ready;

-- Need to wait a clock for the memory to respond to request
-- Can't assume things are instantaneous.


    -- Output Registers
    process (clk_video)
        begin
            if Rising_edge(clk_video) then
                if (reset_n = '1') then
                    aso_vgaout_data             <= (others => '0');
                    aso_vgaout_startofpacket    <= '0';
                    aso_vgaout_endofpacket      <= '0';
                    --aso_vgaout_empty            <= (others => '0');
                    aso_vgaout_valid            <= '0';
                elsif ((aso_vgaout_ready = '1') or (aso_vgaout_valid = '0')) then
                    --aso_vgaout_data             <= converted_data;
                    aso_vgaout_data             <= X"0000" or asi_fifoin_data;
                    aso_vgaout_startofpacket    <= asi_fifoin_startofpacket;
                    aso_vgaout_endofpacket      <= asi_fifoin_endofpacket;
                    --aso_vgaout_empty				<= asi_fifoin_empty;
                    --aso_vgaout_empty            <= (others => '0');
                    aso_vgaout_valid            <= asi_fifoin_valid;
                end if;
        end if;
    end process;

-- Right now this is just a dumb streaming forwarder.



   

end avalon;

