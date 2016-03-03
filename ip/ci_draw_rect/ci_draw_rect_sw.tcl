# Create a new driver - this name must be different than the 
# hardware component name
create_driver ci_draw_rect_driver

# Associate it with some hardware
set_sw_property hw_class_name ci_draw_rect

# The version of this driver is "1.0"
set_sw_property version 1.0

# This driver is proclaimed to be compatible with 'component'
# as old as version "12.0". The component hardware version is set in the 
# _hw.tcl file - If the hardware component  version number is not equal 
# or greater than the min_compatable_hw_version number, the driver 
# source files will not be copied over to the BSP driver directory
set_sw_property min_compatible_hw_version 1.0

# Initialize the driver in alt_sys_init()
set_sw_property auto_initialize true

# Location in generated BSP that sources will be copied into
set_sw_property bsp_subdirectory drivers

#
# Source file listings...
#

# C/C++ source files
add_sw_property c_source HAL/src/ci_draw_rect.c

# Include files
add_sw_property include_source HAL/inc/ci_draw_rect.h

# This driver supports HAL type
add_sw_property supported_bsp_type HAL

# End of file

