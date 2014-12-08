/**
 * @example template_project/main.c
 *
 * <h2>===== Description =====</h2>
 * This is a blank project that can be used as a template for starting new projects.
 * All it does is blink the System LED every second.
 *
 * <h2>===== Required Hardware =====</h2>
 * The project can run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/template/template_project" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
 * 2014-01-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

////////// Includes /////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects

/**
 * Main program entry point.
 */
int main(void)
{
    WORD tmrFlashLed = 0;       //Timer for flashing system LED

    nzSysInitDefault();         //Default initialization. All ports inputs. All analog features disabled. Tick 1ms

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs

    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
        }
        
        //Add custom code here
        //.......................................

    }//end while

    return 0;
}//end main