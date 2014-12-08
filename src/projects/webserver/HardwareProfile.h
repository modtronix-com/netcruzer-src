 /**
 * @brief           Contains hardware specific defines and code.
 * @file            HardwareProfile.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section hardwareprofile_desc Description
 *****************************************
 * This module includes hardware specific files. It requires defines that identify the current
 * hardware. They are normally defined in the MPLAB X Project file. For example BRD_SBC66EC_R2.
 *
 *********************************************************************
 * @section hardwareprofile_lic Software License Agreement
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
 * 2010-11-10, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef _HARDWARWEPROFILE_H_
#define _HARDWARWEPROFILE_H_

//Define the cruzer project. For future, will use this same source code for:
// - WebCruzer, is the current Webserver
// - DroidCruzer, is a project for controlling the SBC board via an Android tablet or phone
//#define PRJ_WEBCRUZER         //Define in MPLAB Project
//#define PRJ_DROIDCRUZER       //Define in MPLAB Project

//Includes required for Webserver
#if defined(PRJ_WEBCRUZER)
#define HAS_WEBSERVER
#endif

//Includes required for Android USB Host
#if defined(PRJ_DROIDCRUZER)
#define HAS_USBHOST
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "nz_genericTypeDefs.h"


//Project Specific Defines. These are defines that are the same for all target hardware.
//Place hardware specific defines in "Configs/HWP_BOARDNAME.h" file for target board
#if defined(MODBUS_DEMO1)
    //Define any expansion boards, is a XBRD_xxx define (see nz_board.h for supported boards).
    #define XBRD_PT66DIN6_R1    //This demo uses the PT66DIN6 expansion daughter board
    #define IMOD1_IS_IM2BL      //The im2BL iMod module is plugged into iMod port 1 of the PT66DIN6
    #include "demos/Modbus_Demo1/projdefs.h"
#elif defined(PT66DIN6_IO_DEMO1)
    //Define any expansion boards, is a XBRD_xxx define (see nz_board.h for supported boards).
    #define XBRD_PT66DIN6_R1    //This demo uses the PT66DIN6 expansion daughter board
    #define IMOD1_IS_IM2BL      //The im2BL iMod module is plugged into iMod port 1 of the PT66DIN6
    #include "demos/PT66DIN6_IO_Demo1/projdefs.h"
#elif defined(DB66DEV1_DEMO1)
    //Define any expansion boards, is a XBRD_xxx define (see nz_board.h for supported boards).
    #define XBRD_DB66DEV1_R1    //This demo uses the DB66DEV1 expansion daughter board
    #include "demos/DB66DEV1_Demo1/projdefs.h"
#elif defined(SENS_DHT22_DEMO1)
    //Define any expansion boards, is a XBRD_xxx define (see nz_board.h for supported boards).
    #define XBRD_DB66DEV1_R1    //This demo uses the DB66DEV1 expansion daughter board
    #include "demos/Sens_DHT22_Demo/projdefs.h"
#elif defined(UART_TO_UDP_DEMO1)
    //Define any expansion boards, is a XBRD_xxx define (see nz_board.h for supported boards).
    #define XBRD_PT66EI_10P_R2  //This demo uses the PT66EI-10P expansion board
    #include "demos/UART_to_UDP/projdefs.h"
#elif defined(CUSTOM_PROJECT)
    #include "CustomFiles/projdefs.h"
    #include "CustomFiles/appConfig_custom.h"
#else
    #include "projdefs.h"
#endif

#include "nz_board.h"       //Includes correct board for this configuration


//Must be declared after nz_boards.h and projdefs.h.
#include "nz_netcruzer.h"        //Netcruzer functions


/////////////////////////////////////////////////
//Hardware Specific Defines. Include "Configs/HWP_BOARDNAME.h" file for target board
#if defined(BRD_SBC66EC_R1)
    #include "Configs/HWP_SBC66EC-R1.h"
#elif defined(BRD_SBC66EC_R2)
    #include "Configs/HWP_SBC66EC-R2.h"
#elif defined(BRD_SBC66ECL_R2)
    #include "Configs/HWP_SBC66ECL-R2.h"
#else
    #error "No extended HWP .h included.  Add the appropriate compiler macro to the MPLAB project."
#endif

//All "Serial Data Port" modules require this include. Can not put it in the "Serial Data Port" module (nz_serI2C.c for example),
//seeing that they are part of the general purpose "Netcruzer Library".
#if defined(THIS_IS_A_SERPORT)
#include "nz_serDataPorts.h"
#endif

#endif

