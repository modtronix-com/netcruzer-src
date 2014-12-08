/**
 * @brief           Interface for accessing an external SPI EEPROM
 * @file            nz_xEeprom.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_xEeprom_desc Description
 *****************************************
 * Code for reading and writing to and from a external EEPROM chip on the SPI bus.
 * This file defines an interface for an external SPI EEPROM chip. The implementation
 * has to be done in a separate c file by creating instances of these functions.
 * The Netcruzer library contains a default implementations which can be used.
 *
 * @subsection nz_xEeprom_conf External EEPROM Project Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *******************************************************
 // ----------------- xEeprom Configuration -----------------
 // ********************************************************
 //#define XEEPROM_BUFFER_SIZE      (32)
 @endcode
 *
 * @subsection nz_xEeprom_hwconf External EEPROM Hardware Configuration
 *****************************************
 * The following defines the hardware used for this module, and is located in the
 * board specific file in the "netcruzer/lib" folder (brd_sbc66ec-r2.h for example).
 * They should only be changed if the default EEPROM is replaced with a custom one.
 @code
 // ********************************************************
 // ------------ xEeprom Hardware Configuration ------------
 // ********************************************************
 #define XEEPROM_SIZE           (8192)      //Default EEPROM is 25LC640 = 64kBits = 8kBytes
 #define XEEPROM_PAGE_SIZE      (32)
 @endcode
 **********************************************************************
 * @section nz_xEeprom_lic Software License Agreement
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
 * 2011-12-10, David H. (DH):
 *  - Initial version, modified version of Microchip's nz_xEeprom.h file from their TCP/IP stack
 *********************************************************************/
#ifndef __XEEPROM_H
#define __XEEPROM_H

#include "HardwareProfile.h"

#if !defined(EEPROM_CS_TRIS)
#error "EEPROM_CS_TRIS is not defined!"
#endif

#if !defined(XEEPROM_SIZE)
    #error "XEEPROM_SIZE not defined!"
#endif
#if !defined(XEEPROM_PAGE_SIZE)
    #error "XEEPROM_PAGE_SIZE not defined!"
#endif


/////////////////////////////////////////////////
//Alternative function names, use by Microchip Stack
typedef BOOL XEE_RESULT;
#define XEE_SUCCESS FALSE
#define XEEInit         xeeInit
#define XEEBeginWrite   xeeBeginWrite
#define XEEWrite        xeeWrite
#define XEEWriteArray   xeeWriteArray
#define XEEEndWrite     xeeEndWrite
#define XEEBeginRead    xeeBeginRead
#define XEERead         xeeRead
#define XEEReadArray    xeeReadArray
#define XEEEndRead      xeeEndRead
#define XEEIsBusy       xeeIsBusy


/**
 * Initialize SPI module to communicate to serial EEPROM.
 */
void xeeInit(void);


/**
 * Modifies internal address counter of EEPROM.
 *
 * @param address address to be set for writing
 *
 * @return TRUE if successful
 */
BOOL xeeBeginWrite(DWORD address);


/**
 * Writes a byte to the write cache, and if full, commits the write. Also, if a
 * write boundary is reached the write is committed. When finished writing,
 * xeeEndWrite() must be called to commit any unwritten bytes from the write cache.
 *
 * @preCondition xeeInit() && xeeBeginWrite() are already called.
 *
 * @param val Byte to be written
 *
 * @return TRUE if successful, else FALSE
 */
BOOL xeeWrite(BYTE val);


/**
 * This function writes an array of bytes to the EEPROM at the address specified
 * when xeeBeginWrite() was called.  Page boundary crossing is handled internally.
 *
 * Details:
 * The internal write cache is flushed at completion, so it is unnecessary to call
 * xeeEndWrite() after calling this function. However, if you do so, no harm will be done.
 *
 * @preCondition xeeInit() was called once and xeeBeginWrite() was called.
 *
 * @param val The array to write to the next memory location
 *
 * @param wLen The length of the data to be written
 */
void xeeWriteArray(BYTE *val, WORD wLen);


/**
 * Commits any last uncommitted bytes in cache to physical storage. Call this function
 * when you no longer need to write any more bytes at the selected address.
 *
 * @preCondition xeeInit() && xeeBeginWrite() are already called.
 *
 * @return TRUE if successful, else FALSE
 */
BOOL xeeEndWrite(void);


/**
 * Sets internal address counter to given address.
 *
 * @param address Address at which read is to be performed.
 *
 * @return TRUE if successful, else FALSE
 */
BOOL xeeBeginRead(DWORD address);


/**
 * Reads next byte from EEPROM; internal address is incremented by one.
 *
 * @preCondition xeeInit() && xeeBeginRead() are already called.
 *
 * @return BYTE that was read
 */
BYTE xeeRead(void);


/**
 * Reads desired number of bytes in sequential mode. This function performs all
 * necessary steps and releases the bus when finished.
 *
 * @preCondition xeeInit() is already called.
 *
 * @param address Address from where array is to be read
 *
 * @param buffer Caller supplied buffer to hold the data
 *
 * @param length Number of bytes to read.
 *
 * @return TRUE if successful, else FALSE
 */
BOOL xeeReadArray(DWORD address, BYTE *buffer, WORD length);


/**
 * Ends read
 *
 * @return TRUE if successful, else FALSE
 */
#define xeeEndRead()


/**
 * Indicates if the external EEPROM is busy
 *
 * @return TRUE if busy, FALSE if available
 */
BOOL xeeIsBusy(void);


#endif
