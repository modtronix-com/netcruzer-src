/**
 * @brief           PT66DIN6 Driver Functions
 * @file            nz_pt66din6.c
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
 * 2013-09-03, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_PT66DIN6_C

#include "HardwareProfile.h"

#if defined(XBRD_PT66DIN6)

#include "nz_pt66din6.h"

/////////////////////////////////////////////////
// Defines


/////////////////////////////////////////////////
// Local variables. Lower nibble is current state, upper nibble is latched values


/////////////////////////////////////////////////
// Global variables.


/**
 * Initializes the PT66DIN6 board
 */
void pt66din6_Init() {
}


/**
 * Service function. Must be called every ??ms
 */
void pt66din6_Service() {
}

#endif  //#if defined(XBRD_PT66DIN6)
