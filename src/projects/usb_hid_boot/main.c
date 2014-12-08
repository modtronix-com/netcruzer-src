/********************************************************************
 FileName:     main.c
 Dependencies: See INCLUDES section
 Processor:                PIC24 USB Microcontrollers
 Hardware:                The code is natively intended to be used on the following
                                 hardware platforms:
                                 Explorer 16 + PIC24FJ256GB110 PIM.  The firmware may be
                                 modified for use on other USB platforms by editing the
                                 HardwareProfile.h file.
 Complier:          Microchip C30 (for PIC24)
 Company:                Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the ?Company?) for its PIC® Microcontroller is intended and
 supplied to you, the Company?s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN ?AS IS? CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:

 2014-07-24, David H. (DH):
    - Changed WDT from 32 to 4 seconds
 20??-??-??
    - Adapted from PIC18F87J50 HID Bootloader Firmware as basis for
	  BootApplication().  The rest of the code was taken from the
	  Simple HID Demo in MCHPFSUSB v2.2.
 20??-??-??
	- Fixed race condition where an OUT packet could potentially
      clear the prior IN packet depending on the bus communication order.

      Removed ability to reflash the interrupt vectors.  This has
      been depricated for a interrupt remapping scheme documented
      in the migration notes and in the getting started for the HID bootloader.

********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

/*
 * This macro uniquely defines this file as the main entry point.
 * There should only be one such definition in the entire project,
 * and this file must define the AppConfig variable as described below.
 */
#define THIS_IS_BOOTLOADER_APPLICATION


/** INCLUDES *******************************************************/
#include <PPS.h>
#include <ports.h>

#include "HardwareProfile.h"

//For some reason MPLAB X only parses "GenericTypeDefs.h" if we #undef __GENERIC_TYPE_DEFS_H_
//#undef __GENERIC_TYPE_DEFS_H_

#define DONT_DEFINE_PACKET_TO_FROM_PC
#include "nz_usbHidDefs.h"

#include "./USB/usb.h"
#include "./USB/usb_function_hid.h"

#include "spiEEProm.h"
#include "spiFlash.h"


//#define DEBUGGING


/////////////////////////////////////////////////
//Firmware Configuration, located in hex file
/////////////////////////////////////////////////
//The external EEPROM is used for configiration data. This data is stored in the "FirmwareX Configuration" section of the external FLASH.
//Under certian conditions, the "User Program" will read this data, and program the External EEPROM. These conditions include when new
//firmware is uploaded to the board, or if the configuration data in the external EEPROM becomes corrupt.
//This data is contained in the hex file from Word address 0x800000 (byte address 0x1000000). When we receive data located in
//this address range, it has to be stored in the "FirmwareX Configuration" section of the external FLASH.
//Only odd addresses in the hex file contains data. Each odd address contains 2 bytes of data, even addresses do NOT contain any data.
#define XEEPROM_CONF_START                   0x00800000  //Beginning of "External EEPROM" memory, Word address. Byte address = 0x1000000. This is the address as given in hex file
#define XEEPROM_CONF_END                     0x00802000  //End of "External EEPROM" memory. Byte address = 0x1004000. This WORD address does not get updated, but the one just below it does:



/////////////////////////////////////////////////
//Firmware, located in hex file
/////////////////////////////////////////////////
//Defines the address ranges of sections of code in the hex file. Data in hex file is not packed, but is packed in External Flash:
// - Program Memory located in hex file from 0x3800 (0x1C00 x 2) to 54FFF (0x2A800 x 2 - 1) is packed in External flash (3 bytes for each 4
//   in hex file) at XFLASH_FIRMWAREx_ADR
#define VECTORS_START                       0x00000000
#define VECTORS_END                         0x00000400  //One page of vectors + general purpose program memory. Word Address
//Bootloader resides in memory range 0x400-0x1BFF
#define PROGRAM_MEM_START                   0x00001C00  //Beginning of "User Program" program memory (Word Address) = not occupied by bootloader.  **THIS VALUE MUST BE ALIGNED WITH BLOCK BOUNDRY** Also, in order to work correctly, make sure the BEGIN_PAGE_TO_ERASE is set to erase this section.

//Flash Page = hex address / 0x400
#if defined(__PIC24FJ256GB210__) || defined(__PIC24FJ256GB206__)
    #define PROGRAM_MEM_WR_ROW_SIZE         128         //For PIC24F family, 128 (0x80) Words can be written at a time. This is 128 x 1.5 = 192 bytes. Upper byte of each odd address Word is not implemented
    #define PROGRAM_MEM_WR_ROW_MASK         (PROGRAM_MEM_WR_ROW_SIZE-1)

    #define PROGRAM_MEM_ERASE_BLOCK_SIZE    1024        //Smallest number of words that can be erased. To get bytes x 1.5
    #define PROGRAM_MEM_ERASE_BLOCK_MASK    (PROGRAM_MEM_ERASE_BLOCK_SIZE-1)

    //#define BEGIN_PAGE_TO_ERASE             5         //Bootloader and vectors occupy first six 1024 word (1536 bytes due to 25% unimplemented bytes) pages
    #define BEGIN_PAGE_TO_ERASE             7           //Bootloader and vectors occupy first seven 1024 word (1536 bytes due to 25% unimplemented bytes) pages
    #define MAX_PAGE_TO_ERASE_NO_CONFIGS    169         //Last full page of flash on the PIC24FJ256GB110, which does not contain the flash configuration words.
    #define MAX_PAGE_TO_ERASE_WITH_CONFIGS  170         //Page 170 contains the flash configurations words on the PIC24FJ256GB110.  Page 170 is also smaller than the rest of the (1536 byte) pages.
    #define PROGRAM_MEM_STOP_NO_CONFIGS     0x0002A800  //Must be instruction word aligned address.  This WORD address does not get updated, but the one just below it does:
                                                        //IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)

    #define PROGRAM_MEM_STOP_WITH_CONFIGS   0x0002ABF8  //Must be instruction word aligned address.  This WORD address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)
    #define CONFIG_WORDS_START_ADDRESS      0x0002ABF8  //0x2ABFA is start of CW3 on PIC24FJ256GB110 Family devices
    #define CONFIG_WORDS_STOP_ADDRESS       0x0002AC00
#elif defined(__PIC24FJ128GB210__) || defined(__PIC24FJ128GB206__) || defined(__PIC24FJ128GB106__)
    #define PROGRAM_MEM_WR_ROW_SIZE         128         //For PIC24F family, 128 (0x80) Words can be written at a time. This is 128 x 1.5 = 192 bytes. Upper byte of each odd address Word is not implemented
    #define PROGRAM_MEM_WR_ROW_MASK         (PROGRAM_MEM_WR_ROW_SIZE-1)

    #define PROGRAM_MEM_ERASE_BLOCK_SIZE    1024        //Smallest number of words that can be erased. To get bytes x 1.5
    #define PROGRAM_MEM_ERASE_BLOCK_MASK    (PROGRAM_MEM_ERASE_BLOCK_SIZE-1)

    //#define BEGIN_PAGE_TO_ERASE             5         //Bootloader and vectors occupy first six 1024 word (1536 bytes due to 25% unimplemented bytes) pages
    #define BEGIN_PAGE_TO_ERASE             7           //Bootloader and vectors occupy first six 1024 word (1536 bytes due to 25% unimplemented bytes) pages
    #define MAX_PAGE_TO_ERASE_NO_CONFIGS    84          //Last full page of flash on the PIC24FJ128GB110, which does not contain the flash configuration words.
    #define MAX_PAGE_TO_ERASE_WITH_CONFIGS  85          //Page 85 contains the flash configurations words on the PIC24FJ128GB110.  Page 85 is also smaller than the rest of the (1536 byte) pages.
    #define PROGRAM_MEM_STOP_NO_CONFIGS     0x00015400  //Must be instruction word aligned address.  This address does not get updated, but the one just below it does:
                                                        //IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)

    #define PROGRAM_MEM_STOP_WITH_CONFIGS   0x000157F8  //Must be instruction word aligned address.  This address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)
    #define CONFIG_WORDS_START_ADDRESS      0x000157F8  //0x157FA is start of CW3 on PIC24FJ128GB110 Family devices
    #define CONFIG_WORDS_STOP_ADDRESS       0x00015800
#else
    #error "This bootloader only covers the PIC24FJ256GB210 family devices.  Please see another folder for the bootloader appropriate for the selected device."
#endif



/////////////////////////////////////////////////
//External FLASH
/////////////////////////////////////////////////
//FirmwareA and FirmwareB contains the Microcontroller's program memory, in a Packed form! Each 3 bytes = one 2 Word Instructions.
//The last 8 bytes of the XFLASH_FIRMWAREX_CONFIG and XFLASH_MODIFIED_CONFIG will be used by the bootloader. It will have
//a checksum, and a magic number. If the magic number is wrong, it is not valid. When new firmware is downloaded, the
//XFLASH_FIRMWAREX_CONFIG is updated with data from new firmware, and after bootloader validates it, the magic number is
//set. This ensure it is only set as "valid" after validation. During the "User Program", if the user changes configuration,
//and decided to save it as the new default configuration, then it is written to the XFLASH_MODIFIED_CONFIG area,
//and the XFLASH_FIRMWAREX_CONFIG area is marked as invalid. If the user saved new default config, it is now written to
//XFLASH_FIRMWAREX_CONFIG, and XFLASH_MODIFIED_CONFIG area is marked as invalid.

#define XFLASH_FIRMWAREA_INFO_ADR           0           //Byte address of start of "FirmwareA Info"
#define XFLASH_FIRMWAREA_ADR                0x1000      //Byte address of start of "FirmwareA"
#define XFLASH_FIRMWAREA_CONFIG             0x40000     //Byte address of start of "FirmwareA Configuration"

#define XFLASH_FIRMWAREB_INFO_ADR           0x42000     //Byte address of start of "FirmwareB Info"
#define XFLASH_FIRMWAREB_ADR                0x43000     //Byte address of start of "FirmwareB"
#define XFLASH_FIRMWAREB_CONFIG             0x82000     //Byte address of start of "FirmwareB Configuration"

#define XFLASH_MODIFIED_CONFIG              0x84000

#define XFLASH_FIRMWARE_SIZE_IN_SECTORS     63          //Indicates size of FirmwareA and FirmwareB areas in 4096 byte sectors = 63x4096 = 258,048 Bytes
#define XFLASH_FIRMWARE_CONFIG_SIZE_IN_SECTORS  2       //Indicates size of "FirmwareA Config" and "FirmwareB Config" in 4096 byte sectors = 2x4096 = 8k
#define XFLASH_SECTOR_SIZE                  4096        //Byte size of a sector of external FLASH. A sector is the smallest area that can be erased.

//Byte address of start of "User Space". User space is followed by File System, which is located at address given by MPFS_RESERVE_BLOCK
//define in main project.
#define XFLASH_USER_SPACE_ADR               0x86000



/////////////////////////////////////////////////
//External FLASH "User Space", located in hex file
/////////////////////////////////////////////////
//It is possible programming the "User Space" in the external flash with data located in the hex file.
//This data is contained in the hex file from Word address 0x820000 (byte address 0x1040000). When we receive data located in
//this address range, it is programmed to the "User Space" area of the external FLASH. Maximum size is 0xE0000 Words = 1,835,008 Bytes
//Only odd addresses in the hex file contains data. Each odd address contains 2 bytes of data, even addresses do NOT contain any data.
#define XFLASH_USERSPACE_START              0x00820000  //Beginning of "External FLASH User Space" memory, Word address. Byte address = 0x1040000. This is the address as given in hex file
#define XFLASH_USERSPACE_END                0x00900000  //End of "External FLASH User Space" memory. Word address. Byte address = 0x1200000. This address does not get updated, but the one just below it does.



/////////////////////////////////////////////////
//General defines
/////////////////////////////////////////////////
//Old Original commands
#define QUERY_DEVICE          0x02        //Command that the host uses to learn about the device (what regions can be programmed, and what type of memory is the region)
//#define UNLOCK_CONFIG         0x03        //Note, this command is used for both locking and unlocking the config bits (see the "//Unlock Configs Command Definitions" below)
//#define ERASE_DEVICE          0x04        //Host sends this command to start an erase operation.  Firmware controls which pages should be erased.
//#define PROGRAM_DEVICE        0x05        //If host is going to send a full RQST_DATA_BLOCK_SIZE to be programmed, it uses this command.
//#define PROGRAM_COMPLETE      0x06        //If host send less than a RQST_DATA_BLOCK_SIZE to be programmed, or if it wished to program whatever was left in the buffer, it uses this command.
//#define GET_DATA              0x07        //The host sends this command in order to read out memory from the device.  Used during verify (and read/export hex operations)
//#define RESET_DEVICE          0x08        //Resets the microcontroller, so it can update the config bits (if they were programmed, and so as to leave the bootloader (and potentially go back into the "User Program")

#define DEVICE_INFO                 0x50
#define UNLOCK_CONFIG               0x51
#define RESET_DEVICE                0x52
#define ERASE_MEMORY                0x53        //Erase given memory
#define PROGRAM_VERIFIED            0x54        //Is sent after a Erase/Program/Verify sequence, if Verify was successful
#define READ_MEMORY                 0x80        //Read data from Memory, lower nibble gives memory type (MEMTYPE_ constant)
#define READ_MEMORY_PROGRAM         (READ_MEMORY | MEMTYPE_PROGRAM)
#define READ_MEMORY_EEPROM          (READ_MEMORY | MEMTYPE_EEPROM)
#define READ_MEMORY_CONFIG_WORDS    (READ_MEMORY | MEMTYPE_CONFIG_WORDS)
#define READ_MEMORY_XEEPROM         (READ_MEMORY | MEMTYPE_XEEPROM)
#define READ_MEMORY_XFLASH          (READ_MEMORY | MEMTYPE_XFLASH)
#define WRITE_MEMORY                0x90        //Write given data to Memory, lower nibble gives memory type (MEMTYPE_ constant)
#define WRITE_MEMORY_PROGRAM        (WRITE_MEMORY | MEMTYPE_PROGRAM)
#define WRITE_MEMORY_EEPROM         (WRITE_MEMORY | MEMTYPE_EEPROM)
#define WRITE_MEMORY_CONFIG_WORDS   (WRITE_MEMORY | MEMTYPE_CONFIG_WORDS)
#define WRITE_MEMORY_XEEPROM        (WRITE_MEMORY | MEMTYPE_XEEPROM)
#define WRITE_MEMORY_XFLASH         (WRITE_MEMORY | MEMTYPE_XFLASH)



//Unlock Configs Command Definitions
#define UNLOCKCONFIG                        0x00        //Sub-command for the ERASE_DEVICE command
#define LOCKCONFIG                          0x01        //Sub-command for the ERASE_DEVICE command

//Memory Types, MUST be a value from 0 to 15! Is used for lower nibble in some commands
#define MEMTYPE_PROGRAM                    0x01        //CPU Internal Program Memory
#define MEMTYPE_EEPROM                     0x02        //CPU Internal EEPROM
#define MEMTYPE_CONFIG_WORDS               0x03
#define MEMTYPE_XEEPROM                    0x04        //External EEPROM
#define MEMTYPE_XFLASH                     0x05        //External FLASH
#define MEMTYPE_END_OF_TYPE_LIST           0xFF        //Sort of serves as a "null terminator" like number, which denotes the end of the memory region list has been reached.

//BootState Variable States
#define IDLE_STATE                          0x00
#define NOT_IDLE_STATE                      0x01

//Other Constants
#define INVALID_ADDRESS                     0xFFFFFFFF

//Application and Microcontroller constants
#define BYTES_PER_FLASH_ADDRESS             0x02        //For Flash memory: One byte per address on PIC18, two bytes per address on PIC24

#define WORDSIZE                            0x02        //PIC18 uses 2 byte instruction words, PIC24 uses 3 byte "instruction words" (which take 2 addresses, since each address is for a 16 bit word; the upper word contains a "phantom" byte which is unimplemented.).
//#define        FlashBlockSize             0x40        //For PIC18F87J50 family devices, a flash block is 64 bytes

#define RQST_DATA_BLOCK_SIZE                56          //Number of bytes in the "Data" field of a standard request to/from the PC.  Must be an even number from 2 to 56.
#define BYTE_DATA_BLOCK_SIZE                58          //Number of bytes in the Byte Data field for read/write from/to commands
#define BUFFER_SIZE                         0x20        //32 16-bit words of buffer

//Jumpers
#define JMPR_PARK                   0
#define JMPR_CONFIG                 1
#define JMPR_FIRMWARE               2



/** PRIVATE PROTOTYPES *********************************************/
void blinkUSBStatus(void);
void usbCBSendResume(void);

void writeFlashRow(void);
void usbInit(void);
void EraseFlash(void);
void WriteFlashSubBlock(void);
void BootApplication(void);
DWORD ReadProgramMemory(DWORD);

void BootloaderTask(void);
void ConfigTask(void);

/**
 * Used for debugging external flash. Reads given address into 1024 byte buf[] array. Repeats given times.
 * @param startAdr
 * @param blocksOf1K
 */
void xflashDebug(DWORD startAdr, BYTE repeat);

/**
 * Updates firmware in external FLASH chip
 */
BYTE FlashFirmwareUpgrade(void);

void LEDTask(void);


/** T Y P E  D E F I N I T I O N S ************************************/

typedef union __attribute__ ((packed)) _USB_HID_BOOTLOADER_COMMAND
{
        unsigned char Contents[64];

        //Reply to DEVICE_INFO command, returns board ID and Revision
        struct __attribute__ ((packed)) {
            unsigned char Command;
            WORD          BoardID;          //A BOARD_ID value
            WORD          BoardRev;         //Board revision
        } DeviceInfo;


        //Structure used for Erasing given memory area
        struct __attribute__ ((packed)) {
            unsigned char Command;
            unsigned char MemoryType;       //A MEMTYPE_ Constant
            DWORD         StartAddress;     //First Byte address of memory area to erase. Is always a Byte address! If MEMTYPE is program memory, convert to Word address
            DWORD         EndAddress;       //Last Byte address of memory area to erase. This next byte should be the first byte of next sector (erase block)
        } EraseMemory;

        //Structure used for Reading and Writing given memory area
        //The lower nibble of the Command gives the memory type (MEMTYPE_ Constant)
        struct __attribute__ ((packed)) {
            unsigned char Command;
            DWORD Address;
            unsigned char Size;                             //Size of data contained in Data[] array
            unsigned char ByteData[BYTE_DATA_BLOCK_SIZE];   //Byte buffer, standard left justified (starts at offset 0)
        } MemoryRdWr;

        struct __attribute__ ((packed)) {
            unsigned char Command;
            WORD AddressHigh;
            WORD AddressLow;
            unsigned char Size;
            unsigned char PadBytes[(TOTAL_PACKET_SIZE - 6) - (RQST_DATA_BLOCK_SIZE)];
            unsigned int Data[RQST_DATA_BLOCK_SIZE/WORDSIZE];
        };

        struct __attribute__ ((packed)) {
            unsigned char Command;
            DWORD Address;
            unsigned char Size;
            unsigned char PadBytes[(TOTAL_PACKET_SIZE - 6) - (RQST_DATA_BLOCK_SIZE)];   //Pad 2 bytes so total size is 64 bytes
            unsigned int Data[RQST_DATA_BLOCK_SIZE/WORDSIZE];
        };

        //Reply to Query command, returns a "Section List" of this board's memory space
        struct __attribute__ ((packed)){
            unsigned char Command;
            unsigned char PacketDataFieldSize;
            unsigned char BytesPerAddress;
            unsigned char Type1;
            unsigned long Address1;
            unsigned long Length1;
            unsigned char Type2;
            unsigned long Address2;
            unsigned long Length2;
            unsigned char Type3;                //End of sections list indicator goes here, when not programming the vectors, in that case fill with 0xFF.
            unsigned long Address3;
            unsigned long Length3;
            unsigned char Type4;
            unsigned long Address4;
            unsigned long Length4;
            unsigned char Type5;
            unsigned long Address5;
            unsigned long Length5;
            unsigned char Type6;                //End of sections list indicator goes here, fill with 0xFF.
        };

        struct __attribute__ ((packed)){        //For lock/unlock config command
            unsigned char Command;
            unsigned char LockValue;
        };
} PacketToFromPC;

/**
 * Gives the current state of FirmwareA and FirmwareB. Possible conditions are:
 */
typedef union __attribute__ ((packed)) _FIRMWARE_STATE
{
    unsigned int Val;

    struct __attribute__ ((packed)){                                                //For lock/unlock config command
        unsigned char LB;
        unsigned char HB;
    };

    struct __attribute__ ((packed)) {
        unsigned int bNew : 1;      //This is new firmware, will be used to upgrade board on next power cycle
        unsigned int bCurrent : 1;  //This is the current firmware
        unsigned int bPrevious : 1; //This is the previous firmware. After upgrade, current firmware get's marked as Previous
        unsigned int bRestored : 1; //This is the current firmware, which has been restored. Changed state from Previous to Restored
        unsigned int bFaulty : 1;   //This is faulty firmware. Changed state from Current. This happens when system is restored with previous firmware
    };
} FIRMWARE_STATE;
#define FIRMWARE_STATE_NEW      0b11111111
#define FIRMWARE_STATE_CURRENT  0b11111110
#define FIRMWARE_STATE_PREVIOUS 0b11111100
#define FIRMWARE_STATE_RESTORED 0b11111000
#define FIRMWARE_STATE_FAULTY   0b11110000


/**
 * There are two copies of the firmware in Flash, FirmwareA and FirmwareB. Both have an
 * associated "Firmware Info" sector.
 *
 * The FirmwareA and FirmwareB both have a Firmware Info structure.
 * 0x0  - 0x7f contains information on the Firmware.
 * 0x80 - 0xfff contains the default configuation data for the firmware (external EEPROM).
 *
 * !!!!! IMPORTANT !!!!!
 * Ensure this structure is NOT larger than 128 bytes
 *
 */
typedef struct __attribute__ ((packed)) _FIRMWARE_INFO
{
    WORD_VAL    magicNumber;    //Must have the value 0x536A to be identified as valid data

    //The fwState variable is re-written after each change of firmware.
    //Initial value will be 0b1111111111111111 (New), which indicates this is new firmware. On power up, the
    //Bootloader will check if FirmwareA or FirmwareB is new, and if it is, upgrade firmware.
    //After programming board with new firmware, value is set to 0b1111111111111110 (Current). The
    //fwState of previous firmware is now set to 0b1111111111111100 (Previous). If the bootloader is
    //used to restore previous firmware, the current firmware is marked as Restored = 0b1111111111111000
    FIRMWARE_STATE    fwState;

    WORD    versionMinor;
    WORD    versionMajor;

    //Checksum's for "Program Memory" stored in External Flash
    DWORD   checksum1ProgMem;       //First checksum, is sum of all bytes
    DWORD   checksum2ProgMem;       //Two's complement type checksum

    //Checksum's for "External Eeprom" Memory stored in External Flash
    DWORD   checksum1XeeMem;       //First checksum, is sum of all bytes
    DWORD   checksum2XeeMem;       //Two's complement type checksum

    //Checksum's for "User Space" Memory stored in External Flash
    DWORD   checksum1XflashMem;       //First checksum, is sum of all bytes
    DWORD   checksum2XflashMem;       //Two's complement type checksum
} FIRMWARE_INFO;
#define FIRMWARE_INFO_MAGIC_NUMBER 0x536A


/** VARIABLES ******************************************************/
#pragma udata

#pragma udata USB_VARS
PacketToFromPC PacketFromPC;            //64 byte buffer for receiving packets on EP1 OUT from the PC
PacketToFromPC PacketToPC;              //64 byte buffer for sending packets on EP1 IN to the PC
PacketToFromPC PacketFromPCBuffer;

#pragma udata
USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;

//MODTRONIX removed, programming of last page containing configuration words is NOT supported. Is write protected
//unsigned char MaxPageToErase;

//MODTRONIX removed, programming of last page containing configuration words is NOT supported. Is write protected
//unsigned long ProgramMemStopAddress;

unsigned char BootState;
unsigned char ErasePageTracker;
unsigned int ProgrammingBuffer[BUFFER_SIZE];
unsigned char BufferedDataIndex;
#ifdef USB_WRITES_TO_PROGRAM_MEMORY
unsigned long ProgrammedPointer;    //Pointer indicating up to where we have programmed
#endif
unsigned char ConfigsProtected;

unsigned char jumpers;              //If 0, no jumper set, if 1 "C" is set, if 2 "F" is set.
unsigned int sysLedMask;            //Used for LED flashing pattern. Has 1 set bit, and it rotated left every 50ms.
//unsigned int sysLedPattern;       //Used for blinking system LED

unsigned int sysLedPattern;
unsigned int sysLedPatternH;        //For when 32 bit sysLedPattern is used
unsigned char sysLedUseH;           //When 0, do not use sysLedPatternH. LED pattern always has 10 or 16 states
unsigned char sysLedUseLorH;        //Only valid when sysLedUseH is NOT 0. Indicates if sysLedPattern or sysLedPatternH is used


//Indicates where new firmware has to be written to
unsigned char updateFirmwareTo;
#define UPDATE_FIRMWARE_TO_A 1
#define UPDATE_FIRMWARE_TO_B 0
#define UPDATE_FIRMWARE_TO_UNDEFINED 0xff

//Indicates where new firmware has to be loaded from
unsigned char updateFirmwareFrom;
#define UPDATE_FIRMWARE_FROM_A 1
#define UPDATE_FIRMWARE_FROM_B 0

unsigned char buf[1024];
FIRMWARE_INFO firmwareInfo;
FIRMWARE_INFO firmwareAInfo;    //FirmwareAInfo
FIRMWARE_INFO firmwareBInfo;    //FirmwareBInfo

DWORD_VAL progmemWriteAdr;      //Word address of program memory to write to

//128 Words of data to write to 1 row of program memory (192 write block). Even and Odd address words are alway written together = 1 Instuction.
//Only lower byte of odd Words are used.
WORD_VAL progmemWriteBuf[128];


/** DECLARATIONS ***************************************************/
#pragma code


/********************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *******************************************************************/
#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{
    DWORD startupDly;
    updateFirmwareTo = UPDATE_FIRMWARE_TO_UNDEFINED;
    #ifdef USB_WRITES_TO_PROGRAM_MEMORY
    ProgrammedPointer = INVALID_ADDRESS;
    #endif

    //Set all Ports as digital inputs
    #if defined(__PIC24FJ256GB206__)
    ANSB = 0x0000;
    ANSC = 0x0000;
    ANSD = 0x0000;
    //ANSF = 0x0000;
    //ANSG = 0x0000;
    #else
    AD1PCFGL=0xffff;    //Configure I/O ports as digital
    AD1PCFGH=0xffff;    //Disable input scans
    #endif

    //Set port B6 as an output, and to 0. This ensures there is no charge on RB6
    _TRISB6 = 0;
    PORTBbits.RB6 = 0;

    //Delay about 50ms. This is because ICD 3 puts some buggy reset pulses of about 20ms at reset. External FLASH
    //also requires about 20ms startup delay before writing to it.
    startupDly = 200000;
    while (startupDly != 0) {
        startupDly--;
    }


    /////////////////////////////////////////////////
    //Initialize SPI bus for EEPROM and FLASH

    //Deassert all chip select lines so there isn't any problem with initialization order.
    EEPROM_CS_IO = 1;
    EEPROM_CS_TRIS = 0;
    SPIFLASH_CS_IO = 1;
    SPIFLASH_CS_TRIS = 0;

    //Configure EEPROM and FLASH to use SPI2. They share the same SPI bus
    __builtin_write_OSCCONL(OSCCON & 0xBF);  // Unlock PPS
    iPPSOutput(PPS_OUT_SPIMEM_SCK, OUT_FN_PPS_SCK2OUT);     //Assign SCK2 (output)
    iPPSOutput(PPS_OUT_SPIMEM_SDO, OUT_FN_PPS_SDO2);        //Assign SDO2 (output)
    iPPSInput(IN_FN_PPS_SDI2, PPS_IN_SPIMEM_SDI);           //Assign SDI2 (input)
    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS

    //Set SPI bus for EEPROM to default states
    EEPROM_SCK_TRIS = 0;    // Set SCK pin as an output
    EEPROM_SDI_TRIS = 1;    // Set SDI pin is an input
    EEPROM_SDO_TRIS = 0;    // Set SDO pin as an output

    //Set SPI bus for FLASH to default states - NO NEED TO DO THIS, they use the same SPI bus
    //SPIFLASH_SCK_TRIS = 0;  // Set SCK pin as an output
    //SPIFLASH_SDI_TRIS = 1;  // Make sure SDI pin is an input
    //SPIFLASH_SDO_TRIS = 0;  // Set SDO pin as an output

    #if defined(__PIC24F__) || defined(__PIC24FK__)
        //SPI Clock = Fcy/2 = 16MHz/2 = 8MHz
        //xxxx xxxx xxxx xx11 - Primary Prescale 1:1
        //xxxx xxxx xxx1 10xx - Secondary prescale 1:2
        //xxxx xxxx xx1x xxxx - Master Mode
        //xxxx xxxx x0xx xxxx - Clock Idle is low level
        //xxxx xxxx 0xxx xxxx - SS Slave Select Disable
        //xxxx xxx1 xxxx xxxx - Serial output data changes on clock falling edge (active clock state to Idle clock state)
        //xxxx xx0x xxxx xxxx - Input data sampled at middle of data output time
        //xxxx x0xx xxxx xxxx - Communication is byte-wide (8 bits)
        //xxxx 0xxx xxxx xxxx - SDOx pin is controlled by the module
        //xxx0 xxxx xxxx xxxx - Internal SPI clock is enabled
        EEPROM_SPICON1 = 0x013b;
    #elif defined(__dsPIC30F__)
        EEPROM_SPICON1 = (0x0017 | 0x0120);     /* 1:1 primary prescale, 3:1 secondary prescale, CKE=1, MASTER mode */
    #elif defined(__dsPIC33F__) || defined(__PIC24H__)
        EEPROM_SPICON1 = (0x0003 | 0x0120);     /* 1:1 primary prescale, 8:1 secondary prescale, CKE=1, MASTER mode */
    #elif defined(__PIC32MX__)
        EEPROM_SPICON1 = (_SPI2CON_ON_MASK | _SPI2CON_FRZ_MASK | _SPI2CON_CKE_MASK | _SPI2CON_MSTEN_MASK);
    #else
        EEPROM_SPICON1 = (0x21);                /* SSPEN bit is set, SPI in master mode, FOSC/16, IDLE state is low level */
    #endif

    EEPROM_SPICON2 = 0;
    EEPROM_SPISTAT = 0;    // clear SPI
    EEPROM_SPISTATbits.SPIEN = 1;



    /////////////////////////////////////////////////
    //Timer 1 Init, config for 32MHz Fosc (16 MIPS), timer increments each 16us
    //T1CONbits.TCKPS = 3;        // 1:256 prescale
    //xxxx xxxx xxxx xx0x - Clock source = Fosc/2 = 16MHz
    //xxxx xxxx xx11 xxxx - Input clock prescaler = 256
    //xxxx xxxx x0xx xxxx - Gated time accumulation disabled
    T1CON = 0x0030;
    //PR1 = 0xFFFF;                    	//Free running timer
    //PR1 = 62500;                      //Timer interrupts every 1 second
    PR1 = 6250;                         //Timer interrupts every 100ms
    //PR1 = 3125;                       //Timer interrupts every 50ms
    //PR1 = 3906;                       //Timer interrupts every 62.5ms. This cases sysLedMask to be reset each 1 second (62.5 x 16)
    //PR1 = 7812;                       //Timer interrupts every 125ms
    //PR1 = 625;                        //Timer interrupts every 10 ms
    TMR1 = 0;                           // Clear counter
    T1CONbits.TON = 1;                	// Start timer



    /////////////////////////////////////////////////
    //Test jumpers. "C" jumper uses RB6, "F" jumper uses RB7

    //Configure jumper pin RB6 and RB7 as inputs
    TRISBbits.TRISB7 = 1;   //Set RB7 as input
    _CN25PDE = 1;           //Enable pull-down resistor for RB7
    TRISBbits.TRISB6 = 1;   //Set RB6 as input
    //Don't enable pull-down on RB6, seeing that voltage will not be pulled up high
    //enough when "C" jumper is made. LED already pulls voltage low.
    //_CN24PDE = 1;           //Enable pull-down resistor for RB6

    sysLedMask = 0x0001;
    sysLedPattern = 0;
    sysLedUseH = 0;

    //Initialize external FLASH. Put it here to add some delay between setting B6 and B7 as inputs.
    spiFlashInit();

    if (PORTBbits.RB6 == 1)
        jumpers = JMPR_CONFIG;
    else if(PORTBbits.RB7 == 1) {
        jumpers = JMPR_FIRMWARE;
    }
    else {
        jumpers = JMPR_PARK;
    }

    //Set port B6 as an output (can use _TRISB6 or TRISBbits.TRISB6
    _TRISB6 = 0;
    #ifdef DEBUGGING
    //_TRISD0 = 0;
    #endif


    // TEST TEST
    // Do this for testing, so we go direct to Bootloader
    //jumpers = JMPR_PARK;
    //jumpers = JMPR_FIRMWARE;
    //jumpers = JMPR_CONFIG;

    // TEST TEST
    //BootloaderTask();
    //sysLedPattern = 0xB010;
    //while (1) { LEDTask(); }

    // TEST TEST
    //_LATD0 = 0;
    //_LATD0 = 1;
    //_LATD0 = 0;
    //_LATD0 = 1;


    // Test if we should enter the Firmware code. This is done if jumper is in "F" position
    if (jumpers == JMPR_FIRMWARE)
    {
        //Jump to USB bootloader code
        BootloaderTask();

        //When done, jump to "User Program"
        __asm__("goto 0x1C00");
    }


    // Test if we should enter the Configuration code. This is done if jumper is in "C" position
    if (jumpers == JMPR_CONFIG)
    {
        //Jump to Configuraiton code
        ConfigTask();

        //When done, jump to "User Program"
        __asm__("goto 0x1C00");
    }

    //Check external Flash for new firmware
    FlashFirmwareUpgrade();

    //Read EEPROM address 0, this is appConf.hasValidData. Value of 0x5A indicates if appConf contains valid data
    //xeeBeginRead(0);
    //buf[0] = xeeRead();
    //xeeEndRead();

    //When done, jump to "User Program"
    __asm__("goto 0x1C00");

    /*
    //One flash per second (500ms on, 500ms off)
    //Set MSB = B. This cases sysLedMask to have 10 states, from 0x0001 to 0x2000. Period = 1 second
    sysLedPattern = 0xB01F;

    while (1)
    {
        LEDTask();
        //.......... Do Nothing
    }
    */
    return 0;
}//end main

void LEDTask(void) {
    if (_T2IF) {
        _T2IF = 0;        //Clear interrupt status bit

        sysLedMask = sysLedMask << 1;

        if (sysLedMask == 0) {
            sysLedMask = 0x0001;

            sysLedUseLorH++;
            if (sysLedUseH == 0)
                sysLedUseLorH=0;    //Only use sysLedPattern
        }

        if ( ((sysLedUseLorH&0x01)?sysLedPatternH:sysLedPattern) & sysLedMask) {
            PORTBbits.RB6 = 1;  // Turn the LED on
            #ifdef DEBUGGING
            //PORTDbits.RD0 = 1;  // Turn the LED on
            #endif
        }
        else {
            PORTBbits.RB6 = 0;  // Turn the LED off
            #ifdef DEBUGGING
            //PORTDbits.RD0 = 0;  // Turn the LED off
            #endif
        }
    }
}

/**
 * This function checks if the External Flash contains new Firmware that has to be copied to flash.
 *
 * @return 0 if firmware was NOT upgraded, else non-zero
 */
BYTE FlashFirmwareUpgrade(void) {
    BYTE i, j;
    DWORD_VAL w;
    DWORD xflAdr;                       //external flash address
    DWORD xflAdrChksum;                 //external flash address
    BYTE firmwareANewStateLB = 0x55;    //Indicate it must NOT be updated
    BYTE firmwareBNewStateLB = 0x55;    //Indicate it must NOT be updated
    BYTE firmwareNewState1;
    BYTE firmwareNewState2;
    DWORD addressNewState1;
    DWORD addressNewState2;
    BYTE allOnes;

    sysLedPattern = 0x0F0F;     //Even on/off flashing. Period = 400mS

    //Turn LED on for duration of Firmware Upgrading
    PORTBbits.RB6 = 1;  // Turn the LED on
    #ifdef DEBUGGING
    //PORTDbits.RD0 = 1;  // Turn the LED on
    #endif


    /////////////////////////////////////////////////
    //Read FirmwareAInfo and FirmwareBInfo structures
    spiFlashReadArray(XFLASH_FIRMWAREA_INFO_ADR, ((BYTE*)&firmwareAInfo), sizeof(firmwareAInfo));
    spiFlashReadArray(XFLASH_FIRMWAREB_INFO_ADR, ((BYTE*)&firmwareBInfo), sizeof(firmwareBInfo));


    /////////////////////////////////////////////////
    //Check if any state of firmwareA or firmwareB is new. If so, we have to program chip with it.
    //The following states are possible (n indicates FirmwareA or FirmwareB)
    //- x=Magic number fault    y=Magic number fault
    //  No new firmware, return 0
    //
    //- x=New(11111111)    y=Magic number fault
    //  Program memory is updated with FirmwareX from External FLASH.
    //  x->Program Memory, Change New->Current    y=NO CHANGE
    //
    //- x=New(11111111)    y=Current(11111110)
    //  Program memory is updated with FirmwareX from External FLASH.
    //  x->Program Memory, Change New->Current    y->Increment State to Previous
    //
    //- x=New(11111111)    y=Restored(11111000)
    //  Program memory is updated with FirmwareX from External FLASH.
    //  x->Program Memory, Change New->Current    y=NO CHANGE, has to stay Restored!
    //
    //- x=New(11111111)    y=???
    //  Program memory is updated with FirmwareX from External FLASH.
    //  x->Program Memory, Change New->Current    y->Set state to Faulty!
    //
    if ((firmwareAInfo.fwState.LB == FIRMWARE_STATE_NEW) && (firmwareAInfo.magicNumber.Val == FIRMWARE_INFO_MAGIC_NUMBER)) {
        updateFirmwareFrom = UPDATE_FIRMWARE_FROM_A;    //Use 'FirmwareA' from external FLASH for new Firmware in Program Memory
        xflAdr = XFLASH_FIRMWAREA_ADR;                  //Address of 'FirmwareA' in external FLASH
        firmwareANewStateLB = FIRMWARE_STATE_CURRENT;

        //firwareB is VALID
        if (firmwareBInfo.magicNumber.Val == FIRMWARE_INFO_MAGIC_NUMBER) {
            //Update FirmwareB state to 'Previous' if current 'Current'
            if (firmwareBInfo.fwState.LB == FIRMWARE_STATE_CURRENT) {
                firmwareBNewStateLB = FIRMWARE_STATE_PREVIOUS;   //Update FirmwareB State to Previous
            }
            //Update FirmwareB state to 'Faulty' if NOT 'Current' or 'Restored'
            else if (firmwareBInfo.fwState.LB != FIRMWARE_STATE_RESTORED) {
                firmwareBNewStateLB = FIRMWARE_STATE_FAULTY;   //Update FirmwareB State to Faulty
            }
        }
        //ELSE firmwareB NOT valid, don't change
    }
    else if ((firmwareBInfo.fwState.LB == FIRMWARE_STATE_NEW) && (firmwareBInfo.magicNumber.Val == FIRMWARE_INFO_MAGIC_NUMBER)) {
        updateFirmwareFrom = UPDATE_FIRMWARE_FROM_B;    //Use 'FirmwareB' from external FLASH for new Firmware in Program Memory
        xflAdr = XFLASH_FIRMWAREB_ADR;                  //Address of 'FirmwareB' in external FLASH
        firmwareBNewStateLB = FIRMWARE_STATE_CURRENT;

        //firwareA is VALID
        if (firmwareAInfo.magicNumber.Val == FIRMWARE_INFO_MAGIC_NUMBER) {
            //Update FirmwareA state to 'Previous' if current 'Current'
            if (firmwareAInfo.fwState.LB == FIRMWARE_STATE_CURRENT) {
                firmwareANewStateLB = FIRMWARE_STATE_PREVIOUS;   //Update FirmwareB State to Previous
            }
            //Update FirmwareA state to 'Faulty' if NOT 'Current' or 'Restored'
            else if (firmwareAInfo.fwState.LB != FIRMWARE_STATE_RESTORED) {
                firmwareANewStateLB = FIRMWARE_STATE_FAULTY;   //Update FirmwareA State to Faulty
            }
        }
        //ELSE firmwareB NOT valid, don't change
    }
    else {
        return 0;   //Firmware NOT updated
    }


    /////////////////////////////////////////////////
    //Do Checksum check of new firmware
    xflAdrChksum = xflAdr;

    /////////////////////////////////////////////////
    //Erase whole program memory.
    for(ErasePageTracker = BEGIN_PAGE_TO_ERASE; ErasePageTracker <= MAX_PAGE_TO_ERASE_NO_CONFIGS; ErasePageTracker++)
    {
        EraseFlash();
        USBDeviceTasks();   //Call USBDriverService() periodically to prevent falling off the bus if any SETUP packets should happen to arrive.
    }
    NVMCONbits.WREN = 0;    //Good practice to clear WREN bit anytime we are not expecting to do erase/write operations, further reducing probability of accidental activation.


    /////////////////////////////////////////////////
    //Program whole program memory.
    for (progmemWriteAdr.Val = PROGRAM_MEM_START; progmemWriteAdr.Val < PROGRAM_MEM_STOP_NO_CONFIGS; ) {
        USBDeviceTasks();       //Call USBDriverService() periodically to prevent falling off the bus if any SETUP packets should happen to arrive.

        // TEST TEST
        //If this address is the first address of new erase block, erase it now
        /*
        if ((progmemWriteAdr.Val & PROGRAM_MEM_ERASE_BLOCK_MASK) == 0) {
            ErasePageTracker = ....
            EraseFlash();   //Erase current page
        }
        */

        //Read next row from external FLASH.
        spiFlashReadArray(xflAdr, &buf[0], ((PROGRAM_MEM_WR_ROW_SIZE/2)*3));
        xflAdr += ((PROGRAM_MEM_WR_ROW_SIZE/2)*3);

        //Copy bytes read from external FLASH to the progmemWriteBuf[] buffer. Used to program the program memory.
        //Instructions are packed into external Flash, and have to be expanded. Each 3 bytes = one 2 Word Instructions.
        j=0;
        allOnes = TRUE;
        for (i=0; i<((PROGRAM_MEM_WR_ROW_SIZE/2)*3); ) {
            w.v[0] = buf[i++];
            w.v[1] = buf[i++];
            w.v[2] = buf[i++];
            w.v[3] = 0xff;
            
            //The first two bytes (from External Flash) are written to the Even address Word (of program memory buffer)
            progmemWriteBuf[j].byte.LB = w.v[0];
            progmemWriteBuf[j++].byte.HB = w.v[1];

            //The third byte (from External Flash) is written to the lower byte of the Odd address Word
            progmemWriteBuf[j++].byte.LB = w.v[2];

            //Check if all ones
            if (w.Val != 0xffffffff) {
                allOnes = FALSE;
            }
        }

        //Write data contained in progmemWriteBuf[] to Microcontroller memory to address given in progmemWriteAdr.
        //!!!!! IMPORTANT !!!!! This function updates progmemWriteAdr!!!
        if (allOnes == FALSE) {
            writeFlashRow();
        }
        //Required if writeFlashRow() not called
        else {
            //Update with address of next row in program memory.
            progmemWriteAdr.Val = progmemWriteAdr.Val + PROGRAM_MEM_WR_ROW_SIZE;
        }
    }


    /////////////////////////////////////////////////
    //Update "FirmwareA Info" and "FirmwareB Info"

    //First update "FirmwareX Info" for sector that did NOT contain new firmware. This is done in case a power
    //failure occurs before updating second "FirmwareX Info" sector. In such a case, the second "FirmwareX Info" sector
    //will still be marked as having new data, and device will be programmed again on next power cycle.
    firmwareNewState1 = firmwareANewStateLB;
    firmwareNewState2 = firmwareBNewStateLB;
    addressNewState1 = XFLASH_FIRMWAREA_INFO_ADR + offsetof(FIRMWARE_INFO, fwState.LB);
    addressNewState2 = XFLASH_FIRMWAREB_INFO_ADR + offsetof(FIRMWARE_INFO, fwState.LB);
    if (updateFirmwareFrom == UPDATE_FIRMWARE_FROM_A) {
        //First update "FirmwareB Info" if firmware was updated from A
        firmwareNewState1 = firmwareBNewStateLB;
        firmwareNewState2 = firmwareANewStateLB;
        addressNewState1 = XFLASH_FIRMWAREB_INFO_ADR + offsetof(FIRMWARE_INFO, fwState.LB);
        addressNewState2 = XFLASH_FIRMWAREA_INFO_ADR + offsetof(FIRMWARE_INFO, fwState.LB);
    }

    //Update state of FirmwareA and FirmwareB. This sector was not erased, so bits can only be cleared
    if (firmwareNewState1 != 0x55) {
        spiFlashBeginWrite(addressNewState1);
        spiFlashWriteArray(&firmwareNewState1, 1);
    }
    if (firmwareNewState2 != 0x55) {
        spiFlashBeginWrite(addressNewState2);
        spiFlashWriteArray(&firmwareNewState2, 1);
    }

    //Turn LED off for duration of Firmware Upgrading
    PORTBbits.RB6 = 0;  // Turn the LED off
    #ifdef DEBUGGING
    //PORTDbits.RD0 = 0;  // Turn the LED off
    #endif

    return 1;   //Successfully programmed
}


/**
 * Bootloader task
 */
void BootloaderTask(void) {
    sysLedPatternH = 0x0f0f;    //Always two long flashes for top 16 sysLedPatternH bits
    sysLedUseH = 1;

    BootState = IDLE_STATE;

    usbInit();

    #if defined(USB_INTERRUPT)
        USBDeviceAttach();
    #endif

    while(1)
    {
        LEDTask();

        #if defined(USB_POLLING)
        // Check bus status and service USB interrupts.
        USBDeviceTasks(); // Interrupt or polling method.  If using polling, must call
                          // this function periodically.  This function will take care
                          // of processing and responding to SETUP transactions
                          // (such as during the enumeration process when you first
                          // plug in).  USB hosts require that USB devices should accept
                          // and process SETUP packets in a timely fashion.  Therefore,
                          // when using polling, this function should be called
                          // regularly (such as once every 1.8ms or faster** [see
                          // inline code comments in usb_device.c for explanation when
                          // "or faster" applies])  In most cases, the USBDeviceTasks()
                          // function does not take very long to execute (ex: <100
                          // instruction cycles) before it returns.
        #endif


        //Blink the LEDs according to the USB device status
        blinkUSBStatus();

        // USB is Configured, and NOT suspended
        if( (USBDeviceState >= CONFIGURED_STATE) && (USBIsDeviceSuspended()==0) ) {
            BootApplication();
        }

    }//end while

    //LED off
    sysLedPattern = 0x0000;
}

/**
 * Config task
 * 1) On first entring this task, will be "1 long, 1 short flashes".
 * 2) Remove jumper and place on "F" position while power on. LED flashes fast.
 * 3) Turn power off, place jumber back on "C", and power up
 * 4) Should now be "1 long, 2 short flash".
 * 5) Remove jumper and place on "F" position while power on.
 * 6) Turn power off, place jumber back on Park position, and power up
 * 7) Board will now restore last firmware. LED will blink very fast, indicating board
 *    is getting programmed. Do NOT remove power until LED returns to slow on/off blinking.
 */
void ConfigTask(void) {
    //BYTE c;

    sysLedPattern = 0x090F;     //1 long, 2 short flash

    while (1)
    {
        LEDTask();

        //.......... Do Nothing
        
        //Do test if we are first step of config was done on last power cycle
        if (0) {
            sysLedPattern = 0x010F;     //1 long, 1 short flash
        }
    }
}


/**
 * USB Initialization. Is required for HID bootloader
 */
void usbInit(void)
{
//        The USB specifications require that USB peripheral devices must never source
//        current onto the Vbus pin.  Additionally, USB peripherals should not source
//        current on D+ or D- when the host/hub is not actively powering the Vbus line.
//        When designing a self powered (as opposed to bus powered) USB peripheral
//        device, the firmware should make sure not to turn on the USB module and D+
//        or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//        firmware needs some means to detect when Vbus is being powered by the host.
//        A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
//         can be used to detect when Vbus is high (host actively powering), or low
//        (host is shut down or otherwise not supplying power).  The USB firmware
//         can then periodically poll this I/O pin to know when it is okay to turn on
//        the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//        peripheral device, it is not possible to source current on D+ or D- when the
//        host is not actively providing power on Vbus. Therefore, implementing this
//        bus sense feature is optional.  This firmware can be made to use this bus
//        sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//        HardwareProfile.h file.
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
    #endif

//        If the host PC sends a GetStatus (device) request, the firmware must respond
//        and let the host know if the USB peripheral device is currently bus powered
//        or self powered.  See chapter 9 in the official USB specifications for details
//        regarding this request.  If the peripheral device is capable of being both
//        self and bus powered, it should not return a hard coded value for this request.
//        Instead, firmware should check if it is currently self or bus powered, and
//        respond accordingly.  If the hardware has been configured like demonstrated
//        on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//        currently selected power source.  On the PICDEM FS USB Demo Board, "RA2"
//        is used for        this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//        has been defined in HardwareProfile.h, and that an appropriate I/O pin has been mapped
//        to it in HardwareProfile.h.
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;        // See HardwareProfile.h
    #endif

    USBDeviceInit();        //usb_device.c.  Initializes USB module SFRs and firmware variables to known states.

    //initialize the variable holding the handle for the last transmission
    USBOutHandle = 0;
    USBInHandle = 0;
}//end UserInit


/********************************************************************
 * Function:        void blinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        blinkUSBStatus turns on and off LEDs
 *                  corresponding to the USB device state.
 *
 * Note:            mLED macros can be found in HardwareProfile.h
 *                  USBDeviceState is declared and updated in
 *                  usb_device.c.
 *******************************************************************/
void blinkUSBStatus(void)
{
    //USB Suspended
    if(USBIsDeviceSuspended())
    {
        sysLedPattern = 0x0249;     //2 long, 4 short flashes
    }
    else
    {
        //If USB is ready and plugged in, 2 long and 2 short flases
        //Else, 2 long and 1 short flash
        if(USBDeviceState == CONFIGURED_STATE)
        {
            sysLedPattern = 0x0009;     //2 long, 2 short flashes
        }
        else
        {
            sysLedPattern = 0x0001;     //2 long, 1 short flash
        }
    }

}//end blinkUSBStatus



/******************************************************************************
 * Function:        void BootApplication(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user routines.
 *                  It is a mixture of both USB and non-USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void BootApplication()
{
    unsigned char i;
    unsigned char bufPtr;
    unsigned int j;
    unsigned char commandNibbleLow;     //Command lower nibble = bits 0-3
    unsigned char commandNibbleHigh;    //Command higher nibble = bits 0-3
    WORD_VAL    w;
    DWORD_VAL   FlashMemoryValue;
    DWORD       xflAdr;         //external flash address
    DWORD       tempAdr;        //Requested address

    unsigned char dataType;
    #define DATA_TYPE_USER_PROG         0
    #define DATA_TYPE_FIRMWARE_CONFIG   1
    #define DATA_TYPE_XFLASH_USERSPACE  2
    #define DATA_TYPE_UNDEFINED         3

    if(BootState == IDLE_STATE)
    {
        //Are we done sending the last response.  We need to be before we
        //  receive the next command because we clear the PacketToPC buffer
        //  once we receive a command
        if(!USBHandleBusy(USBInHandle))
        {
            if(!USBHandleBusy(USBOutHandle))                //Did we receive a command?
            {
                for(i = 0; i < TOTAL_PACKET_SIZE; i++)
                {
                    PacketFromPC.Contents[i] = PacketFromPCBuffer.Contents[i];
                }

                USBOutHandle = USBRxOnePacket(HID_EP,(BYTE*)&PacketFromPCBuffer,64);
                BootState = NOT_IDLE_STATE;

                //Prepare the next packet we will send to the host, by initializing the entire packet to 0x00.
                for(i = 0; i < TOTAL_PACKET_SIZE; i++)
                {
                    //This saves code space, since we don't have to do it independently for all commands
                    PacketToPC.Contents[i] = 0;
                }
            }
        }
    }
    else //(BootState must be in NOT_IDLE_STATE)
    {
        commandNibbleLow = PacketFromPC.Command & 0x0f;

        //A "Read Memory" command, lower nibble contains memory type (MEMTYPE_ constant)
        if ((PacketFromPC.Command & 0xf0) == READ_MEMORY) {
            //Only continue if USBInHandle is not busy
            if(!USBHandleBusy(USBInHandle))
            {
                PacketToPC.Command = PacketFromPC.Command;
                PacketToPC.MemoryRdWr.Address = PacketFromPC.MemoryRdWr.Address;  //Address of requested data. Can be Byte or Word adderss, depend on memory type
                PacketToPC.MemoryRdWr.Size    = PacketFromPC.MemoryRdWr.Size;

                if (commandNibbleLow == MEMTYPE_XFLASH) {
                    xflAdr = PacketFromPC.MemoryRdWr.Address;

                    //Read all requested data, and store in PacketToPC.ByteData.
                    //stored in external Flash
                    spiFlashReadArray(xflAdr, &PacketToPC.MemoryRdWr.ByteData[0], PacketFromPC.MemoryRdWr.Size);
                }

                USBInHandle = USBTxOnePacket(HID_EP,(BYTE*)&PacketToPC.Contents[0],64);
                BootState = IDLE_STATE;
            }
        }
        //A "Write Memory" command, lower nibble contains memory type (MEMTYPE_ constant)
        else if ((PacketFromPC.Command & 0xf0) == WRITE_MEMORY) {
            xflAdr = PacketFromPC.MemoryRdWr.Address;   //Address to write to. Can be Byte or Word adderss, depend on memory type

            if (commandNibbleLow == MEMTYPE_XFLASH) {
                //Write all data received from PC to external FLASH
                spiFlashBeginWrite(xflAdr);
                spiFlashWriteArray(&PacketFromPC.MemoryRdWr.ByteData[0], PacketFromPC.MemoryRdWr.Size);
            }

            BootState = IDLE_STATE;
        } else {
            switch (PacketFromPC.Command) {
                //Gives info of this board
                case DEVICE_INFO:
                {
                    //Prepare a response packet, which lets the PC software know about the memory ranges of this device.
    // TEST TEST
    //_LATD0 = 0;
    //_LATD0 = 1;
    //_LATD0 = 0;
    //_LATD0 = 1;
                    //BoardRev is 1 more than actual revision. This is due to the way it was (accidentally) done
                    //in first version of bootloader. Keep consistent!
                    PacketToPC.DeviceInfo.Command = DEVICE_INFO;
                    #if defined(BRD_SBC66EC_R1)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66EC_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 2;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66EC_R2)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66EC_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 3;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66EC_R3)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66EC_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 4;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66ECL_R2)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66ECL_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 3;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66ECL_R3)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66ECL_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 4;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66ZL_R1)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66ZL_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 2;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66ZL_R2)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66ZL_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 3;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66ZB_R1)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66ZB_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 2;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66ZB_R2)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66ZB_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 3;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66WL_R1)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66WL_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 2;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66WL_R2)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66WL_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 3;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66WB_R1)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66WB_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 2;     //One more than board revision, see explanation above
                    #elif defined(BRD_SBC66WB_R2)
                        PacketToPC.DeviceInfo.BoardID = BOARD_ID_SBC66WB_CONFIG;
                        PacketToPC.DeviceInfo.BoardRev = 3;     //One more than board revision, see explanation above
                    #else
                    	#error "No board defined, required for BOARD_ID_Xxxx"
                    #endif

                    if (!USBHandleBusy(USBInHandle)) {
                        USBInHandle = USBTxOnePacket(HID_EP, (BYTE*) & PacketToPC, 64);
                        BootState = IDLE_STATE;
                    }
                }
                    break;

                case UNLOCK_CONFIG:
                    BootState = IDLE_STATE;
                    break;

                case RESET_DEVICE:
                {
                    U1CON = 0x0000; //Disable USB module
                    //And wait awhile for the USB cable capacitance to discharge down to disconnected (SE0) state.
                    //Otherwise host might not realize we disconnected/reconnected when we do the reset.
                    //A basic for() loop decrementing a 16 bit number would be simpler, but seems to take more code space for
                    //a given delay.  So do this instead:
                    for (j = 0; j < 0xFFFF; j++) {
                        Nop();
                    }
                    Reset();
                }
                    break;

                case ERASE_MEMORY:
                {
                    if (PacketFromPC.EraseMemory.MemoryType == MEMTYPE_XFLASH) {
                        xflAdr = PacketFromPC.EraseMemory.StartAddress;

                        while (xflAdr < PacketFromPC.EraseMemory.EndAddress) {
                            USBDeviceTasks(); //Call USBDriverService() periodically to prevent falling off the bus if any SETUP packets should happen to arrive.
                            spiFlashEraseSector(xflAdr);
                            xflAdr = xflAdr + XFLASH_SECTOR_SIZE;
                        }
                    }

                    BootState = IDLE_STATE;
                }
                    break;

                    //This command is sent to us by the host if the Erase/Program/Verify sequence was successfull
                case PROGRAM_VERIFIED:
                {
                    //Set the state of FIRMWAREx_INFO to new. This can now be done seeing that the host has confirmed the
                    //program sent to us is correct.

                    //Get start BYTE address in external FLASH where firmware (User Program) has to be written to
                    //xflAdr = (updateFirmwareTo==UPDATE_FIRMWARE_TO_A) ? XFLASH_FIRMWAREA_INFO_ADR : XFLASH_FIRMWAREB_INFO_ADR;

                    BootState = IDLE_STATE;
                }
                    break;
            }
        }
    }//End if/else
}//End BootApplication()

/**
 * Writes a row of Flash program memory. A row consists of 64 Instructions = 128 Words = 192 bytes (128 x 1.5).
 * Ensure we write to full row. Also ensure to set any bytes not used to 0xff. Must be edge-aligned, from the
 * beginning of program memory, on boundaries of 1536 bytes and 192 bytes.
 *
 * @important progmemWriteAdr is incremented by PROGRAM_MEM_WR_ROW_SIZE once this function returns
 *
 * @param progmemWriteAdr Contains address of 128 Word row to write. Ensure is on 128 Word boundry ((progmemWriteAdr & PROGRAM_MEM_WR_ROW_MASK)==0)
 *
 * @param progmemWriteBuf Word buffer containing 128 Words to be written to given row
 */
void writeFlashRow(void)
{
    BYTE i;
    NVMCON = 0x4001;                            //Perform ROW write next time WR gets set = 1.

    //Write 128 Words (for PIC24F = 64 Instructions) to programming holding registers
    for (i=0; i<PROGRAM_MEM_WR_ROW_SIZE; ) {
        TBLPAG = progmemWriteAdr.word.HW;       // Initialize PM Page Boundary SFR

        //Perform TBLWT instructions to write latches
        __builtin_tblwtl(progmemWriteAdr.word.LW, progmemWriteBuf[i++].Val);    //Write the low word to the latch
        __builtin_tblwth(progmemWriteAdr.word.LW, progmemWriteBuf[i++].Val);    //Write the high word to the latch (8 bits of data + 8 bits of "phantom data")

        progmemWriteAdr.Val += 2;   //Increment by 2
    }

    //Program row to Program memory
    asm("DISI #16");                        //Disable interrupts for next few instructions for unlock sequence
    __builtin_write_NVM();
    while(NVMCONbits.WR == 1){}
    NVMCONbits.WREN = 0;                //Good practice to clear WREN bit anytime we are not expecting to do erase/write operations, further reducing probability of accidental activation.
}


void EraseFlash(void)
{
    DWORD_VAL MemAddressToErase = {0x00000000};
    MemAddressToErase.Val = (((DWORD)ErasePageTracker) << 10);

    NVMCON = 0x4042;                                //Erase page on next WR

    TBLPAG = MemAddressToErase.byte.UB;
    __builtin_tblwtl(MemAddressToErase.word.LW, 0xFFFF);

    asm("DISI #16");                                        //Disable interrupts for next few instructions for unlock sequence
    __builtin_write_NVM();
    while(NVMCONbits.WR == 1){}

//        EECON1bits.WREN = 0;  //Good practice now to clear the WREN bit, as further protection against any future accidental activation of self write/erase operations.
}


#ifdef USB_WRITES_TO_PROGRAM_MEMORY
void WriteFlashSubBlock(void)                //Use word writes to write code chunks less than a full 64 byte block size.
{
    unsigned int i = 0;
    DWORD_VAL Address;

    NVMCON = 0x4003;                //Perform WORD write next time WR gets set = 1.

    while(BufferedDataIndex > 0)                //While data is still in the buffer.
    {
        Address.Val = ProgrammedPointer - BufferedDataIndex;
        TBLPAG = Address.word.HW;

        __builtin_tblwtl(Address.word.LW, ProgrammingBuffer[i]);                //Write the low word to the latch
        __builtin_tblwth(Address.word.LW, ProgrammingBuffer[i + 1]);        //Write the high word to the latch (8 bits of data + 8 bits of "phantom data")
        i = i + 2;

        asm("DISI #16");                                        //Disable interrupts for next few instructions for unlock sequence
        __builtin_write_NVM();
        while(NVMCONbits.WR == 1){}

        BufferedDataIndex = BufferedDataIndex - 2;                //Used up 2 (16-bit) words from the buffer.
    }

    NVMCONbits.WREN = 0;                //Good practice to clear WREN bit anytime we are not expecting to do erase/write operations, further reducing probability of accidental activation.
}
#endif

/*********************************************************************
 * Function:        DWORD ReadProgramMemory(DWORD address)
 *
 * PreCondition:    None
 *
 * Input:           Program memory address to read from.  Should be
 *                            an even number.
 *
 * Output:          Program word at the specified address.  For the
 *                            PIC24, dsPIC, etc. which have a 24 bit program
 *                            word size, the upper byte is 0x00.
 *
 * Side Effects:    None
 *
 * Overview:        Modifies and restores TBLPAG.  Make sure that if
 *                            using interrupts and the PSV feature of the CPU
 *                            in an ISR that the TBLPAG register is preloaded
 *                            with the correct value (rather than assuming
 *                            TBLPAG is always pointing to the .const section.
 *
 * Note:            None
 ********************************************************************/
/*
DWORD ReadProgramMemory(DWORD address) {
    DWORD_VAL dwvResult;
    WORD wTBLPAGSave;

    wTBLPAGSave = TBLPAG;
    TBLPAG = ((DWORD_VAL*) & address)->w[1];

    dwvResult.w[1] = __builtin_tblrdh((WORD) address);
    dwvResult.w[0] = __builtin_tblrdl((WORD) address);
    TBLPAG = wTBLPAGSave;

    return dwvResult.Val;
}
*/



// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The usbCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.


/**
 * Call back that is invoked when a USB suspend is detected
 */
void USBCBSuspend(void)
{
    //Example power saving code.  Insert appropriate code here for the desired
    //application behavior.  If the microcontroller will be put to sleep, a
    //process similar to that shown below may be used:

    //ConfigureIOPinsForLowPower();
    //SaveStateOfAllInterruptEnableBits();
    //DisableAllInterruptEnableBits();
    //EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();        //should enable at least USBActivityIF as a wake source
    //Sleep();
    //RestoreStateOfAllPreviouslySavedInterruptEnableBits();        //Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
    //RestoreIOPinsToNormal();                                                                        //Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

    //IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is
    //cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause
    //things to not work as intended.


    #if defined(__C30__)
        //USBSleepOnSuspend();      //Need to include usb_hal_pic24.c if this function is enabled.
    #endif
}


/**
 * The host may put USB peripheral devices in low power suspend mode (by "sending" 3+ms
 * of idle). Once in suspend mode, the host may wake the device back up by sending
 * non-idle state signalling. This call back is invoked when a wakeup from USB suspend
 * is detected.
 */
void USBCBWakeFromSuspend(void)
{
    // If clock switching or other power savings measures were taken when
    // executing the USBCBSuspend() function, now would be a good time to
    // switch back to normal full power run mode conditions.  The host allows
    // a few milliseconds of wakeup time, after which the device must be
    // fully back to normal, and capable of receiving and processing USB
    // packets.  In order to do this, the USB module must receive proper
    // clocking (IE: 48MHz clock must be available to SIE for full speed USB
    // operation).
}


/**
 * The USB host sends out a SOF packet to full-speed devices every 1 ms. This interrupt
 * may be useful for isochronous pipes. End designers should implement callback routine
 * as necessary.
 */
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

    // Typically, user firmware does not need to do anything special
    // if a USB error occurs.  For example, if the host sends an OUT
    // packet to your device, but the packet gets corrupted (ex:
    // because of a bad connection, or the user unplugs the
    // USB cable during the transmission) this will typically set
    // one or more USB error interrupt flags.  Nothing specific
    // needs to be done however, since the SIE will automatically
    // send a "NAK" packet to the host.  In response to this, the
    // host will normally retry to send the packet again, and no
    // data loss occurs.  The system will typically recover
    // automatically, without the need for application firmware
    // intervention.

    // Nevertheless, this callback function is provided, such as
    // for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 *                                         firmware must process the request and respond
 *                                        appropriately to fulfill the request.  Some of
 *                                        the SETUP packets will be for standard
 *                                        USB "chapter 9" (as in, fulfilling chapter 9 of
 *                                        the official USB specifications) requests, while
 *                                        others may be specific to the USB device class
 *                                        that is being implemented.  For example, a HID
 *                                        class device needs to be able to respond to
 *                                        "GET REPORT" type of requests.  This
 *                                        is not a standard USB chapter 9 request, and
 *                                        therefore not handled by usb_device.c.  Instead
 *                                        this request should be handled by class specific
 *                                        firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckHIDRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *                                        called when a SETUP, bRequest: SET_DESCRIPTOR request
 *                                        arrives.  Typically SET_DESCRIPTOR requests are
 *                                        not used in most applications, and it is
 *                                        optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 *                                         SET_CONFIGURATION (wValue not = 0) request.  This
 *                                        callback function should initialize the endpoints
 *                                        for the device's usage according to the current
 *                                        configuration.
 *
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
    //enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    //Arm the OUT endpoint for the first packet
    USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&PacketFromPCBuffer,64);
}

/********************************************************************
 * Function:        void usbCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 *                                         peripheral devices to wake up a host PC (such
 *                                        as if it is in a low power suspend to RAM state).
 *                                        This can be a very useful feature in some
 *                                        USB applications, such as an Infrared remote
 *                                        control        receiver.  If a user presses the "power"
 *                                        button on a remote control, it is nice that the
 *                                        IR receiver can detect this signalling, and then
 *                                        send a USB "command" to the PC to wake up.
 *
 *                                        The usbCBSendResume() "callback" function is used
 *                                        to send this special USB signalling which wakes
 *                                        up the PC.  This function may be called by
 *                                        application firmware to wake up the PC.  This
 *                                        function will only be able to wake up the host if
 *                  all of the below are true:
 *
 *                                        1.  The USB driver used on the host PC supports
 *                                                the remote wakeup capability.
 *                                        2.  The USB configuration descriptor indicates
 *                                                the device is remote wakeup capable in the
 *                                                bmAttributes field.
 *                                        3.  The USB host PC is currently sleeping,
 *                                                and has previously sent your device a SET
 *                                                FEATURE setup packet which "armed" the
 *                                                remote wakeup capability.
 *
 *                  If the host has not armed the device to perform remote wakeup,
 *                  then this function will return without actually performing a
 *                  remote wakeup sequence.  This is the required behavior,
 *                  as a USB device that has not been armed to perform remote
 *                  wakeup must not drive remote wakeup signalling onto the bus;
 *                  doing so will cause USB compliance testing failure.
 *
 *                                        This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            This function does nothing and returns quickly, if the USB
 *                  bus and host are not in a suspended condition, or are
 *                  otherwise not in a remote wakeup ready state.  Therefore, it
 *                  is safe to optionally call this function regularly, ex:
 *                  anytime application stimulus occurs, as the function will
 *                  have no effect, until the bus really is in a state ready
 *                  to accept remote wakeup.
 *
 *                  When this function executes, it may perform clock switching,
 *                  depending upon the application specific code in
 *                  USBCBWakeFromSuspend().  This is needed, since the USB
 *                  bus will no longer be suspended by the time this function
 *                  returns.  Therefore, the USB module will need to be ready
 *                  to receive traffic from the host.
 *
 *                  The modifiable section in this routine may be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of ~3-15 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at least 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void usbCBSendResume(void)
{
    static WORD delay_count;

    //First verify that the host has armed us to perform remote wakeup.
    //It does this by sending a SET_FEATURE request to enable remote wakeup,
    //usually just before the host goes to standby mode (note: it will only
    //send this SET_FEATURE request if the configuration descriptor declares
    //the device as remote wakeup capable, AND, if the feature is enabled
    //on the host (ex: on Windows based hosts, in the device manager
    //properties page for the USB device, power management tab, the
    //"Allow this device to bring the computer out of standby." checkbox
    //should be checked).
    if(USBGetRemoteWakeupStatus() == TRUE)
    {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if(USBIsBusSuspended() == TRUE)
        {
            USBMaskInterrupts();

            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0;
            USBBusIsSuspended = FALSE;  //So we don't execute this code again,
                                        //until a new suspend condition is detected.

            //Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            //device must continuously see 5ms+ of idle on the bus, before it sends
            //remote wakeup signalling.  One way to be certain that this parameter
            //gets met, is to add a 2ms+ blocking delay here (2ms plus at
            //least 3ms from bus idle to USBIsBusSuspended() == TRUE, yeilds
            //5ms+ total delay since start of idle).
            delay_count = 3600U;
            do
            {
                delay_count--;
            }while(delay_count);

            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1;       // Start RESUME signaling
            delay_count = 1800U;        // Set RESUME line for 1-13 ms
            do
            {
                delay_count--;
            }while(delay_count);
            USBResumeControl = 0;       //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}


/**
 * This function is called from the USB stack to  notify a user application that a USB event
 * occurred.  This callback is in interrupt context when the USB_INTERRUPT option is selected.
 *
 * @param event the type of event
 * @param pdata pointer to the event data
 * @param size size of the event data
 *
 * @ret None
 */
BOOL USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_TRANSFER:
            //Add application specific callback task or callback function here if desired.
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED:
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER_TERMINATED:
            //Add application specific callback task or callback function here if desired.
            //The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
            //FEATURE (endpoint halt) request on an application endpoint which was
            //previously armed (UOWN was = 1).  Here would be a good place to:
            //1.  Determine which endpoint the transaction that just got terminated was
            //      on, by checking the handle value in the *pdata.
            //2.  Re-arm the endpoint if desired (typically would be the case for OUT
            //      endpoints).
            break;
        default:
            break;
    }
    return TRUE;
}

void xflashDebug(DWORD startAdr, BYTE repeat) {
    BYTE i;

    for (i=0; i<repeat; i++) {
        spiFlashReadArray(startAdr + (i*1024), buf, 1024);
    }

}
