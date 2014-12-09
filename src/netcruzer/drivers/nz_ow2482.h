/**
 * @brief           Functions for DS2482 1-Wire interface
 * @file            nz_ow2482.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_ow2482_desc Description
 *****************************************
 * Functions for using the DS2482 1-Wire interface
 * 
 * @subsection nz_ow2482_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ------------ DS2482 Configuration (from nz_ds2482.h) -------------
// *********************************************************************
#define NZ_OW2482_ENABLED                       //Enable OW2482 module

//---- Disable WATE functions. Not required if only asynchronous functions are used
#define DS2482_DISABLE_WATE_FUNCTIONS

 @endcode
 *
 * 
 * @subsection nz_ow2482_usage Usage
 *****************************************
 * To use this module, the following must be done:
 * - Include nz_ds2482.h in the c file it is used in.
 * - Add nz_ds2482.c to the MPLAB project, this is the main DS2482 driver file.
 * - The following additional files are required by nz_ds2482.c, and must be added to the project:
 *   nz_circularBufferPwr2.c, nz_helpers.c, nz_netcruzer.c and nz_serI2C.c
 * - Add "NZ_I2C1_ENABLE" to projdefs.h file.
 * - In code, initialize the DS2482. Ensure to delay 300ms from power up. For example:<br>
 *      delay_ms(300);<br>
 *      ds2482Init();
 * - In projdefs.h, do any DS2482, I2C or other configuration required. This is done by copying the
 *   configuration section from the *.h files to projdefs.h. Nothing required, defaults will work!
 * - In code, call ds2482Task() repetitively in main loop
 * - <b>All DONE!</b> Can now use DS2482 functions defined in nz_ds2482.h! Some examples:
 *      lcdDisplayString(0, "\fHello\nWorld, Again!");      //Write parsed string to LCD display
 *      lcdDisplayChar(0, 'x');                             //Display 'x' at current cursor position
 * 
 **********************************************************************
 * @section nz_ow2482_lic Software License Agreement
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
#ifndef NZ_OW2482_H
#define NZ_OW2482_H

#if defined(NZ_OW2482_ENABLED)

////////// Defines //////////////////////////////

//Default address is 0x30. This is the case if both AD0 and AD1 pins of the DS2482
//are tied to 0V. By connecting AD0, AD1, or both AD0 and AD1 to Vcc, the address
//could also be 0x31, 0x32 or 0x33
#define DS2482_ADDRESS_00   0x30    //DS2482 address if AD0=0V and AD1=0V
#define DS2482_ADDRESS_01   0x31    //DS2482 address if AD0=Vcc and AD1=0V
#define DS2482_ADDRESS_10   0x32    //DS2482 address if AD0=0V and AD1=Vcc
#define DS2482_ADDRESS_11   0x33    //DS2482 address if AD0=Vcc and AD1=Vcc

#define DS2482_COMMAND_RESET        0xF0    //Device Reset command
#define DS2482_COMMAND_WR_CONF      0xD2    //Write Configuration command
#define DS2482_COMMAND_SET_RD_PTR   0xE1    //Set Read Pointer command
#define DS2482_COMMAND_1W_RESET     0xB4    //1-Wire Reset command
#define DS2482_COMMAND_1W_WR_BYTE   0xA5    //1-Wire Write Byte command
#define DS2482_COMMAND_1W_RD_BYTE   0x96    //1-Wire Read Byte command
#define DS2482_COMMAND_1W_SINGLE_BIT 0x87   //1-Wire Single Bit command
#define DS2482_COMMAND_1W_TRIPLET   0x78    //1-Wire Triplet command

#define DS2482_CONFIG_APU           0x01    //Active Pullup enabled
#define DS2482_CONFIG_PPM           0x02    //Presence-Pulse Masking enabled
#define DS2482_CONFIG_SPU           0x04    //Strong Pullup enabled
#define DS2482_CONFIG_1WS           0x08    //1-Wire Speed High Speed enabled

#define DS2482_STATUS_1WB           0x01    //1-Wire Busy
#define DS2482_STATUS_PPD           0x02    //Presence-Pulse Detect
#define DS2482_STATUS_SD            0x04    //Short Detected
#define DS2482_STATUS_LL            0x08    //Logic Level
#define DS2482_STATUS_RST           0x10    //Device Reset
#define DS2482_STATUS_SBR           0x20    //Single Bit Result
#define DS2482_STATUS_TSB           0x40    //Triplet Second Bit
#define DS2482_STATUS_DIR           0x80    //Branch Direction Taken

#define DS2482_TIMEOUT              100     //100ms

typedef struct DS2482_INFO_
//typedef struct __attribute__((__packed__)) DS2482_INFO_
//typedef struct __attribute__((aligned(2), packed)) DS2482_INFO_
{
    void*   pI2CInfo;   //Pointer to I2C_INFO structure of I2C bus used for this DS2482
    WORD    timeout;
    BYTE    sm;
    BYTE    i2cAdr;     //The I2C address of this DS2482. Use a DS2482_ADDRESS_xx define
    BYTE    status;     //The status of the last transmission, 0=Success, 0xff=Busy, else error code
    BYTE    readByte;   //Byte read from DS2482
    BYTE    param;      //Parameter used for passing data between states

    //Required for search
    BYTE    adr64[8];
    BYTE    lastDiscrepancy;
    BYTE    lastFamilyDiscrepancy;
    BYTE    idBitNumber;
    BYTE    lastZero;
    BYTE    romByteNnumber;
    BYTE    romByteMask;

    BYTE    crc8;

    union {
        struct
        {
            unsigned char lastDevice        : 1;
            unsigned char searchResult      : 1;
            unsigned char idBit             : 1;
            unsigned char cmpIdBit          : 1;
            unsigned char searchDirection   : 1;
        } flags;
        BYTE flagsVal;
    };

    //The DS2482 status register
    union {
        struct
        {
            unsigned int OWB    : 1;    //1-Wire Busy
            unsigned int PPD    : 1;    //Presence-Pulse Detect
            unsigned int SD     : 1;    //Short Detected
            unsigned int LL     : 1;    //Logic Level
            unsigned int RST    : 1;    //Device Reset
            unsigned int SBR    : 1;    //Single Bit Result
            unsigned int TSB    : 1;    //Triplet Second Bit
            unsigned int DIR    : 1;    //Branch Direction Taken
        } statusReg;
        BYTE statusRegVal;
    };

    union {
        struct
        {
            unsigned int APU        : 1;    //Active Pullup = Bit 0 of Configuration Register
            unsigned int PPM        : 1;    //Presence-Pulse Masking = Bit 1 of Configuration Register
            unsigned int SPU        : 1;    //Strong Pullup = Bit 2 of Configuration Register
            unsigned int OWS        : 1;    //1-Wire Speed  = Bit 3 of Configuration Register
            unsigned int fill       : 4;    //Fill
        } configReg;
        BYTE configRegVal;
    };
} DS2482_INFO;


////////// Functions ////////////////////////////

/**
 * DS2482 Initialization. This function will initialize the DS2482 chip, and return
 * when done.
 * 
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @param i2cBus I2C Bus number. Must be an I2C bus that is available, for example
 *        if the board has an I2C 1 and I2C 2 bus, it must be 1 or 2.
 *
 * @param i2cAdr Address of the target DS2482 chip. Use one of the DS2482_ADDRESS_xx
 *        defines. Default address is DS2482_ADDRESS_00, which is for when both AD0
 *        and AD1 pins of the DS2482 are tied to 0V. By connecting AD0, AD1, or both
 *        AD0 and AD1 to Vcc, it is possible having 4 DS2482 devices on a single bus.
 *
 * @return The status of the Initialization, 0=Success, else error code
 */
BYTE ds2482_init(DS2482_INFO* pObj, BYTE i2cBus, BYTE i2cAdr);


/**
 * DS2482 Task
 *
 * @param pObj Pointer to DS2482_INFO structure
 */
void ds2482_task(DS2482_INFO* pObj);


/**
 * Indicates if the DS2482 is currently busy writing or reading data.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return 1 DS2482 is currently busy
 *         0 DS2482 is not busy
 */
#define ds2482_isBusy(pObj) ((pObj)->sm!=0)

/**
 * Gets the status of the last transmission.
 *
 * @preCondition: There must not currently be any transmission in progress. Use
 *      ds2482_isBusy() to confirm bus is idle.
 *
 * @param pObj
 *
 * @return The status of the last transmission, 0=Success, else error code
 */
#define ds2482_getStatus(pObj) ((pObj)->status)

/**
 * Waits for current transmission to finish, and returns status.
 *
 * @param pObj
 *
 * @return The status of the last transmission, 0=Success, else error code
 */
BYTE ds2482_getStatusWait(DS2482_INFO* pObj);


#define ds2482_resetStatus(pObj) ((pObj)->status = 0)

/**
 * Gets the DS2482 status register.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return The status register of the DS2482
 */
#define ds2482_getStatusReg(pObj) ((pObj)->statusRegVal)

/**
 * Gets the DS2482 status register PPD (Presence-Pulse Detect) bit. It 1 if a
 * 1-Wire device was detected on the bus.
 *
 * @preCondition: The DS2482 Status register must have been read prior to this
 * function. Many different functions do this.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return 0 if no 1-Wire devices detected on the 1-Wire Net.<br>
 *         1 if a 1-Wire device was detected on the 1-Wire Net.
 */
#define ds2482_getStatusRegPPD(pObj) ((pObj)->statusReg.PPD)

/**
 * Gets the DS2482 status register SD (Short Detected) bit.
 *
 * @preCondition: The DS2482 Status register must have been read prior to this
 * function. Many different functions do this.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return 0 if 1-Wire Net OK.<br>
 *         1 if a short circuit detected on the 1-Wire Net.
 */
#define ds2482_getStatusRegSD(pObj) ((pObj)->statusReg.SD)


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
BOOL ds2482_reset(DS2482_INFO* pObj);


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
BOOL ds2482_writeConfig(DS2482_INFO* pObj, BYTE config);


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
BYTE owReset(DS2482_INFO* pObj);

/**
 * Same as owReset() function, but with "Wait and Error Processing".
 * - Function blocks and waits till transmission is completed.
 * - Function only executed if current error state is 0 (ds2482_resetStatus() to reset)
 * - Status of transmission is returned.
 *
 * Before using any xxxWATE() functions, call ds2482_resetStatus() to reset "Wait
 * and Error Processing" (resets status to 0 = no error).
 *
 * Call ds2482_getStatus() to get current status (0 = success). If any xxWATE() function
 * called after ds2482_resetStatus() caused an error, this function will return a non-Zero value.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return 0 if success, else error code (same as ds2482_getStatus() function)
 */
BYTE owResetWATE(DS2482_INFO* pObj);


/**
 * Schedules the reading of 1 bit from the 1-Wire Net (is done in background via
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 * The owGetBit() function will return the read bit (if successful).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param pObj Pointer to DS2482_INFO structure
 * @param bVal The bit
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owReadWriteBit(DS2482_INFO* pObj, BOOL bVal);

/**
 * Get bit read with last owReadWriteBit() function
 * @param pObj Pointer to DS2482_INFO structure
 * @return Returns the value of the bit
 */
#define owGetBit(pObj) ((pObj)->statusReg.SBR)


/**
 * Get bit read with last owReadByte() function
 * @param pObj Pointer to DS2482_INFO structure
 * @return Returns the value of the bit
 */
#define owGetByte(pObj) ((pObj)->readByte)


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
BYTE owReadByte(DS2482_INFO* pObj);

/**
 * Same as owReadByte() function, but with "Wait and Error Processing".
 * - Function blocks and waits till transmission is completed.
 * - Function only executed if current error state is 0 (ds2482_resetStatus() to reset)
 * - Read byte is returned.
 *
 * Before using any xxxWATE() functions, call ds2482_resetStatus() to reset "Wait
 * and Error Processing" (resets status to 0 = no error).
 *
 * Call ds2482_getStatus() to get current status (0 = success). If any xxWATE() function
 * called after ds2482_resetStatus() caused an error, this function will return a non-Zero value.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return Returns read byte if successful, else 0 if error (use ds2482_getStatus() to
 *         check for error)
 */
BYTE owReadByteWATE(DS2482_INFO* pObj);


/**
 * Schedules the writing of 1 byte to the 1-Wire Net (is done in background via
 * I2C interrupt).
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * @preCondition: The DS2482 must be not be busy! Ensure ds2482_isBusy() returns FALSE.
 *
 * @param pObj Pointer to DS2482_INFO structure
 * @param val The byte to write
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owWriteByte(DS2482_INFO* pObj, BYTE val);


/**
 * Same as owWriteByte() function, but with "Wait and Error Processing".
 * - Function blocks and waits till transmission is completed.
 * - Function only executed if current error state is 0 (ds2482_resetStatus() to reset)
 * - Status of transmission is returned.
 *
 * Before using any xxxWATE() functions, call ds2482_resetStatus() to reset "Wait
 * and Error Processing" (resets status to 0 = no error).
 *
 * Call ds2482_getStatus() to get current status (0 = success). If any xxWATE() function
 * called after ds2482_resetStatus() caused an error, this function will return a non-Zero value.
 *
 * @param pObj Pointer to DS2482_INFO structure
 * @param val The byte to write
 *
 * @return 0 if success, else error code (same as ds2482_getStatus() function)
 */
BYTE owWriteByteWATE(DS2482_INFO* pObj, BYTE val);


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
 * @param pObj Pointer to DS2482_INFO structure
 * @param val The byte to write
 *
 * @return 0 if command was successfully scheduled for transmission!
 *         1 if Error occurred
 */
BYTE owWriteBytePower(DS2482_INFO* pObj, BYTE val);


/**
 * Same as owWriteBytePower() function, but with "Wait and Error Processing".
 * - Function blocks and waits till transmission is completed.
 * - Function only executed if current error state is 0 (ds2482_resetStatus() to reset)
 * - Status of transmission is returned.
 *
 * Before using any xxxWATE() functions, call ds2482_resetStatus() to reset "Wait
 * and Error Processing" (resets status to 0 = no error).
 *
 * Call ds2482_getStatus() to get current status (0 = success). If any xxWATE() function
 * called after ds2482_resetStatus() caused an error, this function will return a non-Zero value.
 *
 * @param pObj Pointer to DS2482_INFO structure
 * @param val The byte to write
 *
 * @return 0 if success, else error code (same as ds2482_getStatus() function)
 */
BYTE owWriteBytePowerWATE(DS2482_INFO* pObj, BYTE val);


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
BYTE owSpeed(DS2482_INFO* pObj, BOOL speed);


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
BYTE owLevel(DS2482_INFO* pObj, BOOL level);


/**
 * Reset all search parameters. This function should be called before a owSearch()
 */
void owSearchReset(DS2482_INFO* pObj);


/**
 * Schedules a search for the next device on a 1-Wire Network(is done in background
 * via I2C interrupt).
 * Use the ds2482_isBusy() and ds2482_getStatus(), or ds2482_getStatusWait()
 * functions to determine when done, and get transmission status (0=Success).
 *
 * The owGetAdr64() function will return the 64-bit ROM address of the last
 * found device (if successful), or 0 if no new device was found.
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
BOOL owSearch(DS2482_INFO* pObj);


/**
 * Same as owSearch() function, but with "Wait and Error Processing".
 * - Function blocks and waits till transmission is completed.
 * - Function only executed if current error state is 0 (ds2482_resetStatus() to reset)
 * - Status of transmission is returned.
 *
 * Before using any xxxWATE() functions, call ds2482_resetStatus() to reset "Wait
 * and Error Processing" (resets status to 0 = no error).
 *
 * Call ds2482_getStatus() to get current status (0 = success). If any xxWATE() function
 * called after ds2482_resetStatus() caused an error, this function will return a non-Zero value.
 *
 * @param pObj Pointer to DS2482_INFO structure
 *
 * @return Returns a pointer to the 64-bit ROM address of the device found with
 *         the owSearch() function.
 *         Returns 0 when no new device was found. Either the last search was
 *         the last device or there are no devices on the 1-Wire Net.
 */
BYTE* owSearchWATE(DS2482_INFO* pObj);


/**
 * Returns address of last found device, or 0 if non.
 * @param pObj
 * @return Returns a pointer to the 64-bit ROM address of the device found with
 *         the owSearch() function.
 *         Returns 0 when no new device was found. Either the last search was
 *         the last device or there are no devices on the 1-Wire Net.
 */
#define owGetAdr64(pObj) (pObj->flags.searchResult?pObj->adr64:0)


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
BYTE owSelect(DS2482_INFO* pObj, BYTE* adr64);


/**
 * Same as owSelect() function, but with "Wait and Error Processing".
 * - Function blocks and waits till transmission is completed.
 * - Function only executed if current error state is 0 (ds2482_resetStatus() to reset)
 * - Status of transmission is returned.
 *
 * Before using any xxxWATE() functions, call ds2482_resetStatus() to reset "Wait
 * and Error Processing" (resets status to 0 = no error).
 *
 * Call ds2482_getStatus() to get current status (0 = success). If any xxWATE() function
 * called after ds2482_resetStatus() caused an error, this function will return a non-Zero value.
 *
 * @param pObj Pointer to DS2482_INFO structure
 * @param adr64 The 64-bit address of the device to select
 *
 * @return 0 if success, else error code (same as ds2482_getStatus() function)
 */
BYTE owSelectWATE(DS2482_INFO* pObj, BYTE* adr64);

#endif  //#if defined(NZ_OW2482_ENABLED)
#endif  //#ifndef NZ_OW2482_H
