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

#if (nzosENABLE==1) && defined(__PIC32MX__)

#include "nzos_defsInternal.h"


//Add debugging to this file. The DEBUG_CONF_NZOS_MAIN macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_NZOS_MAIN)
    #define DEBUG_CONF_NZOS_MAIN       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_NZOS_MAIN
#include "nz_debug.h"


/**
 * Main Netcruzer RTOS Scheduler.
 */
#if (nzosINT_FOR_RTOS_CUSTOM==0)
void __attribute((interrupt(ipl2), vector(_EXTERNAL_1_VECTOR), nomips16)) _T1ExtInterrupt(void) {
//void __ISR(_EXTERNAL_1_VECTOR, IPL2) _T1ExtInterrupt(void) {   //Using __ISR macro
#else
#error "No RTOS Scheduler defined!"
#endif
    NZOS_INT_KERNEL_CLEAR_IF();   //Clear interrupt status bit
    
}


#if (nzosHANDLE_TRAPS==1)

//Implement traps for PIC32MX
///**
// * Exception Vector handlers for Oscillator Fail.
// */
//void _trapISR _OscillatorFail(void) {
//        INTCON1bits.OSCFAIL = 0;
//        while(1);
//}
//
///**
// * Exception Vector handlers for Address Error.
// */
//void _trapISR _AddressError(void) {
//        INTCON1bits.ADDRERR = 0;
//        while(1);
//}
//
///**
// * Exception Vector handlers for Stack Error.
// */
//void _trapISR _StackError(void) {
//        INTCON1bits.STKERR = 0;
//        while(1);
//}
//
///**
// * Exception Vector handlers for Math Error.
// */
//void _trapISR _MathError(void) {
//        INTCON1bits.MATHERR = 0;
//        while(1);
//}
#endif	//#if (nzosHANDLE_TRAPS==1)

#endif  //#if (nzosENABLE==1)