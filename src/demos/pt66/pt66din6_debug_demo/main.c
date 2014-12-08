/**
 * @example pt66din6_debug_demo/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use the <a href="http://netcruzer.com/pt66din6.html">PT66DIN6</a>
 * DIN Rail board with iMod ports. This demo can be used with any
 * <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with an USB Port (not USB Host). To enable PT66DIN6 specific function, the following
 * line must be added to this project's "HardwareProfile.h" file:<br>
 * @code
 * #define XBRD_PT66DIN6_R1
 * @endcode
 * This will make iMod specific functions available in the IDE. These functions will
 * appear in MPLAB X as code completion suggestions if the module name is types.
 * For details, see @ref nz_pt66din6_desc "PT66DIN6 Documentation".
 *
 * Additionally this demo also implements debugging via the USB port and the
 * <a href="http://www.netcruzer.com/usbterminal/">Netcruzer USB Terminal</a> App.
 * During the program various debug messages are written, which will be displayed
 * on the "Netcruzer USB Terminal" (running on a PC connected to the target
 * board's USB port).
 * This demo also monitors the USB port for Debug messages send to us.
 * <br>- If "hi" is received, it replies with "Hello"
 * <br>Use the "Netcruzer USB Terminal" App to send and receive USB Debug messages.
 * It can be downloaded here: http://www.netcruzer.com/usbterminal/
 *
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with an USB Port (not USB Host), <b>AND</b> a <a href="http://netcruzer.com/pt66din6.html">PT66DIN6</a>
 * development board. The SBC board is plugged onto the PT66DIN6 board (as a daughter board).
 * It requires a button and LED connected to iMod port 5 and 2. This can be provided by the
 * <a href="http://netcruzer.com/im2bl.html">im2BL</a> iMod module with 2 LEDs and buttons.
 * This module is however not required, and a button and LED can be connected to these pins of the
 * iMod port via a 1k resistor to 0V.
 *
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/pt66/pt66din6_debug_demo" folder of the
 * <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer Download</a>. To compile for
 * Netcruzer Board, open this project in MPLAB X, and select the "Project Configuration" for desired board. For
 * example "SBC66ZL_R1" for the <a href="http://netcruzer.com/sbc66zl.html">SBC66ZL</a> Revision 1 board.
 * For details <a target="_blank" href="http://netcruzer.com/nz/doc/building_projects/">click here</a>
 *
 * <h2>===== Programming Board =====</h2>
 * After compiling (build), the board can be programmed via the <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb">
 * USB Bootloader</a> or a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_prog">PIC Programmer</a>.
 * USB Programming is simplified when using the SBC board together with a
 * <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb_pt66">Prototype Board</a>.
 *
 * <h2>===== File History =====</h2>
 * 2013-10-31, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

// Includes /////////////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects

//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN     DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"

// Variables ////////////////////////////////////

/**
* Main application entry point.
*/
int main(void) {
    //Declare all timers, and initiaze with current timer value (tick16Get...() functions)
    WORD tmrFlashLed = tick16Get();   //Timer for flashing system LED

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    //Set System LED port as outputs. Setting DIR_xxx to 1 configures port as input, 0 as output.
    DIR_SYSLED = OUTPUT_PIN;

    pt66din6_Init();
    //im2bl_Init(1);    //Code below can be removed if uncommenting this line.

    //A button is connected to iMod 1 pin 5, via a 1k resistor to 0V. Configure
    //this pin as inputs, and enable internal pull-up resistor on IO Port pins (ports of SBC).
    imod1_ConfDirPin5(INPUT_PIN);   //Button 1 for im2BL
    imod1_ConfPullupPin5(1);

    //A LED is connected to iMod 1 pin 2, configure as outputs.
    imod1_ConfDirPin2(OUTPUT_PIN);  //LED 1 for im2BL

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nThis is a test debug message from pt66din6_debug_demo");

    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //If button on iMod 1 Pin 5 is pressed, turn on LED on iMod 1 Pin 2
        if (imod1_ReadPin5()==0)
            imod1_WritePin2(1); //Turn on LED on iMod 1 Pin 2
        else
            imod1_WritePin2(0); //Turn off LED on iMod 1 Pin 2
            
         //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED; //Toggle System LED
        }
    }//end while
}

#if defined(DEBUGGING_ENABLED)
/**
 * Debug service routine. This function is required when debugging is enabled. It is
 * called from the system! We have to process any received debug message.
 */
void debugService(void) {
    //Check if a debug message was received
    if (cbufHasWholePacket(CIRBUF_RX_DEBUG)) {
        //'hi' - Reply with "Hello" on a new line
        if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "hi") == 0) {
            debugPutString("\nHello");
        }
        //Add custom debug message processing
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "...") == 0) {
            //..... Do something
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif