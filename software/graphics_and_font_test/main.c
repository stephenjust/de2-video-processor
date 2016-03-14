
#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>

#include "graphics_commands.h"



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
	unsigned int delay = 0;

	alt_putstr("Restoring EGA Color Palette \n");
	unsigned int palette_ega[16] =
			/* EGA Colour Palette */
			/* Black			  Blue	       Green       Cyan          Red */
			{/*00*/ 0x0000, /*01*/0x0015, /*02*/0x2704, /*03*/0x1E79, /*04*/0xA800,
			 /*     Magenta       Brown   Light Grey     Dark Grey   Bright Blue*/
			 /*05*/ 0xA815, /*06*/0xE3C1, /*07*/0xAD55, /*08*/0x52AA, /*09*/0x52BF,
			 /* BGreen            BCyan         Bred       B Magenta     B Yellow*/
			 /*10*/ 0x57EA, /*11*/0x57FF, /*12*/0xFAAA, /*13*/0xFABF, /*14*/0xFFEA,
			 0xFFFF /*B White */
			};


	for (i = 0; i < 16; i++){
		IOWR_16DIRECT(COLOUR_PALETTE_SHIFTER_0_BASE, 2*i, 0x0000);
		IOWR_16DIRECT(COLOUR_PALETTE_SHIFTER_0_BASE, 2*i, palette_ega[i]);
	}


	clear_screen();

//	while (1)
//	{
		// Draw to edges of screen
		draw_rectangle(0, 0, 640-1, 480-1, 2); //Green border
		draw_rectangle(1, 1, 640-2, 480-2, 14);
		ALT_CI_CI_FRAME_DONE_0;

		// Draw the colors at the top of the screen
		for (i = 0; i < sizeof(color_array); i++)
		{
			draw_rectangle(i, 0, i, 0, color_array[i]);
		}

		draw_rectangle(300, 50, 301, 127, 3);
		draw_rectangle(201, 111, 400, 230, 7);
		draw_rectangle(100, 200, 500, 430, 10);
		ALT_CI_CI_FRAME_DONE_0;

		for (j = 0; j < 10000; j++)
		{
		// Do nothing
		}

		print2screen(30, 30, 4, 3, "A+");
		ALT_CI_CI_FRAME_DONE_0;

		print2screen(100, 100, 3, 8, "Winning!");
		ALT_CI_CI_FRAME_DONE_0;

		print2screen(200, 200, 8, 1, "Winning!");
		ALT_CI_CI_FRAME_DONE_0;
		//alt_printf("%d", atoi('F'));

		alt_putstr("Drawing circle \n");


		//Do not run as of yet. Currently overwrites palettes.
		draw_circle (300, 300, 3, 0, 0);
		ALT_CI_CI_FRAME_DONE_0;

		draw_circle (300, 300, 20, 0, 0);
		ALT_CI_CI_FRAME_DONE_0;

		draw_circle (400, 400, 20, 0, -1);
		ALT_CI_CI_FRAME_DONE_0;

		draw_circle (350, 350, 20, 0, -2);
		ALT_CI_CI_FRAME_DONE_0;

		//Circle Clipping tests

		//Top Left
		draw_circle (20, 20, 30, 0, -2);
		ALT_CI_CI_FRAME_DONE_0;

		//Top
		draw_circle (200, 20, 30, 3, -2);
		ALT_CI_CI_FRAME_DONE_0;

		//Top Right
		draw_circle (640-40, 20, 30, 4, -2);
		ALT_CI_CI_FRAME_DONE_0;

		//Left
		draw_circle (20, 200, 30, 5, -2);
		ALT_CI_CI_FRAME_DONE_0;

		//Right
		draw_circle (640-20, 200, 30, 6, -2);
		ALT_CI_CI_FRAME_DONE_0;

		//Bottom Left
		draw_circle (0, 480 - 20, 30, 7, -2);
		ALT_CI_CI_FRAME_DONE_0;

		//Bottom
		draw_circle (200, 480 - 20, 30, 8, -2);
		ALT_CI_CI_FRAME_DONE_0;

		//Bottom Right
		draw_circle (640-20, 480 - 20, 30, 9, -2);
		ALT_CI_CI_FRAME_DONE_0;


		//Draw a completely filled circle.
		draw_circle (480, 400, 20, 3, 1);
		ALT_CI_CI_FRAME_DONE_0;


		//Draw a rounded Rect

		//draw_rounded_rect(x1, y1, x2, y2, radius, filled,  color);

		draw_rounded_rect(350, 10, 400, 90 , 3, 1,  1);
		ALT_CI_CI_FRAME_DONE_0;

		draw_rounded_rect(410, 10, 430, 90 , 3, 0,  2);
		ALT_CI_CI_FRAME_DONE_0;

		draw_rounded_rect(440, 10, 490, 90 , 1, 0,  3);
		ALT_CI_CI_FRAME_DONE_0;

		//Triangle
		i = 0;
		for (i ; i < 20; i++){
			draw_line(200, 200, 300, 270+i, 1);
		}
		ALT_CI_CI_FRAME_DONE_0;

		//Triangles to the left of the donut.
		draw_rectangle(30, 300, 130, 400, 4);
		ALT_CI_CI_FRAME_DONE_0;


		i = 0;

		int tx1 = 80;
		int ty1 = 320;

		int tx2 = 60;
		int ty2 = 350;

		int tx3 = 125;
		int ty3 = 350;

		draw_triangle (80, 320, 60, 350, 125, 350, 1, 6);
		draw_triangle (80, 380, 60, 350, 125, 350, 1, 7);

		draw_triangle (125, 350, 80, 380, 205, 385, 1,11);
		draw_triangle (125, 350, 80, 380, 205, 385, 0,15);


		ALT_CI_CI_FRAME_DONE_0;

		alt_putstr("Done \n");

	return 0;
}
