/*********************************************************************
 * spiFlashWinbond.c  -    Functions for accessing an external SPI Winbond FLASH
 * Dependencies:  
 * Processor:     PIC24 or PIC33
 * Complier:      MCC30 v3.23 or higher (Tested on this version, lower versions might also work)
 * Company:       Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description - Functions for accessing an external SPI FLASH
 *
 *********************************************************************
 * File History
 *
 * 2010-10-15, David Hosken (DH):
 *  - Initial version, modified version of Microchip's spiFlash.c file from their TCP/IP stack
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
#define __SPI_FLASH_WINBOND_C

#include "HardwareProfile.h"
#include "spiFlash.h"

#if defined(SPIFLASH_CS_TRIS)

#define READ				0x03    // SPI Flash opcode: Read up up to 25MHz
#define READ_FAST			0x0B    // SPI Flash opcode: Read up to 50MHz with 1 dummy byte
#define ERASE_4K			0x20    // SPI Flash opcode: 4KByte sector erase
#define ERASE_32K			0x52    // SPI Flash opcode: 32KByte block erase
#define ERASE_SECTOR		0xD8    // SPI Flash opcode: 64KByte block erase
#define ERASE_ALL			0x60    // SPI Flash opcode: Entire chip erase
#define WRITE				0x02    // SPI Flash opcode: Write one byte (or a page of up to 256 bytes, depending on device)
#define WRITE_WORD_STREAM	0xAD    // SPI Flash opcode: Write continuous stream of 16-bit words (AAI mode); available on SST25VF016B (but not on SST25VF010A)
#define WRITE_BYTE_STREAM	0xAF    // SPI Flash opcode: Write continuous stream of bytes (AAI mode); available on SST25VF010A (but not on SST25VF016B)
#define RDSR				0x05    // SPI Flash opcode: Read Status Register
#define EWSR				0x50    // SPI Flash opcode: Enable Write Status Register
#define WRSR				0x01    // SPI Flash opcode: Write Status Register
#define WREN				0x06    // SPI Flash opcode: Write Enable
#define WRDI				0x04    // SPI Flash opcode: Write Disable / End AAI mode
#define RDID				0x90    // SPI Flash opcode: Read ID
#define JEDEC_ID			0x9F    // SPI Flash opcode: Read JEDEC ID
#define EBSY				0x70    // SPI Flash opcode: Enable write BUSY status on SO pin
#define DBSY				0x80    // SPI Flash opcode: Disable write BUSY status on SO pin

#define BUSY                0x01    // Mask for Status Register BUSY (Internal Write Operaiton in Progress status) bit
#define WEL                 0x02    // Mask for Status Register WEL (Write Enable status) bit
#define BP0                 0x04    // Mask for Status Register BP0 (Block Protect 0) bit
#define BP1                 0x08    // Mask for Status Register BP1 (Block Protect 1) bit
#define BP2                 0x10    // Mask for Status Register BP2 (Block Protect 2) bit
#define BP3                 0x20    // Mask for Status Register BP3 (Block Protect 3) bit
#define AAI                 0x40    // Mask for Status Register AAI (Auto Address Increment Programming status) bit
#define BPL                 0x80    // Mask for Status Register BPL (BPx block protect bit read-only protect) bit

#define MNFR_ID_WINBOND     0xEF
#define MNFR_ID_SST         0xBF

static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
{
    while ((SPIFLASH_SPISTATbits.SPITBF == 1) || (SPIFLASH_SPISTATbits.SPIRBF == 0));
}

// Internal pointer to address being written
static DWORD dwWriteAddr;

static WORD deviceID;
static BYTE manufactID;     //Flash Manufacturer ID
static BYTE Dummy;

static BYTE initClearProtection;

static void _SendCmd(BYTE cmd);
static void _WaitWhileBusy(void);
//static void _GetStatus(void);
static void SPIFlashClearWriteProtection(void);


/*****************************************************************************
  Function:
    void spiFlashInit(void)

  Description:
    Initializes SPI Flash module.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is only called once during the lifetime of the application.

  Internal:
    This function sends WRDI to clear any pending write operation, and also
    clears the software write-protect on all memory locations.
  ***************************************************************************/
void spiFlashInit(void) {
    //volatile BYTE Dummy;
    //BYTE i;

    initClearProtection = 0;

    // Read Device ID code to determine supported device capabilities/instructions
    {
        // Activate chip select
        SPIFLASH_CS_IO = 0;

        // Send instruction
        SPIFLASH_SSPBUF = JEDEC_ID;
        WaitForDataByte();
        Dummy = SPIFLASH_SSPBUF;

        // Read Manufacture ID
        SPIFLASH_SSPBUF = 0x00;
        WaitForDataByte();
        manufactID = SPIFLASH_SSPBUF;

        // Read Device ID Bits upper bits
        SPIFLASH_SSPBUF = 0x00;
        WaitForDataByte();
        deviceID = SPIFLASH_SSPBUF << 8; //Get Device ID

        // Read Device ID lower upper bits
        SPIFLASH_SSPBUF = 0x00;
        WaitForDataByte();
        deviceID |= (((WORD) SPIFLASH_SSPBUF) & 0xff); //Get Device ID

        // Deactivate chip select
        SPIFLASH_CS_IO = 1;

        //Winbond Flash
        if (manufactID != MNFR_ID_WINBOND)
            while (1); //Flash chip not supported!
    }

    // Clear any pre-existing AAI write mode
    // This may occur if the PIC is reset during a write, but the Flash is
    // not tied to the same hardware reset.
    _SendCmd(WRDI);


    /* Move this to code where we have to write to Flash. Only do when writing to flash
    //Check if board has older W25X32 chip. This chip does not have the Enable-Write-Status-Register
    //instruction, and can not write volatile values to status registers. Only Non-Volatile write to
    //status register is possible, and must be preceded by WREN. Also check if write done.
    if (deviceID == 0x3016) {
        //For WINBOND flash
	    // Enable writing command required to write to Status register on some Winbond flash chips
	    _SendCmd(WREN);
    }
    //Assume this is a newer chip with Enable-Write-Status-Register (EWSR) instruction = 0x50
    else {
        // Execute Enable-Write-Status-Register (EWSR) instruction
        _SendCmd(EWSR);
    }

    // Clear Write-Protect on all memory locations
    SPIFLASH_CS_IO = 0;
    SPIFLASH_SSPBUF = WRSR;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;
    SPIFLASH_SSPBUF = 0x00; // Clear all block protect bits
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;
    SPIFLASH_CS_IO = 1;

    //Check if board has older W25X32 chip. If so, we have to wait for non-volatile write to status register to complete
    if (deviceID == 0x3016) {
        //For Winbond devices without "Status Register Write Enable" command, wait while status write is busy
	    // After writing to the status register, we have to wait for the write to complete
	    _WaitWhileBusy();
    }
    */
}


/*****************************************************************************
  Function:
    void spiFlashReadArray(DWORD dwAddress, BYTE *vData, WORD wLength)

  Description:
    Reads an array of bytes from the SPI Flash module.

  Precondition:
    spiFlashInit has been called, and the chip is not busy (should be
    handled elsewhere automatically.)

  Parameters:
    dwAddress - Address from which to read
    vData - Where to store data that has been read
    wLength - Length of data to read

  Returns:
    None
  ***************************************************************************/
void spiFlashReadArray(DWORD dwAddress, BYTE *vData, WORD wLength)
{

    // Ignore operations when the destination is NULL or nothing to read
    /* //Skip checks to reduce code size
    if(vData == NULL || wLength == 0)
        return;
    */

    // Activate chip select
    SPIFLASH_CS_IO = 0;
    
    // Send READ opcode
    SPIFLASH_SSPBUF = READ;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Send address
    SPIFLASH_SSPBUF = ((BYTE*)&dwAddress)[2];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddress)[1];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddress)[0];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Read data
    while(wLength--)
    {
        SPIFLASH_SSPBUF = 0;
        WaitForDataByte();
        *vData++ = SPIFLASH_SSPBUF;
    }

    // Deactivate chip select
    SPIFLASH_CS_IO = 1;
}

/*****************************************************************************
  Function:
    void spiFlashBeginWrite(DWORD dwAddr)

  Summary:
    Prepares the SPI Flash module for writing.

  Description:
    Prepares the SPI Flash module for writing.  Subsequent calls to
    spiFlashWrite or spiFlashWriteArray will begin at this location and
    continue sequentially.

    SPI Flash

  Precondition:
    spiFlashInit has been called.

  Parameters:
    dwAddr - Address where the writing will begin

  Returns:
    None

  Remarks:
    Flash parts have large sector sizes, and can only erase entire sectors
    at once.  The Winbond parts for which this library was written have sectors
    that are 4kB in size.  Your application must ensure that writes begin on
    a sector boundary so that the spiFlashWrite functions will erase the
    sector before attempting to write.  Entire sectors need not be written
    at once, so applications can begin writing to the front of a sector,
    perform other tasks, then later call spiFlashBeginWrite and point to an
    address in this sector that has not yet been programmed.  However, care
    must taken to ensure that writes are not attempted on addresses that are
    not in the erased state.  The chip will provide no indication that the
    write has failed, and will silently ignore the command.

    !!!!!!!!!!!!!!    WINBOND - IMPORTANT   !!!!!!!!!!!!
    Writing to the WINBOND Flash is disabled for up to 10ms after power up. During
    this time, the CS should be high!
  ***************************************************************************/
void spiFlashBeginWrite(DWORD dwAddr) {
    dwWriteAddr = dwAddr;

    //Ensure write protection bits are not set
    SPIFlashClearWriteProtection();
}

/*****************************************************************************
  Function:
    void spiFlashWrite(BYTE vData)

  Summary:
    Writes a byte to the SPI Flash part.

  Description:
    This function writes a byte to the SPI Flash part.  If the current
    address pointer indicates the beginning of a 4kB sector, the entire
    sector will first be erased to allow writes to proceed.  If the current
    address pointer indicates elsewhere, it will be assumed that the sector
    has already been erased.  If this is not true, the chip will silently
    ignore the write command.

  Precondition:
    spiFlashInit and spiFlashBeginWrite have been called, and the current
    address is either the front of a 4kB sector or has already been erased.

  Parameters:
    vData - The byte to write to the next memory location.

  Returns:
    None

  Remarks:
    See Remarks in spiFlashBeginWrite for important information about Flash
    memory parts.

    !!!!!!!!!!!!!!    WINBOND - IMPORTANT   !!!!!!!!!!!!
    Writing to the WINBOND Flash is disabled for up to 10ms after power up. During
    this time, the CS should be high!
  ***************************************************************************/
/* Not implemented to save space
void spiFlashWrite(BYTE vData)
{
    // If address is a boundary, erase a sector first
    if((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0u)
        spiFlashEraseSector(dwWriteAddr);

    // Enable writing
    _SendCmd(WREN);

    // Activate the chip select
    SPIFLASH_CS_IO = 0;

    // Issue WRITE command with address
    SPIFLASH_SSPBUF = WRITE;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwWriteAddr)[2];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwWriteAddr)[1];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwWriteAddr)[0];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Write the byte
    SPIFLASH_SSPBUF = vData;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;
    dwWriteAddr++;

    // Deactivate chip select and wait for write to complete
    SPIFLASH_CS_IO = 1;
    _WaitWhileBusy();
}
*/

/*****************************************************************************
  Function:
    void spiFlashWriteArray(BYTE* vData, WORD wLen)

  Summary:
    Writes an array of bytes to the SPI Flash part.

  Description:
    This function writes an array of bytes to the SPI Flash part.  If the current
    address pointer indicates an address that is not a sector boundary and is not
    already erased, the chip will silently ignore the write command until the
    next sector boundary is crossed.

  Precondition:
    spiFlashInit and spiFlashBeginWrite have been called, and the current
    address is either the front of a sector or has already been erased.

  Parameters:
    vData - The array to write to the next memory location
    wLen - The length of the data to be written

  Returns:
    None

  Remarks:
    See Remarks in spiFlashBeginWrite for important information about Flash
    memory parts.

    !!!!!!!!!!!!!!    WINBOND - IMPORTANT   !!!!!!!!!!!!
    Writing to the WINBOND Flash is disabled for up to 10ms after power up. During
    this time, the CS should be high!
  ***************************************************************************/
void spiFlashWriteArray(BYTE* vData, WORD wLen) {
    // Do nothing if no data to process
    /*
        //Skip checks to reduce code size
        if(wLen == 0u)
            return;
     */

    if (manufactID == MNFR_ID_WINBOND) {

        // Loop over all data to be written
        while (wLen) {
            /*  !!!!! IMPORTANT !!!!!
             * Disable automatic erasing of sectors for Bootloader app!!!
            // If address is a sector boundary, erase a sector first
            if ((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0)
                spiFlashEraseSector(dwWriteAddr);
            */

            // Enable writing
            _SendCmd(WREN);

            // Activate the chip select
            SPIFLASH_CS_IO = 0;

            // Issue WRITE command with address
            SPIFLASH_SSPBUF = WRITE;
            WaitForDataByte();
            Dummy = SPIFLASH_SSPBUF;

            SPIFLASH_SSPBUF = ((BYTE*) & dwWriteAddr)[2];
            WaitForDataByte();
            Dummy = SPIFLASH_SSPBUF;

            SPIFLASH_SSPBUF = ((BYTE*) & dwWriteAddr)[1];
            WaitForDataByte();
            Dummy = SPIFLASH_SSPBUF;

            SPIFLASH_SSPBUF = ((BYTE*) & dwWriteAddr)[0];
            WaitForDataByte();
            Dummy = SPIFLASH_SSPBUF;

            // TODO
            //Calculate how many bytes must be written, and just do that in loop. Update dwWriteAddr and wLen after loop.
            //Write the bytes, up to a page boundary
            do {
                SPIFLASH_SSPBUF = *vData++;
                WaitForDataByte();
                Dummy = SPIFLASH_SSPBUF;
                dwWriteAddr++;
                wLen--;
            } while (wLen && (dwWriteAddr & SPI_FLASH_PAGE_MASK));

            // Deactivate chip select and wait for write to complete
            SPIFLASH_CS_IO = 1;
            _WaitWhileBusy();
        }
    }
    /*  //Only support WINBOND Flash for this file
    else {
        // If starting at an odd address, write a single byte
        if ((dwWriteAddr & 0x01) && wLen) {
            spiFlashWrite(*vData);
            vData++;
            wLen--;
        }

        // Assume we are using AAI Word program mode unless changed later
        vOpcode = WRITE_WORD_STREAM;

        isStarted = FALSE;

        // Loop over all remaining WORDs
        while (wLen > 1) {
            // Don't do anything until chip is ready
            _WaitWhileBusy();

            // If address is a sector boundary
            if ((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0)
                spiFlashEraseSector(dwWriteAddr);

            // If not yet started, initiate AAI mode
            if (!isStarted) {
                // Enable writing
                _SendCmd(WREN);

                // Select appropriate programming opcode.  The WRITE_WORD_STREAM
                // mode is the default if neither of these flags are set.
                if (deviceCaps.bits.bWriteByteStream)
                    vOpcode = WRITE_BYTE_STREAM;
                else if (deviceCaps.bits.bPageProgram) {
                    // Note: Writing one byte at a time is extremely slow (ex: ~667
                    // bytes/second write speed on SST SST25VF064C).  You can
                    // improve this by over a couple of orders of magnitude by
                    // writing a function to write full pages of up to 256 bytes at
                    // a time.  This is implemented this way only because I don't
                    // have an SST25VF064C handy to test with right now. -HS
                    while (wLen--)
                        spiFlashWrite(*vData++);
                    return;
                }

                // Activate the chip select
                SPIFLASH_CS_IO = 0;

                // Issue WRITE_xxx_STREAM command with address
                SPIFLASH_SSPBUF = vOpcode;
                WaitForDataByte();
                Dummy = SPIFLASH_SSPBUF;

                SPIFLASH_SSPBUF = ((BYTE*) & dwWriteAddr)[2];
                WaitForDataByte();
                Dummy = SPIFLASH_SSPBUF;

                SPIFLASH_SSPBUF = ((BYTE*) & dwWriteAddr)[1];
                WaitForDataByte();
                Dummy = SPIFLASH_SSPBUF;

                SPIFLASH_SSPBUF = ((BYTE*) & dwWriteAddr)[0];
                WaitForDataByte();
                Dummy = SPIFLASH_SSPBUF;

                isStarted = TRUE;
            }                // Otherwise, just write the AAI command again
            else {
                // Assert the chip select pin
                SPIFLASH_CS_IO = 0;

                // Issue the WRITE_STREAM command for continuation
                SPIFLASH_SSPBUF = vOpcode;
                WaitForDataByte();
                Dummy = SPIFLASH_SSPBUF;
            }

            // Write a byte or two
            for (i = 0; i <= deviceCaps.bits.bWriteWordStream; i++) {
                SPIFLASH_SSPBUF = *vData++;
                dwWriteAddr++;
                wLen--;
                WaitForDataByte();
                Dummy = SPIFLASH_SSPBUF;
            }

            // Release the chip select to begin the write
            SPIFLASH_CS_IO = 1;

            // If a boundary was reached, end the write
            if ((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0) {
                _WaitWhileBusy();
                _SendCmd(WRDI);
                isStarted = FALSE;
            }
        }

        // Wait for write to complete, then exit AAI mode
        _WaitWhileBusy();
        _SendCmd(WRDI);

        // If a byte remains, write the odd address
        if (wLen)
            spiFlashWrite(*vData);
    }
    */
}


/*****************************************************************************
  Function:
    void spiFlashEraseSector(DWORD dwAddr)

  Summary:
    Erases a sector.

  Description:
    This function erases a sector in the Flash part.  It is called
    internally by the spiFlashWrite functions whenever a write is attempted
    on the first byte in a sector.

  Precondition:
    spiFlashInit has been called.

  Parameters:
    dwAddr - The address of the sector to be erased.

  Returns:
    None

  Remarks:
    See Remarks in spiFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void spiFlashEraseSector(DWORD dwAddr)
{
    //Ensure write protection bits are not set
    SPIFlashClearWriteProtection();
	
    // Enable writing
    _SendCmd(WREN);

    // Activate the chip select
    SPIFLASH_CS_IO = 0;

    // Issue ERASE command with address
    SPIFLASH_SSPBUF = ERASE_4K;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddr)[2];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddr)[1];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddr)[0];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Deactivate chip select to perform the erase
    SPIFLASH_CS_IO = 1;

    // Wait for erase to complete
    _WaitWhileBusy();
}


/*****************************************************************************
  Function:
    static void _SendCmd(BYTE cmd)

  Summary:
    Sends a single-byte command to the SPI Flash part.

  Description:
    This function sends a single-byte command to the SPI Flash part.  It is
    used for commands such as WREN, WRDI, and EWSR that must have the chip
    select activated, then deactivated immediately after the command is
    transmitted.

  Precondition:
    spiFlashInit has been called.

  Parameters:
    cmd - The single-byte command code to send

  Returns:
    None
  ***************************************************************************/
static void _SendCmd(BYTE cmd)
{
    // Activate chip select
    SPIFLASH_CS_IO = 0;

    // Send instruction
    SPIFLASH_SSPBUF = cmd;
    WaitForDataByte();
    cmd = SPIFLASH_SSPBUF;

    // Deactivate chip select
    SPIFLASH_CS_IO = 1;
}


/*****************************************************************************
  Function:
    static void _WaitWhileBusy(void)

  Summary:
    Waits for the SPI Flash part to indicate it is idle.

  Description:
    This function waits for the SPI Flash part to indicate it is idle.  It is
    used in the programming functions to wait for operations to complete.

  Precondition:
    spiFlashInit has been called.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/
static void _WaitWhileBusy(void)
{
    // Activate chip select
    SPIFLASH_CS_IO = 0;

    // Send Read Status Register instruction
    SPIFLASH_SSPBUF = RDSR;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Poll the BUSY bit
    do
    {
        SPIFLASH_SSPBUF = 0x00;
        WaitForDataByte();
        Dummy = SPIFLASH_SSPBUF;
    } while(Dummy & BUSY);

    // Deactivate chip select
    SPIFLASH_CS_IO = 1;
}

/*****************************************************************************
  Function:
    static void _GetStatus()

  Summary:
    Reads the status register of the part.

  Description:
    This function reads the status register of the part.  It was written
    for debugging purposes, and is not needed for normal operation.  Place
    a breakpoint at the last instruction and check the "status" variable to
    see the result.

  Precondition:
    spiFlashInit has been called.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/
//static void _GetStatus()
//{
//	volatile BYTE Dummy;
//  static BYTE statuses[16];
//  static BYTE *status = statuses;
//
//  // Activate chip select
//  SPIFLASH_CS_IO = 0;
//
//  // Send Read Status Register instruction
//  SPIFLASH_SSPBUF = RDSR;
//  WaitForDataByte();
//  Dummy = SPIFLASH_SSPBUF;
//
//  SPIFLASH_SSPBUF = 0x00;
//  WaitForDataByte();
//  *status = SPIFLASH_SSPBUF;
//  status++;
//
//  // Deactivate chip select
//  SPIFLASH_CS_IO = 1;
//
//  if(status == &statuses[10])
//      statuses[15] = 0;
//}

/**
 * Check write protection bits are not set. Enable device for writing or erasing.
 */
void SPIFlashClearWriteProtection() {
    volatile BYTE Dummy;

    //Clear Write-Protection bits
    if (initClearProtection == 0) {
        initClearProtection = 1;

        //Check if board has older W25X32 chip. This chip does not have the Enable-Write-Status-Register
        //instruction, and can not write volatile values to status registers. Only Non-Volatile write to
        //status register is possible, and must be preceded by WREN. Also check if write done.
        if (deviceID == 0x3016) {
            //For WINBOND flash
            // Enable writing command required to write to Status register on some Winbond flash chips
            _SendCmd(WREN);
        }            //Assume this is a newer chip with Enable-Write-Status-Register (EWSR) instruction = 0x50
        else {
            // Execute Enable-Write-Status-Register (EWSR) instruction
            _SendCmd(EWSR);
        }

        // Clear Write-Protect on all memory locations
        SPIFLASH_CS_IO = 0;
        SPIFLASH_SSPBUF = WRSR;
        WaitForDataByte();
        Dummy = SPIFLASH_SSPBUF;
        SPIFLASH_SSPBUF = 0x00; // Clear all block protect bits
        WaitForDataByte();
        Dummy = SPIFLASH_SSPBUF;
        SPIFLASH_CS_IO = 1;

        //Check if board has older W25X32 chip. If so, we have to wait for non-volatile write to status register to complete
        if (deviceID == 0x3016) {
            //For Winbond devices without "Status Register Write Enable" command, wait while status write is busy
            // After writing to the status register, we have to wait for the write to complete
            _WaitWhileBusy();
        }
    }
}

#endif //#if defined(SPIFLASH_CS_TRIS)

