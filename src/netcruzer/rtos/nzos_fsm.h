/**
 * @brief           Netcruzer RTOS Finite State Machine Task
 * @file            nzos_fsm.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 & XC32 Compilers
 *
 * @section nzos_fsm_desc Description
 *****************************************
 * Netcruzer RTOS Functions
 * 
 * @subsection nzos_fsm_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ----------- RTOS Fsm Configuration (from nzos_fsm.h) ------------
// *********************************************************************
#define nzosFSM_ENABLE                         ( 0 )

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
#ifndef NZOS_FSM_H
#define NZOS_FSM_H

#if (nzosFSM_ENABLE==1)


typedef struct NZOS_FCBSM_
//typedef struct __attribute__((__packed__)) NZOS_FCBSM_
//typedef struct __attribute__((aligned(2), packed)) NZOS_FCBSM_
{
    //Header for Tasks, Fibers and "FSM Tasks" must be the same
    
} NZOS_FCBSM;


////////// Functions ////////////////////////////

#endif  //if (nzosFSM_ENABLE==1)

#endif  //#ifndef NZ_OS_H

