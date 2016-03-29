
#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include "sys/alt_timestamp.h"
#include <string.h>

#include "graphics_commands.h"
#include "palettes.h"





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
	printf("Number of ticks per second: %d", alt_timestamp_freq());




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
	graphics_draw_rectangle(graphics_get_final_buffer(), 0, 0, 640, 480, 4);
	ALT_CI_CI_FRAME_DONE_0;
	time_end = alt_timestamp();

	time_taken = time_end - time_start - timer_overhead;
	printf("Time taken to draw rectangle [ticks]: %d \n", time_taken);

	/* Filled Circle in Hardware, negative fills */

	/* Filled Circle in Software */


	/* Line diagonally across */


	/* Line with as many pixels as a circle (for pixel comparison) */

	/* Fonts. */

	/* Font with 1px size */

	/* Font with 2px size */

	/* Font with 3px size */

	/* Draw time of a filled rectangle vs draw time of a filled two triangles. */

	/* Run CI_FRAME_DONE 100 times and find the min/max. See how long we have to wait for an SRAM window */


	/* Trippy circle loop varying size */


	return 0;
}
