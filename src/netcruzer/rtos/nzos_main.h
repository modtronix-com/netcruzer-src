/**
 * @brief           RTOS Functions
 * @file            nzos_main.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 & XC32 Compilers
 *
 * @section nzos_main_desc Description
 *****************************************
 * RTOS Functions
 * 
 * @subsection nzos_main_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --------- Netcruzer RTOS Main (from nzos_main.h) -----------
// *********************************************************************

 @endcode
 * 
 **********************************************************************
 * @section nzos_main_lic Software License Agreement
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
#ifndef NZOS_MAIN_H
#define NZOS_MAIN_H

#if (nzosENABLE==1)


////////// Functions ////////////////////////////

/**
 * Main Netcruzer RTOS Initialization
 *
 * @param How many ticks per milli second there are for the nzTckGetUtick() function.
 *
 * @return 0 if success, else error number
 */
BYTE nzRtosInit(WORD uticksPerMS);


/**
 * When nzosENABLED is true, this function must be called from the main system tick.
 */
//void nzRtosTick(void);
#define nzRtosTick()		/* Currently not implemented! */

#endif  //#if (nzosENABLE==1)

#endif  //#ifndef NZ_OS_H
