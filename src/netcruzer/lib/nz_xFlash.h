 /**
 * @brief           Interface for accessing an external SPI FLASH
 * @file            nz_xFlash.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_xFlash_desc Description
 *****************************************
 * Code for reading and writing to and from a external FLASH chip on the SPI bus.
 * This file defines an interface for an external SPI Flash chip. The implementation
 * has to be done in a separate c file by creating instances of these functions.
 * The Netcruzer library contains a default implementations which can be used.
 *
 * @subsection nz_xFlash_hwconf External FLASH Hardware Configuration
 *****************************************
 * The following defines the hardware used for this module, and is located in the
 * board specific file in the "netcruzer/lib" folder (brd_sbc66ec-r2.h for example).
 * They should only be changed if the default EEPROM is replaced with a custom one.
 @code
 // ********************************************************
 // ------------ xFlash Hardware Configuration ------------
 // ********************************************************
 #define SPI_FLASH_SECTOR_SIZE      (4096ul)
 //#define SPI_FLASH_PAGE_SIZE       (0ul)           // SST has no page boundary requirements
 #define SPI_FLASH_PAGE_SIZE         (256ul)         // Winbond FLASH has 256 byte pages
 #define SPI_FLASH_SIZE              (0x400000ul)    // Flash is 4MByte
 @endcode
 **********************************************************************
 * @section  nz_xFlash_lic Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2013-08-01, David H. (DH):
 *  - Initial version, modified version of Microchip's SPI Flash file from their TCP/IP stack
 *********************************************************************/
#ifndef __XFLASH_H
#define __XFLASH_H

#include "HardwareProfile.h"

#if !defined(SPIFLASH_CS_TRIS)
#error "SPIFLASH_CS_TRIS is not defined!"
#endif

#if !defined(SPI_FLASH_SECTOR_SIZE)
    #error "SPI_FLASH_SECTOR_SIZE not defined!"
#endif
#if !defined(SPI_FLASH_PAGE_SIZE)
    #error "SPI_FLASH_PAGE_SIZE not defined!"
#endif

#define SPI_FLASH_SECTOR_MASK       (SPI_FLASH_SECTOR_SIZE - 1)
#define SPI_FLASH_PAGE_MASK         (SPI_FLASH_PAGE_SIZE - 1)


/////////////////////////////////////////////////
//Alternative function names, use by Microchip Stack
#define SPIFlashInit        xflashInit
#define SPIFlashReadArray   xflashReadArray
#define SPIFlashBeginWrite  xflashBeginWrite
#define SPIFlashWrite       xflashWrite
#define SPIFlashWriteArray  xflashWriteArray
#define SPIFlashEraseSector xflashEraseSector


/**
 * Initializes SPI Flash module. This function is only called once during the
 * lifetime of the application.
 * Internal:
 * This function sends WRDI to clear any pending write operation, and also clears
 * the software write-protect on all memory locations.
 */
void xflashInit(void);


/**
 * Reads an array of bytes from the SPI Flash module.
 * @preCondition xflashInit has been called, and the chip is not busy (should be
 * handled elsewhere automatically.)
 *
 * @param dwAddress Address from which to read
 * @param vData Where to store data that has been read
 * @param wLen Length of data to read
 */
void xflashReadArray(DWORD dwAddress, BYTE *vData, WORD wLen);


/**
 * Prepares the SPI Flash module for writing. Subsequent calls to xflashWrite or
 * xflashWriteArray will begin at this location and continue sequentially.
 * Remarks:
 * Flash parts have large sector sizes, and can only erase entire sectors at once. The Winbond
 * parts for which this library was written have sectors that are 4kB in size. Your application
 * must ensure that writes begin on a sector boundary so that the xflashWrite functions will
 * erase the sector before attempting to write. Entire sectors need not be written at once, so
 * applications can begin writing to the front of a sector, perform other tasks, then later call
 * xflashBeginWrite and point to anaddress in this sector that has not yet been programmed.
 * However, care must taken to ensure that writes are not attempted on addresses that are
 * not in the erased state. The chip will provide no indication that the write has failed, and
 * will silently ignore the command.
 *
 *   !!!!!!!!!!!!!!    WINBOND - IMPORTANT   !!!!!!!!!!!!
 * Writing to the WINBOND Flash is disabled for up to 10ms after power up. During
 *  this time, the CS should be high!
 *
 * @preCondition xflashInit has been called.
 *
 * @param dwAddr Address where the writing will begin
 */
void xflashBeginWrite(DWORD dwAddr);


/**
 * This function writes a byte to the SPI Flash part. If the current address pointer indicates
 * the beginning of a 4kB sector, the entire sector will first be erased to allow writes to
 * proceed. If the current address pointer indicates elsewhere, it will be assumed that the
 * sector has already been erased.  If this is not true, the chip will silently ignore the
 * write command.
 *
 * Remarks:
 * See Remarks in xflashBeginWrite for important information about Flash memory parts.
 *
 * !!!!!!!!!!!!!!    WINBOND - IMPORTANT   !!!!!!!!!!!!
 * Writing to the WINBOND Flash is disabled for up to 10ms after power up. During
 * this time, the CS should be high!
 *
 * @preCondition xflashInit() and xflashBeginWrite() have been called, and the current
 * address is either the front of a 4kB sector or has already been erased.
 *
 * @param vData The byte to write to the next memory location.
 */
void xflashWrite(BYTE vData);


/**
 * This function writes an array of bytes to the SPI Flash part. When the address pointer
 * crosses a sector boundary (and has more data to write), the next sector will automatically
 * be erased. If the current address pointer indicates an address that is not a sector boundary
 * and is not already erased, the chip will silently ignore the write command until the
 * next sector boundary is crossed.
 *
 * Remarks:
 * See Remarks in xflashBeginWrite() for important information about Flash memory parts.
 *
 * !!!!!!!!!!!!!!    WINBOND - IMPORTANT   !!!!!!!!!!!!
 * Writing to the WINBOND Flash is disabled for up to 10ms after power up. During
 * this time, the CS should be high!
 *
 * @preCondition xflashInit() and xflashBeginWrite() have been called, and the current address
 * is either the front of a sector or has already been erased.
 *
 * @param vData The array to write to the next memory location
 *
 * @param wLen The length of the data to be written
 *
 * @param bEraseSector When true, will erase secotor if given address is on sector
 *        boundry (is first byte of new sector)
 */
void xflashWriteArray(BYTE *vData, WORD wLen, BOOL bEraseSector);


/**
 * This function erases a sector in the Flash part. It is called internally by the
 * xflashWrite functions whenever a write is attempted on the first byte in a sector.
 *
 * Remarks:
 * See Remarks in xflashBeginWrite for important information about Flash memory parts.
 *
 * @preCondition xflashInit has been called.
 *
 * @param dwAddr The address of the sector to be erased.
 */
void xflashEraseSector(DWORD dwAddr);


#endif
