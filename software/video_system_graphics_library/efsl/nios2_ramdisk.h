#include "types.h"
#include "config.h"

#define RAMDISK_SECTOR_SIZE 512

#define RAMDISK_ADDRESS_START  0x00800000
#define RAMDISK_SECTOR_CNT     16384

//#define CHECK_RAM 

EFSBlockDev * ramdisk_init(euint8 DevType);