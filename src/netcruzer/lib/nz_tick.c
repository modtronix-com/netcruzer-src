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
#define THIS_IS_NZ_TICK_C

#define INCLUDE_NETCRUZER_HEADERS
#include "HardwareProfile.h"

#if defined(THIS_IS_NZ_NETCRUZER_C)     //This file is included from netcruzer.c
#if !defined(NZSYS_DONT_MANAGE_TICK) && !defined(NZSYS_ENABLE_COMPLEX_TICK)

#include "nz_tick.h"
#include "GenericTypeDefs.h"
#include "nz_interrupt.h"


/////////////////////////////////////////////////
// Defines
#define NZ_TICK_INT_SAVE_CPU_IPL	(1)

//Note on disabling interrupts! When using a method that does NOT save current IPL, and restoring
//it to all on, any nested calls disabling interrupts will NOT work! Rather use NZ_INT_DIS_PUSH() and
//NZ_INT_EN_POP, seeing that it saves and restores IPL
//#define NZ_TICK_INT_DIS(ipl,lvl)
//#define NZ_TICK_INT_DIS(ipl,lvl)	NZ_INT_DIS_PUSH()
#define NZ_TICK_INT_DIS(ipl,lvl)	NZ_INT_DIS_SAVE(ipl)
//#define NZ_TICK_INT_DIS(ipl,lvl)	NZ_INT_DISABLE_P07();			// Disable interrupts, excluding level 7
//#define NZ_TICK_INT_DIS(ipl,lvl)	NZ_SET_AND_SAVE_CPU_IPL(ipl,lvl);

//#define NZ_TICK_INT_EN(ipl)
//#define NZ_TICK_INT_EN(ipl)		NZ_INT_EN_POP()
#define NZ_TICK_INT_EN(ipl)			NZ_INT_EN_SAVE(ipl);
//#define NZ_TICK_INT_EN(ipl)		NZ_INT_ENABLE_P07()
//#define NZ_TICK_INT_EN(ipl)		NZ_RESTORE_CPU_IPL(ipl)


#if (NZSYS_TICK_TMR2AND3_UTICK==1)
/**
 * Timer 3 ISR, 32-bit combination of TMR2 and TMR3.
 * Current NOT USED (_T3IE is initilized to 0 below)! Only for testing!
 */
#if defined(__C30__)
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt (void)
#elif defined(__PIC32MX__)
void __attribute((interrupt(ipl4), vector(_TIMER_3_VECTOR), nomips16)) _T3Interrupt(void)
#endif
{
	#if (NZ_TICK_INT_SAVE_CPU_IPL==1)
	BYTE saved_ipl;
	#endif

	//Disable all interrupts when modifying _T3IF and tickService().
	//Is required seeing that some functions test _T3IF when getting tick data
	NZ_TICK_INT_DIS(saved_ipl,7);

	#if defined(__C30__)
    _T3IF = 0;                  //Clear interrupt flag
    #elif defined(__PIC32MX__)
	//IFS??bits.T3IF = 0;
	IFS??CLR = _IFS0_T3IF_MASK;  //Clear interrupt flag
    #endif

	NZ_TICK_INT_EN(saved_ipl);	//Enable interrupts again
}
#endif	//#if (NZSYS_TICK_TMR2AND3_UTICK==1)


//Disable this module from:
// - Creating tick variable
// - Managing interrupts
// - Disable tickInit() functions
//All other standard nz_tick.c functions are still available!
#if !defined(NZSYS_TICK_DISABLE_VARS_INT_AND_INIT)
// Global variables.
volatile DWORD_VAL tick_val = {0};  //32-bit System Tick. MUST BE volatile, because is changed by ISR!.

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
        //With PR1 = 1,999, Timer 1 ISR is called each 500ns x 2,000 = 1ms
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

		#if (NZSYS_TICK_TMR2AND3_UTICK==1)
			/////////////////////////////////////////////////
			// Use Timer 2 and 3 for a dedicated uTick counter.
			T3CON = 0;		//Stops any 16-bit Timer3 operation
			PR3 = 0xFFFF;	//Load the Period register3 with the value 0xFFFF
			PR2 = 0xFFFF;	//Load the Period register2 with the value 0xFFFF

			//This configures timer to increment 16MHz/8 = 500ns
			//xxxx xxxx xxxx xx0x - Clock source = Fosc/2
			//xxxx xxxx xxxx 1x0x - 32-bit Timer mode = T32=1
			//xxxx xxxx xx01 xxxx - Input clock prescaler = 8
			//xxxx xxxx x0xx xxxx - Gated time accumulation disabled
			T2CON = 0x0018;
			TMR3 = 0;						//Clear contents of the timer3 register
			TMR2 = 0;						//Clear contents of the timer2 register
			_T2IE = 0;						//Disable TMR2 Interrupt
			_T3IP = nzINT_PRIORITY_UTICK;   //Interrupt priority
			_T3IF = 0;						//Disable TMR3 Interrupt
			_T3IE = 0;
			T2CONbits.TON = 1;				//Start timer
		#endif		//#if (NZSYS_TICK_TMR2AND3_UTICK==1)
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
 * Will return true once the 32 bit (1ms) tick is > given value. For efficiency and code
 * size, try to use tick16 function where possible (tick16TestTmr() for example).
 * Tick is a 1ms counter. Can have a maximum value of:
 * 4,294,967,296 ms = 1,193 hours = 49.7 Days
 *
 * @param v Given value to compare it
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
 * Get the current 16 bit, micro tick. The NZ_UTICKS_PER_MS defines how many micro ticks
 * there are per milli second. For current range of SBC66 boards it is always 2000 (2 per us).
 * 
 * For NZ_UTICKS_PER_MS=1000, Has a maximum value of 32,768us = 32.7 milliseconds
 * For NZ_UTICKS_PER_MS=2000, Has a maximum value of 16,384us = 16 milliseconds
 *
 * @return The current 16-bit, Micro Tick.
 */
#if (NZSYS_TICK_TMR2AND3_UTICK==0)
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
    //The TMR1 IF got triggered recently, and TMR1 ISR has not been executed yet! This
	//means that TMR1 can be = PR value, or reset to 0. It also means that tick_val was NOT
	//incremented yet, and will be incremented once TMR ISR is entered.

    #if (NZ_UTICKS_PER_MS==2000)
		//T1IF is triggered when TRM1=PR=1999. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1999 to 1-2000
		if (tmrCpy==2000) tmrCpy=0;	//Convert from 1-2000 to 0-1999

		return (((tickCpy+1)*NZ_UTICKS_PER_MS) + tmrCpy);
	#else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
}
#endif	//#if (NZSYS_TICK_TMR2AND3_UTICK==0)


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
    //The TMR1 IF got triggered recently, and TMR1 ISR has not been executed yet! This
	//means that TMR1 can be = PR value, or reset to 0. It also means that tick_val was NOT
	//incremented yet, and will be incremented once TMR ISR is entered.

    #if (NZ_UTICKS_PER_MS==2000)
		//T1IF is triggered when TRM1=PR=1999. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1999 to 1-2000
		if (tmrCpy==2000) tmrCpy=0;	//Convert from 1-2000 to 0-1999

		return (DWORD)( ((DWORD)((tickCpy+1)*NZ_UTICKS_PER_MS)) + tmrCpy);
	#else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
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
    //The TMR1 IF got triggered recently, and TMR1 ISR has not been executed yet! This
	//means that TMR1 can be = PR value, or reset to 0. It also means that tick_val was NOT
	//incremented yet, and will be incremented once TMR ISR is entered.

    #if (NZ_UTICKS_PER_MS==2000)
		//T1IF is triggered when TRM1=PR=1999. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1999 to 1-2000
		if (tmrCpy==2000) tmrCpy=0;	//Convert from 1-2000 to 0-1999

		return ( ((tickCpy+1)*125) + (tmrCpy/16));
	#elif (NZ_UTICKS_PER_MS==1250)
		//T1IF is triggered when TRM1=PR=1249. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1249 to 1-1250
		if (tmrCpy==1250) tmrCpy=0;	//Convert from 1-1250 to 0-1249

		//For NZ_UTICKS_PER_MS=1250, it is (TMR1/10)
		return ( ((tickCpy+1)*125) + (tmrCpy/((NZ_UTICKS_PER_MS/1000)*8)));
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
    //The TMR1 IF got triggered recently, and TMR1 ISR has not been executed yet! This
	//means that TMR1 can be = PR value, or reset to 0. It also means that tick_val was NOT
	//incremented yet, and will be incremented once TMR ISR is entered.

    #if (NZ_UTICKS_PER_MS==2000)
		//T1IF is triggered when TRM1=PR=1999. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1999 to 1-2000
		if (tmrCpy==2000) tmrCpy=0;	//Convert from 1-2000 to 0-1999

		return ( ((DWORD)((tickCpy+1)*125)) + (tmrCpy/16));
	#elif (NZ_UTICKS_PER_MS==1250)
		//T1IF is triggered when TRM1=PR=1249. It will only reset to 0 at next TMR1 clock!
		tmrCpy = TMR1+1;			//Convert from 0-1249 to 1-1250
		if (tmrCpy==1250) tmrCpy=0;	//Convert from 1-1250 to 0-1249

		//For NZ_UTICKS_PER_MS=1250, it is (TMR1/10)
		return ( ((DWORD)((tickCpy+1)*125)) + (tmrCpy/((NZ_UTICKS_PER_MS/1000)*8)));
	#else
        #error "NZ_UTICKS_PER_MS not supported!"
    #endif
}

#endif  //#if !defined(NZSYS_DONT_MANAGE_TICK) && !defined(NZSYS_ENABLE_COMPLEX_TICK)
#endif  //#if defined(THIS_IS_NZ_NETCRUZER_C)