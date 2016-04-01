/*
 * Helper methods to return handy struct for reading genesis
 * controller states.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>

#include "pong_helpers.h"


Ball find_end_point(struct Ball my_ball, struct Paddle p1, struct Paddle p2){
	int x_endleft = p1.x + 5*SCALE_FACTOR;
	int x_endright = p2.x - 5*SCALE_FACTOR;
	int y_top = 13*SCALE_FACTOR;
	int y_bottom = 466*SCALE_FACTOR;
	int y_steps, x_steps, new_x, new_y;

	if (my_ball.velocity_y < 0) {
		y_steps = (y_top - my_ball.y) / my_ball.velocity_y;
	} else if (my_ball.velocity_y > 0) {
		y_steps = (y_bottom - my_ball.y) / my_ball.velocity_y;
	} else {
		y_steps = 0;
	}

	if (my_ball.velocity_x > 0)
		x_steps = (x_endright - my_ball.x) / my_ball.velocity_x;
	else if (my_ball.velocity_x < 0)
		x_steps = (x_endleft - my_ball.x) / my_ball.velocity_x;

	if (x_steps > y_steps && y_steps != 0)
	{
		new_y = y_steps*my_ball.velocity_y + my_ball.y;
		new_x = y_steps*my_ball.velocity_x + my_ball.x;
		Ball ball = {.y = new_y, .x = new_x,
				.velocity_x = my_ball.velocity_x, .velocity_y = my_ball.velocity_y*-1};
		return ball;
	}
	else
	{
		new_y = x_steps*my_ball.velocity_y + my_ball.y;
		new_x = x_steps*my_ball.velocity_x + my_ball.x;
		Ball ball = {.y = new_y, .x = new_x,
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

Ball reflect_ball(Paddle paddle, Ball ball)
{
	int delta = (ball.y - paddle.y)/16;

	ball.velocity_y = delta * abs(ball.velocity_x) / 10;//18 came from original height in pixels.

	if (abs(ball.velocity_x) < MAX_BALL_SPEED)
		ball.velocity_x = (abs(ball.velocity_x) + 1) * flip_sign(ball.velocity_x);
	else
		ball.velocity_x = ( abs(ball.velocity_x) ) * flip_sign(ball.velocity_x);

	if (abs(ball.velocity_y) > abs(ball.velocity_x))
		ball.velocity_y = find_sign(ball.velocity_y) * abs(ball.velocity_x);

	return ball;
}
