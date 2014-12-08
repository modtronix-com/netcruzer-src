/**
 * @brief           Commands
 * @file            cmd.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_cmd_desc Description
 *****************************************
 * Module for implementing Netcruzer Commands and Tags.
 *
 * @subsection nz_cmd_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // ************************************************************
 // --------------------  Cmd Configuration --------------------
 // ************************************************************
 //Default "UDP Command Port"
 #define DEFAULT_UDP_COMMAND_PORT               (54123ul)   //[-DEFAULT-]

 //Default "UDP Event Port"
 #define DEFAULT_UDP_EVENT_PORT                 (54124ul)   //[-DEFAULT-]

 //Default "TCP Command Port"
 #define DEFAULT_TCP_COMMAND_PORT               (54125ul)   //[-DEFAULT-]

 //Default "TCP Event Port"
 #define DEFAULT_TCP_EVENT_PORT                 (54126ul)   //[-DEFAULT-]

 //Default size of RAM User Memory
 #define USER_RAM_SIZE                          ( 256 )     //[-DEFAULT-]

 //Disable the given commands. Use these defines to save code space if command is not required
 #define CMD_DISABLE_CMD_LL_AND_LK

 //Number of "User Command" listeners, setting to 1 will create smaller, faster code.
 #define USER_CMD_LISTENERS                     ( 1 )       //Only 1 command listerner [-DEFAULT-]

 //Only the 'x' "User Command" is implemented (not 'y' and 'z'). Setting to 1 will create smaller, faster code.
 #define USER_CMD_ONLY_X                        ( 1 )       //Only implement 'x' "User Commands" [-DEFAULT-]

 //Number of "User Tag" listeners, setting to 1 will create smaller, faster code.
 #define USER_TAG_LISTENERS                     ( 1 )       //Only 1 tag listerner [-DEFAULT-]

 //Only the 'x' "User Tag" is implemented (not 'y' and 'z'). Setting to 1 will create smaller, faster code.
 #define USER_TAG_ONLY_X                        ( 1 )       //Only implement 'x' "User Tags" [-DEFAULT-]

@endcode
 *
 **********************************************************************
 * @section nz_cmd_lic Software License Agreement
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
 * 2010-11-11, David Hosken (DH):
 *  - Initial version, ported from SBC65EC
 *********************************************************************/
#ifndef CMD_H
#define CMD_H


//This file contain circular buffers, this include is required
#include "nz_circularBuffer.h"

////////// Users ////////////////////////////////
#define USER_LEVEL_SUPER    0
#define USER_LEVEL_ADMIN    1
#define USER_LEVEL_USER     2
#define USER_LEVEL_ALL      3


//Default "UDP Command Port"
#if !defined(DEFAULT_UDP_COMMAND_PORT)
#define DEFAULT_UDP_COMMAND_PORT   (54123ul)
#endif

#if !defined(DEFAULT_UDP_EVENT_PORT)
//Default "UDP Event Port"
#define DEFAULT_UDP_EVENT_PORT     (54124ul)
#endif

#if !defined(DEFAULT_TCP_COMMAND_PORT)
//Default "TCP Command Port"
#define DEFAULT_TCP_COMMAND_PORT   (54125ul)
#endif

#if !defined(DEFAULT_TCP_EVENT_PORT)
//Default "TCP Event Port"
#define DEFAULT_TCP_EVENT_PORT     (54126ul)
#endif


/**
 * More Network Settings.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    struct
    {
        unsigned char bUdpCmd : 1;          ///< Enable UDP commands
        unsigned char bUdpEvents : 1;       ///< Enable UDP events
        unsigned char bTcpCmd : 1;          ///< Enable TCP commands
        unsigned char bTcpEvents : 1;       ///< Enable TCP events
        unsigned int  bFill : 12;           ///< Fill to ensure flags take up a 16-bit WORD
    } Flags;                                ///< Flag structure

    WORD    udpCmdPort;                     ///< The UDP command port, is defined by DEFAULT_CMD_UDPPORT (default 54123)
    WORD    udpEventPort;                   ///< The UDP event port, is defined by DEFAULT_EVENT_UDPPORT (default 54124)
    WORD    tcpCmdPort;                     ///< The TCP command port, is defined by ...
    WORD    tcpEventPort;                   ///< The TCP event port, is defined by ...

    BYTE    reserve[18];                    ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_NET2;


/////////////////////////////////////////////////
//  Commands and Tags - documentation module
/**
@defgroup proj_webserver_cmdtag Commands and Tags
Commands and Tags are used to send command, and request data via USB, UDP, TCP, Serial Ports,
Web Server and other methods. For details, download the SBC board's datasheet, or see online
documentation located here:
<a href="http://www.netcruzer.com/project-webserver-cmd">www.netcruzer.com/project-webserver-cmd</a>
and <a href="http://www.netcruzer.com/project-webserver-tags">www.netcruzer.com/project-webserver-tags</a>.
*/


////////// Tags /////////////////////////////////
#define TAG_CONFIG ('c')        // Tag - Config
#define TAG_GENERAL ('g')       // Tag - General
#define TAG_INPUT ('i')         // Tag - Input
#define TAG_CONFIG_BLOCK ('k')  // Tag - Configuration Block
#define TAG_USERMEM ('u')       // Tag - User Memory
#define TAG_OUTPUT ('o')        // Tag - Output
#define TAG_USER_X ('x')        // x "User Tag"
#define TAG_USER_Y ('y')        // y "User Tag", only available if USER_TAG_ONLY_X = 0
#define TAG_USER_Z ('z')        // y "User Tag", only available if USER_TAG_ONLY_X = 0


////////// Commands /////////////////////////////
//k = Application config commands
//g = General Command
//p = Port command (maybe use x,y and t?)
//u = User Ram command
//s = Serial port command
//v = Expansion board
//w = PWM commands
//x,y and t = Alternative Port commands
//o = Output or1(relay 1), od1(digitial 1), ow1(PWM1)...

#define CMDGROUP_CONFIG ('c')       // Config Command
#define CMDGROUP_CFGBLOCK ('k')     // CFG_BLOCK - Configuration Block Command
#define CMDGROUP_GENERAL ('g')      // General Command
#define CMDGROUP_SERPORT ('s')      // Serial Port Command
#define CMDGROUP_USERMEM ('u')      // User Memory Command
#define CMDGROUP_OUTPUT ('o')       // The Command values for this group is the new port value in decimal
#define CMDGROUP_USER_X ('x')       // x "User Command"
#define CMDGROUP_USER_Y ('y')       // y "User Command", only available if USER_CMD_ONLY_X = 0
#define CMDGROUP_USER_Z ('z')       // z "User Command", only available if USER_CMD_ONLY_X = 0


/////////// Global variables ////////////////////
#ifndef THIS_IS_CMD_C
#endif


/**
 * Initializes command processing
 */
void cmdInit(void);


/**
 * Must be called every couple of ms
 *
 */
void cmdTask(void);


/**
 * Returns TRUE if ready to execute a command, else FALSE
 *
 * @return Returns TRUE if ready to execute a command, else FALSE
 */
BOOL cmdReadyToExec(void);


/**
 * If currently executing commands, this will indicate if a reply is ready
 * @return
 */
BOOL cmdReplyReady(void);


/**
 * Execute the given name-value command. The command might not be executed if a higher user level is
 * required.
 *
 * @param name The name part of the name-value command (NULL terminated string)
 *
 * @param value The value part of the name-value command (NULL terminated string)
 *
 * @param param1 A parameter passed to this function.
 *        If flags bit 7 (flags&0x80) = 0, this is the name (and path) of the calling file, for example "config/ports.htm".
 *        If flags bit 7 = 1, this contains a CIRBUF pointer for reply to be written to.
 *
 * @param flags Bits 0-3 is the user level, is a USER_XX constant.
 *              Bit 7 is "PARAM_IS_CIRBUF" flag. When set, param contains a CIRBUF pointer.
 *              Bits 8-15 is param1 specific
 *
 * @return Returns 
 */
WORD cmdExecNameValueCmd(char* name, char* value, void* param1, WORD flags);


/**
 * Check given Circular Buffer for any name-value commands. If any of them are request commands:
 * - If cbufReply != NULL:
 *   The reply is added to given cbufReply Circular Buffer. If this function does not return 0, it is
 *   still processing commands. When done, cmdReadyToExec() will return true.
 *   If cbufReply is a "Packet" type buffer AND replyCmd is not 0, THEN the value passed in replyCmd is
 *   added before each command written to cbufReply.
 * - If cbufReply == NULL:
 *   If waiting for reply (cbufReply==NULL), use cmdReplyReady() to check when a reply is ready.
 *   Have to add more functions to actually get reply, and check if it was last reply.
 *
 * @param pBuf Pointer to CIRBUF Circular Buffer
 *
 * @param size Size of name-value commands contained in pBuf
 *
 * @param cbufReply Pointer to CIRBUF Circular Buffer used for any Command Response.
 *
 * @param replyCmd If cbufReply was given AND it is a "Packet" type buffer, the reply is preceded by the
 * value given in replyCmd. If cbufReply=0, it is not used!
 *
 * @return Lower Byte of return value contains number of bytes processed and removed from buffer.
 * Upper Byte of return value is 0 if all commands processed and finished.
 * Upper Byte of return value is 1 if commands are still busy being processed. Use cmdReadyToExec() to check when done.
 */
WORD_VAL cmdParseCirbuf(CIRBUF* pBuf, BYTE size, CIRBUF* cbufReply, BYTE replyCmd);

#if defined(HAS_WEBSERVER)
//HTTP_IO_RESULT cmdParseHTTPPost(void);
#endif

/**
 * Structure for passing variables to getVarCmd() function
 */
//typedef struct _GETTAG_INFO
//{
//    /** Requested tag */
//    BYTE* tag;
//
//    /**
//     * Input and output variable.
//     * - Will contain HTTP_START_OF_VAR for fist call
//     * - If there are no more values left for this variable, it will contain HTTP_END_OF_VAR on return
//     * - If there are still bytes to send, it will contain a value other than HTTP_START_OF_VAR and
//     *   HTTP_END_OF_VAR reference on return
//     */
//    WORD ref;
//
//    /** Pointer to buffer for writing variable to */
//    BYTE* val;
//
//    /** Pointer to CIRBUF structure */
//    CIRBUF* pBuf;
//
//    /** The current user, is a USER_XX constant */
//    BYTE user;
//
//    struct
//    {
//        unsigned char bFinished : 1;    //If set, getVarCmd() returned all requested data
//        unsigned char bHTTP : 1;        //If the result must be written to a HTTP socket
//    } flags;
//} GETTAG_INFO;


WORD cmdGetTag(BYTE* tag, WORD ref, void* dest, BYTE user, void* param);


/**
 * Writes a single byte to the "User RAM". This function checks the given ptr is located
 * within valid the "User RAM", and returns 0 if all OK.
 *
 * @param c Byte to write
 * @param ptr Pointer to "User RAM" position to write to
 *
 * @return Returns 0 if OK, else non zero
 */
BYTE cmdWriteByteUserRam(BYTE c, BYTE* ptr);

BYTE cmdAddCmdListener(WORD (*ptrCmdListener)(char*, char*, void*, WORD));

/**
 * Add pointer to "Tag Listner". It has following parameters:
 * - tag (BYTE*) = pointer to string containing tag
 * - ref (WORD) =  given reference, 0 if this is the first call for this tag
 * - dest (BYTE*) = point to destination array to write tag to, has size of CMDGETTAG_VAL_SIZE
 * - user (BYTE) = The user level required to process this request. Is a USER_XX constant
 * - Return Bit 0-14 = ref, bit 15 = more data. Bytes added to dest = ((returned bits 0-14) - (ref parameter)).
 *          If returned value = 'ref parameter', no tag was returned
 * 
 * @param ptrTagListener
 * @return 
 */
BYTE cmdAddTagListener(WORD (*ptrTagListener)(BYTE*, WORD, BYTE*, BYTE));

/**
 * Reboot board
 */
void cmdReboot(void);

#endif
