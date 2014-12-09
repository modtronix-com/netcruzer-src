 /**
 * @brief           This file defines a Debug interface
 * @file            nz_debug.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section nz_debug_desc Description
 *****************************************
 * This file defines a Debug interface. The implementation has to be done
 * in a separate c file by creating instances of these functions. To use debugging
 * in a project, the following must be done:
 * - Copy @ref nz_debug_conf "Configuration" section below to projdefs.h file.
 * - In this "Configuration" section, uncomment disired lines, and change any default
 *   values as needed.
 * - Add the nz_debug.c default implementation to the MPLAB project, or create a new
 *   implementation if the functionality should be changed.
 * - The functions defined in this file can now be used in the project.
 *
 * For additional information, see @ref info_debug
 *
 * @subsection nz_debug_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --------------- Debug Configuration (nz_debug.h) --------------------
// *********************************************************************
#define NZ_USBHID_DEBUG_ENABLE
//#define NZ_USBCDC_DEBUG_ENABLE
//#define NZ_UART1_DEBUG_ENABLE

//Size of Debug TX buffer, MUST BE power of 2 value! Increase size of this buffer if debug information
//is getting lost. This can be the case if the application writes debug information to the debug buffer
//faster than it can be outputted.
#define DEBUG_TXBUF_SIZE            ( 256 )     //[-DEFAULT-]

//Size of Debug RX buffer, MUST BE power of 2 value! Ensure it is large enought to hold largest string
//written to debug port. For example, when using the USB port for debugging, text message are sent to
//the debug port from the @ref devtools_nzUsbTerminal.
#define DEBUG_RXBUF_SIZE            ( 32 )      //[-DEFAULT-]

//Gives size of debugTempBuf. If commented, or not present, will be set to default of 128 bytes
//#define DEBUG_TEMP_BUF_SIZE       ( 128 )     //[-DEFAULT-]

  //Uncomment this line to disable all debugging!
//#define DEBUG_LEVEL_ALLOFF

//To enable debug configuration for additional modules, add line to each of the 3 sections below with name of new module. For example in first section, add "#define DEBUG_CONF_NEWMOD 0"
#if defined (DEBUG_LEVEL_ALLOFF)
    #define DEBUG_CONF_MAIN         ( 0 )
    #define DEBUG_CONF_DEFAULT      ( 0 )
#else
    #if defined (RELEASE_BUILD)
        #define DEBUG_CONF_MAIN     DEBUG_LEVEL_ERROR
        #define DEBUG_CONF_DEFAULT  DEBUG_LEVEL_ERROF
    #else
        #define DEBUG_CONF_MAIN     DEBUG_LEVEL_INFO
        #define DEBUG_CONF_DEFAULT  DEBUG_LEVEL_INFO
    #endif
#endif
 @endcode
 **********************************************************************
 * @section nz_debug_lic Software License Agreement
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
 * 2010-11-16, David H. (DH):
 *  - Initial version
 *********************************************************************/

/////////////////////////////////////////////////
//  Debug - documentation module
 /** @addtogroup info_debug
  *
  * @section info_debug_app Debug Terminal
  *
  * The Debug Terminal is very efficient and easy to use. Just plug your board into a USB port of your PC, and debug messages will be
  * received on the @ref devtools_nzUsbTerminal.
  *
  * The "nz_debug.c" file contains code that implements a Debug Terminal. It's main function is to send and receive debug messages to
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
  * #if !defined(DEBUG_CONF_XXX)
  *     #define DEBUG_CONF_XXX     DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
  * #endif
  * #define MY_DEBUG_LEVEL   DEBUG_CONF_XXX
  * #include "nz_debug.h"
  * @endcode
  * 
  * The DEBUG_PUT_XX functions defined in nz_debug.h (DEBUG_PUT_STR() for example) can than be used to write debug information.
  * - The first parameter in all DEBUG_PUT_XX functions is the <b>Debug Level</b>, and can be DEBUG_LEVEL_CRITICAL, DEBUG_LEVEL_ERROR,
  *   DEBUG_LEVEL_WARNING or DEBUG_LEVEL_INFO. Critical is the highest level.
  * - The second parameter is the debug data that is written, and depends on the function (For example, is a string for DEBUG_PUT_STR)
  * 
  * The "Debug Configuration" section in the projdefs.h file is used to set the debug level of each file. For example, for the main.c
  * file, the following define will result in only macros with a <b>Debug Level</b> equal to, or higher than, "Warning" to be executed.
  * @code
  * #define DEBUG_CONF_MAIN             DEBUG_LEVEL_WARNING
  * @endcode
  * For this example, the following code will result in a debug message being sent to the debug port.
  * @code
  * DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nSomething seems wrong in main.c!");
  * @endcode
  * This is the case because we set the <b>Debug Level</b> of DEBUG_CONF_MAIN to warning, meaning all
  * debug messages with Critical, Error or Warning level will be sent. The following message will however not be sent (and not generate any code):
  * @code
  * DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nSomething interesting happened in main.c!");
  * @endcode
  *
  * Additionally the following line in the projdefs.h file will disable all debug messages. It is commented by default.
  * For production code, it is a good idea uncommenting this line. This will make the code smaller and faster (no debug messages
  * are sent).
  * @code
  * #define DEBUG_LEVEL_ALLOFF
  * @endcode
  *
  *
  * @section info_debug_details Details
  * Debugging is implemented in the nz_debugDefault.c file.
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
  *********************************************************************/
#ifndef NZ_DEBUG_H
#define NZ_DEBUG_H

#include "nz_circularBuffer.h"

#if !defined(SERPORT_DEBUG_CREATE_OWN_CIRBUFS)
#include "nz_serDataPorts.h"
#endif

/////////////////////////////////////////////////
//Default Defines

#if !defined(DEBUG_LEVEL_OFF)
#define DEBUG_LEVEL_OFF         0
#endif

#if !defined(DEBUG_LEVEL_CRITICAL)
#define DEBUG_LEVEL_CRITICAL    1
#endif

#if !defined(DEBUG_LEVEL_ERROR)
#define DEBUG_LEVEL_ERROR       2
#endif

#if !defined(DEBUG_LEVEL_WARNING)
#define DEBUG_LEVEL_WARNING     3
#endif

#if !defined(DEBUG_LEVEL_INFO)
#define DEBUG_LEVEL_INFO        4
#endif

#if !defined(DEBUG_LEVEL_FINE)
#define DEBUG_LEVEL_FINE        5
#endif

#if !defined(DEBUG_TEMP_BUF_SIZE)
#define DEBUG_TEMP_BUF_SIZE 128
#endif

/**
 * Size of Debug TX buffer. Depending on circular buffer used for implementation, might
 * HAVE TO BE a power of 2 value (if nz_circularBufferPwr2.c is used)! Increase size
 * of this buffer if debug information is getting lost. This can be the case if the application
 * writes debug information to the debug buffer faster than it can send.
 */
#if !defined(DEBUG_TXBUF_SIZE)
#define DEBUG_TXBUF_SIZE 256
#endif

/**
 * Size of Debug RX buffer. Depending on circular buffer used for implementation, might HAVE TO
 * BE a power of 2 value (if nz_circularBufferPwr2.c is used)! Ensure it is large enought to hold
 * largest string written to debug port. For example, when using the USB port for debugging, text
 * message are sent to the debug port from the @ref devtools_nzUsbTerminal.
 */
#if !defined(DEBUG_RXBUF_SIZE)
#define DEBUG_RXBUF_SIZE 32
#endif


/////////////////////////////////////////////////
//Defines


/////////////////////////////////////////////////
//Variables

extern char debugTempBuf[DEBUG_TEMP_BUF_SIZE];

//TX and RX "Circular Buffers". Use SERPORT_DEBUG_CREATE_OWN_CIRBUFS define to use this
//module without SERPORTs. If NOT defined, this modules requires nz_serDataPorts.c
#if defined(SERPORT_DEBUG_CREATE_OWN_CIRBUFS)
    #if !defined(CIRBUF_TX_DEBUG)
    extern CIRBUF cbufTxDebug;                 //Buffer used for Debug Tx
    #define CIRBUF_TX_DEBUG (&cbufTxDebug)
    #endif

    #if !defined(CIRBUF_RX_DEBUG)
    extern CIRBUF cbufRxDebug;                 //Buffer used for Debug Rx
    #define CIRBUF_RX_DEBUG (&cbufRxDebug)
    #endif
#endif

#if !defined(DEBUG_ERROR_FLAGS_DEFINED)
typedef union __attribute__((aligned(2), packed)) {
    WORD val;
    BYTE v[2];
    struct {
        unsigned long bSerPortMemAllocFail:1;
        unsigned long bCirbufSizeError:1;
        unsigned long bCirbufPacketFormatError:1;
        unsigned long bDebugFlagsRes0_3:1;
        unsigned long bDebugFlagsRes0_4:1;
        unsigned long bDebugFlagsRes0_5:1;
        unsigned long bDebugFlagsRes0_6:1;
        unsigned long bDebugFlagsRes0_7:1;

        unsigned long bDebugFlagsRes1_0:1;
        unsigned long bDebugFlagsRes1_1:1;
        unsigned long bDebugFlagsRes1_2:1;
        unsigned long bDebugFlagsRes1_3:1;
        unsigned long bDebugFlagsRes1_4:1;
        unsigned long bDebugFlagsRes1_5:1;
        unsigned long bDebugFlagsRes1_6:1;
        unsigned long bDebugFlagsRes1_7:1;
    } bits;
} DEBUG_ERROR_FLAGS;
#endif

/**
 * Initialization Error Flags. Are flags for errors that occur during initialization before any
 * debug message can be printed. After initialization, and debug system has been initialized,
 * an attempt will be made to write an error message out onto the debug port.
 */
extern DEBUG_ERROR_FLAGS debugErrorFlags;


#if defined(HAS_NZ_DEBUGGING)
/**
 * Debug Initialization.
 *
 * IMPORTANT!<br>
 * Only call after module that debug uses has been initialized! For example, if debugging
 * is done via USB HID (NZ_USBHID_DEBUG_ENABLE has been defined), then only call this function
 * after the USB HID port has been initialized.
 *
 */
void debugInit(void);
#else
#define debugInit()
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Call once all system initialization is done. Normally just before main system loop.
 */
void debugAllInitDone(void);
#else
#define debugAllInitDone()
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Debug service routine
 */
void debugService(void);
#else
#define debugService()
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Write a single character to the debug port. If no more space in debug port
 * buffer, function returns without doing anything.
 *
 * @param b Byte to add to the buffer
 */
void debugPutChar(CHAR c);
#else
#define debugPutChar(c)
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Write BYTE in decimal format to the debug output. For example, debugPutByte(100) will
 * write the string "100" to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 *
 * @param b Byte to add to the buffer
 */
void debugPutByte(BYTE b);
#else
#define debugPutByte(b)
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Write WORD in decimal format to the debug output. For example, debugPutWord(100) will
 * write the string "100" to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 *
 * @param w Word to add to the buffer
 */
void debugPutWord(WORD w);
#else
#define debugPutWord(w)
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Adds given number of bytes to debug port. If no more space in debug port
 * buffer, function returns without doing anything.
 *
 * @param pArr Pointer to source BYTE array
 * @param size Number of bytes to copy
 */
void debugPutArray(const BYTE* pArr, WORD size);
#else
#define debugPutArray(pArr, size)
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Send given NULL terminated string to debug port. If no more space in debug port
 * buffer, function returns without doing anything.
 *
 * @param str Null terminated string to add
 */
void debugPutString(const char * str);
#else
#define debugPutString(str)
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Write the given BYTE as a 2-character "ASCII Formatted Hex" string to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 * @param b Byte to write
 */
void debugPutHexByte(BYTE b);
#else
#define debugPutHexByte(w)
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Write the given WORD as a 4-character "ASCII Formatted Hex" string to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 * @param w Word to write
 */
void debugPutHexWord(WORD w);
#else
#define debugPutHexWord(w)
#endif



#if defined(HAS_NZ_DEBUGGING)
/**
 * Writes a line in hex encoded (ASCII) format to debug port. For example:
 * 00000080: 00 12 3A 99 E0 88 30 4B 00 12 3A 99 E0 88 30 4B
 *
 * @param adr Address to print at beginning of line.
 * @param buf Buffer
 * @param len Length
 */
void debugPutHexLine(DWORD adr, BYTE* buf, BYTE len);
#else
#define debugPutHexLine(adr, buf, len)
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Wait for given bytes to become available in debug Transmit buffer.
 * @param required Number of bytes to wait for to become available
 */
void debugWaitForSpace(BYTE required);
#else
#define debugWaitForSpace(required)
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Wait till all pending bytes in the debug transmit buffer has been sent.
 */
void debugWaitTillAllSent(void);
#else
#define debugWaitTillAllSent()
#endif


#if defined(HAS_NZ_DEBUGGING)
/**
 * Tests if all pending bytes in the debug transmit buffer has been sent.
 * @return Returns TRUE if all pending data has been sent, else FALSE
 */
BOOL debugIsTxBufEmpty(void);
#else
#define debugIsTxBufEmpty()		(1)
#endif

/**
 * Write string to debug output. For example:<br>
 * DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nThis is a debug string");
 *
 * @param lvl Level above which this debug message is written to the debug port.
 * @param str Null terminated string to write to debug port
 */
#if !defined(DEBUG_PUT_STR)
#define  DEBUG_PUT_STR(lvl, str) {if (MY_DEBUG_LEVEL >= lvl) {debugPutString(str);}}
#endif

/**
 * Write a single character. For example:<br>
 * DEBUG_PUT_CHAR(DEBUG_LEVEL_ERROR, 'm');
 *
 * @param lvl Level above which this debug message is written to the debug port.
 * @param c Null terminated string to write to debug port
 */
#if !defined(DEBUG_PUT_CHAR)
#define  DEBUG_PUT_CHAR(lvl, c) {if (MY_DEBUG_LEVEL >= lvl) {debugPutChar(c);}}
#endif

/**
 * Write BYTE in decimal format to debug output. For example, following will write "100" to debug output:<br>
 * BYTE bVal = 100;
 * DEBUG_PUT_BYTE(DEBUG_LEVEL_ERROR, bVal);
 *
 * @param lvl Level above which this debug message is written to the debug port.
 * @param c Byte to write to the debug port
 */
#if !defined(DEBUG_PUT_BYTE)
#define  DEBUG_PUT_BYTE(lvl, c) {if (MY_DEBUG_LEVEL >= lvl) {debugPutByte(c);}}
#endif

/**
 * Write WORD in decimal format to debug output. For example, following will write "100" to debug output:<br>
 * WORD wVal = 100;
 * DEBUG_PUT_WORD(DEBUG_LEVEL_ERROR, wVal);
 *
 * @param lvl Level above which this debug message is written to the debug port.
 * @param w Word to write to the debug port
 */
#if !defined(DEBUG_PUT_WORD)
#define  DEBUG_PUT_WORD(lvl, w) {if (MY_DEBUG_LEVEL >= lvl) {debugPutWord(w);}}
#endif

/**
 * Write the given BYTE as a 2-character "ASCII Hexidecimal" formatted string to the debug
 * output. For example, following will write "A2" to the debug output:<br>
 * BYTE bVal = 0xa2;
 * DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_ERROR, bVal);
 *
 * @param lvl Level above which this debug message is written to the debug port.
 * @param b Byte to write to the debug port
 */
#if !defined(DEBUG_PUT_HEXBYTE)
#define  DEBUG_PUT_HEXBYTE(lvl, b) {if (MY_DEBUG_LEVEL >= lvl) {debugPutHexByte(b);}}
#endif

/**
 * Write the given WORD as a 4-character "ASCII Hexidecimal" formatted string to the debug
 * output. For example, following will write "01A2" to the debug output:<br>
 * WORD wVal = 0x1a2;
 * DEBUG_PUT_HEXWORD(DEBUG_LEVEL_ERROR, wVal);
 *
 * @param lvl Level above which this debug message is written to the debug port.
 * @param w Word to write to the debug port
 */
#if !defined(DEBUG_PUT_HEXWORD)
#define  DEBUG_PUT_HEXWORD(lvl, w) {if (MY_DEBUG_LEVEL >= lvl) {debugPutHexWord(w);}}
#endif


#endif

