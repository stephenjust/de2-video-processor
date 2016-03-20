/*
 * graphics_defs.h
 *
 *  Created on: Mar 20, 2016
 *      Author: stephen
 */

#ifndef GRAPHICS_DEFS_H_
#define GRAPHICS_DEFS_H_

/*
 * Data structure definitions
 */
typedef struct pixbuf_t
{
	void *base_address;
	unsigned short width;
	unsigned short height;
} pixbuf_t;


/*
 * Constant definitions
 */
#define E_SUCCESS 0
#define E_INVALID_ARG 1
#define E_IO 2
#define E_INVALID_BMP 3
#define E_NOMEM 4

#endif /* GRAPHICS_DEFS_H_ */
