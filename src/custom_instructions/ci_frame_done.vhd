-- ---------------------------------------------------------------------------
-- Custom Instruction: Mark frame complete
--
-- This custom instruction will be executed by the CPU when the application
-- is finished generating frame data. The instruction will return once the
-- video pipeline has completed copying frame data from SDRAM.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY ci_frame_done IS
	PORT (
		-- Custom instruction ports
		ncs_ci_clk       : in     std_logic;
		ncs_ci_clk_en    : in     std_logic;
		ncs_ci_reset     : in     std_logic;
		ncs_ci_start     : in     std_logic;
		ncs_ci_done      : buffer std_logic;

		-- Conduit to talk to video pipeline
		coe_ext_trigger  : buffer std_logic; -- Assert to notify the video pipeline
		coe_ext_done     : in     std_logic  -- Response from the video pipeline
	);
END ENTITY;

ARCHITECTURE Behavioral OF ci_frame_done IS
BEGIN

	PROCESS (ncs_ci_clk)
	BEGIN
		IF rising_edge(ncs_ci_clk) AND ncs_ci_clk_en = '1' THEN
			-- Do instruction stuff
		END IF;
	END PROCESS;

END ARCHITECTURE;