/**
 * @brief           Functions for DS2482 1-Wire interface
 * @file            nz_ds2482.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
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
 * 2014-01-02, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_DS2482_C

#include "HardwareProfile.h"
#include "nz_ow2482.h"
#include "nz_serI2C.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"


//Add debugging to this file. The DEBUG_CONF_DS2482 macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_DS2482)
    #define DEBUG_CONF_DS2482       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_DS2482
#include "nz_debug.h"


////////// Defines //////////////////////////////
#define SM_TASK_WAIT_I2C 0x80   //Wait for I2C Transmission to DS2482 to complete
#define SM_TASK_WAIT_1WB 0x40   //Wait for 1-Wire to go idle (1WB bit of status register = 0)
#define SM_TASK_SM_MASK  0x3F   //Mask out all bits above, so we get state machine value

//Default address is 0x30. This is the case if both AD0 and AD1 pins of the DS2482
//are tied to 0V. By connecting AD0, AD1, or both AD0 and AD1 to Vcc, the address
//could also be 0x31, 0x32 or 0x33
#if !defined(DS2482_ADDRESS)
#define DS2482_ADDRESS  0x30
#endif

enum SM_DS2482_ {
    SM_TASK_IDLE = 0,
    SM_TASK_RESET_WAIT,         //Finished sending DS2482 Reset message via I2C
    SM_TASK_WRITECONFIG_WAIT,   //Finished sending DS2482 Configuration message via I2C
    SM_TASK_OWRESET_WAIT,       //Finished writing 1-Wire reset, and reading DS2482 Status via I2C
    SM_TASK_OW_READ_BIT,        //Finished reading a bit from the 1-Wire bus
    SM_TASK_OW_READ_BYTE,       //Finished reading a byte from the 1-Wire bus
    SM_TASK_OW_READ_BYTE_GETVAL,//Finished getting the value of byte read
    SM_TASK_OW_WRITE_BYTE,      //Finished writing a byte to 1-Wire bus, and reading DS2482 Status via I2C
    SM_TASK_OW_WRITE_BYTE_PWR,  //Finished writing DS2482 Configuration message swith SPU enabled, send byte next
    SM_TASK_OW_SELECT,          //Finished writing ROM Select command
    SM_TASK_OW_SELECT_ADR,      //Finished writing a byte of the 64bit address
    SM_TASK_OW_SEARCH,          //Finished writing 1-Wire reset, and reading DS2482 Status via I2C
    SM_TASK_OW_SEARCH_LOOP,     //Finished writing the "Search Command", and reading DS2482 Status via I2C
    SM_TASK_OW_SEARCH_LOOP2,    //
    SM_TASK_OW_TRIPLET
} SM_DS2482;


////////// Function Prototypes //////////////////
BYTE ds2482_readStatusReg(DS2482_INFO* pObj);
BYTE ds2482_readStatusRegAgain(DS2482_INFO* pObj);
BYTE ds2482_searchTriplet(DS2482_INFO* pObj, BOOL searchDirection);
BYTE calc_crc8(DS2482_INFO* pObj, BYTE data);


////////// Variables ////////////////////////////


BYTE ds2482_init(DS2482_INFO* pObj, BYTE i2cBus, BYTE i2cAdr) {
    //Initialize with 0
    memset(pObj, 0, sizeof(DS2482_INFO));

    pObj->pI2CInfo = i2cGetInfoForBus(i2cBus);
    if (pObj->pI2CInfo==0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 Init() invalid bus");
        return 1;   //Return ERROR
    }

    pObj->i2cAdr = i2cAdr;

    /////////////////////////////////////////////////
    //Send reset command
    if (ds2482_reset(pObj) != 0) {
        goto DS2482_INIT_ERROR;
    }

//    if (ds2482_getStatusWait(pObj) != 0) {
//        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 Init Reset Error!");
//        goto DS2482_INIT_ERROR;
//    }

    //Wait for I2C message to be sent. Normally ds2482_getStatusWait() should be called, but during
    //initialization, main ds2482_task() is not running yet, so use low level I2C functions!
    //Return state to idle (was changed during ds2482_reset()), and wait for I2C message to finish.
    pObj->sm = SM_TASK_IDLE;   //Return to idle state
    if (i2cGetStatusWait(pObj->pI2CInfo, 0) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 Init Reset Error!");
        goto DS2482_INIT_ERROR;
    }

    /////////////////////////////////////////////////
    //Write configuration, enable "Active Pullup", disable all the rest
    if (ds2482_writeConfig(pObj, DS2482_CONFIG_APU) != 0) {
        goto DS2482_INIT_ERROR;
    }

//    if (ds2482_getStatusWait(pObj) != 0) {
//        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 Init Configuration Error!");
//        goto DS2482_INIT_ERROR;
//    }
    
    //Wait for I2C message to be sent. Normally ds2482_getStatusWait() should be called, but during
    //initialization, main ds2482_task() is not running yet, so use low level I2C functions!
    //Return state to idle (was changed during ds2482_reset()), and wait for I2C message to finish.
    pObj->sm = SM_TASK_IDLE;   //Return to idle state
    if (i2cGetStatusWait(pObj->pI2CInfo, 0) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 Init Configuration Error!");
        goto DS2482_INIT_ERROR;
    }
    //Confirm read Configuration byte is the same as configuration byte sent.
    if (pObj->configRegVal != pObj->readByte) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 Init Configuration Confirm Error!");
        goto DS2482_INIT_ERROR;
    }
    
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDS2482 Init Success");
    return 0;   //Return Success

    DS2482_INIT_ERROR:
    return 1;   //Return ERROR
}

/**
 * DS2482 Task
 */
void ds2482_task(DS2482_INFO* pObj) {
    BYTE statusI2C = 0;

    /////////////////////////////////////////////////
    //Wait for I2C Transmission to DS2482 to complete, and I2C status to be available
    if (pObj->sm & SM_TASK_WAIT_I2C) {
        //Check if a timeout occurred
        if (tick16TestTmr(pObj->timeout + tick16ConvertFromMS(DS2482_TIMEOUT))) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\n######## DS2482 I2C Timeout-");
            DEBUG_PUT_WORD(DEBUG_LEVEL_WARNING, tick16Get());
            DEBUG_PUT_CHAR(DEBUG_LEVEL_WARNING, '\n');
            pObj->sm = SM_TASK_IDLE;
            return;
        }

        //Get status for last I2C transmission
        statusI2C = i2cGetStatus(pObj->pI2CInfo, pObj->i2cAdr);

        //Status not available yet, I2C transmission still in progress (background via interrupt)
        if (statusI2C == I2C_TXION_STATUS_NOT_AVAILABLE) {
            //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'x');
            return;
        }

        //Status was just received! Clear SM_TASK_WAIT_I2C flag
        pObj->sm = pObj->sm & ~SM_TASK_WAIT_I2C;

        if (statusI2C != 0) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 I2C comm Error!");
            goto DS2482_TASK_ERROR;
        }
    }


    /////////////////////////////////////////////////
    //Wait for 1-Wire bus to go idle. At this stage we have just received finished
    //reading the DS2482 status byte, is saved to pObj->statusRegVal
    if (pObj->sm & SM_TASK_WAIT_1WB) {
        //Check if a timeout occurred
        if (tick16TestTmr(pObj->timeout + tick16ConvertFromMS(DS2482_TIMEOUT))) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\n######## DS2482 1WB Timeout!");
            pObj->sm = SM_TASK_IDLE;
            return;
        }

        //DS2482 is still busy! Read the status register again.
        if (pObj->statusReg.OWB) {
            //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '.');
            ds2482_readStatusRegAgain(pObj);
            return;
        }

        //Status was just received! Clear SM_TASK_WAIT_1WB flag
        pObj->sm = pObj->sm & ~SM_TASK_WAIT_1WB;
    }


    /////////////////////////////////////////////////
    //State machine
    switch(pObj->sm & SM_TASK_SM_MASK) {
        case SM_TASK_IDLE:
            break;
        //Finished sending DS2482 Reset message via I2C
        case SM_TASK_RESET_WAIT:
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDS2482 Reset");
            //Return error(1) if RST bit of read back status is NOT set. Confirm reset was successfull
            pObj->status = (pObj->statusReg.RST == 0);
            pObj->sm = SM_TASK_IDLE;   //Return to idle state
            break;
        //Finished sending DS2482 Configuration message via I2C
        case SM_TASK_WRITECONFIG_WAIT:
            //Return error(1) if read Configuration byte is not same as configuration byte sent.
            if (pObj->configRegVal != pObj->readByte) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 Write Config Confirm Error!");
                //ds2482_reset(pObj);
                goto DS2482_TASK_ERROR;
            }
            DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nDS2482 Config");
            pObj->status = 0;           //Success
            pObj->sm = SM_TASK_IDLE;    //Return to idle state
            break;
        //Finished sending 1-Wire reset command, and reading DS2482 Status (saved in statusRegVal)
        case SM_TASK_OWRESET_WAIT:
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDS2482 1-Wire Reset");
            //1-Wire bus NOT busy any more
            if (pObj->statusReg.PPD == FALSE) {
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-NO Presence Pulse Detect");
            }
            //Short Detected
            if (pObj->statusReg.SD == TRUE) {
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-Short Detected");
            }
            pObj->status = 0;           //Success
            pObj->sm = SM_TASK_IDLE;    //Return to idle state
            break;
        //Finished reading bit from 1-Wire, and reading DS2482 Status (saved in statusRegVal)
        case SM_TASK_OW_READ_BIT:
            DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nDS2482 1-Wire Read bit=");
            DEBUG_PUT_CHAR(DEBUG_LEVEL_FINE, pObj->statusReg.SBR?'1':'0');
            pObj->status = 0;           //Success
            pObj->sm = SM_TASK_IDLE;    //Return to idle state
            break;
        //Finished reading byte from 1-Wire, and reading DS2482 Status (saved in statusRegVal)
        case SM_TASK_OW_READ_BYTE:
            DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nDS2482 1-Wire Read Byte");
            //Read the byte read in previous state
            //-Set read pointer for read byte (0xE1)
            //-Read 1 byte
            i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr);
            i2cWrite(pObj->pI2CInfo, DS2482_COMMAND_SET_RD_PTR);
            i2cWrite(pObj->pI2CInfo, 0xE1);     //Status register
            i2cReadToArray(pObj->pI2CInfo, &pObj->readByte, 1);
            //Start sending I2C Transmission in background
            if (i2cEndTransmission(pObj->pI2CInfo) != 0) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 Read Comm Err!");
                goto DS2482_TASK_ERROR;
            }
            pObj->timeout = tick16Get();  //Set Timeout
            //Set next state, and SM_TASK_WAIT_I2C flag (wait for I2C Transmission to DS2482 to complete)
            pObj->sm = SM_TASK_OW_READ_BYTE_GETVAL | SM_TASK_WAIT_I2C;
            pObj->status = 0xff;        //Busy
            break;
        //Finished getting the value of byte read
        case SM_TASK_OW_READ_BYTE_GETVAL:
            DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nDS2482 1-Wire Read Byte=");
            DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_FINE, pObj->readByte);
            pObj->status = 0;           //Success
            pObj->sm = SM_TASK_IDLE;    //Return to idle state
            break;
        //Finished writing byte to 1-Wire, and reading DS2482 Status (saved in statusRegVal)
        case SM_TASK_OW_WRITE_BYTE:
            DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nDS2482 1-Wire Write Byte=");
            DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_FINE, pObj->readByte);
            pObj->status = 0;           //Success
            pObj->sm = SM_TASK_IDLE;    //Return to idle state
            break;
        //Finished writing DS2482 Configuration message swith SPU enabled, send byte next
        case SM_TASK_OW_WRITE_BYTE_PWR:
            //Return error(1) if read Configuration byte is not same as configuration byte sent.
            if (pObj->configRegVal != pObj->readByte) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDS2482 Write Config Confirm Error!");
                //ds2482_reset(pObj);
                goto DS2482_TASK_ERROR;
            }

            //Write the byte stored in "param" in the owWriteBytePower() fuction
            if (owWriteByte(pObj, pObj->param) != 0) {
                goto DS2482_TASK_ERROR;
            }

            //SM is set to SM_TASK_OW_WRITE_BYTE by owWriteByte(). This state will be
            //called next, and set status to correct value.
            break;
        //Finished writing 1-Wire "Choose ROM" command, send 64bit ROM Address next
        case SM_TASK_OW_SELECT:
            //Just rinished sending 1-Wire "Choose ROM" command, send 64bit ROM Address next
            
            //Use readByte to mark what byte of adr64 has already been sent.
            pObj->readByte = 0;

            //NO BREAK!!!!!
            //break;
        //Finished writing a byte of the 64bit address
        case SM_TASK_OW_SELECT_ADR:
            //Write next byte of 64-bit ROM address
            if (pObj->readByte < 8) {
                if (owWriteByte(pObj, pObj->adr64[pObj->readByte++]) != 0) {
                    goto DS2482_TASK_ERROR;
                }

                //CHANGE next state to SM_TASK_OW_SELECT_ADR!
                //It was set to SM_TASK_OW_WRITE_BYTE by owWriteByte(), but we don't want that!
                pObj->sm = SM_TASK_OW_SELECT_ADR | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
                break;
            }

            //Done sending all 8 bytes of 64-bit ROM address
            DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nDS2482 1-Wire Select");
            pObj->status = 0;           //Success
            pObj->sm = SM_TASK_IDLE;    //Return to idle state
            break;
        //Finished writing 1-Wire reset, and reading DS2482 Status via I2C
        case SM_TASK_OW_SEARCH:
            //1-Wire bus NOT busy any more
            if (pObj->statusReg.PPD == FALSE) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nowSearch() no PPD!");
                goto DS2482_TASK_ERROR;
            }

            //Write the "Search Command"
            if (owWriteByte(pObj, 0xf0) != 0) {
                goto DS2482_TASK_ERROR;
            }

            //CHANGE next state to SM_TASK_OW_SEARCH_LOOP!
            //It was set to SM_TASK_OW_WRITE_BYTE by owWriteByte(), but we don't want that!
            pObj->sm = SM_TASK_OW_SEARCH_LOOP | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
            break;
        //Search Loop
        //- Finished writing the "Search Command", and reading DS2482 Status via I2C
        //- OR, search loop
        case SM_TASK_OW_SEARCH_LOOP:
            SM_TASK_OW_SEARCH_LOOP_:
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-l-");
            // if this discrepancy if before the Last Discrepancy
            // on a previous next then pick the same as last time
            if (pObj->idBitNumber < pObj->lastDiscrepancy) {
                if ((pObj->adr64[pObj->romByteNnumber] & pObj->romByteMask) > 0)
                    pObj->flags.searchDirection = 1;
                else
                    pObj->flags.searchDirection = 0;
            } else {
                // if equal to last pick 1, if not then pick 0
                if (pObj->idBitNumber == pObj->lastDiscrepancy)
                    pObj->flags.searchDirection = 1;
                else
                    pObj->flags.searchDirection = 0;
            }

            //Perform a triple operation on the DS2482 which will perform 2 read
            //bits and 1 write bit. The result gets placed in pObj->statusReg
            if (ds2482_searchTriplet(pObj, pObj->flags.searchDirection) != 0) {
                goto DS2482_TASK_ERROR;
            }

            //CHANGE next state to SM_TASK_OW_SEARCH_LOOP2!
            //It was set to SM_TASK_OW_TRIPLET by ds2482_searchTriplet(), but we don't want that!
            pObj->sm = SM_TASK_OW_SEARCH_LOOP2 | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
            break;
        case SM_TASK_OW_SEARCH_LOOP2:
            // check bit results in status byte
            pObj->flags.idBit = pObj->statusReg.SBR;
            pObj->flags.cmpIdBit = pObj->statusReg.TSB;
            pObj->flags.searchDirection = pObj->statusReg.DIR;

            //There are still devices on 1-Wire
            if ((pObj->flags.idBit==FALSE) || (pObj->flags.cmpIdBit==FALSE)) {
                if ((!pObj->flags.idBit) && (!pObj->flags.cmpIdBit) && (pObj->flags.searchDirection == 0)) {
                    pObj->lastZero = pObj->idBitNumber;

                    // check for Last discrepancy in family
                    if (pObj->lastZero < 9)
                        pObj->lastFamilyDiscrepancy = pObj->lastZero;
                }

                // set or clear the bit in the ROM byte pObj->romByteNnumber
                // with mask pObj->romByteMask
                if (pObj->flags.searchDirection == 1)
                    pObj->adr64[pObj->romByteNnumber] |= pObj->romByteMask;
                else
                    pObj->adr64[pObj->romByteNnumber] &= ~pObj->romByteMask;

                // increment the byte counter pObj->idBitNumber
                // and shift the mask pObj->romByteMask
                pObj->idBitNumber++;
                pObj->romByteMask <<= 1;

                // if the mask is 0 then go to new SerialNum byte pObj->romByteNnumber
                // and reset mask
                if (pObj->romByteMask == 0) {
                    calc_crc8(pObj, pObj->adr64[pObj->romByteNnumber]); // accumulate the CRC
                    pObj->romByteNnumber++;
                    pObj->romByteMask = 1;
                }

                // loop until through all ROM bytes 0-7
                if (pObj->romByteNnumber < 8) {
                    goto SM_TASK_OW_SEARCH_LOOP_;
                    //pObj->sm = SM_TASK_OW_SEARCH_LOOP;
                    //break;
                }
            }

            // if the search was successful then
            if (!((pObj->idBitNumber < 65) || (pObj->crc8 != 0))) {
                // search successful so set lastDiscrepancy, flags.lastDevice pObj->flags.searchResult
                pObj->lastDiscrepancy = pObj->lastZero;

                // check for last device
                if (pObj->lastDiscrepancy == 0)
                    pObj->flags.lastDevice = TRUE;

                pObj->flags.searchResult = TRUE;
            }
            pObj->status = 0;           //Success
            pObj->sm = SM_TASK_IDLE;    //Return to idle state
            break;
        case SM_TASK_OW_TRIPLET:
            DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nDS2482 1-Wire Triplet");
            pObj->status = 0;           //Success
            pObj->sm = SM_TASK_IDLE;    //Return to idle state
            break;
    }

    return;

    DS2482_TASK_ERROR:
    pObj->sm = SM_TASK_IDLE;   //Return to idle state
    pObj->status = 1;   //Error
    return;
}

/**
 * Waits for current transmission to finish, and returns status.
 *
 * @param pObj
 *
 * @return The status of the last transmission, 0=Success, else error code
 */
BYTE ds2482_getStatusWait(DS2482_INFO* pObj) {
    while(ds2482_isBusy(pObj)) {
        //mainLoop();
        //Include I2C task for I2C channel that this DS2482 chip uses.
        serI2CTask(pObj->pI2CInfo);
        ds2482_task(pObj);
    };
    return pObj->status;
}


/**
 * Sends a reset command to the DS2482 (is done in background via I2C interrupt).
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BOOL ds2482_reset(DS2482_INFO* pObj) {
    //Write reset command to DS2482
    i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr);
    i2cWrite(pObj->pI2CInfo, DS2482_COMMAND_RESET);
    i2cReadToArray(pObj->pI2CInfo, &pObj->statusRegVal, 1);
    //Start sending I2C Transmission in background
    if (i2cEndTransmission(pObj->pI2CInfo) != 0) {
        pObj->status = 1;       //Error
        return 1;               //Return error
    }
    pObj->timeout = tick16Get();  //Set Timeout
    //Set next state, and SM_TASK_WAIT_I2C flag (wait for I2C Transmission to DS2482 to complete)
    pObj->sm = SM_TASK_RESET_WAIT | SM_TASK_WAIT_I2C;
    return 0;
}

/**
 * Write the config register of the DS2482.
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param pObj Pointer to DS2482_INFO structure
 * @param config Combination of DS2482_CONFIG_XX config defines.
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BOOL ds2482_writeConfig(DS2482_INFO* pObj, BYTE config) {
    //Write reset command to DS2482
    i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr);
    i2cWrite(pObj->pI2CInfo, DS2482_COMMAND_WR_CONF);
    i2cWrite(pObj->pI2CInfo, config | (~config << 4));
    i2cReadToArray(pObj->pI2CInfo, &pObj->readByte, 1);
    if (i2cEndTransmission(pObj->pI2CInfo) != 0) {  //Start sending I2C Transmission in background
        pObj->status = 1;       //Error
        return 1;               //Return error
    }

    //Save config byte written
    pObj->configRegVal = config;

    pObj->timeout = tick16Get();  //Set Timeout
    //Set next state, and SM_TASK_WAIT_I2C flag (wait for I2C Transmission to DS2482 to complete)
    pObj->sm = SM_TASK_WRITECONFIG_WAIT | SM_TASK_WAIT_I2C;
    return 0;
}


/**
 * Schedules one bit of a 1-Wire search. This command does two read bits and one
 * write bit. The write bit is either the default direction (all device have same
 * bit) or in case of a discrepancy, the 'searchDirection' parameter is used.
 *
 * The actual command is scheduled in background via I2C interrupt. Use the
 * ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait() functions to
 * determine when done. The result is available in theDS2482 status byte, and can
 * be read with the ds2482_getStatusReg() function.
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param bVal The bit
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE ds2482_searchTriplet(DS2482_INFO* pObj, BOOL searchDirection) {
    i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr);
    i2cWrite(pObj->pI2CInfo, DS2482_COMMAND_1W_TRIPLET);
    i2cWrite(pObj->pI2CInfo, searchDirection ? 0x80 : 0);
    i2cReadToArray(pObj->pI2CInfo, &pObj->statusRegVal, 1);
    //Start sending I2C Transmission in background
    if (i2cEndTransmission(pObj->pI2CInfo) != 0) {
        pObj->status = 1;       //Error
        return 1;               //Return error
    }
    pObj->timeout = tick16Get();  //Set Timeout
    //Set next state, and SM_TASK_WAIT_I2C flag (wait for I2C Transmission to DS2482 to complete)
    pObj->sm = SM_TASK_OW_TRIPLET | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
    return 0;
}


/**
 * Schedules the Resetting all of devices on the 1-Wire Net(is done in background
 * via I2C interrupt).
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 * Use the ds2482_getStatusReg()and ds2482_getStatusRegXxx() functions to get
 * result in status register.
 *
 * This function generates a 1-Wire reset on the 1-Wire network and samples
 * for a 1-Wire device presence pulse. The state of the sample is reported
 * through the Presence-Pulse Detect (PPD) and the Short Detected (SD) fields
 * in the status register. The result of the status register is obtained via the
 * ds2482_getStatusReg() and ds2482_getStatusRegXxx() functions.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owReset(DS2482_INFO* pObj) {
    //Write reset command to DS2482
    i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr);
    i2cWrite(pObj->pI2CInfo, DS2482_COMMAND_1W_RESET);
    i2cReadToArray(pObj->pI2CInfo, &pObj->statusRegVal, 1);
    //Start sending I2C Transmission in background
    if (i2cEndTransmission(pObj->pI2CInfo) != 0) {
        pObj->status = 1;       //Error
        return 1;               //Return error
    }
    pObj->timeout = tick16Get();  //Set Timeout
    //Set next state, and SM_TASK_WAIT_I2C flag (wait for I2C Transmission to DS2482 to complete)
    pObj->sm = SM_TASK_OWRESET_WAIT | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
    return 0;
}

#if !defined(DS2482_DISABLE_WATE_FUNCTIONS)
BYTE owResetWATE(DS2482_INFO* pObj) {
    if (pObj->status == 0) {
        owReset(pObj);
        return ds2482_getStatusWait(pObj);
    }
    return 1;
}
#endif


/**
 * Schedules the reading of 1 bit from the 1-Wire Net (is done in background via
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 * The owGetBit() function will return the read bit (if successful).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param bVal The bit
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owReadWriteBit(DS2482_INFO* pObj, BOOL bVal) {
    //Write "Write Single Bit" command to DS2482
    i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr);
    i2cWrite(pObj->pI2CInfo, DS2482_COMMAND_1W_SINGLE_BIT);
    i2cWrite(pObj->pI2CInfo, bVal ? 0x80 : 0);
    i2cReadToArray(pObj->pI2CInfo, &pObj->statusRegVal, 1);
    //Start sending I2C Transmission in background
    if (i2cEndTransmission(pObj->pI2CInfo) != 0) {
        pObj->status = 1;       //Error
        return 1;               //Return error
    }
    pObj->timeout = tick16Get();  //Set Timeout
    //Set next state, and SM_TASK_WAIT_I2C flag (wait for I2C Transmission to DS2482 to complete)
    pObj->sm = SM_TASK_OW_READ_BIT | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
    return 0;
}


/**
 * Schedules the reading of 1 bit from the 1-Wire Net (is done in background via
 * I2C interrupt).
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 * The owGetByte() function will return the read byte (if successful).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owReadByte(DS2482_INFO* pObj) {
    //Write "Write Single Bit" command to DS2482
    i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr);
    i2cWrite(pObj->pI2CInfo, DS2482_COMMAND_1W_RD_BYTE);
    i2cReadToArray(pObj->pI2CInfo, &pObj->statusRegVal, 1);
    //Start sending I2C Transmission in background
    if (i2cEndTransmission(pObj->pI2CInfo) != 0) {
        pObj->status = 1;       //Error
        return 1;               //Return error
    }
    pObj->timeout = tick16Get();  //Set Timeout
    //Set next state, and SM_TASK_WAIT_I2C flag (wait for I2C Transmission to DS2482 to complete)
    pObj->sm = SM_TASK_OW_READ_BYTE | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
    return 0;
}

#if !defined(DS2482_DISABLE_WATE_FUNCTIONS)
BYTE owReadByteWATE(DS2482_INFO* pObj) {
    if (pObj->status == 0) {
        owReadByte(pObj);
        if (ds2482_getStatusWait(pObj) == 0) {
            return owGetByte(pObj);
        }
    }
    return 0;   //Error
}
#endif


/**
 * Schedules the writing of 1 byte to the 1-Wire Net (is done in background via
 * I2C interrupt).
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param val The byte to write
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owWriteByte(DS2482_INFO* pObj, BYTE val) {
    i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr);
    i2cWrite(pObj->pI2CInfo, DS2482_COMMAND_1W_WR_BYTE);
    i2cWrite(pObj->pI2CInfo, val);
    i2cReadToArray(pObj->pI2CInfo, &pObj->statusRegVal, 1);
    //Start sending I2C Transmission in background
    if (i2cEndTransmission(pObj->pI2CInfo) != 0) {
        pObj->status = 1;       //Error
        return 1;               //Return error
    }
    pObj->timeout = tick16Get();  //Set Timeout
    //Set next state, and SM_TASK_WAIT_I2C flag (wait for I2C Transmission to DS2482 to complete)
    pObj->sm = SM_TASK_OW_WRITE_BYTE | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
    return 0;
}

#if !defined(DS2482_DISABLE_WATE_FUNCTIONS)
BYTE owWriteByteWATE(DS2482_INFO* pObj, BYTE val) {
    if (pObj->status == 0) {
        owWriteByte(pObj, val);
        return ds2482_getStatusWait(pObj);
    }
    return 1;
}
#endif


/**
 * Schedules the writing of 1 byte to the 1-Wire Net (is done in background via
 * I2C interrupt), and implements strong pullup power delivery. The DS2482 will
 * actively pull the 1-Wire network high after the given byte has been sent.
 * The owLevel function can then be called to return the 1-Wire network to
 * standard pullup.
 *
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param val The byte to write
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owWriteBytePower(DS2482_INFO* pObj, BYTE val) {

    //Store byte to write in "param". Is used in SM_TASK_OW_WRITE_BYTE_PWR state
    pObj->param = val;

    //Call write config with SPU enabled!
    pObj->configReg.SPU = 1;
    if (ds2482_writeConfig(pObj, pObj->configRegVal) != 0)
        return 1;   //Error

    //CHANGE next state to SM_TASK_OW_WRITE_BYTE_PWR and SM_TASK_WAIT_I2C flag
    //(wait for I2C Transmission to DS2482 to complete)!
    //It was set to SM_TASK_WRITECONFIG_WAIT by ds2482_writeConfig(), but we don't want that!
    pObj->sm = SM_TASK_OW_WRITE_BYTE_PWR | SM_TASK_WAIT_I2C;
    return 0;
}


#if !defined(DS2482_DISABLE_WATE_FUNCTIONS)
BYTE owWriteBytePowerWATE(DS2482_INFO* pObj, BYTE val) {
    if (pObj->status == 0) {
        owWriteBytePower(pObj, val);
        return ds2482_getStatusWait(pObj);
    }
    return 1;
}
#endif


/**
 * Set the 1-Wire Net communication speed.
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param pObj Pointer to DS2482_INFO structure
 * @param speed Set the 1-Wire Net communication speed.
 *        0 for normal speed
 *        1 for high speed
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owSpeed(DS2482_INFO* pObj, BOOL speed) {
    pObj->configReg.OWS = speed;
    return ds2482_writeConfig(pObj, pObj->configRegVal);
}


/**
 * Set the 1-Wire Net line level pullup to normal. The DS2482 only allows
 * enabling strong pullup on a bit or byte event. Consequently this function
 * only allows the MODE_STANDARD argument. To enable strong pullup use
 * owWriteBytePower or owReadBitPower.
 *
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param pObj Pointer to DS2482_INFO structure
 * @param level Set the 1-Wire Net line level pullup.
 *        0 for normal
 *        1 for strong
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owLevel(DS2482_INFO* pObj, BOOL level) {
    //Must be 0
    if (level == 1)
        level = 0;

    pObj->configReg.SPU = level;
    return ds2482_writeConfig(pObj, pObj->configRegVal);
}


/**
 * Reset all search parameters. This function should be called before a owSearch()
 */
void owSearchReset(DS2482_INFO* pObj) {
   // reset the search state
   pObj->lastDiscrepancy = 0;
   pObj->flags.lastDevice = FALSE;
   pObj->lastFamilyDiscrepancy = 0;
}


/**
 * Schedules a search for the next device on a 1-Wire Network(is done in background
 * via I2C interrupt).
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * The owGetAdr64() function will return the 64-bit ROM address of the last
 * found device (if successful), or 0 if no new device was found. Alternativly the
 * 64-bit ROM address can also be accessed via the DS2482_INFO.adr64[] BYTE array.
 *
 * The owSearch() function does a general search. This function continues from the
 * previous search state. The search state can be reset by using the owSearchReset
 * function.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BOOL owSearch(DS2482_INFO* pObj) {
    union {
        struct {
            unsigned char idBit             : 1;
            unsigned char cmpIdBit          : 1;
            unsigned char searchResult      : 1;
            unsigned char searchDirection   : 1;
        } bits;
        BYTE val;
    } flags;
    flags.val = 0;

    // initialize for search
    pObj->idBitNumber = 1;
    pObj->lastZero = 0;
    pObj->romByteNnumber = 0;
    pObj->romByteMask = 1;
    pObj->flags.searchResult = FALSE;
    pObj->crc8 = 0;

    DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nowSearch");

    //If the last call was not the last one
    if (!pObj->flags.lastDevice) {
        DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "-Next");

        //1-Wire reset. Writes a reset command, AND reads ds2482 status byte
        if (owReset(pObj) != 0) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nowSearch owReset ERROR!");
            return 1;   //Error
        }

        //CHANGE next state to SM_TASK_OW_SEARCH. It was set to
        //SM_TASK_OWRESET_WAIT by owReset(), but we don't want that!
        pObj->sm = SM_TASK_OW_SEARCH | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
        return 0;
    }

    //if no device found then reset counters so next 'search' will be like a first
    pObj->lastDiscrepancy = 0;
    pObj->flags.lastDevice = FALSE;
    pObj->lastFamilyDiscrepancy = 0;
    pObj->flags.searchResult = FALSE;

    pObj->status = 0;   //Success
    return 0;
}

#if !defined(DS2482_DISABLE_WATE_FUNCTIONS)
BYTE* owSearchWATE(DS2482_INFO* pObj) {
    if (pObj->status == 0) {
        owSearch(pObj);

        if(ds2482_getStatusWait(pObj) != 0)
            return 0;   //No new device found

        //Return pointer to 64-bit ROM address, or 0 if no new device was found.
        return (owGetAdr64(pObj));
    }
    return 0;   //No new device found
}
#endif

/**
 * Select an 1-Wire device. Use address obtained via owSearch() function
 *
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * @preCondition: - The DS2482 must be idle! Ensure ds2482_isBusy() returns FALSE.<br>
 *                - This command must be preceded by the owReset() function!
 *
 * @param pObj Pointer to DS2482_INFO structure
 * @param adr64 The 64-bit address of the device to select
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owSelect(DS2482_INFO* pObj, BYTE* adr64) {
    
    //Store adr64 to write in "adr64".
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_FINE)
    BYTE i;
    DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nowSelect 0x");
    for (i=0; i<8; i++) {
        pObj->adr64[i] = adr64[i];
        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_FINE, adr64[i]);
        DEBUG_PUT_CHAR(DEBUG_LEVEL_FINE, ' ');
    }
    #else
    nzMemCpyNoCheck(&pObj->adr64[0], &adr64[0], 8);
    #endif

    //Send Choose ROM command
    if (owWriteByte(pObj, 0x55) != 0)
        return 1;   //Error

    //CHANGE next state to SM_TASK_OW_SELECT!
    //It was set to SM_TASK_OW_WRITE_BYTE by owWriteByte(), but we don't want that!
    pObj->sm = SM_TASK_OW_SELECT | SM_TASK_WAIT_I2C | SM_TASK_WAIT_1WB;
    return 0;
}

#if !defined(DS2482_DISABLE_WATE_FUNCTIONS)
BYTE owSelectWATE(DS2482_INFO* pObj, BYTE* adr64) {
    if (pObj->status == 0) {
        owSelect(pObj, adr64);
        return ds2482_getStatusWait(pObj);
    }
    return 1;
}
#endif


/**
 * Set the "DS2482 Read Pointer" to the status reagister, and read the status
 * register. The DS2482 state machine is NOT changed!
 *
 * @param pObj
 *
 * @return
 */
BYTE ds2482_readStatusReg(DS2482_INFO* pObj) {
    DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nDS2482 readStatusReg");
    //Read the status register
    //-Set read pointer for status register (0xF0)
    //-Read 1 byte
    i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr);
    i2cWrite(pObj->pI2CInfo, DS2482_COMMAND_SET_RD_PTR);
    i2cWrite(pObj->pI2CInfo, 0xF0);     //Status register
    i2cReadToArray(pObj->pI2CInfo, &pObj->statusRegVal, 1);
    //Start sending I2C Transmission in background
    if (i2cEndTransmission(pObj->pI2CInfo) != 0) {
        pObj->status = 1;           //Error
        return 1;                   //Return error
    }
    pObj->timeout = tick16Get();      //Set Timeout
    pObj->sm |= SM_TASK_WAIT_I2C;   //Wait for I2C Transmission to DS2482 to complete
    return 0;
}


/**
 * Read the status register. The DS2482 state machine is NOT changed!
 *
 * @preCondition: The "DS2482 Read Pointer" must point to the status reagister.
 *
 * @param pObj
 *
 * @return
 */
BYTE ds2482_readStatusRegAgain(DS2482_INFO* pObj) {
    DEBUG_PUT_STR(DEBUG_LEVEL_FINE, "\nDS2482 readStatusRegAgain");
    //Read the status register
    i2cBeginTransmission(pObj->pI2CInfo, pObj->i2cAdr+1);
    i2cReadToArray(pObj->pI2CInfo, &pObj->statusRegVal, 1);
    //Start sending I2C Transmission in background
    if (i2cEndTransmission(pObj->pI2CInfo) != 0) {
        pObj->status = 1;           //Error
        return 1;                   //Return error
    }
    pObj->timeout = tick16Get();      //Set Timeout
    pObj->sm |= SM_TASK_WAIT_I2C;   //Wait for I2C Transmission to DS2482 to complete
    return 0;
}


/**
 * Calculate the CRC8 of the byte value provided with the current global 'crc8'
 * value.
 *
 * @param pObj
 * @param data
 *
 * @return Returns current global crc8 value
 */
BYTE calc_crc8(DS2482_INFO* pObj, BYTE data) {
   BYTE i;

   // See Application Note 27
   pObj->crc8 = pObj->crc8 ^ data;
   for (i = 0; i < 8; ++i)
   {
      if (pObj->crc8 & 1)
         pObj->crc8 = (pObj->crc8 >> 1) ^ 0x8c;
      else
         pObj->crc8 = (pObj->crc8 >> 1);
   }

   return pObj->crc8;
}

