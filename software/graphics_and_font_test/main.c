
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

	graphics_init();

	alt_putstr("Restoring EGA Color Palette \n");
	switch_palette(&palette_ega);

	clear_screen();

	int wtf = 0;
	while (wtf < 32)
	{
		wtf++;
		// Draw to edges of screen
		draw_rectangle(0, 0, 640-1, 480-1, 2); //Green border
		draw_rectangle(1, 1, 640-2, 480-2, 14);

		// Draw the colors at the top of the screen
		for (i = 0; i < sizeof(color_array); i++)
		{
			draw_rectangle(i, 0, i, 0, color_array[i]);
		}

		draw_rectangle(300, 50, 301, 127, 3);
		draw_rectangle(201, 111, 400, 230, 7);
		draw_rectangle(100, 200, 500, 430, 10);

		print2screen(30, 30, 4, 3, "A+");

		print2screen(100, 100, wtf%15, 8, "Winning!");

		print2screen(200, 200, 8, 1, "Winning!");

		draw_circle (300, 300, 3, 0, 0);

		draw_circle (300, 300, 20, 0, 0);

		draw_circle (400, 400, 20, 0, -1);

		draw_circle (350, 350, 20, 0, -2);

		//Circle Clipping tests

		//Top Left
		draw_circle (20, 20, 30, 0, -2);

		//Top
		draw_circle (200, 20, 30, 3, -2);

		//Top Right
		draw_circle (640-40, 20, 30, 4, -2);

		//Left
		draw_circle (20, 200, 30, 5, -2);

		//Right
		draw_circle (640-20, 200, 30, 6, -2);

		//Bottom Left
		draw_circle (0, 480 - 20, 30, 7, -2);

		//Bottom
		draw_circle (200, 480 - 20, 30, 8, -2);

		//Bottom Right
		draw_circle (640-20, 480 - 20, 30, 9, -2);

		//Draw a completely filled circle.
		draw_circle (480, 400, 20, 3, 1);


		//Draw a rounded Rect

		//draw_rounded_rect(x1, y1, x2, y2, radius, filled,  color);

		draw_rounded_rect(350, 10, 400, 90 , 3, 1,  1);

		draw_rounded_rect(410, 10, 430, 90 , 3, 0,  2);

		draw_rounded_rect(440, 10, 490, 90 , 1, 0,  3);

		//Triangle
		for (i = 0; i < 20; i++){
			draw_line(200, 200, 300, 270+i, 1);
		}

		//Triangles to the left of the donut.
		draw_rectangle(30, 300, 130, 400, 4);


		draw_triangle (80, 320, 60, 350, 125, 350, 1, 6);
		draw_triangle (80, 380, 60, 350, 125, 350, 1, 7);
		draw_triangle (180, 380, 180, 350, 125, 350, 1, 8);
		draw_triangle (125, 350, 80, 380, 205, 395, 1,11);
		draw_triangle (125, 350, 80, 380, 205, 395, 0,15);


		ALT_CI_CI_FRAME_DONE_0;

	}
	return 0;
}
