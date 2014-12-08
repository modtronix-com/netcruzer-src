/**
 * @example device_HID_debug_demo/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo monitors the USB port for any Debug messages.
 * <br>- If "hi" is received, it replies with "Hello"
 * <br>- If "hello" is received, it replies with "G'Day Mate!"
 * <br>Use the "Netcruzer USB Terminal" app to send and receive USB Debug messages. It can be downloaded here:
 * http://www.netcruzer.com/usbterminal/
 *
 * This demo does not implement any USB code in the main.c file, and instructs the system to do it all. This is
 * done by including the "HAS_SERPORT_USB_HID" define in the projdefs.h file.
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
 * This project is located in the "src/demos/usb/device_HID_debug_demo" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nThis is a test debug message");

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
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "hello") == 0) {
            debugPutString("\nG'Day Mate!");
        }
        //Add your custom debug command here....................
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "myString") == 0) {
            //Do something.........
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif