/**
 * @brief           Contains hardware specific defines for the im4MOS MOSFET output iMod module.
 * @file            nz_im4mos.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section nz_im4mos_desc Description
 *****************************************
 * This module includes hardware specific defines for the im4MOS Mosfet Output iMod module.
 * This file is normally not used by itself, but is included by other files that implement
 * iMod ports. For example, the nz_pt66din6.h and nz_pt66eci.h. For details on using this
 * module, see the documentation of these file.
 *
 * @subsection nz_im4mos_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ---------- nz_im4mos Configuration (from im4mos.h) -----------
// *********************************************************************
//Indicates how many im4MOS iMod modules there are, set to 0 to disable
#define IM4MOS_COUNT     1
//Gives the iMod ports that the im4MOS modules are plugged into, CHANGE as required!
#define IMOD1_IS_IM4MOS

 @endcode
 **********************************************************************
 * @section nz_im4mos_lic Software License Agreement
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
#ifndef IM4MOS_H
#define IM4MOS_H


/////////////////////////////////////////////////
//Buttons

#if defined(IM4MOS_ENABLE_MULTI_PORT_FUNCTIONS)
#define im4mos_WriteOutput2Imod0(port, value)   (imod_SetPin2(port, value))
#define im4mos_WriteOutput3Imod0(port, value)   (imod_SetPin3(port, value))
#define im4mos_WriteOutput4Imod0(port, value)   (imod_SetPin4(port, value))
#define im4mos_WriteOutput5Imod0(port, value)   (imod_SetPin5(port, value))
#endif

#if defined(IMOD0_IS_IM4MOS)
#define imod0_WriteOutput2(value)       (imod0_SetPin2(value))
#define im4mos_WriteOutput2Imod0(value) (imod0_SetPin2(value))
#define imod0_WriteOutput3(value)       (imod0_SetPin3(value))
#define im4mos_WriteOutput3Imod0(value) (imod0_SetPin3(value))
#define imod0_WriteOutput4(value)       (imod0_SetPin4(value))
#define im4mos_WriteOutput4Imod0(value) (imod0_SetPin4(value))
#define imod0_WriteOutput5(value)       (imod0_SetPin5(value))
#define im4mos_WriteOutput5Imod0(value) (imod0_SetPin5(value))
#endif

#if defined(IMOD1_IS_IM4MOS)
#define imod1_WriteOutput2(value)       (imod1_WritePin2(value))
#define im4mos_WriteOutput2Imod1(value) (imod1_WritePin2(value))
#define imod1_WriteOutput3(value)       (imod1_WritePin3(value))
#define im4mos_WriteOutput3Imod1(value) (imod1_WritePin3(value))
#define imod1_WriteOutput4(value)       (imod1_WritePin4(value))
#define im4mos_WriteOutput4Imod1(value) (imod1_WritePin4(value))
#define imod1_WriteOutput5(value)       (imod1_WritePin5(value))
#define im4mos_WriteOutput5Imod1(value) (imod1_WritePin5(value))
#endif

#if defined(IMOD2_IS_IM4MOS)
#define imod2_WriteOutput2(value)       (imod2_WritePin2(value))
#define im4mos_WriteOutput2Imod2(value) (imod2_WritePin2(value))
#define imod2_WriteOutput3(value)       (imod2_WritePin3(value))
#define im4mos_WriteOutput3Imod2(value) (imod2_WritePin3(value))
#define imod2_WriteOutput4(value)       (imod2_WritePin4(value))
#define im4mos_WriteOutput4Imod2(value) (imod2_WritePin4(value))
#define imod2_WriteOutput5(value)       (imod2_WritePin5(value))
#define im4mos_WriteOutput5Imod2(value) (imod2_WritePin5(value))
#endif

#if defined(IMOD3_IS_IM4MOS)
#define imod3_WriteOutput2(value)       (imod3_SetPin2(value))
#define im4mos_WriteOutput2Imod3(value) (imod3_SetPin2(value))
#define imod3_WriteOutput3(value)       (imod3_SetPin3(value))
#define im4mos_WriteOutput3Imod3(value) (imod3_SetPin3(value))
#define imod3_WriteOutput4(value)       (imod3_SetPin4(value))
#define im4mos_WriteOutput4Imod3(value) (imod3_SetPin4(value))
#define imod3_WriteOutput5(value)       (imod3_SetPin5(value))
#define im4mos_WriteOutput5Imod3(value) (imod3_SetPin5(value))
#endif

#if defined(IMOD4_IS_IM4MOS)
#define imod4_WriteOutput2(value)       (imod4_SetPin2(value))
#define im4mos_WriteOutput2Imod4(value) (imod4_SetPin2(value))
#define imod4_WriteOutput3(value)       (imod4_SetPin3(value))
#define im4mos_WriteOutput3Imod4(value) (imod4_SetPin3(value))
#define imod4_WriteOutput4(value)       (imod4_SetPin4(value))
#define im4mos_WriteOutput4Imod4(value) (imod4_SetPin4(value))
#define imod4_WriteOutput5(value)       (imod4_SetPin5(value))
#define im4mos_WriteOutput5Imod4(value) (imod4_SetPin5(value))
#endif

#if defined(IMOD5_IS_IM4MOS)
#define imod5_WriteOutput2(value)       (imod5_SetPin2(value))
#define im4mos_WriteOutput2Imod5(value) (imod5_SetPin2(value))
#define imod5_WriteOutput3(value)       (imod5_SetPin3(value))
#define im4mos_WriteOutput3Imod5(value) (imod5_SetPin3(value))
#define imod5_WriteOutput4(value)       (imod5_SetPin4(value))
#define im4mos_WriteOutput4Imod5(value) (imod5_SetPin4(value))
#define imod5_WriteOutput5(value)       (imod5_SetPin5(value))
#define im4mos_WriteOutput5Imod5(value) (imod5_SetPin5(value))
#endif

#if defined(IMOD6_IS_IM4MOS)
#define imod6_WriteOutput2(value)       (imod6_SetPin2(value))
#define im4mos_WriteOutput2Imod6(value) (imod6_SetPin2(value))
#define imod6_WriteOutput3(value)       (imod6_SetPin3(value))
#define im4mos_WriteOutput3Imod6(value) (imod6_SetPin3(value))
#define imod6_WriteOutput4(value)       (imod6_SetPin4(value))
#define im4mos_WriteOutput4Imod6(value) (imod6_SetPin4(value))
#define imod6_WriteOutput5(value)       (imod6_SetPin5(value))
#define im4mos_WriteOutput5Imod6(value) (imod6_SetPin5(value))
#endif

#if defined(IMOD7_IS_IM4MOS)
#define imod7_WriteOutput2(value)       (imod7_SetPin2(value))
#define im4mos_WriteOutput2Imod7(value) (imod7_SetPin2(value))
#define imod7_WriteOutput3(value)       (imod7_SetPin3(value))
#define im4mos_WriteOutput3Imod7(value) (imod7_SetPin3(value))
#define imod7_WriteOutput4(value)       (imod7_SetPin4(value))
#define im4mos_WriteOutput4Imod7(value) (imod7_SetPin4(value))
#define imod7_WriteOutput5(value)       (imod7_SetPin5(value))
#define im4mos_WriteOutput5Imod7(value) (imod7_SetPin5(value))
#endif

#if defined(IMOD8_IS_IM4MOS)
#define imod8_WriteOutput2(value)       (imod8_SetPin2(value))
#define im4mos_WriteOutput2Imod8(value) (imod8_SetPin2(value))
#define imod8_WriteOutput3(value)       (imod8_SetPin3(value))
#define im4mos_WriteOutput3Imod8(value) (imod8_SetPin3(value))
#define imod8_WriteOutput4(value)       (imod8_SetPin4(value))
#define im4mos_WriteOutput4Imod8(value) (imod8_SetPin4(value))
#define imod8_WriteOutput5(value)       (imod8_SetPin5(value))
#define im4mos_WriteOutput5Imod8(value) (imod8_SetPin5(value))
#endif

#if defined(IMOD9_IS_IM4MOS)
#define imod9_WriteOutput2(value)       (imod9_SetPin2(value))
#define im4mos_WriteOutput2Imod9(value) (imod9_SetPin2(value))
#define imod9_WriteOutput3(value)       (imod9_SetPin3(value))
#define im4mos_WriteOutput3Imod9(value) (imod9_SetPin3(value))
#define imod9_WriteOutput4(value)       (imod9_SetPin4(value))
#define im4mos_WriteOutput4Imod9(value) (imod9_SetPin4(value))
#define imod9_WriteOutput5(value)       (imod9_SetPin5(value))
#define im4mos_WriteOutput5Imod9(value) (imod9_SetPin5(value))
#endif

#if defined(IMOD10_IS_IM4MOS)
#define imod10_WriteOutput2(value)       (imod10_SetPin2(value))
#define im4mos_WriteOutput2Imod10(value) (imod10_SetPin2(value))
#define imod10_WriteOutput3(value)       (imod10_SetPin3(value))
#define im4mos_WriteOutput3Imod10(value) (imod10_SetPin3(value))
#define imod10_WriteOutput4(value)       (imod10_SetPin4(value))
#define im4mos_WriteOutput4Imod10(value) (imod10_SetPin4(value))
#define imod10_WriteOutput5(value)       (imod10_SetPin5(value))
#define im4mos_WriteOutput5Imod10(value) (imod10_SetPin5(value))
#endif


/**
 * im4MOS Initialization
 */
void im4mos_Init(BYTE imodPort);






#endif
