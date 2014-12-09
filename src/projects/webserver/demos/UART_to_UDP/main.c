 /**
 * @example demos/PT66DIN6_IO_Demo1/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use the <a href="http://netcruzer.com/sbc66ec.html/">SBC66EC</a> (SBC66ECL) board together with
 * the <a href="http://netcruzer.com/pt66din6/">PT66DIN6</a> DIN Rail Main board, and the
 * <a href="http://netcruzer.com/dins8r.html/">DINS-8R</a> 8 Relay DIN Rail Module.
 * The PT66DIN6 is a DIN Rail mountable board:
 *  - It takes a SBC66 board as a daughter board
 *  - It has 6 iMod ports for adding any of our available <a href="http://netcruzer.com/products-imod/">iMod Modules</a>
 *  - It has a Sub-D 9 Pin connector for adding additional serial DINS I/O module, like the DINS-8R 8-Relay module for example.
 *
 *
 * <h2>===== Required Hardware =====</h2>
 * This demo requires the following hardware:
 * - The <a href="http://netcruzer.com/sbc66ec.html/">SBC66EC</a> Single Board Computer. It can however easily be compiled
 *    to run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>
 * - The <a href="http://netcruzer.com/pt66din6/">PT66DIN6</a> DIN Rail Main board
 * - The <a href="http://netcruzer.com/dins8r.html/">DINS-8R</a> 8 Relay DIN Rail Module
 * - The <a href="http://netcruzer.com/im2bl.html/">im2BL</a> 2 Button/LED iMod module. This module is not required, and a
 *   button and LED can be connected to the pins of the iMod port via a 1k resistor to 0V.
 * - NOT REQUIRED, but various <a href="http://netcruzer.com/products-imod/">iMod Modules</a> can be very handy for prototyping
 *
 * <h2>===== Building Project =====</h2>
 * This demo is part of the main Webserver project, and is located in the "src/projects/webserver/demos/PT66DIN6_IO_Demo1" folder of the
 * <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer Download</a>. To compile, open the Webserver project
 * in MPLAB X, and select the "Project Configuration" that includes "_PT66DIN6_IO_DEMO1" in it's name. For example, to build firmware
 * for the <a href="http://netcruzer.com/sbc66ec.html">SBC66EC</a> board, select "SBC66EC_PT66DIN6_IO_DEMO1".
 * For details <a target="_blank" href="http://netcruzer.com/nz/doc/building_projects/">click here</a>
 *
 * <h2>===== Programming Board =====</h2>
 * After compiling (build), the board can be programmed via the <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb">
 * USB Bootloader</a> or a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_prog">PIC Programmer</a>. USB Programming is
 * simplified when using the SBC board together with a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb_pt66">Prototype Board</a>.
 *
 * <h2>===== File History =====</h2>
 * 2012-10-28, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

#if defined(UART_TO_UDP_DEMO1)

//Netcruzer include files
#include "HardwareProfile.h"
#include "main.h"
#include "mainApp.h"
#include "nz_serDataPorts.h"
#include "nz_debounce.h"
#include "nz_ioPorts.h"
#include "nz_serUart.h"
#include "cmd.h"
#include "TCPIP Stack/TCPIP.h"

#define USE_AND_OR /* To enable AND_OR mask setting */
#include "uart.h"

//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"

// Function Prototypes //////////////////////////
void processIO(void);
WORD processCmd(char * name, char * value, void* param1, WORD flags);
WORD processTag(BYTE* tag, WORD ref, BYTE* dest, BYTE user);


// Defines //////////////////////////////////////
#define MSG_FOR_SRVR_SIZE 32


// Variables ////////////////////////////////////
static WORD tmrProcessIO = 0;   //Timer definining LED flash rate (16-bit, 1ms Timer)

// Local Server port
#define LOCAL_SERVER_PORT       (54121ul)

// Remote Server URL and port
#define SERVER_PORT             (54120ul)
//#define SERVER_PORT			(123ul)     //Test values for NTP server
//#define SERVER_URL            "pool.ntp.org"
#define SERVER_URL              "chsplt.cloudapp.net"

// Defines how long to wait before assuming the query has failed (in milliseconds)
#define SERVER_REPLY_TIMEOUT	(4000)


//Defines the structure of an the message to the serverNTP packet
typedef struct
{
	BYTE msg[8];
} MSG_FROM_SERVER;
static BOOL rcvdMsgFromServer;    //Flag to indicate we must send message to server

//Defines the structure of an the message to the serverNTP packet
typedef struct
{
	BYTE rtuID;
    BYTE battLevel;
    BYTE signalStrength;
    BYTE logs;
    WORD timestamp;
} MSG_TO_SERVER;
static BOOL sendMsgToServer;    //Flag to indicate we must send message to server


// Function Prototypes //////////////////////////
void msgToServerTask(void);
void serverTask(void);


/**
* Main application entry point.
*/
int main(void) {
    initMainApp();        //Initialize the main Webserver application

    tmrProcessIO = tick16Get() + tick16ConvertFromMS(600); //Startup delay of 600mS

    //Add "User Command" listener, will call cmdProcessing() function
    cmdAddCmdListener(&processCmd);

    //Add "User Command" listener, will call cmdProcessing() function
    cmdAddTagListener(&processTag);

    sendMsgToServer = 0;
    rcvdMsgFromServer = 0;

    debugPutString("\nConfiguring");
    //Assign pins use for iMod 1 TX, RX, RTS and CTS on PT66EI and PT66ECI boards
    //PPSUnLock;  // Unlock PPS, called in nzSysInitDefault()!
    iPPSOutput(PPS_OUT_06, OUT_FN_PPS_U1TX);        //6 (old port names X6) is TX
    iPPSOutput(PPS_OUT_08, OUT_FN_PPS_U1RTS);       //8 (old port names X8) is RTS
    iPPSInput(IN_FN_PPS_U1RX, PPS_IN_07);           //7 (old port names X7) is RX
    iPPSInput(IN_FN_PPS_U1CTS, PPS_IN_01);          //1 (old port names X1) is CTS
    CloseUART1();

    serUart1Init();

    //UART initialized, 9600 Buad, 8bit, no parity, 1 stop bit, High BAUD rate
    //Ensure USER_CONFIGURES_UART1 define is set to 1! Else, system will configure UART1 with default values!
    OpenUART1(UART_EN | UART_NO_PAR_8BIT | UART_1STOPBIT | UART_UEN_00 | UART_BRGH_FOUR , UART_TX_ENABLE, UART_BAUD_9K6);

	PPSLock;    // Lock PPS once all initialization is done
	
    //Now that all items are initialized, begin the co-operative multitasking loop. This infinite loop will
    //continuously execute all stack-related tasks, as well as your own application's functions.  Custom
    //functions should be added at the end of this loop. Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot or part of it) and returns so that other tasks
    //can do their job. If a task needs very long time to do its job, it must be broken down into smaller pieces
    //so that other tasks can have CPU time.
    while (1) {
        mainAppTask();      //Main webserver task
        processIO();
        msgToServerTask();
        serverTask();
    }
}


/**
 * I/O functions
 */
void processIO(void) {

    //Print any data received via UART1
    while (serUart1HasData()) {
        debugPutString("\nReceiveddd 0x");
        debugPutHexByte(serUart1Read());
    }

    //Enter every 500ms
    if (tick16TestTmr(tmrProcessIO)) {
        tick16UpdateTmrMS(tmrProcessIO, 500); //Update timer to expire in 500ms again

        //serUart1Write('B');

        //BYTE bufMsg[] = {0x80, 0};
        //bufMsg[2] = relays++;
        //cbufPutPacket(CIRBUF_TX_I2C1, bufMsg, strlen((const char*)bufMsg));
        //i2c1WriteMessage(0x60, bufMsg, strlen((const char*)bufMsg));
    }
}

void serverTask(void) {
    MSG_FROM_SERVER     msgFromSrvr;
    WORD                w;
    static WORD         timeout;
    static UDP_SOCKET   mySocket = INVALID_UDP_SOCKET;
    static enum {
        SM_SRVR_IDLE = 0,
        SM_SRVR_OPEN_SOCKET,
        SM_SRVR_UDP_LISTEN,
        SM_SRVR_WAIT
    } smMsg2Svr = SM_SRVR_IDLE;

    switch(smMsg2Svr)
    {
        case SM_SRVR_IDLE:
            smMsg2Svr++;
            //break;
        case SM_SRVR_OPEN_SOCKET:
            if(mySocket == INVALID_UDP_SOCKET) {
                //Obtain an UDP socket to listen/transmit on
                mySocket = UDPOpenEx(0, UDP_OPEN_SERVER, LOCAL_SERVER_PORT, LOCAL_SERVER_PORT);

                //If failed to create a socket, wait and try again
                if(mySocket == INVALID_UDP_SOCKET) {
                    timeout = tick16Get() + tick16ConvertFromMS(5000);    //Set Timeout
                    smMsg2Svr = SM_SRVR_WAIT;
                    break;
                }
            }
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nSrvr Listen");
            smMsg2Svr++;
            break;
        case SM_SRVR_UDP_LISTEN:
            if(mySocket == INVALID_UDP_SOCKET) {
                smMsg2Svr = SM_SRVR_IDLE;
            }
            // Look for a response time packet
            if(UDPIsGetReady(mySocket)) {
                BYTE i;
                // Get the response time packet
                w = UDPGetArray((BYTE*)&msgFromSrvr, sizeof(msgFromSrvr));
                #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
                debugPutString("\nUDP Srvr Rxed ");
                debugPutByte(w);
                debugPutString(" bytes:");
                for (i=0; i<w; i++) {
                    debugPutString(" 0x");
                    debugPutHexByte( ((BYTE*)(&msgFromSrvr))[i] );
                }
                #endif
                //UDPDiscard();
            }
            break;    //No break
        case SM_SRVR_WAIT:
            //Wait for timeout
            if (tick16TestTmr(timeout)) {
                UDPClose(mySocket);
                smMsg2Svr = SM_SRVR_IDLE;
                mySocket = INVALID_UDP_SOCKET;
            }
            break;
    }
}

void msgToServerTask(void) {
    MSG_TO_SERVER       msgToSrvr;
    MSG_FROM_SERVER     msgFromSrvr;
    WORD                w;
    static WORD         timeout;
    static UDP_SOCKET   mySocket = INVALID_UDP_SOCKET;
    static enum {
        SM_M2S_IDLE = 0,
        SM_M2S_UDP_OPEN,
        SM_M2S_UDP_IS_OPENED,
        SM_M2S_UDP_SEND,
        SM_M2S_UDP_RECV
    } smMsg2Svr = SM_M2S_IDLE;

    switch(smMsg2Svr)
    {
        case SM_M2S_IDLE:
            //Is there a message to send to the server
            if (sendMsgToServer==TRUE) {
                smMsg2Svr++;
            }
            break;
        case SM_M2S_UDP_OPEN:
            if(mySocket == INVALID_UDP_SOCKET) {
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUDP Open");
                //mySocket = UDPOpenEx((DWORD)(PTR_BASE)SERVER_URL, UDP_OPEN_ROM_HOST, 0, SERVER_PORT);
                mySocket = UDPOpenEx((DWORD)(PTR_BASE)SERVER_URL, UDP_OPEN_ROM_HOST, SERVER_PORT, SERVER_PORT);
                timeout = tick16Get();    //Set Timeout
            }
            smMsg2Svr++;
            break;
        case SM_M2S_UDP_IS_OPENED:
            if(UDPIsOpened(mySocket) == FALSE) {
                //Check if a timeout occurred - 5 Seconds (5000ms)
                if (tick16TestTmr(timeout + tick16ConvertFromMS(5000))) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUDP Open Timeout!");
                    UDPClose(mySocket);
                    smMsg2Svr = SM_M2S_IDLE;
                    mySocket = INVALID_UDP_SOCKET;
                }
                break;
            }
            smMsg2Svr = SM_M2S_UDP_SEND;
            //break;    //No break
        case SM_M2S_UDP_SEND:
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUDP Opened");
            // Make certain the socket can be written to
            if(UDPIsPutReady(mySocket) == FALSE)
            {
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUDP Put NOT ready");
                UDPClose(mySocket);
                smMsg2Svr = SM_M2S_UDP_OPEN;
                mySocket = INVALID_UDP_SOCKET;
                break;
            }

            // Transmit a time request packet
            memset(&msgToSrvr, 0, sizeof(msgToSrvr));
            msgToSrvr.rtuID = 0x5A;
            UDPPutArray((BYTE*) &msgToSrvr, sizeof(msgToSrvr));
            UDPFlush();
            sendMsgToServer = FALSE;    //Message has been sent
            timeout = tick16Get();        //Set Timeout
            smMsg2Svr = SM_M2S_UDP_RECV;
            break;
        case SM_M2S_UDP_RECV:
            // Look for a response time packet
            if(!UDPIsGetReady(mySocket))
            {
                //Check if a timeout occurred
                if (tick16TestTmr(timeout + tick16ConvertFromMS(SERVER_REPLY_TIMEOUT))) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUDP Rcv Timeout!");
                    // Abort the request and wait until the next timeout period
                    UDPClose(mySocket);
                    smMsg2Svr = SM_M2S_IDLE;
                    mySocket = INVALID_UDP_SOCKET;
                    break;
                }
                break;
            }

            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUDP Responce ");

            // Get the response time packet
            w = UDPGetArray((BYTE*)&msgFromSrvr, sizeof(msgFromSrvr));
            UDPClose(mySocket);

            // Validate packet size
            //if(w != sizeof(msgToSrvr)) {
            //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nUDP Rcv size error!");
            //    break;
            //}

            #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
            {
                BYTE i;
                debugPutByte(w);
                debugPutString(" bytes:");
                for (i=0; i<w; i++) {
                    debugPutString(" 0x");
                    debugPutHexByte( ((BYTE*)(&msgFromSrvr))[i] );
                }
            }
            #endif

            rcvdMsgFromServer = TRUE;
            smMsg2Svr = SM_M2S_IDLE;
            mySocket = INVALID_UDP_SOCKET;
            break;
    }
}


/**
 * Callback if a "User Command" is received.
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
 */
WORD processCmd(char * name, char * value, void* param1, WORD flags) {
    //Received user command "testCmd"
    if (memcmp(&name[0], "xMyCmd", 6)==0) {
        //Do something.......
    }

    return 0;
}

/**
 * Callback if a "User Tag" is received.
 *
 * @param tag Pointer to string containing tag
 *
 * @param ref Reference used by this function to determine if it is the first call, or
 *      if it should continue from last return. If 0, this is the first call.
 *
 * @param dest The destination to write data to, has size of CMDGETTAG_VAL_SIZE
 *
 * @param user The user level required to process this request. Is a USER_XX constant
 *
 * Return Bit 0-14 = ref, bit 15 = more data. Bytes added to dest = ((returned bits 0-14) - (ref parameter)).
 *          If returned value = given 'ref parameter', no tag was returned
 */
WORD processTag(BYTE* tag, WORD ref, BYTE* dest, BYTE user) {
    //Received user tag "testTag"
    if (memcmp(&tag[0], "testTag", 7)==0) {
        //Return a value.....return 'hi' for this example
        strcpy((char*)dest, "hi");
        return ref+2;   //Return length of returned value = 2 for "hi"
    }

    return ref;
}


#if defined(HAS_NZ_DEBUGGING) && (HAS_DEMO_DEBUG_HANDLER == 1)
/**
 * Debug service routine. This function is required when debugging is enabled. It is
 * called from the system! We have to process any received debug message.
 */
void demoDebugService(void) {
    BOOL found = FALSE;

    //Check if a debug message was received
    if (cbufHasWholePacket(CIRBUF_RX_DEBUG)) {
        //'send' - Send packet to server
        if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "send") == 0) {
            found = TRUE;
            sendMsgToServer = TRUE;
        }
        //'tx' - Send test byte on UART1
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "tx") == 0) {
            found = TRUE;
            serUart1Write('B');
        }

        //Remove received packet. If it was not processed above, it is lost!
        if (found) {
            cbufRemovePacket(CIRBUF_RX_DEBUG);
        }
    }
}
#endif

#endif  //#if defined(UART_TO_UDP_DEMO1)