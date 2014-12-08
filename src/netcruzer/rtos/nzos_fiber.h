/**
 * @brief           Netcruzer RTOS Fibers
 * @file            nzos_fiber.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 & XC32 Compilers
 *
 * @section nzos_fiber_desc Description
 *****************************************
 * Netcruzer RTOS Functions
 * 
 * @subsection nzos_fiber_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ----------- RTOS Fiber Configuration (from nzos_fiber.h) ------------
// *********************************************************************
#define nzosFIBER_ENABLE                        ( 0 )
#define nzosFIBER_LEVELS                        ( 2 )   //Must be 2,4,6 or 8. Each level has 8 fibers

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
#ifndef NZOS_FIBER_H
#define NZOS_FIBER_H

/////////////////////////////////////////////////
//  "RTOS Fiber" - documentation module
/////////////////////////////////////////////////
/**
@defgroup info_rtos_fiber RTOS Fibers
@ingroup info_rtos
Fibers are a way of deferring the main processing burden from interrupt ISRs to Fibers.
Fibers are executed by the "RTOS Kernel", which run in an @ref info_sys_int "Interrupt" ISR at the
lowest priority. They are above the priority of all tasks, and normal code execution, but below
the priority of all other interrupts (line user interrupt for example).

The main use for fibers are to take the load of IRSs, ensuring they can be short and
fast. An ISR will typically save the condition it was interrupted on, instruct a Fiber to run,
and exit. On exit, if there are no other interrupts pending, the code will execute the Fiber.
Code execution will NOT continue at the position it was interrupt, but only after the Fiber has
finished and exited. This way, the higher priority interrupts are not blocked, and we have
transferred the "not so important" part of the interrupt processing from the interrupt ISR to
the Fiber.
*/

#if (nzosFIBER_ENABLE==1)


////////// Defines //////////////////////////////
#ifndef __INLINE_FUNCTION__
#define __INLINE_FUNCTION__ extern inline __attribute__((always_inline))
#endif


////////// Default Defines //////////////////////
#if !defined(nzosFIBER_LEVELS)
#define nzosFIBER_LEVELS                        ( 2 )
#endif

typedef struct FIBER_TCB_ {
WORD*       pSchedule;
WORD        mask;
} FIBER_TCB;


typedef struct NZOS_FIBER_INFO_
//typedef struct __attribute__((__packed__)) NZOS_FIBER_INFO_
//typedef struct __attribute__((aligned(2), packed)) NZOS_FIBER_INFO_
{
    void (*pFibers[nzosFIBER_LEVELS*8])(void);      //Array of fibers
    WORD runBits[nzosFIBER_LEVELS/2];               //Bits indicating what fibers are scheduled to run

    union {
        struct
        {
            unsigned int fill : 16;
        } flagBits;
        struct
        {
            BYTE    LB;                 ///< Lower Byte, contains Buffer Type and Format
            BYTE    HB;                 ///< Upper Byte
        } flagBytes;
        WORD flagVal;
    };
} NZOS_FIBER_INFO;


//#if !defined(THIS_IS_NZOS_FIBER_C)
extern NZOS_FIBER_INFO zvFbrInfo;
//#endif  //#if !defined(THIS_IS_NZOS_FIBER_C)


/**
 * Creates a fiber. To run the created fiber, call nzFbrSchedule() function.
 * The nzosFIBER_LEVELS define defines how many Fiber levels there are.
 * Each level has 8 fibers of incementing priority.
 * The higher the level, the higher the priority.
 * So, if the system is configured for 4 levels, there can be a total of 32 (4x8)
 * fibers.
 *
 * @param level The fiber level, a value from 1 to nzosFIBER_LEVELS.
 *
 * @param highPriority TRUE if the created fiber must have the highest priority
 *          available in the given level. If FALSE, it will be given the lowest
 *          available priority for the given level.
 *
 * @param ptrFiber Pointer to fiber function.
 *
 * @param fbrTcb Pointer to FIBER_TCB
 *
 * @return Returns 0 if successful, else the error code (a NZ_FBR_ERR_XX define).
 *			- 0 = Success
 *			- NZ_FBR_ERR_CREATE = General error
 *			- NZ_FBR_ERR_LEVEL_FULL = No space left in given level
 *			- NZ_FBR_ERR_INVALID_LEVEL = Invalid level given
 */
WORD nzFbrCreate(BYTE level, BOOL highPriority, void (*ptrFiber)(void), FIBER_TCB* fbrTcb);



/**
 * Schedule the fiber to run once. This call can be made from an ISR or Task.
 * The fiber will be run as soon as there are no priority 2-7 interrups pending
 * or being executed.
 *
 * @param pFbrTCB Pointer to the Fiber TCB of the fiber to schedule for running.
 *
 * @return Returns NZ_RTOS_SUCCESS (0) if success, else an NZ_FBR_ERR_xx error code
 */
#define nzFbrSchedule(pFbrTCB)                                  \
    /* Set flag for kernel to indicate fiber must be ran */     \
    *((pFbrTCB)->pSchedule) |= (pFbrTCB)->mask;                 \
    /* Schedule the "Netcruzer RTOS Kernel" to run!  */         \
    NZOS_INT_KERNEL_SET_IF();

#endif  //if (nzosFIBER_ENABLE==1)

#endif  //#ifndef NZ_OS_H

