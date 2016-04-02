--
-- Mason Strong
-- January 30, 2016
--

-- Reads data from 2 genesis controllers when trigger pulse goes high

-- ftp://ftp.altera.com/up/pub/Altera_Material/12.0/Tutorials/making_qsys_components.pdf 

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

entity genesis is
	port ( 
		-- Clock interface (to CPU) 
		clk              :    in std_logic;
		reset_n          :    in std_logic;

		-- Trigger for input polling
		trigger          :    in std_logic;
	
		--Controller 1
		dpad_up_input1   :    in std_logic;
		dpad_down_input1 :    in std_logic;
		dpad_left_input1 :    in std_logic;
		dpad_right_input1:    in std_logic;
		select_input1    :   out std_logic;
		start_c_input1   :    in std_logic;
		ab_input1        :    in std_logic;

		--Controller 2
		dpad_up_input2   :    in std_logic;
		dpad_down_input2 :    in std_logic;
		dpad_left_input2 :    in std_logic;
		dpad_right_input2:    in std_logic;
		select_input2    :   out std_logic;
		start_c_input2   :    in std_logic;
		ab_input2        :    in std_logic;

		--Reading portion, needs to be 
		avs_s0_read_n    :    in std_logic ;
		avs_s0_readdata  :   out std_logic_vector (31 downto 0)     
	);
end genesis;

architecture avalon of genesis is 

	type state_type is (s0,s0a,s0b,s0c,s0d,s1,s2,s2a,s2b,s2c,s2d,s3,s4,sIDLE); --for reading/writing appropriately
	signal current_state: state_type;

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
	signal counter_delay : std_logic_vector (7 downto 0) := (others => '0');

begin

	state_machine : process(clk)
	begin
		if rising_edge(clk) then
			case current_state is
				when s0 =>
					---Drive Select Line low
					select_input1 <= '0';
					select_input2 <= '0';
					
					if (trigger = '1') then
						current_state <= s0a;
						counter_delay <= (others => '0');
					else
						current_state <= s0;
					end if;
					
					
				when s0a =>
					---Drive Select Line low
					select_input1 <= '0';
					select_input2 <= '0';
					---Kill time for 4 cycles
					counter_delay <= counter_delay + '1';
					if (counter_delay < x"0A") then
						current_state <= s0a;
					else
						counter_delay <= (others => '0');
						current_state <= s1;
					end if;

				when s1 =>
					--Poll for low select values (Up, Down, A, Start)
					------------------------------------------------------------
					up_1    <= not dpad_up_input1;
					down_1  <= not dpad_down_input1;
					a_1     <= not ab_input1;
					start_1 <= not start_c_input1;

					up_2    <= not dpad_up_input2;
					down_2  <= not dpad_down_input2;
					a_2     <= not ab_input2;
					start_2 <= not start_c_input2;
					----------------------------------------------------------
					counter_delay <= (others => '0');
					current_state <= s2;
				
				when s2 =>
					---Drive Select Line High
					select_input1 <= '1';
					select_input2 <= '1';
					counter_delay <= counter_delay + '1';
					if (counter_delay < x"0A") then
						current_state <= s2;
					else
						counter_delay <= (others => '0');
						current_state <= s3;
					end if;

				when s3 =>
					--Poll for high select values (Left, Right, B, C)
					--Could potentially remove up/down polling as its redundant
					------------------------------------------------------------
					left_1  <= not dpad_left_input1;
					right_1 <= not dpad_right_input1;
					b_1     <= not ab_input1;
					c_1     <= not start_c_input1;

					left_2  <= not dpad_left_input2;
					right_2 <= not dpad_right_input2;
					b_2     <= not ab_input2;
					c_2     <= not start_c_input2;
					----------------------------------------------------------
					current_state <= s4;
				
				when s4 =>
					--Read
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
					current_state <= sIDLE;

				when others =>
					select_input1 <= '0';
					select_input2 <= '0';
					if (trigger = '0') then
						current_state <= s0;
					else
						current_state <= sIDLE;
					end if;
			end case;
		end if;
	end process;
end avalon;
