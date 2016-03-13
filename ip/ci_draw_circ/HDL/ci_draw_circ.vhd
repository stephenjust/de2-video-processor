-- ---------------------------------------------------------------------------
-- Custom Instruction: Draw Circle
--
-- This custom instruction draws an un-filled circle.
--
-- Author: Stefan Martynkiw
-- ---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.numeric_std.all;
USE ieee.std_logic_1164.all;

ENTITY ci_draw_circ IS
	PORT (
		clk              : in     std_logic; -- Only keeping QSYS happy - use ncs_ci_clk
		reset            : in     std_logic; -- Only keeping QSYS happy - use ncs_ci_reset

		-- Custom instruction ports
		ncs_ci_clk       : in     std_logic;
		ncs_ci_clk_en    : in     std_logic;
		ncs_ci_reset     : in     std_logic;
		ncs_ci_start     : in     std_logic;
		ncs_ci_done      : buffer std_logic;

		-- Memory-mapped slave for config registers
		avs_s0_write     : in     std_logic;
		avs_s0_writedata : in     std_logic_vector(31 downto 0);
		avs_s0_address   : in     std_logic_vector(2 downto 0);

		-- Memory-mapped master for DMA write
		avm_m0_write       : buffer std_logic;
		avm_m0_writedata   : buffer std_logic_vector(7 downto 0); -- Want 1 byte for writing to SRAM
		avm_m0_burstcount  : buffer std_logic_vector(7 downto 0);
		avm_m0_address     : buffer std_logic_vector(31 downto 0);
		avm_m0_waitrequest : in     std_logic
	);
END ENTITY;

ARCHITECTURE Behavioral OF ci_draw_circ IS

	TYPE op_state IS (IDLE, RUNNING);
    type draw_state is (Octant1, Octant2, Octant3, Octant4, Octant5, Octant6, Octant7, Octant8, MakeDecision);

	CONSTANT FRAME_WIDTH : signed(15 downto 0) := to_signed(640, 16);
	CONSTANT FRAME_HEIGHT : signed(15 downto 0) := to_signed(480, 16);

	CONSTANT REG_BUF_ADDR : std_logic_vector(2 downto 0) := B"000";
	CONSTANT REG_CX : std_logic_vector(2 downto 0) := B"001";
	CONSTANT REG_CY : std_logic_vector(2 downto 0) := B"010";
	CONSTANT REG_RAD : std_logic_vector(2 downto 0) := B"011";
	CONSTANT REG_COLOR : std_logic_vector(2 downto 0) := B"100";


	-- Configuration registers
	SIGNAL buf_addr : std_logic_vector(31 downto 0) := (others => '0');
	SIGNAL cx : std_logic_vector(15 downto 0) := (others => '0');
	SIGNAL cy : std_logic_vector(15 downto 0) := (others => '0');
	SIGNAL rad : std_logic_vector(15 downto 0) := (others => '0');
	SIGNAL color : std_logic_vector(7 downto 0) := (others => '0');

	SIGNAL current_state : op_state := IDLE;
    signal drawing_state : draw_state := Octant1;

    --For the line algorithm
    signal decisionOver2 : signed(15 downto 0);

	SIGNAL x0 : std_logic_vector(15 downto 0) := (others => '0');
	SIGNAL y0 : std_logic_vector(15 downto 0) := (others => '0');


BEGIN



    -- next_x and next_y need to be set as variables within the process block
    -- so that they can be updated at not just the end of a clock cycle.

	-- Do operation
	op : PROCESS(ncs_ci_clk)

        VARIABLE next_x0 : std_logic_vector(15 downto 0);
        VARIABLE next_y0 : std_logic_vector(15 downto 0);

        --Bresenham's Circle Algorithm: 
        --https://en.wikipedia.org/wiki/Midpoint_circle_algorithm#Example

	BEGIN



		IF rising_edge(ncs_ci_clk) THEN
            IF reset = '1' OR ncs_ci_reset = '1' THEN
                current_state <= IDLE;
		    ELSIF current_state = IDLE THEN -- Don't do anything. 
				ncs_ci_done <= '0';
				avm_m0_write <= '0';
				IF ncs_ci_start = '1' THEN
					current_state <= RUNNING;

					x0 <= rad;
					y0 <= 0;

                    decisionOver2 <= to_signed(1, 16) - rad;


				ELSE
					current_state <= IDLE;
				END IF;
			ELSIF current_state = RUNNING THEN  -- This is where we are drawing
                -- These signals are only assigned once we've finished the process.
				avm_m0_burstcount <= x"01";
				avm_m0_writedata <= color; 
				avm_m0_write <= '1';
				next_x0 := x0;
				next_y0 := y0;

				-- Wait until write succeeds to advance to next pixel
				IF avm_m0_waitrequest = '0' THEN
					IF current_y0 > current_x0 THEN
						-- Write completed
						current_state <= IDLE;
						avm_m0_write <= '0';
						ncs_ci_done <= '1'; --Assert frame drawing done.
                    else
--Base case: avm_m0_address <= std_logic_vector(unsigned(buf_addr) + unsigned(next_x) + (unsigned(FRAME_WIDTH) * unsigned(next_y)));
                        case drawing_state is 
                            when Octant1 =>
                                --DrawPixel( x + cx,  y + cy);
                            avm_m0_address <= std_logic_vector(unsigned(buf_addr)
                                                               + unsigned(cx) + unsigned(next_x0)
                                                               + (unsigned(FRAME_WIDTH) * (unsigned(next_y0) +  unsigned(cy) )  )
                                                              );
                            x0 <= next_x0;
                            y0 <= next_y0;
                            drawing_state <= Octant2;
                            when Octant2 =>
                                --DrawPixel( y + cx,  x + cy);
                            avm_m0_address <= std_logic_vector(unsigned(buf_addr)
                                                               + unsigned(cx) + unsigned(next_y0)
                                                               + (unsigned(FRAME_WIDTH) * (unsigned(next_x0) +  unsigned(cy) )  )
                                                              );
                            x0 <= next_x0;
                            y0 <= next_y0;
                            drawing_state <= Octant3;
                            when Octant3 =>
                                --DrawPixel(-y + cx,  x + cy);
                            avm_m0_address <= std_logic_vector(unsigned(buf_addr)
                                                               + unsigned(cx) - unsigned(next_y0)
                                                               + (unsigned(FRAME_WIDTH) * (unsigned(next_x0) +  unsigned(cy) )  )
                                                              );
                            x0 <= next_x0;
                            y0 <= next_y0;
                            drawing_state <= Octant4;
                            when Octant4 =>
                                --DrawPixel(-x + cx,  y + cy);
                            avm_m0_address <= std_logic_vector(unsigned(buf_addr)
                                                               + unsigned(cx) - unsigned(next_x0)
                                                               + (unsigned(FRAME_WIDTH) * (unsigned(next_y0) +  unsigned(cy) )  )
                                                              );
                            x0 <= next_x0;
                            y0 <= next_y0;
                            drawing_state <= Octant5;
                            when Octant5 =>
                                --DrawPixel(-x + cx, -y + cy); 
                            avm_m0_address <= std_logic_vector(unsigned(buf_addr)
                                                               + unsigned(cx) - unsigned(next_x0)
                                                               + (unsigned(FRAME_WIDTH) * (unsigned(cy) -  unsigned(next_y0) )  )
                                                              );
                            x0 <= next_x0;
                            y0 <= next_y0;
                            drawing_state <= Octant6;
                            when Octant6 =>
                                --DrawPixel(-y + cx, -x + cy);
                            avm_m0_address <= std_logic_vector(unsigned(buf_addr)
                                                               + unsigned(cx) - unsigned(next_y0)
                                                               + (unsigned(FRAME_WIDTH) * (unsigned(cy) -  unsigned(next_x0) )  )
                                                              );
                            x0 <= next_x0;
                            y0 <= next_y0;
                            drawing_state <= Octant7;
                            when Octant7 =>
                                --DrawPixel( x + cx, -y + cy);
                            avm_m0_address <= std_logic_vector(unsigned(buf_addr)
                                                               + unsigned(cx) + unsigned(next_x0)
                                                               + (unsigned(FRAME_WIDTH) * (unsigned(cy) -  unsigned(next_y0) )  )
                                                              );
                            x0 <= next_x0;
                            y0 <= next_y0;
                            drawing_state <= Octant8;
                            when Octant8 =>
                                --DrawPixel( y + cx, -x + cy);
                            avm_m0_address <= std_logic_vector(unsigned(buf_addr)
                                                               + unsigned(cx) + unsigned(next_y0)
                                                               + (unsigned(FRAME_WIDTH) * (unsigned(cy) -  unsigned(next_x0) )  )
                                                              );
                            x0 <= next_x0;
                            y0 <= next_y0;
                            drawing_state <= MakeDecision;

                            when MakeDecision => --Here's the if logic from the pseudo-code on wikipedia.
                            next_y0 := next_y0 + to_signed(1, 16); --y++ from pseudo-code.

                                if decisionOver2 <= to_signed(0, 16) then
                                    decisionOver2 <= decisionOver2 + (to_signed(2, 16) * next_y0 + to_signed(1, 16));
                                else
                                    next_x0 := next_x0 - to_signed(-1, 16);
                                    decisionOver2 <= decisionOver2 + (to_signed(2, 16) * (next_y0 - next_x0) + to_signed(1, 16));
                                end if;


                            x0 <= next_x0;
                            y0 <= next_y0;
    

                        end case;

                        --Do a check here for the m0_address between buf_addr and (640,480) before we exit the process for a loop.    
--                        if  unsigned(avm_m0_address) < unsigned(buf_addr) then
--                             avm_m0_address <= buf_addr;
--                        end if;

--                        if unsigned(avm_m0_address) > unsigned(FRAME_HEIGHT + (FRAME_WIDTH * FRAME_HEIGHT)) then
 --                           avm_m0_address <= std_logic_vector(unsigned(FRAME_WIDTH * FRAME_HEIGHT));
  --                      end if;

                    end if;
				END IF;




				
				current_x <= next_x;
				current_y <= next_y;
			END IF;
		END IF;
	END PROCESS op;

	-- Update configuration registers
	-- In the case of drawing a rectangle, we can do some bounds-checking on
	-- the inputs now. In other cases (such as drawing a line) we might want
	-- to allow coordinates outside of the frame window, which would be clipped
	-- during the memory-writing stage of command execution.

    -- Registers: framebuffer00, x1, y1, x2, y2, color


	update_cfg : PROCESS(ncs_ci_clk)
	BEGIN
		IF rising_edge(ncs_ci_clk) THEN
			IF ncs_ci_reset = '1' THEN
				buf_addr <= (others => '0');
				x1 <= (others => '0');
				y1 <= (others => '0');
				rad <= (others => '0');
				color <= (others => '0');
			ELSIF avs_s0_write = '1' THEN
				CASE avs_s0_address IS
					WHEN REG_BUF_ADDR =>
						buf_addr <= avs_s0_writedata; -- Set the address of the framebuffer to write to. (Address of 0,0)
					WHEN REG_CX =>
						IF signed(avs_s0_writedata(15 downto 0)) < to_signed(0, 16) THEN
							cx <= (others => '0');
						ELSIF signed(avs_s0_writedata(15 downto 0)) >= FRAME_WIDTH THEN
							cx <= std_logic_vector(FRAME_WIDTH - to_signed(1, 16));
						ELSE
							cx <= avs_s0_writedata(15 downto 0);
						END IF;
					WHEN REG_CY =>
						IF signed(avs_s0_writedata(15 downto 0)) < to_signed(0, 16) THEN
							cy <= (others => '0');
						ELSIF signed(avs_s0_writedata(15 downto 0)) >= FRAME_HEIGHT THEN
							cy <= std_logic_vector(FRAME_HEIGHT - to_signed(1, 16));
						ELSE
							cy <= avs_s0_writedata(15 downto 0);
						END IF;
					WHEN REG_RAD =>
						IF (signed(avs_s0_writedata(15 downto 0)) >= FRAME_WIDTH) or 
                            (signed(avs_s0_writedata(15 downto 0)) < to_signed(0,16))  THEN
							rad <= std_logic_vector(FRAME_WIDTH - to_signed(1, 16));
						ELSE
							rad <= avs_s0_writedata(15 downto 0);
						END IF;
					WHEN REG_COLOR =>
						color <= avs_s0_writedata(7 downto 0);
					WHEN OTHERS =>
						-- Do nothing
				END CASE;
			END IF;
		END IF;
	END PROCESS update_cfg;

END ARCHITECTURE;
