/* This test program generates a simple pattern to test for tearing.
 *
 * The video pattern consists of a white vertical line that will move
 * from side to side along the frame. If there is tearing, the line will
 * appear broken at some points in time.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>

#include <palettes.h>
#include <graphics_commands.h>

int main()
{
	int row = 0;
	int col = 0;
	int color;

	unsigned int i = 0;
	unsigned int delay = 0;
	pixbuf_t *pixbuf;

	graphics_init();
	pixbuf = graphics_get_final_buffer();


	alt_putstr("Restoring default palette\n");
	switch_palette(&palette_332);

	alt_putstr("Clear screen\n");
	graphics_clear_screen();

	alt_putstr("Drawing pattern\n");
	unsigned int position = 0;
	while (1)
	{
		if (position == 0) {
			graphics_draw_line(pixbuf, 640-8, 0, 640-8, 479, 0x00);
		} else {
			graphics_draw_line(pixbuf, position-8, 0, position-8, 479, 0x00);
		}
		graphics_draw_line(pixbuf, position, 0, position, 479, 0xFF);

		position = (position + 8) % 640;
		ALT_CI_CI_FRAME_DONE_0;
	}

	alt_putstr("Done.\n");
	while (1);

	return 0;
}
