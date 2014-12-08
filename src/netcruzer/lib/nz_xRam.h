 /**
 * @brief           Interface for accessing an external SPI RAM
 * @file            nz_xRam.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_xRam_desc Description
 *****************************************
 * Code for reading and writing to and from a external RAM chip on the SPI bus.
 * This file defines an interface for an external SPI Ram chip. The implementation
 * has to be done in a separate c file by creating instances of these functions.
 * The Netcruzer library contains a default implementations which can be used.
 *
 * @subsection nz_xRam_hwconf External RAM Hardware Configuration
 *****************************************
 * The following defines the hardware used for this module, and is located in the
 * board specific file in the "netcruzer/lib" folder (brd_sbc66ec-r2.h for example).
 * They should only be changed if the default EEPROM is replaced with a custom one.
 @code
 // ********************************************************
 // ------------ xRam Hardware Configuration ------------
 // ********************************************************
 #define SPI_RAM_SECTOR_SIZE    (4096ul)
 #define SPI_RAM_PAGE_SIZE      (0ul)
 @endcode
 **********************************************************************
 * @section  nz_xRam_lic Software License Agreement
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
 *  - Initial version, modified version of Microchip's SPI RAM file from their TCP/IP stack
 *********************************************************************/
#ifndef __XRAM_H
#define __XRAM_H

#include "HardwareProfile.h"

/////////////////////////////////////////////////
//Alternative function names, use by Microchip Stack
#define SPIRAMInit              xramInit
#define SPIRAMGetArray          xramGetArray
#define SPIRAMPutArray          xramPutArray
#define SPIRAMPutString         xramPutString
#define SPIRAMPutROMArray       xramPutROMArray
#define SPIRAMPutROMString      xramPutROMString

void xramInit(void);
void xramGetArray(WORD wAddress, BYTE *vData, WORD wLength);
void xramPutArray(WORD wAddress, BYTE *vData, WORD wLength);
#define xramPutString(a,b)        xramPutArray(a, strlen((char*)b))
#define xramPutROMString(a,b)    xramPutArray(a, strlen((char*)b))
#define xramPutROMArray(a,b,c)    xramPutROMArray(a, b, c)

#endif
