/**
 * @brief           Netcruzer RTOS for PIC24
 * @file            nzos_PIC24_main.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 & XC32 Compilers
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
 * 2014-02-02, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZOS_PIC24_MAIN_C

#include "HardwareProfile.h"

#if (nzosENABLE==1) && defined(__PIC24F__)

#include "nzos_defsInternal.h"
#include "nzos_fiber.h"
#include "nz_helpersCx.h"


//Add debugging to this file. The DEBUG_CONF_NZOS_MAIN macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_NZOS_MAIN)
    #define DEBUG_CONF_NZOS_MAIN       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_NZOS_MAIN
#include "nz_debug.h"

#define nzosINT_HAS_ALT_TRAPS


/**
 * Main Netcruzer RTOS Scheduler.
 */
#if (nzosINT_FOR_RTOS_CUSTOM==0)
void __attribute__((interrupt, no_auto_psv)) _SI2C1Interrupt(void)
#else
#error "No RTOS Scheduler defined!"
#endif
{
    WORD *ptrA;

    NZOS_INT_KERNEL_CLEAR_IF();   //Clear interrupt status bit

    //debugPutString("#");


    ////////// Run all Fibers ///////////////////////
    //If there are only 2 priority fiber levels
#if (nzosFIBER_ENABLE==1)
    #if (nzosFIBER_LEVELS==2)
    //#if (0)
    {
        WORD pos;
        //Check if any level 1 and level 2 fibers have to be run (runBits[0] is a WORD, contains level 1 & 2, which are BYTES)
        ptrA = &zvFbrInfo.runBits[0];
        while (*ptrA != 0) {
            nzWordPosOfFirstLsbBit_ASM(*ptrA, pos);
            pos--;  //Convert from 1-16 to 0-15
            //debugPutString("\nPos is ");
            //debugPutWord(pos);
            zvFbrInfo.pFibers[pos](); //Run Fiber

            //Clear Fiber run bit
            *ptrA = (*ptrA) & ~(0x0001 << (pos));
        }
    }
    //There are more than 2 priority fiber levels
    #else
    {
        WORD pos;
        WORD levelX2=nzosFIBER_LEVELS/2;    //Start at highest interrupt priority first!
        do {
            levelX2--;
            //Check if any "fiber run bits" for the current 2 levels are set=run fiber (runBits[level] is a WORD, contains 2 levels, which are BYTES)
            ptrA = &zvFbrInfo.runBits[levelX2];
            while (*ptrA != 0) {
                nzWordPosOfFirstLsbBit_ASM(*ptrA, pos);
                pos--;  //Convert from 1-16 to 0-15
                //debugPutString("\nPos is ");
                //debugPutWord(pos);
                zvFbrInfo.pFibers[(levelX2*16)+pos](); //Run Fiber

                //Clear Fiber run bit
                *ptrA = (*ptrA) & ~(0x0001 << (pos));
            }
        }  while (levelX2 != 0);
    }
    #endif  //#if (nzosFIBER_LEVELS>2)
#endif	//#if (nzosFIBER_ENABLE==1)


}


#if (nzosHANDLE_TRAPS==1)

#define _trapISR __attribute__((interrupt,no_auto_psv))
/**
 * Exception Vector handlers for Oscillator Fail.
 */
void _trapISR _OscillatorFail(void) {
        INTCON1bits.OSCFAIL = 0;
        while(1);
}

/**
 * Exception Vector handlers for Address Error.
 */
void _trapISR _AddressError(void) {
        INTCON1bits.ADDRERR = 0;
        while(1);
}

/**
 * Exception Vector handlers for Stack Error.
 */
void _trapISR _StackError(void) {
        INTCON1bits.STKERR = 0;
        while(1);
}

/**
 * Exception Vector handlers for Math Error.
 */
void _trapISR _MathError(void) {
        INTCON1bits.MATHERR = 0;
        while(1);
}
#endif	//#if (nzosHANDLE_TRAPS==1)

#if (nzosMAIN_HAS_ALT_TRAPS==1)
/**
 * Alternate Exception Vector handlers for Oscillator Fail. The alternative exception vector
 * is used if ALTIVT (INTCON2<15>) = 1. Not required for Netcruzer RTOS, but good to always include.
 */
void _trapISR _AltOscillatorFail(void) {
        INTCON1bits.OSCFAIL = 0;
        while(1);
}

/**
 * Alternate Exception Vector handlers for Address Error. The alternative exception vector
 * is used if ALTIVT (INTCON2<15>) = 1. Not required for Netcruzer RTOS, but good to always include.
 */
void _trapISR _AltAddressError(void) {
        INTCON1bits.ADDRERR = 0;
        while(1);
}

/**
 * Alternate Exception Vector handlers for Stack Error. The alternative exception vector
 * is used if ALTIVT (INTCON2<15>) = 1. Not required for Netcruzer RTOS, but good to always include.
 */
void _trapISR _AltStackError(void) {
        INTCON1bits.STKERR = 0;
        while(1);
}

/**
 * Alternate Exception Vector handlers for Math Error. The alternative exception vector
 * is used if ALTIVT (INTCON2<15>) = 1. Not required for Netcruzer RTOS, but good to always include.
 */
void _trapISR _AltMathError(void) {
        INTCON1bits.MATHERR = 0;
        while(1);
}
#endif  //#if (nzosMAIN_HAS_ALT_TRAPS==1)

#endif  //#if (nzosENABLE==1)