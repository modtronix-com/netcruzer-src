/**
 * @brief           State Machine Bask Tasking system
 * @file            smtask.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_smtask_desc Description
 *****************************************
 * This module implements a State Machine Based tasking system. All "Tasks" implement switch
 * statements.
 *
 * The following is a simple example of a task that flashes a LED every 500ms
 *
 * void main(void) {
 *     SMTCB smtcbTest;
 *     while(1)  {
 *         if (smtReady(&smtcbTest) {
 *             stateTest();
 *         }
 *     }
 * }
 * 
 * void stateTest(void) {
 *     if (smtCurrentTask == NULL) {
 *         //write debug error message !!!!!!!!
 *         return;
 *     }
 *     
 *     switch (smtCurrentTask->state) {
 *         case 0:
 *             LED0 = 1;
 *             smtDelay(500);  //Delay 500ms. Set this task to be READY in 500ms again. Also increments SMTCB.state
 *             break;
 *         case 1:
 *             LED0 = 0;
 *             smtDelay(500);  //Delay 500ms. Set this task to be READY in 500ms again. Also increments SMTCB.state
 *             smtSetState(0); //smtDelay above will increment SMTCB.state. But, we want it to rest to 0.
 *             break;
 *     }
 * }
 * 
 * @subsection nz_smtask_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // -------------- SM TASK Configuration --------------------
 // *********************************************************************
 //Number of static MUTEXes. A static MUTEX is assigned at compile time. Must be multiple of 16
 #define SMTASK_STATIC_MUTEX_COUNT 16

 //Number of MUTEXes. Is not a static MUTEX, and can be assigned at run time. Must be multiple of 16
 #define SMTASK_MUTEX_COUNT 32

 //Maximum number of Tasks
 #define SMTASK_COUNT 8

 @endcode
 **********************************************************************
 * @section nz_smtask_lic Software License Agreement
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

#ifndef _SMTASK_H_
#define _SMTASK_H_

//Number of binary semaphores. Must be multiple of 16
#if !defined(SMTASK_BINARY_SEMAPHORE_COUNT)
    #define SMTASK_BINARY_SEMAPHORE_COUNT 16    //< Defines number of Semaphores. Must be multiple of 16
#endif

//Number of counting semaphores.
#if !defined(SMTASK_COUNTING_SEMAPHORE_COUNT)
    #define SMTASK_COUNTING_SEMAPHORE_COUNT 8  //< Defines number of Counting Semaphores.
#endif

//Maximum number of Tasks
#if !defined(SMTASK_COUNT)
    #define SMTASK_COUNT 8                  //< /Maximum number of Tasks
#endif


//Defines
#define SMTASK_STATE_RUNNING    0
#define SMTASK_STATE_READY      1
#define SMTASK_STATE_BLOCKED    2
#define SMTASK_STATE_SUSPENDED  3


/**
 * Array of available Binary Semaphores.
 */
extern WORD smtBinSem[SMTASK_BINARY_SEMAPHORE_COUNT/16];

/**
 * Array of available Counting Semaphores.
 */
extern BYTE smtCntSem[SMTASK_COUNTING_SEMAPHORE_COUNT];


/**
 * smtask Task Control Block
 */
typedef struct __attribute__((__packed__))
{
    /// SMTASK ID
    BYTE   id;
    
    union {
        //IMPORTANT!! Must match CIRBUF_FLAGSVAL_xxx defines
        struct
        {
            unsigned char blockType : 2;        ///< Type of even we are waiting on. 00=Binary Samaphore, 01=Counting Semaphore...
            unsigned char bDelayOrTimeout : 1;  ///< Error occurred, data could not be written to buffer because it was full. Data got lost!
            unsigned char bFill : 5;            ///< Ensure byte

        } flags;
        BYTE flagsVal;
    };
    
    BYTE   state;
    
    BYTE   stateSub;
    
    /// Timer. If 0 task is ready to run. Else wait. Decremented in timer ISR
    /// Is used for delays, and blocking timouts. Flag in flags will indicate current use
    WORD timer;

    //When task is blocked, flag will indicate what this is. Can be ID of a Semaphore, Counting Semaphore....
    WORD blockId;
} SMTCB;

/** Current task */
extern SMTCB* smtCurrentTask;


/**
 * Initialize SM TASK module
 */
void smtInit(void);

/**
 * Check if given task is ready to run AND sets it as current task if it is.
 */
BOOL smtReady(SMTCB* smtcb);


/**
 * Set SMTCB.state to given value
 */
void smtSetState(BYTE newState);

 
/**
 * Delays this task for given time.
 * Will increment SMTCB.state
 */
void smtDelay(WORD delay);


#endif

