/*
 * The game of Pong using Genesis Controllers and Hardware Drawing
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>

#define PALETTE_SIZE 256
#define SDRAM_VIDEO_OFFSET 0x300000

struct Paddle{
	int x;
	int y;
};

struct Ball{
	int x;
	int y;
	int velocity_x;
	int velocity_y;
};

unsigned char color_array[] = {
		0xE0, 0xEC, 0xF4, 0xF8,
		0xFC, 0xFA, 0xBE, 0x9D,
		0x1D, 0x1F, 0x17, 0x13,
		0x73, 0xFB, 0xF1, 0xE2
};

struct Ball find_end_point(struct Ball my_ball, struct Paddle p1, struct Paddle p2){
	int x_endleft = p1.x + 5;
	int x_endright = p2.x - 5;
	int y_steps, x_steps, new_x, new_y;
	if (my_ball.velocity_y < 0)
		y_steps = (13 - my_ball.y) / my_ball.velocity_y;
	if (my_ball.velocity_y > 0)
		y_steps = (466 - my_ball.y) / my_ball.velocity_y;
	if (my_ball.velocity_x > 0)
		x_steps = (x_endright - my_ball.x) / my_ball.velocity_x;
	if (my_ball.velocity_x < 0)
		x_steps = (x_endleft - my_ball.x) / my_ball.velocity_x;
	if (x_steps > y_steps){
		new_y = y_steps*my_ball.velocity_y + my_ball.y;
		new_x = y_steps*my_ball.velocity_x + my_ball.x;
		struct Ball ball = {.y = new_y, .x = new_x,
				.velocity_x = my_ball.velocity_x, .velocity_y = my_ball.velocity_y*-1};
		return ball;
	}
	else {
		new_y = x_steps*my_ball.velocity_y + my_ball.y;
		new_x = x_steps*my_ball.velocity_x + my_ball.x;
		struct Ball ball = {.y = new_y, .x = new_x,
				.velocity_x = my_ball.velocity_x*-1, .velocity_y = my_ball.velocity_y};
		return ball;
	}
}

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

void clear_screen()
{
	draw_rectangle(0, 0, 640-1, 480-1, 0x00);
	ALT_CI_CI_FRAME_DONE_0;
}

int main()
{
	int ball_speed = 1;
	int toggle_raytracing = 0;
	int toggle_counter = 0;
	struct Paddle paddle1 = {.y = 240, .x = 20};
	struct Paddle paddle2 = {.y = 240, .x = 620};
	struct Ball ball = {.y = 240, .x = 320, .velocity_x = ball_speed, .velocity_y = ball_speed};
	int paddle_speed = 8;
	int p1_score = 0;
	int p2_score = 0;
	int collision_counter = 0;
	unsigned int controller_value;
	unsigned int row, col;
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int delay = 0;

	alt_putstr("Restoring default palette\n");

	unsigned int palette_default[PALETTE_SIZE] =
			{0x0 , 0xa , 0x15 , 0x1f , 0x120 , 0x12a , 0x135 , 0x13f , 0x240 ,
			0x24a , 0x255 , 0x25f , 0x360 , 0x36a , 0x375 , 0x37f , 0x480 ,
			0x48a , 0x495 , 0x49f , 0x5a0 , 0x5aa , 0x5b5 , 0x5bf , 0x6c0 ,
			0x6ca , 0x6d5 , 0x6df , 0x7e0 , 0x7ea , 0x7f5 , 0x7ff , 0x2000 ,
			0x200a , 0x2015 , 0x201f , 0x2120 , 0x212a , 0x2135 , 0x213f , 0x2240 ,
			0x224a , 0x2255 , 0x225f , 0x2360 , 0x236a , 0x2375 , 0x237f , 0x2480 ,
			0x248a , 0x2495 , 0x249f , 0x25a0 , 0x25aa , 0x25b5 , 0x25bf , 0x26c0 ,
			0x26ca , 0x26d5 , 0x26df , 0x27e0 , 0x27ea , 0x27f5 , 0x27ff , 0x4800 ,
			0x480a , 0x4815 , 0x481f , 0x4920 , 0x492a , 0x4935 , 0x493f , 0x4a40 ,
			0x4a4a , 0x4a55 , 0x4a5f , 0x4b60 , 0x4b6a , 0x4b75 , 0x4b7f , 0x4c80 ,
			0x4c8a , 0x4c95 , 0x4c9f , 0x4da0 , 0x4daa , 0x4db5 , 0x4dbf , 0x4ec0 ,
			0x4eca , 0x4ed5 , 0x4edf , 0x4fe0 , 0x4fea , 0x4ff5 , 0x4fff , 0x6800 ,
			0x680a , 0x6815 , 0x681f , 0x6920 , 0x692a , 0x6935 , 0x693f , 0x6a40 ,
			0x6a4a , 0x6a55 , 0x6a5f , 0x6b60 , 0x6b6a , 0x6b75 , 0x6b7f , 0x6c80 ,
			0x6c8a , 0x6c95 , 0x6c9f , 0x6da0 , 0x6daa , 0x6db5 , 0x6dbf , 0x6ec0 ,
			0x6eca , 0x6ed5 , 0x6edf , 0x6fe0 , 0x6fea , 0x6ff5 , 0x6fff , 0x9000 ,
			0x900a , 0x9015 , 0x901f , 0x9120 , 0x912a , 0x9135 , 0x913f , 0x9240 ,
			0x924a , 0x9255 , 0x925f , 0x9360 , 0x936a , 0x9375 , 0x937f , 0x9480 ,
			0x948a , 0x9495 , 0x949f , 0x95a0 , 0x95aa , 0x95b5 , 0x95bf , 0x96c0 ,
			0x96ca , 0x96d5 , 0x96df , 0x97e0 , 0x97ea , 0x97f5 , 0x97ff , 0xb000 ,
			0xb00a , 0xb015 , 0xb01f , 0xb120 , 0xb12a , 0xb135 , 0xb13f , 0xb240 ,
			0xb24a , 0xb255 , 0xb25f , 0xb360 , 0xb36a , 0xb375 , 0xb37f , 0xb480 ,
			0xb48a , 0xb495 , 0xb49f , 0xb5a0 , 0xb5aa , 0xb5b5 , 0xb5bf , 0xb6c0 ,
			0xb6ca , 0xb6d5 , 0xb6df , 0xb7e0 , 0xb7ea , 0xb7f5 , 0xb7ff , 0xd800 ,
			0xd80a , 0xd815 , 0xd81f , 0xd920 , 0xd92a , 0xd935 , 0xd93f , 0xda40 ,
			0xda4a , 0xda55 , 0xda5f , 0xdb60 , 0xdb6a , 0xdb75 , 0xdb7f , 0xdc80 ,
			0xdc8a , 0xdc95 , 0xdc9f , 0xdda0 , 0xddaa , 0xddb5 , 0xddbf , 0xdec0 ,
			0xdeca , 0xded5 , 0xdedf , 0xdfe0 , 0xdfea , 0xdff5 , 0xdfff , 0xf800 ,
			0xf80a , 0xf815 , 0xf81f , 0xf920 , 0xf92a , 0xf935 , 0xf93f , 0xfa40 ,
			0xfa4a , 0xfa55 , 0xfa5f , 0xfb60 , 0xfb6a , 0xfb75 , 0xfb7f , 0xfc80 ,
			0xfc8a , 0xfc95 , 0xfc9f , 0xfda0 , 0xfdaa , 0xfdb5 , 0xfdbf , 0xfec0 ,
			0xfeca , 0xfed5 , 0xfedf , 0xffe0 , 0xffea , 0xfff5 , 0xffff };

	for (i = 0; i < PALETTE_SIZE; i++)
	{
		IOWR_16DIRECT(COLOUR_PALETTE_SHIFTER_0_BASE, 2*i, palette_default[i]);
	}


	clear_screen();

	while(1){
		/* Read From Controllers*/
		controller_value = IORD_32DIRECT(GENESIS_0_BASE, 0);
		/*player 1*/
		if (controller_value & (1 << 0)){
			if (paddle1.y > 29){
				paddle1.y -= paddle_speed;
			}
			else
				paddle1.y = 29;
		}
		if (controller_value & (1 << 1)){
			if (paddle1.y < 450){
				paddle1.y += paddle_speed;
			}
			else
				paddle1.y = 450;
		}
		if ( (controller_value & (1 << 2)) && (controller_value & (1 << 5)) ){
			if (paddle1.x > 5){
				paddle1.x -= paddle_speed;
			}
			else
				paddle1.x = 5;
		}
		if ( (controller_value & (1 << 3)) && (controller_value & (1 << 5)) ){
			if (paddle1.x < 635){
				paddle1.x += paddle_speed;
			}
			else
				paddle1.x = 635;
		}
		if ( (controller_value & (1 << 4)) && toggle_counter == 0){
			toggle_raytracing = !toggle_raytracing;
			toggle_counter = 30;
		}

		/*player 2*/
		if (controller_value & (1 << 10)){
			if (paddle2.y > 29){
				paddle2.y -= paddle_speed;
			}
			else
				paddle2.y = 29;
		}
		if (controller_value & (1 << 11)){
			if (paddle2.y < 450){
				paddle2.y += paddle_speed;
			}
			else
				paddle2.y = 450;
		}
		if ( (controller_value & (1 << 12)) && (controller_value & (1 << 15)) ){
			if (paddle2.x > 5){
				paddle2.x -= paddle_speed;
			}
			else
				paddle2.x = 5;
		}
		if ( (controller_value & (1 << 13)) && (controller_value & (1 << 15)) ){
			if (paddle2.x < 635){
				paddle2.x += paddle_speed;
			}
			else
				paddle2.x = 635;
		}
		if ( (controller_value & (1 << 14)) && toggle_counter == 0){
			toggle_raytracing = !toggle_raytracing;
			toggle_counter = 30;
		}

		if (toggle_counter != 0)
			toggle_counter--;

		/*ball*/
		/* Test if ball is touching a paddle */
		if (collision_counter != 0)
			collision_counter--;

		if (paddle1.x - 10 < ball.x && paddle1.x + 10 > ball.x && collision_counter == 0
				&& paddle1.y + 6 > ball.y && paddle1.y -6 < ball.y)
		{
			ball.velocity_x *= -1;
			collision_counter = 4;
		}
		if (paddle1.x - 10 < ball.x && paddle1.x + 10 > ball.x && collision_counter == 0
				&& paddle1.y + 18 > ball.y && paddle1.y + 7 < ball.y)
		{
			ball.velocity_x = ball.velocity_x*-1 + 1;
			ball.velocity_y = abs(ball.velocity_y);
			collision_counter = 4;
		}
		if (paddle1.x - 10 < ball.x && paddle1.x + 10 > ball.x && collision_counter == 0
				&& paddle1.y -7 > ball.y && paddle1.y -18 < ball.y)
		{
			ball.velocity_x = ball.velocity_x*-1 + 1;
			ball.velocity_y = -abs(ball.velocity_y);
			collision_counter = 4;
		}
		if (paddle2.x + 10 > ball.x && paddle2.x - 10 < ball.x && collision_counter == 0
				&& paddle2.y + 6 > ball.y && paddle2.y -6 < ball.y)
		{
			ball.velocity_x *= -1;
			collision_counter = 4;
		}
		if (paddle2.x + 10 > ball.x && paddle2.x - 10 < ball.x && collision_counter == 0
				&& paddle2.y + 18 > ball.y && paddle2.y + 7 < ball.y)
		{
			ball.velocity_x = ball.velocity_x*-1 - 1;
			ball.velocity_y = abs(ball.velocity_y);
			collision_counter = 4;
		}
		if (paddle2.x + 10 > ball.x && paddle2.x - 10 < ball.x && collision_counter == 0
				&& paddle2.y -7 > ball.y && paddle2.y -18 < ball.y)
		{
			ball.velocity_x = ball.velocity_x*-1 - 1;
			ball.velocity_y = -abs(ball.velocity_y);
			collision_counter = 4;
		}

		/*Test if ball is touching top/bottom wall*/
		if (ball.y <= 15 || ball.y >= 465)
		{
			ball.velocity_y *= -1;
		}
		/* Test if ball is going to score */
		if (ball.x < 5)
		{
			ball.x = 320;
			ball.velocity_x = -ball_speed;
			ball.velocity_y = ball_speed;
			p2_score+=1;
		}
		if (ball.x > 635)
		{
			ball.x = 320;
			ball.velocity_x = ball_speed;
			ball.velocity_y = ball_speed;
			p1_score+=1;
		}

		/* Update ball location */
		ball.x = ball.x + ball_speed*ball.velocity_x;
		ball.y = ball.y + ball_speed*ball.velocity_y;

		/* Determine next 2 ball locations */
		struct Ball ball_prime = find_end_point(ball, paddle1, paddle2);
		struct Ball ball_doubleprime = find_end_point(ball_prime, paddle1, paddle2);
		struct Ball ball_tprime = find_end_point(ball_doubleprime, paddle1, paddle2);

		/*Draw Everything*/
		draw_rectangle(0, 0, 640-1, 480-1, 0x00);
		draw_field();

		//TODO: Draw Scores

		draw_paddle(paddle1.x, paddle1.y);
		draw_paddle(paddle2.x, paddle2.y);
		draw_ball(ball.x, ball.y);

		/* Draw markers to determine where ball is going */
		if (toggle_raytracing){
			draw_line(ball.x, ball.y, ball_prime.x, ball_prime.y, 0xE0);
			draw_line(ball_prime.x, ball_prime.y, ball_doubleprime.x, ball_doubleprime.y, 0x1A);
			draw_line(ball_doubleprime.x, ball_doubleprime.y, ball_tprime.x, ball_tprime.y, 0xFA);
		}

		ALT_CI_CI_FRAME_DONE_0;

	}

	return 0;
}
