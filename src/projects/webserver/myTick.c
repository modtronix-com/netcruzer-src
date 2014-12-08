/**
 * @brief           System Tick, combination of Netcruzer and Microchip Tick module
 * @file            myTick.c
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
#define THIS_IS_MY_TICK_C

#define INCLUDE_NETCRUZER_HEADERS
#include "HardwareProfile.h"

#include "nz_tick.h"
#include "nz_interrupt.h"
#include "nzos_main.h"
#include "myTick.h"

/////////////////////////////////////////////////
// Defines


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


/////////////////////////////////////////////////
// Global variables.
volatile DWORD_VAL tick_val = {0};  //32-bit System Tick. MUST BE volatile, because is changed by ISR!

//From Microchip TCP/IP stack 6-byte value to store Ticks.  Allows for use over longer periods of time.
static volatile BYTE tickMicrochip[6];


/////////////////////////////////////////////////
// Function prototypes

/**
 * Timer 1 ISR, called every 1ms
 */
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void)
{
	//Disable all interrupts when modifying _T1IF and tickService().
	//Is required seeing that some functions test _T1IF when getting tick data
	NZ_INT_DISABLE_P07();			// Disable interrupts, excluding level 7

    #if defined(__C30__)
    _T1IF = 0;                  //Clear interrupt flag
    #elif defined(__PIC32MX__)
	IFS0CLR = _IFS0_T1IF_MASK;  //Clear interrupt flag
    #endif

    // This ISR takes ? instruction cycles = ? x 62.5ns = ?us
    // Seems like 3 cycles used for interrupt call, and 4 cycles for interrupt return = 7 cycles
    // Total ISR time = 3 + ? + 4 = ? instruction cycles = ?us
    tickService();  //Call every 1ms, increments tick_val

	NZ_INT_ENABLE_P07();			//Enable interrupts again

    //Netcruzer RTOS tick function
    #if (nzosENABLE==1)
    nzRtosTick();
    #endif

    //Execute every 4th IRS = 4ms, and increment bytes 1-5, not 0. Seeing that we skip the LSB, and only
    //increment every 4ms, TICKS_PER_SECOND is 64,000 (256/4 * 1000). The original Microchip TCP/IP stack
    //had 62500.5. The Microchip TCP/IP stack uses a 6 byte array (tickMicrochip)
    //for it's tick. Seeing that our timer only interrupts every 1ms, we do not have the speed to
    //increment the lower byte of tickMicrochip. Only increment upper 5 bytes. Lower byte will always
    //be 0! Ensure code never uses lower byte for timer calculations!
	//Use Netcruzer utickXxx() functions for micro second ticks!
    if ((tick_val.byte.LB & 0x03) == 0) {
        //Increment bytes 1,2,3 & 4

        //Code below treats tickMicrochip[1] to tickMicrochip[5] as a 40-bit variable, and increments it.
        if (++tickMicrochip[1] == 0) {
            //Increment tickMicrochip bytes 2&3 (WORD offset 1)
            if (++((WORD*)&tickMicrochip)[1] == 0) {
                //Increment tickMicrochip bytes 4&5 (WORD offset 2)
                ((WORD*)&tickMicrochip)[2]++;
            }

            //Alternative to code above - NOT tested! Seems to be slower too!
            //( *((DWORD*)&tickMicrochip[2]) )++; //Increment tickMicrochip bytes 2&3 (WORD offset 1)
        }

        //Alternative to code above - will be slower, seeing that all instructions are Byte access!
        //if (++tickMicrochip[1] == 0) {
        //    if (++tickMicrochip[2] == 0) {
        //        if (++tickMicrochip[3] == 0) {
        //            if (++tickMicrochip[4] == 0) {
        //                tickMicrochip[5]++;
        //            }
        //        }
        //    }
        //}
    }
}


/**
 * Initializes tick values
 */
void tickInit() {
    tick_val.Val = 0;
    memset((void*)tickMicrochip, 0, sizeof(tickMicrochip));
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
			_T3IF = 0;						//Disable TMR2 Interrupt
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



///////////////////////////////////////////////////////////////////////////////
// Microchip Tick functions
///////////////////////////////////////////////////////////////////////////////


/**
 * Function required by Microchip TCP/IP tick! We removed microchip's "TCPIP Stack/Tick.c" from
 * the project, and replaced it with myTick. This new file integrates the Netcruzer tick with
 * the Microchip tick into a single module. It allows functions from "TCPIP Stack/Tick.h" and
 * "nz_tick.h" to be used in the project!
 */
void TickInit(void) {
    tickInit();
}


/**
 * Function required by Microchip TCP/IP tick! We removed microchip's "TCPIP Stack/Tick.c" from
 * the project, and replaced it with myTick. This new file integrates the Netcruzer tick with
 * the Microchip tick into a single module. It allows functions from "TCPIP Stack/Tick.h" and
 * "nz_tick.h" to be used in the project!
 */
DWORD TickGet(void) {
    DWORD dw;
    // Disable interrupts and get copy of current tick in thread save manner
    NZ_BUILTIN_DISI(0x3FFF);	// Disable interrupts, excluding level 7
    dw = *((DWORD*)&tickMicrochip[0]);
    NZ_BUILTIN_DISI(0x0000); 	// Enable interrupts

    return dw;
}

/**
 * Function required by Microchip TCP/IP tick! We removed microchip's "TCPIP Stack/Tick.c" from
 * the project, and replaced it with myTick. This new file integrates the Netcruzer tick with
 * the Microchip tick into a single module. It allows functions from "TCPIP Stack/Tick.h" and
 * "nz_tick.h" to be used in the project!
 */
DWORD TickGetDiv256(void) {
    DWORD dw;

    // Disable interrupts and get copy of current tick in thread save manner
    NZ_BUILTIN_DISI(0x3FFF);	// Disable interrupts, excluding level 7
    // Note: This copy must be done one byte at a time to prevent misaligned memory reads, which will reset the PIC!
    ((WORD*)&dw)[0] = tickMicrochip[1];
    ((BYTE*)&dw)[1] = tickMicrochip[2];
    ((BYTE*)&dw)[2] = tickMicrochip[3];
    ((BYTE*)&dw)[3] = tickMicrochip[4];
    NZ_BUILTIN_DISI(0x0000); // Enable interrupts

    return dw;
}

/**
 * Function required by Microchip TCP/IP tick! We removed microchip's "TCPIP Stack/Tick.c" from
 * the project, and replaced it with myTick. This new file integrates the Netcruzer tick with
 * the Microchip tick into a single module. It allows functions from "TCPIP Stack/Tick.h" and
 * "nz_tick.h" to be used in the project!
 */
DWORD TickGetDiv64K(void) {
    DWORD dw;

    // Disable interrupts and get copy of current tick in thread save manner
    NZ_BUILTIN_DISI(0x3FFF);	// Disable interrupts, excluding level 7

    //Can use this in stead of code below seeing that all bytes are on word boundaries! Else, had to use code below!
    ((WORD*)&dw)[0] = ((WORD*)&tickMicrochip)[1];   //Copy (tickMicrochip bytes 2&3) to (dw bytes 0&1)
    ((WORD*)&dw)[1] = ((WORD*)&tickMicrochip)[2];   //Copy (tickMicrochip bytes 4&5) to (dw bytes 2&3)

    // Note: This copy must be done one byte at a time to prevent misaligned memory reads, which will reset the PIC!
//    ((BYTE*)&dw)[0] = tickMicrochip[2];    // Note: This copy must be done one
//    ((BYTE*)&dw)[1] = tickMicrochip[3];    // byte at a time to prevent misaligned
//    ((BYTE*)&dw)[2] = tickMicrochip[4];    // memory reads, which will reset the PIC.
//    ((BYTE*)&dw)[3] = tickMicrochip[5];
    NZ_BUILTIN_DISI(0x0000); // Enable interrupts

    return dw;
}

/**
 * Function required by Microchip TCP/IP tick! We removed microchip's "TCPIP Stack/Tick.c" from
 * the project, and replaced it with myTick. This new file integrates the Netcruzer tick with
 * the Microchip tick into a single module. It allows functions from "TCPIP Stack/Tick.h" and
 * "nz_tick.h" to be used in the project!
 *
 * This function converts a Tick value or difference to milliseconds. For example,
 * TickConvertToMilliseconds(32768) returns 1000 when a 32.768kHz clock with no prescaler drives
 * the Tick module interrupt.
 */
DWORD TickConvertToMilliseconds(DWORD dwTickValue) {
    return (dwTickValue+(TICKS_PER_SECOND/2000ul))/((DWORD)(TICKS_PER_SECOND/1000ul));
}

