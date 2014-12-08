/*********************************************************************
 * cnd.c            Commands
 * Complier:        MPLAB XC16
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 * Commands.
 *
 *********************************************************************
 * File History
 *
 * 2010-11-11, David Hosken (DH):
 *    - Initial version, ported from SBC65EC
 *********************************************************************
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
 *
 ********************************************************************/
#define THIS_IS_CMD_C

#include "HardwareProfile.h"

//Includes required for Webserver
#if defined(HAS_WEBSERVER)
#include "TCPIPConfig.h"
#include "TCPIP Stack/HTTP2.h"
#endif

#include <ctype.h> // for isdigit

#include "nz_serDataPorts.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"
#include "nz_ioPorts.h"
#include "nz_rtc.h"
#include "cmd.h"

//Add debugging to this file. The DEBUG_CONF_CMD macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_CMD)
    #define DEBUG_CONF_CMD        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_CMD
#include "nz_debug.h"



////////// Defines //////////////////////////////
//Default number of "User Command" listeners
#if !defined(USER_CMD_LISTENERS)
    #define USER_CMD_LISTENERS                  ( 1 )       //Only 1 command listerner [-DEFAULT-]
#endif

//Only the 'x' "User Command" is implemented (not 'y' and 'z'). Setting to 1 will create smaller, faster code.
#if !defined(USER_CMD_ONLY_X)
    #define USER_CMD_ONLY_X                     ( 1 )       //Only implement 'x' "User Commands" [-DEFAULT-]
#endif

//Default number of "User Tag" listeners
#if !defined(USER_TAG_LISTENERS)
    #define USER_TAG_LISTENERS                  ( 1 )       //Only 1 tag listerner [-DEFAULT-]
#endif

//Only the 'x' "User Tag" is implemented (not 'y' and 'z'). Setting to 1 will create smaller, faster code.
#if !defined(USER_TAG_ONLY_X)
    #define USER_TAG_ONLY_X                     ( 1 )       //Only implement 'x' "User Commands" [-DEFAULT-]
#endif

//Defines for flags parameter of cmdExecNameValueCmd()
#define FL_PARAM_IS_CIRBUF 0x80;


////////// Variables ////////////////////////////

//Array of "User Cmd Listeners"
WORD (*pUserCmdListeners[USER_CMD_LISTENERS])(char * name, char * value, void* param1, WORD flags);
BYTE numOfUserCmdListeners; //Gives number of listeners registered.

//Array of "User Tag Listeners"
WORD (*pUserTagListeners[USER_TAG_LISTENERS])(BYTE*, WORD, BYTE*, BYTE);
BYTE numOfUserTagListeners; //Gives number of listeners registered.



////////// Function prototypes //////////////////


enum SM_TASK_ {
    SM_TASK_IDLE=0,
    SM_TASK_WAIT_CIRBUF_REPLY
};

BYTE smCmd = SM_TASK_IDLE;
CIRBUF* pBuff = NULL;

/**
 * Initializes command processing
 */
void cmdInit(void) {
    memset(nzGlobals.userRam, 0, USER_RAM_SIZE);

    //Set both User and Tag command listeners to 0
    numOfUserCmdListeners = 0;
    numOfUserTagListeners = 0;
}


/**
 * Must be called every couple of ms.
 * Each UDP message can consists out of any number of "Commands" and "Requests", seperated by '&'
 * characters. Each "Get Tag Command" starts with a % character.
 *
 */
void cmdTask(void) {

    // TODO
    //If a task was executed, and we are waiting for a reply, process that here!

    switch(smCmd) {
        case SM_TASK_IDLE:
            break;
        case SM_TASK_WAIT_CIRBUF_REPLY:
            //We are waiting for a reply from a Circular Buffer, as given by pBuff.
            //No more commands will be taken until reply received. cmdReadyToExec() will return FALSE
            // TODO - Determine what we will do when reply is available!
            // - Do we add reply to a supplied "Reply Buffer"
            // - Or do we just set a flag that will cause cmdReplyReady() to return TRUE. The
            //   interested task will then see this, and get reply from "Circular Buffer"
            break;
    }
}


/**
 * Returns TRUE if ready to execute a command, else FALSE
 *
 * @return Returns TRUE if ready to execute a command, else FALSE
 */
BOOL cmdReadyToExec(void) {
    return TRUE;
}

/**
 * If currently executing commands, this will indicate if a reply is ready
 * @return
 */
BOOL cmdReplyReady(void) {
    return FALSE;
}


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
WORD cmdExecNameValueCmd(char * name, char * value, void* param1, WORD flags) {
    BYTE i;
    WORD_VAL w;

    #define SIZE_BUF 128 //Must be large enough to keep all data sent to LCD display
    BYTE buf[SIZE_BUF];

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ncmdExecNameValueCmd");

    /////////////////////////////////////////////////
    //Config command
    if (name[0] == CMDGROUP_CONFIG) {
        if (isdigit(name[1])) {
            WORD_VAL typeConf;      //MSB=Type, LSB=Configuration
            typeConf.Val = 0;

            //Get "Port ID"
            w.Val = getPortIdForStr(&name[1]);
            if (w.Val != IOPORT_ID_NA) {
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCMD c");
                DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, w.Val);

                //Input
                if (value[0]=='i') {
                    //Configure as input type
                    typeConf.byte.HB = PCFG_DIN_TYPE;

                    //'iu' command = Input with Pull-up resistor enabled
                    if (value[1]=='u') {
                        typeConf.byte.LB |= PCFG_IN_FL_PULLUP;
                    }
                    //'iw' command = Input with Pull-down resistor enabled
                    else if (value[1]=='w') {
                        typeConf.byte.LB |= PCFG_IN_FL_PULLDOWN;
                    }
                    //'id' command = Input Digital
                    else if (value[1]=='d') {
                        //Already done on top
                        //typeConf.byte.HB = PCFG_DIN_TYPE;
                    }
                    //Not a valid input type
                    else {
                        typeConf.Val = 0;   //No type or configuration
                    }
                }
                //Output
                else if (value[0]=='o') {
                    //Configure as digital output type
                    typeConf.byte.HB = PCFG_DOUT_TYPE;

                    //'oc' command = Open Collector Output
                    if (value[1]=='c') {
                        typeConf.byte.LB |= PCFG_IN_FL_PULLUP;
                    }
                    //'od' command = Input
                    else if (value[1]=='d') {
                        //Already done on top
                        //typeConf.byte.HB = PCFG_DOUT_TYPE;
                    }
                    //Not a valid output type
                    else {
                        typeConf.Val = 0;   //No type or configuration
                    }
                }
                //PWM
                else if (value[0]=='p') {
                    
                }
                //Analog Input
                else if (value[0]=='a') {

                }

                //Configure IO Ports
                portConfig(w.Val, typeConf.Val);
            }
        }
        else {
            switch(name[1]) {
                //Port configuration
                case 's':
                {

                }
            }
        }
    }
    /////////////////////////////////////////////////
    //General command
    else if (name[0] == CMDGROUP_GENERAL) {
        if (name[1] == 'l') {
            #if !defined(CMD_DISABLE_CMD_LL_AND_LK)
            //LCD string (ls) or LCB Byte array (lb)
            if((name[2] == 'b') || (name[2] == 's')) {
                BYTE c;
                BYTE tmp;
                BYTE* p;
                //Write the given string to the I2C 1 Tx buffer. This is a Binary buffer, with Escape Sequence (Control Characters).
                // The '^' is the escape character. The escape characters are:
                // - '^^' = '^' character
                // - ^s = Put start on bus. A second 's' character (anywhere in string) before 'p' character will cause a repeated Start condition.
                // - ^p = Put stop on bus
                // - ^rn = Read n number of bytes, where n is a value from 0-254. A value of 255 means read till slave does not ACK
                //
                //We have to format the string received from the ll command to I2C 1 Tx Buffer format
                // - Start with LCD address, is 0x50, 0x52, 0x54 or 0x56
                // - Next byte is 0x80 command = parsed string
                // - Following this is the LCD command, and given in LCD2S datasheet
                enum {
                    SM_DEFAULT=0,
                    SM_HEX_MSB,
                    SM_HEX_LSB
                } sm = SM_DEFAULT;
                p = (BYTE *)value;
                i = 0;

                //I2C Start control character
                buf[i++] = '^';
                buf[i++] = 's';

                //Does the command include the number of the LCD display?
                tmp = 0x50; //Default
                if ((name[3]>='1') && (name[3]<='4')) {
                    tmp += (name[3]-'1')*2;
                }
                buf[i++] = tmp;     //First byte is LCD address
                buf[i++] = 0x80;    //Second byte is "Write Parsed String" command

                //Parse given string. Because the LCD string can only contain a couple of character,
                //special characters must be parsed. An escape sequence starts with a '_' character,
                //and contains a two character hex value
                while (i < (SIZE_BUF-3) ) {
                    c = *p++;

                    //End of string, send to LCD display
                    if (c == 0) {
                        break;
                    }

                    //Default state, search for escape sequence
                    if (sm == SM_DEFAULT) {
                        //Escape sequence to follow
                        if (c == '_') {
                            sm++;       //Set to check escape sequence character
                        }
                        else {
                            //Parse LCD string
                            buf[i++] = c;
                        }
                    }
                    //First character of escape sequence - this is the first character following the _
                    else if (sm == SM_HEX_MSB) {
                        //This is not an escape sequence, but a _ character
                        if (c == '_') {
                            buf[i++] = c;
                            sm = SM_DEFAULT;
                        }
                        //One character (string escape character) follows the _ escape sequence
                        else if (name[2] == 's') {
                            tmp=0;
                            switch(c) {
                                case 'n':
                                    tmp=0x0a;
                                    break;
                                case 'f':
                                    tmp=0x0c;
                                    break;
                                case 'r':
                                    tmp=0x0d;
                                    break;
                                case 'b':
                                    tmp=0x08;
                                    break;
                                case 't':
                                    tmp=0x09;
                                    break;
                            }
                            //Was a valid escape sequence found
                            if (tmp != 0) {
                                buf[i++] = tmp;
                            }
                            sm = 0;
                        }
                        //Two characters (hex code) follows the _ escape sequence
                        else {  //Is CMDCODE_GEN_LCD_ARRAY, no need to test!
                        //else if (name[1] == CMDCODE_GEN_LCD_ARRAY) {
                            //Upper character of two character hex escape sequence
                            tmp = nzAsciiHexNibbleToByte(c) << 4;
                            sm++;
                        }
                    }
                    //Second character of escape sequence - this is the second character following the _
                    else if (sm == SM_HEX_LSB) {
                        tmp |= nzAsciiHexNibbleToByte(c);
                        buf[i++] = tmp;
                        sm = 0;
                    }
                }

                //I2C Stop control character
                buf[i++] = '^';
                buf[i++] = 'p';

                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCMD ll pack size=");
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, i);

                //Write packet to I2C 1 Transmit Buffer
                cbufPutArray(CIRBUF_TX_I2C1, buf, i);
            }
            #endif  //#if !defined(CMD_DISABLE_CMD_LL_AND_LK)
        }
        //Reset the board - "grt=1" command
        else if((name[1] == 'r') && (name[2] == 't')) {
            if (value[0]=='1') {
                cmdReboot();
            }
        }

    }
    /////////////////////////////////////////////////
    //User Memory command
    else if (name[0] == CMDGROUP_USERMEM) {
        //#define CMD_USERMEM_DEBUG_INFO
        #define MAX_WR_SIZE     16
        #define DATATYPE_HEX    0
        #define DATATYPE_DEC    1
        #define DATATYPE_STR    0xff
        BOOL dataType = 0;
        WORD adr;
        BYTE idx;
        BYTE j;

        //Parse the requested "User Memeory" address
        adr = nzParseHexDecWord((const char*)&name[1], &idx);

        //The number of bytes parsed is contained in bits 0-3, clear the rest (flags)
        idx &= 0x0f;
        idx++;         //Get index of first byte after parsed data (we started at name[1], so add 1

        #if defined(CMD_USERMEM_DEBUG_INFO)
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUsrMem 0x");
        DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, adr);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Type=");
        #endif

        //Write a string, Ex. "uFFs=5A"
        if(name[idx]=='s') {
            i = MAX_WR_SIZE;    //Set to max size, will search for NULL termination in code below
            dataType = DATATYPE_STR;
            idx++;              //Inc to point to next char in tag
            #if defined(CMD_USERMEM_DEBUG_INFO)
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "s=");
            #endif
        }
        //Write an array, Ex. "uF5a5=1020335AAB"
        else if(name[idx]=='a') {
            //Get number of bytes
            i = nzParseHexDecWord((const char*)&name[idx+1], NULL);

            //Ensure array is in the range 0-15
            i &= 0x0f;

            //DON'T inc index! We use (name[idx]=='a') below to check if array!!!
            //idx++;              //Inc to point to next char in tag

            #if defined(CMD_USERMEM_DEBUG_INFO)
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "a(");
            DEBUG_PUT_BYTE(DEBUG_LEVEL_INFO, i);
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, ")= ");
            #endif
        }
        //Write a BYTE or WORD, Ex. "uF5=5A", "uF5=d200", "uF5w=5A"
        else {
            i = 1;      //Single byte was requested

            //A WORD was requested, get second byte (Ex. "uF5w")
            if (name[idx]=='w') {
                i = 2;
                idx++;  //Inc to point to next char in tag
                #if defined(CMD_USERMEM_DEBUG_INFO)
                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'w');
                #endif
            }
            #if defined(CMD_USERMEM_DEBUG_INFO)
            else {
                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'b');
            }
            #endif
        }

        //Check if value is Decimal
        //If NOT array (always hex), idx will point to next char of name
        if(name[idx]!='a') {
            //A Decimal BYTE or WORD was requested (Ex. "uF5wd" or "uF5d")
            if(name[idx]=='d') {
                idx++;          //Inc to point to next char in tag
                dataType = DATATYPE_DEC;
                #if defined(CMD_USERMEM_DEBUG_INFO)
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Dec");
                #endif
            }
        }

        //Set "User Memory" with given data
        j = 0;
        while(i--) {
            //Ensure 'adr' is not larger than "User Memory"
            if (adr >= USER_RAM_SIZE) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "u Cmd Adr Overrun!");
                break;
            }

            //Value is a String
            if (dataType == DATATYPE_STR) {
                #if defined(CMD_USERMEM_DEBUG_INFO)
                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, value[j]);
                #endif
                nzGlobals.userRam[adr++] = value[j];
                //END of string has been reached!
                if (value[j++] == 0) {
                    break;  //Done
                }
            }
            //Value is Hex or Decimal Data (Byte, Word or Array)
            else {
                //Decimal data
                if (dataType == DATATYPE_DEC) {
                    //Value is Decimal BYTE or WORD (i was decremented in loop)
                    if (i<=1) {
                        w.Val = atoi(&value[j]);
                        nzGlobals.userRam[adr++] = w.byte.LB;
                        #if defined(CMD_USERMEM_DEBUG_INFO)
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " LB=");
                        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, w.byte.LB);
                        #endif
                        //Value is a Decimal WORD
                        if (i==1) {
                            nzGlobals.userRam[adr++] = w.byte.HB;
                            #if defined(CMD_USERMEM_DEBUG_INFO)
                            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " HB=");
                            DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, w.byte.HB);
                            #endif
                        }
                        break;  //Done
                    }
                    else {
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Dec NOT SPRTD!");
                        break;
                    }
                }
                //Hex data
                else {
                    //Value is Decimal BYTE or WORD - NOT an Array
                    if(name[idx]!='a') {
                        w.Val = nzParseHexDecWord((const char*)&value[0], NULL);

                        //For faster speed, use code below in stead of nzParseHexDecWord()
                        //w.Val = 0;
                        //Convert given "2 character upper case hex" BYTE or WORD to value
                        //while(value[j] != 0) {
                        //    w.Val = (w.Val<<4) + ((value[j]<='9')?(value[j]-'0'):(value[j]-'A'+10));
                        //    j++;
                        //}

                        nzGlobals.userRam[adr++] = w.byte.LB;
                        #if defined(CMD_USERMEM_DEBUG_INFO)
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " LB=");
                        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, w.byte.LB);
                        #endif

                        //Value is a Decimal WORD (i was decremented in loop from 2 to 1)
                        if (i==1) {
                            nzGlobals.userRam[adr++] = w.byte.HB;
                            #if defined(CMD_USERMEM_DEBUG_INFO)
                            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " HB=");
                            DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, w.byte.HB);
                            #endif
                        }
                        break;  //Done
                    }

                    //Value is an Array
                    //Maybe add check if NULL termination is reached. This way do not need to specify number after array.

                    //Convert next two characters to a byte
                    w.byte.HB = value[j++];
                    w.byte.LB = value[j++];
                    nzGlobals.userRam[adr++] = nzAsciiHexToByte(w);
                    #if defined(CMD_USERMEM_DEBUG_INFO)
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " B=");
                    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, nzGlobals.userRam[adr-1]);
                    #endif
                }
            }
        }
    }
    /////////////////////////////////////////////////
    //Output command
    else if (name[0] == CMDGROUP_OUTPUT) {
        if (isdigit(name[1])) {
            //Get "Port ID"
            w.Val = getPortIdForStr(&name[1]);
            if (w.Val != IOPORT_ID_NA) {
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCMD o");
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, w.Val);

                //Write value to given port
                portWriteBitadr(portGetLAT(w.Val), (value[0]=='1'));
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-Out");
            }
        }
    }
    /////////////////////////////////////////////////
    //"Serial Data Port" Command - send following command to "Serial Data Port". Command is in text(ASCII) format
    //- Second parameter is @ref info_serport "Serial Data Port" number 1=uart1, 2=uart2, 3=uart3, 4=uart4, 5=SPI1, 6=SPI3, 7=I2C1, 8=I2C2, 9=I2C3, ....
    else if (name[0] == CMDGROUP_SERPORT) {
        BYTE portID;
        SERPORT*  pSerport;
        WORD putAsciiEscFlags = 0;

        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCMD s");    //s Command Error 1

        //Convert port number to port ID, as defined by SERPORT_ID_xx defines
        if ( (name[1]>='1') && (name[1]<='9') )
            portID=name[1]-'0';        //IDs 0 to 9
        else if ( (name[1]>='a') && (name[1]<='o') )
            portID=name[1]-('a'-10);    //IDs 10 to 24
        else {
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCMD s E1");    //s Command Error 1
            return 0;
        }

        //Get pointer to SERPORT for requested port. Following 3 lines (requires ppSerport) replaced by line below them (4th line)
        //SERPORT** ppSerport;
        //ppSerport = &NZ_GLOBALS_SERPORT_FIRST;
        //pSerport = ppSerport[portID-1];
        pSerport = (&NZ_GLOBALS_SERPORT_FIRST)[portID-1];

        if (pSerport != NULL) {
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCMD s Adding ");
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, value);

            //TODO - Add code here for checking type of destination buffer we have to write to.
            // - If stream, just write available data as array
            // - If packet, place in packet
            //Also check format of destination, and take action.

            //A serial port (UART)
            if (portID<=SERPORT_ID_UART4) {
                // TODO Implement function for adding UART Formatted String to buffer
                //serUartPutAscii(&(pSerport->cbufTx), value, 0, strlen(value));
            }
            //A SPI port
            else if (portID<=SERPORT_ID_SPI3) {
            }
            //An I2C port
            else if (portID<=SERPORT_ID_I2C3) {
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCMD I2C?");
                //Currently only support I2C1
                if (portID==SERPORT_ID_I2C1) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCMD I2C1");

                    //If given string does not start with a 's', add 's' and 'p' control characters!
                    if (value[0] != 's') {
                        putAsciiEscFlags |= ASCIIESC_ADD_S_AND_P;
                    }

                    //Parse given I2C Formatted String, and write it to I2C Tx buffer.
                    cbufPutAsciiEscString(&(pSerport->cbufTx), 0, value, putAsciiEscFlags);
                }
            }
            else {
                // TODO Implement function for adding General(not for any specific port type) String to buffer
                //serPutString(&(pSerport->cbufTx), value, 0, strlen(value));
            }
        }
        #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
        else {
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\npSerport=NULL");
        }
        #endif
    }
    /////////////////////////////////////////////////
    //User command
#if (USER_CMD_ONLY_X == 1)
    else if (name[0] == CMDGROUP_USER_X) {
#else
    else if ((name[0]>=CMDGROUP_USER_X) && (name[0]<=CMDGROUP_USER_Z)) {
#endif

    //There is a maximum of only 1 "User Command" listener
    #if (USER_CMD_LISTENERS == 1)
        if (numOfUserCmdListeners != 0) {
            (*pUserCmdListeners[0])(name, value, param1, flags);    //Call listener
    //Call all "User Command" listeners
    #else
        BYTE i;
        for (i=0; i<numOfUserCmdListeners; i++) {
            (*pUserCmdListeners[i])(name, value, param1, flags);    //Call listener
    #endif
        }
    }
    return 0;
}


/**
 * Parses given Circular Buffer for any name-value commands. All bytes processed are removed, as indicated
 * by lower byte of return parameter. If any of the commands are request commands:
 * - If cbufReply != NULL:
 *   The reply is added to given cbufReply Circular Buffer. If this function does not return 0, it is
 *   still processing commands. When done, cmdReadyToExec() will return true.
 *   If cbufReply is a "Packet" type buffer AND replyCmd is not 0, THEN the value passed in replyCmd is
 *   added before each command written to cbufReply.
 * - If cbufReply == NULL:
 *   If waiting for reply (cbufReply==NULL), use cmdReplyReady() to check when a reply is ready.
 *   Have to add more functions to actually get reply, and check if it was last reply.
 *
 * @param pBuf Pointer to source buffer (CIRBUF Circular Buffer)
 *
 * @param size Size of bytes in source buffer to process
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
WORD_VAL cmdParseCirbuf(CIRBUF* pBuf, BYTE size, CIRBUF* cbufReply, BYTE replyCmd) {
    WORD_VAL ret;
    BYTE name[8];       //name string, NULL terminated
    BYTE value[100];     //value string, NULL terminated
    WORD sizeGetArr;
    BOOL done = FALSE;
    WORD sizeRemoved=0; //Bytes removed
    WORD_VAL paramFlags;

    enum {
        SM_NAME=0,
        SM_VALUE
    } sm = SM_NAME;

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ncmdParseCirbuf()");

    while ((sizeRemoved < size) && (done==FALSE)) {
        switch(sm) {
            case SM_NAME:
                //Found name
                if ( (sizeGetArr=cbufGetArrayTillByte(pBuf, name, '=', min(size-sizeRemoved, sizeof(name))) ) != -1) {
                    sizeRemoved += sizeGetArr;  //Update how many bytes we have processed and removed from source buffer (pBuf)
                    name[sizeGetArr-1]=0;       //Terminate string with NULL, overwrite '=' with NULL
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nName='");
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (char*)name);
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "'");
                    sm = SM_VALUE;
                }
                //Name not found, return
                else {
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nNameNF");
                    done = TRUE;
                }
                break;
            case SM_VALUE:
                //Found value
                if ( (sizeGetArr=cbufGetArrayTillByte(pBuf, value, '&', min(size-sizeRemoved, sizeof(value)-1)) ) != -1) {
                    sizeRemoved += sizeGetArr;  //Update how many bytes we have processed and removed from source buffer (pBuf)

                    //A '&' was found
                    if (value[sizeGetArr-1] == '&') {
                        value[sizeGetArr-1]=0;        //String terminating 0
                        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n&");
                    }
                    //End of source buffer reached
                    else {
                        value[sizeGetArr]=0;      //String terminating 0. No '&' was received, so we have to put 0 one byte after all received bytes
                        done=TRUE;
                        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nEnd");
                    }

                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nValue='");
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (char*)value);
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "'");


                    paramFlags.byte.LB =  USER_LEVEL_ALL | FL_PARAM_IS_CIRBUF;      //User and flags
                    paramFlags.byte.HB = replyCmd;                                  //Param2
                    cmdExecNameValueCmd((char*)name, (char*)value, cbufReply, 0);

                    sm = SM_NAME;
                }
                //Name not found, return
                else {
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nValueNF");
                    done = TRUE;
                }
                break;
        }
    }

    ret.byte.LB = sizeRemoved;  //Removed all bytes given to us from buffer
    ret.byte.HB = 0;            //All commands finished

    return ret;
}

/**
 * Compare first 2 bytes contained in given BYTE[] to bytes contained in
 * lower and upper byte of given word.
 * @param src BYTE array with source characters
 * @param twoChars Two characters contained in lower and upper byte of word.
 * @return Returns 0 if not equal, else a NON-Zero value
 */
BYTE cmp2Bytes(BYTE* src, WORD twoChars) {
    return ((src[0]==nzWordGetLowByte_MACRO(twoChars)) && (src[1]==nzWordGetHighByte_MACRO(twoChars)));
}
BYTE cmp2Bytes_(BYTE* src, BYTE* bytes) {
    return ((src[0]==bytes[0]) && (src[1]==bytes[1]));
}

/**
 * Compare first 3 bytes contained in given BYTE[] to bytes contained in 'bytes' parameter.
 * @param src BYTE array with source characters
 * @param bytes BYTE array with bytes to compare to.
 * @return Returns 0 if not equal, else a NON-Zero value
 */
BYTE cmp3Bytes(BYTE* src, BYTE* bytes) {
    return ((src[0]==bytes[0]) && (src[1]==bytes[1]) && (src[2]==bytes[2]));
}

const BYTE BARR[] = {'5','6'};


/**
 * Get the requested tag. The command might not be executed if a higher user level is
 * required.
 *
 * @param tag Pointer to string containing tag
 *
 * @param ref Reference used by this function to determine if it is the first call, or
 *      if it should continue from last return.
 *      If 0, this is the first call for requesting this tag.
 *
 * @param dest The destination to write data to.
 *      If 0 (tag[0]='#'), the requested data must be written to current TCP socket via TCPPutXxx(sktHTTP,..) functions
 *      If NOT 0, dest is a Circular Buffer (CIRBUF*)
 *
 * @param user The user level required to process this request. Is a USER_XX constant
 *
 * @param param Possible additional parameters
 *
 * @return Returns 0 if done, else a reference. If a reference is returned, it must be passed
 * back to this function on the next call.
 */
WORD cmdGetTag(BYTE* tag, WORD ref, void* dest, BYTE user, void* param) {
    #define CMDGETTAG_VAL_SIZE  64
    WORD_VAL w;
    BYTE i,j;
    BOOL webRequest = FALSE;
    BYTE valLen = 0;
    BYTE val[CMDGETTAG_VAL_SIZE];

    if(tag[0]=='#') {
        webRequest = TRUE;
        tag++;  //Increment past '#'
    }

    /////////////////////////////////////////////////
    //Config Tag
    if (tag[0] == TAG_CONFIG) {
        if (isdigit(tag[1])) {
            //Get "Port ID"
            w.Val = getPortIdForStr((const char*)&tag[1]);
            if (w.Val != IOPORT_ID_NA) {
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nTAG i");
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, w.Val);

                //Port is an output
                if (portReadBitadr(portGetTRIS(w.Val))==0) {
                    val[0]='o';
                    //Port is normal Digital Output = 'od'
                    if (portReadBitadr(portGetODC(w.Val))==0) {
                        val[1]='d';
                    }
                    //Port is an Open Collector = 'oc'
                    else {
                        val[1]='c';
                    }
                }
                //Port is an input
                else {
                    val[0]='i';
                    //Port is a Digital Input with Pullup resistor enabled = 'iu'
                    if (portReadBitadr(portGetCNPU(w.Val))==1) {
                        val[1]='u';
                    }
                    //Port is a Digital Input with Pulldown resistor enabled = 'iw'
                    else if (portReadBitadr(portGetCNPD(w.Val))==1) {
                        val[1] = w.Val;
                    }
                    //Port is an normal Digital Input = 'id'
                    else {
                        val[1]='d';
                    }
                }
                valLen = 2;
                ref = 0;    //Done
            }
        }
    }
    /////////////////////////////////////////////////
    //General Tag
    else if (tag[0] == TAG_GENERAL) {
        //gc: - Return cookie name
        if ((tag[1] =='c') && (tag[2] == ':')) {
        //if (cmp2Bytes(&tag[1], nzWordFromBytes_MACRO('c',':') )) {
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCookie '");
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (const char*)&tag[3]);
            //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '\'');

            BYTE *ptr;
            //Search for a cookie with the given name
            ptr = HTTPGetROMArg(curHTTP.data, &tag[3]);
            if(ptr) {
                TCPPutString(sktHTTP, ptr);
            }
        }
        //gdd - Dynamic DNS
        if ((tag[1] =='d') && (tag[2] == 'd')) {
            //The Dynamic DNS messages are only valid for web requests!
            if (webRequest == 0) {
                return 0;   //Done
            }

            //gdds - DDNS Host
            if (tag[3] == 'h') {
                #if defined(STACK_USE_DYNAMICDNS_CLIENT)
                if(DDNSClient.ROMPointers.Host || !DDNSClient.Host.szRAM)
                    return 0;   //Done
                if(curHTTP.callbackPos == 0x00u)
                    curHTTP.callbackPos = (PTR_BASE)DDNSClient.Host.szRAM;
                curHTTP.callbackPos = (PTR_BASE)TCPPutString(sktHTTP, (BYTE*)(PTR_BASE)curHTTP.callbackPos);
                if(*(BYTE*)(PTR_BASE)curHTTP.callbackPos == '\0')
                    curHTTP.callbackPos = 0x00;
                #endif
            }
            //gddv - DDNS Message
            else if (tag[3] == 'm') {
                if(TCPIsPutReady(sktHTTP) < 75u) {
                    return 1;   //Not done, return 1
                }

                #if defined(STACK_USE_DYNAMICDNS_CLIENT)
                switch(DDNSGetLastStatus()) {
                    case DDNS_STATUS_GOOD:
                    case DDNS_STATUS_NOCHG:
                        TCPPutROMString(sktHTTP, (ROM BYTE*)"The last update was successful.");
                        break;
                    case DDNS_STATUS_UNCHANGED:
                        TCPPutROMString(sktHTTP, (ROM BYTE*)"The IP has not changed since the last update.");
                        break;
                    case DDNS_STATUS_UPDATE_ERROR:
                    case DDNS_STATUS_CHECKIP_ERROR:
                        TCPPutROMString(sktHTTP, (ROM BYTE*)"Could not communicate with DDNS server.");
                        break;
                    case DDNS_STATUS_INVALID:
                        TCPPutROMString(sktHTTP, (ROM BYTE*)"The current configuration is not valid.");
                        break;
                    case DDNS_STATUS_UNKNOWN:
                        TCPPutROMString(sktHTTP, (ROM BYTE*)"The Dynamic DNS client is pending an update.");
                        break;
                    default:
                        TCPPutROMString(sktHTTP, (ROM BYTE*)"An error occurred during the update.<br />The DDNS Client is suspended.");
                        break;
                }
                #else
                TCPPutROMString(sktHTTP, (ROM BYTE*)"The Dynamic DNS Client is not enabled.");
                #endif
                return 0;   //Done
            }
            //gddp - DDNS Pass
            else if (tag[3] == 'p') {
                #if defined(STACK_USE_DYNAMICDNS_CLIENT)
                if(DDNSClient.ROMPointers.Password || !DDNSClient.Password.szRAM)
                    return 0;   //Done
                if(curHTTP.callbackPos == 0x00u)
                    curHTTP.callbackPos = (PTR_BASE)DDNSClient.Password.szRAM;
                curHTTP.callbackPos = (PTR_BASE)TCPPutString(sktHTTP, (BYTE*)(PTR_BASE)curHTTP.callbackPos);
                if(*(BYTE*)(PTR_BASE)curHTTP.callbackPos == '\0')
                    curHTTP.callbackPos = 0x00;
                #endif
                return 0;   //Done
            }
            //gdds - DDNS Status
            else if (tag[3] == 's') {
                #if defined(STACK_USE_DYNAMICDNS_CLIENT)
                DDNS_STATUS s;
                s = DDNSGetLastStatus();
                if(s == DDNS_STATUS_GOOD || s == DDNS_STATUS_UNCHANGED || s == DDNS_STATUS_NOCHG)
                    TCPPutROMString(sktHTTP, (ROM BYTE*)"ok");
                else if(s == DDNS_STATUS_UNKNOWN)
                    TCPPutROMString(sktHTTP, (ROM BYTE*)"unk");
                else
                    TCPPutROMString(sktHTTP, (ROM BYTE*)"fail");
                #else
                TCPPutROMString(sktHTTP, (ROM BYTE*)"fail");
                #endif
                return 0;   //Done
            }
            //gddu - DDNS User
            else if (tag[3] == 'u') {
                #if defined(STACK_USE_DYNAMICDNS_CLIENT)
                if(DDNSClient.ROMPointers.Username || !DDNSClient.Username.szRAM)
                    return 0;   //Done
                if(curHTTP.callbackPos == 0x00u)
                    curHTTP.callbackPos = (PTR_BASE)DDNSClient.Username.szRAM;
                curHTTP.callbackPos = (PTR_BASE)TCPPutString(sktHTTP, (BYTE*)(PTR_BASE)curHTTP.callbackPos);
                if(*(BYTE*)(PTR_BASE)curHTTP.callbackPos == '\0')
                    curHTTP.callbackPos = 0x00;
                #endif
                return 0;   //Done
            }
            //gddu - DDNS Service
            else if (tag[3] == 'v') {
                i = tag[4] - '0';   //position 4 contains service number
                extern ROM char * ROM ddnsServiceHosts[];
                #if defined(STACK_USE_DYNAMICDNS_CLIENT)
                if(!DDNSClient.ROMPointers.UpdateServer || !DDNSClient.UpdateServer.szROM)
                    return 0;   //Done
                if((ROM char*)DDNSClient.UpdateServer.szROM == ddnsServiceHosts[i])
                    TCPPutROMString(sktHTTP, (ROM BYTE*)"selected");
                #endif
                return 0;   //Done
            }
        }
        //gf....
        else if (tag[1] =='f') {
            j=0;
            //gf: - Include given file
            if (tag[2] == ':') {
                HTTPIncFile(&tag[3]);
                return curHTTP.callbackPos;
            }
            //gfd - Get firmware build date
            else if (tag[2] == 'd') {
                strcpy((char*)val, (ROM void*)__DATE__" ""00:08:00");
                j=1;    //Mark that valLen must be set equal to strlen(val)
            }
            //gfv - Get firmware version
            else if (tag[2] == 'v') {
                strcpy((char*)val, (ROM void*)TCPIP_STACK_VERSION);
                j=1;    //Mark that valLen must be set equal to strlen(val)
            }
            if (j==1) {
                valLen = strlen((const char*)val);
            }
            ref = 0;    //Done
        }
        //gs....
        else if (tag[1] =='s') {
            if (tag[2]=='k') {
                j=0;
                switch (tag[3]) {
                    //'gsk0' - TCP/IP stack flags 0
                    case '0':
                        #if defined(STACK_USE_IP_GLEANING)
                        j |= 0x01;
                        #endif
                        #if defined(STACK_USE_ICMP_SERVER)
                        j |= 0x02;
                        #endif
                        #if defined(STACK_USE_ICMP_CLIENT)
                        j |= 0x04;
                        #endif
                        #if defined(STACK_USE_HTTP2_SERVER)
                        j |= 0x08;
                        #endif
                        #if defined(STACK_USE_SSL_SERVER)
                        j |= 0x10;
                        #endif
                        #if defined(STACK_USE_SSL_CLIENT)
                        j |= 0x20;
                        #endif
                        #if defined(STACK_USE_AUTO_IP)
                        j |= 0x40;
                        #endif
                    break;
                    //'gsk0' - TCP/IP stack flags 1
                    case '1':
                        #if defined(STACK_USE_DHCP_CLIENT)
                        j |= 0x01;
                        #endif
                        #if defined(STACK_USE_DHCP_SERVER)
                        j |= 0x02;
                        #endif
                        #if defined(STACK_USE_FTP_SERVER)
                        j |= 0x04;
                        #endif
                        #if defined(STACK_USE_SMTP_CLIENT)
                        j |= 0x08;
                        #endif
                        #if defined(STACK_USE_SNMP_SERVER)
                        j |= 0x10;
                        #endif
                        #if defined(STACK_USE_SNMPV3_SERVER)
                        j |= 0x20;
                        #endif
                        #if defined(STACK_USE_TFTP_CLIENT)
                        j |= 0x40;
                        #endif
                        #if defined(STACK_USE_TELNET_SERVER)
                        j |= 0x80;
                        #endif
                    break;
                    //'gsk0' - TCP/IP stack flags 2
                    case '2':
                        #if defined(STACK_USE_ANNOUNCE)
                        j |= 0x01;
                        #endif
                        #if defined(STACK_USE_DNS)
                        j |= 0x02;
                        #endif
                        #if defined(STACK_USE_DNS_SERVER)
                        j |= 0x04;
                        #endif
                        #if defined(STACK_USE_NBNS)
                        j |= 0x08;
                        #endif
                        #if defined(STACK_USE_REBOOT_SERVER)
                        j |= 0x10;
                        #endif
                        #if defined(STACK_USE_SNTP_CLIENT)
                        j |= 0x20;
                        #endif
                        #if defined(STACK_USE_DYNAMICDNS_CLIENT)
                        j |= 0x40;
                        #endif
                    break;
                    //'gsk0' - TCP/IP stack flags 3
                    case '3':
                        #if defined(STACK_USE_BERKELEY_API)
                        j |= 0x01;
                        #endif
                        #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
                        j |= 0x02;
                        #endif
                        #if defined(STACK_USE_ZEROCONF_MDNS_SD)
                        j |= 0x04;
                        #endif
                    break;
                }
                nzByteToAsciiHexStr(j, (char*)&val[0]);
                valLen=2;
            }
            ref = 0;    //Done
        }
        //gt....
        else if (tag[1] =='t') {
            BYTE* ptr = val;
            valLen = 0;
            //gtt - Current time in HHMMSS format
            if (tag[2] == 't') {
                RTC_TIME tm;
                rtcGetTime(&tm, 1);             //Get time in BCD format
                *ptr++ = '0' + (tm.hour>>4);    //Upper nibble of hour
                *ptr++ = '0' + (tm.hour&0x0f);  //Lower nibble of hour
                *ptr++ = '0' + (tm.min>>4);     //Upper nibble of min
                *ptr++ = '0' + (tm.min&0x0f);   //Lower nibble of min
                *ptr++ = '0' + (tm.sec>>4);     //Upper nibble of sec
                *ptr++ = '0' + (tm.sec&0x0f);   //Lower nibble of sec
                valLen=6;
                //gttd - Current time and date
                if (tag[3] == 'd') {
                    tag[2] = 'd';  //Cause following check for 'gtd' to be triggered, adding date to time
                }
            }
            //gtd - Current date in DDMMYYYY
            if (tag[2] == 'd') {
                RTC_DATE tm;
                rtcGetDate(&tm, 1);             //Get date in BCD format
                *ptr++ = '0' + (tm.day>>4);     //Upper nibble of day
                *ptr++ = '0' + (tm.day&0x0f);   //Lower nibble of day
                *ptr++ = '0' + (tm.month>>4);   //Upper nibble of month
                *ptr++ = '0' + (tm.month&0x0f); //Lower nibble of month
                *ptr++ = '2';                   //Year 20xx
                *ptr++ = '0';
                *ptr++ = '0' + (tm.year>>4);    //Upper nibble of year
                *ptr++ = '0' + (tm.year&0x0f);  //Lower nibble of year
                valLen += 8;
            }
            //gtx - Get firmware build date
            //else if (tag[2] == 'x') {
            //}

            //if (j==1) {
            //    valLen = strlen((const char*)val);
            //}
            ref = 0;    //Done
        }
    }
    /////////////////////////////////////////////////
    //Input Tag
    else if (tag[0] == TAG_INPUT) {
        if (isdigit(tag[1])) {
            //Get "Port ID"
            w.Val = getPortIdForStr((const char*)&tag[1]);
            if (w.Val != IOPORT_ID_NA) {
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nTAG i");
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, w.Val);

                //Read requested port
                if (portReadBitadr(portGetPIN(w.Val))==0) {
                    val[0]='0';
                }
                else {
                    val[0]='1';
                }
                valLen = 1;
                ref = 0;    //Done
            }
        }
    }
    /////////////////////////////////////////////////
    //Configuration Block
    else if (tag[0] == TAG_CONFIG_BLOCK) {
        BYTE retLen;
        BYTE cfgBlkOffset;
        BYTE idx;
        WORD cfgBlkId;

        //tag[1] must be 4 to 6 characters long, and in "2 char upper case hex"
        for (i=1; i<=7; i++) {
            j=tag[i];

            //Character 7 must be NULL. Else, format error!
            if (i==7) {
                if (j!=0) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nTAG k frmt err");
                    return 0;   //Done
                }
                break;  //OK, break
            }

            //Current character is NULL
            if (j==0) {
                //Characters 5 and 6 are optional, and can be NULL
                if (i>4) {
                    break;  //OK, break
                }
                //Error, string too short!
                else {
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nTAG k too short");
                    return 0;   //Done
                }
            }
        }

        //Get number of characters to return
        w.Val = 0;
        w.byte.LB = tag[1];
        retLen = hexatob(w);

        //Get "Config Block" offset
        w.byte.HB = tag[2];
        w.byte.LB = tag[3];
        cfgBlkOffset = hexatob(w);

        //Get "Config Block" address
        cfgBlkId = nzParseHexDecWord((const char*)&tag[4], &idx);

        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCfgBlk ");
        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, retLen);
        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');
        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, cfgBlkOffset);
        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');
        DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, cfgBlkId);
        
        //Ensure requested size can fit into buffer
        if (retLen > CMDGETTAG_VAL_SIZE) {
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nTAG k rqst too much");
            return 0;
        }

        //!!!!!!!!!!!!!!!!!!!!!!! TODO !!!!!!!!!!!!!!!!!!!!!!!
        //Implement the following function
        valLen = cfgBlkGetArray(cfgBlkId, cfgBlkOffset, val, retLen);
    }
    /////////////////////////////////////////////////
    //User Memory Tag
    else if (tag[0] == TAG_USERMEM) {
        //#define TAG_USERMEM_DEBUG_INFO
        #define DATATYPE_HEX    0
        #define DATATYPE_DEC    1
        #define DATATYPE_STR    0xff
        BYTE dataType=0;    //0 if "2 char upper case hex", 1 if Decimal, 2 if String
        WORD adr;
        BYTE j;
        BYTE idx;

        valLen = 0;

        //Parse the requested "User Memeory" address
        adr = nzParseHexDecWord((const char*)&tag[1], &idx);

        //The number of bytes parsed is contained in bits 0-3, clear the rest (flags)
        idx &= 0x0f;
        idx++;          //Get index of first byte after parsed data (we started at name[1], so add 1

        #if defined(TAG_USERMEM_DEBUG_INFO)
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUsrMem 0x");
        DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, adr);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Type=");
        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, tag[idx]);
        #endif

        //Requested a string, Ex. "uFFs"
        if(tag[idx]=='s') {
            i = CMDGETTAG_VAL_SIZE;     //Set to maximum, will terminate when NULL is reached
            dataType = DATATYPE_STR;
            idx++;      //Inc to point to next char in tag
            #if defined(TAG_USERMEM_DEBUG_INFO)
            DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 's');
            #endif
        }
        //Requested a byte array, Ex. "uF5a6"
        else if(tag[idx]=='a') {
            //Get number of bytes requested
            i = nzParseHexDecWord((const char*)&tag[idx+1], &w.byte.LB);
            w.byte.LB &= 0x0f;      //The number of bytes parsed is contained in bits 0-3, clear the rest (flags)

            //DON'T inc index! We use (tag[idx]=='a') below to check if array!!!
            //idx += w.byte.LB;       //Inc to point to next char in tag
            #if defined(TAG_USERMEM_DEBUG_INFO)
            DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'a');
            #endif
        } else {
            i = 1;      //Single byte was requested

            //A WORD was requested, get second byte (Ex. "uF5w")
            if (tag[idx]=='w') {
                i = 2;
                idx++;  //Inc to point to next char in tag
                #if defined(TAG_USERMEM_DEBUG_INFO)
                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'w');
                #endif
            }
            #if defined(TAG_USERMEM_DEBUG_INFO)
            else {
                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'b');
            }
            #endif
        }

        //A Decimal BYTE or WORD was requested (Ex. "uF5wd" or "uF5d")
        if(tag[idx]=='d') {
            idx++;          //Inc to point to next char in tag
            dataType = DATATYPE_DEC;
            #if defined(TAG_USERMEM_DEBUG_INFO)
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Dec");
            #endif
        }

        //Get requested data
        j = 0;
        while(i--) {
            valLen++;

            //A String was requested
            if (dataType == DATATYPE_STR) {
                val[j++] = nzGlobals.userRam[adr++];
                //Next char is 0, END of string has been reached!
                if (nzGlobals.userRam[adr] == 0)
                    break;  //Done
            }
            //Data requested
            else {
                //Decimal data
                if (dataType == DATATYPE_DEC) {
                    //A Decimal BYTE or WORD was requested (i was decremented in loop)
                    if (i<=1) {
                        w.Val = nzGlobals.userRam[adr];    //Get LSB
                        //A Decimal WORD was requested
                        if (i==1) {
                            w.byte.HB = nzGlobals.userRam[++adr];    //Get MSB
                        }
                        nzUintToAsciiStr(w.Val, (BYTE*)&val[0]);
                        valLen = strlen((const char*)&val[0]);
                        break;  //Done
                    }
                    else {
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Dec NOT SPRTD!");
                        break;
                    }
                }
                //Hex data
                else {
                    w.byte.HB =  nzHighNibbleToAsciiHex(nzGlobals.userRam[adr]);
                    w.byte.LB =  nzLowNibbleToAsciiHex(nzGlobals.userRam[adr++]);

                    //Value is Decimal WORD - NOT an Array. Have to first print MSB of WORD!
                    if((i==1) && (tag[idx]!='a')) {
                        //Output High Byte of WORD
                        val[j++] =  nzHighNibbleToAsciiHex(nzGlobals.userRam[adr]);
                        val[j++] =  nzLowNibbleToAsciiHex(nzGlobals.userRam[adr++]);
                        valLen += 2;

                        //Cause loop to end - low byte of WORD will be outputted below
                        i=0;
                    }

                    val[j++] =  w.byte.HB;
                    val[j++] =  w.byte.LB;
                    valLen++;   //Increment once more (first inc above) for array
                }
            }
        }

        ref = 0;    //Done
    }
    /////////////////////////////////////////////////
    //Output Tag
    else if (tag[0] == TAG_OUTPUT) {
        if (isdigit(tag[1])) {
            //Get "Port ID"
            w.Val = getPortIdForStr((const char*)&tag[1]);
            if (w.Val != IOPORT_ID_NA) {
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nTAG o");
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, w.Val);

                //Read requested port
                if (portReadBitadr(portGetLAT(w.Val))==0) {
                    val[0]='0';
                }
                else {
                    val[0]='1';
                }
                valLen = 1;
                ref = 0;    //Done
            }
        }
    }
    /////////////////////////////////////////////////
    //User Tags
#if (USER_TAG_ONLY_X == 1)
    else if (tag[0] == TAG_USER_X) {
#else
    else if ((tag[0]>=TAG_USER_X) && (tag[0]<=TAG_USER_Z)) {
#endif
        #define refReturned w.Val

        //There is a maximum of only 1 "User Tag" listener
        #if (USER_TAG_LISTENERS == 1)
        if (numOfUserTagListeners != 0) {
            refReturned = (*pUserTagListeners[0])(tag, ref, &val[0], user);   //Call listener
        //Call all "User Tag" listeners
        #else
        BYTE i;
        for (i=0; i<numOfUserTagListeners; i++) {
            refReturned = (*pUserTagListeners[i])(tag, ref, &val[0], user);    //Call listener
        #endif

            //If returned Ref != given ref, than a tag was returned.
            if (refReturned != ref) {
                valLen = ((refReturned & 0x7fff) - ref);  //Get number of bytes added to destination
                //All data sent (bit 15 = 0).
                if ((refReturned & 0x8000) == 0) {
                    ref = 0;    //Assume Done
                }
                //There is more data to send (If bit 15 set).
                else {
                    ref = refReturned & 0x7fff;
                }

                #if (USER_TAG_LISTENERS > 1)
                break;
                #endif
            }
        }
    }

    if (valLen) {
        if (webRequest) {
            //Write requested data to current TCP socket
            w.Val = TCPPutArray(sktHTTP, val, valLen);
        }
        else if (dest!=NULL) {
            //Write requested data to circular buffer
            w.Val = cbufPutArray((CIRBUF*)dest, val, valLen);
        }

        //TODO - Add code that writes remaining bytes when this function is called again.
        //Not all bytes were written
        #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
        if (w.Val != valLen) {
            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncmdGetTag() no space!");
        }
        #endif
    }

    return ref;
}


//Functions required for Webserver
#if defined(HAS_WEBSERVER)

/**
 * This function has four states. The first reads a name from the data
 * string returned as part of the POST request.  If a name cannot
 * be found, it returns, asking for more data. Next it reads the value. It then
 * creates a "name=value" string with read name and value.
 * This name=value pair is sent to cmdExecNameValueCmd() for processing.
 *
 * In the case where the expected string is never found, this function
 * will eventually return HTTP_IO_NEED_DATA when no data is left.  In that
 * case, the HTTP2 server will automatically trap the error and issue an
 * Internal Server Error to the browser.
 *
 * @return
 *      HTTP_IO_DONE - the parameter has been found and saved
 *      HTTP_IO_WAITING - the function is pausing to continue later
 *      HTTP_IO_NEED_DATA - data needed by this function has not yet arrived
 */
HTTP_IO_RESULT cmdParseHTTPPost(void)
{
    WORD    lenName = 0;        //Lenght of Name, inluding NULL terminator! For example, "lcd" will be 4
    BYTE*   pValue;

    #define CMDPARSE_SIZE_FILENAME 32   //Size of filename (actual maximum filename size is 2 less because we add '/', and NULL terminator

    //In this function, we use curHTTP.data for the following:
    // - The MPFS Filename of current HTTP requested file, is NULL terminated. We
    //   add '/' to front of filename (is expected by HTTP_REDIRECT).
    // - The 'name', is NULL terminated. Location is ((char*)(curHTTP.data + offsetName)) = &curHTTP.data[CMDPARSE_SIZE_FILENAME]
    // - The 'value', is NULL terminated. Location is ((char*)(curHTTP.data + offsetValue))

	#define SM_POST_CMD_INIT            (0u)
    #define SM_POST_CMD_READ_NAME		(1u)
	#define SM_POST_CMD_READ_VALUE		(2u)
    #define SM_POST_CMD_DONE            (3u)

    while(curHTTP.byteCount != 0) {
        switch(curHTTP.smPost)
        {
            // Find the name
            case SM_POST_CMD_INIT:

                // Load the file name
                curHTTP.data[0] = '/';  //add '/' to front of filename, is expected by HTTP_REDIRECT.
                MPFSGetFilename(curHTTP.file, &curHTTP.data[1], CMDPARSE_SIZE_FILENAME-1);

                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ncmdPost Filename=");
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (const char*)&curHTTP.data[0]);

                curHTTP.smPost++;
                // No break...continue reading value

            // Find the name
            case SM_POST_CMD_READ_NAME:

                // If all parameters have been read, end
                if(curHTTP.byteCount == 0u)
                {
                    curHTTP.smPost = SM_POST_CMD_DONE;
                    continue;
                }

                lenName = curHTTP.byteCount;   //Store length before reading "name"
                if(HTTPReadPostName(&curHTTP.data[CMDPARSE_SIZE_FILENAME], HTTP_MAX_DATA_LEN-CMDPARSE_SIZE_FILENAME-2) == HTTP_READ_INCOMPLETE) {
                    return HTTP_IO_NEED_DATA;
                }

                lenName = lenName - curHTTP.byteCount;  //Lenght of Name, inluding NULL terminator! For example, "lcd" will be 4

                curHTTP.smPost++;
                // No break...continue reading value

            // Found the value, so store the LCD and return
            case SM_POST_CMD_READ_VALUE:
                pValue = &curHTTP.data[CMDPARSE_SIZE_FILENAME] + lenName;

                // Read a value string
                if(HTTPReadPostValue(pValue, HTTP_MAX_DATA_LEN-CMDPARSE_SIZE_FILENAME-lenName-2) == HTTP_READ_INCOMPLETE) {
                    return HTTP_IO_NEED_DATA;
                }

                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, ", name(");
                DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, lenName);
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, ")=");
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (const char*)&curHTTP.data[CMDPARSE_SIZE_FILENAME]);

                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, ", value=");
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (const char*)pValue);

                //Process "name=value". The 3rd parameter is the filename, pass it from [1] and not [0] -
                //we do not include the leading '/' characters in the passed filename
                cmdExecNameValueCmd((char*)&curHTTP.data[CMDPARSE_SIZE_FILENAME], (char*)pValue, (char*)&curHTTP.data[1], 0);

                //Return to reading names
                curHTTP.smPost = SM_POST_CMD_READ_NAME;
                continue;
            // Done! There are no more name-value pairs to read
            case SM_POST_CMD_DONE:
                //Once done reading all name-value pairs, so callback is done
                //The HTTP_REDIRECT state will expect the filename in curHTTP.data[0], starting with a '/'
                //character. We already copied the filename there in the SM_POST_CMD_INIT state.
                //strcpypgm2ram((char*)curHTTP.data, "/forms.htm");
                curHTTP.httpStatus = HTTP_REDIRECT;
                return HTTP_IO_DONE;
        }
    }

	// Default assumes that we're returning for state machine convenience.
	// Function will be called again later.
	return HTTP_IO_WAITING;
}



/**
 * Get the value of the given string, applying any leading value format specifier.
 * String is a upper case hex representation of a value. Following possible leading
 * format specifiers:
 * - d = Decimal value to follow
 *
 * @param str String containing hex value
 *
 * @return Parsed value, or 0x80000000 if error
 */
//DWORD parseAsciiVal(const char* str) {
//    #define DECIMAL 0x01
//    #define INVALUE 0x80    //We are currently in value part of string
//    DWORD val = 0;
//    BYTE i;
//    BYTE flags=0;
//    BYTE idx=0;
//    char c,ch;
//    const char* p;
//    DWORD mulplr=1;
//
//    p = str;
//
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nparseAsciiVal()");
//
//    while(1) {
//        c = *p++;
//
//        //We are currently parsing the leading format specifiers of the string
//        if ( (flags & INVALUE) == 0) {
//            if (c=='d') {
//                flags |= DECIMAL;
//                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-d");
//                continue;
//            }
//            else if ( (((c>='0')&&(c<='9')) || ((c>='A')&&(c<='F'))) == 0) {
//                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "ERR: parseAsciiVal()!");
//                return 0;
//            }
//
//            //We are now in the Value part of the string
//            flags |= INVALUE;
//            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-inVal");
//
//            //Skip all leading 0s, and update c each time
//            while(c == '0') {
//                c = *p++;
//                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " r");
//            }
//
//            //Count lenght of value part of string.
//            // - c currently contains first NON-zero number value
//            // - p currently point to character after first NON-zero number value
//            ch = c;
//            while((((ch>='0')&&(ch<='9')) || ((ch>='A')&&(ch<='F'))) != 0) {
//                //Get next character
//                ch = p[idx++];
//            }
//            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " len=");
//            DEBUG_PUT_BYTE(DEBUG_LEVEL_INFO, idx);
//
//            if (idx==0)
//                return 0;
//        }
//
//        if (flags&DECIMAL) {
//            mulplr=1;
//            for(i=idx-1; i!=0; i--) {
//                mulplr *= 10;
//            }
//            val = val + ((c-'0')*(mulplr));
//        }
//        else {
//            //valStr[idx] = (c<='9')?(c-'0'):(c-'A'+10);
//            val = val + (((c<='9')?(c-'0'):(c-'A'+10)) << ((idx-1)*4));
//        }
//
//        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " lp");
//
//        //Are we done?
//        if (--idx == 0)
//            break;
//    };
//
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-v=");
//    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, val);
//}


#endif  //#if defined(HAS_WEBSERVER)


/**
 * Writes a single byte to the "User RAM". This function checks the given ptr is located
 * within valid the "User RAM", and returns 0 if all OK.
 *
 * @param c Byte to write
 * @param ptr Pointer to "User RAM" position to write to
 *
 * @return Returns 0 if OK, else non zero
 */
BYTE cmdWriteByteUserRam(BYTE c, BYTE* ptr) {
    if ( (ptr < (&nzGlobals.userRam[0])) || (ptr > (&nzGlobals.userRam[USER_RAM_SIZE-1])) ) {
        return -1;
    }
    *ptr = c;
    return 0;
}


/**
 * Add a "Command Listener". All listeners (until command is consumed) will be called
 * if a "User Command" is received.
 * 
 * @param ptrCmdListener
 * @return 
 */
BYTE cmdAddCmdListener(WORD (*ptrCmdListener)(char*, char*, void*, WORD)) {
    if (numOfUserCmdListeners < USER_CMD_LISTENERS) {
        pUserCmdListeners[numOfUserCmdListeners++] = ptrCmdListener;
    }
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    else {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "Cmd Listener array full!");
    }
    #endif

    return 0;
}

/**
 * Add a "Tag Listener". All listeners (until command is consumed) will be called
 * if a "User Tag" is requested.
 *
 * @param ptrTagListener
 * @return
 */
BYTE cmdAddTagListener(WORD (*ptrTagListener)(BYTE*, WORD, BYTE*, BYTE)) {
    if (numOfUserTagListeners < USER_TAG_LISTENERS) {
        pUserTagListeners[numOfUserTagListeners++] = ptrTagListener;
    }
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    else {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "Tagd Listener array full!");
    }
    #endif

    return 0;
}

/**
 * Reboot board
 */
void cmdReboot(void) {
    WORD j;
    //Reset board
    U1CON = 0x0000; //Disable USB module
    //And wait awhile for the USB cable capacitance to discharge down to disconnected (SE0) state.
    //Otherwise host might not realize we disconnected/reconnected when we do the reset.
    for (j = 0; j < 0xFFFF; j++) {
        Nop();
    }
    Reset();
}
