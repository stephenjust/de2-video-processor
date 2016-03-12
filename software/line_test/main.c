/* This test program generates a simple pattern to test the draw_rect primitive.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>

#define PALETTE_SIZE 256
#define SDRAM_VIDEO_OFFSET 0x300000

unsigned char color_array[] = {
		0xE0, 0xEC, 0xF4, 0xF8,
		0xFC, 0xFA, 0xBE, 0x9D,
		0x1D, 0x1F, 0x17, 0x13,
		0x73, 0xFB, 0xF1, 0xE2
};

void draw_rectangle(int x1, int y1, int x2, int y2, unsigned char color)
{
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 0, SDRAM_0_BASE + SDRAM_VIDEO_OFFSET); // Frame address
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 4, x1); // X1
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 8, y1); // Y1
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 12, x2); // X2
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 16, y2); // Y2
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 20, color); // Color
	ALT_CI_CI_DRAW_RECT_0;
}

void draw_line(int x1, int y1, int x2, int y2, unsigned char color)
{
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 0, SDRAM_0_BASE + SDRAM_VIDEO_OFFSET); // Frame address
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 4, x1); // X1
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 8, y1); // Y1
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 12, x2); // X2
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 16, y2); // Y2
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 20, color); // Color
	ALT_CI_CI_DRAW_LINE_0;
}

void clear_screen()
{
	draw_rectangle(0, 0, 640-1, 480-1, 0x00);
	ALT_CI_CI_FRAME_DONE_0;
}

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
		draw_rectangle(1, 1, 640-2, 480-2, 5);
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


		alt_putstr("Drawing line. \n");



		clear_screen();
		draw_rectangle(0,0, 20, 20, 3);

		//Draw with increasing x, increasing y.
		draw_line(20, 20, 480, 300, 14);
		//draw_line(110, 120, 48, 300, 14);

		//Draw with decreasing x, increasing y.
		draw_line(420, 20, 20, 300, 13);

		//Draw with increasing x, decreasing y.
		draw_line(20, 20, 200, 10, 12);

		//Draw with decreasing x, decreasing y.
		//FAILS
		draw_line(200, 10, 120, 5, 11);

		ALT_CI_CI_FRAME_DONE_0;
		//Test how close to vertical we can get. (With one line per frame).
		draw_line(200, 200, 300, 200, 1);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 220, 2);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 230, 3);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 235, 4);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 240, 5);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 245, 6);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 249, 7);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 250, 8);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 260, 9);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 270, 10);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 280, 11);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 290, 12);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 300, 13);
			//Now start making >45 degree lines.
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 300, 300, 1);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 290, 300, 2);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 280, 300, 3);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 270, 300, 4);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 260, 300, 5);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 250, 300, 6);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 240, 300, 7);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 230, 300, 8);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 220, 300, 9);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 210, 300, 10);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 200, 300, 11);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 190, 300, 12);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 180, 300, 13);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 170, 300, 14);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 160, 300, 15);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 150, 300, 1);
		ALT_CI_CI_FRAME_DONE_0;
		draw_line(200, 200, 140, 300, 2);


		//Draw an almost vertical line, with increasing y.
		draw_line(20, 20, 21, 200, 10);


		//Draw an almost vertical line, with decreasing y.
		draw_line(40, 200, 41, 20, 9);



		//Draw a vertical line, with increasing y.
		draw_line(20, 20, 20, 200, 10);


		//Draw a vertical line, with decreasing y.
		draw_line(30, 200, 30, 20, 10);


		ALT_CI_CI_FRAME_DONE_0;

		alt_putstr("Done Drawing line. \n");

	return 0;
}
