/**
 * @brief           Functions for ISL1208 RTC used on SBC66 Boards
 * @file            nz_rtc.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_rtc_desc Description
 *****************************************
 * Functions for using the ISL1208 RTC used on SBC66 Boards
 * 
 * @subsection nz_rtc_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ------------ RTC Configuration (from nz_rtc.h) -------------
// *********************************************************************
//#define     NZ_RTC_ENABLE                         //[-DEFAULT-] RTC is disabled by default
#define     NZ_RTC_UNLOCK                       (1) //[-DEFAULT-] Unlock RTC during initialization
 @endcode
 *
 * 
 * @subsection nz_rtc_usage Usage
 *****************************************
 * To use this module, the following must be done:
 * - Include nz_rtc.h in the c file it is used in.
 * - Add nz_rtc.c to the MPLAB project, this is the main RTC driver file.
 * - The following additional files are required by nz_rtc.c, and must be added to the project:
 *   nz_circularBufferPwr2.c, nz_helpers.c, nz_netcruzer.c and nz_serI2C.c
 * - Add "#define NZ_RTC_ENABLE (1)" to projdefs.h file.
 * - In projdefs.h, do any RTC, I2C or other configuration required. This is done by copying the
 *   configuration section from the *.h files to projdefs.h. Nothing required, defaults will work!
 * - <b>All DONE!</b> Can now use RTC functions defined in nz_rtc.h! Some examples:
 *     RTC_TIME_AND_DATE td = {8,15,30,25,12,10,0};    //8:15;30 25 Dec 2010
 *     rtcSetTimeAndDate(&td);                         //Set RTC time and date
 * 
 **********************************************************************
 * @section nz_rtc_lic Software License Agreement
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
#ifndef NZ_RTC_H
#define NZ_RTC_H

#if defined(HAS_NZ_RTC)

#include "nz_tick.h"

////////// Defines //////////////////////////////
#ifndef __INLINE_FUNCTION__
#define __INLINE_FUNCTION__ extern inline __attribute__((always_inline))
#endif

typedef struct __attribute__((__packed__)) {
    BYTE hour;  //The current time in Hours ( 0 to 23 )
    BYTE min;   //The current time in Minutes ( 0 to 59 )
    BYTE sec;   //The current time in seconds ( 0 to 59 )
    BYTE day;   //The current date, in day of the month ( 1 to 31 )
    BYTE month; //The current date, in month of the year ( 1 to 12 )
    BYTE year;  //The current date, in years since 2000. For example, is 14 for 2014
    BYTE wday;  //The current date, in day of week ( 0 to 6 where Sunday = 0)
} RTC_TIME_AND_DATE;

typedef struct __attribute__((__packed__)) {
    BYTE hour;  //The current time in Hours ( 0 to 23 )
    BYTE min;   //The current time in Minutes ( 0 to 59 )
    BYTE sec;   //The current time in seconds ( 0 to 59 )
} RTC_TIME;

typedef struct __attribute__((__packed__)) {
    BYTE day;   //The current date, in day of the month ( 1 to 31 )
    BYTE month; //The current date, in month of the year ( 1 to 12 )
    BYTE year;  //The current date, in years since 2000. For example, is 14 for 2014
    BYTE wday;  //The current date, in day of week ( 0 to 6 where Sunday = 0)
} RTC_DATE;


////////// Functions ////////////////////////////


/**
 * RTC Initialization functions
 * @return Returns 0 if OK, else error code
 */
BYTE rtcInit(void);


/**
 * Disable modifying RTC time and date registers. This function disables all rtcSetXxx()
 * functions that modify the RTC time and date.
 * @return Returns 0 if OK, else error code
 */
BYTE rtcLock(void);


/**
 * Enable modifying RTC time and date registers. This function is required before using
 * any rtcSetXxx() functions that modify the RTC time and date.
 * @return Returns 0 if OK, else error code
 */
BYTE rtcUnlock(void);


/**
 * Get a register that is in BCD format (sec, min, hour, day, month, year), and
 * return the converted binary format.
 * @param reg Register address to get
 * @return Returns the requested register, converted to binary (from BCD)
 */
BYTE nz_rtcGetRegBcd2Bin(BYTE reg);


/**
 * Set given register (sec, min, hour, day, month, year) with given value. The given value
 * must be in binary format, and it converted to BCD format as required by the register.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param reg Register address to write given value to
 * @param val Value to convert to BCD, and write to given register.
 * @return Returns 0 if OK, else error
 */
BYTE nz_rtcSetRegBin2Bcd(BYTE reg, BYTE val);

/**
 * Get value of given RTC register
 * @param regAdr Address of register to get
 * @param reg Pointer to byte to write read value to
 * @return Returns 0 if success, else error code
 */
BYTE rtcGetReg(BYTE regAdr, BYTE* reg);


/**
 * Set value of given RTC register.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param regAdr Address of register to set
 * @param val Value to set register to
 * @return Returns 0 if success, else error code
 */
BYTE rtcSetReg(BYTE regAdr, BYTE val);



/////////////////////////////////////////////////
// Get Time & Date Functions
/////////////////////////////////////////////////

/**
 * Get the current time in seconds ( 0 to 59 ). This function blocks until data
 * is read from RTC, which is about 150uS for default I2C speed of 400kbps.
 * @return The current time in seconds ( 0 to 59 )
 */
BYTE __INLINE_FUNCTION__ rtcGetSec(void) {
    return nz_rtcGetRegBcd2Bin(0x0);  //Return second register = 0x00
}


/**
 * Get the current time in Minutes ( 0 to 59 ). This function blocks until data
 * is read from RTC, which is about 150uS for default I2C speed of 400kbps.
 * @return The current time in Minutes ( 0 to 59 )
 */
BYTE __INLINE_FUNCTION__ rtcGetMin(void) {
    return nz_rtcGetRegBcd2Bin(0x01);  //Return min register = 0x01
}


/**
 * Get the current time in Hours ( 0 to 23 ). This function blocks until data
 * is read from RTC, which is about 150uS for default I2C speed of 400kbps.
 * @return The current time in Hours ( 0 to 23 )
 */
BYTE rtcGetHour(void);


/**
 * Get the current date, in day of the month ( 1 to 31 ). This function blocks until data
 * is read from RTC, which is about 150uS for default I2C speed of 400kbps.
 * @return The current date, in day of the month ( 1 to 31 )
 */
BYTE __INLINE_FUNCTION__ rtcGetDay(void)  {
    return nz_rtcGetRegBcd2Bin(0x03);  //Return hour register = 0x03
}


/**
 * Get the current date, in month of the year ( 1 to 12 ). This function blocks until data
 * is read from RTC, which is about 150uS for default I2C speed of 400kbps.
 * @return The current date, in month of the year ( 1 to 12 )
 */
BYTE __INLINE_FUNCTION__ rtcGetMonth(void)  {
    return nz_rtcGetRegBcd2Bin(0x04);  //Return hour register = 0x04
}


/**
 * Get the current date, in years since 2000. A value from 0 to 99
 * is returned. For example, 14 is returned for 2014. This function blocks until data
 * is read from RTC, which is about 150uS for default I2C speed of 400kbps.
 * @return The current date, in year since 2000. A value from 0 to 99.
 */
WORD __INLINE_FUNCTION__ rtcGetYear(void) {
    return nz_rtcGetRegBcd2Bin(0x05);  //Return hour register = 0x05
}


/**
 * Get the current week day. A number from 0 to 6 where Sunday = 0. This function blocks
 * until data is read from RTC, which is about 150uS for default I2C speed of 400kbps.
 * @return The current week day. A number from 0 to 6 where Sunday = 0.
 */
WORD __INLINE_FUNCTION__ rtcGetWeekDay(void) {
    return nz_rtcGetRegBcd2Bin(0x06);  //Return hour register = 0x06
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
 * rtcGetTime(&tm);
 * @endcode
 *
 * @param td Pointer to a RTC_TIME structure containing new time and date
 * @param bcdFormat If 0, returned data is in standard binary format. If 1, it is in BCD format.
 * @return 0 if success, else error
 */
BYTE rtcGetTime(RTC_TIME* tm, BOOL bcdFormat);


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
BYTE rtcGetDate(RTC_DATE* dt, BOOL bcdFormat);


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
BYTE rtcGetTimeAndDate(RTC_TIME_AND_DATE* td, BOOL bcdFormat);


/////////////////////////////////////////////////
// Set Time & Date Functions
/////////////////////////////////////////////////

/**
 * Set the 'seconds' part of the current time ( 0 to 59 ). This function blocks until data
 * is written to RTC, which is about 100uS for default I2C speed of 400kbps.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param sec The seconds to set the RTC seconds to ( 0 to 59 )
 * @return 0 if success, else error
 */
BYTE __INLINE_FUNCTION__ rtcSetSec(BYTE sec) {
    return nz_rtcSetRegBin2Bcd(0x0, sec);   //Set seconds register = address 0x0
}

/**
 * Set the 'minutes' part of the current time ( 0 to 59 ). This function blocks until data
 * is written to RTC, which is about 100uS for default I2C speed of 400kbps.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param min The minutes to set the RTC minutes to ( 0 to 59 )
 * @return 0 if success, else error
 */
BYTE __INLINE_FUNCTION__ rtcSetMin(BYTE min) {
    return nz_rtcSetRegBin2Bcd(0x01, min);  //Set minutes register = address 0x01
}


/**
 * Set the 'hour' part of the current time ( 0 to 23 ). This function blocks until data
 * is written to RTC, which is about 100uS for default I2C speed of 400kbps.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param hour The hour to set the RTC hours to (0 to 23)
 * @return 0 if success, else error
 */
BYTE rtcSetHour(BYTE hour);


/**
 * Set the 'day' part of the current time ( 0 to 6 ). This function blocks until data
 * is written to RTC, which is about 100uS for default I2C speed of 400kbps.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param day The day to set the RTC days to ( 0 to 6, where 0 is Sunday )
 * @return 0 if success, else error
 */
BYTE __INLINE_FUNCTION__ rtcSetDay(BYTE day) {
    return nz_rtcSetRegBin2Bcd(0x03, day);  //Set day register = address 0x03
}


/**
 * Set the 'month' part of the current date ( 0 to 11 ). This function blocks until data
 * is written to RTC, which is about 100uS for default I2C speed of 400kbps.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param month The month to set the RTC months to ( 0 to 11 )
 * @return 0 if success, else error
 */
BYTE __INLINE_FUNCTION__ rtcSetMonth(BYTE month) {
    return nz_rtcSetRegBin2Bcd(0x04, month);    //Set month register = address 0x04
}


/**
 * Set the 'year' part of the current date ( 0 to 99 ). This function blocks until data
 * is written to RTC, which is about 100uS for default I2C speed of 400kbps.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param year The year to set the RTC years to ( 0 to 99 )
 * @return 0 if success, else error
 */
BYTE __INLINE_FUNCTION__ rtcSetYear(WORD year) {
    return nz_rtcSetRegBin2Bcd(0x05, year);     //Set year register = address 0x05
}


/**
 * Set the 'week day' part of the current date ( 0 to 11 ). This function blocks until data
 * is written to RTC, which is about 100uS for default I2C speed of 400kbps.
 * @preCondition rtcUnlock() must have been called to enable register modification.
 * @param wday The week day to set the RTC week days to ( 0 to 6 )
 * @return 0 if success, else error
 */
BYTE __INLINE_FUNCTION__ rtcSetWeekDay(BYTE wday) {
    return nz_rtcSetRegBin2Bcd(0x06, wday);    //Set weekday register = address 0x06
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
BYTE rtcSetTime(RTC_TIME* tm);


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
BYTE rtcSetTimeAndDate(RTC_TIME_AND_DATE* td);


/////////////////////////////////////////////////
// Flag and Alarm Functions
/////////////////////////////////////////////////

BOOL rtcDidPowerFail();

BOOL rtcGetBatteryFlag();

void rtcClearBatteryFlag();

BOOL rtcGetAlarmFlag();

void rtcClearAlarmFlag();


/////////////////////////////////////////////////
// RAM Functions
/////////////////////////////////////////////////

/**
 * Get "User Ram 1". This function blocks until data is read from RTC, which is
 * about 150uS for default I2C speed of 400kbps.
 * @return The value of "User Ram 1" read from the RTC.
 */
BYTE __INLINE_FUNCTION__ rtcGetRAM1(void) {
    BYTE val;
    rtcGetReg(0x12, &val); //Get user ram 1 = address 0x12
    return val;
}

/**
 * Get "User Ram 2". This function blocks until data is read from RTC, which is
 * about 150uS for default I2C speed of 400kbps.
 * @return The value of "User Ram 2" read from the RTC.
 */
BYTE __INLINE_FUNCTION__ rtcGetRAM2(void) {
    BYTE val;
    rtcGetReg(0x13, &val); //Get user ram 2 = address 0x13
    return val;
}

/**
 * Set "User Ram 1". This function blocks until data is written to RTC, which is
 * about 100uS for default I2C speed of 400kbps.
 * @param val The value to set "User Ram 1" of the RTC to.
 * @return 0 if success, else error
 */
BYTE __INLINE_FUNCTION__ rtcSetRAM1(BYTE val) {
    return rtcSetReg(0x12, val);   //Set user ram 1 = address 0x12
}


/**
 * Set "User Ram 2". This function blocks until data is written to RTC, which is
 * about 100uS for default I2C speed of 400kbps.
 * @param val The value to set "User Ram 2" of the RTC to.
 * @return 0 if success, else error
 */
BYTE __INLINE_FUNCTION__ rtcSetRAM2(BYTE val) {
    return rtcSetReg(0x13, val);   //Set user ram 2 = address 0x13
}

#endif  //#if defined(HAS_NZ_RTC)
#endif  //#ifndef NZ_RTC_H
