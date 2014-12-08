/**
 * @brief           Netcruzer RTOS Task
 * @file            nzos_task.c
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
#define THIS_IS_NZOS_TASK_C

#include "HardwareProfile.h"
#include "nzos_task.h"

#if (nzosTASK_ENABLE==1)

//Add debugging to this file. The DEBUG_CONF_NZOS_TASK macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_NZOS_TASK)
    #define DEBUG_CONF_NZOS_TASK       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_NZOS_TASK
#include "nz_debug.h"


////////// Defines //////////////////////////////


////////// Function Prototypes //////////////////


////////// Variables ////////////////////////////


BYTE nzTskCreate(BYTE priority, void (*ptrTask)(void)) {
    return 0;
}

enum SM_ZO_BLOCK_ {
    SM_BLK_IDLE = 0,
    SM_BLK_DELAY_16BIT_1MS,         ///< Timeout, param1 contains timout value
    SM_BLK_DELAY_32BIT_1MS,         ///< Timeout, param1 contains timout value
    SM_BLK_CIRBUF_EMPTY,            ///< Wait till CIRBUF empty, param1=CIRBUF*
    //Wait till CIRBUF has given bytes available, param1=CIRBUF*, param2=size required
    SM_BLK_CIRBUF_HAS_AVAILABLE,    
} SM_NZOS_BLOCK;

/**
 * Check if task can exit the blocked state
 * @return Return 0 if task is still blocked, else 1
 */
BYTE nzTskCheckBlock(NZOS_TCB* pTCB) {

    switch(pTCB->blocked.lsb) {
        case SM_BLK_IDLE:
            break;
        case SM_BLK_DELAY_16BIT_1MS:
            //Flash system LED every 500ms
            if (tick16TestTmr(pTCB->param1.Val)) {
                goto NZOS_CHECKBLOCK_TO_READY;
            }
            break;
        case SM_BLK_DELAY_32BIT_1MS:
            //Flash system LED every 500ms
            if (tick16TestTmr(pTCB->param1.Val)) {
                goto NZOS_CHECKBLOCK_TO_READY;
            }
            break;
        case SM_BLK_CIRBUF_EMPTY:
            if (cbufIsEmpty((CIRBUF*)(pTCB->param1.Val))) {
                goto NZOS_CHECKBLOCK_TO_READY;
            }
            break;
        case SM_BLK_CIRBUF_HAS_AVAILABLE:
            if (cbufGetCount((CIRBUF*)(pTCB->param1.Val)) > pTCB->param2.Val) {
                goto NZOS_CHECKBLOCK_TO_READY;
            }
            break;
    }
    return 0;   //Task still blocked

    NZOS_CHECKBLOCK_TO_READY:
    NZOS_CHANGE_STATE_BLOCKED_TO_READY(pTCB);
    return 1;   //Ready
}

#endif  //if (nzosTASK_ENABLE==1)