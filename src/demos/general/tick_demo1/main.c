/**
 * @example tick_demo1/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use the system tick. The system tick uses Timer 1, and has a period of 1ms. Each 1ms an interrupt is triggered,
 * and the 32-bit system tick is incremented. To make code smaller and more efficient, default tick functions use the lower 16 bits of the
 * 32-bit system tick. This limits it to a maximum time of 32,768ms = 32.7 seconds.
 * If this is not sufficient, the 32-bit tick functions can be used, which have a maximum time of 4,294,967,296 ms = 1,193 hours = 49.7 Days.
 * The 16-bit variants are the most efficient, and create the smallest code, seeing that the native data size for this CPU is 16-bits.
 *
 * <h2>===== Required Hardware =====</h2>
 * This project can be run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>. For prototyping, we recommend
 * combining this board with a <a href="http://netcruzer.com/products-sbc66-prototype/">Prototyping Board</a>, like the PT66ECI for example. This
 * low cost prototyping board makes all the I/O ports of the SBC66 board available via marked labels on the PCB. It also provides a reset and
 * firmware button that simplifies prototyping.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/general/tick_demo1" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
 * Download</a>. To compile for Netcruzer Board, open this project in MPLAB X, and select the "Project Configuration" for desired board. For
 * example "SBC66ECL_R2" for the <a href="http://netcruzer.com/sbc66ecl.html">SBC66ECL</a> Revision 2 board.
 * For details <a target="_blank" href="http://netcruzer.com/nz/doc/building_projects/">click here</a>.<br>
 * A common error is "The system cannot find the path specified". This generally means you don't have the required XC16 compiler version installed.
 * Go to "Project Properties", and select your installed XC16 compiler in the "Project Configuration" section.
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

/////////////////////////////////////////////////
// Includes
#include "HardwareProfile.h"            //Required for all Netcruzer projects

/**
 * Main program entry point.
 */
int main(void)
{
    DWORD tmr32Task1Hour;           //Do something every hour (32-bit, 1ms Timer)
    WORD tmrFlashLed;               //Timer definining LED flash rate (16-bit, 1ms Timer)

    nzSysInitDefault();             //Default initialization. All ports inputs. All analog features disabled. Tick 1ms.

    tmr32Task1Hour = tick32Get();   //Set with current 32-bit tick.
    tmrFlashLed = tick16Get();        //Set with current 16-bit tick.

    //Set System LED port, and port 5 (old port name X5) as outputs. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output
    DIR_SYSLED = 0;
    DIR_05 = 0;

    while(1)
    {
        /////////////////////////////////////////////////
        //Flash System LED every 200ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 200);     //Update timer to expire in 200ms again

            LAT_SYSLED = !LAT_SYSLED;
        }

        /////////////////////////////////////////////////
        //Do something each hour = 3,600 seconds = 3,600,000 ms
        if (tick32TestTmr(tmr32Task1Hour)) {
            tick32UpdateTmrMin(tmr32Task1Hour, 60);
            //tmr32Task1Hour += 3600000;   //Set to trigger again in 1 hour (3,600,000 ms)

            //..... Add code here to be executed every hour
        }

    }//end while

    return 0;
}//end main
