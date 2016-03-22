/*
 * Helper methods to return handy struct for reading genesis
 * controller states.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>

#define MAX_BALL_SPEED 20

typedef struct Paddle{
	int x;
	int y;
	int velocity_x;
	int velocity_y;
} Paddle;

typedef struct Ball{
	int x;
	int y;
	int velocity_x;
	int velocity_y;
}Ball;

Ball find_end_point(struct Ball my_ball, struct Paddle p1, struct Paddle p2){
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
		Ball ball = {.y = new_y, .x = new_x,
				.velocity_x = my_ball.velocity_x, .velocity_y = my_ball.velocity_y*-1};
		return ball;
	}
	else {
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
