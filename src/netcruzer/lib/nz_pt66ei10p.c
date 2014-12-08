/**
 * @brief           PT66EI-10P Driver Functions
 * @file            nz_pt66ei10p.c
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
 * 2012-02-22, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_PT66EI_10P_C

#include "HardwareProfile.h"

#if defined(XBRD_PT66EI_REV1) || defined(XBRD_PT66EI_REV2)

#include "nz_pt66ei10p.h"

/////////////////////////////////////////////////
// Defines


/////////////////////////////////////////////////
// Global variables.
BYTE pt66ei10pLeds0to2;

/**
 * Initializes the PT66EI-10P board
 */
void pt66ei10p_Init() {
    //TODO
}

/**
 * Sets all 3 LEDs to the given value. Each bit represents a LED.<br>
 * For example:
 * - To clear all LEDs, call pt66ei10p_WriteLeds(0);
 * - To set first LED (binary format), call  pt66ei10p_WriteLeds(0b00000001);
 * - To set first LED (hex format), call  pt66ei10p_WriteLeds(0x01);
 * - To set first and third LED (binary format), call  pt66ei10p_WriteLeds(0b00000101);
 *
 * @param value The value to set all 8 LEDs to. Each bit represents a LED, 0 if off, 1 is on
 */
void pt66ei10p_WriteLeds(BYTE value) {
    //TODO
}

#endif  //#if defined(XBRD_PT66EI_REV1) || defined(XBRD_PT66EI_REV2)
