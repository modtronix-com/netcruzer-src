/**
 * @brief           Netcruzer RTOS Mutexs
 * @file            nzos_mutex.c
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
#define THIS_IS_NZOS_MUTEX_C

#include "HardwareProfile.h"
#include "nzos_mutex.h"

#if (nzosMUTEX_ENABLE==1)

//Add debugging to this file. The DEBUG_CONF_NZOS_MUTEX macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_NZOS_MUTEX)
    #define DEBUG_CONF_NZOS_MUTEX       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_NZOS_MUTEX
#include "nz_debug.h"


////////// Defines //////////////////////////////


////////// Function Prototypes //////////////////


////////// Variables ////////////////////////////


#endif  //if (nzosMUTEX_ENABLE==1)