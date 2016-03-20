/* This test program draws some bitmaps from an SD card.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>

#include <sdcard_ops.h>
#include <graphics_commands.h>

#include <efsl/efs.h>
#include <efsl/ls.h>

#include <altera_avalon_sgdma.h>
#include <altera_avalon_sgdma_descriptor.h>
#include <altera_avalon_sgdma_regs.h>

int min(int a, int b) {
	if (a < b) return a;
	else return b;
}

int main()
{
	char error;
	EmbeddedFileSystem efsl;

	graphics_init();
	clear_screen();

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
	error = load_file(&efsl, "trump.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE, 512);
	error = load_bmp(&efsl, "trump.bmp", &test_img_pixbuf);

	printf("Copying image buffer to output buffer\n");
	pixbuf_t sdram_buf = {
			.base_address = graphics_sdram_backbuffer,
			.width = 640,
			.height = 480
	};

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
	point_t dest_offset_1 = {
			.x = 10,
			.y = 10
	};
	point_t dest_offset_2 = {
			.x = 80,
			.y = 200
	};

	copy_buffer_area(&test_img_pixbuf, &sdram_buf, &source_rect, &dest_offset_1);
	copy_buffer_area_transparent(&test_img_pixbuf, &sdram_buf, &source_rect, &dest_offset_2, 223);

	ALT_CI_CI_FRAME_DONE_0;
	printf("Done!\n");

	while (1) {};

	return 0;
}
