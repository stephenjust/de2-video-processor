#ifndef __GRAPHICSCMDS_H_
#define __GRAPHICSCMDS_H_

#include "graphics_defs.h"

extern void *graphics_sdram_backbuffer;

char graphics_init();

void draw_rectangle(int x1, int y1, int x2, int y2, unsigned char color);

void draw_line(int x1, int y1, int x2, int y2, unsigned char color);
void draw_circle (int cx, int cy, int radius, int color, int filled);
void draw_rounded_rect(int x1, int y1, int x2, int y2, int radius, int filled, unsigned char color);


void clear_screen();


/* Fonts */
extern char font8x8_block[][8];
extern char font8x8_basic[][8];

void draw_letter(int y1, int x1, int color, int pixel_size, char* letter);
void print2screen(int x1, int y1, int color, int pixel_size, char* string);

void copy_buffer_area(pixbuf_t *source, pixbuf_t *dest, rect_t *source_area, point_t *dest_offset);
void copy_buffer_area_transparent(pixbuf_t *source, pixbuf_t *dest, rect_t *source_area, point_t *dest_offset, unsigned char t_color);

#endif /* __GRAPHICSCMDS_H_ */
