
#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <string.h>

#include "graphics_commands.h"



unsigned char color_array[] = {
		0xE0, 0xEC, 0xF4, 0xF8,
		0xFC, 0xFA, 0xBE, 0x9D,
		0x1D, 0x1F, 0x17, 0x13,
		0x73, 0xFB, 0xF1, 0xE2
};



int main()
{
	unsigned int row, col;
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int delay = 0;

	alt_putstr("Restoring EGA Color Palette \n");
	unsigned int palette_ega[16] =
			/* EGA Colour Palette */
			/* Black			  Blue	       Green       Cyan          Red */
			{/*00*/ 0x0000, /*01*/0x0015, /*02*/0x2704, /*03*/0x1E79, /*04*/0xA800,
			 /*     Magenta       Brown   Light Grey     Dark Grey   Bright Blue*/
			 /*05*/ 0xA815, /*06*/0xE3C1, /*07*/0xAD55, /*08*/0x52AA, /*09*/0x52BF,
			 /* BGreen            BCyan         Bred       B Magenta     B Yellow*/
			 /*10*/ 0x57EA, /*11*/0x57FF, /*12*/0xFAAA, /*13*/0xFABF, /*14*/0xFFEA,
			 0xFFFF /*B White */
			};


	for (i = 0; i < 16; i++){
		IOWR_16DIRECT(COLOUR_PALETTE_SHIFTER_0_BASE, 2*i, 0x0000);
		IOWR_16DIRECT(COLOUR_PALETTE_SHIFTER_0_BASE, 2*i, palette_ega[i]);
	}


	clear_screen();



	//Draw a bunch of primitives in for loops, use the benchmark routine to alt_printf an
	//fps counter.


	return 0;
}
