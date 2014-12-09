/**
 * @brief           in6BT Bluetooth iMod Driver Functions
 * @file            nz_in6bt.c
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
#define THIS_IS_NZ_IN6BT_C

//!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!
//This file is included by the Netcruzer system from nz_imod.c, and should not be included in a project.
//!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!

#if defined(THIS_IS_NZ_IMOD_C)     //This file is included from nz_imod.c

/**
 * Initialize the in6BT iMod module on given iMod port
 */
void in6bt_Init(BYTE imodPort) {

    //Pin 2 is Reset pin of Bluetooth module, active low

    switch(imodPort) {
        #if defined(IMOD0_IS_IN6BT)
        case 0: //iMod 0
            ///....
        break;
        #endif
        #if defined(IMOD1_IS_IN6BT)
        case 1: //iMod 1
        break;
        #endif
        #if defined(IMOD2_IS_IN6BT)
        case 2: //iMod 2
            imod2_WritePin2(0);
            imod2_ConfDirPin2(OUTPUT_PIN);
        break;
        #endif
        #if defined(IMOD3_IS_IN6BT)
        case 3: //iMod 3
            ///....
        break;
        #endif
        #if defined(IMOD4_IS_IN6BT)
        case 4: //iMod 4
            ///....
        break;
        #endif
        #if defined(IMOD5_IS_IN6BT)
        case 5: //iMod 5
            ///....
        break;
        #endif
        #if defined(IMOD6_IS_IN6BT)
        case 6: //iMod 6
            ///....
        break;
        #endif
        default:
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nin6bt_Init() invalid port!");
            break;
    }
}

/**
 * Enable or disable bluetooth module.
 *
 * @param enable When 0, disable module, else enable it
 */
void in6bt_Enable(BYTE imodPort, BOOL enable) {
    //iMod 1
    if (imodPort==1) {
    }
    //iMod 2
    else if (imodPort==2) {
        //Pin 2 is Reset pin of Bluetooth module, active low
        imod2_WritePin2(enable==0 ? 0 : 1);
    }
}

#endif  //#if defined(THIS_IS_NZ_IMOD_C)     //This file is included from nz_imod.c
#undef THIS_IS_NZ_IN6BT_C

