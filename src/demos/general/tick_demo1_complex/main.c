/**
 * @example tick_demo1_complex/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use the complex version of the system tick. It can also be used an example of how to overwrite
 * the default netcruzer tick with a custom implementation. It is enabled by adding the following line to the projdefs.h file:<br>
 * @code
 * #define NZSYS_ENABLE_COMPLEX_TICK  //Indicates that the complex tick file nz_tickCx must be used, and not nz_tick<br>
 * @endcode
 * The system tick uses Timer 1, and has a period of 1ms. Each 1ms an interrupt is triggered,
 * and the 32-bit system tick is incremented. To make code smaller and more efficient, various functions are available for using 8, 16 and
 * 32-bit variables for implementing timers and delays. The 16-bit variants are the most efficient, and create the smallest code, seeing that
 * the native data size for this CPU is 16-bits. The 8-bit variants use the least RAM. The 32-bit variables can create the longest delays.
 *
 * <h2>===== Required Hardware =====</h2>
 * This project can be run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>. For prototyping, we recommend
 * combining this board with a <a href="http://netcruzer.com/products-sbc66-prototype/">Prototyping Board</a>, like the PT66ECI for example. This
 * low cost prototyping board makes all the I/O ports of the SBC66 board available via marked labels on the PCB. It also provides a reset and
 * firmware button that simplifies prototyping.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/general/tick_demo1_complex" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
    WORD tmr16bit256ms_Task1Hour;   //Do something (approximately) every hour (16-bit, 256ms Timer)
    BYTE tmr8bit16ms_ToggleX5;      //Toggle port 5 (old port name X5) (8-bit, 16ms Timer)

    nzSysInitDefault();             //Default initialization. All ports inputs. All analog features disabled. Tick 1ms.

    tmr32Task1Hour = tick32Get();   //Set with current 32-bit tick.
    tmrFlashLed = tick16Get();        //Set with current 16-bit tick.
    tmr16bit256ms_Task1Hour = tick16Get_256ms();   //Set with current 16-bit tick, 256 ms tick
    tmr8bit16ms_ToggleX5 = tick8Get_16ms();        //Set with current 8-bit, 16 ms tick

    //Set System LED port, and port 5 (old port name X5) as outputs. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output
    DIR_SYSLED = 0;
    DIR_05 = 0;

    while(1)
    {
        /////////////////////////////////////////////////
        //Flash System LED every 200ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 200);             //Update timer to expire in 200ms again

            LAT_SYSLED = !LAT_SYSLED;
        }

        /////////////////////////////////////////////////
        //Do something each hour = 3,600 seconds = 3,600,000 ms
        if (tick32TestTmr(tmr32Task1Hour)) {
            tick32UpdateTmrMin(tmr32Task1Hour, 60);
            //tmr32Task1Hour += 3600000;   //Set to trigger again in 1 hour (3,600,000 ms)

            //..... Add code here to be executed every hour
        }

        /////////////////////////////////////////////////
        //Do something (approximately) each hour = 3,600 seconds = 3,600,000 ms
        if (tick16HasExpired_256ms(tmr16bit256ms_Task1Hour)) {
            //Set to trigger again in 1 hour (3,600,000 ms) = 3,600,000/256 = 14,062 (actually 3,599,872)
            tmr16bit256ms_Task1Hour += 14062;

            //..... Add code here to be executed every hour
        }

        /////////////////////////////////////////////////
        //Toggle port 5 (old port name X5) every 800ms
        if (tick8HasExpired_16ms(tmr8bit16ms_ToggleX5)) {
            tmr8bit16ms_ToggleX5 += 50;   //Set to trigger again in 50x16 = 800ms

            LAT_05 = !LAT_05;
        }
    }//end while

    return 0;
}//end main
