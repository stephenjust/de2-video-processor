/* This test program generates a simple video pattern in the device's SRAM.
 * 
 * The video pattern consists of a white box along the edges of the screen,
 * along with blocks of colour forming diagonal stripes across the screen.
 * 
 * The purpose of this program is to verify frame alignment on the video
 * output signal, i.e. to verify that the frame is not translated in some
 * direction, and that it is stationary.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>


void printPalette(int n){
	// Print everything in the palette ram, upto int colours.
	int i;
	unsigned int c;

	unsigned int results[512] = {'\0'};

	for (i = 0; i < n; i++){
		c = IORD_16DIRECT(COLOUR_PALETTE_SHIFTER_0_BASE, 2*i); //offset multiplied by 2 to be on 16-bit boundaries.
		//alt_printf("palette[ %x ]: %x ", 2*i, c);
		results[i] = c;
	}

	for (i = 0; i < n; i++){
		alt_printf("palette[ %x ]: %x ", 2*i, results[i]);
	}

}


//Setting address 0xFF will change the border colour.
void changeBorderColor(unsigned int col){

	IOWR_16DIRECT(COLOUR_PALETTE_SHIFTER_0_BASE, 2*0xFF, col);


}


int main()
{ 
	int row = 0;
	int col = 0;
	int color;

	unsigned int i = 0;
	unsigned int delay = 0;
	// Clear the screen first.
	alt_putstr("Clearing Screen\n");


	for (col = 0; col < 640; col = col + 4){
		for (row = 0; row < 480; row++){
			color = 0;
			IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, color << 24 | color << 16 | color << 8 | color << 0);
		}
	}

	changeBorderColor(0xF800);

//	for (i = 0; i <= 16; i++){
//		alt_printf("Colour: %u", i);
//		for (delay = 0; delay < 2000; delay++){
//			unsigned int tdelay = delay;
//			for (tdelay; tdelay > 0; tdelay--){}
//		}
//
//		for (row=0; row<480; row++){
//			for (col = 0; col < 640; col=col+4){
//				color = i;
//				IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, color << 24 | color << 16 | color << 8 | color << 0);
//			}
//
//		}
//	}

	printPalette(7);

	alt_putstr("\nStarting write\n");

	for (row = 0; row < 480; row++)
	{

//		if (row%2 == 1){
//			//Skip every 4th row for clarity? Keeping the first one and last one.
//			continue;
//		}

		for (col = 0; col < 640; col = col + 4)
		{
			color = ((row + col) % 256) << 0 | ((row + col) % 256) << 8 | ((row + col) % 256) << 16 | ((row + col) % 256) << 24;
			//color = 3;

			//color = 0; // Dark blue is first on colour band.

			//Want to get a bunch of 1px by 1px colour blocks. Also want the colours to keep incrementing and wrapping around.
			//color = (color + col)%15; //max val of colour is 14, which should be yellow.

			//IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, (color+3) << 24 | (color+2) << 16 | (color+1) << 8 | (color+0) << 0);
			if (row == 0 || row == 479)
			{
				IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, 0xFFFFFFFF);
			}
			else if (col == 0)
			{
				IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, 0x000000FF | color);
			}
			else if (col == 636)
			{
				IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, 0xFF000000 | color);
			}
			else
			{
				IOWR_32DIRECT(SRAM_0_BASE, row * 640 + col, color);
			}
		}
	}

	alt_putstr("Done.\n");
	while (1);

	return 0;
}
