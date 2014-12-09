/**
 * @brief           Functions for ISL1208 RTC used on SBC66 Boards
 * @file            nz_rtc.c
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
 * 2014-01-02, David H (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_RTC_C

#include "HardwareProfile.h"

#if defined(HAS_NZ_RTC)

#include "nz_rtc.h"
#include "nz_serI2C.h"

//Add debugging to this file. The DEBUG_CONF_DHT22 macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_RTC)
    #define DEBUG_CONF_RTC       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_RTC
#include "nz_debug.h"


////////// Defines //////////////////////////////
#if !defined (NZ_RTC_UNLOCK)
#define NZ_RTC_UNLOCK           (1)
#endif

#define RTC_GET_ERROR_CHECKING  (1)
#define RTC_SET_ERROR_CHECKING  (1)
#define RTC_I2C_ADDR            (0xDE)

//RTC register addresses
#define RTC_SEC_REG_ADDR        0x0
#define RTC_MIN_REG_ADDR        0x1
#define RTC_HR_REG_ADDR         0x2
#define RTC_DATE_REG_ADDR       0x3
#define RTC_MON_REG_ADDR        0x4
#define RTC_YR_REG_ADDR         0x5
#define RTC_DAY_REG_ADDR        0x6
#define RTC_STAT_REG_ADDR       0x7

//RTC control register bits

// RTC status register bits
#define RTC_STAT_BIT_ARST       0x80    // AUTO RESET ENABLE BIT
#define RTC_STAT_BIT_XTOSCB     0x40    // CRYSTAL OSCILLATOR ENABLE BIT
#define RTC_STAT_BIT_WRTC       0x10    // WRITE RTC ENABLE BIT
#define RTC_STAT_BIT_ALM        0x04    // ALARM BIT
#define RTC_STAT_BIT_BAT        0x02    // BATTERY BIT
#define RTC_STAT_BIT_RTCF       0x01    // REAL TIME CLOCK FAIL BIT

typedef struct __attribute__((__packed__)) {
    BYTE sec;   //The current time in seconds ( 0 to 59 )
    BYTE min;   //The current time in Minutes ( 0 to 59 )
    BYTE hour;  //The current time in Hours ( 0 to 23 )
    BYTE day;   //The current date, in day of the month ( 1 to 31 )
    BYTE month; //The current date, in month of the year ( 1 to 12 )
    BYTE year;  //The current date, in years since 2000. For example, is 14 for 2014
    BYTE wday;  //The current date, in day of week ( 0 to 6 where Sunday = 0)

    struct
    {
        unsigned char bDst : 1;     ///< Daylight saving time flag
        unsigned int  bFill : 7;
    } Flags;                        ///< Flag structure
} RTC_INFO;

////////// Function Prototypes //////////////////
BYTE bcd2bin (BYTE val, BYTE bcdFormat);
BYTE bin2bcd (BYTE val);



////////// Variables ////////////////////////////
//RTC_INFO    rtcInfo;
I2C_INFO*   objI2C = I2C1_INFO;     //Use I2C 1

/**
 * RTC Initialization functions
 * @return Returns 0 if OK, else error code
 */
BYTE rtcInit(void) {
    BYTE c;
    //memset(&rtcInfo, 0, sizeof(RTC_INFO));

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nrtcInit()");

    //If NZ_RTC_UNLOCK is set to '1', unlock RTC during initialization
    #if (NZ_RTC_UNLOCK==1)
    rtcUnlock();
    #endif

    if (rtcGetReg(RTC_STAT_REG_ADDR, &c) == 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nRTC Status Reg = 0x");
        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, c);
    }

    return 0;
}


/**
 * Disable modifying RTC time and date registers. This function disables all rtcSetXxx()
 * functions that modify the RTC time and date.
 * @return Returns 0 if OK, else error code
 */
BYTE rtcLock(void) {
    BYTE c;

    if (rtcGetReg(RTC_STAT_REG_ADDR, &c) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nrtcUnlock Error!");
        return 1;   //Error
    }

    //Clear bit 4 - "Write RTC Enable" bit
    rtcSetReg(RTC_STAT_REG_ADDR, (c & 0xEF));
    return 0;
}


/**
 * Enable modifying RTC time and date registers. This function is required before using
 * any rtcSetXxx() functions that modify the RTC time and date.
 * @return Returns 0 if OK, else error code
 */
BYTE rtcUnlock(void) {
    BYTE c;

    if (rtcGetReg(RTC_STAT_REG_ADDR, &c) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nrtcUnlock Error!");
        return 1;   //Error
    }

    //Set bit 4 - "Write RTC Enable" bit
    rtcSetReg(RTC_STAT_REG_ADDR, (c | 0x10));
    return 0;
}


/////////////////////////////////////////////////
// Get Time & Date Functions
/////////////////////////////////////////////////

/**
 * Get the current time in Hours ( 0 to 23 )
 * @return The current time in Hours ( 0 to 23 )
 */
BYTE rtcGetHour(void) {
    BYTE value;
    //Read the current 8-bit register value
#if (RTC_GET_ERROR_CHECKING==1)
    if (rtcGetReg(RTC_HR_REG_ADDR, &value) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nRTC Error!");
        return 0;
    }
#else
    rtcGetReg(RTC_HR_REG_ADDR, &value);
#endif

    //12H format
    if ((value | 0x80) == 0) {
        //Bit 5 indicates AM/PM. If set, add 12 hours to time
        if ((value | 0x20) != 0) {
            value = ((value & 0x1F)+12);
        }
    }

    return bcd2bin(value & 0x7f, 0);   //Return hours value 0 to 23. Clear bit 7, is the 12H/24H flag
}


/**
 * For time only, use offset 0 and size 3, pt contains pointer to RTC_TIME
 * For date only, use offset 3 and size 4, pt contains pointer to RTC_DATE
 * For time and date only, use offset 0 and size 7, pt contains pointer to RTC_TIME_AND_DATE
 *
 * @param pt Pointer to RTC_TIME, RTC_DATE or RTC_TIME_AND_DATE structure
 * @param offset Use 0 for time, and 3 for date
 * @param size Number of bytes to read
 * @param bcdFormat If 1, the returned data is in BCD format. Else, it is normal binary
 * @return Returns 0 if success, else error
 */
BYTE nz_rtcGetTimeAndDate(BYTE* pt, BYTE offset, BYTE size, BYTE bcdFormat) {
    BYTE msg[7];
    //memset(msg, 0, sizeof(msg));
    i2cBeginTransmission(objI2C, RTC_I2C_ADDR);     //Start transmission to slave at address RTC_I2C_ADDR
    i2cWrite(objI2C, 0x0 + offset);                 //Write 0 = address of register to read = 0 = 'seconds'
    i2cReadToArray(objI2C, &msg[offset], size);     //Read 2 bytes to buf
    i2cEndTransmission(objI2C);                     //End transmission, and start sending message in background

    //Wait till transmission finished (background via interrupt), and get status
#if (RTC_GET_ERROR_CHECKING==1)
    if (i2cGetStatusWait(objI2C, 0) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nnz_rtcGetTimeAndDate() I2C Error!");
        return 1;   //Error
    }
#else
    i2cGetStatusWait(objI2C, 0);
#endif

    //Requested Time
    if (offset==0) {
        //12H format
        if ((msg[2] | 0x80) == 0) {
            *pt = bcd2bin(msg[2] & 0x1f, bcdFormat);
            //Bit 5 indicates AM/PM. If set, add 12 hours to time
            if ((msg[2] | 0x20) != 0) {
                if (bcdFormat==1) {
                    *pt += 0x12;  //BCD format, add 1 to upper nibble, and 2 to lower nibble
                }
                else {
                    *pt += 12;  //Binary format
                }
            }
        }
        //24H
        else {
            *pt = bcd2bin(msg[2] & 0x3f, bcdFormat);
        }
        pt++;
        //Minute
        *pt++ = bcd2bin(msg[1], bcdFormat);
        //Second
        *pt++ = bcd2bin(msg[0], bcdFormat);
    }

    //Requested Time and Data, or Date only.
    if (size != 3) {
        *pt++ = bcd2bin(msg[3], bcdFormat);
        *pt++ = bcd2bin(msg[4], bcdFormat);
        *pt++ = bcd2bin(msg[5], bcdFormat);
        *pt++ = msg[6];
    }

    return 0;
}


/**
 * Get current time, and write values to given td (RTC_TIME) structure.
 * This function blocks until data is read from RTC, which takes about 220uS when
 * the I2C bus is configured for 400kbps.
 *
 * The given RTC_TIME structure has the following format:
 * td.hour The current time in Hours ( 0 to 23 )
 * td.min The current time in Minutes ( 0 to 59 )
 * td.sec The current time in seconds ( 0 to 59 )
 *
 * For example:
 * @code
 * RTC_TIME tm;
 * rtcGetTime(&tm, 0);
 * @endcode
 *
 * @param td Pointer to a RTC_TIME structure containing new time and date
 * @param bcdFormat If 0, returned data is in standard binary format. If 1, it is in BCD format.
 * 
 * @return 0 if success, else error
 */
BYTE rtcGetTime(RTC_TIME* tm, BOOL bcdFormat)  {
    return nz_rtcGetTimeAndDate((BYTE*)tm, 0, 3, bcdFormat);
}


/**
 * Get current date, and write values to given td (RTC_DATE) structure.
 * This function blocks until data is read from RTC, which takes about 250uS when
 * the I2C bus is configured for 400kbps.
 *
 * The given RTC_DATE structure has the following format:
 * td.day The current date, in day of the month ( 1 to 31 )
 * td.month The current date, in month of the year ( 1 to 12 )
 * td.year The current date, in years since 2000. For example, is 14 for 2014
 * td.wday The current date, in day of week ( 0 to 6 where Sunday = 0)
 *
 * For example:
 * @code
 * RTC_DATE dt;
 * rtcGetDate(&dt);
 * @endcode
 *
 * @param td Pointer to a RTC_DATE structure containing new time and date
 * @param bcdFormat If 0, returned data is in standard binary format. If 1, it is in BCD format.
 * @return 0 if success, else error
 */
BYTE rtcGetDate(RTC_DATE* dt, BOOL bcdFormat)  {
    return nz_rtcGetTimeAndDate((BYTE*)dt, 3, 4, bcdFormat);
}


/**
 * Get current time, and write values to given td (RTC_TIME_AND_DATE) structure.
 * This function blocks until data is read from RTC, which takes about 350uS when
 * the I2C bus is configured for 400kbps.
 *
 * The given RTC_TIME_AND_DATE structure has the following format:
 * td.hour The current time in Hours ( 0 to 23 )
 * td.min The current time in Minutes ( 0 to 59 )
 * td.sec The current time in seconds ( 0 to 59 )
 * td.day The current date, in day of the month ( 1 to 31 )
 * td.month The current date, in month of the year ( 1 to 12 )
 * td.year The current date, in years since 2000. For example, is 14 for 2014
 * td.wday The current date, in day of week ( 0 to 6 where Sunday = 0)
 *
 * For example:
 * @code
 * RTC_TIME_AND_DATE td;
 * rtcGetTimeAndDate(&td);
 * @endcode
 *
 * @param td Pointer to a RTC_TIME_AND_DATE structure containing new time and date
 * @param bcdFormat If 0, returned data is in standard binary format. If 1, it is in BCD format.
 * @return 0 if success, else error
 */
BYTE rtcGetTimeAndDate(RTC_TIME_AND_DATE* td, BOOL bcdFormat) {
    return nz_rtcGetTimeAndDate((BYTE*)td, 0, 7, bcdFormat);
}


/////////////////////////////////////////////////
// Set Time & Date Functions
/////////////////////////////////////////////////


/**
 * Set the 'hour' part of the current time ( 0 to 23 ). This function blocks until data
 * is written to RTC, which is about 100uS for default I2C speed of 400kbps.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param hour The hour to set the RTC hours to (0 to 23)
 * @return 0 if success, else error
 */
BYTE rtcSetHour(BYTE hour) {
    //Set hour register. Set bit 7 = 24H format
    return rtcSetReg(RTC_HR_REG_ADDR, (bin2bcd(hour) | 0x80) );
}


/**
 * Set current time with values given in tm (RTC_TIME) structure.
 * This function blocks until data is read from RTC, which takes about 175uS when
 * the I2C bus is configured for 400kbps.
 *
 * @preCondition rtcUnlock() must have been called to enable register modification.
 *
 * The given RTC_TIME structure must have the following format:
 * td.hour The current time in Hours ( 0 to 23 )
 * td.min The current time in Minutes ( 0 to 59 )
 * td.sec The current time in seconds ( 0 to 59 )
 *
 * For example:
 * @code
 * RTC_TIME tm = {8,15,30};    //8:15;30
 * rtcSetTime(&tm);
 * @endcode
 *
 * @param tm Pointer to a RTC_TIME structure containing new time
 * @return 0 if success, else error
 */
BYTE rtcSetTime(RTC_TIME* tm) {
    BYTE msg[4];

    //Convert all fields to BCD
    msg[1] = bin2bcd(tm->sec);
    msg[2] = bin2bcd(tm->min);
    msg[3] = bin2bcd(tm->hour) | 0x80;  //Set bit 7 - 24H format

    //Sequencial write, starting at regigister with address 0
    msg[0] = 0; //Register address 0
    i2cWriteMessage(objI2C, RTC_I2C_ADDR, msg, sizeof(msg));

    //Wait till transmission finished (background via interrupt), and check if successful
#if (RTC_SET_ERROR_CHECKING==1)
    if (i2cGetStatusWait(objI2C, 0) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\rtcSetTime() I2C Error!");
        return 1;   //Error
    }
#else
    i2cGetStatusWait(objI2C, 0);
#endif
    return 0;
}


/**
 * Set current time with values given in td (RTC_TIME_AND_DATE) structure.
 * This function blocks until data is read from RTC, which takes about 300uS when
 * the I2C bus is configured for 400kbps.
 *
 * @preCondition rtcUnlock() must have been called to enable register modification.
 *
 * The given RTC_TIME_AND_DATE structure must have the following format:
 * td.hour The current time in Hours ( 0 to 23 )
 * td.min The current time in Minutes ( 0 to 59 )
 * td.sec The current time in seconds ( 0 to 59 )
 * td.day The current date, in day of the month ( 1 to 31 )
 * td.month The current date, in month of the year ( 1 to 12 )
 * td.year The current date, in years since 2000. For example, is 14 for 2014
 * td.wday The current date, in day of week ( 0 to 6 where Sunday = 0)
 *
 * For example:
 * @code
 * RTC_TIME_AND_DATE td = {8,15,30,25,12,10,0};    //8:15;30 25 Dec 2010
 * rtcSetTimeAndDate(&td);
 * @endcode
 *
 * @param td Pointer to a RTC_TIME_AND_DATE structure containing new time and date
 * @return 0 if success, else error
 */
BYTE rtcSetTimeAndDate(RTC_TIME_AND_DATE* td) {
    BYTE msg[8];

    //Convert all fields to BCD
    msg[1] = bin2bcd(td->sec);
    msg[2] = bin2bcd(td->min);
    msg[3] = bin2bcd(td->hour) | 0x80;  //Set bit 7 - 24H format
    msg[4] = bin2bcd(td->day);
    msg[5] = bin2bcd(td->month);
    msg[6] = bin2bcd(td->year);
    msg[7] = td->wday;

    //Sequencial write, starting at regigister with address 0
    msg[0] = 0; //Register address 0
    i2cWriteMessage(objI2C, RTC_I2C_ADDR, msg, sizeof(msg));

    //Wait till transmission finished (background via interrupt), and check if successful
#if (RTC_SET_ERROR_CHECKING==1)
    if (i2cGetStatusWait(objI2C, 0) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\rtcSetTimeAndDate() I2C Error!");
        return 1;   //Error
    }
#else
    i2cGetStatusWait(objI2C, 0);
#endif
    return 0;
}


/////////////////////////////////////////////////
// Flag and Alarm Functions
/////////////////////////////////////////////////


BOOL rtcDidPowerFail() {
    BYTE value;
    //Read the 8-bit register value
    rtcGetReg(RTC_STAT_REG_ADDR, &value);

    //Return the status of the RTCF bit
    return ((value&0x01)==0) ? 0 : 1;
}

BOOL rtcGetBatteryFlag() {
    BYTE value;
    //Read the 8-bit register value
    rtcGetReg(RTC_STAT_REG_ADDR, &value);

    //Return the status of the BAT bit
    return ((value&0x02)==0) ? 0 : 1;
}

void rtcClearBatteryFlag() {
    BYTE value;
    //Read the current 8-bit register value
    if (rtcGetReg(RTC_STAT_REG_ADDR, &value) == 0) {
        //Write the value back out, and clear BAT bit
        rtcSetReg(RTC_STAT_REG_ADDR, (value & 0b11111101));
    }
}

BOOL rtcGetAlarmFlag() {
    BYTE value;
    //Read the 8-bit register value
    rtcGetReg(RTC_STAT_REG_ADDR, &value);

    //Return the status of the ALM bit
    return ((value&0x04)==0) ? 0 : 1;
}

void rtcClearAlarmFlag() {
    BYTE value;
    //Read the current 8-bit register value
    if (rtcGetReg(RTC_STAT_REG_ADDR, &value) == 0) {
        //Write the value back out, and clear ALM bit
        rtcSetReg(RTC_STAT_REG_ADDR, (value & 0b11111011));
    }
}

//BYTE rtcGetOscillatorMode()
//{
//    //Read the 8-bit register value
//    char value = rtcGetReg(RTC_STAT_REG_ADDR);
//
//    //Return the status of the XTOSCB bit
//    if (value & (1 << 6))
//        return OSCILLATOR_EXTERNAL;
//    else
//        return OSCILLATOR_CRYSTAL;
//}

//void rtcSetOscillatorMode(BYTE mode)
//{
//    //Read the current 8-bit register value
//    char value = rtcGetReg(RTC_STAT_REG_ADDR);
//
//    //Set or clear the XTOSCB bit
//    if (mode == OSCILLATOR_EXTERNAL)
//        value |= (1 << 6);
//    else
//        value &= ~(1 << 6);
//
//    //Write the value back out
//    rtcSetReg(RTC_STAT_REG_ADDR, value);
//}


/////////////////////////////////////////////////
// Other Functions
/////////////////////////////////////////////////

/**
 * Get a register that is in BCD format (sec, min, hour, day, month, year), and
 * return the converted binary format.
 * @param reg Register address to get
 * @return Returns the requested register, converted to binary (from BCD)
 */
BYTE nz_rtcGetRegBcd2Bin(BYTE reg) {
    BYTE value;
    rtcGetReg(reg, &value);
    return bcd2bin(value, 0);
}


/**
 * Set given register (sec, min, hour, day, month, year) with given value. The given value
 * must be in binary format, and it converted to BCD format as required by the register.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param reg Register address to write given value to
 * @param val Value to convert to BCD, and write to given register.
 * @return Returns 0 if OK, else error
 */
BYTE nz_rtcSetRegBin2Bcd(BYTE reg, BYTE val) {
    return rtcSetReg(reg, bin2bcd(val));
}


/**
 * Get value of given RTC register
 * @param regAdr Address of register to get
 * @param reg Pointer to byte to write read value to
 * @return Returns 0 if success, else error code
 */
BYTE rtcGetReg(BYTE regAdr, BYTE* reg) {
    //Read status register, and save result in c
    i2cReadSlaveReg(objI2C, RTC_I2C_ADDR, regAdr, reg, 1);

    //Wait till transmission finished (background via interrupt), and check if successful
    if (i2cGetStatusWait(objI2C, 0) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\rtcGetReg() I2C Error!");
        *reg = 0;   //Return 0
        return 1;   //Error
    }
    return 0;
}


/**
 * Set value of given RTC register
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param regAdr Address of register to set
 * @param val Value to set register to
 * @return Returns 0 if success, else error code
 */
BYTE rtcSetReg(BYTE regAdr, BYTE val) {
    i2cBeginTransmission(objI2C, RTC_I2C_ADDR);
    i2cWrite(objI2C, regAdr);
    i2cWrite(objI2C, val);
    i2cEndTransmission(objI2C);

    //Wait till transmission finished (background via interrupt), and check if successful
    if (i2cGetStatusWait(objI2C, 0)!=0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\rtcSetReg() I2C Error!");
        return 1;
    }
    return 0;
}


/**
 * BCD format to binary
 * @param val
 * @param noConversion If 1, the returned data is in BCD format - no conversion is done!
 *        Else, it is converted to binary
 * @return
 */
BYTE bcd2bin (BYTE val, BYTE noConversion) {
    //If 1, no conversion is done
    if (noConversion==1)
        return val;

    return ((((val >> 4) & 0x0F) * 10) + (val & 0x0F));
}

/**
 * Binary to BCD format
 * @param val
 * @return
 */
BYTE bin2bcd (BYTE val) {
    return (((val / 10) << 4) | (val % 10));
}

#endif  //#if defined(HAS_NZ_RTC)
