--
-- Mason Strong
-- January 30, 2016
--

-- ECE 492 Lab 2, Custom QSYS Components

-- This thing has a counter in it that increments a value in a register.
-- The counter counts on a conduit signal. When there's an edge change on
-- that signal, we increment a register.

-- The register is interfaced as an Avalon Memory-Mapped Slave.

-- ftp://ftp.altera.com/up/pub/Altera_Material/12.0/Tutorials/making_qsys_components.pdf 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.VITAL_Primitives.all;
--use work.DE2_CONSTANTS.all;

-- Code for Register

entity genesis_controller_interface is
    port ( 
        -- Clock interface (to CPU) 
        clk             :   in std_logic;

		-- reset interface (Magic avalon reset) WTF
		reset_n		:	in std_logic;

        -- Input signal mapped to the conduit (frequency generator)
		--Controller 1
		dpad_up_input1	:	in std_logic;
		dpad_down_input1	:	in std_logic;
		dpad_left_input1	:	in std_logic;
		dpad_right_input1	: in std_logic;
		select_input1	:	in std_logic;
		start_c_input1	:	in std_logic;
		ab_input1	:	in std_logic;
		
		--Controller 2
		dpad_up_input2	:	in std_logic;
		dpad_down_input2	:	in std_logic;
		dpad_left_input2	:	in std_logic;
		dpad_right_input2	: in std_logic;
		select_input2	:	in std_logic;
		start_c_input2	:	in std_logic;
		ab_input2		:	in std_logic;
		
        --Reading portion, needs to be 
		avs_s0_read_n	: in std_logic ;
		avs_s0_readdata : out std_logic_vector (31 downto 0)     
    );
end genesis_controller_interface;

architecture avalon of genesis_controller_interface is 
    -- Store the integer locally
	signal up_1	:	std_logic;
	signal up_2	: 	std_logic;
	signal down_1	:	std_logic;
	signal down_2	:	std_logic;
	signal left_1	:	std_logic;
	signal left_2	:	std_logic;
	signal right_1	:	std_logic;
	signal right_2	:	std_logic;
	signal start_1	:	std_logic;
	signal start_2	:	std_logic;
	signal a_1		:	std_logic;
	signal a_2		:	std_logic;
	signal b_1		:	std_logic;
	signal b_2		:	std_logic;
	signal c_1		:	std_logic;
	signal c_2		:	std_logic;

begin
    assign_signals_1:
		--controller 1 dpad
        process(dpad_up_input1, dpad_down_input1, dpad_left_input1, dpad_right_input1, select_input1, start_c_input1, ab_input1) is
            -- Declarations
        begin
			if ( dpad_up_input1 = '1' and dpad_up_input1'event) then
				up_1 <= '1';
			else
				up_1 <= '0';
			end if;
			
			if ( dpad_down_input1 = '1' and dpad_down_input1'event) then
				down_1 <= '1';
			else
				down_1 <= '0';
			end if;
			
			--Technically, select_input1 should be high for left/right, 0 o.w.
			if ( dpad_left_input1 = '1' and dpad_left_input1'event) then
				left_1 <= '1';
			else
				left_1 <= '0';
			end if;
			
			if ( dpad_right_input1 = '1' and dpad_right_input1'event) then
				right_1 <= '1';
			else
				right_1 <= '0';
			end if;
			
			--A and B can't b pressed at the same time in this implementation
			if ( select_input1 = '1' and ab_input1 = '1' and ab_input1'event) then
				a_1 <= '1';
			else
				a_1 <= '0';
			end if;
			
			if ( select_input1 = '1' and ab_input1 = '1' and ab_input1'event) then
				a_1 <= '1';
			else
				a_1 <= '0';
			end if;
			
			if ( select_input1 = '0' and ab_input1 = '1' and ab_input1'event) then
				b_1 <= '1';
			else
				b_1 <= '0';
			end if;
			
			if ( select_input1 = '1' and start_c_input1 = '1' and start_c_input1'event) then
				c_1 <= '1';
			else
				c_1 <= '0';
            end if;
			
			if ( select_input1 = '0' and start_c_input1 = '1' and start_c_input1'event) then
				start_1 <= '1';
			else
				start_1 <= '0';
            end if;
        end process;
    assign_signals_2:
		--controller 2 dpad
        process(dpad_up_input2, dpad_down_input2, dpad_left_input2, dpad_right_input2, select_input2, start_c_input2, ab_input2) is
            -- Declarations
        begin
			if ( dpad_up_input2 = '1' and dpad_up_input2'event) then
				up_2 <= '1';
			else
				up_2 <= '0';
			end if;
			
			if ( dpad_down_input2 = '1' and dpad_down_input2'event) then
				down_2 <= '1';
			else
				down_2 <= '0';
			end if;
			
			--Technically, select_input2 should be high for left/right, 0 o.w.
			if ( dpad_left_input2 = '1' and dpad_left_input2'event) then
				left_2 <= '1';
			else
				left_2 <= '0';
			end if;
			
			if ( dpad_right_input2 = '1' and dpad_right_input2'event) then
				right_2 <= '1';
			else
				right_2 <= '0';
			end if;
			
			--A and B can't b pressed at the same time in this implementation
			if ( select_input2 = '1' and ab_input2 = '1' and ab_input2'event) then
				a_2 <= '1';
			else
				a_2 <= '0';
			end if;
			
			if ( select_input2 = '1' and ab_input2 = '1' and ab_input2'event) then
				a_2 <= '1';
			else
				a_2 <= '0';
			end if;
			
			if ( select_input2 = '0' and ab_input2 = '1' and ab_input2'event) then
				b_2 <= '1';
			else
				b_2 <= '0';
			end if;
			
			if ( select_input2 = '1' and start_c_input2 = '1' and start_c_input2'event) then
				c_2 <= '1';
			else
				c_2 <= '0';
            end if;
			
			if ( select_input2 = '0' and start_c_input2 = '1' and start_c_input2'event) then
				start_2 <= '1';
			else
				start_2 <= '0';
            end if;
        end process;
		
	  copy_data_out:
			process(clk, avs_s0_read_n) is
			begin
				if (avs_s0_read_n = '0' ) then 
					avs_s0_readdata(0) <= up_1;
					avs_s0_readdata(1) <= down_1;
					avs_s0_readdata(2) <= left_1;
					avs_s0_readdata(3) <= right_1;
					avs_s0_readdata(4) <= a_1;
					avs_s0_readdata(5) <= b_1;
					avs_s0_readdata(6) <= c_1;
					avs_s0_readdata(7) <= start_1;
					
					avs_s0_readdata(10) <= up_2;
					avs_s0_readdata(11) <= down_2;
					avs_s0_readdata(12) <= left_2;
					avs_s0_readdata(13) <= right_2;
					avs_s0_readdata(14) <= a_2;
					avs_s0_readdata(15) <= b_2;
					avs_s0_readdata(16) <= c_2;
					avs_s0_readdata(17) <= start_2;
				else 
					-- Others is magic. Any part of the array not referenced (none), we use to get all
					-- and set to Z. Could use (31 downto 7) = Z, others = whatever... check syntax.
					avs_s0_readdata <= (others => 'Z');
				end if;
			end process;

end avalon;

