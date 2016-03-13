/* This test program draws some bitmaps from an SD card.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>

#include <efsl/efs.h>
#include <efsl/ls.h>

#include <altera_avalon_sgdma.h>
#include <altera_avalon_sgdma_descriptor.h>
#include <altera_avalon_sgdma_regs.h>

#define PALETTE_SIZE 256
#define SDRAM_VIDEO_OFFSET 0x300000
#define CHUNK_SIZE 2048

// BMP file offsets
#define PIXDATA_OFFSET 0x0A
#define BMP_TYPE_OFFSET 0x0E
#define WIDTH_OFFSET 0x12
#define HEIGHT_OFFSET 0x16
#define BITS_OFFSET 0x1C

short int copy_file(EmbeddedFileSystem *efsl, char *file_name, void *dest_addr)
{
	File file;
	unsigned char buffer[CHUNK_SIZE];
	unsigned int offset;
	int bytes_read, status;
	alt_sgdma_descriptor descriptor, next_descriptor;
	alt_sgdma_dev *dev;

	printf("Copying file %s\n", file_name);

	if (file_fopen(&file, &(efsl->myFs), file_name, 'r') != 0)
		return -1;

	dev = alt_avalon_sgdma_open(SGDMA_0_NAME);
	offset = 0;
	while (offset < file.FileSize)
	{
		bytes_read = file_read(&file, CHUNK_SIZE, buffer);
		alt_avalon_sgdma_construct_mem_to_mem_desc(
				&descriptor,
				&next_descriptor,
				(alt_u32 *) buffer,
				(alt_u32 *) (dest_addr + offset),
				bytes_read,
				0,
				0);
		status = alt_avalon_sgdma_do_sync_transfer(dev, &descriptor);
		offset += bytes_read;
	}

	// Close the file
	if (file_fclose(&file) != 0)
		return -1;

	return 0;
}

short int copy_bmp(EmbeddedFileSystem *efsl, char *file_name, void *dest_addr)
{
	File file;
	unsigned char buffer[CHUNK_SIZE];
	int pixdata_offset, bmp_width, bmp_height, i, bytes_read, status;
	alt_sgdma_descriptor descriptor, next_descriptor;
	alt_sgdma_dev *dev;

	printf("Copying bitmap %s\n", file_name);

	if (file_fopen(&file, &(efsl->myFs), file_name, 'r') != 0)
		return -1;

	// Read the first block of the file to get metadata
	file_setpos(&file, 0);
	file_read(&file, CHUNK_SIZE, buffer);

	// Read bitmap metadata
	pixdata_offset = *((short *) (buffer + PIXDATA_OFFSET));
	if (*(buffer + BMP_TYPE_OFFSET) != 40) {
		alt_putstr("Unsupported BMP type.\n");
		file_fclose(&file);
		return -1;
	}
	bmp_width = *((short *) (buffer + WIDTH_OFFSET));
	bmp_height = *((short *) (buffer + HEIGHT_OFFSET));
	if (*((short *) (buffer + BITS_OFFSET)) != 8) {
		alt_putstr("BMP must be 8 bits per pixel.\n");
		file_fclose(&file);
		return -1;
	}

	if (bmp_width > CHUNK_SIZE) {
		printf("Maximum BMP width exceeded: %d > %d.\n", bmp_width, CHUNK_SIZE);
		file_fclose(&file);
		return -1;
	}

	// Advance the file pointer to the beginning of the image
	file_setpos(&file, pixdata_offset);

	dev = alt_avalon_sgdma_open(SGDMA_0_NAME);
	// Copy the bitmap, mirroring in the y-direction to match our coordinate system
	for (i = bmp_height-1; i >= 0; --i)
	{
		bytes_read = file_read(&file, bmp_width, buffer);
		alt_avalon_sgdma_construct_mem_to_mem_desc(
				&descriptor,
				&next_descriptor,
				(alt_u32 *) buffer,
				(alt_u32 *) (dest_addr + i*bmp_width),
				bytes_read,
				0,
				0);
		status = alt_avalon_sgdma_do_sync_transfer(dev, &descriptor);
	}

	// Close the file
	if (file_fclose(&file) != 0)
		return -1;

	return 0;
}


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
	EmbeddedFileSystem efsl;
	unsigned short palette_buffer[256];

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

	while (1)
	{
		copy_file(&efsl, "fish.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE);
		copy_bmp(&efsl, "fish.bmp", (void *) SRAM_0_BASE);
		copy_file(&efsl, "fish_a.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE);
		copy_bmp(&efsl, "fish_a.bmp", (void *) SRAM_0_BASE);
		copy_file(&efsl, "igloo.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE);
		copy_bmp(&efsl, "igloo.bmp", (void *) SRAM_0_BASE);
		copy_file(&efsl, "igloo_a.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE);
		copy_bmp(&efsl, "igloo_a.bmp", (void *) SRAM_0_BASE);
		copy_file(&efsl, "stove.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE);
		copy_bmp(&efsl, "stove.bmp", (void *) SRAM_0_BASE);
		copy_file(&efsl, "stove_a.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE);
		copy_bmp(&efsl, "stove_a.bmp", (void *) SRAM_0_BASE);
	}

	return 0;
}
