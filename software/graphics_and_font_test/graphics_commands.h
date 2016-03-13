#ifndef __GRAPHICSCMDS_H_
#define __GRAPHICSCMDS_H_



#define PALETTE_SIZE 256
#define SDRAM_VIDEO_OFFSET 0x300000

extern unsigned int palette_ega[];
extern unsigned int palette_stephen[];
extern unsigned int palette_magenta[];

extern unsigned int palette_bunch_o_blues[];

extern unsigned int palette_bunch_o_reds[];



extern unsigned int* palettes[];
extern unsigned int palettes_legnth;


/** Switch the palette loaded in the palette shifter. Takes an int* to the palette
 * definition, and the length of that colour palette. */
void switchPalette(unsigned int* palette, int length);


/** Prints the current colour palette in the palette shifter to the niosII
 *  console */
void printPalette(int n);


void draw_rectangle(int x1, int y1, int x2, int y2, unsigned char color);

void draw_line(int x1, int y1, int x2, int y2, unsigned char color);


void clear_screen();


/* Fonts */
extern char font8x8_block[][8];
extern char font8x8_basic[][8];

void draw_letter(int y1, int x1, int color, int pixel_size, char* letter);
void print2screen(int x1, int y1, int color, int pixel_size, char* string);
void draw_circle (int cx, int cy, int radius, int color, int filled);


#endif /* __GRAPHICSCMDS_H_ */
