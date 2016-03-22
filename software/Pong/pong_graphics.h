#ifndef __PONGGFX_H_
#define __PONGGFX_H_

#include <graphics_defs.h>

int scale_input(int x);

void draw_ball(pixbuf_t *pixbuf, int x, int y, int color);

void draw_paddle(pixbuf_t *pixbuf, int x, int y);

void draw_field(pixbuf_t *pixbuf);

void draw_int(pixbuf_t *pixbuf, int x, int y, int value, int color);

void draw_wall(pixbuf_t *pixbuf);

void draw_grass(pixbuf_t *bmp_asset, pixbuf_t *sdram_buf, int p1_score, int p2_score);

void end_game(pixbuf_t *pixbuf, pixbuf_t *composited_pixbuf,int p1_score, int p2_score);

#endif /* __PONGGFX_H_ */
