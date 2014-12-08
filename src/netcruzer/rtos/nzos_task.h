/**
 * @brief           Netcruzer RTOS Task
 * @file            nzos_task.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 & XC32 Compilers
 *
 * @section nzos_task_desc Description
 *****************************************
 * Netcruzer RTOS Functions
 * 
 * @subsection nzos_task_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ----------- RTOS Task Configuration (from nzos_task.h) ------------
// *********************************************************************
#define nzosTASK_ENABLE                      ( 0 )

 @endcode
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
#ifndef NZOS_TASK_H
#define NZOS_TASK_H

#if (nzosTASK_ENABLE==1)

//Task states
#define NZOS_TASKSTATE_BLOCKED   0
#define NZOS_TASKSTATE_READY     1
#define NZOS_TASKSTATE_RUNNING   2
#define NZOS_TASKSTATE_SUSPENDED 3

/**
 * Change Task from Blocked to Ready state
 */
#define nzTskStateBlockedToReady(pTCB) ((pTCB)->stateVal++)

/**
 * Change Task from Ready to Running
 */
#define nzTskStateReadyToRunning(pTCB) ((pTCB)->stateVal++)


typedef struct NZOS_TCB_
//typedef struct __attribute__((__packed__)) NZOS_TCB_
//typedef struct __attribute__((aligned(2), packed)) NZOS_TCB_
{
    //---- Header Begin, the same for Tasks, Fibers and "FSM Tasks" -----
    //Blocked state
    union {
        struct
        {
            BYTE lsb;
            BYTE msb;
        } blocked;
        WORD blockedVal;
    };
    void*       pFunc;      //Function Pointer
    WORD        funcParam;  //Function Parameter
    WORD_VAL    param1;
    WORD_VAL    param2;
    WORD        timeout;    //All Blocked and Running tasks have a timeout
    BYTE        sm;
    //---- Header End -----
    BYTE        errNo;      //Error number, 0=OK, else error code
    BYTE        priority;   //Task priority, 0=lowest

    union {
        struct
        {
            unsigned char val   : 4;    ///<Task state, is a NZOS_TASKSTATE_XX define
            unsigned char f1    : 1;
        } state;
        BYTE stateVal;
    };
} NZOS_TCB;


////////// Functions ////////////////////////////

#endif  //if (nzosTASK_ENABLE==1)

#endif  //#ifndef NZ_OS_H

