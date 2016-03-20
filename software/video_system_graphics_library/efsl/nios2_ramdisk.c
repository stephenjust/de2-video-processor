

/*****************************************************************************/
#include "system.h"
#include "alt_types.h"
#include "nios2_ramdisk.h"


//#define CHECK_RAM 1

static esint8 ramdisk_setup(euint8 DevType)
{
#ifdef CHECK_RAM
UINT32 i;
UINT16 *pbuff;
    //test whole ram space
    memset((eunit8 *)(RAMDISK_ADDRESS_START),0xAA,RAMDISK_SECTOR_CNT * RAMDISK_SECTOR_SIZE);
    for(i=RAMDISK_ADDRESS_START;i<(RAMDISK_ADDRESS_START + RAMDISK_SECTOR_CNT * RAMDISK_SECTOR_SIZE);i+=2)
    {
        pbuff = (UINT16 *)i;
        
        if((*pbuff)!= 0xAAAA)
            return -1;
    }
#endif   
    
   // memset((eunit8 *)(RAMDISK_ADDRESS_START),0x0,RAMDISK_SECTOR_CNT * RAMDISK_SECTOR_SIZE);
	return 0;
}

/*--------------------------------------
  return actual bytes read
*/
static esint8 ramdisk_read( int block, char *buf,int blkcnt)
{
#ifdef EN_DMA
    memcpy_dma(buf,(eunit8 *)(RAMDISK_ADDRESS_START + block * RAMDISK_SECTOR_SIZE),blkcnt * RAMDISK_SECTOR_SIZE);
#else
	memcpy(buf,(euint8 *)(RAMDISK_ADDRESS_START + block * RAMDISK_SECTOR_SIZE),blkcnt * RAMDISK_SECTOR_SIZE);
#endif
	return blkcnt*512;
}
/*---------------------------------------
  return actual bytes written
*/
static esint8 ramdisk_write(int block, char *buf,int blkcnt)
{
#ifdef EN_DMA
    
	memcpy_dma((eunit8 *)(RAMDISK_ADDRESS_START + block * RAMDISK_SECTOR_SIZE),buf,blkcnt * RAMDISK_SECTOR_SIZE);
#else

	memcpy((euint8 *)(RAMDISK_ADDRESS_START + block * RAMDISK_SECTOR_SIZE),buf,blkcnt * RAMDISK_SECTOR_SIZE);
#endif
	return blkcnt*512;
}


static EFSBlockDev spifuncs ={
	ramdisk_setup,
	ramdisk_read,
	ramdisk_write
};

/*
*******************************************************************************
*                           GLOBAL FUNCTIONS
*******************************************************************************
*/
EFSBlockDev * ramdisk_init(euint8 DevType)
{
	return &spifuncs;
}

