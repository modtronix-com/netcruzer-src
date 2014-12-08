/**
 * @brief           External Flash Defines for SBC66 range of boards
 * @file            nz_xflashDefsSbc66.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    none
 * @compiler        MPLAB XC16
 *
 * @section xflashDefs_desc Description
 *****************************************
 * Code for using and configuring I/O ports
 *
 **********************************************************************
 * @section xflashDefs_lic Software License Agreement
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
 * 2013-11-20, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef XFLASH_DEFS_H
#define XFLASH_DEFS_H


/////////////////////////////////////////////////
//Hex File
/////////////////////////////////////////////////
// Types of memory regions in hex file. Defines the address ranges of sections of code in the hex file. Data in hex file
//is not packed, but is packed in External Flash:
// - MEMTYPE_PROGRAM
//   Program Memory located in hex file from 0x3800 (0x1C00 x 2) to 54FFF (0x2A800 x 2 - 1). Is packed and stored in External
//   flash (3 bytes for each 4 in hex file) in "FirmwareA" or "FirmwareB".
//
// - MEMTYPE_XEEPROM
//   Data for the external EEPROM. Is stored in External FLASH ("FirmwareX Configuration" area) for SBC66EC, and
//   written to external EEPROM by firmware on device. This data is contained in the hex file from Word address
//   0x800000 (byte address 0x1000000). When we receive data located in this address range,
//   it has to be stored in the "FirmwareX Configuration" section of the external FLASH. Only odd addresses in the
//   hex file contains data. Each odd address contains 2 bytes of data, even addresses do NOT contain any data.
//   Under certian conditions, the "User Program" will read this data, and program the External EEPROM. These conditions include when new
//   firmware is uploaded to the board, or if the configuration data in the external EEPROM becomes corrupt.
//
// - MEMTYPE_XFLASH
//   Data for the external FLASH.
//   It is possible programming the "User Space" in the external flash with data located in the hex file.
//   For the SBC66EC, this data is contained in the hex file from Word address 0x820000 (byte address 0x1040000).
//   When we receive data located in this address range, it is programmed to the "User Space" area of the external
//   FLASH. Maximum size is 0xE0000 Words = 1,835,008 Bytes. Only odd addresses in the hex file contains data, so
//   data = 1,835008/2 = 917,503. Each odd address contains 2 bytes of data, even addresses do NOT contain any data.
//
#define MEM_VECTORS_START           0x00000000
#define MEM_VECTORS_END             0x00000400  //One page of vectors + general purpose program memory. Word Address
//Bootloader resides in memory range 0x400-0x1BFF

//Flash Page = hex address / 0x400
#if defined(__PIC24FJ256GB210__) || defined(__PIC24FJ256GB206__)
    #define MEM_PROGRAM_START           0x00003800  //Byte address in internal Program Memory of PIC (Word adr = 0x1C00)
    #define MEM_PROGRAM_LAST            0x00054FFF  //Last Byte address in internal Program Memory of PIC (Word adr = 0x2A7FF)

    #define MEM_EEPROM_START            0xFFFFFFFF  //No internal EEPROM
    #define MEM_EEPROM_LAST             0xFFFFFFFF  //No internal EEPROM

    #define MEM_CONFIG_START            0x00055000  //Byte address in internal Program Memory of PIC (Word adr = 0x2A800)
    #define MEM_CONFIG_LAST             0x000557FF  //Last Byte address in internal Program Memory of PIC (Word adr = 0x2ABFF)

    #define MEM_XEEPROM_START           0x01000000  //Byte address (in hex file) of start of "FirmwareX Configuration" in external Flash = 0x800000 word address
    //SBC66EC has 8K Byte EEPROM. 16K Bytes of hex file space is needed because only 50% contains data
    #define MEM_XEEPROM_LAST           (MEM_XEEPROM_START + (XFLASH_FIRMWARE_CONFIG_SIZE_IN_SECTORS*0x1000*2) - 1)
    //#define MEM_XEEPROM_LAST          0x01003FFF
    //#define MEM_XEEPROM_LAST_MAX      0x0103FFFF  //Maximum possible is 3FFFF. Only 50% contain data = 256/2 = 128 KBytes.

    #define MEM_XFLASH_START            0x01040000  //Byte address (in hex file) of start of User Space in external Flash = 0x820000 word address
    //SBC66EC has 4096 Bytes of User Data (1 sector). 8K Bytes (1FFF) of hex file space is needed because only 50% contains data
    #define MEM_XFLASH_LAST            (MEM_XFLASH_START_SBC66EC + (XFLASH_USER_SPACE_SIZE_IN_SECTORS*0x1000*2) - 1)
    //#define MEM_XFLASH_LAST           0x01041FFF
    //#define MEM_XFLASH_LAST_MAX       0x011FFFFF  //Maximum possible is 11FFFFF-1040000 = 16FFFF. Only 50% contain data = 1,835008/2 = 917,503 Bytes

    #define PROGRAM_MEM_WR_ROW_SIZE         128         //For PIC24F family, 128 (0x80) Words can be written at a time. This is 128 x 1.5 = 192 bytes. Upper byte of each odd address Word is not implemented
    #define PROGRAM_MEM_WR_ROW_MASK         (PROGRAM_MEM_WR_ROW_SIZE-1)

    #define PROGRAM_MEM_ERASE_BLOCK_SIZE    1024        //Smallest number of words that can be erased. To get bytes x 1.5
    #define PROGRAM_MEM_ERASE_BLOCK_MASK    (PROGRAM_MEM_ERASE_BLOCK_SIZE-1)
#elif defined(__PIC24FJ128GB210__) || defined(__PIC24FJ128GB206__) || defined(__PIC24FJ128GB106__)
    #define MEM_PROGRAM_START           0x00003800  //Byte address in internal Program Memory of PIC (Word adr = 0x1C00)
    #define MEM_PROGRAM_LAST            0x0002A7FF  //Last Byte address in internal Program Memory of PIC (Word adr = 0x153FF)

    #define MEM_EEPROM_START            0xFFFFFFFF  //No internal EEPROM
    #define MEM_EEPROM_LAST             0xFFFFFFFF  //No internal EEPROM

    #define MEM_CONFIG_START            0x0002A800  //Byte address in internal Program Memory of PIC (Word adr = 0x???)
    #define MEM_CONFIG_LAST             0x0002AFFF  //Last Byte address in internal Program Memory of PIC (Word adr = 0x???)

    #define MEM_XEEPROM_START           0x01000000  //Byte address (in hex file) of start of "FirmwareX Configuration" in external Flash = 0x800000 word address
    //SBC66ECL has 8K Byte EEPROM. 16K Bytes of hex file space is needed because only 50% contains data
    #define MEM_XEEPROM_LAST           (MEM_XEEPROM_START + (XFLASH_FIRMWARE_CONFIG_SIZE_IN_SECTORS*0x1000*2) - 1)
    //#define MEM_XEEPROM_LAST          0x01003FFF
    //#define MEM_XEEPROM_LAST_MAX      0x0103FFFF  //Maximum possible is 3FFFF. Only 50% contain data = 256/2 = 128 KBytes.

    #define MEM_XFLASH_START            0x01040000  //Byte address (in hex file) of start of User Space in external Flash = 0x820000 word address
    //SBC66EC has 4096 Bytes of User Data (1 sector). 8K Bytes (1FFF) of hex file space is needed because only 50% contains data
    #define MEM_XFLASH_LAST            (MEM_XFLASH_START_SBC66ECL + (XFLASH_USER_SPACE_SIZE_IN_SECTORS*0x1000*2) - 1)
    //#define MEM_XFLASH_LAST           0x01041FFF  //Copied from SBC66EC - CHECK!!!!!!!!!!!!!!!!
    //#define MEM_XFLASH_LAST_MAX       0x011FFFFF  //Copied from SBC66EC - CHECK!!!!!!!!!!!!!!!!  Maximum possible is 11FFFFF-1040000 = 16FFFF. Only 50% contain data = 1,835008/2 = 917,503 Bytes

    #define PROGRAM_MEM_WR_ROW_SIZE         128         //For PIC24F family, 128 (0x80) Words can be written at a time. This is 128 x 1.5 = 192 bytes. Upper byte of each odd address Word is not implemented
    #define PROGRAM_MEM_WR_ROW_MASK         (PROGRAM_MEM_WR_ROW_SIZE-1)

    #define PROGRAM_MEM_ERASE_BLOCK_SIZE    1024        //Smallest number of words that can be erased. To get bytes x 1.5
    #define PROGRAM_MEM_ERASE_BLOCK_MASK    (PROGRAM_MEM_ERASE_BLOCK_SIZE-1)
#else
    #error "This bootloader only covers the PIC24FJ256GB210 family devices.  Please see another folder for the bootloader appropriate for the selected device."
#endif



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
//External FLASH
/////////////////////////////////////////////////
//FirmwareA and FirmwareB contains the Microcontroller's program memory, in a Packed form! Each 3 bytes = one 2 Word Instructions.
//The last 8 bytes of the XFLASH_FIRMWAREX_CONFIG and XFLASH_MODIFIED_CONFIG will be used by the bootloader. It will have
//a checksum, and a magic number. If the magic number is wrong, it is not valid. When new firmware is downloaded, the
//XFLASH_FIRMWAREX_CONFIG is updated with data from new firmware, and after bootloader validates it, the magic number is
//set. This ensure it is only set as "valid" after validation. During the "User Program", if the user changes configuration,
//and decided to save it as the new default configuration, then it is written to the XFLASH_MODIFIED_CONFIG area,
//and the XFLASH_FIRMWAREX_CONFIG area is marked as invalid. If the user again saved new default config, it is now written to
//XFLASH_FIRMWAREX_CONFIG, and XFLASH_MODIFIED_CONFIG area is marked as invalid. Each time the user saves new data, it is stored
//in either XFLASH_FIRMWAREX_CONFIG or XFLASH_MODIFIED_CONFIG (alternates one, then other....).
#define XFLASH_FIRMWAREA_INFO_ADR           0           //Byte address of start of "FirmwareA Info"
#define XFLASH_FIRMWAREA_ADR                0x1000      //Byte address of start of "FirmwareA"
#define XFLASH_FIRMWAREA_CONFIG             0x40000     //Byte address of start of "FirmwareA Configuration"

#define XFLASH_FIRMWAREB_INFO_ADR           0x42000     //Byte address of start of "FirmwareB Info"
#define XFLASH_FIRMWAREB_ADR                0x43000     //Byte address of start of "FirmwareB"
#define XFLASH_FIRMWAREB_CONFIG             0x82000     //Byte address of start of "FirmwareB Configuration"

#define XFLASH_MODIFIED_CONFIG              0x84000

#define XFLASH_FIRMWAREB_OFFSET_FROMA       (XFLASH_FIRMWAREB_INFO_ADR - XFLASH_FIRMWAREA_INFO_ADR)

#define XFLASH_FIRMWARE_SIZE_IN_SECTORS     63          //Indicates size of FirmwareA and FirmwareB areas in 4096 byte sectors = 63x4096 = 258,048 Bytes
#define XFLASH_FIRMWARE_CONFIG_SIZE_IN_SECTORS  2       //Indicates size of "FirmwareA Config" and "FirmwareB Config" in 4096 byte sectors = 2x4096 = 8k
#define XFLASH_SECTOR_SIZE                  4096        //Byte size of a sector of external FLASH. A sector is the smallest area that can be erased.

//Byte address of start of "User Space". User space is followed by File System, which is located at address given by MPFS_RESERVE_BLOCK
//define in main project. User can use the space in external FLASH from XFLASH_USER_SPACE_ADR up till (MPFS_RESERVE_BLOCK - 1). To increase
//user space, increase MPFS_RESERVE_BLOCK value.
#define XFLASH_USER_SPACE_ADR               0x86000




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
		unsigned int bFill : 11;	//Fill till 16 bits
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


#endif

