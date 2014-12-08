/**
 * @brief           Contains hardware specific defines for iMod modules.
 * @file            nz_imod.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section imod_desc Description
 *****************************************
 * This module includes hardware specific defines for iMod modules.
 *
 * @subsection nz_imod_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ---------- nz_imod Configuration (from nz_imod.h) -----------
// *********************************************************************

 @endcode
 **********************************************************************
 * @section imod_lic Software License Agreement
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
#ifndef IMOD_H
#define IMOD_H



/////////////////////////////////////////////////
// im2BL
#if (  defined(IMOD0_IS_IM2BL) || defined(IMOD1_IS_IM2BL) || defined(IMOD2_IS_IM2BL)    \
    || defined(IMOD3_IS_IM2BL) || defined(IMOD4_IS_IM2BL) || defined(IMOD5_IS_IM2BL)    \
    || defined(IMOD6_IS_IM2BL) || defined(IMOD7_IS_IM2BL) || defined(IMOD8_IS_IM2BL)    \
    || defined(IMOD9_IS_IM2BL) || defined(IMOD10_IS_IM2BL) )

#if !defined(IM2BL_COUNT)
    #define IM2BL_COUNT 1
#endif

#include "nz_im2bl.h"
#endif



/////////////////////////////////////////////////
// im4MOS
#if (  defined(IMOD0_IS_IM4MOS) || defined(IMOD1_IS_IM4MOS) || defined(IMOD2_IS_IM4MOS)    \
    || defined(IMOD3_IS_IM4MOS) || defined(IMOD4_IS_IM4MOS) || defined(IMOD5_IS_IM4MOS)    \
    || defined(IMOD6_IS_IM4MOS) || defined(IMOD7_IS_IM4MOS) || defined(IMOD8_IS_IM4MOS)    \
    || defined(IMOD9_IS_IM4MOS) || defined(IMOD10_IS_IM4MOS) )

#if !defined(IM4MOS_COUNT)
    #define IM4MOS_COUNT 1
#endif

#include "nz_im4mos.h"
#endif



/////////////////////////////////////////////////
// in6BT
#if (  defined(IMOD0_IS_IN6BT) || defined(IMOD1_IS_IN6BT) || defined(IMOD2_IS_IN6BT)    \
    || defined(IMOD3_IS_IN6BT) || defined(IMOD4_IS_IN6BT) || defined(IMOD5_IS_IN6BT)    \
    || defined(IMOD6_IS_IN6BT) || defined(IMOD7_IS_IN6BT) || defined(IMOD8_IS_IN6BT)    \
    || defined(IMOD9_IS_IN6BT) || defined(IMOD10_IS_IN6BT) )

#if !defined(IN6BT_COUNT)
    #define IN6BT_COUNT 1
#endif

#include "nz_in6bt.h"
#endif



/////////////////////////////////////////////////
/////////////////////////////////////////////////
// imodx_GetPin() default defines
// imodx_SetPin() default defines

//Pin 2
#if !defined(IMOD0_PIN2_IS_DIG) && !defined(IMOD0_PIN2_IS_XDIG)
#define imod0_GetPin2() 0
#define imod0_SetPin2() 0
#endif
#if !defined(IMOD1_PIN2_IS_DIG) && !defined(IMOD1_PIN2_IS_XDIG)
#define imod1_ReadPin2() 0
#define imod1_WritePin2() 0
#endif
#if !defined(IMOD2_PIN2_IS_DIG) && !defined(IMOD2_PIN2_IS_XDIG)
#define imod2_ReadPin2() 0
#define imod2_WritePin2() 0
#endif
#if !defined(IMOD3_PIN2_IS_DIG) && !defined(IMOD3_PIN2_IS_XDIG)
#define imod3_GetPin2() 0
#define imod3_SetPin2() 0
#endif
#if !defined(IMOD4_PIN2_IS_DIG) && !defined(IMOD4_PIN2_IS_XDIG)
#define imod4_GetPin2() 0
#define imod4_SetPin2() 0
#endif
#if !defined(IMOD5_PIN2_IS_DIG) && !defined(IMOD5_PIN2_IS_XDIG)
#define imod5_GetPin2() 0
#define imod5_SetPin2() 0
#endif
#if !defined(IMOD6_PIN2_IS_DIG) && !defined(IMOD6_PIN2_IS_XDIG)
#define imod6_GetPin2() 0
#define imod6_SetPin2() 0
#endif
#if !defined(IMOD7_PIN2_IS_DIG) && !defined(IMOD7_PIN2_IS_XDIG)
#define imod7_GetPin2() 0
#endif
#if !defined(IMOD8_PIN2_IS_DIG) && !defined(IMOD8_PIN2_IS_XDIG)
#define imod8_GetPin2() 0
#endif
#if !defined(IMOD9_PIN2_IS_DIG) && !defined(IMOD9_PIN2_IS_XDIG)
#define imod9_GetPin2() 0
#endif
#if !defined(IMOD10_PIN2_IS_DIG) && !defined(IMOD10_PIN2_IS_XDIG)
#define imod10_GetPin2() 0
#endif

//Pin 3
#if !defined(IMOD0_PIN3_IS_DIG) && !defined(IMOD0_PIN3_IS_XDIG)
#define imod0_GetPin3() 0
#define imod0_SetPin3() 0
#endif
#if !defined(IMOD1_PIN3_IS_DIG) && !defined(IMOD1_PIN3_IS_XDIG)
#define imod1_ReadPin3() 0
#define imod1_WritePin3() 0
#endif
#if !defined(IMOD2_PIN3_IS_DIG) && !defined(IMOD2_PIN3_IS_XDIG)
#define imod2_ReadPin3() 0
#define imod2_WritePin3() 0
#endif
#if !defined(IMOD3_PIN3_IS_DIG) && !defined(IMOD3_PIN3_IS_XDIG)
#define imod3_GetPin3() 0
#define imod3_SetPin3() 0
#endif
#if !defined(IMOD4_PIN3_IS_DIG) && !defined(IMOD4_PIN3_IS_XDIG)
#define imod4_GetPin3() 0
#define imod4_SetPin3() 0
#endif
#if !defined(IMOD5_PIN3_IS_DIG) && !defined(IMOD5_PIN3_IS_XDIG)
#define imod5_GetPin3() 0
#define imod5_SetPin3() 0
#endif
#if !defined(IMOD6_PIN3_IS_DIG) && !defined(IMOD6_PIN3_IS_XDIG)
#define imod6_GetPin3() 0
#define imod6_SetPin3() 0
#endif
#if !defined(IMOD7_PIN3_IS_DIG) && !defined(IMOD7_PIN3_IS_XDIG)
#define imod7_GetPin3() 0
#endif
#if !defined(IMOD8_PIN3_IS_DIG) && !defined(IMOD8_PIN3_IS_XDIG)
#define imod8_GetPin3() 0
#endif
#if !defined(IMOD9_PIN3_IS_DIG) && !defined(IMOD9_PIN3_IS_XDIG)
#define imod9_GetPin3() 0
#endif
#if !defined(IMOD10_PIN3_IS_DIG) && !defined(IMOD10_PIN3_IS_XDIG)
#define imod10_GetPin3() 0
#endif

//Pin 4
#if !defined(IMOD0_PIN4_IS_DIG) && !defined(IMOD0_PIN4_IS_XDIG)
#define imod0_GetPin4() 0
#define imod0_SetPin4() 0
#endif
#if !defined(IMOD1_PIN4_IS_DIG) && !defined(IMOD1_PIN4_IS_XDIG)
#define imod1_ReadPin4() 0
#define imod1_WritePin4() 0
#endif
#if !defined(IMOD2_PIN4_IS_DIG) && !defined(IMOD2_PIN4_IS_XDIG)
#define imod2_ReadPin4() 0
#define imod2_WritePin4() 0
#endif
#if !defined(IMOD3_PIN4_IS_DIG) && !defined(IMOD3_PIN4_IS_XDIG)
#define imod3_GetPin4() 0
#define imod3_SetPin4() 0
#endif
#if !defined(IMOD4_PIN4_IS_DIG) && !defined(IMOD4_PIN4_IS_XDIG)
#define imod4_GetPin4() 0
#define imod4_SetPin4() 0
#endif
#if !defined(IMOD5_PIN4_IS_DIG) && !defined(IMOD5_PIN4_IS_XDIG)
#define imod5_GetPin4() 0
#define imod5_SetPin4() 0
#endif
#if !defined(IMOD6_PIN4_IS_DIG) && !defined(IMOD6_PIN4_IS_XDIG)
#define imod6_GetPin4() 0
#define imod6_SetPin4() 0
#endif
#if !defined(IMOD7_PIN4_IS_DIG) && !defined(IMOD7_PIN4_IS_XDIG)
#define imod7_GetPin4() 0
#endif
#if !defined(IMOD8_PIN4_IS_DIG) && !defined(IMOD8_PIN4_IS_XDIG)
#define imod8_GetPin4() 0
#endif
#if !defined(IMOD9_PIN4_IS_DIG) && !defined(IMOD9_PIN4_IS_XDIG)
#define imod9_GetPin4() 0
#endif
#if !defined(IMOD10_PIN4_IS_DIG) && !defined(IMOD10_PIN4_IS_XDIG)
#define imod10_GetPin4() 0
#endif

//Pin 5
#if !defined(IMOD0_PIN5_IS_DIG) && !defined(IMOD0_PIN5_IS_XDIG)
#define imod0_GetPin5() 0
#define imod0_SetPin5() 0
#endif
#if !defined(IMOD1_PIN5_IS_DIG) && !defined(IMOD1_PIN5_IS_XDIG)
#define imod1_ReadPin5() 0
#define imod1_WritePin5() 0
#endif
#if !defined(IMOD2_PIN5_IS_DIG) && !defined(IMOD2_PIN5_IS_XDIG)
#define imod2_ReadPin5() 0
#define imod2_WritePin5() 0
#endif
#if !defined(IMOD3_PIN5_IS_DIG) && !defined(IMOD3_PIN5_IS_XDIG)
#define imod3_GetPin5() 0
#define imod3_SetPin5() 0
#endif
#if !defined(IMOD4_PIN5_IS_DIG) && !defined(IMOD4_PIN5_IS_XDIG)
#define imod4_GetPin5() 0
#define imod4_SetPin5() 0
#endif
#if !defined(IMOD5_PIN5_IS_DIG) && !defined(IMOD5_PIN5_IS_XDIG)
#define imod5_GetPin5() 0
#define imod5_SetPin5() 0
#endif
#if !defined(IMOD6_PIN5_IS_DIG) && !defined(IMOD6_PIN5_IS_XDIG)
#define imod6_GetPin5() 0
#define imod6_SetPin5() 0
#endif
#if !defined(IMOD7_PIN5_IS_DIG) && !defined(IMOD7_PIN5_IS_XDIG)
#define imod7_GetPin5() 0
#endif
#if !defined(IMOD8_PIN5_IS_DIG) && !defined(IMOD8_PIN5_IS_XDIG)
#define imod8_GetPin5() 0
#endif
#if !defined(IMOD9_PIN5_IS_DIG) && !defined(IMOD9_PIN5_IS_XDIG)
#define imod9_GetPin5() 0
#endif
#if !defined(IMOD10_PIN5_IS_DIG) && !defined(IMOD10_PIN5_IS_XDIG)
#define imod10_GetPin5() 0
#endif

//Pin 6
#if !defined(IMOD0_PIN6_IS_DIG) && !defined(IMOD0_PIN6_IS_XDIG)
#define imod0_GetPin6() 0
#define imod0_SetPin6() 0
#endif
#if !defined(IMOD1_PIN6_IS_DIG) && !defined(IMOD1_PIN6_IS_XDIG)
#define imod1_ReadPin6() 0
#define imod1_WritePin6() 0
#endif
#if !defined(IMOD2_PIN6_IS_DIG) && !defined(IMOD2_PIN6_IS_XDIG)
#define imod2_ReadPin6() 0
#define imod2_WritePin6() 0
#endif
#if !defined(IMOD3_PIN6_IS_DIG) && !defined(IMOD3_PIN6_IS_XDIG)
#define imod3_GetPin6() 0
#define imod3_SetPin6() 0
#endif
#if !defined(IMOD4_PIN6_IS_DIG) && !defined(IMOD4_PIN6_IS_XDIG)
#define imod4_GetPin6() 0
#define imod4_SetPin6() 0
#endif
#if !defined(IMOD5_PIN6_IS_DIG) && !defined(IMOD5_PIN6_IS_XDIG)
#define imod5_GetPin6() 0
#define imod5_SetPin6() 0
#endif
#if !defined(IMOD6_PIN6_IS_DIG) && !defined(IMOD6_PIN6_IS_XDIG)
#define imod6_GetPin6() 0
#define imod6_SetPin6() 0
#endif
#if !defined(IMOD7_PIN6_IS_DIG) && !defined(IMOD7_PIN6_IS_XDIG)
#define imod7_GetPin6() 0
#endif
#if !defined(IMOD8_PIN6_IS_DIG) && !defined(IMOD8_PIN6_IS_XDIG)
#define imod8_GetPin6() 0
#endif
#if !defined(IMOD9_PIN6_IS_DIG) && !defined(IMOD9_PIN6_IS_XDIG)
#define imod9_GetPin6() 0
#endif
#if !defined(IMOD10_PIN6_IS_DIG) && !defined(IMOD10_PIN6_IS_XDIG)
#define imod10_GetPin6() 0
#endif

//Pin 7
#if !defined(IMOD0_PIN7_IS_DIG) && !defined(IMOD0_PIN7_IS_XDIG)
#define imod0_GetPin7() 0
#define imod0_SetPin7() 0
#endif
#if !defined(IMOD1_PIN7_IS_DIG) && !defined(IMOD1_PIN7_IS_XDIG)
#define imod1_ReadPin7() 0
#define imod1_WritePin7() 0
#endif
#if !defined(IMOD2_PIN7_IS_DIG) && !defined(IMOD2_PIN7_IS_XDIG)
#define imod2_ReadPin7() 0
#define imod2_WritePin7() 0
#endif
#if !defined(IMOD3_PIN7_IS_DIG) && !defined(IMOD3_PIN7_IS_XDIG)
#define imod3_GetPin7() 0
#define imod3_SetPin7() 0
#endif
#if !defined(IMOD4_PIN7_IS_DIG) && !defined(IMOD4_PIN7_IS_XDIG)
#define imod4_GetPin7() 0
#define imod4_SetPin7() 0
#endif
#if !defined(IMOD5_PIN7_IS_DIG) && !defined(IMOD5_PIN7_IS_XDIG)
#define imod5_GetPin7() 0
#define imod5_SetPin7() 0
#endif
#if !defined(IMOD6_PIN7_IS_DIG) && !defined(IMOD6_PIN7_IS_XDIG)
#define imod6_GetPin7() 0
#define imod6_SetPin7() 0
#endif
#if !defined(IMOD7_PIN7_IS_DIG) && !defined(IMOD7_PIN7_IS_XDIG)
#define imod7_GetPin7() 0
#endif
#if !defined(IMOD8_PIN7_IS_DIG) && !defined(IMOD8_PIN7_IS_XDIG)
#define imod8_GetPin7() 0
#endif
#if !defined(IMOD9_PIN7_IS_DIG) && !defined(IMOD9_PIN7_IS_XDIG)
#define imod9_GetPin7() 0
#endif
#if !defined(IMOD10_PIN7_IS_DIG) && !defined(IMOD10_PIN7_IS_XDIG)
#define imod10_GetPin7() 0
#endif

//Pin 8
#if !defined(IMOD0_PIN8_IS_DIG) && !defined(IMOD0_PIN8_IS_XDIG)
#define imod0_GetPin8() 0
#define imod0_SetPin8() 0
#endif
#if !defined(IMOD1_PIN8_IS_DIG) && !defined(IMOD1_PIN8_IS_XDIG)
#define imod1_ReadPin8() 0
#define imod1_WritePin8() 0
#endif
#if !defined(IMOD2_PIN8_IS_DIG) && !defined(IMOD2_PIN8_IS_XDIG)
#define imod2_ReadPin8() 0
#define imod2_WritePin8() 0
#endif
#if !defined(IMOD3_PIN8_IS_DIG) && !defined(IMOD3_PIN8_IS_XDIG)
#define imod3_GetPin8() 0
#define imod3_SetPin8() 0
#endif
#if !defined(IMOD4_PIN8_IS_DIG) && !defined(IMOD4_PIN8_IS_XDIG)
#define imod4_GetPin8() 0
#define imod4_SetPin8() 0
#endif
#if !defined(IMOD5_PIN8_IS_DIG) && !defined(IMOD5_PIN8_IS_XDIG)
#define imod5_GetPin8() 0
#define imod5_SetPin8() 0
#endif
#if !defined(IMOD6_PIN8_IS_DIG) && !defined(IMOD6_PIN8_IS_XDIG)
#define imod6_GetPin8() 0
#define imod6_SetPin8() 0
#endif
#if !defined(IMOD7_PIN8_IS_DIG) && !defined(IMOD7_PIN8_IS_XDIG)
#define imod7_GetPin8() 0
#endif
#if !defined(IMOD8_PIN8_IS_DIG) && !defined(IMOD8_PIN8_IS_XDIG)
#define imod8_GetPin8() 0
#endif
#if !defined(IMOD9_PIN8_IS_DIG) && !defined(IMOD9_PIN8_IS_XDIG)
#define imod9_GetPin8() 0
#endif
#if !defined(IMOD10_PIN8_IS_DIG) && !defined(IMOD10_PIN8_IS_XDIG)
#define imod10_GetPin8() 0
#endif

//Pin 9
#if !defined(IMOD0_PIN9_IS_DIG) && !defined(IMOD0_PIN9_IS_XDIG)
#define imod0_GetPin9() 0
#define imod0_SetPin9() 0
#endif
#if !defined(IMOD1_PIN9_IS_DIG) && !defined(IMOD1_PIN9_IS_XDIG)
#define imod1_ReadPin9() 0
#define imod1_WritePin9() 0
#endif
#if !defined(IMOD2_PIN9_IS_DIG) && !defined(IMOD2_PIN9_IS_XDIG)
#define imod2_ReadPin9() 0
#define imod2_WritePin9() 0
#endif
#if !defined(IMOD3_PIN9_IS_DIG) && !defined(IMOD3_PIN9_IS_XDIG)
#define imod3_GetPin9() 0
#define imod3_SetPin9() 0
#endif
#if !defined(IMOD4_PIN9_IS_DIG) && !defined(IMOD4_PIN9_IS_XDIG)
#define imod4_GetPin9() 0
#define imod4_SetPin9() 0
#endif
#if !defined(IMOD5_PIN9_IS_DIG) && !defined(IMOD5_PIN9_IS_XDIG)
#define imod5_GetPin9() 0
#define imod5_SetPin0() 0
#endif
#if !defined(IMOD6_PIN9_IS_DIG) && !defined(IMOD6_PIN9_IS_XDIG)
#define imod6_GetPin9() 0
#define imod6_SetPin9() 0
#endif
#if !defined(IMOD7_PIN9_IS_DIG) && !defined(IMOD7_PIN9_IS_XDIG)
#define imod7_GetPin9() 0
#endif
#if !defined(IMOD8_PIN9_IS_DIG) && !defined(IMOD8_PIN9_IS_XDIG)
#define imod8_GetPin9() 0
#endif
#if !defined(IMOD9_PIN9_IS_DIG) && !defined(IMOD9_PIN9_IS_XDIG)
#define imod9_GetPin9() 0
#endif
#if !defined(IMOD10_PIN9_IS_DIG) && !defined(IMOD10_PIN9_IS_XDIG)
#define imod10_GetPin9() 0
#endif

//Pin 10
#if !defined(IMOD0_PIN10_IS_DIG) && !defined(IMOD0_PIN10_IS_XDIG)
#define imod0_GetPin10() 0
#define imod0_SetPin10() 0
#endif
#if !defined(IMOD1_PIN10_IS_DIG) && !defined(IMOD1_PIN10_IS_XDIG)
#define imod1_ReadPin10() 0
#define imod1_WritePin10() 0
#endif
#if !defined(IMOD2_PIN10_IS_DIG) && !defined(IMOD2_PIN10_IS_XDIG)
#define imod2_ReadPin10() 0
#define imod2_WritePin10() 0
#endif
#if !defined(IMOD3_PIN10_IS_DIG) && !defined(IMOD3_PIN10_IS_XDIG)
#define imod3_GetPin10() 0
#define imod3_SetPin10() 0
#endif
#if !defined(IMOD4_PIN10_IS_DIG) && !defined(IMOD4_PIN10_IS_XDIG)
#define imod4_GetPin10() 0
#define imod4_SetPin10() 0
#endif
#if !defined(IMOD5_PIN10_IS_DIG) && !defined(IMOD5_PIN10_IS_XDIG)
#define imod5_GetPin10() 0
#define imod5_SetPin10() 0
#endif
#if !defined(IMOD6_PIN10_IS_DIG) && !defined(IMOD6_PIN10_IS_XDIG)
#define imod6_GetPin10() 0
#define imod6_SetPin10() 0
#endif
#if !defined(IMOD7_PIN10_IS_DIG) && !defined(IMOD7_PIN10_IS_XDIG)
#define imod7_GetPin10() 0
#endif
#if !defined(IMOD8_PIN10_IS_DIG) && !defined(IMOD8_PIN10_IS_XDIG)
#define imod8_GetPin10() 0
#endif
#if !defined(IMOD9_PIN10_IS_DIG) && !defined(IMOD9_PIN10_IS_XDIG)
#define imod9_GetPin10() 0
#endif
#if !defined(IMOD10_PIN10_IS_DIG) && !defined(IMOD10_PIN10_IS_XDIG)
#define imod10_GetPin10() 0
#endif






/////////////////////////////////////////////////
// Functions
void ERROR_passed_invalid_port_to_function___imod_ConfDirPin2(void);
#define imod_ConfDirPin2_INVALID_PORT() ERROR_passed_invalid_port_to_function___imod_ConfDirPin2()
#define imod_ConfDirPin2(port,dir)          \
    ((port==1)?imod1_ConfDirPin2(dir) :     \
    ((port==2)?imod2_ConfDirPin2(dir) :     \
    imod_ConfDirPin2_INVALID_PORT()))


/*
#define imod_GetPin2(port)                  \
    ((port==1)?imod1_ReadPin2(dir) :         \
    ((port==2)?imod2_ReadPin2(dir) :         \
    imod_ConfDirPin2_INVALID_PORT()))
*/

/**
 * Get the value of Pin 3 for the given iMod port. This is the MACRO implementation,
 * and should ONLY be called with port being a literal constant! If port is not a
 * literal constant (variable), use the non macro version -> imod_GetPin3().
 *
 * @param port The iMod port, a value from 0 to n. MUST be a literal constant
 *
 * @return The value of the requested iMod Port's pin 3
 */
#define imod_GetPin3_MACRO(port)    \
    ((port==0)?imod0_GetPin3() :    \
    ((port==1)?imod1_ReadPin3() :    \
    ((port==2)?imod2_ReadPin3() :    \
    ((port==3)?imod3_GetPin3() :    \
    ((port==4)?imod4_GetPin3() :    \
    ((port==5)?imod5_GetPin3() :    \
    ((port==6)?imod6_GetPin3() :    \
    ((port==7)?imod7_GetPin3() :    \
    ((port==8)?imod8_GetPin3() :    \
    ((port==9)?imod9_GetPin3() :    \
    ((port==10)?imod10_GetPin3() :  \
    0)))))))))))

//Alternative method using function
BOOL imod_GetPin3(BYTE port);

BOOL imod_GetPin5(BYTE port);

#endif
