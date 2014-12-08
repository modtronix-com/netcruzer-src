/**
 * @example cirbuf_packet_demo/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo demonstrates how to use "Circular Buffers" as specified in the "nz_circularBuffer.h" interface.
 *
 * USB debugging is enabled (see "Debug Configuration" in projdefs.h), meaning all
 * debug and system output is directed via USB to the "Netcruzer USB Terminal" app. It will be
 * displayed in the "Debug" tab of this app, which can be downloaded here:<br>
 * http://www.netcruzer.com/usbterminal/
 *
 * Additionally debug message can also be processed in the debugService() function. This example simply checks
 * is "hi" is received, and if found, replies with "Hello". Sending and receiving debug message is
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
 * This project is located in the "src/demos/usb/cirbuf_packet_demo" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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


/**
* Main application entry point.
*/
int main(void) {
    //Create and initialize a "Circular Buffer". It is initialized as a "Packet Buffer". This
    //means we can write/read packets to/from the "Circular Buffer". It is VERY IMPORTANT not
    //to use normal put and get functions for adding/removing data to a packet buffer! It will
    //corrupt the buffer!
    CIRBUF cbufTest;
    BYTE testBuf[128];
    cbufInit(&cbufTest, testBuf, sizeof(testBuf), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);

    //Two BYTE arrays used for testing
    BYTE arr1[32], arr2[32];
    BYTE* pByte;
    WORD size;

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    delay_ms(1000);     //Delay 1 second at startup

    printf("\nThis will be displayed in the debug tab of the 'Netcruzer USB Terminal' app!");

    //Do some checks on buffer
    //<b>Output:</b> "Buffer does not have any packets"
    if (cbufHasWholePacket(&cbufTest) == FALSE)
        printf("\n\nBuffer does not have any packets");

    //Add string to "arr1" buffer, than add it to "Circular Buffer" as a packet.
    //Note that string terminating NULL character is also added to buffer.
    //<b>Output:</b><br>
    //  "After adding packet, cbufHasWholePacket() returns true"
    //  "Test string (size=12) is: Test String"
    //  "Size is now 0"
    strcpy((char*)arr1, "Test String");     //Copy string to our BYTE array
    cbufPutPacket(&cbufTest, arr1, 12);     //Add this BYTE array to our "Circular Buffer" as a packet. Include string NULL (12 not 11)
    if (cbufHasWholePacket(&cbufTest))
        printf("\n\nAfter adding packet, cbufHasWholePacket() returns true");
    size = cbufGetContiguousPacket(&cbufTest, &pByte);      //Get pointer to packet data. Nothing is removed!
    printf("\nTest string (size=%d) is: %s", size, pByte);  //Below we will remove and print string
    cbufRemovePacket(&cbufTest);            //Remove packet
    printf("\nSize is now %d", cbufGetCount(&cbufTest));

    //Main loop
    while (1) {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.
    }
}

#if defined(DEBUGGING_ENABLED)
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
