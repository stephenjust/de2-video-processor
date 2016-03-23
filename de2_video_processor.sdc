create_clock -name CLOCK_50 -period 20 [get_ports {CLOCK_50}]
create_clock -name CLOCK_27 -period 37.037 [get_ports {CLOCK_27}]

create_generated_clock -name CLOCK_VGA -source [get_ports {CLOCK_27}] \
-divide_by 15 -multiply_by 14 -duty_cycle 50 [get_nets {u0|altpll_1|sd1|_clk0}]

create_generated_clock -name CLOCK_DRAM -source [get_ports {CLOCK_50}] \
-multiply_by 3 -divide_by 2 -phase -81 [get_nets {u0|altpll_0|sd1|_clk0}]

create_generated_clock -name CLOCK_SYS -source [get_ports {CLOCK_50}] \
-multiply_by 3 -divide_by 2 [get_nets {u0|altpll_0|sd1|_clk1}]
