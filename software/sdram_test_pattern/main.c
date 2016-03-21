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

#include <palettes.h>
#include <graphics_commands.h>

int main()
{ 
	int row = 0;
	int col = 0;
	int color;
	int invert = 0;
	pixbuf_t *pixbuf;

	graphics_init();
	pixbuf = graphics_get_final_buffer();

	alt_putstr("Restoring default palette\n");
	switch_palette(&palette_332);

	while (1)
	{
		alt_putstr("Starting write\n");
		for (row = 0; row < 480; row++)
		{
			for (col = 0; col < 640; col = col + 4)
			{
				int *pixel_group_address = (int *) (pixbuf->base_address + row * 640 + col);
				color = ((row + col) % 256) << 0 | ((row + col) % 256) << 8 | ((row + col) % 256) << 16 | ((row + col) % 256) << 24;
				if (invert)
				{
					color = ~color;
				}
				if (row == 0 || row == 479)
				{
					*pixel_group_address = 0xFFFFFFFF;
				}
				else if (col == 0)
				{
					*pixel_group_address = 0x000000FF | color;
				}
				else if (col == 636)
				{
					*pixel_group_address = 0xFF000000 | color;
				}
				else
				{
					*pixel_group_address = color;
				}
			}
		}
		alt_putstr("Swapping Frame.\n");
		ALT_CI_CI_FRAME_DONE_0;
		invert = !invert;
	}

	return 0;
}
