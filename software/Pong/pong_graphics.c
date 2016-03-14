#include <system.h>
#include <io.h>

#include "graphics_commands.h"

void draw_ball(int x, int y)
{
	draw_rectangle(x-1, y+3, x+1, y+3, 0xFF);
	draw_rectangle(x-2, y+2, x+2, y+2, 0xFF);
	draw_rectangle(x-3, y-1, x+3, y+1, 0xFF);
	draw_rectangle(x-2, y-2, x+2, y-2, 0xFF);
	draw_rectangle(x-1, y-3, x+1, y-3, 0xFF);
}

void draw_paddle(int x, int y)
{
	draw_rectangle(x-5, y-18, x+5, y+18, 0xFF);
}

void draw_field()
{
	//black background
	draw_rectangle(0, 0, 640-1, 480-1, 0x00);
	//Top and bottom white walls
	draw_rectangle(0, 0, 640-1, 10, 0xFF);
	draw_rectangle(0, 480-11, 640-1, 480-1, 0xFF);
	//dashed line down center court
	int i;
	for (i = 0; i < 16; i++){
		draw_rectangle(317, i*30 + 13, 321, i*30 + 17, 0xFF);
	}
}






