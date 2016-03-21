/*
 * graphics_layers.c
 *
 *  Created on: Mar 21, 2016
 *      Author: stephen
 */

#include <sys/alt_cache.h>

#include "graphics_defs.h"
#include "graphics_commands.h"

#define MAX_LAYERS 3
static pixbuf_t layer_buffers[MAX_LAYERS];
static unsigned short allocated_layers = 0;

static const rect_t frame_rect = {
		.p1 = {
				.x = 0,
				.y = 0
		},
		.p2 = {
				.x = FRAME_WIDTH - 1,
				.y = FRAME_HEIGHT - 1
		}
};

static const point_t zero_point = {
		.x = 0,
		.y = 0
};

unsigned short graphics_layer_add(char *error)
{
	if (allocated_layers < MAX_LAYERS)
	{
		layer_buffers[allocated_layers].base_address = (void *) alt_uncached_malloc(FRAME_WIDTH * FRAME_HEIGHT);
		if (layer_buffers[allocated_layers].base_address == 0) {
			*error = -E_NOMEM;
			return 0;
		}
		layer_buffers[allocated_layers].width = FRAME_WIDTH;
		layer_buffers[allocated_layers].height = FRAME_HEIGHT;
		*error = E_SUCCESS;
		return allocated_layers++;
	}
	else
	{
		*error = -E_ILLEGAL_OP;
		return 0;
	}
}

pixbuf_t *graphics_layer_get(unsigned short index, char *error)
{
	if (index < allocated_layers)
	{
		*error = E_SUCCESS;
		return &layer_buffers[index];
	}
	else
	{
		*error = -E_INVALID_ARG;
		return 0;
	}
}

void graphics_layer_copy(pixbuf_t *source, pixbuf_t *dest)
{
	copy_buffer_area(source, dest, &frame_rect, &zero_point);
}

void graphics_layer_copy_transparent(pixbuf_t *source, pixbuf_t *dest, unsigned char t_color)
{
	copy_buffer_area_transparent(source, dest, &frame_rect, &zero_point, t_color);
}
