 /**
 * @brief           Default debug implementation.
 * @file            nz_debugDefault.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 **********************************************************************
 * Software License Agreement
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
 * 2013-07-01, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_DEBUG_DEFAULT_C

#include "HardwareProfile.h"

#if defined(HAS_NZ_DEBUGGING) && !defined(NZSYS_DISABLE_DEFAULT_DEBUG)

#include "nz_debug.h"
#include "nz_circularBuffer.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"
#include "nz_serDataPorts.h"



//If "Power of 2" buffer used, size is power of 2 -AND- equal to or larger than CIRBUF_MIN_SIZE
#if defined(CIRBUF_USE_CIRCULAR_BUFFER_PWR2)
    //Ensure size is power of 2 -AND- equal to or larger than CIRBUF_MIN_SIZE
    #if ( ((DEBUG_TXBUF_SIZE & ~(DEBUG_TXBUF_SIZE-1))!=DEBUG_TXBUF_SIZE) || (DEBUG_TXBUF_SIZE < CIRBUF_MIN_SIZE))
    #error "DEBUG_TXBUF_SIZE is not power of 2, AND larger or equal to CIRBUF_MIN_SIZE!"
    #endif

    //Ensure size is power of 2 -AND- equal to or larger than CIRBUF_MIN_SIZE
    #if ( ((DEBUG_RXBUF_SIZE & ~(DEBUG_RXBUF_SIZE-1))!=DEBUG_RXBUF_SIZE) || (DEBUG_RXBUF_SIZE < CIRBUF_MIN_SIZE))
    #error "DEBUG_RXBUF_SIZE is not power of 2, AND larger or equal to CIRBUF_MIN_SIZE!"
    #endif
#endif

//Test if space if available in output buffer, and wait if not
#if !defined(DEBUG_WAIT_FOR_SPACE)
#define DEBUG_WAIT_FOR_SPACE					( 0 )
#endif

//TX and RX "Circular Buffers". Use SERPORT_DEBUG_CREATE_OWN_CIRBUFS define to use this
//module without SERPORTs. If NOT defined, this modules requires nz_serDataPorts.c
#if defined(SERPORT_DEBUG_CREATE_OWN_CIRBUFS)
    CIRBUF  bufTxDebug;
    BYTE    bufTxDebug[DEBUG_TXBUF_SIZE];   //Buffer used for Debug Tx

    CIRBUF  cbufRxDebug;
    BYTE    cbufRxDebug[DEBUG_RXBUF_SIZE];   //Buffer used for Debug Rx
#endif

DEBUG_ERROR_FLAGS debugErrorFlags = {.val = 0};
char debugTempBuf[DEBUG_TEMP_BUF_SIZE];

void debugCheckErrorFlags(void);

/**
 * Initialize debug
 */
void debugInit(void) {
    //!!!!! IMPORTANT !!!!!
    //This gets done right at the start of initialization! Don't rely on ANY other code being initialize
    //at this stage! Only make the buffers available so that any debug information can be added to the
    //buffers. At a later stage, when the debug port (USB, RS232...) has been initialized, it will be
    //sent to the configured debug port

    #if defined(SERPORT_DEBUG_CREATE_OWN_CIRBUFS)
    #if defined(DEBUG_USE_STREAMING )
        //USB TX and RX buffer initialization. Configure as Packet (not large packet) Circular Buffer
        cbufInit(&cbufTxDebug,
                bufTxDebug,
                DEBUG_TXBUF_SIZE,
                CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);

        cbufInit(&cbufRxDebug,
                bufRxDebug,
                DEBUG_RXBUF_SIZE,
                CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
    #else
        //USB TX and RX buffer initialization. Configure as Packet (not large packet) Circular Buffer
        cbufInit(&cbufTxDebug,
                bufTxDebug,
                DEBUG_TXBUF_SIZE,
                CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);

        cbufInit(&cbufRxDebug,
                cbufRxDebug,
                DEBUG_RXBUF_SIZE,
                CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);
    #endif
    #endif  //#if defined(SERPORT_DEBUG_CREATE_OWN_CIRBUFS)

    debugCheckErrorFlags();
}

/**
 * Call once all system initialization is done. Normally just before main system loop.
 */
void debugAllInitDone(void)
{
    debugCheckErrorFlags();
}


void debugCheckErrorFlags(void) {
    //Check if any initialization errors occurred
    if (debugErrorFlags.bits.bCirbufSizeError == 1) {
        debugErrorFlags.bits.bCirbufSizeError = 0;
        //DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nCirbuf size ERROR!");
        debugPutString("\nErr: Cirbuf size!");
    }
    
    //Check if any initialization errors occurred
    if (debugErrorFlags.bits.bCirbufPacketFormatError == 1) {
        debugErrorFlags.bits.bCirbufPacketFormatError = 0;
        debugPutString("\nErr: Cirbuf Packet Format Error!");
    }
}

/**
 * Write a single character to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 *
 * @param c Character to add to the buffer
 */
void debugPutChar(CHAR c)
{
    //Wait for space to become available
    #if (DEBUG_WAIT_FOR_SPACE==1)
    debugWaitForSpace(1);
    #endif

    cbufPutByte(CIRBUF_TX_DEBUG, c);
}


/**
 * Write BYTE in decimal format to the debug output. For example, debugPutByte(100) will
 * write the string "100" to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 *
 * @param b Byte to add to the buffer
 */
void debugPutByte(BYTE b)
{
    char buf[4];

    //Wait for space to become available
    #if (DEBUG_WAIT_FOR_SPACE==1)
    debugWaitForSpace(4);
    #endif

    utoa(buf, ((WORD)b) & 0x00ff, 10);
    cbufPutString(CIRBUF_TX_DEBUG, buf);
}

/**
 * Write WORD in decimal format to the debug output. For example, debugPutWord(100) will
 * write the string "100" to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 *
 * @param w Word to add to the buffer
 */
void debugPutWord(WORD w) {
    char buf[6];

    //Wait for space to become available
    #if (DEBUG_WAIT_FOR_SPACE==1)
    debugWaitForSpace(6);
    #endif

    utoa(buf, w, 10);
    cbufPutString(CIRBUF_TX_DEBUG, buf);
}


/**
 * Adds given number of bytes to debug port. If no more space in debug port
 * buffer, function returns without doing anything.
 *
 * @param pArr Pointer to source BYTE array
 * @param size Number of bytes to copy
 */
void debugPutArray(const BYTE* pArr, WORD size)
{
    //Wait for space to become available
    #if (DEBUG_WAIT_FOR_SPACE==1)
    debugWaitForSpace(size);
    #endif

    cbufPutArray(CIRBUF_TX_DEBUG, pArr, size);
}


/**
 * Send given NULL terminated string to debug port. If no more space in debug port
 * buffer, function returns without doing anything.
 *
 * @param str Null terminated string to add
 */
void debugPutString(const char * str)
{
    //Wait for space to become available
    #if (DEBUG_WAIT_FOR_SPACE==1)
    debugWaitForSpace(strlen(str));
    #endif

    cbufPutString(CIRBUF_TX_DEBUG, str);
}


/**
 * Write the given BYTE as a 2-character "ASCII Formatted Hex" string to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 * @param b Byte to write
 */
void debugPutHexByte(BYTE b) {
    char cBuf[2];

    //Wait for space to become available
    #if (DEBUG_WAIT_FOR_SPACE==1)
    debugWaitForSpace(2);
    #endif

    cBuf[0] = nzHighNibbleToAsciiHex(((BYTE)b));
    cBuf[1] = nzLowNibbleToAsciiHex(((BYTE)b));
    cbufPutArray(CIRBUF_TX_DEBUG, (BYTE*)cBuf, 2);
}


/**
 * Write the given WORD as a 4-character "ASCII Formatted Hex" string to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 * @param w Word to write
 */
void debugPutHexWord(WORD w) {
    char cBuf[4];

    //Wait for space to become available
    #if (DEBUG_WAIT_FOR_SPACE==1)
    debugWaitForSpace(4);
    #endif

    cBuf[0] = nzHighNibbleToAsciiHex(((BYTE)(w>>8)));
    cBuf[1] = nzLowNibbleToAsciiHex(((BYTE)(w>>8)));
    cBuf[2] = nzHighNibbleToAsciiHex(((BYTE)w));
    cBuf[3] = nzLowNibbleToAsciiHex(((BYTE)w));
    cbufPutArray(CIRBUF_TX_DEBUG, (BYTE*)cBuf, 4);
}

void debugPutHexLine(DWORD adr, BYTE* buf, BYTE len) {
    BYTE i;

    //Print Address - 11 bytes buffer space required
    cbufPutByte(CIRBUF_TX_DEBUG, '\n');
    debugPutHexWord( ((WORD)(adr>>16)) );
    debugPutHexWord(((WORD)adr));
    cbufPutByte(CIRBUF_TX_DEBUG, ':');
    cbufPutByte(CIRBUF_TX_DEBUG, ' ');

    //Print line, maximum 16 bytes
    i=0;
    while (i<len) {
        cbufPutByte(CIRBUF_TX_DEBUG, nzHighNibbleToAsciiHex(buf[i]));
        cbufPutByte(CIRBUF_TX_DEBUG, nzLowNibbleToAsciiHex(buf[i]));
        if (len > ++i)
            cbufPutByte(CIRBUF_TX_DEBUG, ' ');
    }
}


/**
 * Prints the contents of the FLASH to the debug port.
 * @param address
 * @param length
 */
/*
void debugPrintFlash(DWORD address, WORD length)
{
    BYTE buf[16];
    BYTE len;

    len = 16 - (((BYTE)address) & 0x0f);
    if (length < len)
        len = length;

    while ((len != 0) && (cbufGetFree(CIRBUF_TX_DEBUG) > (11 + len*2))) {
        xflashReadArray(address, buf, len);

        debugPutHexLine(address, buf, len);

        address += len;
        length -= len;
        len = 16;
        if (length < len)
            len = length;
    }
}
*/


/**
 * Send given NULL terminated string to debug port. If no more space in debug port
 * buffer, function returns without doing anything.
 *
 * @param str Null terminated string to add
 */
/*
void debugPutString(const char * str)
{
    //Has buffer got space for string and control bytes
    if (cbufGetFree(CIRBUF_TX_DEBUG) > strlen(str)) {
        cbufPutString(CIRBUF_TX_DEBUG, str);
    }
}
*/

/**
 * Wait for given bytes to become available in debug Transmit buffer.
 * @param required Number of bytes to wait for to become available
 */
void debugWaitForSpace(BYTE required) {
    //If USB is used for Debugging, call usbTask() if TX buffer is full.
    #if defined(HAS_USBHID_DEBUGGING) || defined(HAS_USBCDC_DEBUGGING)

	//If not Configured, we do NOT wait!
	if(serUSBIsSuspended()) return;

    while (cbufGetFree(CIRBUF_TX_DEBUG) < (required)) {
        serUSBTask();
    }
    #endif
}

/**
 * Wait till all pending bytes in the debug transmit buffer has been sent.
 */
void debugWaitTillAllSent(void) {
    //If USB is used for Debugging, call usbTask() if TX buffer is full.
    #if defined(HAS_USBHID_DEBUGGING) || defined(HAS_USBCDC_DEBUGGING)

	//If not Configured, we do NOT wait!
	if(serUSBIsSuspended()) return;

    while (cbufIsEmpty(CIRBUF_TX_DEBUG) == FALSE) {
        serUSBTask();
    }
    #endif
}

/**
 * Tests if all pending bytes in the debug transmit buffer has been sent.
 * @return Returns TRUE if all pending data has been sent, else FALSE
 */
BOOL debugIsTxBufEmpty(void) {
    //If USB is used for Debugging, call usbTask() if TX buffer is full.
    #if defined(HAS_USBHID_DEBUGGING) || defined(HAS_USBCDC_DEBUGGING)

    return cbufIsEmpty(CIRBUF_TX_DEBUG);
    #endif
}

#endif  //#if defined(HAS_NZ_DEBUGGING) && !defined(NZSYS_DISABLE_DEFAULT_DEBUG)