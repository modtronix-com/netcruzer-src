/**
 * @example i2c_debug_demo/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to communicate with the I2C port. It can be used with any
 * <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with an USB Port (not USB Host). It uses USB Commands and Debug Messages sent
 * and received via the <a href="http://www.netcruzer.com/usbterminal/">Netcruzer USB Terminal</a>
 * app to communicate with board.
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
 * with an USB Port (not USB Host), like the
 * <a href="http://netcruzer.com/sbc66zl/">SBC66ZL</a> for example.
 * Additionally some I2C device should be attached to the board. We recommend using one of our
 * <a href="http://netcruzer.com/lcd2s/">LCD2S</a> serial LCD displays, like the
 * <a href="http://netcruzer.com/lcd2s-162yhy.html/">LCD2S-162YHY</a> for example. This
 * can be attached to the red Micro Match connector on the SBC board, which is connected
 * to I2C Bus 1.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/i2c/i2c_debug_demo" folder of the
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
#include "nz_serI2C.h"

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
    //Declare all timers, and initiaze with current timer value (tick16Get...() functions)
    WORD tmrFlashLed = tick16Get();   //Timer for flashing system LED

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();
    //serI2C1Init();              //Initialize I2C 1 - default CALLED BY SYSTEM! (nz_netcruzer.c)

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nThis is a test debug message from i2c_debug_demo");

    //Set System LED port as outputs. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output
    DIR_SYSLED = OUTPUT_PIN;

    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.
        //serI2C1Task();        //I2C1 Task - default CALLED BY SYSTEM! (nz_netcruzer.c)

         //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED; //Toggle System LED
        }
    }//end while
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
            debugPutString("\nHello");
        }
        //'lcd1' - Send a test message to a LCD2S serial display
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "lcd1") == 0) {
            //The following code sends a message to a LCD2S serial LCD display connected to the
            //SBC board. It can be connected via the red Micro Match connector.
            //- The LCD2S default address is 0x50.
            //- First byte of data is 0x80, which is LCD2S Write Parsed String Command
            //- Second byte of data is 0x0C, which is LCD2S Clear display and go to
            //  beginning of first line escape character
            //- Next 5 bytes is the message to be displayed on the LCD = "Hello"
            i2c1BeginTransmission(0x50);
            i2c1Write(0x80);
            i2c1Write(0x0C);
            i2c1WriteString("Hello");
            i2c1EndTransmission();
            if (i2c1GetStatusWait(0) != 0) {
                debugPutString("\nI2C Transmission error!");
            }
        }
        //'lcd2' - Send a test message to a LCD2S serial display
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "lcd2") == 0) {
            //The following code sends a message to a LCD2S serial LCD display connected to the
            //SBC board. It can be connected via the red Micro Match connector.
            //- The LCD2S default address is 0x50.
            //- First byte of data is 0x80, which is LCD2S Write Parsed String Command
            //- Second byte of data is 0x0C, which is LCD2S Clear display and go to
            //  beginning of first line escape character
            //- Following string is displayed on first line of LCD Display = "Hello"
            //- Following 0x0A is the LCD2S "Go to next line" Command
            //- Following string is displayed on second line of LCD Display = "World"
            i2c1WriteAsciiEscString("50800C'Hello'0A'World'");
            //Port independent version
            //i2cWriteAsciiEscString(I2C1_INFO, "50800C'Hello'0A'World'");
            if (i2c1GetStatusWait(0) != 0) {
                debugPutString("\nI2C Transmission error!");
            }
        }
        //'msg1' - Send a test message to a I2C device with address 0x60
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "msg1") == 0) {
            //Write message to slave at address 0x60 on I2C 1 bus, using i2c1WriteMessage()
            //- First byte of data is 0x80 ("Write All Relays" command for DINS-8R board)
            //- Second byte of data is 0x0f (value of relays for DINS-8R board)
            BYTE msg[] = {0x80, 0x0f};
            i2c1WriteMessage(0x60, msg, sizeof(msg));
            if (i2c1GetStatusWait(0) != 0) {
                debugPutString("\nI2C Transmission error!");
            }
        }
        //'msg2' - Same as 'msg1' - Alternative method
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "msg2") == 0) {
            i2c1WriteMessage(0x60, (BYTE*)"\x80\0x0f", 2);  //Send Byte1=0x80, Byte2=0x0f to slave at address 0x60
            if (i2c1GetStatusWait(0) != 0) {
                debugPutString("\nI2C Transmission error!");
            }
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