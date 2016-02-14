--
-- Mason Strong
-- January 30, 2016
--

-- Reads data from 2 genesis controllers when vsync goes high

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
    clk	:	in std_logic;
    --Vsync for timing
    vsync		: in std_logic;

		-- reset interface (Magic avalon reset) WTF
		reset_n		:	in std_logic;
	
		--Controller 1
		dpad_up_input1	:	in std_logic;
		dpad_down_input1	:	in std_logic;
		dpad_left_input1	:	in std_logic;
		dpad_right_input1	:	in std_logic;
		select_input1	:	out std_logic;
		start_c_input1	:	in std_logic;
		ab_input1	:	in std_logic;
		
		--Controller 2
		dpad_up_input2	:	in std_logic;
		dpad_down_input2	:	in std_logic;
		dpad_left_input2	:	in std_logic;
		dpad_right_input2	:	in std_logic;
		select_input2	:	out std_logic;
		start_c_input2	:	in std_logic;
		ab_input2		:	in std_logic;
		
        --Reading portion, needs to be 
		avs_s0_read_n	: in std_logic ;
		avs_s0_readdata : out std_logic_vector (31 downto 0)     
    );
end genesis_controller_interface;

architecture avalon of genesis_controller_interface is 

	type state_type is (s0,s1,s2,s3,s4); --for reading/writing appropriately
	signal current_state, next_state: state_type;
	signal vsync_triggered: std_logic := '0';

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
		process(vsync, clk, reset_n)
			--This may need to be set up 
			begin
				if(reset_n = '0') then
					current_state <= s0;
				elsif(rising_edge(clk)) then
					current_state <= next_state;
				end if;
		
				if (rising_edge(vsync)) then
					vsync_triggered <= '1';
				end if;
		end process;
			
		process(current_state, vsync_triggered, dpad_up_input1, dpad_down_input1, dpad_up_input2, dpad_down_input2, 
						dpad_left_input1, dpad_right_input1, start_c_input1, ab_input1, 
						dpad_left_input2, dpad_right_input2, start_c_input2, ab_input2)
				begin
					case current_state is
						when s0 =>
							---Drive Select Line low
							select_input1 <= '0';
							select_input2 <= '0';
							next_state <= s1;
					
						when s1 =>
							--Poll for low select values (Up, Down, A, Start)
							------------------------------------------------------------
							if (dpad_up_input1 = '0') then
								up_1 <= '1';
							else
								up_1 <= '0';
							end if;
							if (dpad_down_input1 = '0') then
								down_1 <= '1';
							else
								down_1 <= '0';
							end if;
							if (ab_input1 = '0') then
								a_1 <= '1';
							else
								a_1 <= '0';
							end if;
							if (start_c_input1 = '0') then
								start_1 <= '1';
							else
								start_1 <= '0';
							end if;
							if (dpad_up_input2 = '0') then
								up_2 <= '1';
							else
								up_2 <= '0';
							end if;
							if (dpad_down_input2 = '0') then
								down_2 <= '1';
							else
								down_2 <= '0';
							end if;
							if (ab_input2 = '0') then
								a_2 <= '1';
							else
								a_2 <= '0';
							end if;
							if (start_c_input2 = '0') then
								start_2 <= '1';
							else
								start_2 <= '0';
							end if;
							----------------------------------------------------------
							next_state <= s2;
						
						when s2 =>
							---Drive Select Line High
							select_input1 <= '1';
							select_input2 <= '1';
							next_state <= s3;
						
						when s3 =>
							--Poll for high select values (Up, Down, Left, Right, B, C)
							--Could potentially remove up/down polling as its redundant
							------------------------------------------------------------
							if (dpad_up_input1 = '0') then
								up_1 <= '1';
							else
								up_1 <= '0';
							end if;
							if (dpad_down_input1 = '0') then
								down_1 <= '1';
							else
								down_1 <= '0';
							end if;
							if (dpad_left_input1 = '0') then
								left_1 <= '1';
							else
								left_1 <= '0';
							end if;
							if (dpad_right_input1 = '0') then
								right_1 <= '1';
							else
								right_1 <= '0';
							end if;
							if (ab_input1 = '0') then
								b_1 <= '1';
							else
								b_1 <= '0';
							end if;
							if (start_c_input1 = '0') then
								c_1 <= '1';
							else
								c_1 <= '0';
							end if;
							if (dpad_up_input2 = '0') then
								up_2 <= '1';
							else
								up_2 <= '0';
							end if;
							if (dpad_down_input2 = '0') then
								down_2 <= '1';
							else
								down_2 <= '0';
							end if;
							if (dpad_left_input2 = '0') then
								left_2 <= '1';
							else
								left_2 <= '0';
							end if;
							if (dpad_right_input2 = '0') then
								right_2 <= '1';
							else
								right_2 <= '0';
							end if;
							if (ab_input2 = '0') then
								b_2 <= '1';
							else
								b_2 <= '0';
							end if;
							if (start_c_input2 = '0') then
								c_2 <= '1';
							else
								c_2 <= '0';
							end if;
							----------------------------------------------------------
							next_state <= s4;
						
						when s4 =>
							--IDLE
							--Read on Vsync
							if (vsync_triggered='1') then
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
						
								next_state <= s0;
							end if;							
					end case;
		end process;
end avalon;
