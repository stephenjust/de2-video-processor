/*
 * graphics_layers.h
 *
 *  Created on: Mar 21, 2016
 *      Author: stephen
 */

#ifndef GRAPHICS_LAYERS_H_
#define GRAPHICS_LAYERS_H_

#include "graphics_defs.h"

unsigned short graphics_layer_add(char *error);
pixbuf_t *graphics_layer_get(unsigned short index, char *error);
void graphics_layer_copy(pixbuf_t *source, pixbuf_t *dest);
void graphics_layer_copy_transparent(pixbuf_t *source, pixbuf_t *dest, unsigned char t_color);

#endif /* GRAPHICS_LAYERS_H_ */
