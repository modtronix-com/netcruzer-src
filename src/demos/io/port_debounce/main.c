/**
 * @example port_debounce/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use the nz_debounce.h and nz_debounce.c files for debouncing
 * noisy digital inputs. For this demo, port 32 and 33 (old port names Y2 and Y3) are used. These
 * <a href="http://netcruzer.com/nzdoc-sbc66-ports">SBC66 Ports</a> are
 * connected to buttons on the <a href="http://netcruzer.com/db66dev1.html">DB66DEV1</a>
 * development daughter board.
 * These 2 buttons will rotate LEDs 1 to 4 left or right. LEDs 1 to 4 are located on
 * IO Ports (ports of SBC) 6, 7, 8 and 9 (old port names X6, X7, X8 and X9) of the DB66DEV1.
 * The following lines in the projdefs.h file specifies what ports are debounced:
 * @code
 * #define DEBOUNCE_GET_PORT1()    (!PIN_32)
 * #define DEBOUNCE_GET_PORT2()    (!PIN_33)
 * @endcode
 *
 * Additionally the System LED blinks every second.
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>,
 * <b>AND</b> a <a href="http://netcruzer.com/db66dev1.html">DB66DEV1</a> development board.
 * The DB66DEV1 is plugged into the daughter board connector of the SBC66 board.
 * The DB66DEV1 module is however not required, and discrete buttons and LEDs can be connected to the
 * SBC port pins via 1k resistors to 0V.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/io/port_debounce" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
 * Download</a>. To compile for Netcruzer Board, open this project in MPLAB X, and select the "Project Configuration" for desired board. For
 * example "SBC66ECL_R2" for the <a href="http://netcruzer.com/sbc66ecl.html">SBC66ECL</a> Revision 2 board.
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
 * 2013-05-03, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

////////// Includes /////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects
#include "nz_db66dev1.h"
#include "nz_debounce.h"

/**
 * Main program entry point.
 */
int main(void)
{
    BYTE ledRotate = 0x01;
    BYTE ledRotateOld = 0;
    WORD tmrFlashLed = 0;   //Timer for flashing system LED

    nzSysInitDefault();         //Default initialization. All ports inputs. All analog features disabled. Tick 1ms

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs

    //Initialize DB66DEV1, all LEDs are output and turned off, Buttons inputs
    db66dev1_Init();

while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //Get debounced port 1, this is defined as PIN_32 in the projdefs.h file.
        if (debounceGetLatchedPort(1)) {
            //Rotate LEDs 5 to 8 right
            ledRotate = ledRotate >> 1;
            if (ledRotate==0x0)
                ledRotate = 0x08;
        }
        //Get debounced port 2, this is defined as PIN_33 in the projdefs.h file.
        if (debounceGetLatchedPort(2)) {
            //Rotate LEDs 5 to 8 left
            ledRotate = ledRotate << 1;
            if (ledRotate==0x10)
                ledRotate = 0x01;
        }

        //Output LEDs
        if (ledRotate != ledRotateOld) {
            ledRotateOld = ledRotate;
            if (ledRotate&0x01) LAT_LED5 = 1; else LAT_LED5 = 0;
            if (ledRotate&0x02) LAT_LED6 = 1; else LAT_LED6 = 0;
            if (ledRotate&0x04) LAT_LED7 = 1; else LAT_LED7 = 0;
            if (ledRotate&0x08) LAT_LED8 = 1; else LAT_LED8 = 0;
        }

        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
        }

    }//end while

    return 0;
}//end main
