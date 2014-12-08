/*********************************************************************
 * spiEEProm.c  -    Functions for accessing an external SPI EEPROM
 * Dependencies:  
 * Processor:     PIC24 or PIC33
 * Complier:      MCC30 v3.23 or higher (Tested on this version, lower versions might also work)
 * Company:       Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description - Functions for accessing an external SPI EEPROM
 *
 *********************************************************************
 * File History
 *
 * 2010-10-15, David Hosken (DH):
 *  - Initial version, modified version of Microchip's SPIEEPROM.H file from their TCP/IP stack
 *********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is intended and supplied to you,
 * the Company customer, for use solely and exclusively on products
 * manufactured by Modtronix Engineering. The code may be modified
 * and can be used free of charge for non commercial and commercial
 * applications. All rights are reserved. Any use in violation of the
 * foregoing restrictions may subject the user to criminal sanctions
 * under applicable laws, as well as to civil liability for the
 * breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */
#define SPIEEPROM_C

#include "HardwareProfile.h"
#include "spiEEProm.h"


// SPI Serial EEPROM buffer size.  To enhance performance while
// cooperatively sharing the SPI bus with other peripherals, bytes
// read and written to the memory are locally buffered. Legal
// sizes are 1 to the EEPROM page size.
#define EEPROM_BUFFER_SIZE              (32)

// Must be the EEPROM write page size, or any binary power of 2 divisor.  If 
// using a smaller number, make sure it is at least EEPROM_BUFFER_SIZE big for 
// max performance.  CAT25320 has a 32byte page. Microchip 25LC256 uses 64 byte page
//size, 25LC1024 uses 256 byte page size, so 64 is compatible with both.
#define EEPROM_PAGE_SIZE                (32)

// EEPROM SPI opcodes
#define OPCODE_READ    0x03    // Read data from memory array beginning at selected address
#define OPCODE_WRITE   0x02    // Write data to memory array beginning at selected address
#define OPCODE_WRDI    0x04    // Reset the write enable latch (disable write operations)
#define OPCODE_WREN    0x06    // Set the write enable latch (enable write operations)
#define OPCODE_RDSR    0x05    // Read Status register
#define OPCODE_WRSR    0x01    // Write Status register

static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
{
    while ((EEPROM_SPISTATbits.SPITBF == 1) || (EEPROM_SPISTATbits.SPIRBF == 0));
}

static void DoWrite(void);

static XEE_ADDRESS EEPROMAddress;
static BYTE EEPROMBuffer[EEPROM_BUFFER_SIZE];
static BYTE vBytesInBuffer;

/*********************************************************************
 * Function:        void xeeInit(unsigned char speed)
 *
 * PreCondition:    None
 *
 * Input:           speed - not used (included for compatibility only)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize SPI module to communicate to serial
 *                  EEPROM.
 *
 * Note:            Code sets SPI clock to Fosc/16.
 ********************************************************************/
 /* Not used to save space
void xeeInit(void)
{
    EEPROM_CS_IO = 1;
    EEPROM_CS_TRIS = 0;     // Drive SPI EEPROM chip select pin
}
*/


/*********************************************************************
 * Function:        XEE_RESULT xeeBeginRead(XEE_ADDRESS address)
 *
 * PreCondition:    None
 *
 * Input:           address - Address at which read is to be performed.
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Sets internal address counter to given address.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT xeeBeginRead(XEE_ADDRESS address)
{
    // Save the address and emptry the contents of our local buffer
    EEPROMAddress = address;
    vBytesInBuffer = 0;
    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        BYTE xeeRead(void)
 *
 * PreCondition:    xeeInit() && xeeBeginRead() are already called.
 *
 * Input:           None
 *
 * Output:          BYTE that was read
 *
 * Side Effects:    None
 *
 * Overview:        Reads next byte from EEPROM; internal address
 *                  is incremented by one.
 *
 * Note:            None
 ********************************************************************/
BYTE xeeRead(void)
{
    // Check if no more bytes are left in our local buffer
    if(vBytesInBuffer == 0u)
    {
        // Get a new set of bytes
        xeeReadArray(EEPROMAddress, EEPROMBuffer, EEPROM_BUFFER_SIZE);
        EEPROMAddress += EEPROM_BUFFER_SIZE;
        vBytesInBuffer = EEPROM_BUFFER_SIZE;
    }

    // Return a byte from our local buffer
    return EEPROMBuffer[EEPROM_BUFFER_SIZE - vBytesInBuffer--];
}

/*********************************************************************
 * Function:        XEE_RESULT xeeEndRead(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        This function does nothing.
 *
 * Note:            Function is used for backwards compatability with
 *                  I2C EEPROM module.
 ********************************************************************/
XEE_RESULT xeeEndRead(void)
{
    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT xeeReadArray(XEE_ADDRESS address,
 *                                          BYTE *buffer,
 *                                          WORD length)
 *
 * PreCondition:    xeeInit() is already called.
 *
 * Input:           address     - Address from where array is to be read
 *                  buffer      - Caller supplied buffer to hold the data
 *                  length      - Number of bytes to read.
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Reads desired number of bytes in sequential mode.
 *                  This function performs all necessary steps
 *                  and releases the bus when finished.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT xeeReadArray(XEE_ADDRESS address,
                        BYTE *buffer,
                        WORD length)
{
    volatile BYTE Dummy;

    EEPROM_CS_IO = 0;

    // Send READ opcode
    EEPROM_SSPBUF = OPCODE_READ;
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    // Send address
    #if (EEPROM_SIZE > 65535)
    EEPROM_SSPBUF = ((DWORD_VAL*)&address)->v[2];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;
    #endif

    EEPROM_SSPBUF = ((WORD_VAL*)&address)->v[1];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    EEPROM_SSPBUF = ((WORD_VAL*)&address)->v[0];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    while(length--)
    {
        EEPROM_SSPBUF = 0;
        WaitForDataByte();
        Dummy = EEPROM_SSPBUF;
        if(buffer != NULL)
            *buffer++ = Dummy;
    };

    EEPROM_CS_IO = 1;

    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT xeeBeginWrite(XEE_ADDRESS address)
 *
 * PreCondition:    None
 *
 * Input:           address     - address to be set for writing
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Modifies internal address counter of EEPROM.
 *
 * Note:            Unlike xeeSetAddr() in xeeprom.c for I2C EEPROM
 *                  memories, this function is used only for writing
 *                  to the EEPROM.  Reads must use xeeBeginRead(),
 *                  xeeRead(), and xeeEndRead().
 *                  This function does not use the SPI bus.
 ********************************************************************/
XEE_RESULT xeeBeginWrite(XEE_ADDRESS address)
{
    vBytesInBuffer = 0;
    EEPROMAddress = address;
    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT xeeWrite(BYTE val)
 *
 * PreCondition:    xeeInit() && xeeBeginWrite() are already called.
 *
 * Input:           val - Byte to be written
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Writes a byte to the write cache, and if full, 
 *                    commits the write.  Also, if a write boundary is 
 *                    reached the write is committed.  When finished 
 *                    writing, xeeEndWrite() must be called to commit 
 *                    any unwritten bytes from the write cache.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT xeeWrite(BYTE val)
{
    EEPROMBuffer[vBytesInBuffer++] = val;
    if(vBytesInBuffer >= sizeof(EEPROMBuffer))
        DoWrite();
    else if( (((BYTE)EEPROMAddress + vBytesInBuffer) & (EEPROM_PAGE_SIZE-1)) == 0u)
        DoWrite();

    return XEE_SUCCESS;
}


/*****************************************************************************
  Function:
    XEE_RESULT xeeWriteArray(BYTE *val, WORD wLen)

  Summary:
    Writes an array of bytes to the EEPROM part.

  Description:
    This function writes an array of bytes to the EEPROM at the address 
    specified when xeeBeginWrite() was called.  Page boundary crossing is 
    handled internally.
    
  Precondition:
    xeeInit() was called once and xeeBeginWrite() was called.

  Parameters:
    vData - The array to write to the next memory location
    wLen - The length of the data to be written

  Returns:
    None

  Remarks:
    The internal write cache is flushed at completion, so it is unnecessary 
    to call xeeEndWrite() after calling this function.  However, if you do 
    so, no harm will be done.
  ***************************************************************************/
void xeeWriteArray(BYTE *val, WORD wLen)
{
    while(wLen--)
        xeeWrite(*val++);
    
    xeeEndWrite();
}


/*********************************************************************
 * Function:        XEE_RESULT xeeEndWrite(void)
 *
 * PreCondition:    xeeInit() && xeeBeginWrite() are already called.
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Commits any last uncommitted bytes in cache to 
 *                    physical storage.
 *
 * Note:            Call this function when you no longer need to 
 *                    write any more bytes at the selected address.
 ********************************************************************/
XEE_RESULT xeeEndWrite(void)
{
    if(vBytesInBuffer)
        DoWrite();

    return XEE_SUCCESS;
}

static void DoWrite(void)
{
    BYTE i;
    volatile BYTE vDummy;

    // Set the Write Enable latch
    EEPROM_CS_IO = 0;
    EEPROM_SSPBUF = OPCODE_WREN;
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;
    EEPROM_CS_IO = 1;

    // Send WRITE opcode
    EEPROM_CS_IO = 0;
    EEPROM_SSPBUF = OPCODE_WRITE;
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;

    // Send address
    #if (EEPROM_SIZE > 65535)
    EEPROM_SSPBUF = ((DWORD_VAL*)&EEPROMAddress)->v[2];
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;
    #endif

    EEPROM_SSPBUF = ((WORD_VAL*)&EEPROMAddress)->v[1];
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;

    EEPROM_SSPBUF = ((WORD_VAL*)&EEPROMAddress)->v[0];
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;


    for(i = 0; i < vBytesInBuffer; i++)
    {
        // Send the byte to write
        EEPROM_SSPBUF = EEPROMBuffer[i];
        WaitForDataByte();
        vDummy = EEPROM_SSPBUF;
    }

    // Begin the write
    EEPROM_CS_IO = 1;

    // Update write address and clear write cache
    EEPROMAddress += vBytesInBuffer;
    vBytesInBuffer = 0;

    // Wait for write to complete
    while( xeeIsBusy() );
}


/*********************************************************************
 * Function:        BOOL xeeIsBusy(void)
 *
 * PreCondition:    xeeInit() is already called.
 *
 * Input:           None
 *
 * Output:          FALSE if EEPROM is not busy
 *                  TRUE if EEPROM is busy
 *
 * Side Effects:    None
 *
 * Overview:        Reads the status register
 *
 * Note:            None
 ********************************************************************/
BOOL xeeIsBusy(void)
{
    volatile BYTE_VAL result;

    EEPROM_CS_IO = 0;
    // Send RDSR - Read Status Register opcode
    EEPROM_SSPBUF = OPCODE_RDSR;
    WaitForDataByte();
    result.Val = EEPROM_SSPBUF;

    // Get register contents
    EEPROM_SSPBUF = 0;
    WaitForDataByte();
    result.Val = EEPROM_SSPBUF;
    EEPROM_CS_IO = 1;

    return result.bits.b0;
}


