/**
 * @brief           im4MOS 4 MOSFET Output iMod Driver Functions
 * @file            nz_im4MOS.c
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
 * 2013-11-14, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_IM4MOS_C

//!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!
//This file is included by the Netcruzer system from nz_imod.c, and should not be included in a project.
//!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!

//Not required if including this file from nz_imod.c. It is already included from nz_imod.c
#if !defined(THIS_IS_NZ_IMOD_C)
#include "HardwareProfile.h"
#include "nz_im4mos.h"
#endif

/**
 * Initialize the im4MOS iMod module on given iMod port
 */
void im4mos_Init(BYTE imodPort) {
    switch(imodPort) {
        #if defined(IMOD0_IS_IM4MOS)
        case 0: //iMod 0
            imod0_SetPin2(0);
            imod0_ConfDirPin2(OUTPUT_PIN);  //MOSFET output 2
            imod0_SetPin3(0);
            imod0_ConfDirPin3(OUTPUT_PIN);  //MOSFET output 3
            imod0_SetPin4(0);
            imod0_ConfDirPin4(OUTPUT_PIN);  //MOSFET output 4
            imod0_SetPin5(0);
            imod0_ConfDirPin5(OUTPUT_PIN);  //MOSFET output 5
        break;
        #endif
        #if defined(IMOD1_IS_IM4MOS)
        case 1: //iMod 1
            imod1_WritePin2(0);
            imod1_ConfDirPin2(OUTPUT_PIN);  //MOSFET output 2
            imod1_WritePin3(0);
            imod1_ConfDirPin3(OUTPUT_PIN);  //MOSFET output 3
            imod1_WritePin4(0);
            imod1_ConfDirPin4(OUTPUT_PIN);  //MOSFET output 4
            imod1_WritePin5(0);
            imod1_ConfDirPin5(OUTPUT_PIN);  //MOSFET output 5
        break;
        #endif
        #if defined(IMOD2_IS_IM4MOS)
        case 2: //iMod 2
            imod2_WritePin2(0);
            imod2_ConfDirPin2(OUTPUT_PIN);  //MOSFET output 2
            imod2_WritePin3(0);
            imod2_ConfDirPin3(OUTPUT_PIN);  //MOSFET output 3
            imod2_WritePin4(0);
            imod2_ConfDirPin4(OUTPUT_PIN);  //MOSFET output 4
            imod2_WritePin5(0);
            imod2_ConfDirPin5(OUTPUT_PIN);  //MOSFET output 5
        break;
        #endif
        #if defined(IMOD3_IS_IM4MOS)
        case 3: //iMod 3
            imod3_SetPin2(0);
            imod3_ConfDirPin2(OUTPUT_PIN);  //MOSFET output 2
            imod3_SetPin3(0);
            imod3_ConfDirPin3(OUTPUT_PIN);  //MOSFET output 3
            imod3_SetPin4(0);
            imod3_ConfDirPin4(OUTPUT_PIN);  //MOSFET output 4
            imod3_SetPin5(0);
            imod3_ConfDirPin5(OUTPUT_PIN);  //MOSFET output 5
        break;
        #endif
        #if defined(IMOD4_IS_IM4MOS)
        case 4: //iMod 4
            imod4_SetPin2(0);
            imod4_ConfDirPin2(OUTPUT_PIN);  //MOSFET output 2
            imod4_SetPin3(0);
            imod4_ConfDirPin3(OUTPUT_PIN);  //MOSFET output 3
            imod4_SetPin4(0);
            imod4_ConfDirPin4(OUTPUT_PIN);  //MOSFET output 4
            imod4_SetPin5(0);
            imod4_ConfDirPin5(OUTPUT_PIN);  //MOSFET output 5
        break;
        #endif
        #if defined(IMOD5_IS_IM4MOS)
        case 5: //iMod 5
            imod5_SetPin2(0);
            imod5_ConfDirPin2(OUTPUT_PIN);  //MOSFET output 2
            imod5_SetPin3(0);
            imod5_ConfDirPin3(OUTPUT_PIN);  //MOSFET output 3
            imod5_SetPin4(0);
            imod5_ConfDirPin4(OUTPUT_PIN);  //MOSFET output 4
            imod5_SetPin5(0);
            imod5_ConfDirPin5(OUTPUT_PIN);  //MOSFET output 5
        break;
        #endif
        #if defined(IMOD6_IS_IM4MOSIM2BL)
        case 6: //iMod 6
            imod6_SetPin2(0);
            imod6_ConfDirPin2(OUTPUT_PIN);  //MOSFET output 2
            imod6_SetPin3(0);
            imod6_ConfDirPin3(OUTPUT_PIN);  //MOSFET output 3
            imod6_SetPin4(0);
            imod6_ConfDirPin4(OUTPUT_PIN);  //MOSFET output 4
            imod6_SetPin5(0);
            imod6_ConfDirPin5(OUTPUT_PIN);  //MOSFET output 5
        break;
        #endif
        default:
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nim4mos_Init() invalid port!");
            break;
    }
}

#undef THIS_IS_NZ_IM4MOS_C

