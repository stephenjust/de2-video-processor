/*
 * palettes.h
 *
 *  Created on: Mar 20, 2016
 *      Author: stefan
 */

#ifndef PALETTES_H_
#define PALETTES_H_

#include "graphics_defs.h"

extern const palette_t palette_ega;
extern const palette_t palette_332;
extern const palette_t palette_magenta;
extern const palette_t palette_bunch_o_blues;
extern const palette_t palette_bunch_o_reds;

/**
 * Switch the palette loaded in the palette shifter.
 *
 * Arguments:
 *     palette: Pointer to palette structure
 */
void switch_palette(palette_t *palette);

/**
 * Print the current colour palette to stdout
 *
 * Arguments:
 *     n: The number of entries to print the value of
 */
void print_palette(int n);

#endif /* PALETTES_H_ */
