 /**
 * @brief           This file defines a Command Interface
 * @file            nz_command.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 *
 * @section nz_command_desc Description
 *****************************************
 * This file defines a Command interface. The implementation has to be done
 * in a separate c file by creating instances of these functions.
 *
 * @subsection nz_command_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --------------- Command Configuration (nz_command.h) ----------------
// *********************************************************************
//Indicates if this application handles commands. When defined, the application must implement the "nz_cmdXxx" callback functions!
#define APP_HANDLES_COMMANDS

 @endcode
 **********************************************************************
 * @section nz_command_lic Software License Agreement
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
 * 2010-10-11, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_COMMAND_H
#define    NZ_COMMAND_H

//Check if the application handles commands
#if !defined(APP_HANDLES_COMMANDS)
    //Check if the application handles commands via any of the known methods
    #if defined(APP_HANDLES_USB_COMMANDS)
    #define APP_HANDLES_COMMANDS
    #endif
#endif

//If USB module is to handle commands, these callback functions have to be implemented by the application
#if defined(APP_HANDLES_COMMANDS)
extern BOOL nz_cmdReadyToExec(void);
extern BOOL nz_cmdCommandReplyReady(void);
extern WORD_VAL nz_cmdParseCirbuf(CIRBUF* pBuf, BYTE size, CIRBUF* cbufReply, BYTE replyCmd);
#endif


#endif  //#ifndef NZ_COMMAND_H
