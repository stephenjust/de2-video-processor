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
	File readFile;
	unsigned char buffer[CHUNK_SIZE];
	char *fileName = "fish.bmp";
	int block_offset = 0;
	int bmp_width, bmp_height, file_len, pixdata_offset, i, bytes_read;

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

	// Open the test file
	printf("\nAttempting to open file: \"%s\"\n", fileName);

	if (file_fopen(&readFile, &efsl.myFs, fileName, 'r') != 0)
	{
		printf("Error:\tCould not open file\n");
		return(1);
	}
	else
	{
		printf("Reading file...\n");
	}

	// Read the first block of the file
	file_read(&readFile, CHUNK_SIZE, buffer);

	// Bitmap metadata is in the first file chunk at fixed offsets.
	pixdata_offset = *((short *) (buffer + PIXDATA_OFFSET));
	if (*(buffer + BMP_TYPE_OFFSET) != 40) {
		alt_putstr("Unsupported BMP type.\n");
	}
	bmp_width = *((int *) (buffer + WIDTH_OFFSET));
	bmp_height = *((int *) (buffer + HEIGHT_OFFSET));
	if (*((short *) (buffer + BITS_OFFSET)) != 8) {
		alt_putstr("BMP must be 8 bits per pixel.\n");
	}

	file_setpos(&readFile, pixdata_offset);

	unsigned char status;

	alt_sgdma_descriptor descriptor;
	alt_sgdma_descriptor next_descriptor;
	alt_sgdma_dev *dev;
	dev = alt_avalon_sgdma_open(SGDMA_0_NAME);
	int target_addr = SRAM_0_BASE;

	for (i = 0; i < 640*480; i = i + CHUNK_SIZE) {
		printf("Writing at index %d\n", i);
		bytes_read = file_read(&readFile, CHUNK_SIZE, buffer);
		alt_avalon_sgdma_construct_mem_to_mem_desc(&descriptor, &next_descriptor, (alt_u32 *) buffer, (alt_u32 *) target_addr, bytes_read, 0, 0);
		status = alt_avalon_sgdma_do_sync_transfer(dev, &descriptor);
		target_addr += CHUNK_SIZE;
	}

	// Close the file
	if (file_fclose(&readFile) != 0)
	{
		printf("Error:\tCould not close file properly\n");
		return(1);
	}

	return 0;
}
