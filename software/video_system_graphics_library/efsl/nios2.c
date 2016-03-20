/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : nios2.c                                                          *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an Altera FPGA     *
*               running on NIOS2.                                             *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
                                                                              *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* As a special exception, if other files instantiate templates or             *
* use macros or inline functions from this file, or you compile this          *
* file and link it with other works to produce a work based on this file,     *
* this file does not by itself cause the resulting work to be covered         *
* by the GNU General Public License. However the source code for this         *
* file must still be made available in accordance with section (3) of         *
* the GNU General Public License.                                             *
*                                                                             *
* This exception does not invalidate any other reasons why a work based       *
* on this file might be covered by the GNU General Public License.            *
*                                                                             *
*                                                    (c)2005 Marcio Troccoli  *
\*****************************************************************************/

/*****************************************************************************/
#include "nios_2.h"
#include "sd.h"

#include "altera_avalon_spi_regs.h"
#include "altera_avalon_spi.h"
#include "system.h"

#define  DUMMY 0xFF
/*****************************************************************************/

esint8 if_initInterface(hwInterface* file, eint8* opts)
{
euint32 size;
	if_spiInit(file);
	if(sd_Init(file)<0)	{
		DBG((TXT("Card failed to init, breaking up...\n")));
		return(-1);
	}
	if(sd_State(file)<0){
		DBG((TXT("Card didn't return the ready state, breaking up...\n")));
		return(-2);
	}
    
    sd_getDriveSize(file,&size);

	file->sectorCount=size>>9; /* FIXME ASAP!! */
    DBG(TXT(("Card size:%d\n",size)));
	DBG((TXT("Init done...\n")));
	return(0);
}
/*****************************************************************************/ 

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
	return(sd_readSector(file,address,buf,512));
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf)
{
	return(sd_writeSector(file,address, buf));
}
/*****************************************************************************/ 

esint8 if_setPos(hwInterface* file,euint32 address)
{
	return(0);
}
/*****************************************************************************/ 

void if_spiInit(hwInterface *iface)
{
	euint8 i;
	IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SPI_0_BASE, 1 );
	printf("\n spi ini");
}
/*****************************************************************************/

euint8 if_spiSend(hwInterface *iface, euint8 outgoing)
{
	euint8 incoming=0;
	alt_u8  SD_Data=0,status;
  
	
	/* Set the SSO bit (force chipselect) */
	IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_0_BASE, 0x400);

	do{
    	    status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE);//get status
	}while (((status & ALTERA_AVALON_SPI_STATUS_TRDY_MSK) == 0 ) &&
            (status & ALTERA_AVALON_SPI_STATUS_RRDY_MSK) == 0);
	/* wait till transmit and ready ok */

	IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_0_BASE, outgoing);
    
	/* Wait until the interface has finished transmitting */
	do{status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE);}
	while ((status & ALTERA_AVALON_SPI_STATUS_TMT_MSK) == 0);

	/* reciver ready */
	if (((status & 0x80) != 0) ){
    	    SD_Data = IORD_ALTERA_AVALON_SPI_RXDATA(SPI_0_BASE);
	}
	else{
		printf("\n no recive after transmit");
	}
 
	IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SPI_0_BASE, 1);
	IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_0_BASE, 0);
 
	if( (status & 0x100) !=0)
    	printf("\n error in spi error in spi");
  
	return (SD_Data);
}
/*****************************************************************************/

euint8 if_spi_sendbuf(hwInterface *iface, euint8 *buf, euint16 len)
{
    alt_u8  SD_Data=0,status;
    alt_u16 i;
   
    /* Set the SSO bit (force chipselect) */
    IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_0_BASE, 0x400);

    for(i=0;i<len;i++)
    {
        do{
            status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE);//get status
        }while ((status & ALTERA_AVALON_SPI_STATUS_TRDY_MSK) == 0 );
        /* wait till transmit and ready ok */

        IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_0_BASE, buf[i]);
    }

    
    do{
        status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE);//get status
    }while ((status & ALTERA_AVALON_SPI_STATUS_TMT_MSK) == 0 );
 
    IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_0_BASE, 0);
    return (0);
}

/*****************************************************************************/

euint8 if_spi_readbuf(hwInterface *iface, euint8 *buf, euint16 len)
{
    alt_u8  status;
    alt_u16 i;
   
    /* Set the SSO bit (force chipselect) */
    IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_0_BASE, 0x400);
    /* Clear RX reg*/
    buf[0] = IORD_ALTERA_AVALON_SPI_RXDATA(SPI_0_BASE);
    /* wait till transmit and ready ok */
    do{
        status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE);//get status
    }while ((status & ALTERA_AVALON_SPI_STATUS_TRDY_MSK) == 0 );
    
    for(i=0;i<len;i++)
    {
        IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_0_BASE, DUMMY);
        /* Wait until the interface has finished transmitting */
        
        while ((IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE) & ALTERA_AVALON_SPI_STATUS_RRDY_MSK) == 0);

        /* reciver ready */
        buf[i] = IORD_ALTERA_AVALON_SPI_RXDATA(SPI_0_BASE);
    
    }
 
    IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_0_BASE, 0);
    return (0);
}

