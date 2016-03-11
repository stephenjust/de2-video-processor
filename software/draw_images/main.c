/* This test program draws some bitmaps from an SD card.
 */

#include <io.h>
#include <system.h>
#include <altera_sd_card_avalon_interface_mod.h>
#include <sys/alt_stdio.h>
#include <string.h>

#define PALETTE_SIZE 256
#define SDRAM_VIDEO_OFFSET 0x300000

void draw_rectangle(int x1, int y1, int x2, int y2, unsigned char color)
{
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 0, SDRAM_0_BASE + SDRAM_VIDEO_OFFSET); // Frame address
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 4, x1); // X1
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 8, y1); // Y1
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 12, x2); // X2
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 16, y2); // Y2
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 20, color); // Color
	ALT_CI_CI_DRAW_RECT_0;
}

void clear_screen()
{
	draw_rectangle(0, 0, 640-1, 480-1, 0x00);
	ALT_CI_CI_FRAME_DONE_0;
}

int main()
{
	alt_up_sd_card_dev *dev;
	unsigned int row, col;
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int delay = 0;
	short int fh;
	unsigned char data;

	clear_screen();

	dev = alt_up_sd_card_open_dev(SD_CARD_0_NAME);
	if (dev == NULL) {
		alt_putstr("Failed to initialize driver.\n");
	}

	// Wait for SD card
	alt_putstr("Waiting for SD card...\n");
	while (!alt_up_sd_card_is_Present()) {
		// busy-waiting
	}

	if (!alt_up_sd_card_is_FAT16()) {
		alt_putstr("SD Card is not FAT16.\n");
	}

	// Load images into memory
	fh = alt_up_sd_card_fopen("fish.bmp", 0 /*create*/);
	if (fh == -1) {
		alt_putstr("File not found.\n");
	} else {
		alt_putstr("File exists.\n");

		char *buffer;
		short int length;
		int block_offset = 0;
		int file_len = 0;
		int pixdata_offset = 0;
		int bmp_width = 0;
		int bmp_height = 0;
		buffer = alt_up_sd_card_read_sector(fh, &length);
		pixdata_offset = *((short *) (buffer + 0x0A));
		if (*(buffer + 0x0E) != 40) {
			alt_putstr("Unsupported BMP type.\n");
		}
		bmp_width = *((int *) (buffer + 0x12));
		bmp_height = *((int *) (buffer + 0x16));
		if (*((short *) (buffer + 0x1C)) != 8) {
			alt_putstr("BMP must be 8 bits per pixel.\n");
		}

		while (block_offset + 512 < pixdata_offset) {
			buffer = alt_up_sd_card_read_sector(fh, &length);
			block_offset += 512;
		}

		for (i = 0; i < 480; i++) {
			for (j = 0; j < 640; j++) {
				if (i * 640 + j >= block_offset - pixdata_offset + 512) {
					buffer = alt_up_sd_card_read_sector(fh, &length);
					block_offset += 512;
				}
				IOWR_8DIRECT(SRAM_0_BASE, 640*i + j, *(buffer + ((i * 640 + j + pixdata_offset) % 512)));
			}
		}
		alt_up_sd_card_fclose(fh);
	}

	return 0;
}
