/*********************************************************************
 *
 *               Data SPI EEPROM Access Routines
 *
 *********************************************************************
 * FileName:        spiEeprom.c
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *                    Microchip C30 v3.12 or higher
 *                    Microchip C18 v3.30 or higher
 *                    HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *        ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *        used in conjunction with a Microchip ethernet controller for
 *        the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/20/02     Original (Rev. 1.0)
 * Howard Schlunder     9/01/04     Rewritten for SPI EEPROMs
 * Howard Schlunder     8/10/06     Modified to control SPI module
 *                                  frequency whenever EEPROM accessed
 *                                  to allow bus sharing with different
 *                                  frequencies.
********************************************************************/
#define __SPIEEPROM_C           //This define is used by the Microchip code
#define THIS_IS_SPI_EEPROM_C

#include "HardwareProfile.h"

// If the CS line is not defined, spiEeprom.c's content will not be compiled.
// If you are using a serial EEPROM please define the CS pin as EEPROM_CS_TRIS
// in HardwareProfile.h
#if defined(EEPROM_CS_TRIS)

#include "TCPIP Stack/TCPIP.h"

//Default EEPROM is 25LC640 = 64kBits = 8kBytes
#if !defined (XEEPROM_SIZE)
    #define XEEPROM_SIZE                    (8192)      //Default EEPROM is 25LC640 = 64kBits = 8kBytes)
#endif

// Must be the EEPROM write page size, or any binary power of 2 divisor.  If  using a smaller number,
// make sure it is at least XEEPROM_BUFFER_SIZE big for  max performance.  25LC08 - 25LC640 uses 32 byte
//page size. 25LC256 uses 64 byte page size, 25LC1024 uses 256 byte page size.
#if !defined (XEEPROM_PAGE_SIZE)
    #define XEEPROM_PAGE_SIZE                (32)        //25LC640 has a 32byte page.
#endif

// SPI Serial EEPROM buffer size.  To enhance performance while cooperatively sharing the SPI
// bus with other peripherals, bytes read and written to the memory are locally buffered. Legal
// sizes are 1 to the EEPROM page size.
#if !defined (XEEPROM_BUFFER_SIZE)
    #define XEEPROM_BUFFER_SIZE              (32)
#endif

#if !defined (XEEPROM_MAX_SPI_FREQ)
    #define XEEPROM_MAX_SPI_FREQ     (10000000ul)    // Hz
#endif

// EEPROM SPI opcodes
#define OPCODE_READ    0x03    // Read data from memory array beginning at selected address
#define OPCODE_WRITE   0x02    // Write data to memory array beginning at selected address
#define OPCODE_WRDI    0x04    // Reset the write enable latch (disable write operations)
#define OPCODE_WREN    0x06    // Set the write enable latch (enable write operations)
#define OPCODE_RDSR    0x05    // Read Status register
#define OPCODE_WRSR    0x01    // Write Status register


#if defined (__18CXX)
    #define ClearSPIDoneFlag()  {EEPROM_SPI_IF = 0;}
    #define WaitForDataByte()   {while(!EEPROM_SPI_IF); EEPROM_SPI_IF = 0;}
#elif defined(__C30__)
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while ((EEPROM_SPISTATbits.SPITBF == 1) || (EEPROM_SPISTATbits.SPIRBF == 0));
    }
#elif defined( __PIC32MX__ )
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while (!EEPROM_SPISTATbits.SPITBE || !EEPROM_SPISTATbits.SPIRBF);
    }
#else
    #error Determine SPI flag mechanism
#endif

static void DoWrite(void);

static DWORD EEPROMAddress;
static BYTE EEPROMBuffer[XEEPROM_BUFFER_SIZE];
static BYTE vBytesInBuffer;


/**
 * Initialize SPI module to communicate to serial EEPROM.
 */
void xeeInit(void)
{
}


/**
 * Sets internal address counter to given address.
 *
 * @param address Address at which read is to be performed.
 *
 * @return TRUE if successful, else FALSE
 */
BOOL xeeBeginRead(DWORD address)
{
    // Save the address and emptry the contents of our local buffer
    EEPROMAddress = address;
    vBytesInBuffer = 0;
    return XEE_SUCCESS;
}


/**
 * Reads next byte from EEPROM; internal address is incremented by one.
 *
 * @preCondition xeeInit() && xeeBeginRead() are already called.
 *
 * @return BYTE that was read
 */
BYTE xeeRead(void)
{
    // Check if no more bytes are left in our local buffer
    if(vBytesInBuffer == 0u)
    {
        // Get a new set of bytes
        xeeReadArray(EEPROMAddress, EEPROMBuffer, XEEPROM_BUFFER_SIZE);
        EEPROMAddress += XEEPROM_BUFFER_SIZE;
        vBytesInBuffer = XEEPROM_BUFFER_SIZE;
    }

    // Return a byte from our local buffer
    return EEPROMBuffer[XEEPROM_BUFFER_SIZE - vBytesInBuffer--];
}


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
BOOL xeeReadArray(DWORD address,
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
    #if defined(USE_EEPROM_25LC1024)
    EEPROM_SSPBUF = ((DWORD_VAL*)&address)->v[2];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;
    #endif

    EEPROM_SSPBUF = ((DWORD_VAL*)&address)->v[1];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    EEPROM_SSPBUF = ((DWORD_VAL*)&address)->v[0];
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


/**
 * Modifies internal address counter of EEPROM.
 *
 * @param address address to be set for writing
 *
 * @return TRUE if successful
 */
BOOL xeeBeginWrite(DWORD address)
{
    vBytesInBuffer = 0;
    EEPROMAddress = address;
    return TRUE;
}


/**
 * Writes a byte to the write cache, and if full, commits the write. Also, if a
 * write boundary is reached the write is committed. When finished writing,
 * xeeEndWrite() must be called to commit any unwritten bytes from the write cache.
 *
 * @PreCondition xeeInit() && xeeBeginWrite() are already called.
 *
 * @param val Byte to be written
 *
 * @return TRUE if successful, else FALSE
 */
BOOL xeeWrite(BYTE val)
{
    EEPROMBuffer[vBytesInBuffer++] = val;
    if(vBytesInBuffer >= sizeof(EEPROMBuffer))
        DoWrite();
    else if((((BYTE)EEPROMAddress + vBytesInBuffer) & (XEEPROM_PAGE_SIZE-1)) == 0u)
        DoWrite();

    return TRUE;
}


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
void xeeWriteArray(BYTE *val, WORD wLen)
{
    while(wLen--)
        xeeWrite(*val++);

    xeeEndWrite();
}


/**
 * Commits any last uncommitted bytes in cache to physical storage. Call this function
 * when you no longer need to write any more bytes at the selected address.
 *
 * @preCondition xeeInit() && xeeBeginWrite() are already called.
 *
 * @return TRUE if successful, else FALSE
 */
BOOL xeeEndWrite(void)
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
    #if defined(USE_EEPROM_25LC1024)
    EEPROM_SSPBUF = ((DWORD_VAL*)&EEPROMAddress)->v[2];
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;
    #endif

    EEPROM_SSPBUF = ((DWORD_VAL*)&EEPROMAddress)->v[1];
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;

    EEPROM_SSPBUF = ((DWORD_VAL*)&EEPROMAddress)->v[0];
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


/**
 * Indicates if the external EEPROM is busy
 *
 * @return TRUE if busy, FALSE if available
 */
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


#endif //#if defined(EEPROM_CS_TRIS)
