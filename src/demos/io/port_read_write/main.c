/**
 * @example port_read_write/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to read and write port pins of a Netcruzer Single Board Computer. All ports on Netcruzer boards are named
 * 0 - 41 (old names used to be T0 to T7, Y0 to Y11 and X0 to X11). For details how they map to the Microcontroller port pins,
 * see http://netcruzer.com/nzdoc-sbc66-ports/
 * For this demo, connect a LED to port 38 (old port name Y8) (anode to port 38, cathode to 0V via a 1k resistor), and a button to
 * port 34 (old port name Y4) (other side of button to 0V). This example will continuously check the state of the button input. When
 * the button is pressed, it will turn on the LED on port 38 (old port name Y8). When not pressed, it will turn off the LED.
 *
 * <h2>===== Required Hardware =====</h2>
 * This project can be run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>. For prototyping, we recommend
 * combining this board with a <a href="http://netcruzer.com/products-sbc66-prototype/">Prototyping Board</a>, like the PT66ECI for example. This
 * low cost prototyping board makes all the I/O ports of the SBC66 board available via marked labels on the PCB. It also provides a reset and
 * firmware button that simplifies prototyping.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/io/port_read_write" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
 * Download</a>. To compile for Netcruzer Board, open this project in MPLAB X, and select the "Project Configuration" for desired board. For
 * example "SBC66ECL_R2" for the <a href="http://netcruzer.com/sbc66ecl.html">SBC66ECL</a> Revision 2 board.
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

////////// Includes /////////////////////////////
#include "HardwareProfile.h"            //Required for all Netcruzer projects


/**
 * Main program entry point.
 */
int main(void)
{
    //Declare all timers, and initiaze with current timer value (tick16Get...() functions)
    WORD tmrFlashLed = tick16Get();     //Timer for flashing system LED

    nzSysInitDefault();         //Default initialization. All ports inputs. All analog features disabled. Tick 1ms

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port=outputs. Setting DIR_xxx to 1 set's port to input, and 0 as output

    //We want to connect a button to port 34 (old port name Y4). When pressed, it will pull port 34 to 0 (connect to 0V). Enable pull-up
    //resistor so when button not pressed, port is 1
    DIR_34 = INPUT_PIN;         //Configure Port 34 (old port name Y4) to be an input
    PULLUP_34 = 1;              //Enable pull-up resistors on port 34 (old port name Y4)

    //Connect a LED to port 38 (old port name Y8), and configure as output pin. Connect anode (+) to port 38, and cathode to 0V via
    //a 1k resistor.
    DIR_38 = OUTPUT_PIN;

    while(1)
    {
        //If button on port 34 (old port name Y4) is pressed, turn LED on port 38 (old port name Y8) on. Else, LED is off
        if (PIN_34 == 0) {
            LAT_38 = 1; //If button on port 34 (old port name Y4) is pressed, turn LED on port 38 (Y8) on. Set port 38 to 1 (3.3V)
        }
        else {
            LAT_38 = 0; //If button on port 34 (old port name Y4) NOT pressed, turn LED on port 38 (Y8) off. Set Y8 to 0 (0V)
        }

        /////////////////////////////////////////////////
        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
        }
    }//end while

    return 0;
}//end main
