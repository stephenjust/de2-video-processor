/*
 * sdcard_ops.c
 *
 * Functions to handle loading bitmaps and files from SD cards
 *
 *  Created on: Mar 20, 2016
 *      Author: stephen
 */

#include <system.h>
#include <stdlib.h>

#include <altera_avalon_sgdma.h>
#include <altera_avalon_sgdma_descriptor.h>
#include <altera_avalon_sgdma_regs.h>

#include "efsl/efs.h"
#include "efsl/ls.h"

#include "graphics_defs.h"
#include "sdcard_ops.h"


#define FILE_CHUNK_SIZE 2048
#define BMP_PIXDATA_OFFSET 0x0A
#define BMP_TYPE_OFFSET 0x0E
#define BMP_WIDTH_OFFSET 0x12
#define BMP_HEIGHT_OFFSET 0x16
#define BMP_BITS_OFFSET 0x1C

static int min(int a, int b) {
	if (a < b) return a;
	else return b;
}


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
char load_bmp(EmbeddedFileSystem *efsl, char *file_name, pixbuf_t *pixbuf)
{
	File file;\
	unsigned char buffer[FILE_CHUNK_SIZE];
	int pixdata_offset, i, bytes_read, status;
	alt_sgdma_descriptor descriptor, next_descriptor;
	alt_sgdma_dev *dev;

	if (file_fopen(&file, &(efsl->myFs), file_name, 'r') != 0) {
		return -E_IO;
	}

	// Read the first block of the file to get metadata
	file_setpos(&file, 0);
	file_read(&file, FILE_CHUNK_SIZE, buffer);

	// Read bitmap metadata
	pixdata_offset = *((short *) (buffer + BMP_PIXDATA_OFFSET));
	if (*(buffer + BMP_TYPE_OFFSET) != 40) {
		file_fclose(&file);
		return -E_INVALID_BMP;
	}
	pixbuf->width = *((unsigned short *) (buffer + BMP_WIDTH_OFFSET));
	pixbuf->height = *((unsigned short *) (buffer + BMP_HEIGHT_OFFSET));

	// Check that we have an 8-bit bitmap
	if (*((short *) (buffer + BMP_BITS_OFFSET)) != 8) {
		file_fclose(&file);
		return -E_INVALID_BMP;
	}

	// To simplify logic, require that the image width is less than
	// the size of our copy chunk size
	if (pixbuf->width > FILE_CHUNK_SIZE) {
		file_fclose(&file);
		return -E_INVALID_BMP;
	}

	// Allocate memory for our pixel buffer
	pixbuf->base_address = malloc(pixbuf->width * pixbuf->height);
	if (pixbuf->base_address == NULL) {
		file_fclose(&file);
		return -E_NOMEM;
	}

	// Advance the file pointer to the beginning of the image
	file_setpos(&file, pixdata_offset);

	// Initialize the DMA copier
	dev = alt_avalon_sgdma_open(SGDMA_0_NAME);
	if (dev == NULL) {
		free(pixbuf->base_address);
		file_fclose(&file);
		return -E_IO;
	}

	// Copy the bitmap, mirroring in the y-direction to match our coordinate system
	for (i = pixbuf->height-1; i >= 0; --i)
	{
		bytes_read = file_read(&file, pixbuf->width, buffer);
		alt_avalon_sgdma_construct_mem_to_mem_desc(
				&descriptor,
				&next_descriptor,
				(alt_u32 *) buffer,
				(alt_u32 *) (pixbuf->base_address + i*pixbuf->width),
				bytes_read,
				0,
				0);
		status = alt_avalon_sgdma_do_sync_transfer(dev, &descriptor);
	}

	// Close the file
	if (file_fclose(&file) != 0) {
		free(pixbuf->base_address);
		return -E_IO;
	}

	return 0;
}

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
char load_file(EmbeddedFileSystem *efsl, char *file_name, void *dest_addr, unsigned int dest_size)
{
	File file;
	unsigned char buffer[FILE_CHUNK_SIZE];
	unsigned int offset;
	int bytes_read, bytes_to_copy, status;
	alt_sgdma_descriptor descriptor, next_descriptor;
	alt_sgdma_dev *dev;

	if (file_fopen(&file, &(efsl->myFs), file_name, 'r') != 0) {
		return -E_IO;
	}

	// Open the DMA device
	dev = alt_avalon_sgdma_open(SGDMA_0_NAME);
	if (dev == 0) {
		file_fclose(&file);
		return -E_IO;
	}

	offset = 0;
	while (offset < file.FileSize)
	{
		bytes_read = file_read(&file, FILE_CHUNK_SIZE, buffer);
		bytes_to_copy = min(bytes_read, dest_size - offset);
		alt_avalon_sgdma_construct_mem_to_mem_desc(
				&descriptor,
				&next_descriptor,
				(alt_u32 *) buffer,
				(alt_u32 *) (dest_addr + offset),
				bytes_to_copy,
				0,
				0);
		status = alt_avalon_sgdma_do_sync_transfer(dev, &descriptor);
		offset += bytes_read;
	}

	// Close the file
	if (file_fclose(&file) != 0) {
		return -E_IO;
	}

	return 0;
}
