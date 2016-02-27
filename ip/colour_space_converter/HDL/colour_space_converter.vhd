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
        -- Width of Avalon Streaming Source to VGA Output.
        VGA_INPUT_STREAM_WIDTH       : integer               := 8;
        VGA_OUTPUT_STREAM_WIDTH       : integer              := 16
	);
	port (
		------------------------------------------------------------------------
		-- Clock interface (to CPU)
		clk_cpu                         : in std_logic;
		-- Clock interface (to video clock)
		clk_video                       : in std_logic;
		-- reset interface (Magic avalon reset)
		reset_n		                    : in std_logic;
		------------------------------------------------------------------------
		------------------------------------------------------------------------
		-- Avalon Streaming Sink                                        --------
		-- (FIFO plugs into this)                                       --------
		----- Interface Prefix: asi                                     --------
		asi_fifoin_ready         : out    std_logic;
		asi_fifoin_data          : in     std_logic_vector(VGA_INPUT_STREAM_WIDTH-1 downto 0);
		asi_fifoin_startofpacket : in     std_logic;
		asi_fifoin_endofpacket   : in     std_logic;
		asi_fifoin_valid         : in     std_logic;       
		------------------------------------------------------------------------
		-- Avalon Streaming Source                                      --------
		-- (Outputs into VGA signal generator)                          --------
		----- Interface Prefix: aso                                     --------
		aso_vgaout_ready         : in     std_logic;
		aso_vgaout_data          : out    std_logic_vector(VGA_OUTPUT_STREAM_WIDTH-1 downto 0);
		aso_vgaout_startofpacket : out    std_logic;
		aso_vgaout_endofpacket   : out    std_logic;
		aso_vgaout_valid         : buffer std_logic;
		------------------------------------------------------------------------
		-- Avalon Memory-mapped slave                                   --------
		-- (For accessing palette ram)                                  --------
		----- Interface Prefix: avs                                     --------
		avs_paletteram_read_n	     : in     std_logic;
		avs_paletteram_readdata      : out    std_logic_vector (15 downto 0);
		avs_paletteram_address       : in     std_logic_vector (7 downto 0);
		avs_paletteram_write_n       : in     std_logic;
		avs_paletteram_writedata     : in     std_logic_vector (15 downto 0)
		------------------------------------------------------------------------
	);
end colour_space_converter;


architecture avalon of colour_space_converter is 
	-- Store the integer locally
	signal pre_count_val    :   integer;

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

	-- Expose signals from palette ram for controls.

	-- Write enable on A (avalon side) required. Read optional.
	signal sram_palette_store_portA_rden_a  : std_logic;
	signal sram_palette_store_portA_wren_a  : std_logic;
	-- For port B, only a read enable is required. Write is optional, and
	-- typically unused.
	signal sram_palette_store_portB_rden_b  : std_logic;
	signal sram_palette_store_portB_wren_b  : std_logic;


	-- Signal to store the pixel clogged in the pixel pipeline while ready = '0'
	signal buffer_aso_vgaout_startofpacket : std_logic;
	signal buffer_aso_vgaout_endofpacket   : std_logic;
	signal buffer_aso_vgaout_data : std_logic_vector(VGA_OUTPUT_STREAM_WIDTH-1 downto 0);

	signal clk_video_mem : std_logic;

begin

	-- Create a clock that is offset from the video clock by half a cycle.
	-- This will make sure the altsyncram is read half a clock cycle before
	-- it is needed by the output interface.
	-- This only works because we know the input stage will be updated very
	-- soon after the rising edge of the video clock, so when this memory clock
	-- hits its rising edge, it can immediately load the pixel data into its
	-- unregistered output, in time for it to be read by the next stages in the
	-- video pipeline.
	clk_video_mem <= not clk_video;

	-- This ram stores the palette data needed for the conversion.
	-- This ram is set up as a dual port ram so that it can be written
	-- to and read from at the same time. (This can cause some race conditions
	-- and invalid colour palette data during a frame draw, if current palette
	-- is being updated the same time it's being used to draw, but it should
	-- be fixed by the time the next frame is drawn. The programmer can work
	-- around this.)

	-- Port A: To signals that hold lines for Avalon bus.
	-- Port B: To video output / colour conversion logic. 

	-- Combinational logic.
	sram_palette_store_portA_rden_a <= not avs_paletteram_read_n;
	sram_palette_store_portA_wren_a  <= not avs_paletteram_write_n; --turns out wren_a is active low as well. ??
	sram_palette_store_portA_address <= avs_paletteram_address;
	sram_palette_store_portA_datain  <= avs_paletteram_writedata;
	avs_paletteram_readdata <=  sram_palette_store_portA_dataout;


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
                --                  Port settings                             --
                --                                                            --
                -- Clocks to use for async clear                              --
                outdata_aclr_a          => "NONE",                            --
                outdata_aclr_b          => "NONE",                            --
                -- Clock to update output registers                           --
                outdata_reg_a           => "CLOCK0",                          --
                outdata_reg_b           => "UNREGISTERED",                    --
                -- Note: If output registers are enabled, this will add an    --
                --       additional clock cycle of read latency.              --
                ----------------------------------------------------------------

                ----------------------------------------------------------------
                --                      Ram Block Type                        --
                --                                                            --
                ram_block_type          => "AUTO",
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
                clock1          => clk_video_mem, -- Port B, timed to video.

		        address_a		=> sram_palette_store_portA_address,     -- Address bus for port A.
	            address_b		=> sram_palette_store_portB_address,     -- Address bus for port B.

                data_a          => sram_palette_store_portA_datain,     --Data input for port A.
                data_b          => sram_palette_store_portB_datain,     --Data input for port B.
        		-- Outputs
                q_a             => sram_palette_store_portA_dataout,     --Data output port from memory
                q_b             => sram_palette_store_portB_dataout,     --Data output port from memory

                -- Read/Write enable control signals
                rden_a => sram_palette_store_portA_rden_a,
                rden_b => sram_palette_store_portB_rden_b,
                wren_a => sram_palette_store_portA_wren_a,
                wren_b => sram_palette_store_portB_wren_b


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

-- 8 bit pixel data is the address into the ram.
sram_palette_store_portB_address <= asi_fifoin_data;

-- Assert chip ram read before the pixel clock. (Stephen has explaination).

-- A is CPU
-- B is video.


--Sink valid signal. We have a valid address. So read from it.
sram_palette_store_portB_rden_b <= '1' when asi_fifoin_valid = '1' and aso_vgaout_ready = '1' else '0';

--Set to zero for now. Typically tie it to write signal on avalon bus.
-- Since using write_n, need to negate. 
--sram_palette_store_portA_wren_a <= not avs_paletteram_write_n;



    -- Output Registers
    process (clk_video)
        begin
            if Rising_edge(clk_video) then
                if (reset_n = '0') then
                    buffer_aso_vgaout_data             <= (others => '0');
                    buffer_aso_vgaout_startofpacket    <= '0';
                    buffer_aso_vgaout_endofpacket      <= '0';
                    --aso_vgaout_empty            <= (others => '0');
                    aso_vgaout_valid            <= '0';
                elsif ((aso_vgaout_ready = '1') and (asi_fifoin_valid = '1')) then
                    --aso_vgaout_data             <= converted_data;
                    buffer_aso_vgaout_data             <= sram_palette_store_portB_dataout;
                    buffer2_aso_vgaout_data             <= buffer_aso_vgaout_data;

                    buffer_aso_vgaout_startofpacket    <= asi_fifoin_startofpacket;
                    buffer2_aso_vgaout_startofpacket    <= buffer_aso_vgaout_startofpacket;

                    buffer_aso_vgaout_endofpacket      <= asi_fifoin_endofpacket;
                    buffer2_aso_vgaout_endofpacket      <= buffer_aso_vgaout_endofpacket;
                    --aso_vgaout_empty				<= asi_fifoin_empty;
                    --aso_vgaout_empty            <= (others => '0');
                    aso_vgaout_valid            <= asi_fifoin_valid;
                end if;
        end if;
    end process;
-- Stephen comments for the delays needed to make this work:
-- I thought about it for a while, and I think all you need to do is keep 
-- delaying aso_start, end and data by one pixel at a time until there's no 
-- longer any duplicate pixels.
-- Once you verify that this works, you can swap the mess with a fifo
-- No idea why this works. Stephen knows.
-- Combinational logic.
aso_vgaout_data <= buffer_aso_vgaout_data when aso_vgaout_ready = '1' else (others => '0');
aso_vgaout_startofpacket <= buffer_aso_vgaout_startofpacket when aso_vgaout_ready = '1' else '0';
aso_vgaout_endofpacket <= buffer_aso_vgaout_endofpacket when aso_vgaout_ready = '1' else '0';
   





end avalon;

