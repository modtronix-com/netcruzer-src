/**
 * @brief           Tick Functions, Complex version! For the simple version, use nz_tick.h and nz_tick.c files.
 * @file            nz_tickCx.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_tickCx_desc Description
 *****************************************
 *  To use this module:
 *  - If required, customize defines specified in "tick Configuration" below
 *  - tickInit() must be called before using this module
 *  - tickService() must be called every system tick time
 * <br><b>NOTE:</b> when nzSysInitDefault() is used, tickInit() and tickService() are automatically called by the system, and nothing has to be done.
 *
 * The system tick uses Timer 1, and has a period of 1ms. Each 1ms an interrupt is triggered, and the 32-bit system tick is incremented.
 * To make code smaller and more efficient, various functions are available for using 8, 16 and 32-bit variables for implementing timers
 * and delays. The 16-bit variants are the most efficient, and create the smallest code, seeing that the native data size for this CPU
 * is 16-bits. The 8-bit variants use the least RAM. The 32-bit variables can create the longest delays.
 *
 *  <b>Example default tick (16-bit, 1ms).</b> Can be used for a maximum of 32,768 ms = 32 seconds delay.
 * Requires 16-bit timer variable, which is the native data width of the processor! Produces fastest code.
 @code
    WORD tmrFlashLed=0;          //16-bit, 1ms Timer
    tmrFlashLed = tick16Get();      //Set with current 16-bit tick

    //Do something every 200ms
    if (tick16TestTmr(tmrFlashLed)) {
        //!!!!! IMPORTANT !!!!! Ensure given value is not more than 32,767!
        tick16UpdateTmrMS(tmrFlashLed, 200);     //Update timer to expire in 200ms again
        ..... Do Something ....
    }
 @endcode
 *
 *  <b>Example using 8-bit, 16ms tick.</b> Can be used for a maximum of 1,024ms = 1 second delay.
 * Requires only 8-bit timer variable, but has small maximum delay of 1 second. Can use 8-bit 256ms timer for longer delay with 8 bit timer.
 @code
    WORD tmr8bit16ms_ToggleX5=0;                //8-bit, 16ms Timer
    tmr8bit16ms_ToggleX5 = tick8Get_16ms();    //Set with current 8-bit, 16 ms tick

    //Do something every 800ms
    if (tick8HasExpired_16ms(tmr8bit16ms_ToggleX5)) {
        //!!!!! IMPORTANT !!!!! Never increment a 8-bit timer by more than 127!
        tick8_16ms_Update(tmr8bit16ms_ToggleX5, 800);   //Set to trigger again in 50x16 = 800ms
        LAT_05 = !LAT_05;                               //Toggle state of port 5 (old port name X5)
    }
 @endcode
 *
 *  <b>Example using 32-bit, 1ms tick.</b> Can be used for a maximum of 4,294,967,296 ms = 1,193 hours = 49.7 Days delay.
 * Requires 32-bit timer variable, generates more code, and is slower than other 8 and 16-bit functions.
 @code
    DWORD tmr32Task1Hour=0;             //Do something every hour (32-bit, 1ms Timer)
    tmr32Task1Hour = tick32Get();       //Set with current 32-bit tick.

    //Do something each hour = 3,600 seconds = 3,600,000 ms
    if (tick32TestTmr(tmr32Task1Hour)) {
        tick32UpdateTmrMin(tmr32Task1Hour, 60);    //Update timer to expire in 60 minutes

        //..... Add code here to be executed every hour
    }
 @endcode
 *
 *  <b>Example using 16-bit, 256ms tick.</b> Can be used for a maximum of 8,338 seconds = 2.3 Hours delay.
 * Requires 16-bit timer variable, which is the native data width of the processor! Produces fastest code.
 @code
    WORD tmr16bit256ms_Task1Hour=0;                 //Do something (approximately) every hour (16-bit, 256ms Timer)
    tmr16bit256ms_Task1Hour = tick16Get_256ms();   //Set with current 16-bit tick, 256 ms tick

    //Do something (approximately) each hour = 3,600 seconds = 3,600,000 ms
    if (tick16HasExpired_256ms(tmr16bit256ms_Task1Hour)) {
        //Set to trigger again in 1 hour (3,600,000 ms) = 3,600,000/256 = 14,062 (actually 3,599,872)
        tick16Update_256ms(tmr16bit256ms_Task1Hour, 14062);

        //..... Add code here to be executed every hour
    }
 @endcode
 *
 *
 * @subsection nz_tickCx_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // -------------- Tick Configuration (from nz_tickCx.h) ------------------
 // *********************************************************************
 //Indicates that the complex tick file nz_tickCx must be used, and not nz_tick
 #define NZSYS_ENABLE_COMPLEX_TICK  
   @endcode
 ************************************************************************
 * @section nz_tickCx_lic Software License Agreement
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
 * 2012-08-08, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#ifndef _NZ_TICK_CX_H_
#define _NZ_TICK_CX_H_

#if !defined(NZSYS_DONT_MANAGE_TICK)

#include "nz_tick.h"

///////////////////////////////////////////////////////////////////////////////
// 16-bit, 256ms tick functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Get the current 16-bit, 256ms Tick. Has a maximum value
 * 8,338 seconds = 2.3 Hours
 *
 * @return The current 16-bit, 256ms Tick.
 */
WORD tick16Get_256ms(void);

/** Alternative function name for tick16Get_256ms(). Gets 16-bit, 256ms tick. */
#define getTick16bit_256ms()  tick16Get_256ms()


/**
 * Will return true once the 16 bit(256ms) tick is > given value. Tick is a 256ms counter.
 * Can have a maximum value of 8,338 seconds = 2.3 Hours
 *
 * @param v Given value to compare it
 */
BOOL tick16HasExpired_256ms(WORD v);


/**
 * Update the given timer to expire in the given multiple of 256ms. This timer has a 256ms
 * base, so the given value will be multiplied by 256ms. For example, if 5 is given, it will cause
 * the timer to expire in 5x256ms = 1.28 seconds
 *
 * @param tmr   Timer to add the given milliseconds too
 * @param msVal Multiple of 256ms when timer will exprire!
 */
#define tick16Update_256ms(tmr, msVal) (tmr += msVal)



///////////////////////////////////////////////////////////////////////////////
// 8-bit, 1ms tick functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Current system 8-bit tick. Has a resolution of 1ms. Has a maximum value of 128ms
 */
#define tick8Get_1ms()   (tick_val.v[0])


/**
 * Will return true once the "8 bit tick" is > given value.
 * Can have a maximum value of 128ms
 */
#define tick8HasExpired_1ms(v)      ((v - TICK.v[0]) & 0x80)


///////////////////////////////////////////////////////////////////////////////
// 8-bit, 16ms tick functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Get the current 8-bit, 16ms Tick. Has a maximum value of 2,048ms = 2 seconds
 *
 * @return The current 8-bit, 16ms Tick.
 */
BYTE tick8Get_16ms(void);


/**
 * Will return true once the "8 bit tick" is > given value.
 * Can have a maximum value of 1,024ms = 1 second
 *
 * @param v Given value to compare it
 */
#define tick8HasExpired_16ms(v)      ((v - ((UINT8)(tick_val.w[0]>>4))) & 0x80)


///////////////////////////////////////////////////////////////////////////////
// 8-bit, 256ms tick functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Second byte of current system 8-bit tick. Has a resolution of 256ms.
 * Has a maximum value of 32,768ms = 32.7 seconds
 */
#define tick8Get_256ms() (tick_val.v[1])

#endif  //#if !defined(NZSYS_DONT_MANAGE_TICK)
#endif  //end of #define _NZ_TICK_CX_H_
