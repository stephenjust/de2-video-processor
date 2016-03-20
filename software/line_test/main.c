/* This test program generates a simple pattern to test the draw_rect primitive.
 */

#include <io.h>
#include <system.h>
#include <stdio.h>

#include <graphics_commands.h>
#include <palettes.h>


int main()
{
	unsigned int j = 0;

	graphics_init();
	printf("Restoring EGA Color Palette \n");
	switch_palette(&palette_ega);

	clear_screen();

	// Draw to edges of screen
	draw_rectangle(0, 0, 640-1, 480-1, 2); //Green border
	draw_rectangle(1, 1, 640-2, 480-2, 5);
	ALT_CI_CI_FRAME_DONE_0;

	draw_rectangle(300, 50, 301, 127, 3);
	draw_rectangle(201, 111, 400, 230, 7);
	draw_rectangle(100, 200, 500, 430, 10);
	ALT_CI_CI_FRAME_DONE_0;

	for (j = 0; j < 10000; j++)
	{
	// Do nothing
	}


	printf("Drawing line. \n");



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

	printf("Done Drawing line. \n");

	return 0;
}
