/**
 * @brief           Tick Functions
 * @file            nz_tick.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_tick_desc Description
 *****************************************
 *  To use this module:
 *  - tickInit() must be called before using this module
 *  - tickService() must be called every system tick time
 * <br><b>NOTE:</b> when nzSysInitDefault() is used, tickInit() and tickService() are automatically
 * called by the system, and nothing has to be done.
 *
 * The system tick uses Timer 1, and has a period of 1ms. Each 1ms an interrupt is triggered, and the
 * 32-bit system tick is incremented. To make code smaller and more efficient, default tick functions
 * use the lower 16 bits of the 32-bit system tick. This limits the default functions to a maximum time
 * of 32,768ms = 32.7 seconds. If this is not sufficient, the 32-bit tick functions (tick32Xxx) can be
 * used, which have a maximum time of 4,294,967,296 ms = 1,193 hours = 49.7 Days.
 *
 * The 16-bit variants are the most efficient, and create the smallest and fastest code. Only use the
 * 32-bit variants when required!
 *
 *  <b>Example default tick (16-bit, 1ms).</b> Can be used for a maximum of 32,768 ms = 32 seconds delay.
 * Requires 16-bit timer variable, which is the native data width of the processor! Produces fastest code.
 @code
    WORD tmrFlashLed=0;             //16-bit, 1ms Timer
    tmrFlashLed = tick16Get();        //Set with current 16-bit tick

    //Do something every 200ms
    if (tick16TestTmr(tmrFlashLed)) {
        //!!!!! IMPORTANT !!!!! Ensure given value is not more than than 32,767!
        tick16UpdateTmrMS(tmrFlashLed, 200);    //Update timer to expire in 200ms again
        ..... Do Something ....
    }
 @endcode
 *
 * An alternative method is:
 @code
    WORD tmrFlashLed=0;             //16-bit, 1ms Timer
    tmrFlashLed = tick16Get();        //Set with current 16-bit tick

    //Do something every 200ms (IMPORTANT - ensure value is not more than 32,767!)
    if (tick16TestTmr(tmrFlashLed + tick16ConvertFromMS(200))) {
        tmrFlashLed = tick16Get();        //Undate with current 16-bit tick
        //..... Do Something ....
    }
 @endcode
 *
 *  <b>Example using 32-bit, 1ms tick.</b> Can be used for a maximum of 4,294,967,296 ms = 1,193 hours = 49.7 Days delay.
 * Requires 32-bit timer variable, generates more code, and is slower than other 8 and 16-bit functions.
 @code
    DWORD tmr32Task1Hour=0;         //Do something every hour (32-bit, 1ms Timer)
    tmr32Task1Hour = tick32Get();   //Set with current 32-bit tick.

    //Do something each hour = 3,600 seconds = 3,600,000 ms
    if (tick32TestTmr(tmr32Task1Hour)) {
        tick32UpdateTmrMin(tmr32Task1Hour, 60);     //Update timer to expire in 60 minutes

        //..... Add code here to be executed every hour
    }
 @endcode
 *
 * @subsection nz_tick_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // -------------- Tick Configuration (from nz_tick.h) ------------------
 // *********************************************************************
 //Define the interrupt priority level for system tick(TMR1), 1-7 (7 is highest)
 #define nzINT_PRIORITY_TICK                    ( 5 )       //[-DEFAULT-]

 //Define the interrupt priority level for TMR2 & TMR3 if uTick is configured to use them
 #define nzINT_PRIORITY_UTICK                   ( 6 )       //[-DEFAULT-]

 //Use Timer 2 and 3 for a dedicated uTick counter - for utickXxx() functions
 #define NZSYS_TICK_TMR2AND3_UTICK              ( 0 )       //[-DEFAULT-]
 @endcode
 ************************************************************************
 * @section nz_tick_lic Software License Agreement
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
#ifndef _NZDEFAULT_TICK_H_
#define _NZDEFAULT_TICK_H_

/////////////////////////////////////////////////
//  "Interrupts" - documentation module
/////////////////////////////////////////////////
/**
 @defgroup info_sys_tick System Tick
 @ingroup info_sys
 System ticks are used for timing and to periodically execute code. For details, see @ref nz_tick_desc "System Ticks".
*/

//The amount of uticks per micro second, default is 2
#if defined(__PIC24FJ256GB206__) || defined(__PIC24FJ256GB208__) || defined(__PIC24FJ256GB210__) || defined(__PIC24FJ128GB106__) || defined(__PIC24FJ128GB108__) || defined(__PIC24FJ128GB110__)
#ifndef NZ_UTICKS_PER_MS
#define NZ_UTICKS_PER_MS 2000
#endif
#elif defined(__PIC32MX__)
//Default is 80MHz crystal with 64 timer prescaller = 1250 ticks per milli second for timer
#ifndef NZ_UTICKS_PER_MS
#define NZ_UTICKS_PER_MS 1250
#endif
#endif


#ifndef __INLINE_FUNCTION__
#define __INLINE_FUNCTION__ extern inline __attribute__((always_inline))
#endif

////////// Default defines //////////////////////
#if defined(__C30__)

#ifndef nzINT_PRIORITY_TICK
#define nzINT_PRIORITY_TICK                     ( 5 )   //Define the interrupt priority level, 1-7 (7 is highest)
#endif

#ifndef nzINT_PRIORITY_UTICK
#define nzINT_PRIORITY_UTICK                    ( 6	)   //Define the interrupt priority level, 1-7 (7 is highest)
#endif

#if !defined(NZSYS_TICK_TMR2AND3_UTICK)
#define NZSYS_TICK_TMR2AND3_UTICK               ( 0 )   //Use Timer 2 and 3 for a dedicated uTick counter - for utickXxx() functions
#endif


#elif defined(__PIC32MX__)
	#if !defined(_T1IF)
		#define _T1IF	IFS0bits.T1IF
	#endif
	//IFS0CLR = _IFS0_T1IF_MASK;  //Clear interrupt flag
#endif

typedef WORD    TICK16;     //Used for tick16 and utick16 variables
typedef DWORD   TICK32;     //Used for tick32 and utick32 variables


/**
 * System 32-bit tick. Be very carefull if using this varialble. Ensure all operations are atomic.
 * If not sure, use SETTO_XX macros below!
 */
extern volatile DWORD_VAL tick_val;


/**
 * Increments all tick values when called
 */
#define tickService() {     \
    /* Increment tick */    \
    tick_val.Val++;         \
}


/**
 * Initializes tick values
 */
void tickInit(void);



///////////////////////////////////////////////////////////////////////////////
// Standard 16-bit, 1ms tick functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Current default tick, which is the 16-bit, 1ms tick. Has a resolution of 1ms.
 * Has a maximum value of 32,768ms = 32.7 seconds
 *
 * @return The current 16-bit, 1ms Tick.
 */
TICK16 __INLINE_FUNCTION__ tick16Get(void)
{
    return tick_val.w[0];
}


/**
 * @function BOOL tick16TestTmr(TICK16 tmr)
 *
 * Tests if the current 16-bit(1ms) tick has passed the given timer value.
 *
 * Will return true once the 16 bit(1ms) tick is > given value.
 * The given timer can have a maximum value of 32,768ms = 32 seconds.
 *
 * @param tmr   Timer to test
 *
 * @return      Returns TRUE if given timer has expired, else FALSE
 */
#define tick16TestTmr(tmr)          ( (((((tmr) - (tick_val.w[0]) ) & 0x8000))==0) ? 0 : 1)
//#define tick16HasTmrExpired(tmr)    ( (((((tmr) - (tick_val.w[0]) ) & 0x8000))==0) ? 0 : 1)


/**
 * @function void tick16SetTmrMS(TICK16 tmr, WORD msVal)
 *
 * Set the given timer to a time in the future, as given by msVal parameter in milliseconds.
 *
 * @param tmr   Timer to add set expirty time for
 * @param msVal Amount of milliseconds in which the timer will expire
 */
#define tick16SetTmrMS(tmr, msVal) (tmr = (tick16Get() + msVal))


/**
 * @function WORD tick16GetElapsedMS(TICK16 oldTick)
 *
 * Returns the time in milliseconds that has elapsed since the given old tick value.
 *
 * For example, to calculate how long some code takes:
 * @code
 *  TICK16 timeStart;
 *  timeStart = tick16Get();
 *
 *  //Do something that takes time....
 *
 *  //Print the time taken to the debug console
 *  debugPutString("\nTime taken in ms = ");
 *  debugPutWord(tick16GetElapsedMS(timeStart));
 * @endcode
 *
 * @param oldTick   Old TICK16 value
 *
 * @return Returns elapsed time in milliseconds
 */
#define tick16GetElapsedMS(oldTick) ((WORD)((tick_val.w[0]) - (oldTick)))


//The method below DOES NOT work for TRUE comparison!!!! Because value will be 0 or 0x8000, and TRUE is 1!
//For example, this will NEVER return qualify:  if (tick16TestTmr(100) == TRUE)
//#define tick16TestTmr(tmr)      ( ((tmr) - (tick_val.w[0]) ) & 0x8000)

/**
 * @function void tick16UpdateTmrMS(TICK16 tmr, WORD msVal)
 *
 * Add the given amount of milliseconds to the given timer. Updates the time when it will expire.
 *
 * The given time is added to the current timer value! Ensure current timer has a valid value
 * before calling this function!
 *
 * @param tmr   Timer to add the given milliseconds too
 * @param msVal Amount of milliseconds to add to timer
 */
#define tick16UpdateTmrMS(tmr, msVal) (tmr += msVal)


/**
 * @function void tick16ConvertFromMS(WORD msVal)
 *
 * Convert given value to a tick millisecond value. Seeing that the system tick is 1ms, no
 * conversion is required (1 to 1).
 *
 * For example, to initialize a timer with a value that will expire in 500 ms, following code can be used.
 * @code
 *  WORD tmrStartupDelay;   //Startup delay
 *  tmrStartupDelay = tick16Get() + tick16ConvertFromMS(500);
 *  while() {
 *      if (tick16TestTmr(tmrStartupDelay)) {
 *          tick16UpdateTmrMS(tmrStartupDelay, 100); //Update timer to expire in 100ms again
 *          // Do something ......
 *      }
 *  }
 * @endcode
 *
 * @param msVal Value to convert to a tick milliseconds value
 */
#define tick16ConvertFromMS(msVal) (msVal)



///////////////////////////////////////////////////////////////////////////////
// Long 32-bit, 1ms tick functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Get the current 32-bit, 1ms Tick. Can have a maximum value of:
 * 4,294,967,296 ms = 1,193 hours = 49.7 Days
 *
 * @return The current 32-bit, 1ms Tick.
 */
DWORD tick32Get(void);


/**
 * Same as tick32Get() but does not disable interrupts.
 * Only call this function from an ISR with higher priority than the system tick.
 * That is an ISR with priority 5 - 7, for default system tick with priority 4.
 */
#define tick32Get_noDisi()	(tick_val.Val)


/**
 * @function BOOL tick32TestTmr(TICK32 tmr)
 *
 * Tests if the current 32-bit(1ms) tick has passed the given timer's value.
 *
 * For efficiency and code size, try to use tick16 functions where possible
 * (tick16TestTmr() for example).
 *
 * Will return true once the 32 bit(1ms) tick is > given value.
 * Tick is a 1ms counter. Can have a maximum value of:
 * 4,294,967,296 ms = 1,193 hours = 49.7 Days
 *
 * @param tmr   Timer to test
 *
 * @return      Returns TRUE if given timer has expired, else FALSE
 */
BOOL tick32TestTmr(DWORD tmr);


/**
 * @function void tick32SetTmrMS(TICK32 tmr, WORD msVal)
 *
 * Set the given timer to a time in the future, as given by msVal parameter in milliseconds.
 *
 * @param tmr   Timer to add set expirty time for
 * @param msVal Amount of milliseconds in which the timer will expire
 */
#define tick32SetTmrMS(tmr, msVal) (tmr = (tick32Get() + msVal))


/**
 * Same as tick32TestTmr, but does not disable interrupts
 * Only call this function from an ISR with higher priority than the system tick.
 * That is an ISR with priority 5 - 7, for default system tick with priority 4.
 * @param tmr Given value to compare it
 */
#define  tick32TestTmr_noDisi(tmr)	((((tmr-tick_val.Val)&0x80000000)==0) ? 0 : 1)


/**
 * @function void tick32UpdateTmrMS(TICK32 tmr, WORD msVal)
 *
 * Add the given amount of milliseconds to the given timer. Updates the time when it will expire.
 *
 * The given time is added to the current timer value! Ensure current timer has a valid value
 * before calling this function!
 *
 * @param tmr   Timer to add the given milliseconds too
 * @param msVal Amount of milliseconds to add to timer
 */
#define tick32UpdateTmrMS(tmr, msVal) (tmr += msVal)


/**
 * Update the given timer to expire in the given amount of seconds.
 *
 * The given time is added to the current timer value! Ensure current timer has a valid value
 * before calling this function!

 * @param tmr   Timer to add the given milliseconds too
 * @param secVal Amount of seconds to add to timer
 */
#define tick32UpdateTmrSec(tmr, secVal) (tmr += (((DWORD)secVal)*1000))


/**
 * Update the given timer to expire in the given number of minutes.
 *
 * @param tmr   Timer to add the given milliseconds too
 * @param minVal Amount of minutes to add to timer
 */
#define tick32UpdateTmrMin(tmr, minVal) (tmr += ( ((DWORD)minVal)*60000))


/**
 * @function void tick32ConvertFromMS(DWORD msVal)
 *
 * Convert given value to a tick millisecond value. Seeing that the system tick is 1ms, no
 * conversion is required (1 to 1).
 *
 * For example, to initialize a timer with a value that will expire in 500 ms, following code can be used.
 * @code
 *  WORD tmr32StartupDelay;         //Startup delay
 *  tmr32StartupDelay = tick32Get() + tick32ConvertFromMS(500);
 *  while() {
 *      if (tick32TestTmr(tmr32StartupDelay)) {
 *          tick32UpdateTmrMS(tmr32StartupDelay, 100); //Update timer to expire in 100ms again
 *          // Do something ......
 *      }
 *  }
 * @endcode
 *
 * @param msVal Value to convert to a tick milliseconds value
 */
#define tick32ConvertFromMS(msVal) ((DWORD)(msVal))



///////////////////////////////////////////////////////////////////////////////
// Micro Tick functions
///////////////////////////////////////////////////////////////////////////////

/**
 * A "Micro Tick" has a resolution of 1us or less. It will always have a whole number
 * of ticks per millisecond, as defined by NZ_UTICKS_PER_MS. The following are common
 * values:
 * 16MHz timer with 8 prescaler = 0.5us resolution = NZ_UTICKS_PER_MS = 2000
 * 60MHz timer with 8 prescaler = 0.13333us resolution = NZ_UTICKS_PER_MS = 7500
 * 72MHz timer with 8 prescaler = 0.88888us resolution = NZ_UTICKS_PER_MS = 1125
 * 80MHz timer with 8 prescaler = 0.1us resolution = NZ_UTICKS_PER_MS = 10000
 * 80MHz timer with 64 prescaler = 0.8us resolution = NZ_UTICKS_PER_MS = 800
 * 120MHz timer with 64 prescaler = 0.53333us resolution = NZ_UTICKS_PER_MS = 1875
 */


/**
 * Get the current 16 bit, micro tick. The NZ_UTICKS_PER_MS defines how many micro ticks
 * there are per milli second. For current range of SBC66 boards it is always 2000.
 *
 * For NZ_UTICKS_PER_MS=1000, Has a maximum value of 32,768us = 32.7 milliseconds
 * For NZ_UTICKS_PER_MS=2000, Has a maximum value of 16,384us = 16 milliseconds
 *
 * @return The current 16-bit, Micro Tick.
 */
#if (NZSYS_TICK_TMR2AND3_UTICK==1)
#define utick16Get()	(TMR2)
#else
WORD utick16Get(void);
#endif


/**
 * Same as utick16Get(), but does not disable interrupts.
 * Only call this function from an ISR with higher priority than the system tick.
 * That is an ISR with priority 5 - 7, for default system tick with priority 4.
 * @return The current 16-bit, Micro Tick.
 */
WORD __INLINE_FUNCTION__ utick16Get_noDisi(void) {
#if (NZSYS_TICK_TMR2AND3_UTICK==0)
    WORD tmrCpy;
    tmrCpy  = TMR1;

	//No tick ISR got triggered while we disabled interrupts. This means TMR1 did
    //NOT roll over!
    if (_T1IF == 0) {
        return ((tick_val.word.LW*NZ_UTICKS_PER_MS) + tmrCpy);
    }
    //ELSE
    //The TMR1 IF got triggered recently, and TMR1 ISR has not been executed yet! This
	//means that TMR1 can be = PR value, or reset to 0. It also means that tick_val was NOT
	//incremented yet, and will be incremented once TMR ISR is entered.

    #if (NZ_UTICKS_PER_MS==2000)
		//T1IF is triggered when TRM1=PR=1999. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1999 to 1-2000
		if (tmrCpy==2000) tmrCpy=0;	//Convert from 1-2000 to 0-1999

		return (((tick_val.word.LW+1)*NZ_UTICKS_PER_MS) + tmrCpy);
	#else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
#else
	return TMR2;
#endif
}


/**
 * Get the current 32 bit, micro tick. The NZ_UTICKS_PER_MS defines how many micro ticks
 * there are per milli second. For current range of SBC66 boards it is always 2000.
 *
 * For NZ_UTICKS_PER_MS=1000, Has a maximum value of 2,147 Seconds = 35.7 Minutes
 * For NZ_UTICKS_PER_MS=2000, Has a maximum value of 1,073 Seconds = 17.9 Minutes
 * For NZ_UTICKS_PER_MS=10000, Has a maximum value of 214 Seconds = 3.57 Minutes
 *
 * @return The current 32-bit, Micro Tick.
 */
#if (NZSYS_TICK_TMR2AND3_UTICK==1)
DWORD __INLINE_FUNCTION__ utick32Get(void) {
	DWORD_VAL ret;
	ret.word.LW = TMR2;
	ret.word.HW = TMR3HLD;
	return ret.Val;
}
#else
DWORD utick32Get(void);
#endif	//#if (NZSYS_TICK_TMR2AND3_UTICK==1)



/**
 * Same as utick32Get(), but does not disable interrupts.
 * Only call this function from an ISR with higher priority than the system tick.
 * That is an ISR with priority 5 - 7, for default system tick with priority 4.
 * @return The current 32-bit, Micro Tick.
 */
DWORD __INLINE_FUNCTION__ utick32Get_noDisi(void) {
#if (NZSYS_TICK_TMR2AND3_UTICK==1)
	DWORD_VAL ret;
	ret.word.LW = TMR2;
	ret.word.HW = TMR3HLD;
	return ret.Val;
#else
    WORD  tmrCpy;
    tmrCpy  = TMR1;

    //No tick ISR got triggered while we disabled interrupts. This means TMR1 did
    //NOT roll over!
	if (_T1IF == 0) {
        return (DWORD)( ((DWORD)(tick_val.Val*NZ_UTICKS_PER_MS)) + tmrCpy);
    }
    //ELSE
    //The TMR1 IF got triggered recently, and TMR1 ISR has not been executed yet! This
	//means that TMR1 can be = PR value, or reset to 0. It also means that tick_val was NOT
	//incremented yet, and will be incremented once TMR ISR is entered.

    #if (NZ_UTICKS_PER_MS==2000)
		//T1IF is triggered when TRM1=PR=1999. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1999 to 1-2000
		if (tmrCpy==2000) tmrCpy=0;	//Convert from 1-2000 to 0-1999

		return (DWORD)( ((DWORD)((tick_val.Val+1)*NZ_UTICKS_PER_MS)) + tmrCpy);
	#else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
#endif	//#if (NZSYS_TICK_TMR2AND3_UTICK==1)
}


/**
 * @function BOOL utick16TestTmr(TICK16 tmr)
 *
 * Tests if the current 16-bit utick(micro tick) has passed the given timer's value.
 *
 * Will return true once the 16 bit utick is > given value.
 * Can have a maximum value of 32,768
 *
 * @param tmr   Timer to test
 *
 * @return      Returns TRUE if given timer has expired, else FALSE
 */
#define utick16TestTmr(tmr)      ( (((((tmr) - (utick16Get()) ) & 0x8000))==0) ? 0 : 1)


/**
 * @function TICK16 utick16GetElapsedUS(TICK16 oldTick)
 *
 * Returns the time in microseconds that has elapsed since the given old tick value.
 * Seeing that this is a 16-bit function, maximum value of difference in uticks
 * is 32,768. For the default SBC66 boards there are 2 ticks per US (2000 ticks per
 * millisecond), meaning maximum elapsed time that can be measured is 32,768/2 = 16,384
 *
 * For example, to calculate how long some code takes:
 * @code
 *  TICK16 utickStart;
 *  utickStart = utick16Get();
 *
 *  //Do something that takes time....
 *
 *  //Print the time taken to the debug console
 *  debugPutString("\nTime taken in us = ");
 *  debugPutWord(utick16GetElapsedUS(utickStart));
 * @endcode
 *
 * @param oldTick   Old TICK16 value
 *
 * @return Returns elapsed time in microseconds
 */
#define utick16GetElapsedUS(oldTick) utick16ConvertToUS(((WORD)((utick16Get()) - (oldTick))))


/**
 * @function WORD utick16ConvertToUS(WORD utickVal)
 *
 * Convert given utick value to micro seconds.
 *
 * @param utickVal Value to convert to micro seconds
 */
#define utick16ConvertToUS(utickVal) ( ((WORD)utickVal) / ((WORD)(NZ_UTICKS_PER_MS/1000)) )


/**
 * @function WORD utick16ConvertFromUS(WORD usVal)
 *
 * Converts the given micro second value to system "16-bit Micro Tick".
 *
 * @param usVal The micro second value
 *
 * @return The number of system ticks for the given micro tick value
 */
#define utick16ConvertFromUS(usVal) (WORD)( ((WORD)usVal) * (NZ_UTICKS_PER_MS/1000) )


/**
 * @function BOOL utick32TestTmr(TICK32 tmr)
 *
 * Tests if the current 32-bit utick(micro tick) has passed the given timer's value.
 *
 * For efficiency and code size, try to use utick16 functions where possible
 * (utick16TestTmr() for example).
 *
 * Will return true once the 32 bit tick is > given value.
 * Can have a maximum value of 2,147,483,648
 *
 * @param tmr   Timer to test
 *
 * @return      Returns TRUE if given timer has expired, else FALSE
 */
#define utick32TestTmr(tmr)      ((((tmr-utick32Get())&0x80000000)==0) ? 0 : 1)


/**
 * @function DWORD utick32ConvertToUS(DWORD utickVal)
 *
 * Convert given utick value to micro seconds.
 *
 * @param utickVal Value to convert to micro seconds
 */
#define utick32ConvertToUS(utickVal) ( ((DWORD)utickVal) / ((DWORD)(NZ_UTICKS_PER_MS/1000)) )


/**
 * @function DWORD utick32ConvertFromUS(DWORD usVal)
 *
 * Converts the given micro second value to system "32-bit Micro Tick".
 *
 * @param usVal The micro second value
 *
 * @return The number of system ticks for the given micro tick value
 */
#define utick32ConvertFromUS(usVal) (DWORD)( ((DWORD)usVal) * (NZ_UTICKS_PER_MS/1000) )



///////////////////////////////////////////////////////////////////////////////
// 8us tick functions
///////////////////////////////////////////////////////////////////////////////


/**
 * Get the current 16 bit, 8us tick. Has a resolution of 8us.
 * Has a maximum value of 262,144us = 262.1 milliseconds
 *
 * @return The current 16-bit, 8us Tick.
 */
#if (NZSYS_TICK_TMR2AND3_UTICK==1)
WORD __INLINE_FUNCTION__ tick16Get_8us(void) {
	DWORD_VAL dw;
	dw.word.LW = TMR2;
	dw.word.HW = TMR3HLD;
    #if (NZ_UTICKS_PER_MS==2000)
		return ((WORD)(dw.Val/16));
    #else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
}
#else
WORD tick16Get_8us(void);
#endif


/**
 * Same as tick16Get_8us(), but does NOT disable interrupts!
 * Only call this function from an ISR with higher priority than the system tick.
 * That is an ISR with priority 5 - 7, for default system tick with priority 4.
 * @return The current 16-bit, 8us Tick.
 */
WORD __INLINE_FUNCTION__ tick16Get_8us_noDisi(void) {
#if (NZSYS_TICK_TMR2AND3_UTICK==1)
	DWORD_VAL dw;
	dw.word.LW = TMR2;
	dw.word.HW = TMR3HLD;
    #if (NZ_UTICKS_PER_MS==2000)
		return ((WORD)(dw.Val/16));
    #else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
#else
    WORD tmrCpy;
#if defined(__C30__)
    tmrCpy = TMR1;
    if (_T1IF == 0) {
		//This function is ONLY called from and ISR with higher priority than
		//the TMR1 interrupt, so not TMR1 interrupt will occur while processing!
		//At this stage we know that tmrCpy did NOT reset to 0, use it!
	    #if (NZ_UTICKS_PER_MS==2000)
			/* Convert 1ms resolution to 8us = x * (1000/8) = x * 125 */
			/* Convert 500ns to 8us */
			return ((tick_val.word.LW*125) + (tmrCpy/16));
		#else
			#error "NZ_UTICKS_PER_MS not supported!"
		#endif
    }
    //ELSE
    //The TMR1 IF got triggered recently, and TMR1 ISR has not been executed yet! This
	//means that TMR1 can be = PR value, or reset to 0. It also means that tick_val was NOT
	//incremented yet, and will be incremented once TMR ISR is entered.

    #if (NZ_UTICKS_PER_MS==2000)
		//T1IF is triggered when TRM1=PR=1999. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1999 to 1-2000
		if (tmrCpy==2000) tmrCpy=0;	//Convert from 1-2000 to 0-1999

		return (( ((WORD)(tick_val.word.LW+1))*125) + (tmrCpy/16));
	#else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
#elif defined(__PIC32MX__)
    return 0;
#endif
#endif	//#if (NZSYS_TICK_TMR2AND3_UTICK==1)
}



/**
 * Get the current 32 bit, 8us tick. Has a resolution of 8us.
 * Has a maximum value of 17,179 sec = 286 Minutes = 4.77 Hours
 *
 * @return The current 32-bit, 8us Tick.
 */
#if (NZSYS_TICK_TMR2AND3_UTICK==1)
WORD __INLINE_FUNCTION__ tick32Get_8us(void) {
	DWORD_VAL dw;
	dw.word.LW = TMR2;
	dw.word.HW = TMR3HLD;
    #if (NZ_UTICKS_PER_MS==2000)
		return (dw.Val/16);
    #else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
}
#else
WORD tick32Get_8us(void);
#endif


/**
 * Get the current 32 bit, 8us tick. Has a resolution of 8us.
 * Has a maximum value of 17,179 sec = 286 Hours = 11.9 Days
 * Only call this function from an ISR with higher priority than the system tick.
 * That is an ISR with priority 5 - 7, for default system tick with priority 4.
 *
 * @return The current 32-bit, 8us Tick.
 */
WORD __INLINE_FUNCTION__ tick32Get_8us_noDisi(void) {
#if (NZSYS_TICK_TMR2AND3_UTICK==1)
	DWORD_VAL dw;
	dw.word.LW = TMR2;
	dw.word.HW = TMR3HLD;
    #if (NZ_UTICKS_PER_MS==2000)
		return (dw.Val/16);
    #else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
#else
    WORD  tmrCpy;
#if defined(__C30__)
    tmrCpy  = TMR1;
    //No tick ISR got triggered while we disabled interrupts. This means TMR1 did
    //NOT roll over! Convert 1ms resolution to 8us = x * (1000/8) = x * 125
    if (_T1IF == 0) {
		#if (NZ_UTICKS_PER_MS==2000)
			return (DWORD)( ((DWORD)(tick_val.Val*125)) + tmrCpy/16);
		#else
			#error "NZ_UTICKS_PER_MS not supported!"
		#endif
    }
    //ELSE
    //The TMR1 IF got triggered recently, and TMR1 ISR has not been executed yet! This
	//means that TMR1 can be = PR value, or reset to 0. It also means that tick_val was NOT
	//incremented yet, and will be incremented once TMR ISR is entered.

    #if (NZ_UTICKS_PER_MS==2000)
		//T1IF is triggered when TRM1=PR=1999. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1999 to 1-2000
		if (tmrCpy==2000) tmrCpy=0;	//Convert from 1-2000 to 0-1999

		return ( ((DWORD)((tick_val.Val+1)*125)) + (tmrCpy/16));
	#else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
#elif defined(__PIC32MX__)
#endif
#endif	//#if (NZSYS_TICK_TMR2AND3_UTICK==1)
}

#endif  //end of #define _NZDEFAULT_TICK_H_
