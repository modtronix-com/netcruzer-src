/********************************************************************
 FileName:      HardwareProfile.h
 Dependencies:  See INCLUDES section
 Processor:     PIC18, PIC24, or PIC32 USB Microcontrollers
 Hardware:      The code is natively intended to be used on the 
                  following hardware platforms: 
                    PICDEM™ FS USB Demo Board
                    PIC18F46J50 FS USB Plug-In Module
                    PIC18F87J50 FS USB Plug-In Module
                    Explorer 16 + PIC24 or PIC32 USB PIMs
                    PIC24F Starter Kit
                    Low Pin Count USB Development Kit
                  The firmware may be modified for use on other USB 
                    platforms by editing this file (HardwareProfile.h)
 Compiler:  	Microchip C18 (for PIC18), C30 (for PIC24), 
                  or C32 (for PIC32)
 Company:       Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company”) for its PIC® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
  Rev   Date         Description
  1.0   11/19/2004   Initial release
  2.1   02/26/2007   Updated for simplicity and to use common
                     coding style
  2.3   09/15/2008   Broke out each hardware platform into its own
                     "HardwareProfile - xxx.h" file
********************************************************************/
#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "nz_genericTypeDefs.h"

//Project Specific Defines. These are defines that are the same for all target hardware.
//Place hardware specific defines in "Configs/HWP_BOARDNAME.h" file for target board
#include "projdefs.h"

//#include "nz_board.h"       //Includes correct board for this configuration

//For MPLAB X, it does not seem to recognize macros in project
//#ifndef BRD_SBC66EC_R2
//#define BRD_SBC66EC_R2
//#endif

// These definitions are set in the MPLAB Project settings.  If you are starting
// a new project, you should start by modifying one of the pre-existing .mcp 
// files.  To modify the macro used, in MPLAB IDE, click on Project -> Build 
// Options... -> Project -> MPLAB XXX C Compiler -> Preprocessor Macros -> 
// Add.... Note that you may also have to add this macro to the assembler 
// (MPLAB XXX Assembler tab).
#if defined(BRD_SBC66EC_R1)
	// SBC66EC Revision 1
	#include "Configs/HWP SBC66EC-R1.h"
#elif defined(BRD_SBC66EC_R2)
	// SBC66EC Revision 2
	#include "Configs/HWP SBC66EC-R2.h"
#elif defined(BRD_SBC66EC_R3)
	// SBC66EC Revision 3
	#include "Configs/HWP SBC66EC-R3.h"
#elif defined(BRD_SBC66ECL_R2)
	// SBC66ECL Revision 2
	#include "Configs/HWP SBC66ECL-R2.h"
#elif defined(BRD_SBC66ECL_R3)
	// SBC66ECL Revision 3
	#include "Configs/HWP SBC66ECL-R3.h"
#else
	#error "No extended HWP .h included.  Add the appropriate compiler macro to the MPLAB project."
#endif

#endif  //HARDWARE_PROFILE_H
