/* This test program generates a simple pattern to test the draw_rect primitive.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>

#include <palettes.h>
#include <graphics_commands.h>

unsigned char color_array[] = {
		0xE0, 0xEC, 0xF4, 0xF8,
		0xFC, 0xFA, 0xBE, 0x9D,
		0x1D, 0x1F, 0x17, 0x13,
		0x73, 0xFB, 0xF1, 0xE2
};

int main()
{
	unsigned int row, col;
	unsigned int i = 0;
	unsigned int j = 0;

	graphics_init();

	alt_putstr("Restoring default palette\n");
	switch_palette(&palette_332);

	clear_screen();

	// Draw to edges of screen
	draw_rectangle(0, 0, 640-1, 480-1, 0xE0);
	draw_rectangle(1, 1, 640-2, 480-2, 0xFF);
	ALT_CI_CI_FRAME_DONE_0;

	// Draw the colors at the top of the screen
	for (i = 0; i < sizeof(color_array); i++)
	{
		draw_rectangle(i, 0, i, 0, color_array[i]);
	}

	// Cycle through a pattern
	for (i = 0; i < 256; i++)
	{
		draw_rectangle(300, 50, 301, 127, color_array[i % sizeof(color_array)]);
		draw_rectangle(201, 111, 400, 230, color_array[(i+4) % sizeof(color_array)]);
		draw_rectangle(100, 200, 500, 430, color_array[(i+8) % sizeof(color_array)]);
		ALT_CI_CI_FRAME_DONE_0;
		for (j = 0; j < 10000; j++)
		{
			// Do nothing
		}
	}

	clear_screen();

	// Draw a geometric pattern in the most inefficient way possible...
	for (row = 0; row < 480; row++)
	{
		for (col = 0; col < 640; col++)
		{
			if (row == col) {
				draw_rectangle(col, row, col, row, 0xFF);
			}
			if (row == col - 160) {
				draw_rectangle(col, row, col, row, 0xFF);
			}
			if (480 - row - 1 == col) {
				draw_rectangle(col, row, col, row, 0xFF);
			}
			if (480 - row - 1 == col - 160) {
				draw_rectangle(col, row, col, row, 0xFF);
			}
		}
	}
	ALT_CI_CI_FRAME_DONE_0;
	for ( i = 0; i < 100000; i++)
	{
		//wait for a while
	}
	clear_screen();
	int genesis_value;
	while(1)
	{
		genesis_value = IORD_32DIRECT(GENESIS_0_BASE, 0);
		if ((genesis_value)& (1 << 4)){
			draw_rectangle(100, 100, 150, 150, 0x1A);
			  }
		else
			draw_rectangle(100, 100, 150, 150, 0x0);
			  if ((genesis_value)& (1 << 5)){
				  draw_rectangle(200, 200, 250, 250, 0xE0);
			  }
			  else
				  draw_rectangle(200, 200, 250, 250, 0x0);
			  if ((genesis_value)& (1 << 6)){
				  draw_rectangle(300, 300, 350, 350, 0x03);
			  }
			  else
				  draw_rectangle(300, 300, 350, 350, 0x0);

			  ALT_CI_CI_FRAME_DONE_0;
		// Do nothing
	}

	return 0;
}
