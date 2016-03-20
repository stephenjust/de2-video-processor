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
#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>

#include <graphics_defs.h>
#include <graphics_commands.h>
#include <palettes.h>

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480

palette_t *palettes[] = {
		&palette_ega, &palette_332, &palette_magenta, &palette_bunch_o_blues, &palette_bunch_o_reds
};
int palette_count = 5;

int main()
{


	unsigned int row = 0;
	unsigned int col = 0;
	unsigned int delay = 0;

	unsigned int color;
	int i;



	// Clear the screen first.
	alt_putstr("Clear the screen\n");
	for (col = 0; col < FRAME_WIDTH; col = col + 4){
		for (row = 0; row < FRAME_HEIGHT; ++row){
			color = 0;
			IOWR_32DIRECT(SRAM_0_BASE, row * FRAME_WIDTH + col, color << 24 | color << 16 | color << 8 | color << 0);
		}
	}

	// Print first 16 elements of current palette
	print_palette(16);
	// Switch to EGA colour palette
	switch_palette(&palette_ega);
	print_palette(16);

	alt_putstr("Screen painting demo\n");

	// Cycle through the colours of the EGA colour palette
	for (i = 0; i < 16; i++){
		for (delay = 0; delay < 700/*2000*/; delay++){
			unsigned int tdelay = delay;
			for (tdelay; tdelay > 0; tdelay--){}
		}

		for (row=0; row<480; row++){
			for (col = 0; col < 640; col=col+4){
				color = i;
				IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, color << 24 | color << 16 | color << 8 | color << 0);
			}

		}
	}



	alt_putstr("\nStarting Stephen Test Pattern\n");
	int p;
	for (p = 0; p < palette_count; p++)
	{
		switch_palette(palettes[p]);

		alt_putstr("Clear the screen\n");
		for (col = 0; col < FRAME_WIDTH; col = col + 4){
			for (row = 0; row < FRAME_HEIGHT; row++){
				color = 0;
				IOWR_32DIRECT(SRAM_0_BASE, row * FRAME_WIDTH + col, color << 24 | color << 16 | color << 8 | color << 0);
			}
		}


		//Now draw the test pattern
		for (row = 0; row < 480; row++)
		{
			for (col = 0; col < 640; col = col + 4)
			{
				color = ((row + col) % 256) << 0 | ((row + col) % 256) << 8 | ((row + col) % 256) << 16 | ((row + col) % 256) << 24;
				if (row == 0 || row == 479)
				{
					IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, 0xFFFFFFFF);
				}
				else if (col == 0)
				{
					IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, 0x000000FF | color);
				}
				else if (col == 636)
				{
					IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, 0xFF000000 | color);
				}
				else
				{
					IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, color);
				}
			}
		}
	}





	alt_putstr("Done.\n");


  return 0;
}
