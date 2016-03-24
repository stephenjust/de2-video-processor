#include <system.h>
#include <io.h>

#include <graphics_commands.h>

int scale_input(int x){
	return x / 10;
}

void draw_ball(pixbuf_t *pixbuf, int x, int y, int color)
{
	x = scale_input(x);
	y = scale_input(y);
	graphics_draw_rectangle(pixbuf, x-1, y+3, x+1, y+3, color);
	graphics_draw_rectangle(pixbuf, x-2, y+2, x+2, y+2, color);
	graphics_draw_rectangle(pixbuf, x-3, y-1, x+3, y+1, color);
	graphics_draw_rectangle(pixbuf, x-2, y-2, x+2, y-2, color);
	graphics_draw_rectangle(pixbuf, x-1, y-3, x+1, y-3, color);
}

void draw_paddle(pixbuf_t *pixbuf, int x, int y)
{
	x = scale_input(x);
	y = scale_input(y);
	graphics_draw_rectangle(pixbuf, x-5, y-18, x+5, y+18, 0x00);
}

void draw_field(pixbuf_t *pixbuf)
{
	//black background
	//graphics_draw_rectangle(pixbuf, 0, 0, 640-1, 480-1, 0xFF);
	//Top and bottom white walls
	graphics_draw_rectangle(pixbuf, 0, 0, 640-1, 10, 0x00);
	graphics_draw_rectangle(pixbuf, 0, 480-11, 640-1, 480-1, 0x00);
	//dashed line down center court
	int i;
	for (i = 0; i < 16; i++){
		graphics_draw_rectangle(pixbuf, 317, i*30 + 13, 321, i*30 + 17, 0x00);
	}
}

void draw_int(pixbuf_t *pixbuf, int x, int y, int value, int color){
	/*
	 * Fonts based on https://saelaenx.files.wordpress.com/2012/10/emon-font-size-04-01.png
	 */
	if (value > 9 || value < 0)
		return;
	else if (value == 0){
		graphics_draw_rectangle(pixbuf, (x+2), (y+0), (x+12), (y+0), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+1), (x+13), (y+1), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+2), (x+14), (y+3), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+4), (x+3), (y+23), color);
		graphics_draw_rectangle(pixbuf, (x+11), (y+4), (x+14), (y+21), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+22), (x+14), (y+23), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+24), (x+13), (y+24), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+25), (x+12), (y+25), color);
	}

	else if (value == 1){
		graphics_draw_rectangle(pixbuf, (x+2), (y+0), (x+8), (y+0), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+1), (x+9), (y+1), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+2), (x+10), (y+3), color);
		graphics_draw_rectangle(pixbuf, (x+5), (y+4), (x+10), (y+21), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+22), (x+14), (y+25), color);
	}

	else if (value == 2){
		graphics_draw_rectangle(pixbuf, (x+0), (y+0), (x+12), (y+0), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+1), (x+13), (y+1), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+2), (x+14), (y+3), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+4), (x+14), (y+10), color);

		graphics_draw_rectangle(pixbuf, (x+2), (y+11), (x+14), (y+11), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+12), (x+14), (y+12), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+13), (x+13), (y+13), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+14), (x+12), (y+14), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+15), (x+3), (y+21), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+22), (x+14), (y+23), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+24), (x+14), (y+24), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+25), (x+14), (y+25), color);
	}

	else if (value == 3){
		graphics_draw_rectangle(pixbuf, (x+0), (y+0), (x+12), (y+0), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+1), (x+13), (y+1), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+2), (x+14), (y+3), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+4), (x+14), (y+10), color);

		graphics_draw_rectangle(pixbuf, (x+2), (y+11), (x+14), (y+14), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+15), (x+14), (y+21), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+22), (x+14), (y+23), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+24), (x+13), (y+24), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+25), (x+12), (y+25), color);
	}

	else if (value == 4){
		graphics_draw_rectangle(pixbuf, (x+0), (y+0), (x+3), (y+10), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+0), (x+14), (y+10), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+11), (x+14), (y+12), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+13), (x+14), (y+13), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+14), (x+14), (y+14), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+15), (x+14), (y+25), color);
	}

	else if (value == 5){
		graphics_draw_rectangle(pixbuf, (x+2), (y+0), (x+14), (y+0), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+1), (x+14), (y+1), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+2), (x+14), (y+3), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+4), (x+3), (y+10), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+11), (x+12), (y+11), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+12), (x+13), (y+12), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+13), (x+14), (y+13), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+14), (x+14), (y+14), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+15), (x+14), (y+21), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+22), (x+14), (y+23), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+24), (x+13), (y+24), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+25), (x+12), (y+25), color);
	}

	else if (value == 6){
		graphics_draw_rectangle(pixbuf, (x+2), (y+0), (x+12), (y+0), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+1), (x+12), (y+1), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+2), (x+12), (y+3), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+4), (x+3), (y+10), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+11), (x+12), (y+12), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+13), (x+13), (y+13), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+14), (x+14), (y+14), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+15), (x+14), (y+21), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+15), (x+3), (y+21), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+22), (x+14), (y+23), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+24), (x+13), (y+24), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+25), (x+12), (y+25), color);
	}

	else if (value == 7){
		graphics_draw_rectangle(pixbuf, (x+0), (y+0), (x+12), (y+0), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+1), (x+13), (y+1), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+2), (x+14), (y+3), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+4), (x+14), (y+25), color);
	}

	else if (value == 8){
		graphics_draw_rectangle(pixbuf, (x+2), (y+0), (x+12), (y+0), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+1), (x+13), (y+1), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+2), (x+14), (y+3), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+4), (x+3), (y+10), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+4), (x+14), (y+10), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+11), (x+14), (y+14), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+15), (x+14), (y+21), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+15), (x+3), (y+21), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+22), (x+14), (y+23), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+24), (x+13), (y+24), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+25), (x+12), (y+25), color);
	}

	else if (value == 9){
		graphics_draw_rectangle(pixbuf, (x+2), (y+0), (x+12), (y+0), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+1), (x+13), (y+1), color);
		graphics_draw_rectangle(pixbuf, (x+0), (y+2), (x+14), (y+3), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+4), (x+3), (y+10), color);

		graphics_draw_rectangle(pixbuf, (x+11), (y+4), (x+14), (y+10), color);

		graphics_draw_rectangle(pixbuf, (x+0), (y+11), (x+14), (y+12), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+13), (x+14), (y+13), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+14), (x+14), (y+14), color);


		graphics_draw_rectangle(pixbuf, (x+11), (y+15), (x+14), (y+21), color);


		graphics_draw_rectangle(pixbuf, (x+0), (y+22), (x+14), (y+23), color);
		graphics_draw_rectangle(pixbuf, (x+1), (y+24), (x+13), (y+24), color);
		graphics_draw_rectangle(pixbuf, (x+2), (y+25), (x+12), (y+25), color);
	}

	else
		return;
}

void draw_wall(pixbuf_t *pixbuf){
	//138 = brown, 14 = grey, 120=dark grey
	graphics_draw_rectangle(pixbuf, (280), (11), (360), (468), 14);
	int i, j, k;
	//Lines slow down the game speed by a fair bit.
	for (i = 0; i < 45; i++){
		graphics_draw_line(pixbuf,280, 20+i*10, 360, 20+i*10, 138);
		if (i%2 == 0){
			for (j = 0; j < 3; j++)
				graphics_draw_line(pixbuf, 300+j*20, 10+i*10, 300+j*20, 20+i*10, 120);
		}
		else{
			for (j = 0; j < 4; j++)
				graphics_draw_line(pixbuf,290+j*20, 10+i*10, 290+j*20, 20+i*10, 120);
		}
	}
}

void draw_grass(pixbuf_t *bmp_asset, pixbuf_t *sdram_buf, int p1_score, int p2_score){
	rect_t source_rect = {
			.p1 = {
					.x = 0,
					.y = 0
			},
			.p2 = {
					.x = 640 - 1,
					.y = 480*1-1
			}
	};
	point_t dest_offset = {
			.x = 0,
			.y = 0
	};

	copy_buffer_area(bmp_asset, sdram_buf, &source_rect, &dest_offset);
	draw_field(sdram_buf);
	/*Fancy numbers!*/
	draw_int(sdram_buf, 200+2,40-1, p1_score, 0xFF);
	draw_int(sdram_buf, 430+2,40-1, p2_score, 0xFF);
	draw_int(sdram_buf, 200,40, p1_score, 0x00);
	draw_int(sdram_buf, 430,40, p2_score, 0x00);
}

void draw_trump(pixbuf_t *bmp_asset, pixbuf_t *sdram_buf, int player){
	rect_t source_rect;
	point_t dest_offset_1;
	if (player == 1){
		source_rect.p1.x = 110;
		source_rect.p1.y = 960;
		source_rect.p2.x = 250;
		source_rect.p2.y = 1125;
		dest_offset_1.x = 0;
		dest_offset_1.y = 303;
	}
	else{
		source_rect.p1.x = 250;
		source_rect.p1.y = 960;
		source_rect.p2.x = 389;
		source_rect.p2.y = 1125;
		dest_offset_1.x = 500;
		dest_offset_1.y = 303;
	}

	copy_buffer_area_transparent(bmp_asset, sdram_buf, &source_rect, &dest_offset_1, 175);
}

void end_game(pixbuf_t *pixbuf, pixbuf_t *composited_pixbuf, int p1_score, int p2_score)
{
	int controller_value;
	while (1){
		controller_value = IORD_32DIRECT(GENESIS_0_BASE, 0);
		graphics_draw_rectangle(pixbuf, 0,0,639,479,0x00);
		if (p1_score > p2_score)
			print2screen(pixbuf, 90, 200, 0xFF, 4, "Player 1 Wins!");
		else
			print2screen(pixbuf, 90, 200, 0xFF, 4, "Player 2 Wins!");
		print2screen(pixbuf, 140, 400, 0xFF, 2, "Press A to Continue");

		graphics_layer_copy(pixbuf, composited_pixbuf);
		ALT_CI_CI_FRAME_DONE_0;

		if ((controller_value & (1 << 4)) || (controller_value & (1 << 14)))
			break;
	}
}






