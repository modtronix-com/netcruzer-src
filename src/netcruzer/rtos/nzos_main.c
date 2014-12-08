/**
 * @brief           Netcruzer RTOS
 * @file            nzos_main.c
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
#define THIS_IS_NZOS_MAIN_C

#include "HardwareProfile.h"
#include "nzos_defsInternal.h"
#include "nzos_main.h"

#if (nzosENABLE==1)

//Normally NEVER include a *.c file, but this is an exception! Include file with processor
//specific interrupts
#if (nzosINCLUDE_PORT_C_FILE==1)
	#if defined(__PIC24F__)
	#include "nzos_PIC24_main.c"
	#elif defined(__PIC32MX__)
	#include "nzos_PIC32MX_main.c"
	#endif
#endif

//Add debugging to this file. The DEBUG_CONF_NZOS_MAIN macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_NZOS_MAIN)
    #define DEBUG_CONF_NZOS_MAIN       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_NZOS_MAIN
#include "nz_debug.h"


////////// Default Defines //////////////////////



////////// Defines //////////////////////////////
enum SM_NZOS_ {
    SM_NZOS_IDLE = 0,
    SM_NZOS_1,
    SM_NZOS_2
} SM_NZOS;


////////// Function Prototypes //////////////////


////////// Global Variables /////////////////////
BYTE zvHeap[nzosHEAP_SIZE];             //Heap
WORD zvUticksPerMS = 0;                 //How many ticks there are per milli second for the nzTckGetUtick() function


////////// Internal Global Variables ////////////


/**
 * Main Netcruzer RTOS Initialization
 *
 * @param How many ticks per milli second there are for the nzTckGetUtick() function.
 *
 * @return 0 if success, else error number
 */
BYTE nzRtosInit(WORD uticksPerMS) {
    zvUticksPerMS = uticksPerMS;

    #if (nzosENABLE==1)

    #endif

    //FSM Tasks
    #if (nzosFSM_ENABLE==1)
    #endif

    //Fibers
    #if (nzosFIBER_ENABLE==1)
        nzFbrInit();
    #endif

    //Tasks
    #if (nzosTASK_ENABLE==1)
    #endif

    //Events
    #if (nzosEVENT_ENABLE==1)
    #endif

    //Mutex
    #if (nzosMUTEX_ENABLE==1)
    #endif

    //Queues
    #if (nzosQUEUE_ENABLE==1)
    #endif

    //Semephores
    #if (nzosSEMAPHORE_ENABLE==1)
    #endif


    //Set main RTOS interrupt priority, clear IF, and enable it
    NZOS_INT_KERNEL_SET_IP(nzosMAIN_INT_PRIORITY);
    NZOS_INT_KERNEL_CLEAR_IF();
    NZOS_INT_KERNEL_ENABLE();

    return 0;
}


/**
 * Main Netcruzer RTOS Task
 */
void nzRtosTask() {
    static BYTE sm_nzos = 0;

    /////////////////////////////////////////////////
    //State machine
    switch(sm_nzos) {
        case SM_NZOS_IDLE:
            break;
    }
    
    return;
}

/**
 * ----- Currently not implemented! -----
 * When nzosENABLED is true, this function must be called from the main system tick.
 */
//void nzRtosTick(void) {
//
//}


#endif  //#if (nzosENABLE==1)