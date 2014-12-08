/**
 * @brief           This file defines the Application Configuration interface
 * @file            nz_appConfig.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section nz_appConfig_desc Description
 *****************************************
 * This file defines the Application Configuration interface. The implementation has to be done
 * in a separate c file by creating instances of the defined functions.
 * The Netcruzer library contains one implementation:
 * - nz_appConfigXEE.c, this is a default implementation for storing the application configuration in
 *   an external SPI EEPROM.
 *
 * See @ref nz_appConfig_usage for details how to use it.
 *
 * @subsection nz_appConfig_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ------------ AppConfig Configuration (from nz_appConfig.h) -------------
// *********************************************************************
//Normally the CFG_STRUCT is located in the external Flash. It is placed there by the bootloader.
//If a bootloader is not used, it has to be define in code, and copied to the external Flash.
#define CFG_STRUCT_IN_CODE

 @endcode
 *
 *
 * @subsection nz_appConfig_usage Usage
 *****************************************
 * To use this module, the following must be done:
 * - Copy the @ref nz_appConfig_conf "Configuration" section above (from nz_appConfig.h)
 *   to the projdefs.h file.
 * - In this "Configuration" section, change any default values if required (if default
 *   values should be used, define is not required in projdefs.h).
 * - Add a default implementation c file to the MPLAB project, like nz_appConfigXEE.c for example.
 * - Include nz_appConfig.h in the c file it is used in.
 * - <b>All DONE!</b> The functions defined in this file can now be used in the project.
 *
 **********************************************************************
 * @section nz_appConfig_lic Software License Agreement
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
 * 2013-10-28, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_APPCONFIG_H
#define NZ_APPCONFIG_H


/* ************************************************************************************************
=============== Modify firmware upgrading and Netcruzer Bootloader ===============
 - Modify bootloader on PIC, so that at startup it checks if there is any configuration data in external
   Flash. If not, it stays in Bootloader mode until config data is uploaded.
 - Modify "Netcruzer USB Bootloader" with new mode where it will automatically upload firmware and config
   from HEX file as soon as board has been programmed. Add a "Run" button to app that becomes available
   once this has been uploaded to device.

=============== Low cost future version using external FLASH - Currently NOT USED ===============
No EEPROM method for configuration data. Store in external serial Flash. Erase sectors 4k.
Configuration data is stored as Id, Value entries. Flash is devided in blocks of 8 bytes.
Each 8 byte block has following format:
- 0-1: First 2 bytes is ID, length and deleted info
       x--- ---- ---- ---- = Deleted bit
       -xxx x--- ---- ---- = Length, gives multiples of 8 byte blocks used. First block has 6 bytes of data, subsequent
                             blocks 8 bytes. This gives max of 6 + (16 x 8) = 134 bytes
       ---- -xxx xxxx xxxx = ID, max of 2048 settings
- 2-7: Data of 6 bytes. If more is needed, length above is used to assign more 8 byte blocks

Each 4K erase block in flash has following format:
- 64 Bytes = Used bit fields (512 bits = 512 x 8 byte blocks). Bit is cleared if pointed to 8 byte block is used
- 64 Bytes = Deleted bit fields (512 bits = 512 x 8 byte blocks). Bit is cleared if pointed to 8 byte block is
             deleted. This will happen if a setting is updated. In this case the old setting will have it's deleted
             bit cleared, and written to next available position in flash.
- 3968 Bytes = 496 x 8 byte blocks

In internal RAM of CPU, a cach area is reserved. Default is 256 bytes. Will contain latest read Id/Value data.
Is saved in 8 byte format, same as defined above. Implemented as circular buffer. Each time a setting is needed that
is not in cach, it is read from Flash, and added to cach. Entries are deleted from head until enough space is made for
newly read setting to be saved.
************************************************************************************************ */


/////////////////////////////////////////////////
//  Application Configuration - documentation module
/** @defgroup info_conf_app Application Configuration
 @ingroup info_conf
 All SBC66 Netcruzer boards have an external EEPROM and FLASH, which is normally used by the project to store
 application configuration data. The "nz_appConfig.h" file provides an interface for storing,
 retrieving and modifying application configuration data. It is recommended to implement it in a project
 if application configuration data is required.

 For details how a projects stores it's configuration data, refer to
 the specific @ref info_proj "Project's" documentation.

 The application configuration is stored in the external EEPROM of the SBC board.
 It is stored in the CFG_STRUCT section of the EEPROM. The XEEMEM_USER_START_ADR and XEEMEM_USER_END_ADR
 defines should be used to get the address range available for user data.


 @section info_conf_app_memmap External EEPROM Memory Map
 The external EEPROM is devided into various sections, as defined by XEEMEM_xxx defines.
 For example, if the user wants to use the external EEPROM for project specific data, the
 XEEMEM_USER_START_ADR define should be used to located the "User EEPROM" space.
 
 The following example shows the memory map for the "Webserver" project:
 <table>
 <tr>
 <td><b>0x0000-0x001F</b> (0-31)</td>
 <td>CFG_INFO - Info, Magic Number, CRC... Used for checksum calculations, and to check if EEPROM contains valid data.</td>
 </tr>
 <tr>
 <td><b>0x0020-0x03FF</b> (32-1023)</td>
 <td>CFG_COPY - Copy of last written settings. Use write blocks of 32 bytes</td>
 </tr>
 <tr>
 <td><b>0x0400-0x0FFF</b> (1024-4095)</td>
 <td>CFG_STRUCT - Main Content.<br>In code this address range is defined by the XEEMEM_CFG_STRUCT_START_ADR and XEEMEM_CFG_STRUCT_END_ADR defines.</td>
 </tr>
 <tr>
 <td><b>0x1000-0x1FFF</b> (4096-8191)</td>
 <td><b>User EEPROM</b> space, can be used by user application to store non volatile data.<br>In code this address range is defined by the XEEMEM_USER_START_ADR and XEEMEM_USER_END_ADR defines.</td>
 </tr>
 </tr></table>
 
 
 @section info_conf_app_details Details
 The default configuration is stored in the cfgStructHexfile and cfgStruct structures in the
 project's appConfig.c file. Both these strucures should be kept identical! When making a
 change to one, do the same to the other!
 - The <b>cfgStructHexfile</b> structure places the default configuration in the generated
   hex file at byte address 0x1000000. This is outside the target device's FLASH program memory,
   and will not be used by a PIC programmer. This data is only used by the @ref devtools_nzUsbBootloader
   The @ref devtools_nzUsbBootloader takes this data from the hex file, and programs it onto the external FLASH
   of the target board when doing a firmware upgrade. This default configuration data is then
   used by the target board to update it's application configuration data.
 - The <b>cfgStruct</b> structure places the default configuration in the target device's FLASH
   program memory. It only happens when the CFG_STRUCT_IN_CODE macro is defines. This has to
   be done when the firmware is NOT programmed with the @ref devtools_nzUsbBootloader, but with
   a PIC programmer. The reason is that the cfgStructHexfile is only programmed into the target's
   external EEPROM when the @ref devtools_nzUsbBootloader is used. When a PIC programmer is used,
   the cfgStruct is required. It uses additional FLASH program memory of the target! When the
   CFG_STRUCT_IN_CODE macro is defined, the firmware will update the application configuration data
   (in external EEPROM) with the contents of the cfgStruct structure.
 
 
 @section info_conf_app_read Reading AppConfig
 
 
 
 @section info_conf_app_modify Modifying AppConfig
 When upgrading firmware, the old configuration data should be preserved. To do this, it is very important
 that if the config structures are changed in new firmware, changes are ONLY made to back of structure.
 The upgrade process will work as follows:
 - Find current copy of CFG_STRUCT in external FLASH. Remember it as "Previous CFG_STRUCT"
 - Overwrite external EEPROM with new CFG_STRUCT (contained in new firmware)
 - Iterate through all CFG_BLOCKs in "Previous CFG_STRUCT". For each one found, search for the corresponding
      CFG_BLOCK in new CFG_STRUCT (external EEPROM). When found, copy data from "Previous" CFG_BLOCK to new
      one in external EEPROM.
 */


///////////////////////////////////////////////////////////////////////////////
//////////////// Config Block (CFG_BLOCK_XXX) defines to follow ///////////////
//CFG_BLOCK_XXX Structures are used in CFG_STRUCT. Ensure each CFG_BLOCK_XXX
//structure is smaller than 255 bytes.
//Ensure ((size of block)+4) is a 32 byte multiple. Ensure size is 4 less then 32 byte
//multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long).
//Each block MUST have a 'reserve' member (byte array), even if it is 0.
//
//The only reason all CFG_BLOCKs should be 32 byte multiples, is because EEPROM has
//a page write size of 32. So, if we change a whole CFG_BLOCK, and the boundaries
//are on 32 byte blocks, chances are it will require less "EEPROM block writes". Each
//"EEPROM block write" takes about 5 ms, so this speeds things up a bit. BUT, it will
//not make a big difference if CFG_BLOCKs are 16 byte multiples. Do NOT use less than
//16 byte multiples, because the CFG_BLOCK_HDR.size counts in multiples of 16 bytes!

//Config block ID. The following ranges are reserved:
// - 0x0000 to 0x00ff: Used to mark config blocks as deleted
// - 0xff00 to 0xffff: Used to mark config blocks as free
// - 0x8000 to 0xefff: Used for TABLE_BLOCKS = a constant CFG_BLOCK
#define CFG_BLOCK_INFO_ID           0x0100
#define CFG_BLOCK_NET_ID            0x0101
#define CFG_BLOCK_WIFI_ID           0x0102
#define CFG_BLOCK_SNMP_ID           0x0103
#define CFG_BLOCK_GENERAL_ID        0x0104
#define CFG_BLOCK_NET2_ID           0x0105
#define CFG_BLOCK_SER_0TO7_ID       0x0108
#define CFG_BLOCK_SER_8TO15_ID      0x0109
#define CFG_BLOCK_SER_16TO23_ID     0x010A
#define CFG_BLOCK_SER_24TO31_ID     0x010B
#define CFG_BLOCK_STRING_31X8_ID    0x0110  ///< 31 strings of 8 bytes each
#define CFG_BLOCK_UC66_PORT_ID      0x0114  ///< UC Port ID for SBC66 boards
#define CFG_BLOCK_SBC66_PORT_ID     0x0115  ///< IO Ports for SBC66 Boards
#define CFG_BLOCK_RESERVE_ID        0xFF00  ///< ID 0xff00 to 0xffff is reserved

/**
 * Header for all CFG_BLOCK_xxx structures. Is 4 bytes long, is important seeing that we try to keep CFG_BLOCK_XXX
 * structures on 32 byte boundaries, and each one preceded by a CFG_BLOCK_HDR
 */
typedef struct __attribute__((__packed__))
{
    WORD    id;         ///< ID of this CFG_BLOCK
    BYTE    reserved;  ///< Bits 0-5 is reserved space in following CFG_BLOCK_xx structure
    /// Total size available for data in following CFG_BLOCK_xxx structure in 16 byte multiple! Size in bytes = (size*16).
    /// Does not include CFG_BLOCK_HDR. Includes reserve[] array, which does not store data.
    BYTE    size;
} CFG_BLOCK_HDR;



///////////////////////////////////////////////////////////////////////////////
/////////////// Table Structure (TABLE_STRUCT) defines to follow //////////////
//The TABLE_STRUCT is a structure defining static constants.
//They can not be change, like the CFG_STRUCT.

//Table block ID, use range 0x8000 to 0xefff.
#define TABLE_BLOCK_UC66_PORT_ID        0x8200  ///< UC port ID for SBC66 boards
#define TABLE_BLOCK_SBC66_PORT_ID       0x8220  ///< IO Ports for SBC66 Boards

/**
 * Header for all TABLE_BLOCK_xxx structures. Must be same as CFG_BLOCK_HDR
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    WORD    id;         ///< ID of this TABLE_BLOCK, is a TABLE_BLOCK_xxx_ID constant
    BYTE    reserved;  ///< Bits 0-5 is reserved space in following CFG_BLOCK_xx structure
    /// Total size available for data in following TABLE_BLOCK_xxx structure in 16 byte multiple! Size in bytes = (size*16).
    /// Does not include TABLE_BLOCK_HDR. Includes reserve[] array, which does not store data.
    BYTE    size;
} TABLE_BLOCK_HDR;


/**
 * Get's size of CFG_BLOCK element in CFG_STRUCT, minus the reserve bytes.
 */
#define CFG_BLOCK_SIZE(cfgBlk) (sizeof (((CFG_STRUCT*) 0)->cfgBlk) - sizeof (((CFG_STRUCT*) 0)->cfgBlk.reserve))



///////////////////////////////////////////////////////////////////////////////
/////////////////////////// General Defines and Data //////////////////////////

/**
 * Header for CFG_STRUCT structures.
 */
typedef struct __attribute__((__packed__))
{
    CFG_BLOCK_HDR   hdr;
    
    //Size of configuration data part of CFG_STRUCT. The last part of CFG_STRUCT is TABLE_BLOCKs, and is
    //not part of the configuration data.
    WORD            confSize;

    /**
     * Is used to determine if EEPROM configuration has been updated yet. Is always set to 0xff.
     * In application, will check if this value is 0xff. If so, we know new configuration data is
     * available. After updating external EEPROM with new data, the lower byte is set to 0.
     * When CFG_STRUCT_IN_CODE is defined, CFG_STRUCT will be in program FLASH.
     * When CFG_STRUCT_IN_CODE is NOT defined, CFG_STRUCT will be in external FLASH (be placed there
     * by bootloader).
     */
    BYTE            fillConfUpdated;    //Can maybe use for adding to 'confSize', to make it 24 bits
    volatile BYTE   eeConfUpdated;

    BYTE            reserve[24];            ///< Reserve, and get block size on 32 byte multiple.
} CFG_BLOCK_INFO;


/**
 * Firmware flags
 */
typedef struct __attribute__((__packed__))
{
    union {
        struct
        {
            unsigned char bCurr : 2;     //Indicates which is current firmware. 0=Unknown, 1=FirmwareA, 2=FirmwareB
            unsigned char bFill : 6;     //Fill to ensure flags take up a 8-bit WORD
        } bits;
        BYTE val;
    } flags;
} FIRMWARE_FLAGS;
#define CURRENT_FIRMWARE_UNKNOWN 0
#define CURRENT_FIRMWARE_A 1
#define CURRENT_FIRMWARE_B 2

/////////// Global variables ////////////////////
#ifndef THIS_IS_NZ_APPCONFIG_C
    extern FIRMWARE_FLAGS   firmwareFlags;
#endif


//External EEPROM Memory defines. IMPORTANT!!! Ensure all start addresses are on 32 byte boundaries
#define XEEMEM_CFG_STRUCT_SIZE          0x0C00      //3 kBytes

#define XEEMEM_CFG_INFO_START_ADR       0x0000
#define XEEMEM_CFG_INFO_END_ADR         0x001F
#define XEEMEM_CFG_COPY_START_ADR       0x0020
#define XEEMEM_CFG_COPY_END_ADR         0x03FF
#define XEEMEM_CFG_COPY_STOP_ADR        0x0400  /* Address of first byte outside of CFG_COPY area */
#define XEEMEM_CFG_COPY_BLOCKS          ((XEEMEM_CFG_COPY_END_ADR-XEEMEM_CFG_COPY_START_ADR+1)/32)
#define XEEMEM_CFG_STRUCT_START_ADR     0x0400
#define XEEMEM_CFG_STRUCT_END_ADR       (XEEMEM_CFG_STRUCT_START_ADR + XEEMEM_CFG_STRUCT_SIZE - 1)
#define XEEMEM_USER_START_ADR           (XEEMEM_CFG_STRUCT_START_ADR + XEEMEM_CFG_STRUCT_SIZE)
#define XEEMEM_USER_END_ADR             (XEEPROM_SIZE - 1)

/**
 * Increment CFG_COPY address. Can use a mask if XEEMEM_CFG_COPY_END_ADR is all '1' on right
 */
#define cfgIncCfgCopyAdr(adr) ((adr>=(XEEMEM_CFG_COPY_STOP_ADR-32)) ? XEEMEM_CFG_COPY_START_ADR : (adr+32))


/**
 * Structure used to check if EEPROM contains valid data
 */
typedef struct __attribute__((packed))
{
    WORD_VAL    magicNumber;    /// Must have the value 0x536A to be identified as valid data
    WORD_VAL    cfgVersion;     /// Version of configuration data
    WORD_VAL    cfgChecksum;    /// Checksum
    WORD_VAL    cfgCRC;         /// CRC Checksum
} CFG_INFO;
#define CFG_INFO_MAGIC_NUMBER  0x536A
#define CFG_VERSION_MAJOR       1
#define CFG_VERSION_MINOR       0

#define CFG_COPY_DATA_SIZE 26
/**
 * @section info_conf_app_cfgcopy Config Copy
 * Each time a setting is updated, it is first written to the @ref CFG_COPY structure. On startup, appConfig will check if the last
 * entry(s) in this area is(are) consistent with value in EEPROM. Only whole 32 byte blocks are used.
 * The format of each 32 byte block is:
 *@li 0:     Incrementing number, even number from 0 to 254.
 *@li 1-2:   Setting address in External EEPROM (0xfffe indicates multiple blocks to follow)
 *@li 3:     Size of Data. If larger than 26, consecutive blocks are used.
 *@li 4:     CRC
 *@li 5-30   Data, max of 26 bytes
 *@li 31:    Incrementing number + 1
 *
 * On startup, appConfig will look for 32 byte block with last "Incrementing number". To confirm contents is
 * valid, last byte of block must be "Incrementing number + 1" - this indicates that whole block was successfully
 * written. Maximum data is 26 bytes. If more is required, consecutive blocks are used. Following is possible:
 *
 * ----- Single Block ----
 * - Setting Address gives address of setting, size has value from 1-26. All data contained in single block
 *
 * ----- Multiple Blocks, Non-Contiguous data ----
 * - First block contains NO data! Address=0xfffe. Size=number of blocks to follow with data.
 * - Following blocks are normal "Single Block". For data to be valid, ensure all following blocks are there!
 */
typedef struct __attribute__((packed))
{
    BYTE        IncNum;
    WORD        Address;
    BYTE        Size;
    BYTE        CRC;
    BYTE        Data[CFG_COPY_DATA_SIZE];
    BYTE        IncNumPlusOne;
} CFG_COPY;


/**
 * @preCondition  fsysInit() is already called.
 */
void cfgInit(BYTE* pCfgStructInfo);

/**
 * Validate if the contents of the external EEPROM is valid. If not, it is initialized with
 * the default configuration.
 * @return 0 if not valid, else non 0 value
 */
BYTE cfgIsXeepromValid(void);

/**
 * Invalidate external EEPROM. Done by setting magic number to 0. On reboot, contents of External
 * EEPROM will be reloaded.
 */
void cfgInvalidateXeeprom(void);

/**
 * Copy array of bytes from given offset in DEFAULT cfgStruct to given destination buffer (buf).
 * The default values for cfgStruct is returned, NOT THE CURRENT application configuration
 * values! The current application configuration values are stored in the external EEPROM,
 * and are retrieved via the cfgGetArray() function!
 *
 * @param offset Offset in cfgStruct to use as source
 * @param buf Destination buffer
 * @param length Number of bytes to copy
 *
 * @return Returns number of bytes copied
 *
 */
BYTE cfgGetDfltArray(WORD offset, BYTE* buf, BYTE length);

/**
 * Copy given member structure (structure in CFG_STRUCT) to destination structure.
 * The default values for cfgStruct is returned, NOT THE CURRENT application configuration
 * values! The current application configuration values are stored in the external EEPROM,
 * and are retrieved via the cfgGetStruct() function!
 * For example:
 *  cfgGetDfltStruct(ser.avail, serInfo.avail);
 *
 * @param member Requested member in CFG_STRUCT
 * @param struc Destination structure to copy requested structure to
 */
#define cfgGetDfltStruct(member, struc) cfgGetDfltArray(offsetof(CFG_STRUCT, member), ((BYTE*)&struc), sizeof(struc))

/**
 * Get array from the given Config Block (CFG_BLOCK_XXX). Is copied to given destination
 * buffer (buf).
 *
 * @param cfgBlkId The source Config Block (CFG_BLOCK_XXX) id, is a CFG_BLOCK_xxx_ID define,
 *        like CFG_BLOCK_NET_ID for example.
 * @param offset Offset of the array to return, in the given "Config Block"
 * @param buf Destination buffer
 * @param length Number of bytes to copy
 *
 * @return Returns number of bytes copied
 *
 */
BYTE cfgBlkGetArray(WORD cfgBlkId, WORD offset, BYTE* buf, BYTE length);

/**
 * Get array from Configuration Structure in the External EEPROM. Is copied to
 * given destination buffer (buf).
 *
 * @param offset Offset in cfgStruct to use as source
 * @param buf Destination buffer
 * @param length Number of bytes to copy
 *
 * @return Returns number of bytes copied
 *
 */
BYTE cfgGetArray(WORD offset, BYTE* buf, BYTE length);

/**
 * Copy given member structure in EEPROM cfgStruct to destination structure.
 * For example:
 *  cfgGetStruct(ser.avail, serInfo.avail);
 *
 * @param member Requested member in APP_CONFIG
 * @param struc Destination structure to copy requested structure to
 */
#define cfgGetStruct(member, struc) cfgGetArray(offsetof(CFG_STRUCT, member), ((BYTE*)&struc), sizeof(struc))


/**
 * Get a single byte in cfgStruct structure.
 *
 * @param offset Offset in cfgStruct of requested byte
 *
 * @return Returns requested byte
 *
 */
//#define cfgGetByte(offset) (*(((BYTE*)&cfgStruct) + offset));
#define cfgGetByte(offset) (((BYTE*)&cfgStruct)[offset]);

/**
 * Saves a single by to the Configuration Structure of the External EEPROM.
 *
 * @param offset Contains the offset in the CFG_STRUCT structure of the byte to save
 * @param val Byte to save
 */
void cfgSaveByte(WORD offset, BYTE val);

/**
 * Saves the given array in CFG_STRUCT configuration (external EEPROM) with the given data.
 * The array is securely saved, ensuring EEPROM is not corrupted if power faulure occurs
 * during before done. This is achieved by first saving a copy to the CFG_COPY area of
 * the EEPROM, and then saving the actual value in CFG_STRUCT.
 *
 * @param offsetCfg Contains the offset in the APP_CONFIG structure of the array to save
 * @param buf Source data to save
 * @param length Number of bytes to save
 */
void cfgSaveArray(WORD offsetCfg, BYTE* buf, WORD length);

/**
 * Save Application Configuartion data to EEPROM
 */
void cfgSave(void);

#endif
