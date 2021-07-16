/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
#include "../../drivers/microSD/microSD.h" /* Example: Header file of existing MMC/SDC contorl module */
#include <string.h>

/* Definitions of physical drive number for each drive */
#define ATA 0 /* Example: Map ATA harddisk to physical drive 0 */
#define MMC 1 /* Example: Map MMC/SD card to physical drive 1 */

//SD卡块大小
#define SD_BLOCKSIZE 512

extern SD_CardInfo SDCardInfo;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	//int result;

	switch (pdrv)
	{
	case ATA:
		stat &= ~STA_NOINIT;
		return stat;

	case MMC:

		return stat;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	//int result;

	switch (pdrv)
	{
	case ATA:
		if (SD_Init() == SD_OK)
		{
			stat &= ~STA_NOINIT;
		}
		else
		{
			stat = STA_NOINIT;
		}
		return stat;

	case MMC:

		return stat;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv,	  /* Physical drive nmuber to identify the drive */
	BYTE *buff,	  /* Data buffer to store read data */
	DWORD sector, /* Sector address in LBA */
	UINT count	  /* Number of sectors to read */
)
{
	DRESULT res = RES_OK;
	//int result;
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;

	switch (pdrv)
	{
	case ATA:
		if ((DWORD)buff & 3) //判断存储区地址是否为4字节对齐
		{
			DWORD scratch[SD_BLOCKSIZE / 4];

			while (count--)
			{
				res = disk_read(ATA, (void *)scratch, sector++, 1);

				if (res != RES_OK)
				{
					break;
				}
				memcpy(buff, scratch, SD_BLOCKSIZE);
				buff += SD_BLOCKSIZE;
			}
			return res;
		}

		SD_state = SD_ReadMultiBlocks(buff, (uint64_t)sector * SD_BLOCKSIZE, SD_BLOCKSIZE, count);
		if (SD_state == SD_OK)
		{
			/* Check if the Transfer is finished */
			SD_state = SD_WaitReadOperation();
			while (SD_GetStatus() != SD_TRANSFER_OK)
				;
		}
		if (SD_state != SD_OK)
			status = RES_PARERR;
		else
			status = RES_OK;
		break;

	case MMC:

		return res;
	}

	return status;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write(
	BYTE pdrv,		  /* Physical drive nmuber to identify the drive */
	const BYTE *buff, /* Data to be written */
	DWORD sector,	  /* Sector address in LBA */
	UINT count		  /* Number of sectors to write */
)
{
	DRESULT res = RES_OK;
	//int result;

	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;

	if (!count)
	{
		return RES_PARERR; /* Check parameter */
	}

	switch (pdrv)
	{
	case ATA:
		if ((DWORD)buff & 3)
		{
			DWORD scratch[SD_BLOCKSIZE / 4];

			while (count--)
			{
				memcpy(scratch, buff, SD_BLOCKSIZE);
				res = disk_write(ATA, (void *)scratch, sector++, 1);
				if (res != RES_OK)
				{
					break;
				}
				buff += SD_BLOCKSIZE;
			}
			return res;
		}

		SD_state = SD_WriteMultiBlocks((uint8_t *)buff, (uint64_t)sector * SD_BLOCKSIZE, SD_BLOCKSIZE, count);
		if (SD_state == SD_OK)
		{
			/* Check if the Transfer is finished */
			SD_state = SD_WaitWriteOperation();

			/* Wait until end of DMA transfer */
			while (SD_GetStatus() != SD_TRANSFER_OK)
				;
		}
		if (SD_state != SD_OK)
			status = RES_PARERR;
		else
			status = RES_OK;
		break;

	case MMC:

		return res;
	}

	return status;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	BYTE cmd,  /* Control code */
	void *buff /* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;
	//int result;
	DRESULT status = RES_PARERR;

	switch (pdrv)
	{
	case ATA:

		switch (cmd)
		{
		// Get R/W sector size (WORD)
		case GET_SECTOR_SIZE:
			*(WORD *)buff = SD_BLOCKSIZE;
			break;
		// Get erase block size in unit of sector (DWORD)
		case GET_BLOCK_SIZE:
			*(DWORD *)buff = 1;
			break;

		case GET_SECTOR_COUNT:
			*(DWORD *)buff = SDCardInfo.CardCapacity / SDCardInfo.CardBlockSize;
			break;
		case CTRL_SYNC:
			break;
		}
		status = RES_OK;
		break;

	case MMC:

		return res;
	}

	return status;
}
#endif

DWORD get_fattime(void)
{
	/* 返回当前时间戳 */
	return ((DWORD)(2021 - 1980) << 25) /* Year 2021 */
		   | ((DWORD)5 << 21)			/* Month 5 */
		   | ((DWORD)21 << 16)			/* Mday 21 */
		   | ((DWORD)16 << 11)			/* Hour 16 */
		   | ((DWORD)40 << 5)			/* Min 40 */
		   | ((DWORD)0 >> 1);			/* Sec 0 */
}