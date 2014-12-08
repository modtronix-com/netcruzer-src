/*********************************************************************
 * serportNet.c     Network Serial ports, UDP and TCP Client and Server code
 * Complier:        MPLAB XC16
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 * Network Serial ports, UDP and TCP Client and Server code
 *
 *********************************************************************
 * File History
 *
 * 2014-06-06, David Hosken (DH):
 *    - Initial version
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
#define THIS_IS_SERPORT_NET

#include "HardwareProfile.h"

#if defined(HAS_SERPORT_NET)

//Includes required for Webserver
#if defined(HAS_WEBSERVER)
#include "TCPIPConfig.h"
#include "TCPIP Stack/HTTP2.h"
#endif

#include "nz_serDataPorts.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"
#include "cmd.h"
#include "serportNet.h"

//Add debugging to this file. The DEBUG_CONF_SERPORT_NET macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_SERPORT_NET)
    #define DEBUG_CONF_SERPORT_NET        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_SERPORT_NET
#include "nz_debug.h"


////////// Default Defines //////////////////////
// Local Server port
#if !defined(REMOTE_SERVER_PORT)
#define REMOTE_SERVER_PORT       (54121ul)
#endif

// Remote Server URL and port
#if !defined(SERVER_PORT)
#define SERVER_PORT             (54120ul)
#endif


////////// Defines //////////////////////////////
static enum {
    SM_SRVR_IDLE = 0,
    SM_SRVR_OPEN_SOCKET,
    SM_SRVR_UDP_LISTEN,
    SM_SRVR_TX_SOMETHING,
    SM_SRVR_WAIT
} smServer = SM_SRVR_IDLE;


////////// Variables ////////////////////////////


////////// Function prototypes //////////////////


/**
 * Initializes Serport Net module
 */
void serNetInit(void) {
}


/**
 * Must be called every couple of ms.
 *
 */
void serNetTask(void) {
}


/**
 * UDP Server, listens for messages
 */
void serverTask(void) {
    WORD                w, i;
    char* ptr;
    char str[64];
    static WORD         timeout;
    static UDP_SOCKET   mySocket = INVALID_UDP_SOCKET;
    static DWORD        data[8];
    static DWORD        dwLastIP = 0;

    switch(smServer) {
        case SM_SRVR_IDLE:
            // Check for link before continuing
        	if(MACIsLinked() == FALSE) {
                timeout = tick16Get() + tick16ConvertFromMS(250); //Set Timeout
                smServer = SM_SRVR_WAIT;
                break;
            }

            // If DHCP is enabled, wait until DHCP module is finished
            #if defined(STACK_USE_DHCP_CLIENT)
            if (AppConfig.Flags.bIsDHCPEnabled == TRUE) {
                if(DHCPIsBound(0) == FALSE)
                    break;
            }
            #endif
            if (dwLastIP != AppConfig.MyIPAddr.Val) {
                dwLastIP = AppConfig.MyIPAddr.Val;
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nServer New IP");
                mySocket = INVALID_UDP_SOCKET;
            }

            smServer++;
            break;
        case SM_SRVR_OPEN_SOCKET:
            if(mySocket == INVALID_UDP_SOCKET) {
                //Obtain an UDP socket to listen/transmit on
                mySocket = UDPOpenEx(0, UDP_OPEN_SERVER, REMOTE_SERVER_PORT, REMOTE_SERVER_PORT);

                //If failed to create a socket, wait and try again
                if(mySocket == INVALID_UDP_SOCKET) {
                    timeout = tick16Get() + tick16ConvertFromMS(5000);    //Set Timeout
                    smServer = SM_SRVR_WAIT;
                    break;
                }
            }
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nSrvr Listen");
            smServer++;
            break;
        case SM_SRVR_UDP_LISTEN:
        {
/*
            WORD strLen;

            // Check for link before continuing
        	if(MACIsLinked() == FALSE) {
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nServer Lost Link");
                UDPClose(mySocket);
                smServer = SM_SRVR_IDLE;
                mySocket = INVALID_UDP_SOCKET;
                break;
            }

            if(mySocket == INVALID_UDP_SOCKET) {
                smServer = SM_SRVR_IDLE;
            }

            // Look for a response time packet
            if(UDPIsGetReady(mySocket)) {
                //BYTE i;
                w = UDPGetArray((BYTE*)&str, sizeof(str));
                #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
                debugPutString("\nUDP Srvr Rxed '");
                debugPutString(str);
                debugPutChar('\'');
                //Decode message
                //for (i=0; i<strLen; i++) {
                //    debugPutString(" 0x");
                //    debugPutHexByte( ((BYTE*)(&msgFromSrvr))[i] );
                //}
                #endif
                //UDPDiscard();

                //Get length of command string. That is string up till AND including ';' character
                //For example "1,10,35;" will be 8
                ptr = strchr(str, ';');
                if (ptr==NULL)
                    strLen=0;
                else
                    strLen = ptr-str+1;
                //Encode received message to data[] array of DWORDs
                //Ensure NULL terminated string received, string length MUST be less than sizeof(str)
                if ((strLen<sizeof(str)) && (strLen>0)) {
                    BYTE commas=0;
                    BOOL done = FALSE;

                    //Replace ';' with NULL
                    ptr = str;
                    //Count number of ',' characters AND reply with NULLs
                    for (i=0; (i<strLen) && !done; i++) {
                        if((str[i]==',') || (str[i]==';')) {
                            if (str[i]==';')
                                done=TRUE;
                            if(commas < 8) {
                                str[i] = '\0';
                                data[commas] = atoi(ptr);
                                ptr = str+i+1;
                                commas++;
                            }
                        }
                    }

                    //Process message
                    if (data[0] == MSG_TYPE_TEST) {

                        //Transmit config echo
                        smServer = SM_SRVR_TX_SOMETHING;
                        break;
                    }
                    //else if (data[0] == MSG_TYPE_...) {
                    //}
                    else {
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nMsg ignored!");
                    }
                }
                else {
                    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR!!! Rxed invalid packet!");
                    UDPDiscard();   //Discard rest of UDP data, if any
                }
            }
*/
            break;    //No break
        }
        case SM_SRVR_TX_SOMETHING:
            smServer = SM_SRVR_IDLE;
            break;
        case SM_SRVR_WAIT:
            //Wait for timeout
            if (tick16TestTmr(timeout)) {
                UDPClose(mySocket);
                smServer = SM_SRVR_IDLE;
                mySocket = INVALID_UDP_SOCKET;
            }
            break;
    }
}

#endif  //#if defined(HAS_SERPORT_NET)