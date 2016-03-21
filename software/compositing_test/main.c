/*
 * main.c
 *
 *  Created on: Mar 21, 2016
 *      Author: stephen
 */

#include <system.h>
#include <graphics_commands.h>
#include <graphics_layers.h>
#include <palettes.h>

int main(void)
{
	int error;
	short bg_index, fg_index;
	pixbuf_t *composited_pixbuf, *bg_pixbuf, *fg_pixbuf;

	graphics_init();

	// Create layers for fg and bg
	bg_index = graphics_layer_add(&error);
	fg_index = graphics_layer_add(&error);

	// Get pixbufs
	composited_pixbuf = graphics_get_final_buffer();
	bg_pixbuf = graphics_layer_get(bg_index, &error);
	fg_pixbuf = graphics_layer_get(fg_index, &error);

	// Reset the display
	switch_palette(&palette_332);
	graphics_clear_screen();

	// Populate background layer with a red background
	graphics_draw_rectangle(bg_pixbuf, 0, 0, 639, 479, 0xE0);

	// Populate foreground layer with a white rectangle in the center
	graphics_draw_rectangle(fg_pixbuf, 0, 0, 639, 479, 0x00); // Fill with black - this will be transparent
	graphics_draw_rectangle(fg_pixbuf, 10, 10, 629, 469, 0xFF); // White rectangle

	// Composite layers
	graphics_layer_copy(bg_pixbuf, composited_pixbuf);
	graphics_layer_copy_transparent(fg_pixbuf, composited_pixbuf, 0x00);

	ALT_CI_CI_FRAME_DONE_0;
}
