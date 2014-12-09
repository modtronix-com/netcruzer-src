/**
 * @example uart_debug_demo1/main.c
 *
 * <h2>===== Description =====</h2>
 * Project showing how to use the UARTs of a SBC66 board. This project uses the
 * netcruzer nz_serUart.h and nz_serUart.c UART drivers.
 *
 * Every 500ms the System LED is blinked, and a message sent on UART1.
 *
 * Additionally this demo also implements debugging via the USB port and the
 * <a href="http://www.netcruzer.com/usbterminal/">Netcruzer USB Terminal</a> App.
 * During the program various debug messages are written, which will be displayed
 * on the "Netcruzer USB Terminal" (running on a PC connected to the target
 * board's USB port).
 * This demo also monitors the USB port for Debug messages send to us.
 * <br>If "hi" is received, it replies with "Hello"
 * <br>Use the "Netcruzer USB Terminal" App to send and receive USB Debug messages.
 * It can be downloaded here: http://www.netcruzer.com/usbterminal/
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a SBC66 Netcruzer board (http://netcruzer.com/products-sbc66-main) with
 * an USB Port (not USB Host). To access the SBC66 board's port pins, a prototype board like the
 * PT02TC, PT66EI or PT66ECI can be used. For available boards, see http://netcruzer.com/products-sbc66-prototype.
 * Or a development board, like the DB66DEV1 (http://netcruzer.com/db66dev1.html) can be used.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/template/uart_debug_demo1" folder of the
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
 * 2014-05-03, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

// Includes /////////////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects
#include "nz_circularBuffer.h"
#include "nz_serUart.h"
#define USE_AND_OR              //Required by XC16 to enable AND_OR mask setting
#include "uart.h"


//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN     DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"

// Variables ////////////////////////////////////
WORD tmrFlashLed = 0;        //Timer for flashing system LED

/**
* Main application entry point.
*/
int main(void) {

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nThis is a test debug message from uart_debug_demo1 demo");

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs

    nzDelayMs(300);

    //Assign pins use for iMod 1 TX, RX, RTS and CTS on PT66EI and PT66ECI boards
	//PPSUnLock;  // Unlock PPS, called in nzSysInitDefault()!
    iPPSOutput(PPS_OUT_06, OUT_FN_PPS_U1TX);        //Port 6 (old port name X6) is TX
    iPPSOutput(PPS_OUT_08, OUT_FN_PPS_U1RTS);       //Port 8 (old port name X8) is RTS
    iPPSInput(IN_FN_PPS_U1RX, PPS_IN_07);           //Port 7 (old port name X7) is RX
    iPPSInput(IN_FN_PPS_U1CTS, PPS_IN_01);          //Port 1 (old port name X1) is CTS
    CloseUART1();

    serUart1Init();

    //UART initialized, 9600 Buad, 8bit, no parity, 1 stop bit, High BAUD rate
    //Ensure USER_CONFIGURES_UART1 define is set to 1! Else, system will configure UART1 with default values!
    OpenUART1(UART_EN | UART_NO_PAR_8BIT | UART_1STOPBIT | UART_UEN_00 | UART_BRGH_FOUR , UART_TX_ENABLE, UART_BAUD_9K6);

	PPSLock;    // Lock PPS once all initialization is done
	
    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //Print any data received via UART1
        while (cbufHasData(CIRBUF_RX_UART1)) {
            debugPutString("\nReceived 0x");
            debugPutHexByte(cbufGetByte(CIRBUF_RX_UART1));
        }

        //Enter every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 10ms again

            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED

            //Send test message each time LED turns on
            if (LAT_SYSLED) {
                cbufPutByte(CIRBUF_TX_UART1, 'M');
            }
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
        //Add custom debug message processing
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "...") == 0) {
            //..... Do something
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif