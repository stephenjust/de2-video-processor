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

Ball find_end_point(struct Ball my_ball, struct Paddle p1, struct Paddle p2);

int flip_sign(int x);

int find_sign(int x);

Ball reflect_ball(Paddle paddle, Ball ball);
