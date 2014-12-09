 /**
 * @example demos/Modbus_Demo1/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to implement ModbusTCP using an open source Modbus library (modbus.c and modbus.h). It is
 * in Beta stage! For future releases of this stack, ModbusTCP support will be added to the Netcruzer library.
 * The <a href="http://netcruzer.com/sbc66ec.html/">SBC66EC</a> (SBC66ECL) board is used in combination with the
 * <a href="http://netcruzer.com/pt66din6/">PT66DIN6</a> DIN Rail Main board, and the
 * <a href="http://netcruzer.com/dins8r.html/">DINS-8R</a> 8 Relay DIN Rail Module.
 * The PT66DIN6 is a DIN Rail mountable board:
 *  - It takes a SBC66 board as a daughter board
 *  - It has 6 iMod ports for adding any of our available <a href="http://netcruzer.com/products-imod/">iMod Modules</a>
 *  - It has a Sub-D 9 Pin connector for adding additional serial DINS I/O module, like the DINS-8R 8-Relay
 *    module used in this demo.
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
 * This demo is part of the main Webserver project, and is located in the "src/projects/webserver/demos/ModbusDemo1" folder of the
 * <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer Download</a>. To compile, open the Webserver project
 * in MPLAB X, and select the "Project Configuration" that includes "_MODBUS_DEMO1" in it's name. For example, to build firmware
 * for the <a href="http://netcruzer.com/sbc66ec.html">SBC66EC</a> board, select "SBC66EC_MODBUS_DEMO1".
 * For details <a target="_blank" href="http://netcruzer.com/nz/doc/building_projects/">click here</a>
 *
 * <h2>===== Programming Board =====</h2>
 * After compiling (build), the board can be programmed via the <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb">
 * USB Bootloader</a> or a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_prog">PIC Programmer</a>. USB Programming is
 * simplified when using the SBC board together with a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb_pt66">Prototype Board</a>.
 *
 * <h2>===== File History =====</h2>
 * 2012-08-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

#if defined(MODBUS_DEMO1)

//Netcruzer include files
#include "HardwareProfile.h"

#include "main.h"
#include "mainApp.h"
#include "nz_serDataPorts.h"
#include "nz_debounce.h"
#include "nz_helpers.h"
#include "cmd.h"
#include "demos/Modbus.h"       //Include file for modbus demo library

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


// Variables ////////////////////////////////////
static WORD tmrProcessIO = 0;   //Timer definining LED flash rate (16-bit, 1ms Timer)
static BYTE relays = 0x01;


/**
* Main application entry point.
*/
int main(void) {
    initMainApp();        //Initialize the main Webserver application

    tmrProcessIO = tick16Get() + tick16ConvertFromMS(600); //Startup delay of 600mS

    im2bl_Init(1);  //Initialize im2BL iMod module to be located on iMod port 1

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
        MBRun();
        processIO();
    }
}


/**
 * I/O functions
 */
void processIO(void) {

    im2bl_WriteLed1Imod1(im2bl_ReadButton1Imod1());     //im2BL LED1 on if Button 1 pressed
    im2bl_WriteLed2Imod1(im2bl_ReadButton2Imod1());     //im2BL LED2 on if Button 2 pressed

//    //If button1 on in2BL iMod module (imod port 1) is pressed, run relays one direction
//    if(debounceGetLatchedPort(DEBOUNCE_PORT_IMOD1_BUTTON1)) {
//        relays = relays << 1;
//        if (relays == 0)
//            relays = 0x01;
//        i2c1BeginTransmission(0x60);  //Default DINS-8R address is 0x60
//        i2c1Write(0x80);
//        i2c1Write(relays);
//        i2c1EndTransmission();
//    }
//
//    //If button2 on in2BL iMod module (imod port 1) is pressed, run relays other direction
//    if(debounceGetLatchedPort(DEBOUNCE_PORT_IMOD1_BUTTON2)) {
//        relays = relays >> 1;
//        if (relays == 0)
//            relays = 0x80;
//        i2c1BeginTransmission(0x60);  //Default DINS-8R address is 0x60
//        i2c1Write(0x80);
//        i2c1Write(relays);
//        i2c1EndTransmission();
//    }

    //Enter every 500ms
    if (tick16TestTmr(tmrProcessIO)) {
        tick16UpdateTmrMS(tmrProcessIO, 500); //Update timer to expire in 500ms again

        if (!imod1_ReadButton1()) {
            relays = relays << 1;
            if (relays == 0)
                relays = 0x01;

            //Write message to I2C 1 port
            //- First byte is address (0x60), follow by two data bytes.
            //- First byte of data is 0x80 ("Write All Relays" command for DINS-8R board)
            //- Second byte of data is 0x0f (value of relays for DINS-8R board)
            i2c1BeginTransmission(0x60);
            i2c1Write(0x80);
            i2c1Write(relays);
            i2c1EndTransmission();

            //Write message to I2C 1 port, using an alternative method using i2c1WriteMessage()
            //command and message in array.
            //BYTE bufMsg[] = {0x80, 0};
            //bufMsg[2] = relays++;
            //cbufPutPacket(CIRBUF_TX_I2C1, bufMsg, strlen((const char*)bufMsg));
            //i2c1WriteMessage(0x60, bufMsg, strlen((const char*)bufMsg));
        }
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


#if defined(HAS_NZ_DEBUGGING)
/**
 * Debug service routine. This function is required when debugging is enabled. It is
 * called from the system! We have to process any received debug message.
 */
void debugService(void) {
    //Check if a debug message was received
    if (cbufHasWholePacket(CIRBUF_RX_DEBUG)) {
        //'hi' - Reply with "Hello" on a new line
        if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "hi") == 0) {
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHello");
        }
        //Add custom debug message processing
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "...") == 0) {
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nReceived unknown debug message");
            //..... Do something
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif

#endif  //#if defined(MODBUS_DEMO1)