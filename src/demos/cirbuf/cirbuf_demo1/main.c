/**
 * @example cirbuf_demo1/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo show how to use "Circular Buffers" as specified in the "nz_circularBuffer.h" interface.
 *
 * USB debugging is enabled (see "Debug Configuration" in projdefs.h), meaning all debug and system
 * output is directed via USB to the "Netcruzer USB Terminal" app. It will be
 * displayed in the "Debug" tab of this app, which can be downloaded here:<br>
 * http://www.netcruzer.com/usbterminal/
 *
 * Additionally debug message can also be processed in the debugService() function. This example simply checks
 * if "hi" is received, and if so, replies with "Hello". Sending and receiving debug message is
 * done in the "Debug" tab of the "Netcruzer USB Terminal" app.
 *
 * <b>Details:</b><br>
 * The "Netcruzer Library" contains two circular buffer implementations. Any one can be used for this project.
 * The standard one (nz_circularBufferStd.c) is used for this demo, requiring the following to be done(is already done!):
 * - Copy @ref nz_circularBuffer_conf "Circular Buffer Configuration" section from nz_circularBuffer.h to projdefs.h
 * - In this copied section, ensure "#define    CIRBUF_USE_CIRCULAR_BUFFER_STD" is uncommented. This
 *   configures what "Circular Buffer" implementation is used.
 * - Include "nz_circularBufferStd.c" in project
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a> with an USB Port (not USB Host).
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/usb/cirbuf_demo1" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
 * Download</a>. To compile for Netcruzer Board, open this project in MPLAB X, and select the "Project Configuration" for desired board. For
 * example "SBC66ZL_R1" for the <a href="http://netcruzer.com/sbc66zl.html">SBC66ZL</a> Revision 1 board.
 * For details <a target="_blank" href="http://netcruzer.com/nz/doc/building_projects/">click here</a>
 * A common error is "The system cannot find the path specified". This generally means you don't have the required XC16 compiler version installed.
 * Go to "Project Properties", and select your installed XC16 compiler in the "Project Configuration" section.
 *
 * <h2>===== Programming Board =====</h2>
 * After compiling (build), the board can be programmed via the <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb">
 * USB Bootloader</a> or a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_prog">PIC Programmer</a>. USB Programming is
 * simplified when using the SBC board together with a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb_pt66">Prototype Board</a>.
 *
 * <h2>===== File History =====</h2>
 * 2013-07-22, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

////////// Includes /////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects
#include "nz_helpers.h"

//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN     DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"

/** Main application entry point. */
int main(void) {
    //Create and initialize a streaming type "Circular Buffer" with no formatting
    CIRBUF cbufTest;
    BYTE testBuf[128];
    cbufInit(&cbufTest, testBuf, sizeof(testBuf), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    delay_ms(1000);     //Delay 1 second at startup

    printf("\nThis will be displayed in the debug tab of the 'Netcruzer USB Terminal' app!");

    //Do some checks on buffer
    //<b>Output:</b> "Buffer is empty"
    if (cbufIsEmpty(&cbufTest)) {
        printf("\n\nBuffer is empty");
    }

    //Add/remove bytes to/from buffer
    //<b>Output:</b> "After adding A and B, buffer size is 2"
    cbufPutByte(&cbufTest, 'A');
    cbufPutByte(&cbufTest, 'B');
    printf("\n\nAfter adding A and B, buffer size is %d", cbufGetCount(&cbufTest));
    cbufEmpty(&cbufTest);   //Empty buffer

    //Add string to buffer using cbufPutString(), and remove all byte using cbufGetByte().
    //Note that string terminating NULL character is NOT added to buffer.
    //<b>Output:</b><br>
    //  "After adding 'Test String', buffer size is 11"
    //  "Test string is: Test String"
    //  "Size is now 0"
    cbufPutString(&cbufTest, "Test String");  //Add this to "Circular Buffer"
    printf("\n\nAfter adding 'Test String', buffer size is %d", cbufGetCount(&cbufTest));
    printf("\nTest string is: ");               //Below we will remove and print string
    while (cbufHasData(&cbufTest)) {
        putchar(cbufGetByte(&cbufTest));        //Print every character of the string
    }
    printf("\nSize is now %d", cbufGetCount(&cbufTest));

    //Main loop
    while (1) {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.
    }
}

#if defined(HAS_NZ_DEBUGGING)
/**
 * Debug service routine. This function is required when debugging is enabled. It is
 * called from the system! We have to process any received debug message.
 */
void debugService(void) {
    //Check if a debug message was received
    if (cbufHasWholePacket(CIRBUF_RX_DEBUG)) {
        //Check if we received "hi". If so, reply with "Hello" on a new line
        if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "hi") == 0) {
            debugPutString("\nHello");
        }
        //Check if we received "hello". If so, reply with "G'Day Mate!" on a new line
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "myMessage...") == 0) {
            //Do something.....
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif
