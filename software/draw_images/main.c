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
#define SDRAM_IMAGE_OFFSET 0x500000
#define CHUNK_SIZE 2048

// BMP file offsets
#define PIXDATA_OFFSET 0x0A
#define BMP_TYPE_OFFSET 0x0E
#define WIDTH_OFFSET 0x12
#define HEIGHT_OFFSET 0x16
#define BITS_OFFSET 0x1C

typedef struct pixbuf_t {
	void *address;
	unsigned short width;
	unsigned short height;
} pixbuf_t;

typedef struct point_t {
	unsigned short x;
	unsigned short y;
} point_t;

void *pixbuf_point_address(pixbuf_t *buf, point_t *point)
{
	return buf->address + point->x + point->y * buf->width;
}

void copy_window(pixbuf_t *source_buf, pixbuf_t *dest_buf, point_t *src_p1, point_t *src_p2, point_t *dest_p1)
{
	alt_sgdma_dev *dev;
	alt_sgdma_descriptor descriptor, next_descriptor;
	short int i;
	short int num_rows = src_p2->y - src_p1->y + 1;
	short int num_cols = src_p2->x - src_p1->x + 1;
	point_t next_src_point, next_dest_point;

	dev = alt_avalon_sgdma_open(SGDMA_0_NAME);

	// Construct standard copy descriptors
	alt_avalon_sgdma_construct_mem_to_mem_desc(
			&descriptor,
			&next_descriptor,
			(alt_u32 *) 0,
			(alt_u32 *) 0,
			num_cols,
			0,
			0);
	next_src_point.x = src_p1->x;
	next_src_point.y = src_p1->y;
	next_dest_point.x = dest_p1->x;
	next_dest_point.y = dest_p1->y;

	for (i = 0; i < num_rows; i++) {
		descriptor.read_addr = pixbuf_point_address(source_buf, &next_src_point);
		descriptor.write_addr = pixbuf_point_address(dest_buf, &next_dest_point);
		alt_avalon_sgdma_do_sync_transfer(dev, &descriptor);
		next_src_point.y++;
		next_dest_point.y++;
	}
}

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

	copy_file(&efsl, "fish.pal", (void *) COLOUR_PALETTE_SHIFTER_0_BASE);
	copy_bmp(&efsl, "fish.bmp", (void *) SDRAM_0_BASE + SDRAM_IMAGE_OFFSET);

	printf("Copying image buffer to output buffer\n");
//	pixbuf_t source_buf = {
//			.address = SDRAM_0_BASE + SDRAM_IMAGE_OFFSET,
//			.width = 64,
//			.height = 64
//	};
//	pixbuf_t dest_buf = {
//			.address = SDRAM_0_BASE + SDRAM_VIDEO_OFFSET,
//			.width = 640,
//			.height = 480
//	};
//	point_t src_p1 = {
//			.x = 0,
//			.y = 0
//	};
//	point_t src_p2 = {
//			.x = 63,
//			.y = 63
//	};
//	point_t dest_p1 = {
//			.x = 0,
//			.y = 0
//	};
//
//	copy_window(&source_buf, &dest_buf, &src_p1, &src_p2, &dest_p1);
//	CONSTANT REG_SRC_BUF_ADDR  : std_logic_vector(3 downto 0) := B"0000";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 0, SDRAM_0_BASE + SDRAM_IMAGE_OFFSET);
//	CONSTANT REG_SRC_W         : std_logic_vector(3 downto 0) := B"0001";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 4, 640);
//	CONSTANT REG_SRC_H         : std_logic_vector(3 downto 0) := B"0010";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 8, 480);
//	CONSTANT REG_DEST_BUF_ADDR : std_logic_vector(3 downto 0) := B"0011";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 12, SDRAM_0_BASE + SDRAM_VIDEO_OFFSET);
//	CONSTANT REG_DEST_W        : std_logic_vector(3 downto 0) := B"0100";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 16, 640);
//	CONSTANT REG_DEST_H        : std_logic_vector(3 downto 0) := B"0101";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 20, 480);
//	CONSTANT REG_SRC_RECT_X1   : std_logic_vector(3 downto 0) := B"0110";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 24, 20);
//	CONSTANT REG_SRC_RECT_Y1   : std_logic_vector(3 downto 0) := B"0111";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 28, 10);
//	CONSTANT REG_SRC_RECT_X2   : std_logic_vector(3 downto 0) := B"1000";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 32, 639);
//	CONSTANT REG_SRC_RECT_Y2   : std_logic_vector(3 downto 0) := B"1001";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 36, 479);
//	CONSTANT REG_DEST_RECT_X1  : std_logic_vector(3 downto 0) := B"1010";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 40, 20);
//	CONSTANT REG_DEST_RECT_Y1  : std_logic_vector(3 downto 0) := B"1011";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 44, 10);
//	CONSTANT REG_TRANS_ENABLE  : std_logic_vector(3 downto 0) := B"1100";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 48, 0);
//	CONSTANT REG_TRANS_COLOR   : std_logic_vector(3 downto 0) := B"1101";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 52, 0);
	ALT_CI_CI_COPY_RECT_0;

	ALT_CI_CI_FRAME_DONE_0;
	printf("Done!\n");

	while (1) {};

	return 0;
}
