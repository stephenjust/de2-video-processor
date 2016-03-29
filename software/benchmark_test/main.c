
#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include "sys/alt_timestamp.h"
#include <string.h>

#include "graphics_commands.h"
#include "palettes.h"


#include <efsl/efs.h>
#include <efsl/ls.h>
#include <sdcard_ops.h>


#include <altera_avalon_sgdma.h>
#include <altera_avalon_sgdma_descriptor.h>
#include <altera_avalon_sgdma_regs.h>


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




	graphics_init();
	graphics_clear_screen();
	switch_palette(&palette_ega);

	/* Draw Rectangle, full screen */
	time_start = alt_timestamp();
	graphics_draw_rectangle(graphics_get_final_buffer(), 0, 0, 640, 480, 4);
	ALT_CI_CI_FRAME_DONE_0;
	time_end = alt_timestamp();

	time_taken = time_end - time_start - timer_overhead;
	printf("Time taken to draw rectangle [ticks]: %d \n", time_taken);


	/* Draw a circle from the centre of the screen with a large radius.
	 * This should be the worst case scenario because the outside must
	 * have lots of pixels to plot. (Large circumference, exit the RUNNING
	 * state quite late.
	 */
	time_start = alt_timestamp();
	graphics_draw_rectangle(graphics_get_final_buffer(), 0, 0, 640, 480, 15);
	ALT_CI_CI_FRAME_DONE_0;
	time_end = alt_timestamp();

	time_taken = time_end - time_start - timer_overhead;
	printf("Time taken to draw rectangle [ticks]: %d \n", time_taken);

	/* Filled Circle in Hardware, negative fills */
	time_start = alt_timestamp();
	graphics_draw_circle(graphics_get_final_buffer(), 640/2, 480/2, 239, 3, 0);
	ALT_CI_CI_FRAME_DONE_0;
	time_end = alt_timestamp();
	time_taken = time_end - time_start - timer_overhead;
	printf("Time taken to draw an unfilled circle [ticks]: %d \n", time_taken);


	/* Filled Circle in Software */
	time_start = alt_timestamp();
	graphics_draw_circle(graphics_get_final_buffer(), 640/2, 480/2, 239, 3, 1);
	ALT_CI_CI_FRAME_DONE_0;
	time_end = alt_timestamp();
	time_taken = time_end - time_start - timer_overhead;
	printf("Time taken to draw an filled (software) circle [ticks]: %d \n", time_taken);


	/* Line diagonally across */
	time_start = alt_timestamp();
	graphics_draw_line(graphics_get_final_buffer(), 0, 0, 640, 480, 5);
	ALT_CI_CI_FRAME_DONE_0;
	time_end = alt_timestamp();
	time_taken = time_end - time_start - timer_overhead;
	printf("Time taken to draw a line from corner to corner [ticks]: %d \n", time_taken);



	/* Line with as many pixels as a circle (for pixel comparison) */

	/* Fonts. */

	/* Font with 1px size */

	int rectsize = 1;

	for (rectsize; rectsize < 10; rectsize++){
		time_start = alt_timestamp();
		print2screen(graphics_get_final_buffer(), 20, 20, 6, rectsize, "Hello, World!");
		ALT_CI_CI_FRAME_DONE_0;
		time_end = alt_timestamp();
		time_taken = time_end - time_start - timer_overhead;
		printf("Time taken to draw font of size %d  [ticks]: %d \n", rectsize, time_taken);
	}



	/* Draw time of a filled rectangle vs draw time of a filled two triangles. */
	time_start = alt_timestamp();
	graphics_draw_triangle(graphics_get_final_buffer(), 0, 0, 0, 480, 640, 480, 1, 10);
	graphics_draw_triangle(graphics_get_final_buffer(), 0, 0, 640, 0, 640, 480, 1, 11);
	ALT_CI_CI_FRAME_DONE_0;
	time_end = alt_timestamp();
	time_taken = time_end - time_start - timer_overhead;
	printf("Time taken to draw two triangles that fill the screen  [ticks]: %d \n", time_taken);



	/* Run CI_FRAME_DONE 100 times and find the min/max. See how long we have to wait for an SRAM window */
	int rd = 0;
	alt_u32 mintime = 0;
	alt_u32 maxtime = 0;
	alt_u32 sumtimes = 0; //Used for finding average.

	for (rd; rd < 100; rd++){
		time_start = alt_timestamp();
		ALT_CI_CI_FRAME_DONE_0;
		time_end = alt_timestamp();
		time_taken = time_end - time_start - timer_overhead;
		if (time_taken <= mintime || mintime == 0){
			mintime = time_taken;
		}
		if (time_taken >= maxtime) {
			maxtime = time_taken;
		}
		sumtimes += time_taken;
	}

	printf("Minimum time to get an SRAM window [ticks]: %d \n", mintime);
	printf("Maximum time to get an SRAM window [ticks]: %d \n", maxtime);
	printf("Average time to get an SRAM window [ticks]: %d \n", sumtimes / rd);


	/* All of the above tests, in one frame */
	time_start = alt_timestamp();

	graphics_draw_rectangle(graphics_get_final_buffer(), 0, 0, 640, 480, 4);
	graphics_draw_circle(graphics_get_final_buffer(), 640/2, 480/2, 239, 3, 0);
	graphics_draw_circle(graphics_get_final_buffer(), 640/2, 480/2, 239, 3, 1);
	graphics_draw_line(graphics_get_final_buffer(), 0, 0, 640, 480, 5);
	for (rectsize = 1; rectsize < 10; rectsize++){
		print2screen(graphics_get_final_buffer(), 20, 20, 6, rectsize, "Hello, World!");
	}
	graphics_draw_triangle(graphics_get_final_buffer(), 0, 0, 0, 480, 640, 480, 1, 10);
	graphics_draw_triangle(graphics_get_final_buffer(), 0, 0, 640, 0, 640, 480, 1, 11);

	ALT_CI_CI_FRAME_DONE_0;
	time_end = alt_timestamp();
	time_taken = time_end - time_start - timer_overhead;
	printf("All of the above in one frame [ticks]: %d \n", time_taken);

	/* 640x480 bitmap draw from ram, same as a rect....... lame test. */

	/* 6480x480 bitmap draw from SD card */


	time_start = alt_timestamp();
	char error;
		EmbeddedFileSystem efsl;


		// Initialises the filesystem on the SD card, if the filesystem does not
		// init properly then it displays an error message.
		printf("Attempting to init filesystem");
		int ret = efs_init(&efsl, SPI_0_NAME);

		// Initialize efsl
		if(ret != 0)
		{
			printf("...could not initialize filesystem.\n");
			return(1);
		}
		else
			printf("...success!\n");

		pixbuf_t test_img_pixbuf;
		error = load_file(&efsl, "fish.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE, 512);
		error = load_bmp(&efsl, "fish.bmp", &test_img_pixbuf);

		printf("Copying image buffer to output buffer\n");
		pixbuf_t *sdram_buf = graphics_get_final_buffer();

		rect_t source_rect = {
				.p1 = {
						.x = 0,
						.y = 0
				},
				.p2 = {
						.x = test_img_pixbuf.width - 1,
						.y = test_img_pixbuf.height - 1
				}
		};
		point_t dest_offset_0 = {
				.x = 0,
				.y = 0
		};

		copy_buffer_area(&test_img_pixbuf, sdram_buf, &source_rect, &dest_offset_0);

		ALT_CI_CI_FRAME_DONE_0;

		time_end = alt_timestamp();
		time_taken = time_end - time_start - timer_overhead;
		printf("Fish drawn in  [ticks]: %d \n", time_taken);


	/* Trippy circle loop varying size */

	//graphics_draw_triangle(graphics_get_final_buffer(), 15, 112, 300, 112, 170, 240, 1, 9);
	//ALT_CI_CI_FRAME_DONE_0;


	return 0;
}
