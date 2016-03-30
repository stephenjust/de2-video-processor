
#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include "sys/alt_timestamp.h"
#include <string.h>

#include <graphics_commands.h>
#include <palettes.h>
#include <graphics_layers.h>


#include <efsl/efs.h>
#include <efsl/ls.h>
#include <sdcard_ops.h>


#include <altera_avalon_sgdma.h>
#include <altera_avalon_sgdma_descriptor.h>
#include <altera_avalon_sgdma_regs.h>

alt_u32 max_frame_draw_time()
{
	alt_u32 t_start, t_draw_start, t_end;
	alt_u32 max_delay = 0;
	alt_u32 min_frame_done_time = (alt_u32) -1;
	alt_u32 delay_size = 1;
	alt_u32 counter = 0;
	while (1)
	{
		alt_u32 new_delay, new_frame_done_time;
		ALT_CI_CI_FRAME_DONE_0;
		t_start = alt_timestamp();
		// Delay to fake "draw time"
		for (counter = 0; counter < delay_size; counter++)
		{
		}
		t_draw_start = alt_timestamp();
		ALT_CI_CI_FRAME_DONE_0;
		t_end = alt_timestamp();
		new_delay = t_draw_start - t_start;
		new_frame_done_time = t_end - t_draw_start;
		if (new_frame_done_time < min_frame_done_time)
		{
			min_frame_done_time = new_frame_done_time;
			max_delay = new_delay;
		}
		else if (new_frame_done_time > min_frame_done_time + 1000000)
		{
			return max_delay;
		}
		delay_size += 100;
	}
}

alt_u32 palette_switch_time(alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	switch_palette(&palette_332);
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

alt_u32 draw_rectangle_time(alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	graphics_draw_rectangle(graphics_get_final_buffer(), 0, 0, 640-1, 480-1, 4);
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

alt_u32 draw_circle_outline(alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	graphics_draw_circle(graphics_get_final_buffer(), 640/2, 480/2, 239, 3, 0);
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

alt_u32 draw_circle_filled(alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	graphics_draw_circle(graphics_get_final_buffer(), 640/2, 480/2, 400, 3, 1);
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

alt_u32 draw_line_horizontal(alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	graphics_draw_line(graphics_get_final_buffer(), 0, 0, 640-1, 0, 1);
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

alt_u32 draw_line_diagonal(alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	graphics_draw_line(graphics_get_final_buffer(), 0, 0, 640-1, 480-1, 1);
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

alt_u32 draw_font(int fontsize, alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	print2screen(graphics_get_final_buffer(), 20, 20, 6, fontsize, "Hello, World!");
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

alt_u32 draw_triangles(alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	graphics_draw_triangle(graphics_get_final_buffer(), 0, 0, 0, 480-1, 640-1, 480-1, 1, 10);
	graphics_draw_triangle(graphics_get_final_buffer(), 0, 0, 640-1, 0, 640-1, 480-1, 1, 11);
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

alt_u32 layer_copy_time(pixbuf_t *source_layer, alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	graphics_layer_copy(source_layer, graphics_get_final_buffer());
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

alt_u32 layer_copy_transparent_time(pixbuf_t *source_layer, alt_u32 timer_overhead)
{
	alt_u32 time_start, time_end;
	time_start = alt_timestamp();
	graphics_layer_copy_transparent(source_layer, graphics_get_final_buffer(), 0);
	time_end = alt_timestamp();
	return time_end - time_start - timer_overhead;
}

int main()
{

	alt_u32 timer_overhead;
	alt_u32 time_end;
	alt_u32 time_start;
	alt_u32 time_taken;


	if(alt_timestamp_start() < 0){
		alt_putstr("Timer init failed \n");
		return 1;
	}

	//http://cseweb.ucsd.edu/~tweng/cse143/part3/template.txt
    // Get the number of clocks it takes + record time stamp:
	time_start = alt_timestamp();
	time_end = alt_timestamp();
	timer_overhead = time_end - time_start;

	printf("Timer overhead [ticks]: %d \n", timer_overhead);
	printf("Number of ticks per second: %d \n", alt_timestamp_freq());
	printf("-----\n");

	graphics_init();
	graphics_clear_screen();

	// Get the maximum frame draw time
	// This value should be interpreted as the maximum number of ticks that
	// we can draw something, and maintain a 60Hz frame-rate.
	printf("Max frame draw period [ticks]: %u\n", max_frame_draw_time());
	printf("Max frame draw period [ticks]: %u\n", max_frame_draw_time());
	printf("Max frame draw period [ticks]: %u\n", max_frame_draw_time());
	printf("Max frame draw period [ticks]: %u\n", max_frame_draw_time());
	printf("Max frame draw period [ticks]: %u\n", max_frame_draw_time());
	printf("-----\n");

	// Get the amount of time to switch a palette
	printf("Time taken to switch palettes [ticks]: %u\n", palette_switch_time(timer_overhead));
	printf("Time taken to switch palettes [ticks]: %u\n", palette_switch_time(timer_overhead));
	printf("Time taken to switch palettes [ticks]: %u\n", palette_switch_time(timer_overhead));
	printf("Time taken to switch palettes [ticks]: %u\n", palette_switch_time(timer_overhead));
	printf("Time taken to switch palettes [ticks]: %u\n", palette_switch_time(timer_overhead));
	printf("-----\n");

	switch_palette(&palette_ega);

	/* Draw Rectangle, full screen */
	printf("Time taken to draw rectangle [ticks]: %u \n", draw_rectangle_time(timer_overhead));
	printf("Time taken to draw rectangle [ticks]: %u \n", draw_rectangle_time(timer_overhead));
	printf("Time taken to draw rectangle [ticks]: %u \n", draw_rectangle_time(timer_overhead));
	printf("Time taken to draw rectangle [ticks]: %u \n", draw_rectangle_time(timer_overhead));
	printf("Time taken to draw rectangle [ticks]: %u \n", draw_rectangle_time(timer_overhead));
	printf("-----\n");

	/* Draw a circle from the centre of the screen with a large radius.
	 * This should be the worst case scenario because the outside must
	 * have lots of pixels to plot. (Large circumference, exit the RUNNING
	 * state quite late.
	 */
	printf("Time taken to draw an unfilled circle [ticks]: %u \n", draw_circle_outline(timer_overhead));
	printf("Time taken to draw an unfilled circle [ticks]: %u \n", draw_circle_outline(timer_overhead));
	printf("Time taken to draw an unfilled circle [ticks]: %u \n", draw_circle_outline(timer_overhead));
	printf("Time taken to draw an unfilled circle [ticks]: %u \n", draw_circle_outline(timer_overhead));
	printf("Time taken to draw an unfilled circle [ticks]: %u \n", draw_circle_outline(timer_overhead));
	printf("-----\n");


	/* Filled Circle in Software, to cover the whole frame */
	printf("Time taken to draw a filled circle [ticks]: %u \n", draw_circle_filled(timer_overhead));
	printf("Time taken to draw a filled circle [ticks]: %u \n", draw_circle_filled(timer_overhead));
	printf("Time taken to draw a filled circle [ticks]: %u \n", draw_circle_filled(timer_overhead));
	printf("Time taken to draw a filled circle [ticks]: %u \n", draw_circle_filled(timer_overhead));
	printf("Time taken to draw a filled circle [ticks]: %u \n", draw_circle_filled(timer_overhead));
	printf("-----\n");

	/* Horizontal line, corner to corner */
	printf("Time taken to draw a horizontal line [ticks]: %u \n", draw_line_horizontal(timer_overhead));
	printf("Time taken to draw a horizontal line [ticks]: %u \n", draw_line_horizontal(timer_overhead));
	printf("Time taken to draw a horizontal line [ticks]: %u \n", draw_line_horizontal(timer_overhead));
	printf("Time taken to draw a horizontal line [ticks]: %u \n", draw_line_horizontal(timer_overhead));
	printf("Time taken to draw a horizontal line [ticks]: %u \n", draw_line_horizontal(timer_overhead));
	printf("-----\n");

	/* Line diagonally across */
	printf("Time taken to draw a line from corner to corner [ticks]: %u \n", draw_line_diagonal(timer_overhead));
	printf("Time taken to draw a line from corner to corner [ticks]: %u \n", draw_line_diagonal(timer_overhead));
	printf("Time taken to draw a line from corner to corner [ticks]: %u \n", draw_line_diagonal(timer_overhead));
	printf("Time taken to draw a line from corner to corner [ticks]: %u \n", draw_line_diagonal(timer_overhead));
	printf("Time taken to draw a line from corner to corner [ticks]: %u \n", draw_line_diagonal(timer_overhead));
	printf("-----\n");

	/* Test fonts at each font size */
	int fontsize;
	for (fontsize = 0; fontsize < 10; ++fontsize)
	{
		printf("Time taken to draw font of size %d [ticks]: %u \n", fontsize, draw_font(fontsize, timer_overhead));
		printf("Time taken to draw font of size %d [ticks]: %u \n", fontsize, draw_font(fontsize, timer_overhead));
		printf("Time taken to draw font of size %d [ticks]: %u \n", fontsize, draw_font(fontsize, timer_overhead));
		printf("Time taken to draw font of size %d [ticks]: %u \n", fontsize, draw_font(fontsize, timer_overhead));
		printf("Time taken to draw font of size %d [ticks]: %u \n", fontsize, draw_font(fontsize, timer_overhead));
		printf("-----\n");
	}

	/* Draw time of a filled rectangle vs draw time of a filled two triangles. */
	printf("Time taken to draw two triangles that fill the screen [ticks]: %u \n", draw_triangles(timer_overhead));
	printf("Time taken to draw two triangles that fill the screen [ticks]: %u \n", draw_triangles(timer_overhead));
	printf("Time taken to draw two triangles that fill the screen [ticks]: %u \n", draw_triangles(timer_overhead));
	printf("Time taken to draw two triangles that fill the screen [ticks]: %u \n", draw_triangles(timer_overhead));
	printf("Time taken to draw two triangles that fill the screen [ticks]: %u \n", draw_triangles(timer_overhead));
	printf("-----\n");

	/* Time to copy a layer into the composited buffer */
	char error;
	unsigned short layer_id = graphics_layer_add(&error);
	pixbuf_t *layer = graphics_layer_get(layer_id, &error);
	printf("Time taken to perform a layer copy [ticks]: %u \n", layer_copy_time(layer, timer_overhead));
	printf("Time taken to perform a layer copy [ticks]: %u \n", layer_copy_time(layer, timer_overhead));
	printf("Time taken to perform a layer copy [ticks]: %u \n", layer_copy_time(layer, timer_overhead));
	printf("Time taken to perform a layer copy [ticks]: %u \n", layer_copy_time(layer, timer_overhead));
	printf("Time taken to perform a layer copy [ticks]: %u \n", layer_copy_time(layer, timer_overhead));
	printf("-----\n");

	/* Time to copy a layer with transparency */
	printf("Time taken to perform a layer copy with transparency [ticks]: %u \n", layer_copy_transparent_time(layer, timer_overhead));
	printf("Time taken to perform a layer copy with transparency [ticks]: %u \n", layer_copy_transparent_time(layer, timer_overhead));
	printf("Time taken to perform a layer copy with transparency [ticks]: %u \n", layer_copy_transparent_time(layer, timer_overhead));
	printf("Time taken to perform a layer copy with transparency [ticks]: %u \n", layer_copy_transparent_time(layer, timer_overhead));
	printf("Time taken to perform a layer copy with transparency [ticks]: %u \n", layer_copy_transparent_time(layer, timer_overhead));
	printf("-----\n");

	return 0;
}
