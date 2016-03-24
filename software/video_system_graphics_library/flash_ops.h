/*
 * flash_ops.h
 *
 *  Created on: Mar 20, 2016
 *      Author: stephen
 */

#ifndef FLASH_OPS_H_
#define FLASH_OPS_H_

#include "graphics_defs.h"

/**
 * load_bmp: Read a bitmap file from SD card to SDRAM
 *
 * Arguments:
 *     file_name: String with the name of the file to open
 *     pixbuf: Pointer to pixbuf_t struct for output
 * Returns:
 *     0 on success, otherwise failure.
 */
char load_flash_bmp(char *file_name, pixbuf_t *pixbuf);

/**
 * load_file: Read a file from SD card to SDRAM
 *
 * Arguments:
 *     file_name: String with the name of the file to open
 *     dest_addr: Pointer to output buffer
 *     dest_size: Size of output buffer
 * Returns:
 *     0 on success, otherwise failure.
 */
char load_flash_file(char *file_name, void *dest_addr, unsigned int dest_size);

#endif /* FLASH_OPS_H_ */
