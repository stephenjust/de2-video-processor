#ifndef __PONGGFX_H_
#define __PONGGFX_H_

//#define SDRAM_VIDEO_OFFSET 0x300000

int scale_input(int x);

void draw_ball(int x, int y);

void draw_paddle(int x, int y);

void draw_field();

void draw_int(int x, int y, int scale, int value, int color);

void draw_wall();

void end_game(int p1_score, int p2_score);

#endif /* __PONGGFX_H_ */
