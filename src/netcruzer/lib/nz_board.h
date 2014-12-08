/**
 * @brief           Netcruzer File for including correct board Include file
 * @file            nz_board.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section nz_board_desc Description
 *****************************************
 * This module includes hardware specific defines for a project.
 *
 **********************************************************************
 * @section nz_board_lic Software License Agreement
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
#ifndef NZ_BOARD_H
#define NZ_BOARD_H


/// Macros for input and output pins
#ifndef INPUT_PIN
#define INPUT_PIN   1
#endif
#ifndef OUTPUT_PIN
#define OUTPUT_PIN  0
#endif



/////////////////////////////////////////////////
//Hardware Specific Defines - first include the Processor Board. To include one of these file with the project, add
//the BRD_XX define in the MPLAB project (macros), or in the HardwareProfile.h file (before nz_board.h include)
#if defined(BRD_SBC66EC_R1)
    #include "brd_sbc66ec-r1.h"
#elif defined(BRD_SBC66EC_R2)
    #include "brd_sbc66ec-r2.h"
#elif defined(BRD_SBC66ECL_R2)
    #include "brd_sbc66ecl-r2.h"
#elif defined(BRD_SBC66UAL_R1)
    #include "brd_sbc66ual-r1.h"
#elif defined(BRD_SBC66ZL_R1)
    #include "brd_sbc66zl-r1.h"
#elif defined(BRD_SBC32UL_R1)
    #include "brd_sbc32ul-r1.h"
#else
    #error "No extended HWP .h included.  Add the appropriate compiler macro to the MPLAB project."
#endif


/////////////////////////////////////////////////
//Hardware Specific Defines for any add-on boards. To include one of these file with the project, add
//the BRD_XX define in the MPLAB project (macros), or in the HardwareProfile.h file (before nz_board.h include)
#if defined(XBRD_PT66DIN6_R1)
    #define XBRD_PT66DIN6_REV1
    #include "nz_pt66din6.h"        //Board define for PT66DIN6 board
    #include "nz_imod.h"            //This expansion board has iMod ports
#elif defined(XBRD_PT66ECI_R1)
    #define XBRD_PT66ECI_REV1
    #include "nz_pt66eci.h"         //Board define for PT66ECI prototype board
    #include "nz_imod.h"            //This expansion board has iMod ports
#elif defined(XBRD_PT66EI_10P_R1)
    #define XBRD_PT66EI_REV1
    #include "nz_pt66ei10p.h"       //Board define for PT66EI prototype board
    #include "nz_imod.h"            //This expansion board has iMod ports
#elif defined(XBRD_PT66EI_10P_R2)
    #define XBRD_PT66EI_REV2
    #include "nz_pt66ei10p.h"       //Board define for PT66EI prototype board
    #include "nz_imod.h"            //This expansion board has iMod ports
#elif defined(XBRD_PT66EI_24P_R1)
    #define XBRD_PT66EI_REV1
    #include "nz_pt66ei24p.h"       //Board define for PT66EI prototype board
    #include "nz_imod.h"            //This expansion board has iMod ports
#elif defined(XBRD_PT66EI_24P_R2)
    #define XBRD_PT66EI_REV2
    #include "nz_pt66ei24p.h"       //Board define for PT66EI prototype board
    #include "nz_imod.h"            //This expansion board has iMod ports
#elif defined(XBRD_DB66DEV1_R1)
    #define XBRD_DB66DEV1_REV1
    #include "nz_db66dev1.h"        //Board define for DB66DEV1 development board
#endif


/////////////////////////////////////////////////
//Define defaults for LEDs and Buttons not present on this board, but that are often used by Microchip project
//included with MAL. To NOT include with the project, add BRD_NO_DEFAULTS define in the MPLAB project (macros), or
//in the HardwareProfile.h file (before nz_board.h include)
#ifndef BRD_NO_DEFAULTS
#include "nz_board_defaults.h"
#endif


#define IOPORT_X0    0
#define IOPORT_X1    1
#define IOPORT_X2    2
#define IOPORT_X3    3
#define IOPORT_X4    4
#define IOPORT_X5    5
#define IOPORT_X6    6
#define IOPORT_X7    7
#define IOPORT_X8    8
#define IOPORT_X9    9
#define IOPORT_X10   10
#define IOPORT_X11   11
#define IOPORT_SD    12
#define IOPORT_SC    13
#define IOPORT_T0    20
#define IOPORT_T1    21
#define IOPORT_T2    22
#define IOPORT_T3    23
#define IOPORT_S0    24
#define IOPORT_S1    25
#define IOPORT_T4    26
#define IOPORT_T5    27
#define IOPORT_T6    28
#define IOPORT_T7    29
#define IOPORT_Y0    30
#define IOPORT_Y1    31
#define IOPORT_Y2    32
#define IOPORT_Y3    33
#define IOPORT_Y4    34
#define IOPORT_Y5    35
#define IOPORT_Y6    36
#define IOPORT_Y7    37
#define IOPORT_Y8    38
#define IOPORT_Y9    39
#define IOPORT_Y10   40
#define IOPORT_Y11   41

#endif  //#ifndef NZ_BOARD_H
