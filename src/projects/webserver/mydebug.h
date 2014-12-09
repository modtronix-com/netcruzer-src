/**
 * @brief           Debug
 * @file            myDebug.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section debug_desc Description
 *****************************************
 * This module contains code for debugging
 *
 * @subsection debug_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --------------- Debug Configuration (mydebug.h) --------------------
// *********************************************************************
#define NZ_USBHID_DEBUG_ENABLE
#define NZ_UART1_DEBUG_ENABLE

 @endcode
 *********************************************************************
 * @section debug_lic Software License Agreement
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
 * 2010-11-16, David H. (DH):
 *  - Initial version
 *********************************************************************/

/////////////////////////////////////////////////
//  Debug - documentation module
 /** @addtogroup proj_webserver_debug
  * Debugging can be performed:
  * - Using a PIC Programmer/Debugger, like the @ref devtools_icd3.
  * - Using a Debug Terminal, like the USB port of the board and the @ref devtools_nzUsbTerminal
  *
  * @section proj_webserver_debug_app Debug Terminal
  *
  * The Debug Terminal is very efficient and easy to use. Just plug your board into a USB port of your PC, and debug messages will be
  * received on the @ref devtools_nzUsbTerminal.
  *
  * The "debug.c" file contains code that implements a Debug Terminal. It's main function is to send and receive debug messages to
  * and from a configurable port. By default it is configured to use the USB HID port of the board, and the @ref devtools_nzUsbTerminal (Application
  * running on PC, and connected to board via USB port). This configuration allows the @ref devtools_nzUsbTerminal to:
  * - Display debug messages received from the board. Throughout the code there are DEBUG_PUT_STR and DEBUG_PUT_WORD macros that send
  *   debug message to the debug port (configued to use the USB HID port)
  * - Send messages to the board. These messages are parsed in the debugService() function in "debug.c".
  * - Request information from the board. These requests are also parsed in the debugService() function. For example, entering
  *   "ip" in the "Send Debug Text" box of the @ref devtools_nzUsbTerminal, and sending it, will result in the board's current IP address
  *   to be sent by the board, and displayed in the @ref devtools_nzUsbTerminal.
  *
  * All *.c files that want to send debug messages should have the following defined at the top of the file (where XXX is the filename):
  * @code
  * //Add debugging to this file. The DEBUG_CONF_XXX macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
  * #define MY_DEBUG_LEVEL   DEBUG_CONF_XXX
  * #define  DEBUG_PUT_STR(lvl, str) {if (MY_DEBUG_LEVEL >= lvl) {debugPutString(str);}}
  * #define  DEBUG_PUT_WORD(lvl, w) {if (MY_DEBUG_LEVEL >= lvl) {debugPutWord(w);}}
  * @endcode
  * 
  * The DEBUG_PUT_STR and DEBUG_PUT_WORD macros can then be used to send debug information.
  * - The first parameter is the <b>Debug Level</b>, and can be DEBUG_LEVEL_CRITICAL, DEBUG_LEVEL_ERROR, DEBUG_LEVEL_WARNING or
  * DEBUG_LEVEL_INFO. Critical is the highest level.
  * - The second parameter is a string (for DEBUG_PUT_STR) or a WORD (for DEBUG_PUT_WORD)
  * 
  * The "Debug Configuration" section in the Hardware Profile file (located in Configs folder, @ref HWP_SBC66EC-R2.h for example)
  * is used to set the debug level of each file. For example, for the main.c file, the following define
  * will result in only macros with a <b>Debug Level</b> equal to, or higher than, "Warning" to be executed.
  * @code
  * #define DEBUG_CONF_MAIN             DEBUG_LEVEL_WARNING
  * @endcode
  * For this example, the following code will result in a debug message being sent to the debug port.
  * @code
  * DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nSomething seems wrong in main.c!");
  * @endcode
  * This is the case because we set the <b>Debug Level</b> of DEBUG_CONF_MAIN to warning, meaning all
  * debug messages with Critical, Error or Warning level will be sent. The following message will however not be sent:
  * @code
  * DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nSomething interesting happened in main.c!");
  * @endcode
  *
  * Additionally the following line in the Hardware Prifile file will disable all debug messages. It is commented by default.
  * For production code, it is a good idea uncommenting this line. This will make the code smaller and faster (no debug messages
  * are sent.
  * @code
  * #define DEBUG_LEVEL_ALLOFF
  * @endcode
  *
  *
  * @section proj_webserver_debug_details Details
  * Debugging is implemented in the debug.c file.
  * It provides a simple way for the application to send and receive text based debug messages.
  * The debugPutString() and debugPutWord() functions can be used to write text based debug information
  * to the debug port. The debugGetString() function is used to get the last received debug message.
  *
  * By default debugging is configured to use the USB HID port, and the @ref devtools_nzUsbTerminal. This can be changed if
  * debugging should use some other port, like a Serial Port, UDP or something else.
  *
  * Using the @ref devtools_nzUsbTerminal for debugging is very efficient and easy. The debugService() function in "debug.c"
  * parses strings received from the @ref devtools_nzUsbTerminal (entered in the "Send Debug Text" box), and sends a reply debug
  * message. This reply message is displayed in the "Received From Device" message box. To try it, connect the USB port of the
  * board, start the @ref devtools_nzUsbTerminal, and enter "ip" in the "Send Debug Text" box. The current IP address should be
  * received from the board, and displayed in the "Received From Device" message box.
  * See example debugService() function in @ref ex_debugService.c example file. It shows how to implement two debug commands.
  *********************************************************************/
#ifndef MYDEBUG_H
#define MYDEBUG_H

#include "nz_serDataPorts.h"

/** Debug flags */
typedef union __attribute__((aligned(2), packed)) {
    WORD val;
	BYTE v[2];
	struct {
     	unsigned char bSerPortMemAllocFail:1;
       	unsigned char bCirbufSizeError:1;
        unsigned char bCirbufPacketFormatError:1;
        unsigned char bDebugFlagsRes0_3:1;
        unsigned char bDebugFlagsRes0_4:1;
        unsigned char bDebugFlagsRes0_5:1;
        unsigned char bDebugFlagsRes0_6:1;
        unsigned char bDebugFlagsRes0_7:1;

        unsigned char bDebugFlagsRes1_0:1;
       	unsigned char bDebugFlagsRes1_1:1;
        unsigned char bDebugFlagsRes1_2:1;
        unsigned char bDebugFlagsRes1_3:1;
        unsigned char bDebugFlagsRes1_4:1;
        unsigned char bDebugFlagsRes1_5:1;
        unsigned char bDebugFlagsRes1_6:1;
        unsigned char bDebugFlagsRes1_7:1;
	} bits;
} DEBUG_ERROR_FLAGS;
#define DEBUG_ERROR_FLAGS_DEFINED

/**
 * Debug Variables
 */
typedef struct _DEBUG_VARS
{
    BYTE    b1;
    BYTE    b2;
    WORD    w1;
} DEBUG_VARS;


//TX and RX buffers
extern DEBUG_VARS           debugVars;


/**
 * Prints the contents of the FLASH to the debug port.
 * @param address
 * @param length
 */
void debugPrintFlash(DWORD address, WORD length);

#endif	//#ifndef MYDEBUG_H

