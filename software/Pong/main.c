/*
 * The game of Pong using Genesis Controllers and Hardware Drawing
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>
#include <graphics_commands.h>
#include <palettes.h>

#include <efsl/efs.h>
#include <efsl/ls.h>

#include "pong_graphics.h"

#define PALETTE_SIZE 256

#define COLLISION_COUNT 5
#define MAX_BALL_SPEED 20//10

struct Paddle{
	int x;
	int y;
	int velocity_x;
	int velocity_y;
};

struct Ball{
	int x;
	int y;
	int velocity_x;
	int velocity_y;
};

struct Ball find_end_point(struct Ball my_ball, struct Paddle p1, struct Paddle p2){
	int x_endleft = p1.x + 50;
	int x_endright = p2.x - 50;
	int y_steps, x_steps, new_x, new_y;
	if (my_ball.velocity_y < 0)
		y_steps = (130 - my_ball.y) / my_ball.velocity_y;
	if (my_ball.velocity_y > 0)
		y_steps = (4660 - my_ball.y) / my_ball.velocity_y;
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

int flip_sign(int x)
{
	if (x > 0)
		return -1;
	else
		return 1;
}

int find_sign(int x)
{
	if (x > 0)
		return 1;
	else
		return -1;
}

struct Ball reflect_ball(struct Paddle paddle, struct Ball ball)
{
	int delta = ball.y - paddle.y;

	ball.velocity_y = delta * abs(ball.velocity_x) / 18;

	if (abs(ball.velocity_x) < MAX_BALL_SPEED)
		ball.velocity_x = (abs(ball.velocity_x) + 1) * flip_sign(ball.velocity_x);
	else
		ball.velocity_x = ( abs(ball.velocity_x) ) * flip_sign(ball.velocity_x);

	if (abs(ball.velocity_y) > abs(ball.velocity_x))
		ball.velocity_y = find_sign(ball.velocity_y) * abs(ball.velocity_x);

	return ball;
}

struct Ball reflect_puck(struct Ball puck, struct Paddle p1, struct Paddle p2 )
{
	int p1_d = (puck.x - p1.x)*(puck.x - p1.x) + (puck.y - p1.y)*(puck.y - p1.y);
	int p2_d = (puck.x - p2.x)*(puck.x - p2.x) + (puck.y - p2.y)*(puck.y - p2.y);
	int top_d = (puck.y - 100)*(puck.y - 100);
	int bottom_d = (puck.y - 4700)*(puck.y - 4700);
	//This might be a shady way of handling walls, need to account for goal corners
	int left_d = (puck.x - 100)*(puck.x - 100);
	int right_d = (puck.x - 6300)*(puck.x - 6300);
	/* Goal Corners */
	int top_left_d = (puck.x - 100)*(puck.x - 100) + (puck.y - 1600)*(puck.y - 1600);
	int bottom_left_d = (puck.x - 100)*(puck.x - 100) + (puck.y - 3200)*(puck.y - 3200);
	int top_right_d = (puck.x - 6300)*(puck.x - 6300) + (puck.y - 1600)*(puck.y - 1600);
	int bottom_right_d = (puck.x - 6300)*(puck.x - 6300) + (puck.y - 3200)*(puck.y - 3200);
	int circle_radius_squared = 270400; //(104/2*10)^2

	if (p1_d < circle_radius_squared){
		/* TODO: Calculate new velocity for puck based on position and velocity of puck and p1 */
	}

	if (p2_d < circle_radius_squared){
		/* TODO: Calculate new velocity for puck based on position and velocity of puck and p2 */
	}

	if (top_d < circle_radius_squared || bottom_d < circle_radius_squared){
		puck.velocity_y *=-1;
	}

	if ( top_right_d < circle_radius_squared || bottom_right_d < circle_radius_squared
			|| top_left_d < circle_radius_squared || bottom_left_d < circle_radius_squared){
		puck.velocity_y *= -1;
		puck.velocity_x *= -1;
	}

	else if ( left_d < circle_radius_squared || right_d < circle_radius_squared)
		puck.velocity_x *= -1;
	else{}

	return puck;
}

int main()
{
	int ball_speed = 4;
	int toggle_raytracing = 0;
	int toggle_counter = 0;
	int end_game_bool = 0;
	struct Paddle paddle1 = {.x = 200, .y = 2400, .velocity_x = 40, .velocity_y = 40};
	struct Paddle paddle2 = {.x = 6200, .y = 2400, .velocity_x = 40, .velocity_y = 40};
	struct Ball ball = {.y = 2400, .x = 3200, .velocity_x = ball_speed, .velocity_y = ball_speed};
	//TODO: Replace paddle1_speed/paddle2_speed with structure velocity references
	int paddle1_speed = 40;
	int paddle2_speed = 40;
	int paddle_counter = 0;
	int p1_score = 0;
	int p2_score = 0;
	int collision_counter = 0;
	int game_mode_bool = 0;
	int trump_counter = 0;
	unsigned int controller_value;
	unsigned int row, col;
	unsigned int i = 0;
	unsigned int j = 0;
	char error1[13];
	char error2[13];
	pixbuf_t *pixbuf_background;
	pixbuf_t *pixbuf;
	pixbuf_t *bmp_foreground;
	pixbuf_t *composited_pixbuf;

	/* Read BMP asset from SDcard */
	char error;
	EmbeddedFileSystem efsl;

	graphics_init();
	//clear_screen();
	printf("Attempting to init filesystem");
	int ret = efs_init(&efsl, SPI_0_NAME);
	// Initialize efsl
	if(ret != 0)
	{
		printf("...could not initialize filesystem.\n");
		return(1);
	}
	else
		printf("...success!\n");

	pixbuf_t bmp_spritesheet;
	error = load_file(&efsl, "small.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE, 512);
	error = load_bmp(&efsl, "small.bmp", &bmp_spritesheet);

	printf("Copying image buffer to output buffer\n");
	pixbuf_t sdram_buf = {
			.base_address = graphics_sdram_backbuffer,
			.width = 640,
			.height = 480
	};


	//graphics_init();

	pixbuf_background = graphics_layer_get(graphics_layer_add(error1), error2);
	pixbuf = graphics_layer_get(graphics_layer_add(error1), error2);
	bmp_foreground = graphics_layer_get(graphics_layer_add(error1), error2);
	composited_pixbuf = graphics_get_final_buffer();
	/* Write grass image to background */
	draw_grass(&bmp_spritesheet, pixbuf_background);

	//alt_putstr("Restoring default palette\n");
	//switch_palette(&palette_332);

	graphics_clear_screen();

	while(1){
		/* Pong Mode */
		if (game_mode_bool == 0)
		{
			/* Read From Controllers*/
			controller_value = IORD_32DIRECT(GENESIS_0_BASE, 0);
			/*player 1*/
			/* Move Vertically */
			if (controller_value & (1 << 0)){
				if (paddle1.y > 290){
					paddle1.y -= paddle1_speed;
				}
				else
					paddle1.y = 290;
			}
			if (controller_value & (1 << 1)){
				if (paddle1.y < 4500){
					paddle1.y += paddle1_speed;
				}
				else
					paddle1.y = 4500;
			}
			/* Move Horizontally */
			if ( (controller_value & (1 << 2)) && (controller_value & (1 << 5)) ){
				if (paddle1.x > 50){
					paddle1.x -= paddle1_speed;
				}
				else
					paddle1.x = 50;
			}
			if ( (controller_value & (1 << 3)) && (controller_value & (1 << 5)) ){
				if (paddle1.x < 3200){
					paddle1.x += paddle1_speed;
				}
				else
					paddle1.x = 3200;
			}

			/* Slow other player down */
			if ( !(controller_value & (1 << 4)) && (controller_value & (1 << 5))
					&& (controller_value & (1 << 6)) && paddle_counter == 0){
				if (paddle2_speed > 4)
				{
					paddle_counter = 50;
					paddle2_speed--;
				}
			}

			/*Toggle Raytracing*/
			if ( (controller_value & (1 << 4)) && !(controller_value & (1 << 5)) && toggle_counter == 0){
				toggle_raytracing = !toggle_raytracing;
				toggle_counter = 30;
			}

			/* Active Trump Tower */
			if ((controller_value & (1 << 4)) && (controller_value & (1 << 5))
					&& (controller_value & (1 << 6)) && toggle_counter == 0 &&
					!(2800 < ball.x && 3600 > ball.x))
			{
				trump_counter = 500;
				draw_trump(&bmp_spritesheet, pixbuf_background, 1);
			}

			/*player 2*/
			/* Move Vertically */
			if (controller_value & (1 << 10)){
				if (paddle2.y > 290){
					paddle2.y -= paddle2_speed;
				}
				else
					paddle2.y = 290;
			}
			if (controller_value & (1 << 11)){
				if (paddle2.y < 4500){
					paddle2.y += paddle2_speed;
				}
				else
					paddle2.y = 4500;
			}
			/* Move Horizontally */
			if ( (controller_value & (1 << 12)) && (controller_value & (1 << 15)) ){
				if (paddle2.x > 3200){
					paddle2.x -= paddle2_speed;
				}
				else if (paddle2.x < 3200)
					paddle2.x = 3200;
			}
			if ( (controller_value & (1 << 13)) && (controller_value & (1 << 15)) ){
				if (paddle2.x < 6350){
					paddle2.x += paddle2_speed;
				}
				else
					paddle2.x = 6350;
			}
			/* Slow other player down */
			if ( !(controller_value & (1 << 14)) && (controller_value & (1 << 15))
					&& (controller_value & (1 << 16)) && paddle_counter == 0){
				if (paddle1_speed > 4)
				{
					paddle_counter = 50;
					paddle1_speed--;
				}
			}
			/* Toggle Raytracing */
			if ( (controller_value & (1 << 14)) && !(controller_value & (1 << 15)) && toggle_counter == 0){
				toggle_raytracing = !toggle_raytracing;
				toggle_counter = 30;
			}
			/* Toggle Game Mode */
			if ( (controller_value & (1 << 17)) || (controller_value & (1 << 7))){
				//TODO: Debounce that button press...
				game_mode_bool = 1;
			}

			/* Active Trump Tower */
			if ((controller_value & (1 << 14)) && (controller_value & (1 << 15))
					&& (controller_value & (1 << 16)) && toggle_counter == 0 &&
					!(2800 < ball.x && 3600 > ball.x))
			{
				trump_counter = 500;
				draw_trump(&bmp_spritesheet, pixbuf_background, 2);
			}

			/*ball*/
			/* Test if ball is touching a paddle */
			if (collision_counter != 0)
				collision_counter--;

			if (paddle1.x - 100 < ball.x && paddle1.x + 100 > ball.x && collision_counter == 0
					&& paddle1.y + 180 > ball.y && paddle1.y - 180 < ball.y)
			{
				ball = reflect_ball(paddle1, ball);
				/*Rocket the ball back horizontally if A&B held during collision*/
				if ( (controller_value & (1 << 4)) && (controller_value & (1 << 5)) && paddle_counter == 0){
					ball.velocity_x = find_sign(ball.velocity_x)*MAX_BALL_SPEED;
					ball.velocity_y = 0;
				}
				collision_counter = COLLISION_COUNT;
			}

			if (paddle2.x + 100 > ball.x && paddle2.x - 100 < ball.x && collision_counter == 0
					&& paddle2.y + 180 > ball.y && paddle2.y - 180 < ball.y)
			{
				ball = reflect_ball(paddle2, ball);
				/*Rocket the ball back horizontally if A&B held during collision*/
				if ( (controller_value & (1 << 14)) && (controller_value & (1 << 15)) && paddle_counter == 0){
					ball.velocity_x = find_sign(ball.velocity_x)*MAX_BALL_SPEED;
					ball.velocity_y = 0;
				}
				collision_counter = COLLISION_COUNT;
			}

			/*Test if ball is touching top/bottom wall*/
			if ( (ball.y <= 150 || ball.y >= 4650) && collision_counter == 0)
			{
				ball.velocity_y *= -1;

				/*Stop ball getting stuck in wall*/
				if (ball.y < 150)
					ball.y = 150;
				else if (ball.y > 4650)
					ball.y = 4650;

				collision_counter = COLLISION_COUNT;
			}

			/* Test if ball will hit trump's wall */
			if (trump_counter > 0)
			{
				if (ball.x > 2800 && ball.x < 3600){
					ball.velocity_x*=-1;

					if (ball.x < 3000)
						ball.x = 2750;
					if (ball.x > 3400)
						ball.x = 3650;
				}
			}

			/* Test if ball is going to score */
			if (ball.x < 50)
			{
				ball.x = 3200;
				ball.velocity_x = -ball_speed;
				ball.velocity_y = ball_speed;
				p2_score+=1;
				trump_counter = 2;
			}
			if (ball.x > 6350)
			{
				ball.x = 3200;
				ball.velocity_x = ball_speed;
				ball.velocity_y = ball_speed;
				p1_score+=1;
				trump_counter = 2;
			}

			/* Decrement Counters */
			if (toggle_counter != 0)
				toggle_counter--;
			if (paddle_counter != 0)
				paddle_counter--;
			if (trump_counter != 0)
				trump_counter--;

			/* Update ball location */
			ball.x = ball.x + ball_speed*ball.velocity_x;
			ball.y = ball.y + ball_speed*ball.velocity_y;

			/* Determine next 2 ball locations */
			struct Ball ball_prime = find_end_point(ball, paddle1, paddle2);
			struct Ball ball_doubleprime = find_end_point(ball_prime, paddle1, paddle2);
			struct Ball ball_tprime = find_end_point(ball_doubleprime, paddle1, paddle2);

			/*Draw Everything*/
			//graphics_draw_rectangle(pixbuf, 0, 0, 640-1, 480-1, 0x00);
			draw_field(pixbuf);

			if (trump_counter > 2)
				draw_wall(pixbuf);
			else if (trump_counter == 1)
				draw_grass(&bmp_spritesheet, pixbuf_background);

			/* Draw markers to determine where ball is going */
			if (toggle_raytracing){
				graphics_draw_line(pixbuf, scale_input(ball.x), scale_input(ball.y),
						scale_input(ball_prime.x), scale_input(ball_prime.y), 79);
				graphics_draw_line(pixbuf, scale_input(ball_prime.x), scale_input(ball_prime.y),
						scale_input(ball_doubleprime.x), scale_input(ball_doubleprime.y), 103);
				graphics_draw_line(pixbuf, scale_input(ball_doubleprime.x), scale_input(ball_doubleprime.y),
						scale_input(ball_tprime.x), scale_input(ball_tprime.y), 163);
			}

			draw_paddle(pixbuf, paddle1.x, paddle1.y);
			draw_paddle(pixbuf, paddle2.x, paddle2.y);
			draw_ball(pixbuf, ball.x, ball.y, 0x00);

			/* Scores */
			draw_int(pixbuf, 200,40, p1_score, 0x00);
			draw_int(pixbuf, 440,40, p2_score, 0x00);

			if (p1_score == 10 || p2_score == 10){
				end_game(pixbuf, composited_pixbuf, p1_score, p2_score);
				end_game_bool = 1;
			}

			if (end_game_bool == 1){
				ball_speed = 4;
				toggle_raytracing = 0;
				toggle_counter = 0;
				end_game_bool = 0;
				paddle1 = (struct Paddle){.y = 2400, .x = 200};
				paddle2 = (struct Paddle){.y = 2400, .x = 6200};
				ball = (struct Ball){.y = 2400, .x = 3200, .velocity_x = ball_speed, .velocity_y = ball_speed};
				paddle1_speed = 40;
				paddle2_speed = 40;
				paddle_counter = 0;
				p1_score = 0;
				p2_score = 0;
				collision_counter = 0;
			}
			/* Composite Frames */
			graphics_layer_copy(pixbuf_background, composited_pixbuf);
			graphics_layer_copy_transparent(pixbuf, composited_pixbuf, 0xFF);
			//graphics_layer_copy_transparent(bmp_foreground, composited_pixbuf, 194);

			ALT_CI_CI_FRAME_DONE_0;
			//break;
		}

		else
		{
			/* Air Hockey Mode */
			/* Read From Controllers*/
			controller_value = IORD_32DIRECT(GENESIS_0_BASE, 0);
			/*player 1*/
			/* Move Vertically */
			//Padle is 104x104 px, Rings are 150x150, puck is 104x104
			if (controller_value & (1 << 0)){
				if (paddle1.y > 100 + 104/2*10){
					paddle1.y -= paddle1_speed;
				}
				else
					paddle1.y = 100 + 104/2*10;
			}
			//TODO: Update values for air hockey
			if (controller_value & (1 << 1)){
				if (paddle1.y < 4500){
					paddle1.y += paddle1_speed;
				}
				else
					paddle1.y = 4500;
			}
			/* Move Horizontally */
			if ( (controller_value & (1 << 2)) && (controller_value & (1 << 5)) ){
				if (paddle1.x > 50){
					paddle1.x -= paddle1_speed;
				}
				else
					paddle1.x = 50;
			}
			if ( (controller_value & (1 << 3)) && (controller_value & (1 << 5)) ){
				if (paddle1.x < 6350){
					paddle1.x += paddle1_speed;
				}
				else
					paddle1.x = 6350;
			}

			draw_table(pixbuf);
			/* Composite Frames */
			graphics_layer_copy(pixbuf_background, composited_pixbuf);
			graphics_layer_copy_transparent(pixbuf, composited_pixbuf, 0xFF);
			ALT_CI_CI_FRAME_DONE_0;

		}


	}

	return 0;
}
