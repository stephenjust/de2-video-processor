/* This test program generates a simple video pattern in the device's SDRAM.
 * 
 * The video pattern consists of a white box along the edges of the screen,
 * along with blocks of colour forming diagonal stripes across the screen,
 * which will alternate with the inverse of that.
 * 
 * The purpose of this program is to verify that frame swap from SDRAM works
 * without tearing between frames.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>

#define SDRAM_VIDEO_OFFSET 0x180000

int main()
{ 
	int row = 0;
	int col = 0;
	int color;
	int invert = 0;

	while (1)
	{
		alt_putstr("Starting write\n");
		for (row = 0; row < 480; row++)
		{
			for (col = 0; col < 640; col = col + 4)
			{
				color = ((row + col) % 256) << 0 | ((row + col) % 256) << 8 | ((row + col) % 256) << 16 | ((row + col) % 256) << 24;
				if (invert)
				{
					color = ~color;
				}
				if (row == 0 || row == 479)
				{
					IOWR_32DIRECT(SDRAM_0_BASE, SDRAM_VIDEO_OFFSET + row * 640 + col, 0xFFFFFFFF);
				}
				else if (col == 0)
				{
					IOWR_32DIRECT(SDRAM_0_BASE, SDRAM_VIDEO_OFFSET + row * 640 + col, 0x000000FF | color);
				}
				else if (col == 636)
				{
					IOWR_32DIRECT(SDRAM_0_BASE, SDRAM_VIDEO_OFFSET + row * 640 + col, 0xFF000000 | color);
				}
				else
				{
					IOWR_32DIRECT(SDRAM_0_BASE, SDRAM_VIDEO_OFFSET + row * 640 + col, color);
				}
			}
		}
		alt_putstr("Swapping Frame.\n");
		ALT_CI_CI_FRAME_DONE_0;
		invert = !invert;
	}

	return 0;
}
