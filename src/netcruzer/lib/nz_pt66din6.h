/**
 * @brief           Contains hardware specific defines for PT66DIN6 board.
 * @file            nz_pt66din6.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section nz_pt66din6_desc Description
 *****************************************
 * This file includes hardware specific defines for the PT66DIN6 board. This board
 * has 6 iMod ports allowing many different iMod modules to be added. The pins of the
 * iMod ports can be configured, read and written using functions defined in this
 * file.
 *
 * @subsection nz_pt66din6_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // ---------- nz_pt66din6 Configuration (from nz_pt66din6.h) -----------
 // *********************************************************************
  @endcode
 *
 * @subsection nz_pt66din6_usage Usage
 *****************************************
 * To use the PT66DIN6 with iMod modules in a project, the following must be done:
 * - Copy the @ref nz_pt66din6_conf "Configuration" section above (from nz_pt66din6.h)
 *   to the projdefs.h file.
 * - In this "Configuration" section, change any default values if required (if default
 *   values should be used, define is not required in projdefs.h).
 * - Add nz_pt66din6.c and nz_imod.c to the MPLAB project, and include nz_pt66din6.h in
 *   the c file it is used in
 * - In HardwareProfile.h, add defines for this board, and any iMod modules used.
 *
 * For example, if the im2BL (2 buttons and LEDs) is mounted in iMod port 1, and the
 * im4MOS (4 MOSFET outputs) is mounted in iMod port 2, the following code is required:
 @code
//Define any expansion boards, is a XBRD_xxx define (see nz_board.h for supported boards).
#define XBRD_PT66DIN6_R1    //This demo uses the PT66DIN6 expansion daughter board

//Define iMod modules
#define IMOD1_IS_IM2BL      //iMod port 1 has a im2BL module
#define IMOD2_IS_IM4MOS     //iMod port 2 has a im4MOD module
 @endcode
 *
 * This code will make iMod specific functions available for the defined modules.
 * These functions will appear in MPLAB X as code completion suggestions if the module
 * name is types. For the example above, if "im2bl_" is typed in the editor, and "Control-Space"
 * pressed (code completion), all available im2BL functions for iMod port 1 are shown, like:
 @code
    im2bl_ReadButton1Imod1()
    im2bl_ReadButton2Imod1()
    im2bl_WriteLed1Imod1()
    im2bl_WriteLed2Imod1()
    im2bl_Init()
 @endcode
 *
 * To configure an iMod port pin as an input or output, the <i>ConfDirPin</i> functions
 * are provided. For example:
 @code
    imod1_ConfDirPin3(INPUT_PIN);  //Configure iMod port 1, pin 3 as an output
    imod2_ConfDirPin2(OUTPUT_PIN); //Configure iMod port 2, pin 2 is an output
 @endcode
 *
 * To write or read to/from an iMod port pin, the <i>WritePin</i> and <i>ReadPin</i>
 * functions are provided. For example:
 @code
    imod1_ReadPin2();   //Read iMod port 1 pin 2
    imod2_WritePin5(1); //Set iMod port 2 pin 5
 @endcode
 *
 * To enable or disable pull-up and pull-down resistors on iMod port pins, the <i>ConfPullupPin</i>
 * and <i>ConfPulldownPin</i> functions are provided. For example:
 @code
    imod2_ConfPulldownPin2(0);  //Disable pull-down resistors on iMod 2 pin 2
    imod1_ConfPullupPin4(1);    //Enable pull-up resistors on iMod 1 pin 4
 @endcode
 *
 **********************************************************************
 * @section nz_pt66din6_lic Software License Agreement
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
#ifndef PT66DIN6_H
#define PT66DIN6_H

#if defined(XBRD_PT66DIN6)

// *********************************************************************
// ------------------------- iMod Port 1 Defines -----------------------
// *********************************************************************
#define HAS_IMOD1

#define IMOD1_PIN2
#define IMOD1_PIN2_IS_DIG           //Digital I/O port of CPU
#define IMOD1_PIN2_IS_PPS
#ifdef PIN_08
#define PIN_IMOD1_P2                PIN_08
#define LAT_IMOD1_P2                LAT_08
#define DIR_IMOD1_P2                DIR_08
#define PULLUP_IMOD1_P2             PULLUP_08
#define PULLDOWN_IMOD1_P2            PULLDOWN_08
#define PPS_LAT_IMOD1_P2            PPS_OUT_08
#define PPS_IN_IMOD1_P2             PPS_IN_08
#define imod1_ConfDirPin2(dir)      (DIR_IMOD1_P2 = dir)
#define imod1_WritePin2(val)        (LAT_IMOD1_P2 = val)
#define imod1_ReadPin2()            (PIN_IMOD1_P2)
#define imod1_ConfPullupPin2(val)   (PULLUP_IMOD1_P2 = val)
#define imod1_ConfPulldownPin2(val) (PULLDOWN_IMOD1_P2 = val)
#endif

#define IMOD1_PIN3
#define IMOD1_PIN3_IS_DIG
#define IMOD1_PIN3_IS_PPS
#ifdef PIN_07
#define PIN_IMOD1_P3                PIN_07
#define LAT_IMOD1_P3                LAT_07
#define DIR_IMOD1_P3                DIR_07
#define PULLUP_IMOD1_P3             PULLUP_07
#define PULLDOWN_IMOD1_P3            PULLDOWN_07
#define PPS_LAT_IMOD1_P3            PPS_OUT_07
#define PPS_IN_IMOD1_P3             PPS_IN_07
#define imod1_ConfDirPin3(dir)      (DIR_IMOD1_P3 = dir)
#define imod1_WritePin3(val)        (LAT_IMOD1_P3 = val)
#define imod1_ReadPin3()            (PIN_IMOD1_P3)
#define imod1_ConfPullupPin3(val)   (PULLUP_IMOD1_P3 = val)
#define imod1_ConfPulldownPin3(val) (PULLDOWN_IMOD1_P3 = val)
#endif

#define IMOD1_PIN4
#define IMOD1_PIN4_IS_DIG
#define IMOD1_PIN4_IS_PPS
#ifdef PIN_06
#define PIN_IMOD1_P4                PIN_06
#define LAT_IMOD1_P4                LAT_06
#define DIR_IMOD1_P4                DIR_06
#define PULLUP_IMOD1_P4             PULLUP_06
#define PULLDOWN_IMOD1_P4            PULLDOWN_06
#define PPS_LAT_IMOD1_P4            PPS_OUT_06
#define PPS_IN_IMOD1_P4             PPS_IN_06
#define imod1_ConfDirPin4(dir)      (DIR_IMOD1_P4 = dir)
#define imod1_WritePin4(val)        (LAT_IMOD1_P4 = val)
#define imod1_ReadPin4()            (PIN_IMOD1_P4)
#define imod1_ConfPullupPin4(val)   (PULLUP_IMOD1_P4 = val)
#define imod1_ConfPulldownPin4(val) (PULLDOWN_IMOD1_P4 = val)
#endif

#define IMOD1_PIN5
#define IMOD1_PIN5_IS_PPS
#define IMOD1_PIN5_IS_DIG
#define IMOD1_PIN5_IS_ANALOG
#ifdef PIN_01
#define PIN_IMOD1_P5                PIN_01
#define LAT_IMOD1_P5                LAT_01
#define DIR_IMOD1_P5                DIR_01
#define PULLUP_IMOD1_P5             PULLUP_01
#define PULLDOWN_IMOD1_P5            PULLDOWN_01
#define PPS_LAT_IMOD1_P5            PPS_OUT_01
#define PPS_IN_IMOD1_P5             PPS_IN_01
#define imod1_ConfDirPin5(dir)      (DIR_IMOD1_P5 = dir)
#define imod1_WritePin5(val)        (LAT_IMOD1_P5 = val)
#define imod1_ReadPin5()            (PIN_IMOD1_P5)
#define imod1_ConfPullupPin5(val)   (PULLUP_IMOD1_P5 = val)
#define imod1_ConfPulldownPin5(val) (PULLDOWN_IMOD1_P5 = val)
#endif

#define IMOD1_PIN6
#define IMOD1_PIN6_IS_DIG
#define IMOD1_PIN6_IS_ANALOG
#ifdef PIN_03
#define PIN_IMOD1_P6                PIN_03
#define LAT_IMOD1_P6                LAT_03
#define DIR_IMOD1_P6                DIR_03
#define PULLUP_IMOD1_P6             PULLUP_03
#define PULLDOWN_IMOD1_P6            PULLDOWN_03
#define imod1_ConfDirPin6(dir)      (DIR_IMOD1_P6 = dir)
#define imod1_WritePin6(val)        (LAT_IMOD1_P6 = val)
#define imod1_ReadPin6()            (PIN_IMOD1_P6)
#define imod1_ConfPullupPin6(val)   (PULLUP_IMOD1_P6 = val)
#define imod1_ConfPulldownPin6(val) (PULLDOWN_IMOD1_P6 = val)
#endif

#define IMOD1_PIN7
#define IMOD1_PIN7_IS_XDIG          //Port expander Digital pin
#define IMOD1_PIN7_IS_XANALOG       //Port expander Analog pin
//Add implementations for following functions. This will prevent default implementation in nz_imod.h from being used:
// imod1_ConfDirPin7()
// imod1_WritePin7()
// imod1_ReadPin7()
// imod1_ConfPullupPin7()
// imod1_ConfPulldownPin7()


extern BYTE ERROR_passed_invalid_port_to_function___imod1_Pin___Must_be_2_or_3_or_4_or_5;
#define imod1_Pin_INVALID ERROR_passed_invalid_port_to_function___imod1_Pin___Must_be_2_or_3_or_4_or_5
#define imod1_Pin(p)            \
    ((p==2)?PIN_08 :            \
    ((p==3)?PIN_07 :            \
    ((p==4)?PIN_06 :            \
    ((p==5)?PIN_04 :            \
    imod1_Pin_INVALID ))))      \



// *********************************************************************
// ------------------------- iMod Port 2 Defines -----------------------
// *********************************************************************
#define HAS_IMOD2

#define IMOD2_PIN2
#define IMOD2_PIN2_IS_DIG
#define IMOD2_PIN2_IS_ANALOG
#define IMOD2_PIN2_IS_PPS
#define PIN_IMOD2_P2                PIN_01
#define LAT_IMOD2_P2                LAT_01
#define DIR_IMOD2_P2                DIR_01
#define PULLUP_IMOD2_P2             PULLUP_01
#define PULLDOWN_IMOD2_P2            PULLDOWN_01
#define PPS_LAT_IMOD2_P2            PPS_OUT_01
#define PPS_IN_IMOD2_P2             PPS_IN_01
#define imod2_ConfDirPin2(dir)      (DIR_IMOD2_P2 = dir)
#define imod2_WritePin2(val)        (LAT_IMOD2_P2 = val)
#define imod2_ReadPin2()            (PIN_IMOD2_P2)
#define imod2_ConfPullupPin2(val)   (PULLUP_IMOD2_P2 = val)
#define imod2_ConfPulldownPin2(val) (PULLDOWN_IMOD2_P2 = val)

#define IMOD2_PIN3
#define IMOD2_PIN3_IS_DIG
#define IMOD2_PIN3_IS_PPS
#define PIN_IMOD2_P3                PIN_39
#define LAT_IMOD2_P3                LAT_39
#define DIR_IMOD2_P3                DIR_39
#define PULLUP_IMOD2_P3             PULLUP_39
#define PULLDOWN_IMOD2_P3            PULLDOWN_39
#define PPS_LAT_IMOD2_P3            PPS_OUT_39
#define PPS_IN_IMOD2_P3             PPS_IN_39
#define imod2_ConfDirPin3(dir)      (DIR_IMOD2_P3 = dir)
#define imod2_WritePin3(val)        (LAT_IMOD2_P3 = val)
#define imod2_ReadPin3()            (PIN_IMOD2_P3)
#define imod2_ConfPullupPin3(val)   (PULLUP_IMOD2_P3 = val)
#define imod2_ConfPulldownPin3(val) (PULLDOWN_IMOD2_P3 = val)

#define IMOD2_PIN4
#define IMOD2_PIN4_IS_DIG
#define IMOD2_PIN4_IS_PPS
#define PIN_IMOD2_P4                PIN_38
#define LAT_IMOD2_P4                LAT_38
#define DIR_IMOD2_P4                DIR_38
#define PULLUP_IMOD2_P4             PULLUP_38
#define PULLDOWN_IMOD2_P4            PULLDOWN_38
#define PPS_LAT_IMOD2_P4            PPS_OUT_38
#define PPS_IN_IMOD2_P4             PPS_IN_38
#define imod2_ConfDirPin4(dir)      (DIR_IMOD2_P4 = dir)
#define imod2_WritePin4(val)        (LAT_IMOD2_P4 = val)
#define imod2_ReadPin4()            (PIN_IMOD2_P4)
#define imod2_ConfPullupPin4(val)   (PULLUP_IMOD2_P4 = val)
#define imod2_ConfPulldownPin4(val) (PULLDOWN_IMOD2_P4 = val)

#define IMOD2_PIN5
#define IMOD2_PIN5_IS_DIG
#define IMOD2_PIN5_IS_ANALOG
#define IMOD2_PIN5_IS_PPS
#define PIN_IMOD2_P5                PIN_02
#define LAT_IMOD2_P5                LAT_02
#define DIR_IMOD2_P5                DIR_02
#define PULLUP_IMOD2_P5             PULLUP_02
#define PULLDOWN_IMOD2_P5            PULLDOWN_02
#define PPS_LAT_IMOD2_P5            PPS_OUT_02
#define PPS_IN_IMOD2_P5             PPS_IN_02
#define imod2_ConfDirPin5(dir)      (DIR_IMOD2_P5 = dir)
#define imod2_WritePin5(val)        (LAT_IMOD2_P5 = val)
#define imod2_ReadPin5()            (PIN_IMOD2_P5)
#define imod2_ConfPullupPin5(val)   (PULLUP_IMOD2_P5 = val)
#define imod2_ConfPulldownPin5(val) (PULLDOWN_IMOD2_P5 = val)

#define IMOD2_PIN6
#define IMOD2_PIN6_IS_DIG
#define IMOD2_PIN6_IS_ANALOG
#define PIN_IMOD2_P6                PIN_32
#define LAT_IMOD2_P6                LAT_32
#define DIR_IMOD2_P6                DIR_32
#define PULLUP_IMOD2_P6             PULLUP_32
#define PULLDOWN_IMOD2_P6            PULLDOWN_32
#define imod2_ConfDirPin6(dir)      (DIR_IMOD2_P6 = dir)
#define imod2_WritePin6(val)        (LAT_IMOD2_P6 = val)
#define imod2_ReadPin6()            (PIN_IMOD2_P6)
#define imod2_ConfPullupPin6(val)   (PULLUP_IMOD2_P6 = val)
#define imod2_ConfPulldownPin6(val) (PULLDOWN_IMOD2_P6 = val)

#define IMOD2_PIN7
#define IMOD2_PIN7_IS_XDIG          //Port expander Digital pin
#define IMOD2_PIN7_IS_XANALOG       //Port expander Analog pin
//Add implementations for following functions. This will prevent default implementation in nz_imod.h from being used:
// imod2_ConfDirPin7()
// imod2_WritePin7()
// imod2_ReadPin7()
// imod2_ConfPullupPin7()
// imod2_ConfPulldownPin7()



// iMod 3 ///////////////////////////////////////
#define HAS_IMOD3


// iMod 4 ///////////////////////////////////////
#define HAS_IMOD4


// iMod 5 ///////////////////////////////////////
#define HAS_IMOD5


// iMod 6 ///////////////////////////////////////
#define HAS_IMOD6





/**
 * Initializes the PT66DIN6 board
 */
void pt66din6_Init();


/**
 * Service function. Must be called every 1ms
 */
void pt66din6_Service();


#endif  //#if defined(XBRD_PT66DIN6)

#endif
