/*
 * graphics_defs.h
 *
 *  Created on: Mar 20, 2016
 *      Author: stephen
 */

#ifndef GRAPHICS_DEFS_H_
#define GRAPHICS_DEFS_H_

/*
 * Constant definitions
 */
#define E_SUCCESS 0
#define E_INVALID_ARG 1
#define E_IO 2
#define E_INVALID_BMP 3
#define E_NOMEM 4
#define E_ILLEGAL_OP 5

#define PALETTE_SIZE 256
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480

/*
 * Data structure definitions
 */
typedef struct pixbuf_t
{
	void *base_address;
	unsigned short width;
	unsigned short height;
} pixbuf_t;

typedef struct point_t
{
	short x;
	short y;
} point_t;

typedef struct rect_t
{
	point_t p1;
	point_t p2;
} rect_t;

typedef struct palette_t
{
	unsigned short colors[PALETTE_SIZE];
} palette_t;


#endif /* GRAPHICS_DEFS_H_ */
