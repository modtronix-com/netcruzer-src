/*********************************************************************
 * nz_appConfigXEE.c - Application Configuration functions
 * Dependencies:    
 * Complier:        MPLAB XC16
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Application configuration.
 *
 *********************************************************************
 * File History
 *
 * 2010-11-16, David H. (DH):
 *    - Initial version
 *
 *********************************************************************
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
 *
 ********************************************************************/
#define THIS_IS_NZ_APPCONFIG_C

#include "HardwareProfile.h"
#include "nz_appConfig.h"

#include "nz_xEeprom.h"
#include "nz_xFlash.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"
#include "nz_xflashDefsSbc66.h"

#include "appConfig.h"

//Add debugging to this file. The DEBUG_CONF_NZ_APPCONFIG macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_NZ_APPCONFIG)
    #define DEBUG_CONF_NZ_APPCONFIG        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_NZ_APPCONFIG
#include "nz_debug.h"


/////////////////////////////////////////////////
//Variables
BYTE            cfgCopyNextIncNum;
WORD            cfgCopyNextAdr;     //Address of where next CFG_COPY block must be written in External EEPROM, or 0 if uninitialized
FIRMWARE_FLAGS  firmwareFlags;


/////////////////////////////////////////////////
//Function Prototypes
void cfgInitCopy(void);


/**
 * Initializes configuration data
 */
void cfgInit(BYTE* pCfgStruct)
{
    FIRMWARE_INFO firmwareAInfo;
    FIRMWARE_INFO firmwareBInfo;
    WORD w;
    DWORD xflAdr;               //external flash address
    DWORD xflAdrConfig = 0;     //external flash address of 'FirmwareX Configuration'
    //DWORD xflAdrChksum = 0;   //external flash address
    struct {
        WORD    confSize;
        BYTE    fill;
        BYTE    eeConfUpdated;
    } confInfo;

    //First incNum to use when writing CFG_COPY blocks. Will be updated in cfgInitCopy() function to (incNum of last found valid block)+2
    cfgCopyNextIncNum = 0;
    cfgCopyNextAdr = XEEMEM_CFG_COPY_START_ADR; //Initialize with first CFG_COPY address


    /////////////////////////////////////////////////
    //Check external FLASH to see if FirmwareA or FirmwareB is the current firmware.
    firmwareFlags.flags.val = 0;
    //Check if "FirmwareA Info" or "FirmwareB Info" is current
    xflashReadArray(XFLASH_FIRMWAREA_INFO_ADR, ((BYTE*)&firmwareAInfo), sizeof(firmwareAInfo));
    xflashReadArray(XFLASH_FIRMWAREB_INFO_ADR, ((BYTE*)&firmwareBInfo), sizeof(firmwareBInfo));
    if ((firmwareAInfo.fwState.LB == FIRMWARE_STATE_CURRENT) && (firmwareAInfo.magicNumber.Val == FIRMWARE_INFO_MAGIC_NUMBER)) {
        firmwareFlags.flags.bits.bCurr = CURRENT_FIRMWARE_A;
        xflAdr = XFLASH_FIRMWAREA_ADR;                  //Address of 'FirmwareA' in external FLASH
        xflAdrConfig = XFLASH_FIRMWAREA_CONFIG;
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nFirmwareA current");
    }
    else if ((firmwareBInfo.fwState.LB == FIRMWARE_STATE_CURRENT) && (firmwareBInfo.magicNumber.Val == FIRMWARE_INFO_MAGIC_NUMBER)) {
        firmwareFlags.flags.bits.bCurr = CURRENT_FIRMWARE_B;
        xflAdr = XFLASH_FIRMWAREB_ADR;                  //Address of 'FirmwareB' in external FLASH
        xflAdrConfig = XFLASH_FIRMWAREB_CONFIG;
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nFirmwareB current");
    }
    else {
        //Not needed, "firmwareFlags.flags.val = 0;" above does this
        //firmwareFlags.flags.bits.bCurr = CURRENT_FIRMWARE_UNKNOWN;

        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nNo Firmware on FLASH!");
    }


    /////////////////////////////////////////////////
    //New Configuration data is available. We check if the CFG_STRUCT_INFO.eeConfUpdated byte in CFG_STRUCT = 0xff.
    //This is the case after the firmware was updated. This code will update the application configuration
    //in the external EEPROM with this new data, and write 0 to info.eeConfUpdated.
    //- CFG_STRUCT is located in internal FLASH if CFG_STRUCT_IN_CODE is defined
    //- CFG_STRUCT is located in external FLASH if CFG_STRUCT_IN_CODE is NOT defined
    #if defined(CFG_STRUCT_IN_CODE)
        confInfo.confSize =  ((CFG_BLOCK_INFO*)pCfgStruct)->confSize;
        confInfo.eeConfUpdated = ((CFG_BLOCK_INFO*)pCfgStruct)->eeConfUpdated;
    #else
        //External Flash contains configuatioin data
        if (firmwareFlags.flags.bits.bCurr != CURRENT_FIRMWARE_UNKNOWN) {
            //Read info.eeConfUpdated value
            xflashReadArray(xflAdrConfig + offsetof(CFG_BLOCK_INFO, confSize), ((BYTE*)&confInfo), sizeof(confInfo));
        }
    #endif

    //Do sanity check on confInfo.confSize
//    if (confInfo.confSize > sizeof(CFG_STRUCT)) {
//        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nERROR! confSize too large!");
//        return;
//    }

    //New configuration data is available!
    if (confInfo.eeConfUpdated == 0xff) {
        CFG_INFO cfgInfo;
        static ROM BYTE * pCfg;

        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nNew Config - ");
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, confInfo.confSize);
        //Update external EEPROM configuration data. Existing data is NOT overwritten.
        //TODO - Config upgrade.
        // - Determine if there is a valid "Previous CFG_STRUCT". Can be located in either XFLASH_FIRMWAREX_CONFIG or XFLASH_MODIFIED_CONFIG
        // - Overwrite external EEPROM contents with new Config data (xflAdrConfig)
        // - If old config is to be preserved, Iterate through all CFG_BLOCKs in "Previous CFG_STRUCT". For each one found, we
        //   write it's data to new CFG_BLOCK.


        //Invalidata all CFG_COPY blocks. Set all bytes to 0xff.s
        xeeBeginWrite(XEEMEM_CFG_COPY_START_ADR);  //Write incNum
        for (w=(XEEMEM_CFG_COPY_BLOCKS*32); w!=0; w--) {
            xeeWrite(0xff);
        }
        xeeEndWrite();

        #if defined(CFG_STRUCT_IN_CODE)
            //TODO - Replace with "write array" function. Check if this works with "const"

            //Write CFG_STRUCT to External EEPROM
            xeeBeginWrite(XEEMEM_CFG_STRUCT_START_ADR);
            pCfg = (const BYTE*)pCfgStruct;
            for (w=confInfo.confSize; w!=0; w--) {
                xeeWrite(*pCfg++);
            }
            xeeEndWrite();
        #else
        #endif

        //Write the CFG_INFO struct to the External EEPROM. Do this Last, so if power fail occurs, CFG_INFO
        //is only written after everything else. Once CFG_INFO is written, EEPROM is marked as valid.
        cfgInfo.magicNumber.Val = CFG_INFO_MAGIC_NUMBER;
        cfgInfo.cfgVersion.Val = 0x0101;    //Version 1.01
        cfgInfo.cfgCRC.Val = 0;
        cfgInfo.cfgChecksum.Val = 0;
        xeeBeginWrite(XEEMEM_CFG_INFO_START_ADR);
        xeeWriteArray((BYTE*) &cfgInfo, sizeof(cfgInfo));
        xeeEndWrite();

        //When done, set eeConfUpdated to 0 to mark that external EEPROM configuration data has been updated.
        #if defined(CFG_STRUCT_IN_CODE)
            //Write 0 to given address in FLASH
            nzFlashWriteConst(__builtin_tbladdress(&cfgStruct.info.eeConfUpdated), 0);
        #else
            //External Flash contains configuatioin data
            if (firmwareFlags.flags.bits.bCurr != CURRENT_FIRMWARE_UNKNOWN) {
                //Write 0 to info.eeConfUpdated. No erase is required, seeing that FLASH value can always be changed from 1 to 0 (0xff to 0)
                xflashBeginWrite(xflAdrConfig + offsetof(CFG_BLOCK_INFO, eeConfUpdated));
                xflashWrite(0);
            }
        #endif
    }


    /////////////////////////////////////////////////
    //External EEPROM Contains valid configuration data
    if (cfgIsXeepromValid()) {
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nXEE Valid");

        //Check "Config Copy" area of external EEPROM.
        cfgInitCopy();
    }
    //External EEPROM does NOT contains valid EEPROM data. Restore with default data.
    else {
        CFG_INFO cfgInfo;
        static ROM BYTE * pCfg;
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nXEE NOT Valid");

        //Configuration data is located in Program Memory
        #if defined(CFG_STRUCT_IN_CODE)
        {
            //Invalidata all CFG_COPY blocks. Set all bytes to 0xff.s
            xeeBeginWrite(XEEMEM_CFG_COPY_START_ADR);  //Write incNum
            for (w=(XEEMEM_CFG_COPY_BLOCKS*32); w!=0; w--) {
                xeeWrite(0xff);
            }
            xeeEndWrite();

            //Write CFG_STRUCT to External EEPROM
            xeeBeginWrite(XEEMEM_CFG_STRUCT_START_ADR);
            pCfg = (const BYTE*)pCfgStruct;
            for (w=confInfo.confSize; w!=0; w--) {
                xeeWrite(*pCfg++);
            }
            xeeEndWrite();

            //Write the CFG_INFO struct to the External EEPROM. Do this Last, so if power fail occurs, CFG_INFO
            //is only written after everything else. Once CFG_INFO is written, EEPROM is marked as valid.
            cfgInfo.magicNumber.Val = CFG_INFO_MAGIC_NUMBER;
            cfgInfo.cfgVersion.Val = 0x0101;    //Version 1.01
            cfgInfo.cfgCRC.Val = 0;
            cfgInfo.cfgChecksum.Val = 0;
            xeeBeginWrite(XEEMEM_CFG_INFO_START_ADR);
            xeeWriteArray((BYTE*) &cfgInfo, sizeof(cfgInfo));
            xeeEndWrite();

            //debugPrintFlash(xflAdrConfig, 32);
        }
        //Configuration data is (possibly) located in external FLASH. Determine if there is a valid "Previous CFG_STRUCT".
        //Can be located in either XFLASH_FIRMWAREX_CONFIG or XFLASH_MODIFIED_CONFIG. Overwrite external EEPROM contents with default
        //Config data (xflAdrConfig)
        #else
            // TODO Implement this!
            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nNO CONFIGURATION DATA!!!");
        #endif

    }
}


/**
 * Function called during intialization to check "Config Copy". The "Config Copy" is a
 * area in the external EEPROM that is used to ensure the "Config Structure" contains valid
 * data.
 */
void cfgInitCopy(void)
{
    CFG_COPY cfgCopy;
    BYTE i;
    BYTE incNumLast;    //Last incNum. Is not always highest number. Some times it resets from 254 to 0.
    WORD xeeAdr;        //External EEPROM address
    BYTE iBlocks;       //Used for counting how many blocks are to follow for "Multiple Block"

    BYTE loopType;
    #define FIND_START 0
    #define FIND_NEWEST 1
    #define CHECK_CFG_STRUCT 2

    //Address of oldest block. Must start search for newest block from here.
    WORD adrOldestBlock;

    //Address of last written (newest) valid block(s). Is only valid when validBlockCount != 0
    // - For "Single Block", this is the last written block.
    // - For "Multiple Block", this is the address of first data block of "Multiple Block" group. First block of "Multiple Block" contains no data!s
    WORD adrValidBlock;

    //Gives number of valid blocks to compare to CFG_STRUCT. Address of first valid block is in adrValidBlock
    BYTE validBlockCount;
    BYTE validBlockCountSave=0;

    incNumLast = 0xff;      //Initialize with invalid number. Must be even number.
    validBlockCount = 0;    //No valid blocks have been found yet
    iBlocks = 0;
    adrValidBlock = 0xffff;
    adrOldestBlock = 0xffff;

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ncfgInitCopy");

    //Invalidata all CFG_COPY blocks. Is done by setting first byte of each block (IncNum)
    //to 0xff. IncNum must be an even number.
    xeeAdr = XEEMEM_CFG_COPY_START_ADR;

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nBUFFFREE: ");
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cbufGetFree(CIRBUF_TX_DEBUG));

    //Loop through all copy blocks twice.
    // - In first loop we search for oldest valid block written. Will start second loop from here.
    // - In second loop we search for last (newest) valid block written. If last entry is "Multiple Block" group,
    //   we search for first block of this "Multiple Block" group.
    loopType = FIND_START;  //Indicate this is first loop where we search for oldest valid block written
    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nLook 4 Start");
    i=XEEMEM_CFG_COPY_BLOCKS;
    do {
        xeeReadArray(xeeAdr, (BYTE *)&cfgCopy, sizeof(cfgCopy));

        //Check if this is a valid block
        if (cfgCopy.IncNumPlusOne == cfgCopy.IncNum + 1) {
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nValid Blk");

            //This is first loop through all copy blocks. Search for place to start second loop from = oldest valid block.
            if (loopType == FIND_START) {

                if ( ((BYTE)(incNumLast+2)) != cfgCopy.IncNum ) {
                    adrOldestBlock = xeeAdr;     //Save address of current copy block
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nFnd ");
                    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, xeeAdr);
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Inc=");
                    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cfgCopy.IncNum);
                }
                else {
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n Not ");
                    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, xeeAdr);
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Inc=");
                    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cfgCopy.IncNum);
                }
                incNumLast = cfgCopy.IncNum;
            }
            //This is the second loop through all copy blocks. Find newest block. Write it's External EEPROM address to adrValidBlock.
            //A value of 0xffff means it was not valid. Update validBlockCount with number of valid blocks.
            else {
                //This is the newest block found
                // - This is first block read, there is no previous incNum
                // - Check if incNum is incremented from last incNum (incNumLast)
                if ( (incNumLast==0xff) || (((BYTE)(incNumLast+2))==cfgCopy.IncNum) ) {
                    incNumLast = cfgCopy.IncNum;

                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n Fnd ");
                    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, xeeAdr);
                    
                    //Remember in case this is next incNum we have to use when writing to CFG_COPY area. Is needed in other parts of code.
                    cfgCopyNextIncNum = incNumLast+2;
                    cfgCopyNextAdr = cfgIncCfgCopyAdr(xeeAdr);

                    //We are currently reading data blocks of a "Multiple Block". Confirm all data blocks are present!
                    if (iBlocks != 0) {
                        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, ".");             //Multiple block

                        //All data blocks found for this "Multiple Block". Set validBlockCount to indicate how many valid block are located at adrValidBlock
                        if (--iBlocks == 0) {
                            validBlockCount = validBlockCountSave;

                            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n MB Fnd ");    //Multiple block found
                            //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, adrValidBlock);
                            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Cnt=");    //Multiple block found
                            //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, validBlockCount);
                        }

                        //This is the first data block of "Mutiple Block"s. Save it's address
                        if (adrValidBlock == 0xffff) {
                            adrValidBlock = xeeAdr;
                        }
                    }
                    else {
                        adrValidBlock = xeeAdr;

                        //"Multiple Block" found. This block contains no data, but indicates how many
                        //blocks have to follow.
                        if (cfgCopy.Address == 0xfffe) {
                            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n MB Sz=");     //Multiple block found
                            //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cfgCopy.Size);
                            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " ");     //Multiple block found

                            validBlockCountSave = cfgCopy.Size; //Give number of data blocks contained in "Multiple Block"
                            iBlocks = validBlockCountSave;
                            validBlockCount = 0;                //Set to number of data blocks (validBlockCountSave) AFTER all blocks have been confirmed as valid and present
                            adrValidBlock = 0xffff;             //This block contains no data, set adrValidBlock to address of next block (is done in next loop)
                        }
                        else {
                            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n SB ");    //Single Block founds
                            //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, xeeAdr);
                            validBlockCount = 1;                        //Single block. Not "Multiple Block"
                        }
                    }
                }
                //End of second search loop!
                else {
                    break;
                }
            }
        }
        else {
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nInvld Blk ");
            //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, xeeAdr);
        }

        //Increment xeeAdr to next CFG_COPY block, or reset to start
        xeeAdr = cfgIncCfgCopyAdr(xeeAdr);

        //Check if we have looped through all copy blocks
        if (--i == 0) {
            //This was search loop.
            if (loopType == FIND_START) {
                loopType = FIND_NEWEST;
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nLook 4 New");
    
                //Check if search loop found any valid blocks.
                // - If it did, we initiate second loop and search for newest blocks
                // - ELSE, if not, i is NOT reset, and this do {} will exit below. Second loop never occurs.
                if (adrOldestBlock != 0xffff) {
                    incNumLast = 0xff;          //Initialize with invalid number. Must be even number.
                    i=XEEMEM_CFG_COPY_BLOCKS;   //Search all copy blocks for newest block
                    xeeAdr = adrOldestBlock;    //Start search from oldest block found
                }
                else {
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCFG_COPY contains no data");
                }
            }
        }
    } while (i !=0 );

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nVrfy ");
    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, validBlockCount);
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Blks");

    //At this stage validBlockCount gives number of valid blocks found (adrValidBlock = address). These are the newest "Config Copy" blocks
    //written, and the contents of the CFG_STRUCT must be verified with their data.
    while (validBlockCount-- != 0) {
        BYTE bufData[26];   //Contents of CFG_STRUCT

        //Read CFG_COPY block.
        xeeReadArray(adrValidBlock, (BYTE *)&cfgCopy, sizeof(cfgCopy));

        //Check size is less then 26
        if (cfgCopy.Size > 26) {
            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nCFG_COPY size > 26");
            continue;
        }

        //Read contents of CFG_STRUC pointed to by current CFG_COPY block
        xeeReadArray(cfgCopy.Address, bufData, cfgCopy.Size);

        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n BlkAdr=");
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, adrValidBlock);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Adr=");
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cfgCopy.Address);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Sz=");
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cfgCopy.Size);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " D=");

        //Ensure CFG_STRUCT matches values of CFG_COPY block
        for (i=0; i<cfgCopy.Size; i++) {
            DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, bufData[i]);
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " ");

            if (cfgCopy.Data[i] != bufData[i]) {
                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nCFG_STRUCT invalid data");

                //Fix it
                xeeBeginWrite(cfgCopy.Address + i);
                xeeWrite(cfgCopy.Data[i]);
                xeeEndWrite();
            }
        }

        //Increment xeeAdr to next CFG_COPY block, or reset to start
        adrValidBlock = cfgIncCfgCopyAdr(adrValidBlock);
    }
}


/**
 * Copy array of bytes from given offset in DEFAULT cfgStruct to given destination buffer (buf).
 * Default cfgStruct can be located in Program Memory OR External Flash
 * - If CFG_STRUCT_IN_CODE is defined, cfgStruct is located in Program Memory.
 * - If CFG_STRUCT_IN_CODE is NOT defined, cfgStruct is located in Program Memory. Can be located
 *   in either XFLASH_FIRMWAREX_CONFIG or XFLASH_MODIFIED_CONFIG.
 *
 * The default values for cfgStruct is returned, NOT THE CURRENT application configuration
 * values! The current application configuration values are stored in the external EEPROM,
 * and are retrieved via the cfgGetArray() function!
 *
 * @param offset Offset in cfgStruct to use as source
 * @param buf Destination buffer
 * @param length Number of bytes to copy
 *
 * @return Returns number of bytes copied
 */
BYTE cfgGetDfltArray(WORD offset, BYTE* buf, BYTE length) {
    ROM BYTE* p;    //Compiler can read constants located in FLASH
    BYTE count;
    unsigned char i;

    count = 0;

    //Configuration data is located in Program Memory
    #if defined(CFG_STRUCT_IN_CODE)

    //Get pointer to source
    p = ((ROM BYTE*)&cfgStruct) + offset;

    for (i = 0; i < length; i++) {
        count++;
        *buf++ = *p++;
    }

    //Return bytes read
    return count;

    //Configuration data is (possibly) located in external FLASH. Determine if there is a valid "Previous CFG_STRUCT".
    //Can be located in either XFLASH_FIRMWAREX_CONFIG or XFLASH_MODIFIED_CONFIG. Overwrite external EEPROM contents with default
    //Config data (xflAdrConfig)
    #else
        // TODO Implement this
        #error "cfgGetDfltArray() not implemented yet for when CFG_STRUCT_IN_CODE is not defined"
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nNO CONFIGURATION DATA!!!");
    #endif
}

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
BYTE cfgBlkGetArray(WORD cfgBlkId, WORD offset, BYTE* buf, BYTE length) {
    return 0;
}


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
BYTE cfgGetArray(WORD offset, BYTE* buf, BYTE length) {
    //Read array of data from external EEPROM
    xeeReadArray(XEEMEM_CFG_STRUCT_START_ADR + offset, buf, length);

    //Return bytes read
    return length;
}


/**
 * Saves a single by to the Configuration Structure of the External EEPROM.
 *
 * @param offset Contains the offset in the CFG_STRUCT structure of the byte to save
 * @param val Byte to save
 */
void cfgSaveByte(WORD offset, BYTE val) {

}

/**
 * Saves the given array in CFG_STRUCT configuration (external EEPROM) with the given data.
 * The array is securely saved, ensuring EEPROM is not corrupted if power failure occurs
 * while busy. This is achieved by first saving a copy to the CFG_COPY area of
 * the EEPROM, and then saving the actual value in CFG_STRUCT.
 *
 * This function will only save data that has changed. If no data has changed, nothing is saved
 * to CFG_COPY or CFG_STRUCT.
 *
 * @param offsetCfg Contains the offset in CFG_STRUCT of given array
 * @param buf Source array of data to save
 * @param length Number of bytes to save
 */
void cfgSaveArray(WORD offsetCfg, BYTE* buf, WORD length) {
    #define LOOP_COUNT_BLOCKS   0
    #define LOOP_WRITE_CONFIG   1
    #define BLOCKS_COUNT        10  //Maximum number of blocks we can save
    BYTE loopType = LOOP_COUNT_BLOCKS;
    BYTE reading = 0;
    BYTE c;

    WORD offsetBuf;         //Offset in given array
    WORD xeeAdr;
    WORD xeeAdrStop;        //Address AFTER Last address in External EEPROM of givan array. First address that is NOT part of given array
    WORD xeeAdrLastToSave;  //Address of last byte in External EEPROM that must be updated. This is the last CFG_STRUCT byte that does not match value in given array.
    BYTE blocks = 0;        //Indicate how many CFG_COPY blocks are required to save given array
    WORD adrCopyBlocks[BLOCKS_COUNT];     //External EEPROM Addresses of CFG_COPY blocks required to save given array
    BYTE i;

    WORD xeeAdrSaveStart;
    WORD xeeAdrSaveEnd;
    WORD offsetBufSaveStart;

    CFG_COPY cfgCopy;

    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
    sprintf(debugTempBuf, "\nncfgSaveArray() offsetCfg=0x%x(%d), length=%d Data: ", offsetCfg, offsetCfg, length);
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, debugTempBuf);
    for(i=0; i<length; i++) {
        sprintf(debugTempBuf, "0x%x(%d) ", buf[i], buf[i]);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, debugTempBuf);
    }
    #endif

    xeeAdrSaveStart = 0;    //Initialize with 0
    xeeAdrSaveEnd = 1;      //Initialize with 1. Will not satisfy test of address at end of EEPROM page
    offsetBufSaveStart = 0;

    xeeAdr = XEEMEM_CFG_STRUCT_START_ADR + offsetCfg;
    xeeAdrStop = xeeAdr + length;
    xeeAdrLastToSave = 0;
    offsetBuf = 0;
    do {
        if (reading==0) {
            reading=1;
            xeeBeginRead(xeeAdr);
        }

        //During first loop we count how many blocks are required to save all changed
        //data. Each block can hold CFG_COPY_DATA_SIZE bytes.
        if (loopType==LOOP_COUNT_BLOCKS) {
            //Given data and current data in CFG_STRUCT do not match
            if (buf[offsetBuf] != (c=xeeRead())) {
                //ERROR!!! Given array is too large
                if (blocks >= BLOCKS_COUNT) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncfgSaveArray() too large!");
                    break;
                }

                xeeAdrLastToSave = xeeAdr;          //Save, this could be last mismatch = last address to save

                //This is first mismatch found. Mark as first block.
                if (blocks == 0) {
                    //Save current address. This is start address of first CFG_COPY block
                    adrCopyBlocks[blocks++] = xeeAdr;

                    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
                    sprintf(debugTempBuf, "\nRsrvd Blk for: %d (new=%d, cur=%d)", xeeAdr, buf[offsetBuf], c);
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, debugTempBuf);
                    #endif
                }
                //This is the first block, we have to check if all mismatched data can fit into a "Single Block"
                else if (blocks == 1) {
                    //Can not fit into "Single Block". Save next adrCopyBlocks, which means we now need "Multiple Blocks"
                    if ( (xeeAdr-adrCopyBlocks[0]) >= CFG_COPY_DATA_SIZE ) {
                        //We must skip to next block. Indicated by setting reading=0
                        reading=0;
                    }
                    //All mismatched data still fits into a single block
                    else {
                        //Already done above
                        //xeeAdrLastToSave = xeeAdr;
                    }
                }
                //This is NOT the first block, we need "Multiple Blocks".
                else {
                    //We must skip to next block. Indicated by setting reading=0
                    reading=0;
                }

                //We have to skip to next possible block (indicated by reading==0).
                if (reading==0) {
                    //Save current address. This is start address of a CFG_COPY block
                    adrCopyBlocks[blocks++] = xeeAdr;

                    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
                    sprintf(debugTempBuf, "\nRsrvd Mul Blk for: %d (new=%d, cur=%d)", xeeAdr, buf[offsetBuf], c);
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, debugTempBuf);
                    #endif

                    //Each CFG_COPY block can save CFG_COPY_DATA_SIZE bytes. Increment xeeAdr to last byte we can save in this block.
                    //We will start searching for next byte in array that has changed, from xeeAdr+1 (xeeAdr is incremented below).  When found,
                    //that position is marked as start of next CFG_COPY block.
                    //xeeAdr and offsetBuf are both incremented below, the values set in next two lines are both 1 less then what they should be.
                    xeeAdr = xeeAdr + CFG_COPY_DATA_SIZE - 1;
                    offsetBuf = offsetBuf + CFG_COPY_DATA_SIZE - 1;

                    //Save External EEPROM address of last postion that has to be saved to CFG_STRUCT. We do not really test all bytes from current
                    //xeeAdr to xeeAdrLastToSave, but know that xeeAdrLastToSave is the last possible bytes that could be saved. In next iteration we start
                    //comparing bytes from xeeAdrLastToSave+1, and if a mismatch if found, xeeAdrLastToSave will be updated again.
                    xeeAdrLastToSave = (xeeAdr >= xeeAdrStop) ? (xeeAdrStop-1) : xeeAdr;

                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nxeeAdrLastToSave=");
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, xeeAdrLastToSave);

                    //End current read, and begin reading at new xeeAdr position
                    xeeEndRead();
                    //reading=0;    //Already 0
                }
            }
        }
        //During second loop we save given array to CFG_STRUCT. To make process much faster, following is done:
        // - We do not call xeeWrite() function for each byte. This will be very slow, and cause a EEPROM
        //   write delay (normally about 5ms) each time.
        // - We check when data in given array and current EEPROM data do not match. From this address we find
        //   last address (in that EEPROM page) where data does not match. This block of data is within a single
        //   EEPROM page, and is written to the external EEPROM with the xeeWriteArray() function. This causes a 
        //   single EEPROM write delay.
        else if (loopType==LOOP_WRITE_CONFIG) {
            //Given data and current data in CFG_STRUCT do not match.
            if (buf[offsetBuf] != xeeRead()) {

                //Each time data does not match, update address of were we will save data to for next xeeWriteArray() function call
                xeeAdrSaveEnd = xeeAdr;

                //This is the first mismatch found - save External EEPROM address. This must be written to CFG_STRUCT
                if (xeeAdrSaveStart == 0) {
                    xeeAdrSaveStart=xeeAdr;
                    offsetBufSaveStart=offsetBuf;
                }
            }

            //Update value in External EEPROM if:
            // - There is something to save (xeeAdrSaveStart != 0)  AND
            //   - This was last byte of an EEPROM page
            //   OR
            //   - This was last byte that had to be saved, and last iteration of loop. Have to save now, seeing that this is last iteration.
            if( (xeeAdrSaveStart != 0) &&
                ((((xeeAdr+1) & (XEEPROM_PAGE_SIZE-1)) == 0u) || (xeeAdr >= xeeAdrLastToSave)) )
            {
                xeeEndRead();
                reading = 0;    //Indicate we are currently NOT reading

                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nSvng CFG_STRUCT Adr: ");
                DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, xeeAdrSaveStart);
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " to ");
                DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, xeeAdrSaveEnd);

                xeeBeginWrite(xeeAdrSaveStart);
                xeeWriteArray(&buf[offsetBufSaveStart], xeeAdrSaveEnd-xeeAdrSaveStart+1);

                xeeAdrSaveStart = 0;    //Initialize with 0
                xeeAdrSaveEnd = 1;      //Initialize with 1. Will not satisfy test of address at end of EEPROM page
                offsetBufSaveStart = 0;
            }
        }


        //Increment to next position in buf[] parameter passed to this function
        offsetBuf++;

        //Finished reading all External EEPROM positions
        if (++xeeAdr >= xeeAdrStop) {
            if (reading)
                xeeEndRead();
            reading=0;

            if (loopType==LOOP_COUNT_BLOCKS) {
                BYTE hasData;   //Does current block have data

                //Given array is identical to current CFG_STRUCT. Nothing to do, return
                if (blocks == 0) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nNthng to save");
                    break;
                }

                /////////////////////////////////////////////////
                //The following code saves the data marked by adrCopyBlocks to CFG_COPY:
                // - A "Single Block" of CFG_COPY, if blocks == 1
                // - To "Multiple Blocks" if blocks > 1
                for (i=0; i<=blocks; i++) {
                    //Check if this block has data. Only first block of "Multiple Blocks" does not have data.
                    if ( (i==0) && (blocks>1) )
                        hasData = FALSE;    //This is first block of "Multiple Block", does not have data.
                    else
                        hasData = TRUE;

                    //memclear(cfgCopy.Data, sizeof(cfgCopy.Data));       //Set all to 0

                    cfgCopy.IncNum = cfgCopyNextIncNum++;
                    cfgCopy.IncNumPlusOne = cfgCopyNextIncNum++;

                    if (hasData == TRUE) {
                        //For "Single Block", adjust i (from 0 to 1) so for() loop ends after this iteration. Also needed for code below.
                        if (blocks==1)
                            i++;

                        cfgCopy.Address = adrCopyBlocks[i-1];   //External EEPROM address of data to store

                        //Get offset of current block in given array. This functions 'offsetCfg' parameter gives the offset in CFG_STRUCT of given array.
                        // (cfgCopy.Address - XEEMEM_CFG_STRUCT_START_ADR) =  offset of cfgCopy.Address in CFG_STRUCT
                        // (cfgCopy.Address - XEEMEM_CFG_STRUCT_START_ADR) - offset = offset of cfgCopy.Address in given array
                        offsetBuf = cfgCopy.Address - XEEMEM_CFG_STRUCT_START_ADR - offsetCfg;

                        //This is the last block. This block (cfgCopy.Data element) is possibly not full with data. xeeAdrLastToSave gives last
                        //address to save. This block can be:
                        // - Only block of "Single Block"
                        // - Last block of "Multiple Block"
                        //TODO - We could calculate accurate size by searching the last byte (in given array) that is different to
                        //the contents of CFG_STRUCT. In most cases this will give a smaller cfgCopy.Size. But, does not have many
                        //benifits, because:
                        // - We are only writing a single block of EEPROM data, so takes same time
                        // - Will take just as long to iterate through CFG_STRUCT and search for last byte, then it does writing more byte to EEPROM.
                        if (i == blocks) {
                            cfgCopy.Size = (xeeAdrLastToSave + 1) - cfgCopy.Address;

                            //This is the case if there are many bytes at the end of the given array match the contents of CFG_STRUCT.
                            //They do not need to be save to CFG_COPY. They will not be counted in LOOP_COUNT_BLOCKS loop.
                            if (cfgCopy.Size > CFG_COPY_DATA_SIZE)
                                cfgCopy.Size = CFG_COPY_DATA_SIZE;
                        }
                        //This block if full with data. More blocks to follow. This is a "Multiple Block"
                        else {
                            cfgCopy.Size = CFG_COPY_DATA_SIZE;
                        }

                        memcpy(cfgCopy.Data, &buf[offsetBuf], cfgCopy.Size);
                    }
                    else {
                        cfgCopy.Address = 0xfffe;   //For "Multiple Blocks", address of first block must be 0xfffe
                        cfgCopy.Size = blocks;      //For "Multiple Blocks", size of first block states how many data blocks are to follow
                    }

                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nSvng CFG_COPY Inc: ");
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cfgCopy.IncNum);
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Adr: ");
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cfgCopy.Address);
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Sz: ");
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cfgCopy.Size);

                    //Write cfgCopy to External EEPROM
                    xeeBeginWrite(cfgCopyNextAdr);
                    xeeWriteArray((BYTE *)&cfgCopy, sizeof(cfgCopy));

                    //Update cfgCopyNextAdr to address where next CFG_COPY block must be written to
                    cfgCopyNextAdr = cfgIncCfgCopyAdr(cfgCopyNextAdr);
                }

                //Save given array to CFG_STRUCT
                loopType = LOOP_WRITE_CONFIG;

                //Start LOOP_WRITE_CONFIG loop with first address that did not match
                xeeAdr = adrCopyBlocks[0];

                //Get offset of xeeAdr in given array. This function's 'offsetCfg' parameter gives the offset in CFG_STRUCT of given array.
                offsetBuf = xeeAdr - XEEMEM_CFG_STRUCT_START_ADR - offsetCfg;;
            }   //if (loopType==LOOP_COUNT_BLOCKS)
        }

    } while (xeeAdr < xeeAdrStop);

    if (reading)
        xeeEndRead();
}

/**
 * Save Application Configuartion data to EEPROM
 */
void cfgSave(void)
{
    /* Currently not required
    BYTE c;
    BYTE *p;

    p = (BYTE*)&AppConfig;

    for ( c = 0; c < sizeof(AppConfig); c++ ) {
        EEDATA=*p++;
        _cfgPutc(c);
    }
    */
}

BYTE cfgIsXeepromValid(void)
{
    CFG_INFO cfgInfo;

    //Read the CFG_INFO structure to see if EEPROM contains valid data
    xeeReadArray(XEEMEM_CFG_INFO_START_ADR, (BYTE*) &cfgInfo, sizeof(cfgInfo));
    return (cfgInfo.magicNumber.Val==CFG_INFO_MAGIC_NUMBER);
}

void cfgInvalidateXeeprom(void)
{
    //Invalidate external EEPROM. Done by setting magic number to 0
    xeeBeginWrite(XEEMEM_CFG_INFO_START_ADR + offsetof(CFG_INFO, magicNumber));
    xeeWrite(0);
    xeeEndWrite();
}
