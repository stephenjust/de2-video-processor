/*
 * sdcard_ops.h
 *
 *  Created on: Mar 20, 2016
 *      Author: stephen
 */

#ifndef SDCARD_OPS_H_
#define SDCARD_OPS_H_

#include "efsl/efs.h"
#include "graphics_defs.h"

/**
 * load_bmp: Read a bitmap file from SD card to SDRAM
 *
 * Arguments:
 *     efsl: EmbeddedFileSystem struct (create with efs_init(&efsl, SPI_0_NAME))
 *     file_name: String with the name of the file to open
 *     pixbuf: Pointer to pixbuf_t struct for output
 * Returns:
 *     0 on success, otherwise failure.
 */
char load_bmp(EmbeddedFileSystem *efsl, char *file_name, pixbuf_t *pixbuf);

/**
 * load_file: Read a file from SD card to SDRAM
 *
 * Arguments:
 *     efsl: EmbeddedFileSystem struct (create with efs_init(&efsl, SPI_0_NAME))
 *     file_name: String with the name of the file to open
 *     dest_addr: Pointer to output buffer
 *     dest_size: Size of output buffer
 * Returns:
 *     0 on success, otherwise failure.
 */
char load_file(EmbeddedFileSystem *efsl, char *file_name, void *dest_addr, unsigned int dest_size);

#endif /* SDCARD_OPS_H_ */
