/**
 * @brief           Contains hardware specific defines for the in6BT Bluetooth iMod module.
 * @file            nz_in6bt.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section in6bt_desc Description
 *****************************************
 * This module includes hardware specific defines for the in6BT Bluetooth iMod module.
 * This file is normally not used by itself, but is included by other files that implement
 * iMod ports. For example, the nz_pt66din6.h and nz_pt66eci.h. For details on using this
 * module, see the documentation of these file.
 *
 * @subsection nz_in6bt_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ---------- nz_in6bt Configuration (from nz_in6bt.h) -----------
// *********************************************************************
#define IN6BT_COUNT     1   //Gives number of in6BT modules in system
#define IMOD2_IS_IN6BT      //iMod port 2 has a in6BT module, CHANGE as required!

 @endcode
 **********************************************************************
 * @section in6bt_lic Software License Agreement
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
 * 2012-08-06, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef IN6BT_H
#define IN6BT_H

/**
 * Initialize in6BT iMod module
 * @param imodPort
 */
void in6bt_Init(BYTE imodPort);

/**
 * Enable in6BT iMod module
 * @param imodPort Port, is a 0-n value
 * @param enable 1 to enable, 0 to disable
 */
void in6bt_Enable(BYTE imodPort, BOOL enable);

#endif
