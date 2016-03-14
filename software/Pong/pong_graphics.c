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

void draw_int(int x, int y, int value, int color){
	if (value > 9 || value < 0)
		return;
	else if (value == 0){
		draw_rectangle( (x+2), (y+0), (x+12), (y+0), color);
		draw_rectangle( (x+1), (y+1), (x+13), (y+1), color);
		draw_rectangle( (x+0), (y+2), (x+14), (y+3), color);
		draw_rectangle( (x+0), (y+4), (x+3), (y+23), color);
		draw_rectangle( (x+11), (y+4), (x+14), (y+23), color);
		draw_rectangle( (x+0), (y+24), (x+14), (y+25), color);
		draw_rectangle( (x+1), (y+26), (x+13), (y+26), color);
		draw_rectangle( (x+2), (y+27), (x+12), (y+27), color);
	}

	else if (value == 1){
		draw_rectangle( (x+1), (y+1), (x+1), (y+1), color);
		draw_rectangle( (x+2), (y), (x+2), (y+4), color);
	}

	else if (value == 2){
		draw_rectangle( (x+0), (y+0), (x+3), (y+0), color);
		draw_rectangle( (x+4), (y+1), (x+4), (y+1), color);
		draw_rectangle( (x+1), (y+2), (x+3), (y+2), color);
		draw_rectangle( (x+0), (y+3), (x+0), (y+4), color);
		draw_rectangle( (x+1), (y+4), (x+4), (y+4), color);
	}

	else if (value == 3){
		draw_rectangle( (x+0), (y+0), (x+3), (y+0), color);
		draw_rectangle( (x+4), (y+1), (x+4), (y+1), color);
		draw_rectangle( (x+1), (y+2), (x+3), (y+2), color);
		draw_rectangle( (x+4), (y+3), (x+4), (y+3), color);
		draw_rectangle( (x+0), (y+4), (x+3), (y+4), color);
	}

	else
		return;

}






