#include <system.h>
#include <io.h>
#include <stdlib.h>
#include <sys/alt_cache.h>

#include "graphics_defs.h"
#include "graphics_commands.h"
#include "palettes.h"

void *graphics_sdram_backbuffer;
static pixbuf_t sdram_backbuffer;

static int min(int a, int b)
{
	if (a < b) return a;
	else return b;
}

static int max(int a, int b)
{
	if (a > b) return a;
	else return b;
}

char graphics_init()
{
	sdram_backbuffer.base_address = alt_uncached_malloc(FRAME_WIDTH * FRAME_HEIGHT);
	sdram_backbuffer.width = FRAME_WIDTH;
	sdram_backbuffer.height = FRAME_HEIGHT;
	if (sdram_backbuffer.base_address == 0) {
		return -E_NOMEM;
	} else {
		IOWR_32DIRECT(VIDEO_FB_STREAMER_0_BASE, 0, sdram_backbuffer.base_address);
		graphics_sdram_backbuffer = sdram_backbuffer.base_address; // FIXME: Remove this
		return E_SUCCESS;
	}
}

pixbuf_t *graphics_get_final_buffer()
{
	return &sdram_backbuffer;
}


void graphics_draw_pixel(pixbuf_t *pixbuf, int x, int y, unsigned char color)
{
	unsigned char *pixel = (unsigned char *) (pixbuf->base_address + x + y * pixbuf->width);
	*pixel = color;
}


/**
 * graphcis_draw_rectangle: Draw a filled rectangle between two points
 *
 * Arguments:
 *     pixbuf: The pixel buffer to draw to
 *     x1: X-coordinate of the first point
 *     y1: Y-coordinate of the first point
 *     x2: X-coordinate of the second point
 *     y2: Y-coordinate of the second point
 *     color: Color of the rectangle
 */
void graphics_draw_rectangle(pixbuf_t *pixbuf, int x1, int y1, int x2, int y2, unsigned char color)
{
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 0, (int) pixbuf->base_address); // Frame address
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 4, min(x1, x2)); // X1
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 8, min(y1, y2)); // Y1
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 12, max(x1, x2)); // X2
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 16, max(y1, y2)); // Y2
	IOWR_32DIRECT(CI_DRAW_RECT_0_BASE, 20, color); // Color
	ALT_CI_CI_DRAW_RECT_0;
}

/**
 * graphcis_draw_line: Draw a line between two points on the screen
 *
 * Arguments:
 *     pixbuf: The pixel buffer to draw to
 *     x1: X-coordinate of the start of the line
 *     y1: Y-coordinate of the start of the line
 *     x2: X-coordinate of the end of the line
 *     y2: Y-coordinate of the end of the line
 *     color: Color of the line
 */
void graphics_draw_line(pixbuf_t *pixbuf, int x1, int y1, int x2, int y2, unsigned char color)
{
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 0, (int) pixbuf->base_address); // Frame address
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 4, x1); // X1
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 8, y1); // Y1
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 12, x2); // X2
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 16, y2); // Y2
	IOWR_32DIRECT(CI_DRAW_LINE_0_BASE, 20, color); // Color
	ALT_CI_CI_DRAW_LINE_0;
}

/**
 * graphics_clear_screen: Clear the SDRAM back-buffer, and copy it to SRAM
 */
void graphics_clear_screen()
{
	graphics_draw_rectangle(&sdram_backbuffer, 0, 0, sdram_backbuffer.width-1, sdram_backbuffer.height-1, 0x00);
	ALT_CI_CI_FRAME_DONE_0;
}

/**
 * graphics_clear_buffer: Set all of the pixels in a buffer to zero
 *
 * Arguments:
 *     pixbuf: Buffer to clear
 */
void graphics_clear_buffer(pixbuf_t *pixbuf)
{
	graphics_draw_rectangle(pixbuf, 0, 0, pixbuf->width-1, pixbuf->height-1, 0x00);
}

//Fonts:
//https://github.com/dhepper/font8x8
//License, Public Domain.


/**
 * 8x8 monochrome bitmap fonts for rendering
 * Author: Daniel Hepper <daniel@hepper.net>
 *
 * License: Public Domain
 *
 * Based on:
 * // Summary: font8x8.h
 * // 8x8 monochrome bitmap fonts for rendering
 * //
 * // Author:
 * //     Marcel Sondaar
 * //     International Business Machines (public domain VGA fonts)
 * //
 * // License:
 * //     Public Domain
 *
 * Fetched from: http://dimensionalrift.homelinux.net/combuster/mos3/?p=viewsource&file=/modules/gfx/font8_8.asm
 **/

// Constant: font8x8_basic
// Contains an 8x8 font map for unicode points U+0000 - U+007F (basic latin)
char font8x8_basic[128][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0000 (nul)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0001
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0002
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0003
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0004
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0005
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0006
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0007
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0008
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0009
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0010
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0011
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0012
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0013
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0014
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0015
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0016
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0017
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0018
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0019
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0020 (space)
    { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},   // U+0021 (!)
    { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0022 (")
    { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},   // U+0023 (#)
    { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},   // U+0024 ($)
    { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},   // U+0025 (%)
    { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},   // U+0026 (&)
    { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0027 (')
    { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},   // U+0028 (()
    { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},   // U+0029 ())
    { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},   // U+002A (*)
    { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},   // U+002B (+)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+002C (,)
    { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},   // U+002D (-)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+002E (.)
    { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},   // U+002F (/)
    { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},   // U+0030 (0)
    { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},   // U+0031 (1)
    { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},   // U+0032 (2)
    { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},   // U+0033 (3)
    { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},   // U+0034 (4)
    { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},   // U+0035 (5)
    { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},   // U+0036 (6)
    { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},   // U+0037 (7)
    { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},   // U+0038 (8)
    { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},   // U+0039 (9)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+003A (:)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+003B (//)
    { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00},   // U+003C (<)
    { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00},   // U+003D (=)
    { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},   // U+003E (>)
    { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00},   // U+003F (?)
    { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00},   // U+0040 (@)
    { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},   // U+0041 (A)
    { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},   // U+0042 (B)
    { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},   // U+0043 (C)
    { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00},   // U+0044 (D)
    { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00},   // U+0045 (E)
    { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00},   // U+0046 (F)
    { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00},   // U+0047 (G)
    { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},   // U+0048 (H)
    { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0049 (I)
    { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00},   // U+004A (J)
    { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},   // U+004B (K)
    { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00},   // U+004C (L)
    { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},   // U+004D (M)
    { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},   // U+004E (N)
    { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},   // U+004F (O)
    { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},   // U+0050 (P)
    { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00},   // U+0051 (Q)
    { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00},   // U+0052 (R)
    { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00},   // U+0053 (S)
    { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0054 (T)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00},   // U+0055 (U)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0056 (V)
    { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},   // U+0057 (W)
    { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},   // U+0058 (X)
    { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},   // U+0059 (Y)
    { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},   // U+005A (Z)
    { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00},   // U+005B ([)
    { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00},   // U+005C (\)
    { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},   // U+005D (])
    { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00},   // U+005E (^)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},   // U+005F (_)
    { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0060 (`)
    { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},   // U+0061 (a)
    { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00},   // U+0062 (b)
    { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00},   // U+0063 (c)
    { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00},   // U+0064 (d)
    { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00},   // U+0065 (e)
    { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00},   // U+0066 (f)
    { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0067 (g)
    { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00},   // U+0068 (h)
    { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0069 (i)
    { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E},   // U+006A (j)
    { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00},   // U+006B (k)
    { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+006C (l)
    { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00},   // U+006D (m)
    { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00},   // U+006E (n)
    { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},   // U+006F (o)
    { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F},   // U+0070 (p)
    { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78},   // U+0071 (q)
    { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00},   // U+0072 (r)
    { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00},   // U+0073 (s)
    { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00},   // U+0074 (t)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00},   // U+0075 (u)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0076 (v)
    { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00},   // U+0077 (w)
    { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00},   // U+0078 (x)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0079 (y)
    { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00},   // U+007A (z)
    { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00},   // U+007B ({)
    { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},   // U+007C (|)
    { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00},   // U+007D (})
    { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+007E (~)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}    // U+007F
};

/**
 * 8x8 monochrome bitmap fonts for rendering
 * Author: Daniel Hepper <daniel@hepper.net>
 *
 * License: Public Domain
 *
 * Based on:
 * // Summary: font8x8.h
 * // 8x8 monochrome bitmap fonts for rendering
 * //
 * // Author:
 * //     Marcel Sondaar
 * //     International Business Machines (public domain VGA fonts)
 * //
 * // License:
 * //     Public Domain
 *
 * Fetched from: http://dimensionalrift.homelinux.net/combuster/mos3/?p=viewsource&file=/modules/gfx/font8_8.asm
 **/

// Constant: font8x8_2580
// Contains an 8x8 font map for unicode points U+2580 - U+259F (block elements)
char font8x8_block[32][8] = {
    { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00},   // U+2580 (top half)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},   // U+2581 (box 1/8)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF},   // U+2582 (box 2/8)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF},   // U+2583 (box 3/8)
    { 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF},   // U+2584 (bottom half)
    { 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},   // U+2585 (box 5/8)
    { 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},   // U+2586 (box 6/8)
    { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},   // U+2587 (box 7/8)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},   // U+2588 (solid)
    { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F},   // U+2589 (box 7/8)
    { 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F},   // U+258A (box 6/8)
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F},   // U+258B (box 5/8)
    { 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F},   // U+258C (left half)
    { 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07},   // U+258D (box 3/8)
    { 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03},   // U+258E (box 2/8)
    { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},   // U+258F (box 1/8)
    { 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0},   // U+2590 (right half)
    { 0x55, 0x00, 0xAA, 0x00, 0x55, 0x00, 0xAA, 0x00},   // U+2591 (25% solid)
    { 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA},   // U+2592 (50% solid)
    { 0xFF, 0xAA, 0xFF, 0x55, 0xFF, 0xAA, 0xFF, 0x55},   // U+2593 (75% solid)
    { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+2594 (box 1/8)
    { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},   // U+2595 (box 1/8)
    { 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F},   // U+2596 (box bottom left)
    { 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0},   // U+2597 (box bottom right)
    { 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00},   // U+2598 (box top left)
    { 0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF},   // U+2599 (boxes left and bottom)
    { 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0},   // U+259A (boxes top-left and bottom right)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0x0F},   // U+259B (boxes top and left)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xF0, 0xF0},   // U+259C (boxes top and right)
    { 0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00},   // U+259D (box top right)
    { 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F},   // U+259E (boxes top right and bottom left)
    { 0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF},   // U+259F (boxes right and bottom)
};


void draw_letter(pixbuf_t *pixbuf, int y1, int x1, int color, int pixel_size, char* letter){

	//x1 and y1 are the top left corner of the letter.

    int x,y;
    int set;
    int mask;
    for (x=0; x < 8; x++) { //++x might be faster?
        for (y=0; y < 8; y++) { //++y might be faster??
            set = letter[x] & 1 << y;
            	if (set) {
            		if (pixel_size > 1) {
            			graphics_draw_rectangle(pixbuf, y1 + y*pixel_size, x1 + x*pixel_size, y1 + (y*pixel_size)+pixel_size, x1 + (x*pixel_size)+pixel_size, color);
            		} else {
            			//Drawing individual pixels faster than drawing a rectangle?
            			//draw_rectangle(y1 + y, x1 + x, y1 + y, x1 + x, color);
            			graphics_draw_pixel(pixbuf, y, x, color);
            		}
            	}
            //alt_printf("%c", set ? 'X' : ' ');
        }
        //alt_printf("\n");
    }


}

void print2screen(pixbuf_t *pixbuf, int x1, int y1, int color, int pixel_size, char* string){

	int i = 0;

	int spacing = 1.5 * pixel_size;

	while (string[i] != '\0'){
		//printf("%c %d", string[i], (int)string[i]);
		draw_letter(pixbuf, x1 + (i* 8 * pixel_size /* letters are 8 wide */) + (i ? 1 : 0)*spacing,
					y1, /* y shouldn't change, we are writing on a line */
					color,
					pixel_size,
					font8x8_basic[  (int)string[i]  ]
					);
		i++;

	}


}

void graphics_draw_circle(pixbuf_t *pixbuf, int cx, int cy, int radius, int color, int filled){

	//Filled = 0 only draws the border
	//Filled = 1 will fill it, but in software.

	if (!filled){
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 0, pixbuf->base_address); // Frame address
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 4, cx); // CX
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 8, cy); // CY
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 12, radius); // Radius
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 16, color); // Color
		ALT_CI_CI_DRAW_CIRC_0;
	}

	if (filled < 0) {//Interference pattern causes trippy effect. Get the effect by setting filled to be <0.
		int r = 0;
		for (r = 1; r < radius; r++) {
			IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 0, pixbuf->base_address); // Frame address
			IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 4, cx); // CX
			IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 8, cy); // CY
			IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 12, r*(-1)*filled); // Radius
			IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 16, color); // Color
			ALT_CI_CI_DRAW_CIRC_0;
		}
	}

	if (filled == 1) {
		//Building filled circles in hardware causes a brain stack overflow with how deep and complicated the state machine is.
		//What we need instead is draw a circle, then in software, call the line algorithm a bunch of times to draw a line from
		//The center to the edges of the circle. It's not as fast as doing it all in hardware, but it should still be relatively
		//fast since we're not plotting individual pixels in C.
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 0, pixbuf->base_address); // Frame address
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 4, cx); // CX
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 8, cy); // CY
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 12, radius); // Radius
		IOWR_32DIRECT(CI_DRAW_CIRC_0_BASE, 16, color); // Color
		ALT_CI_CI_DRAW_CIRC_0;

		//How to draw the filler lines:
		//http://stackoverflow.com/questions/5607946/bresenhams-circle-algorithm-filling-question
		//p.x -> cx
		//p.y -> cy
		//Stack overflow code has x, y reversed.
//		( cx+x, cy+y ) to ( cx-x, cy+y )
//		( cx+y, cy+x ) to ( cx-y, cy+x )
//		( cx+x, cy-y ) to ( cx-x, cy-y )
//		( cx+y, cy-x ) to ( cx-y, cy-x )

		//Bresenham's Circle Drawing Algorithm, modified.
		  int x = radius;
		  int y = 0;
		  int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

		  while( y <= x )
		  {
			graphics_draw_line(pixbuf, cx+x, cy+y, cx-x, cy+y, color);
			graphics_draw_line(pixbuf, cx+y, cy+x, cx-y, cy+x, color);
			graphics_draw_line(pixbuf, cx+x, cy-y, cx-x, cy-y, color);
			graphics_draw_line(pixbuf, cx+y, cy-x, cx-y, cy-x, color);

		    y++;
		    if (decisionOver2<=0)
		    {
		      decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
		    }
		    else
		    {
		      x--;
		      decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
		    }
		  }
	}
}

//Rounded Rectangle (filled)
void graphics_draw_rounded_rect(pixbuf_t *pixbuf, int x1, int y1, int x2, int y2, int radius, int filled, unsigned char color){

	if (radius == 0){
		//Regular unfilled rectangle. Draw with 4 lines.
		graphics_draw_line(pixbuf, x1, y1, x2, y1, color); //Top
		graphics_draw_line(pixbuf, x1, y1, x1, y2, color); //Left
		graphics_draw_line(pixbuf, x2, y1, x2, y2, color); //Right
		graphics_draw_line(pixbuf, x1, y2, x2, y2, color); //Bottom

		if (filled == 1) {
			graphics_draw_rectangle(pixbuf, x1, y1, x2, y1, color);
		}
	} else {
		graphics_draw_circle(pixbuf, x1 + radius, y1 + radius, radius, color, 1); //TL
		graphics_draw_circle(pixbuf, x2 - radius, y1 + radius, radius, color, 1); //TR
		graphics_draw_circle(pixbuf, x1 + radius, y2 - radius, radius, color, 1); //BL
		graphics_draw_circle(pixbuf, x2 - radius, y2 - radius, radius, color, 1); //BR

		if (filled == 1){
			graphics_draw_rectangle(pixbuf, x1 + radius, y1 + radius, x2 - radius, y2 - radius, color); //Center
		}

		graphics_draw_rectangle(pixbuf, x1 + radius, y1, x2 - radius, y1 + radius, color); //top
		graphics_draw_rectangle(pixbuf, x1, y1 + radius, x1 + radius, y2 - radius, color); //left
		graphics_draw_rectangle(pixbuf, x2 - radius, y1 + radius, x2, y2 - radius, color); //right
		graphics_draw_rectangle(pixbuf, x1 + radius, y2 - radius, x2 - radius, y2, color); //bottom
	}


}


//Triangle command is just a bunch of lines from a point.

//If bottom is flat line, then just do one line from top, along the bottom. Optimization.
//if bottom is not flat, then look at coordinates and find a flat line. For loop over that thing from
//the other line.
//If no flat sides, then need to plot 2 triangles. Make a flat one, then recurse into self :)


void graphics_draw_triangle(pixbuf_t *pixbuf, int x1, int y1, int x2, int y2, int x3, int y3, int filled, int color){

	if (filled == 0) {
		//Easiest base case
		graphics_draw_line(pixbuf, x1, y1, x2, y2, color);
		graphics_draw_line(pixbuf, x2, y2, x3, y3, color);
		graphics_draw_line(pixbuf, x1, y1, x3, y3, color);
	} else {

		//0 for no flat sides. 1 for flat side in x direction opposite point 1, -1 for flat
		//side in y direction opposite point 1, and so on.
		int flatside = 0;
		int i = 0;
		int t; //temp.

		//Also re-arrange the assigned points so that the for loops work.
		if (y2 == y3){
			flatside = 1; //Sweep from 1 to (x[2..3], y2)

			if (x2 > x3){//Need to re-arrange points so x2<x3.
				t = x2;
				x2 = x3; // x2 <= x3.
				x3 = t;
			}
		}
		if (x2 == y3){
			flatside = -1; //Sweep from 1 to (x2, y[2..3])

			if (y2 > y3) { //Swap so y2 < y3
				t = y2;	y2 = y3; y3 = t;
			}
		}
		if (y1 == y3){
			flatside = 2; //Sweep from 2 to (x[1..3], y1)

			if (x1 > x3){//Need to re-arrange points so x1<x3.
				t = x1;	x1 = x3; /* x1 <= x3. */ x3 = t;
			}
		}
		if (x1 == x3){
			flatside = -2; //Sweep from 2 to (x1, y[1..3])
			if (y1 > y3) { //Swap so y1 < y3
				t = y1;	y1 = y3; y3 = t;
			}
		}
		if (y1 == y2){
			flatside = 3; //Sweep from 3 to (x[1..2], y1)
			if (x1 > x2){//Need to re-arrange points so x1<x2.
				t = x1;	x1 = x2; /* x1 <= x2. */ x2 = t;
			}
		}
		if (x1 == x2){
			flatside = -3; //Sweep from 3 to (x1, y[1..2])
			if (y1 > y2) { //Swap so y1 < y2
				t = y1;	y1 = y2; y2 = t;
			}
		}

		if (flatside == 1){
			for (i = x2 ; i < x3; i++){
				graphics_draw_line(pixbuf, x1, y1, i, y2, color);
			}
			return;
		}
		if (flatside == -1){
			for (i = y2 ; i < y3; i++){
				graphics_draw_line(pixbuf, x1, y1, x2, i, color);
			}
			return;
		}
		if (flatside == 2){
			for (i = y1 ; i < y3; i++){
				graphics_draw_line(pixbuf, x2, y2, i, y3, color);
			}
			return;
		}
		if (flatside == -2){
			for (i = y1 ; i < y3; i++){
				graphics_draw_line(pixbuf, x2, y2, x1, i, color);
			}
			return;
		}
		if (flatside == 3) {
			for (i = x1 ; i < x2; i++){
				graphics_draw_line(pixbuf, x3, y3, i, y2, color);
			}
			return;
		}
		if (flatside == -3){
			for (i = x1 ; i < y2; i++){
				graphics_draw_line(pixbuf, x3, y3, y1, i, color);
			}
			return;
		}

		if (flatside == 0){
			//Now we have to split up the triangle into 2, calculate the bounds of the sweeps.

			//First, re-arrange the points to the top most is 1, left most is 2, right most is 3.
			int tx1;
			int ty1;

			int tx2;
			int ty2;

			int tx3;
			int ty3;

			if (y1 < y2 && y1 < y3){ tx1 = x1; ty1 = y1; }
			if (y2 < y1 && y2 < y3){ tx1 = x2; ty1 = y2; }
			if (y3 < y1 && y3 < y2){ tx1 = x3; ty1 = y3; }

			if (x1 < x2 && x1 < x3){ tx2 = x1; ty2 = y1; }
			if (x2 < x1 && x2 < x3){ tx2 = x2; ty2 = y2; }
			if (x3 < x2 && x3 < x1){ tx2 = x3; ty2 = y3; }

			if (x1 > x2 && x1 > x3){ tx3 = x1; ty3 = y1; }
			if (x2 > x1 && x2 > x3){ tx3 = x2; ty3 = y2; }
			if (x3 > x2 && x3 > x1){ tx3 = x3; ty3 = y3; }


			//Next imagine a vertical line dropped down from 1, stopping where it intersects with
			//the 2->3 line. Calculate that intersection, then plot 2 triangles.

			int dropdowny = ty1;

			int dy_offset =(tx1 - tx2) * (abs(ty3 - ty2) / abs(tx3 - tx2)) ;

			dropdowny = ty1 + ((ty2 < ty3) ? ty2 : ty3) + dy_offset;

			//left
			graphics_draw_triangle(pixbuf, tx1, ty1, tx2, ty2, tx1, dropdowny, filled, color);
			//right
			//draw_triangle (tx1, ty1, tx3, ty3, tx1, dropdowny, filled, color+1);


		}
	}
}

/**
 * copy_buffer_area: Copy part of a pixel buffer to another pixel buffer in memory
 *
 * Arguments:
 *     source:
 *     dest:
 *     source_area:
 *     dest_offset:
 */
void copy_buffer_area(pixbuf_t *source, pixbuf_t *dest, rect_t *source_area, point_t *dest_offset)
{
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 0, source->base_address);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 4, source->width);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 8, source->height);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 12, dest->base_address);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 16, dest->width);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 20, dest->height);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 24, source_area->p1.x);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 28, source_area->p1.y);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 32, source_area->p2.x);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 36, source_area->p2.y);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 40, dest_offset->x);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 44, dest_offset->y);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 48, 0);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 52, 0);
	ALT_CI_CI_COPY_RECT_0;
}

/**
 * copy_buffer_area_transparent: Copy part of a pixel buffer to another pixel buffer in memory
 *
 * Colors matching t_color will not be copied, leaving whatever color was
 * previously present at that location
 *
 * Arguments:
 *     source:
 *     dest:
 *     source_area:
 *     dest_offset:
 *     t_color: Color representing transparent pixels
 */
void copy_buffer_area_transparent(pixbuf_t *source, pixbuf_t *dest, rect_t *source_area, point_t *dest_offset, unsigned char t_color)
{
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 0, source->base_address);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 4, source->width);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 8, source->height);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 12, dest->base_address);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 16, dest->width);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 20, dest->height);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 24, source_area->p1.x);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 28, source_area->p1.y);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 32, source_area->p2.x);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 36, source_area->p2.y);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 40, dest_offset->x);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 44, dest_offset->y);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 48, 1);
	IOWR_32DIRECT(CI_COPY_RECT_0_BASE, 52, t_color);
	ALT_CI_CI_COPY_RECT_0;
}
