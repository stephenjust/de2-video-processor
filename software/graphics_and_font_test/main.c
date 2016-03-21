
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
	unsigned int i = 0;
	pixbuf_t *pixbuf;

	graphics_init();
	pixbuf = graphics_get_final_buffer();

	alt_putstr("Restoring EGA Color Palette \n");
	switch_palette(&palette_ega);

	graphics_clear_screen();

	int wtf = 0;
	while (wtf < 32)
	{
		wtf++;
		// Draw to edges of screen
		graphics_draw_rectangle(pixbuf, 0, 0, 640-1, 480-1, 2); //Green border
		graphics_draw_rectangle(pixbuf, 1, 1, 640-2, 480-2, 14);

		// Draw the colors at the top of the screen
		for (i = 0; i < sizeof(color_array); i++)
		{
			graphics_draw_rectangle(pixbuf, i, 0, i, 0, color_array[i]);
		}

		graphics_draw_rectangle(pixbuf, 300, 50, 301, 127, 3);
		graphics_draw_rectangle(pixbuf, 201, 111, 400, 230, 7);
		graphics_draw_rectangle(pixbuf, 100, 200, 500, 430, 10);

		print2screen(pixbuf, 30, 30, 4, 3, "A+");

		print2screen(pixbuf, 100, 100, wtf%15, 8, "Winning!");

		print2screen(pixbuf, 200, 200, 8, 1, "Winning!");

		graphics_draw_circle(pixbuf, 300, 300, 3, 0, 0);

		graphics_draw_circle(pixbuf, 300, 300, 20, 0, 0);

		graphics_draw_circle(pixbuf, 400, 400, 20, 0, -1);

		graphics_draw_circle(pixbuf, 350, 350, 20, 0, -2);

		//Circle Clipping tests

		//Top Left
		graphics_draw_circle(pixbuf, 20, 20, 30, 0, -2);

		//Top
		graphics_draw_circle(pixbuf, 200, 20, 30, 3, -2);

		//Top Right
		graphics_draw_circle(pixbuf, 640-40, 20, 30, 4, -2);

		//Left
		graphics_draw_circle(pixbuf, 20, 200, 30, 5, -2);

		//Right
		graphics_draw_circle(pixbuf, 640-20, 200, 30, 6, -2);

		//Bottom Left
		graphics_draw_circle(pixbuf, 0, 480 - 20, 30, 7, -2);

		//Bottom
		graphics_draw_circle(pixbuf, 200, 480 - 20, 30, 8, -2);

		//Bottom Right
		graphics_draw_circle(pixbuf, 640-20, 480 - 20, 30, 9, -2);

		//Draw a completely filled circle.
		graphics_draw_circle(pixbuf, 480, 400, 20, 3, 1);


		//Draw a rounded Rect

		//graphics_draw_rounded_rect(pixbuf, x1, y1, x2, y2, radius, filled,  color);

		graphics_draw_rounded_rect(pixbuf, 350, 10, 400, 90 , 3, 1,  1);

		graphics_draw_rounded_rect(pixbuf, 410, 10, 430, 90 , 3, 0,  2);

		graphics_draw_rounded_rect(pixbuf, 440, 10, 490, 90 , 1, 0,  3);

		//Triangle
		for (i = 0; i < 20; i++){
			graphics_draw_line(pixbuf, 200, 200, 300, 270+i, 1);
		}

		//Triangles to the left of the donut.
		graphics_draw_rectangle(pixbuf, 30, 300, 130, 400, 4);


		graphics_draw_triangle(pixbuf, 80, 320, 60, 350, 125, 350, 1, 6);
		graphics_draw_triangle(pixbuf, 80, 380, 60, 350, 125, 350, 1, 7);
		graphics_draw_triangle(pixbuf, 180, 380, 180, 350, 125, 350, 1, 8);
		graphics_draw_triangle(pixbuf, 125, 350, 80, 380, 205, 395, 1,11);
		graphics_draw_triangle(pixbuf, 125, 350, 80, 380, 205, 395, 0,15);


		ALT_CI_CI_FRAME_DONE_0;

	}
	return 0;
}
