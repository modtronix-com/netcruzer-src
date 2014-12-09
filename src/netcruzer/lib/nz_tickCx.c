/**
 * @brief           System Tick
 * @file            nz_tick.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
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
 **********************************************************************
 * File History
 *
 * 2012-08-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_TICK_CX_C

#define INCLUDE_NETCRUZER_HEADERS
#include "HardwareProfile.h"

#if defined(THIS_IS_NZ_NETCRUZER_C)     //This file is included from netcruzer.c
#if !defined(NZSYS_DONT_MANAGE_TICK) && defined(NZSYS_ENABLE_COMPLEX_TICK)

#include "nz_tickCx.h"
#include "GenericTypeDefs.h"
#include "nz_interrupt.h"
//#include "nzos_main.h"


/////////////////////////////////////////////////
// Defines

#define NZ_TICK_INT_SAVE_CPU_IPL	(0)

//Note on disabling interrupts! When using a method that does NOT save current IPL, and restoring
//it to all on, any nested calls disabling interrupts will NOT work! Rather use NZ_INT_DIS_PUSH() and
//NZ_INT_EN_POP, seeing that it saves and restores IPL
#define NZ_TICK_INT_DIS(ipl,lvl)	NZ_INT_DIS_PUSH()
//#define NZ_TICK_INT_DIS(ipl,lvl)	NZ_INT_DISABLE_P07();			// Disable interrupts, excluding level 7
//#define NZ_TICK_INT_DIS(ipl,lvl)	NZ_SET_AND_SAVE_CPU_IPL(ipl,lvl);

#define NZ_TICK_INT_EN(ipl)			NZ_INT_EN_POP()
//#define NZ_TICK_INT_EN(ipl)		NZ_INT_ENABLE_P07()
//#define NZ_TICK_INT_EN(ipl)		NZ_RESTORE_CPU_IPL(ipl)

//Disable this module from:
// - Creating tick variable
// - Managing interrupts
// - Disable tickInit() functions
//All other standard nz_tick.c functions are still available!
#if !defined(NZSYS_TICK_DISABLE_VARS_INT_AND_INIT)

/////////////////////////////////////////////////
// Global variables.
volatile DWORD_VAL tick_val = {0};  //32-bit System Tick. MUST BE volatile, because is changed by ISR!


/////////////////////////////////////////////////
// Function prototypes

/**
 * Timer 1 ISR, called every 1ms
 */
#if defined(__C30__)
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void)
#elif defined(__PIC32MX__)
void __attribute((interrupt(ipl4), vector(_TIMER_1_VECTOR), nomips16)) _T1Interrupt(void)
#endif
{
	#if (NZ_TICK_INT_SAVE_CPU_IPL==1)
	BYTE saved_ipl;
	#endif

	//Disable all interrupts when modifying _T1IF and tickService().
	//Is required seeing that some functions test _T1IF when getting tick data
	NZ_TICK_INT_DIS(saved_ipl,7);

    #if defined(__C30__)
    _T1IF = 0;                  //Clear interrupt flag
    #elif defined(__PIC32MX__)
	//IFS0bits.T1IF = 0;
	IFS0CLR = _IFS0_T1IF_MASK;  //Clear interrupt flag
    #endif

    // This ISR takes 9 instruction cycles = 9 x 62.5ns = 0.5625us
    // Seems like 3 cycles used for interrupt call, and 4 cycles for interrupt return = 7 cycles
    // Total ISR time = 3 + 9 + 4 = 16 instruction cycles = 1us
    tickService();  //Call every 1ms

	NZ_TICK_INT_EN(saved_ipl);	//Enable interrupts again
	
    //Netcruzer RTOS tick function
    #if (nzosENABLE==1)
    nzRtosTick();
    #endif
}

/**
 * Initializes tick values
 */
void tickInit() {
    tick_val.Val = 0;
#if defined(__C30__)
    #if (NZ_UTICKS_PER_MS==2000)
        /////////////////////////////////////////////////
        // Use Timer 1 for 1ms system tick module, with 8 prescaler
        //This configures timer to increment 16MHz/8 = 500ns
        //With PR1 = 2,000, Timer 1 ISR is called each 500ns x 2,000 = 1ms
        //xxxx xxxx xxxx xx0x - Clock source = Fosc/2
        //xxxx xxxx xx01 xxxx - Input clock prescaler = 8
        //xxxx xxxx x0xx xxxx - Gated time accumulation disabled
        T1CON = 0x0010;
        PR1 = 1999;                     //Timer interrupts every 1ms
        TMR1 = 0;                       //Clear counter
        _T1IP = nzINT_PRIORITY_TICK;    //Interrupt priority
        _T1IF = 0;
        _T1IE = 1;
        T1CONbits.TON = 1;          //Start timer
    #else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
#elif defined(__PIC32MX__)
	// Enable timer interrupt
	IPC1bits.T1IP = 2;	// Interrupt priority 4
	IFS0CLR = _IFS0_T1IF_MASK;
	IEC0SET = _IEC0_T1IE_MASK;
#endif
}
#endif //#if !defined(NZSYS_TICK_DISABLE_VARS_INT_AND_INIT)


/**
 * Get the current 32-bit, 1ms Tick.
 *
 * @return The current 32-bit, 1ms Tick.
 */
DWORD tick32Get(void) {
	#if (NZ_TICK_INT_SAVE_CPU_IPL==1)
	BYTE saved_ipl;
	#endif
    DWORD dw;

    //Critical Section to begin
    NZ_TICK_INT_DIS(saved_ipl,7);
    dw = tick_val.Val;
    NZ_TICK_INT_EN(saved_ipl);	//Enable interrupts again

    return dw;
}


/**
 * Get the current 16-bit, 256ms Tick. Has a maximum value
 * 8,338 seconds = 2.3 Hours
 *
 * @return The current 16-bit, 256ms Tick.
 */
WORD tick16Get_256ms(void)
{
    //BYTE saved_ipl;
    WORD w;

    NZ_BUILTIN_DISI(0x3FFF);                 //Disable all interrupt, excluding level 7
    //SET_AND_SAVE_CPU_IPL(saved_ipl,7);    //Disable all interrupt, including level 7

    w = ((WORD)(tick_val.Val<<8));

    NZ_BUILTIN_DISI(0x0000);                 //Enable interrupts
    //RESTORE_CPU_IPL(saved_ipl);           //Restore IPL interrupt level

    return w;
}

/**
 * Get the current 8-bit, 16ms Tick. Has a maximum value of 2,048ms = 2 seconds
 *
 * @return The current 8-bit, 16ms Tick.
 */
BYTE tick8Get_16ms(void)
{
    //BYTE saved_ipl;
    BYTE b;

    NZ_BUILTIN_DISI(0x3FFF);                 //Disable all interrupt, excluding level 7
    //SET_AND_SAVE_CPU_IPL(saved_ipl,7);    //Disable all interrupt, including level 7

    b = ((BYTE)(tick_val.w[0]>>4));

    NZ_BUILTIN_DISI(0x0000);                 //Enable interrupts
    //RESTORE_CPU_IPL(saved_ipl);           //Restore IPL interrupt level

    return b;
}


/**
 * Will return true once the "8 bit tick" is > given value
 *
 * @param Given value to compare it to
 */
BOOL tick32TestTmr(DWORD v)
{
	#if (NZ_TICK_INT_SAVE_CPU_IPL==1)
	BYTE saved_ipl;
	#endif
    DWORD dw;

    //Critical Section to begin
    NZ_TICK_INT_DIS(saved_ipl,7);
    dw = tick_val.Val;
	NZ_TICK_INT_EN(saved_ipl);	//Enable interrupts again

    return ((((v-dw)&0x80000000)==0) ? 0 : 1);
}


/**
 * Will return true once the 16 bit(256ms) tick is > given value. Tick is a 256ms counter.
 * Can have a maximum value of 8,338 seconds = 2.3 Hours
 *
 * @param v Given value to compare it
 */
BOOL tick16HasExpired_256ms(WORD v)
{
    //BYTE saved_ipl;
    WORD w;

    NZ_BUILTIN_DISI(0x3FFF);                 //Disable all interrupt, excluding level 7
    //SET_AND_SAVE_CPU_IPL(saved_ipl,7);    //Disable all interrupt, including level 7

    w = tick_val.Val << 8;

    NZ_BUILTIN_DISI(0x0000);                 //Enable interrupts
    //RESTORE_CPU_IPL(saved_ipl);           //Restore IPL interrupt level

    return ((v - w) & 0x8000);
}


/**
 * Get the current 16 bit, micro tick. The NZ_UTICKS_PER_MS defines how many micro ticks
 * there are per milli second. For current range of SBC66 boards it is always 2000 (2 per us).
 * 
 * For NZ_UTICKS_PER_MS=1000, Has a maximum value of 32,768us = 32.7 milliseconds
 * For NZ_UTICKS_PER_MS=2000, Has a maximum value of 16,384us = 16 milliseconds
 *
 * @return The current 16-bit, Micro Tick.
 */
WORD utick16Get(void) {
	#if (NZ_TICK_INT_SAVE_CPU_IPL==1)
	BYTE saved_ipl;
	#endif
    WORD tmrCpy;
    WORD tickCpy;
    WORD ifs0Cpy;

#if defined(__C30__)
    //Thread save, get copies, and only use them in this function.
    NZ_TICK_INT_DIS(saved_ipl,7);	// Disable interrupts
    tickCpy = tick_val.word.LW;
    tmrCpy  = TMR1;
	//Read IF last!! This ensures we know if an interrupt occurred while reading tickCpy and tmrCpy.
	ifs0Cpy = IFS0;
    NZ_TICK_INT_EN(saved_ipl);		//Enable interrupts again
#elif defined(__PIC32MX__)
    
#endif

    //No tick ISR got triggered while we disabled interrupts. This means TMR1 did
    //NOT roll over!
    if ((ifs0Cpy & _IFS0_T1IF_MASK)==0) {
        return ((tickCpy*NZ_UTICKS_PER_MS) + tmrCpy);
    }
    //ELSE
    //The timer ISR got triggered while we were reading the copy values. This means
    //that tmrCpy can either be before or after the rollover. IGNORE tmrCpy, and use
	//TMR1! It also means that tickCpy was NOT incremented yet, and will be incremented
	//right after the interrups were enabled again (ONLY if not higher interrupt level
	//ISR called this function).

    return (((tickCpy+1)*NZ_UTICKS_PER_MS) + TMR1);
}


/**
 * Get the current 32 bit, micro tick. The NZ_UTICKS_PER_MS defines how many micro ticks
 * there are per milli second. For current range of SBC66 boards it is always 2000.
 *
 * For NZ_UTICKS_PER_MS=1000, Has a maximum value of 2,147 Seconds = 35.7 Minutes
 * For NZ_UTICKS_PER_MS=2000, Has a maximum value of 1,073 Seconds = 17.9 Minutes
 *
 * @return The current 32-bit, Micro Tick.
 */
DWORD utick32Get(void) {
	#if (NZ_TICK_INT_SAVE_CPU_IPL==1)
	BYTE saved_ipl;
	#endif
    WORD  tmrCpy;
    DWORD tickCpy;
	WORD  ifs0Cpy;

#if defined(__C30__)
    //Thread save, get copies, and only use them in this function.
    NZ_TICK_INT_DIS(saved_ipl,7);	// Disable interrupts
    tickCpy = tick_val.Val;
    tmrCpy  = TMR1;
	ifs0Cpy = IFS0;
	NZ_TICK_INT_EN(saved_ipl);	//Enable interrupts again
#elif defined(__PIC32MX__)

#endif
    //No tick ISR got triggered while we disabled interrupts. This means TMR1 did
    //NOT roll over!
	if ((ifs0Cpy & _IFS0_T1IF_MASK)==0) {
        return (DWORD)( ((DWORD)(tickCpy*NZ_UTICKS_PER_MS)) + tmrCpy);
    }
    //ELSE
    //The timer ISR got triggered while we were reading the copy values. This means
    //that tmrCpy can either be before or after the rollover. IGNORE tmrCpy, and use
	//TMR1! It also means that tickCpy was NOT incremented yet, and will be incremented
	//right after the interrups were enabled again (ONLY if not higher interrupt level
	//ISR called this function).

    return (DWORD)( ((DWORD)((tickCpy+1)*NZ_UTICKS_PER_MS)) + TMR1);
}


/**
 * Get the current 16 bit, 8us tick. Has a resolution of 8us.
 * Has a maximum value of 262,144us = 262.1 milliseconds
 *
 * @return The current 16-bit, 8us Tick.
 */
WORD tick16Get_8us(void) {
	#if (NZ_TICK_INT_SAVE_CPU_IPL==1)
	BYTE saved_ipl;
	#endif
    WORD  tmrCpy;
    WORD  tickCpy;
    WORD  ifs0Cpy;

#if defined(__C30__)
	NZ_TICK_INT_DIS(saved_ipl,7);	// Disable interrupts
    tickCpy = tick_val.word.LW;
    tmrCpy  = TMR1;
	//Read IF last!! This ensures we know if an interrupt occurred while reading tickCpy and tmrCpy.
	ifs0Cpy = IFS0;
    NZ_TICK_INT_EN(saved_ipl);	//Enable interrupts again
#elif defined(__PIC32MX__)

#endif

    //No tick ISR got triggered while we disabled interrupts. This means TMR1 did
    //NOT roll over! Convert 1ms resolution to 8us = x * (1000/8) = x * 125
    if ((ifs0Cpy & _IFS0_T1IF_MASK)==0) {
#if (NZ_UTICKS_PER_MS==2000)
        return ((tickCpy*125) + tmrCpy/16);
#elif (NZ_UTICKS_PER_MS==1250)
		//For NZ_UTICKS_PER_MS=1250, it is (tmrCpy/10)
		return ((tickCpy*125) + tmrCpy/((NZ_UTICKS_PER_MS/1000)*8));
#else
    #error "NZ_UTICKS_PER_MS not supported!"
#endif
    }
    //ELSE
    //The timer ISR got triggered while we were reading the copy values. This means
    //that tmrCpy can either be before or after the rollover. IGNORE tmrCpy, and use
	//TMR1! It also means that tickCpy was NOT incremented yet, and will be incremented
	//right after the interrups were enabled again (ONLY if not higher interrupt level
	//ISR called this function).
#if (NZ_UTICKS_PER_MS==2000)
	return ( ((tickCpy+1)*125) + (TMR1/16));
#elif (NZ_UTICKS_PER_MS==1250)
	//For NZ_UTICKS_PER_MS=1250, it is (TMR1/10)
	return ( ((tickCpy+1)*125) + (TMR1/((NZ_UTICKS_PER_MS/1000)*8)));
#else
    #error "NZ_UTICKS_PER_MS not supported!"
#endif
}


/**
 * Get the current 32 bit, 8us tick. Has a resolution of 8us.
 * Has a maximum value of 17,179 sec = 286 Hours = 11.9 Days
 *
 * @return The current 32-bit, 8us Tick.
 */
WORD tick32Get_8us(void) {
	#if (NZ_TICK_INT_SAVE_CPU_IPL==1)
	BYTE saved_ipl;
	#endif
    WORD  tmrCpy;
    DWORD tickCpy;
    WORD  ifs0Cpy;

#if defined(__C30__)
    //Thread save, get copies, and only use them in this function.
    NZ_TICK_INT_DIS(saved_ipl,7);	// Disable interrupts
    tickCpy = tick_val.Val;
    tmrCpy  = TMR1;
	//Read IF last!! This ensures we know if an interrupt occurred while reading tickCpy and tmrCpy.
	ifs0Cpy = IFS0;
    NZ_TICK_INT_EN(saved_ipl);	//Enable interrupts again
#elif defined(__PIC32MX__)

#endif

    //No tick ISR got triggered while we disabled interrupts. This means TMR1 did
    //NOT roll over! Convert 1ms resolution to 8us = x * (1000/8) = x * 125
    if ((ifs0Cpy & _IFS0_T1IF_MASK)==0) {
#if (NZ_UTICKS_PER_MS==2000)
	return (DWORD)( ((DWORD)(tickCpy*125)) + tmrCpy/16);
#elif (NZ_UTICKS_PER_MS==1250)
	//For NZ_UTICKS_PER_MS=1250, it is (TMR1/10)
	return (DWORD)( ((DWORD)(tickCpy*125)) + tmrCpy/((NZ_UTICKS_PER_MS/1000)*8));
#else
    #error "NZ_UTICKS_PER_MS not supported!"
#endif

    }
    //ELSE
    //The timer ISR got triggered while we were reading the copy values. This means
    //that tmrCpy can either be before or after the rollover. IGNORE tmrCpy, and use
	//TMR1! It also means that tickCpy was NOT incremented yet, and will be incremented
	//right after the interrups were enabled again (ONLY if not higher interrupt level
	//ISR called this function).
#if (NZ_UTICKS_PER_MS==2000)
	return ( ((DWORD)((tickCpy+1)*125)) + (TMR1/16));
#elif (NZ_UTICKS_PER_MS==1250)
	//For NZ_UTICKS_PER_MS=1250, it is (TMR1/10)
	return ( ((DWORD)((tickCpy+1)*125)) + (TMR1/((NZ_UTICKS_PER_MS/1000)*8)));
#else
    #error "NZ_UTICKS_PER_MS not supported!"
#endif

}

#endif  //#if !defined(NZSYS_DONT_MANAGE_TICK) && defined(NZSYS_ENABLE_COMPLEX_TICK)
#endif  //#if defined(THIS_IS_NZ_NETCRUZER_C)