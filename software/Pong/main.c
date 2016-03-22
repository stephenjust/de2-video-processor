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
#include "genesis.h"
#include "pong_helpers.h"

#define PALETTE_SIZE 256

#define COLLISION_COUNT 5

int main()
{
	int ball_speed = 4;
	int toggle_raytracing = 0;
	int toggle_counter = 0;
	int end_game_bool = 0;
	Paddle paddle1 = {.x = 200, .y = 2400, .velocity_x = 40, .velocity_y = 40};
	Paddle paddle2 = {.x = 6200, .y = 2400, .velocity_x = 40, .velocity_y = 40};
	Ball ball = {.y = 2400, .x = 3200, .velocity_x = ball_speed, .velocity_y = ball_speed};
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
	//pixbuf_t *bmp_foreground;
	pixbuf_t *composited_pixbuf;
	Controller player1;
	Controller player2;

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

	pixbuf_background = graphics_layer_get(graphics_layer_add(error1), error2);
	pixbuf = graphics_layer_get(graphics_layer_add(error1), error2);
	//bmp_foreground = graphics_layer_get(graphics_layer_add(error1), error2);
	composited_pixbuf = graphics_get_final_buffer();
	draw_grass(&bmp_spritesheet, pixbuf_background, p1_score, p2_score);

	graphics_clear_screen();

	while(1){
		/* Pong Mode */
		if (game_mode_bool == 0)
		{
			/* Read From Controllers*/
			player1 = get_player1();
			player2 = get_player2();
			/*player 1*/
			/* Move Vertically */
			if (player1.up){
				if (paddle1.y > 290){
					paddle1.y -= paddle1_speed;
				}
				else
					paddle1.y = 290;
			}
			if (player1.down){
				if (paddle1.y < 4500){
					paddle1.y += paddle1_speed;
				}
				else
					paddle1.y = 4500;
			}
			/* Move Horizontally */
			if ( player1.left && player1.b ){
				if (paddle1.x > 50){
					paddle1.x -= paddle1_speed;
				}
				else
					paddle1.x = 50;
			}
			if ( player1.right && player1.b ){
				if (paddle1.x < 3200){
					paddle1.x += paddle1_speed;
				}
				else
					paddle1.x = 3200;
			}

			/* Slow other player down */
			if ( !player1.a && player1.b && player1.c && paddle_counter == 0){
				if (paddle2_speed > 4)
				{
					paddle_counter = 50;
					paddle2_speed--;
				}
			}

			/*Toggle Raytracing*/
			if ( player1.a && !player1.b && toggle_counter == 0){
				toggle_raytracing = !toggle_raytracing;
				toggle_counter = 10;
			}

			/* Active Trump Tower */
			if (player1.a && player1.b && player1.c && toggle_counter == 0
					&& !(2800 < ball.x && 3600 > ball.x) )
			{
				trump_counter = 500;
				draw_trump(&bmp_spritesheet, pixbuf_background, 1);
			}

			/*player 2*/
			/* Move Vertically */
			if (player2.up){
				if (paddle2.y > 290){
					paddle2.y -= paddle2_speed;
				}
				else
					paddle2.y = 290;
			}
			if (player2.down){
				if (paddle2.y < 4500){
					paddle2.y += paddle2_speed;
				}
				else
					paddle2.y = 4500;
			}
			/* Move Horizontally */
			if ( player2.left && player2.b ){
				if (paddle2.x > 3200){
					paddle2.x -= paddle2_speed;
				}
				else if (paddle2.x < 3200)
					paddle2.x = 3200;
			}
			if ( player2.right && player2.b ){
				if (paddle2.x < 6350){
					paddle2.x += paddle2_speed;
				}
				else
					paddle2.x = 6350;
			}
			/* Slow other player down */
			if ( !player2.a && player2.b && player2.c && paddle_counter == 0){
				if (paddle1_speed > 4)
				{
					paddle_counter = 50;
					paddle1_speed--;
				}
			}
			/* Toggle Raytracing */
			if ( player2.a && !player2.b && toggle_counter == 0){
				toggle_raytracing = !toggle_raytracing;
				toggle_counter = 10;
			}
			/* Toggle Game Mode */
			if ( player1.start || player2.start){
				//TODO: Debounce that button press...
				game_mode_bool = 1;
			}

			/* Active Trump Tower */
			if (player2.a && player2.b && player2.c && toggle_counter == 0
					&& !(2800 < ball.x && 3600 > ball.x))
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
				if ( player1.a && player1.b && paddle_counter == 0){
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
				if ( player2.a && player2.b && paddle_counter == 0){
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
				draw_grass(pixbuf_background, composited_pixbuf, p1_score, p2_score);
			}
			if (ball.x > 6350)
			{
				ball.x = 3200;
				ball.velocity_x = ball_speed;
				ball.velocity_y = ball_speed;
				p1_score+=1;
				trump_counter = 2;
				draw_grass(pixbuf_background, composited_pixbuf, p1_score, p2_score);
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
			Ball ball_prime = find_end_point(ball, paddle1, paddle2);
			Ball ball_doubleprime = find_end_point(ball_prime, paddle1, paddle2);
			Ball ball_tprime = find_end_point(ball_doubleprime, paddle1, paddle2);

			/*Draw Everything*/
			//draw_field(pixbuf);

			if (trump_counter > 2)
				draw_wall(pixbuf);
			else if (trump_counter == 1)
				draw_grass(&bmp_spritesheet, pixbuf_background, p1_score, p2_score);

			//TODO: Make this fix conditional for raytracing...
			graphics_draw_rectangle(pixbuf, 0, 0, 640-1, 480-1, 0xFF);

			/* Draw markers to determine where ball is going */
			if (toggle_raytracing){
				graphics_draw_line(pixbuf, scale_input(ball.x), scale_input(ball.y),
						scale_input(ball_prime.x), scale_input(ball_prime.y), 181);
				graphics_draw_line(pixbuf, scale_input(ball_prime.x), scale_input(ball_prime.y),
						scale_input(ball_doubleprime.x), scale_input(ball_doubleprime.y), 180);
				graphics_draw_line(pixbuf, scale_input(ball_doubleprime.x), scale_input(ball_doubleprime.y),
						scale_input(ball_tprime.x), scale_input(ball_tprime.y), 236);
			}


			draw_paddle(pixbuf, paddle1.x, paddle1.y);
			draw_paddle(pixbuf, paddle2.x, paddle2.y);
			draw_ball(pixbuf, ball.x, ball.y, 0x00);

			if (p1_score == 10 || p2_score == 10){
				end_game(pixbuf, composited_pixbuf, p1_score, p2_score);
				end_game_bool = 1;
			}

			if (end_game_bool == 1){
				ball_speed = 4;
				toggle_raytracing = 0;
				toggle_counter = 0;
				end_game_bool = 0;
				paddle1 = (Paddle){.y = 2400, .x = 200};
				paddle2 = (Paddle){.y = 2400, .x = 6200};
				ball = (Ball){.y = 2400, .x = 3200, .velocity_x = ball_speed, .velocity_y = ball_speed};
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
		}

		else
		{
			/* Demo Mode for primitives */

		}


	}

	return 0;
}
