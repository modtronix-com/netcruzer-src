 /**
 * @brief           State Machine Based Tasking system
 * @file            smtask.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
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
 * 2012-06-13, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_SMTASK_MODULE

#define INCLUDE_NETCRUZER_HEADERS
#include "HardwareProfile.h"

#include "smtask.h"


/////////////////////////////////////////////////
// Defines


/////////////////////////////////////////////////
// Global variables. Lower nibble is current state, upper nibble is latched values

WORD smtBinSem[SMTASK_BINARY_SEMAPHORE_COUNT/16];

BYTE smtCntSem[SMTASK_COUNTING_SEMAPHORE_COUNT];

SMTCB* smtCurrentTask;

//Array of all smTasks. Is pointer to a SMTCB object, or NULL
SMTCB* smtTasks[SMTASK_COUNT];


/////////////////////////////////////////////////
// Function prototypes

/**
 * Initialize smTask module
 */
void smtInit(void)
{
    BYTE i;

    //Set all smTasks to NULL
    for (i=SMTASK_COUNT; i>0; i--) {
        smtTasks[i] = NULL;
    }
}


/**
 * Check if given task is ready to run. Checks following:
 * - timer == 0
 */
BOOL smtReady(SMTCB* smtcb)
{
    return 1;
}

/**
 * Set SMTCB.state of current smtState to given value
 */
void smtSetState(BYTE newState)
{
    smtCurrentTask->state = newState;
}


/**
 * Delays this task for given time.
 */
void smtDelay(WORD delay)
{

}

