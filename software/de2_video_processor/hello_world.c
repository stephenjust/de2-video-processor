/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include <system.h>
#include <io.h>

int main()
{
  printf("Hello from Nios II!\n");

  int genesis_value;
  while(1){
	  genesis_value = IORD_32DIRECT(GENESIS_0_BASE, 0);
	  /*Write some printf's to test if &'nd values correspond with button presses
	   * stuff like 1=up, 2=down, 4 = left, 8=right, 16=a, 32=b, 64=c, 128=start
	   */

	  printf("%08X\n", genesis_value);
	  if (genesis_value&0x1){
		  printf("Up was pressed\n");
	  }
	  if (genesis_value&0x2){
		  printf("Down was pressed\n");
	  }
	  if (genesis_value&0x3){
		  printf("Left was pressed\n");
	  }
	  if (genesis_value&0x4){
		  printf("Right was pressed\n");
	  }
	  if (genesis_value&0x5){
		  printf("A was pressed\n");
	  }
	  if (genesis_value&0x6){
		  printf("B was pressed\n");
	  }
	  if (genesis_value&0x7){
		  printf("C was pressed\n");
	  }
	  if (genesis_value&0x8){
		  printf("Start was pressed\n");
	  }
  }
  return 0;
}
