 /**
 * @brief           Contains hardware specific defines and code.
 * @file            HardwareProfile.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section description Description
 *****************************************
 * This module includes hardware specific files. It requires defines that identify the current
 * hardware. They are normally defined in the MPLAB X Project file. For example BRD_SBC66EC_R2.
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
 * 2012-08-06, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef _HARDWARWEPROFILE_H_
#define _HARDWARWEPROFILE_H_

#include <p24fxxxx.h>
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "nz_genericTypeDefs.h"


//Define any expansion boards, is a XBRD_xxx define (see nz_board.h for supported boards).
#define XBRD_DB66DEV1_R1    //This demo uses the DB66DEV1 expansion daughter board

#include "nz_board.h"       //Includes correct board for this configuration

//Project Specific Defines. These are defines that are the same for all target hardware.
//Place hardware specific defines in "Configs/HWP_BOARDNAME.h" file for target board
#include "projdefs.h"

//Must be declared after nz_boards.h and projdefs.h.
#include "nz_netcruzer.h"        //Netcruzer functions

//Other defines and include files
#if defined(INCLUDE_NETCRUZER_HEADERS)
    //No Netcruzer headers for this project
#endif


/////////////////////////////////////////////////
//Hardware Specific Defines
//None


#endif

