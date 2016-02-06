-- ---------------------------------------------------------------------------
-- DMA burst reader
--
-- This component handles reading a burst of data over the Avalon-MM i/f.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;
USE ieee.numeric_std.all;

ENTITY video_fb_dma_burst_reader IS 
	PORT (
		clk                 : in     std_logic;
		reset               : in     std_logic;

		ctl_start           : in     std_logic;
		ctl_burstcount      : in     std_logic_vector(7 downto 0);
		ctl_busy            : buffer std_logic := '0';

		dma_read            : buffer std_logic;
		dma_readdatavalid	  : in     std_logic;
		dma_waitrequest     : in     std_logic;
		dma_address         : buffer std_logic_vector(31 downto 0);
		dma_burstcount      : buffer std_logic_vector(7 downto 0)
	);
END video_fb_dma_burst_reader;

ARCHITECTURE Behaviour OF video_fb_dma_burst_reader IS
	TYPE dma_state_t IS (IDLE, RUNNING);

	SIGNAL dma_state : dma_state_t := IDLE;
	SIGNAL next_state : dma_state_t;

	SIGNAL count_remaining : std_logic_vector(7 downto 0);
	
BEGIN

	PROCESS (clk)
	BEGIN
		IF rising_edge(clk) THEN
			CASE dma_state IS
				WHEN IDLE =>
					IF ctl_start = '1' THEN
						ctl_busy <= '1';
						count_remaining <= ctl_burstcount;
						dma_burstcount <= ctl_burstcount;
						dma_read <= '1';
						next_state <= RUNNING;
					ELSE
						next_state <= IDLE;
					END IF;
				WHEN RUNNING =>
					IF OR_REDUCE(count_remaining) = '0' THEN
						ctl_busy <= '0';
						next_state <= IDLE;
					ELSIF dma_waitrequest = '1' THEN
						next_state <= RUNNING;
					ELSE
						dma_read <= '0';
						IF dma_readdatavalid = '1' THEN
							count_remaining <= count_remaining - '1';
						END IF;
						next_state <= RUNNING;
					END IF;
				WHEN OTHERS =>
					next_state <= IDLE;
			END CASE;
			dma_state <= next_state;
		END IF;
	END PROCESS;

END Behaviour;
