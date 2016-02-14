/* This test program generates a simple video pattern in the device's SRAM.
 * 
 * The video pattern consists of a white box along the edges of the screen,
 * along with blocks of colour forming diagonal stripes across the screen.
 * 
 * The purpose of this program is to verify frame alignment on the video
 * output signal, i.e. to verify that the frame is not translated in some
 * direction, and that it is stationary.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>

int main()
{ 
	int row = 0;
	int col = 0;
	int color;
	alt_putstr("Starting write\n");

	for (row = 0; row < 480; row++)
	{
		for (col = 0; col < 640; col = col + 4)
		{
			color = ((row + col) % 256) << 0 | ((row + col) % 256) << 8 | ((row + col) % 256) << 16 | ((row + col) % 256) << 24;
			//color = 3;

			IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, color << 24 | color << 16 | color << 8 | color << 0);
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

	alt_putstr("Done.\n");
	while (1);

	return 0;
}
