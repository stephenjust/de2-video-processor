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
		clk              : in     std_logic; -- Only keeping QSYS happy - use ncs_ci_clk
		reset            : in     std_logic; -- Only keeping QSYS happy - use ncs_ci_reset

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

	coe_ext_trigger <= ncs_ci_start WHEN ncs_ci_reset = '0' ELSE '0';
	ncs_ci_done <= coe_ext_done WHEN ncs_ci_reset = '0' ELSE '0';

END ARCHITECTURE;