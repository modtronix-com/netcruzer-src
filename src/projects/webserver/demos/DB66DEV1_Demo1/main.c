/**
 * @example demos/DB66DEV1_Demo1/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use the <a href="http://netcruzer.com/sbc66ec.html/">SBC66EC</a> (SBC66ECL) board together with
 * the <a href="http://netcruzer.com/db66dev1/">DB66DEV1</a> Development daughter board.
 * The DB66DEV1 is a Development daughter board that plugs into the daughter board connector of the SBC66 board. It has:
 *  - 8 LEDs
 *  - 4 Buttons
 *  - A Buzzer
 *  - A potentiometer
 *  - Lots of pin headers making port pins available
 *
 *
 * <h2>===== Required Hardware =====</h2>
 * This demo requires the following hardware:
 * - The <a href="http://netcruzer.com/sbc66ec.html/">SBC66EC</a> Single Board Computer. It can however easily be compiled
 *    to run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>
 * - The <a href="http://netcruzer.com/db66dev1/">DB66DEV1</a> Development daughter board.
 *
 * <h2>===== Building Project =====</h2>
 * This demo is part of the main Webserver project, and is located in the "src/projects/webserver/demos/DB66DEV1_Demo1" folder of the
 * <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer Download</a>. To compile, open the Webserver project
 * in MPLAB X, and select the "Project Configuration" that includes "_DB66DEV1_Demo1" in it's name. For example, to build firmware
 * for the <a href="http://netcruzer.com/sbc66ec.html">SBC66EC</a> board, select "SBC66EC_DB66DEV1_Demo1".
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

//Netcruzer include files
#include "HardwareProfile.h"
#include "main.h"
#include "mainApp.h"
#include "nz_serDataPorts.h"
#include "nz_db66dev1.h"
//#include "nz_debounce.h"
#include "nz_ioPorts.h"
#include "nz_rtc.h"
#include "cmd.h"
#include "TCPIPConfig.h"


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
extern void uitoa(WORD Value, BYTE* Buffer);

// Variables ////////////////////////////////////
static WORD tmrProcessIO = 0;		//Timer definining LED flash rate (16-bit, 1ms Timer)
static WORD tmrServiceDB66DEV1 = 0; //Timer for checking button and Potentiometer inputs


/**
* Main application entry point.
*/
int main(void) {
    initMainApp();        //Initialize the main Webserver application

    tmrProcessIO = tick16Get() + tick16ConvertFromMS(600); //Startup delay of 600mS

    db66dev1_Init();

    //portWriteBitadr(DIR_20);

    //Add "User Command" listener, will call cmdProcessing() function
    cmdAddCmdListener(&processCmd);

    //Add "User Command" listener, will call cmdProcessing() function
    cmdAddTagListener(&processTag);

    //Now that all items are initialized, begin the co-operative multitasking loop. This infinite loop will
    //continuously execute all stack-related tasks, as well as your own application's functions.  Custom
    //functions should be added at the end of this loop. Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot or part of it) and returns so that other tasks
    //can do their job. If a task needs very long time to do its job, it must be broken down into smaller pieces
    //so that other tasks can have CPU time.
    while (1) {
        mainAppTask();      //Main webserver task
        processIO();

		//!!!!! ENABLE FOLLOWING CODE IF USING WITH DB66DEV1 DEVELOPMENT BOARD !!!!!
        //Service DB66DEV1 every 10ms
        if (tick16TestTmr(tmrServiceDB66DEV1)) {
            tick16UpdateTmrMS(tmrServiceDB66DEV1, 10);   //Update timer to expire in 10ms again
            db66dev1_Service();						//DB66DEV1 service routine. Must be called each 10ms (default, can be changed)
		}
    }
}


/**
 * I/O functions
 */
void processIO(void) {
    //static BYTE leds = 0x01;
    //static BYTE ledPortID = UCPORT_ID_06;

    //static BYTE ledPortID = 6;
    static BYTE ledPortID = 26;
    static WORD portBitadr = LAT_06_BITADR;

    //Enter every 500ms
    if (tick16TestTmr(tmrProcessIO)) {
        tick16UpdateTmrMS(tmrProcessIO, 500); //Update timer to expire in 500ms again

        /////////////////////////////////////////////////
        //Rotate LEDs 26 to 29 (old port names T4 to T7) using "Port ID"
        //Clear previous bitAdr
        portWriteBitadr(portBitadr, 0);

        //Rotate LEDs 26 to 29 (old port names T4 to T7) using "Port ID"
        if (ledPortID++ == 29)
            ledPortID = 26;
        portBitadr = portGetLAT(ledPortID);

        //Set bitAdr
        portWriteBitadr(portBitadr, 1);

        /////////////////////////////////////////////////
        //Check button inputs on port 30, 31, 32 and 33 (old port names Y0, Y1, Y2 and Y3)
        //portWri
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
    //debugPutString("\ncmdProcessing() ");
    //debugPutString(name);
    //debugPutChar('=');
    //debugPutString(value);
    
    WORD bitadrLAT = BITADR_NA;
    BOOL bVal;  //Get boolean representation for first character of value = 0 or 1


    //Command start with 'xled', Can be the tag xled1, xled2, xled3....
    if (memcmp(&name[0], "xled", 4)==0) {
        switch(name[4]) {
            case '0':
                bitadrLAT = LAT_SYSLED_BITADR;  //Get
                break;
            case '1':
                bitadrLAT = LAT_06_BITADR;
                break;
            case '2':
                bitadrLAT = LAT_07_BITADR;
                break;
            case '3':
                bitadrLAT = LAT_08_BITADR;
                break;
            case '4':
                bitadrLAT = LAT_09_BITADR;
                break;
            case '5':
                bitadrLAT = LAT_26_BITADR;
                break;
            case '6':
                bitadrLAT = LAT_27_BITADR;
                break;
            case '7':
                bitadrLAT = LAT_28_BITADR;
                break;
            case '8':
                bitadrLAT = LAT_29_BITADR;
                break;
        }

        bVal = (value[0]=='0'?0:1);
        if (bitadrLAT != BITADR_NA) {
            //If '2', toggle
            if (value[0]=='2')
                bVal = !portReadBitadr(bitadrLAT);
            portWriteBitadr(bitadrLAT, bVal);
        }
    }

    return 0;
}

const char UP_ARROW[] = "up";
const char DOWN_ARROW[] = "dn";


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
    BYTE val = 0xff;
    //debugPutString("\ntagProcessing() - ");
    //debugPutString(tag);

    //Tag start with 'xled', Can be the tag xled1, xled2, xled3....
    if (memcmp(&tag[0], "xled", 4)==0) {
        switch(tag[4]) {
            case '0':
                val = LAT_SYSLED;
                break;
            case '1':
                val = LAT_06;   //Port 6 (old port name X6)
                break;
            case '2':
                val = LAT_07;   //Port 7 (old port name X7)
                break;
            case '3':
                val = LAT_08;   //Port 8 (old port name X8)
                break;
            case '4':
                val = LAT_09;   //Port 9 (old port name X9)
                break;
            case '5':
                val = LAT_26;   //Port 26 (old port name T4)
                break;
            case '6':
                val = LAT_27;   //Port 27 (old port name T5)
                break;
            case '7':
                val = LAT_28;   //Port 28 (old port name T6)
                break;
            case '8':
                val = LAT_29;   //Port 29 (old port name T7)    
                break;
        }
        //If a 'xled' tag was found
        if (val != 0xff) {
            *dest = val==0?'0':'1';
            return ref+1;
        }
    }
    else if (memcmp(&tag[0], "xbtn", 4)==0) {
        switch(tag[4]) {
            case '1':
                val = PIN_30;
                break;
            case '2':
                val = PIN_31;
                break;
            case '3':
                val = PIN_32;
                break;
            case '4':
                val = PIN_33;
                break;
        }
        //If a 'xbtn' was tag was found
        if (val != 0xff) {
            //Write "up" or "dw" string to dest
            strcpy((char*)dest, (val?UP_ARROW:DOWN_ARROW));
            return ref+2;   //Return ref+2, indicating that the string written to dest was 2 characters long.
        }
    }
    else if (memcmp(&tag[0], "xpot1", 5)==0) {
        //Get Analog value of channel A3 (old port name X3) = port used for potentiometer on DB66DEV1
        uitoa((WORD) adcReadChan(ADC_CH_A3), dest);
        
        return ref + strlen((const char*)dest);  //Return ref + (lengh of written string)
    }

    return ref;
}
