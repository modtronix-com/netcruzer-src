/**
 * @brief           Contains hardware specific defines for the im2BL Button/LED iMod module.
 * @file            nz_im2bl.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section im2bl_desc Description
 *****************************************
 * This module includes hardware specific defines for the im2BL Button/LED iMod module.
 * This file is normally not used by itself, but is included by other files that implement
 * iMod ports. For example, the nz_pt66din6.h and nz_pt66eci.h. For details on using this
 * module, see the documentation of these file.
 *
 * @subsection nz_im2bl_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ---------- nz_im2bl Configuration (from nz_im2bl.h) -----------
// *********************************************************************
//Indicates how many im2BL iMod modules there are, set to 0 to disable
#define IM2BL_COUNT     1
//Gives the iMod ports that the im2BL modules are plugged into, CHANGE as required!
#define IMOD1_IS_IM2BL

 @endcode
 **********************************************************************
 * @section im2bl_lic Software License Agreement
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
#ifndef IM2BL_H
#define IM2BL_H



/////////////////////////////////////////////////
//Compatibility Tests

#if defined(IMOD2_IS_IM2BL)
#if    ( (!defined(IMOD2_PIN2_IS_DIG) && !defined(IMOD2_PIN2_IS_XDIG))  \
    || (!defined(IMOD2_PIN3_IS_DIG) && !defined(IMOD2_PIN3_IS_XDIG))    \
    || (!defined(IMOD2_PIN4_IS_DIG) && !defined(IMOD2_PIN4_IS_XDIG))    \
    || (!defined(IMOD2_PIN5_IS_DIG) && !defined(IMOD2_PIN5_IS_XDIG)) )
#error "iMod port 2 is not compatible with the im2BL module!"
#endif
#endif



/////////////////////////////////////////////////
//Buttons

#if defined(IM2BL_ENABLE_MULTI_PORT_FUNCTIONS)
#define im2bl_ReadButton1(port) (imod_GetPin5(port)==0?1:0)
#endif

#if defined(IM2BL_ENABLE_MULTI_PORT_FUNCTIONS)
#define im2bl_ReadButton2(port) (imod_GetPin3(port)==0?1:0)
#endif


#if defined(IMOD0_IS_IM2BL)
#define imod0_ReadButton1()      (imod0_GetPin5()==0?1:0)
#define im2bl_ReadButton1Imod0() (imod0_GetPin5()==0?1:0)
#define imod0_ReadButton2()      (imod0_GetPin3()==0?1:0)
#define im2bl_ReadButton2Imod0() (imod0_GetPin3()==0?1:0)
#define imod0_WriteLed1(val)      (imod0_SetPin2(val))
#define im2bl_WriteLed1Imod0(val) (imod0_SetPin2(val))
#define imod0_WriteLed2(val)      (imod0_SetPin4(val))
#define im2bl_WriteLed2Imod0(val) (imod0_SetPin4(val))
#endif

#if defined(IMOD1_IS_IM2BL)
#define imod1_ReadButton1()      (imod1_ReadPin5()==0?1:0)
#define im2bl_ReadButton1Imod1() (imod1_ReadPin5()==0?1:0)
#define imod1_ReadButton2()      (imod1_ReadPin3()==0?1:0)
#define im2bl_ReadButton2Imod1() (imod1_ReadPin3()==0?1:0)
#define imod1_WriteLed1(val)      (imod1_WritePin2(val))
#define im2bl_WriteLed1Imod1(val) (imod1_WritePin2(val))
#define imod1_WriteLed2(val)      (imod1_WritePin4(val))
#define im2bl_WriteLed2Imod1(val) (imod1_WritePin4(val))
#endif

#if defined(IMOD2_IS_IM2BL)
#define imod2_ReadButton1()      (imod2_ReadPin5()==0?1:0)
#define im2bl_ReadButton1Imod2() (imod2_ReadPin5()==0?1:0)
#define imod2_ReadButton2()      (imod2_ReadPin3()==0?1:0)
#define im2bl_ReadButton2Imod2() (imod2_ReadPin3()==0?1:0)
#define imod2_WriteLed1(val)      (imod2_WritePin2(val))
#define im2bl_WriteLed1Imod2(val) (imod2_WritePin2(val))
#define imod2_WriteLed2(val)      (imod2_WritePin4(val))
#define im2bl_WriteLed2Imod2(val) (imod2_WritePin4(val))
#endif

#if defined(IMOD3_IS_IM2BL)
#define imod3_ReadButton1()      (imod3_GetPin5()==0?1:0)
#define im2bl_ReadButton1Imod3() (imod3_GetPin5()==0?1:0)
#define imod3_ReadButton2()      (imod3_GetPin3()==0?1:0)
#define im2bl_ReadButton2Imod3() (imod3_GetPin3()==0?1:0)
#define imod3_WriteLed1(val)      (imod3_SetPin2(val))
#define im2bl_WriteLed1Imod3(val) (imod3_SetPin2(val))
#define imod3_WriteLed2(val)      (imod3_SetPin4(val))
#define im2bl_WriteLed2Imod3(val) (imod3_SetPin4(val))
#endif

#if defined(IMOD4_IS_IM2BL)
#define imod4_ReadButton1()      (imod4_GetPin5()==0?1:0)
#define im2bl_ReadButton1Imod4() (imod4_GetPin5()==0?1:0)
#define imod4_ReadButton2()      (imod4_GetPin3()==0?1:0)
#define im2bl_ReadButton2Imod4() (imod4_GetPin3()==0?1:0)
#define imod4_WriteLed1(val)      (imod4_SetPin2(val))
#define im2bl_WriteLed1Imod4(val) (imod4_SetPin2(val))
#define imod4_WriteLed2(val)      (imod4_SetPin4(val))
#define im2bl_WriteLed2Imod4(val) (imod4_SetPin4(val))
#endif

#if defined(IMOD5_IS_IM2BL)
#define imod5_ReadButton1()      (imod5_GetPin5()==0?1:0)
#define im2bl_ReadButton1Imod5() (imod5_GetPin5()==0?1:0)
#define imod5_ReadButton2()      (imod5_GetPin3()==0?1:0)
#define im2bl_ReadButton2Imod5() (imod5_GetPin3()==0?1:0)
#define imod5_WriteLed1(val)      (imod5_SetPin2(val))
#define im2bl_WriteLed1Imod5(val) (imod5_SetPin2(val))
#define imod5_WriteLed2(val)      (imod5_SetPin4(val))
#define im2bl_WriteLed2Imod5(val) (imod5_SetPin4(val))
#endif

#if defined(IMOD6_IS_IM2BL)
#define imod6_ReadButton1()      (imod6_GetPin5()==0?1:0)
#define im2bl_ReadButton1Imod6() (imod6_GetPin5()==0?1:0)
#define imod6_ReadButton2()      (imod6_GetPin3()==0?1:0)
#define im2bl_ReadButton2Imod6() (imod6_GetPin3()==0?1:0)
#define imod6_WriteLed1(val)      (imod6_SetPin2(val))
#define im2bl_WriteLed1Imod6(val) (imod6_SetPin2(val))
#define imod6_WriteLed2(val)      (imod6_SetPin4(val))
#define im2bl_WriteLed2Imod6(val) (imod6_SetPin4(val))
#endif

#if defined(IMOD7_IS_IM2BL)
#define imod7_ReadButton1()      (imod7_GetPin5()==0?1:0)
#define im2bl_ReadButton1Imod7() (imod7_GetPin5()==0?1:0)
#define imod7_ReadButton2()      (imod7_GetPin3()==0?1:0)
#define im2bl_ReadButton2Imod7() (imod7_GetPin3()==0?1:0)
#define imod7_WriteLed1(val)      (imod7_SetPin2(val))
#define im2bl_WriteLed1Imod7(val) (imod7_SetPin2(val))
#define imod7_WriteLed2(val)      (imod7_SetPin4(val))
#define im2bl_WriteLed2Imod7(val) (imod7_SetPin4(val))
#endif

#if defined(IMOD8_IS_IM2BL)
#define imod8_ReadButton1()      (imod8_GetPin5()==0?1:0)
#define im2bl_ReadButton1Imod8() (imod8_GetPin5()==0?1:0)
#define imod8_ReadButton2()      (imod8_GetPin3()==0?1:0)
#define im2bl_ReadButton2Imod8() (imod8_GetPin3()==0?1:0)
#define imod8_WriteLed1(val)      (imod8_SetPin2(val))
#define im2bl_WriteLed1Imod8(val) (imod8_SetPin2(val))
#define imod8_WriteLed2(val)      (imod8_SetPin4(val))
#define im2bl_WriteLed2Imod8(val) (imod8_SetPin4(val))
#endif

#if defined(IMOD9_IS_IM2BL)
#define imod9_ReadButton1()      (imod9_GetPin5()==0?1:0)
#define im2bl_ReadButton1Imod9() (imod9_GetPin5()==0?1:0)
#define imod9_ReadButton2()      (imod9_GetPin3()==0?1:0)
#define im2bl_ReadButton2Imod9() (imod9_GetPin3()==0?1:0)
#define imod9_WriteLed1(val)      (imod9_SetPin2(val))
#define im2bl_WriteLed1Imod9(val) (imod9_SetPin2(val))
#define imod9_WriteLed2(val)      (imod9_SetPin4(val))
#define im2bl_WriteLed2Imod9(val) (imod9_SetPin4(val))
#endif

#if defined(IMOD10_IS_IM2BL)
#define imod10_ReadButton1()      (imod10_GetPin5()==0?1:0)
#define im2bl_ReadButton1Imod10() (imod10_GetPin5()==0?1:0)
#define imod10_ReadButton2()      (imod10_GetPin3()==0?1:0)
#define im2bl_ReadButton2Imod10() (imod10_GetPin3()==0?1:0)
#define imod10_WriteLed1(val)      (imod10_SetPin2(val))
#define im2bl_WriteLed1Imod10(val) (imod10_SetPin2(val))
#define imod10_WriteLed2(val)      (imod10_SetPin4(val))
#define im2bl_WriteLed2Imod10(val) (imod10_SetPin4(val))
#endif


/**
 * im2BL Initialization
 */
void im2bl_Init(BYTE imodPort);






#endif
