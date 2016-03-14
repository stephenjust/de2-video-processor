#ifndef __PONGGFX_H_
#define __PONGGFX_H_

//#define SDRAM_VIDEO_OFFSET 0x300000

void draw_ball(int x, int y);

void draw_paddle(int x, int y);

void draw_field();

void draw_int(int x, int y, int scale, int value, int color);

#endif /* __PONGGFX_H_ */
