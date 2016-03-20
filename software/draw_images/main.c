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

#define PALETTE_SIZE 256
#define SDRAM_VIDEO_OFFSET 0x300000

int min(int a, int b) {
	if (a < b) return a;
	else return b;
}

int main()
{
	char error;
	EmbeddedFileSystem efsl;

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
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 0, test_img_pixbuf.base_address);
//	CONSTANT REG_SRC_W         : std_logic_vector(3 downto 0) := B"0001";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 4, test_img_pixbuf.width);
//	CONSTANT REG_SRC_H         : std_logic_vector(3 downto 0) := B"0010";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 8, test_img_pixbuf.height);
//	CONSTANT REG_DEST_BUF_ADDR : std_logic_vector(3 downto 0) := B"0011";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 12, SDRAM_0_BASE + SDRAM_VIDEO_OFFSET);
//	CONSTANT REG_DEST_W        : std_logic_vector(3 downto 0) := B"0100";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 16, 640);
//	CONSTANT REG_DEST_H        : std_logic_vector(3 downto 0) := B"0101";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 20, 480);
//	CONSTANT REG_SRC_RECT_X1   : std_logic_vector(3 downto 0) := B"0110";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 24, 0);
//	CONSTANT REG_SRC_RECT_Y1   : std_logic_vector(3 downto 0) := B"0111";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 28, 0);
//	CONSTANT REG_SRC_RECT_X2   : std_logic_vector(3 downto 0) := B"1000";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 32, 139);
//	CONSTANT REG_SRC_RECT_Y2   : std_logic_vector(3 downto 0) := B"1001";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 36, 168);
//	CONSTANT REG_DEST_RECT_X1  : std_logic_vector(3 downto 0) := B"1010";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 40, 20);
//	CONSTANT REG_DEST_RECT_Y1  : std_logic_vector(3 downto 0) := B"1011";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 44, 10);
//	CONSTANT REG_TRANS_ENABLE  : std_logic_vector(3 downto 0) := B"1100";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 48, 1);
//	CONSTANT REG_TRANS_COLOR   : std_logic_vector(3 downto 0) := B"1101";
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 52, 223);
	ALT_CI_CI_COPY_RECT_0;

	ALT_CI_CI_FRAME_DONE_0;
	printf("Done!\n");

	while (1) {};

	return 0;
}
