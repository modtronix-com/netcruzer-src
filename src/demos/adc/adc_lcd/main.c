/**
 * @example adc_lcd/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use the ADC (Analog to Digital) converter on SBC66 Netcruzer boards. The SBC66 Netcruzer boards have
 * 12 Analog input ports, port 0-5 and 30-35 (old port names X0-X5, and Y0-Y5). For details see http://netcruzer.com/nzdoc-sbc66-ports/
 * For this demo, ports 1,2 and 3 (X1, X2 and X3) are configured as Analog Inputs, and their values are displays on a LCD display.
 * By default the ADC is configured for voltages between 0 - 2.5V (2.5V reference). To configure it for 3.3V inputs, see nz_analog.h file.
 * A Modtronix LCD2S type LCD display is used, and is enabled by adding the following line to projdefs.h:
 * @code
 * #define  NZSYS_ENABLE_LCD2S_AND_I2C1
 * @endcode
 * Additionally the System LED blinks every second.
 *
 * <h2>===== Required Hardware =====</h2>
 * This project can be run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>. For prototyping, we recommend
 * combining this board with a <a href="http://netcruzer.com/products-sbc66-prototype/">Prototyping Board</a>, like the PT66ECI for example. This
 * low cost prototyping board makes all the I/O ports of the SBC66 board available via marked labels on the PCB. It also provides a reset and
 * firmware button that simplifies prototyping.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/adc/adc_lcd" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
#include "HardwareProfile.h"    //Required for all Netcruzer projects
#include <stdio.h>              // For sprintf

/**
 * Main program entry point.
 */
int main(void)
{
    char buf[32];
    WORD tmrFlashLed = 0;      //Timer for flashing system LED
    WORD tmrDisplayADC = 0;    //Timer for printing LCD value on LCD.

    nzSysInitDefault();         //Default initialization. All ports inputs. All analog features disabled. Tick 1ms

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs.

    //Configure port 1, 2 and 3 (old port names X1, X2 and X3) as ADC channels
    adcOpen(ADC_OPEN_A1 | ADC_OPEN_A2 | ADC_OPEN_A3);

    //Write string to LCD display. '\f' and '\n' are special characters for the LCD2S
    //- '\f' (0x0c)= Clear display and go to beginning of first line.
    //- '\n' (0x0a)= Go to beginning of next line.
    lcdDisplayString(0, "\fADC Demo A1=0000 mV\nA2=0000  A3=0000");

    //Set first LCD update to happen in 500ms. This ensures that LCD display internal buffer does not overrun at first update.
    //Can happen because above we sent lots of data to LCD, and LCD updates below also sends quite a bit of data to LCD. The
    //LCD2S has an internal buffer, and needs time to process received commands.
    tmrDisplayADC = tick16Get() + 500;

    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        /////////////////////////////////////////////////
        //Display ADC values on LCD display every 200ms
        if (tick16TestTmr(tmrDisplayADC)) {
            tick16UpdateTmrMS(tmrDisplayADC, 200);   //Update timer to expire in 200ms again

            //Write the millivolt value of port 1 (X1) to LCD display (%04d means write decimal, always 4 numbers long, pad with 0's)
            //By default, the ADC is configured to use the external 2.5V precision reference, and input has a range from 0 - 2.5V.
            //To use internal 3.3V supply as reference (less accurate), add following line to projdefs.h file (see nz_analog.h for details):
            //#define    ADC_3V3_VREF
            lcdSetCursorPosition(0, 1, 13);  //Position cursor at value for port 1 (X1) (row 1, column 13)
            sprintf(buf, "%04d", adcReadChanMv(ADC_CH_A1));
            lcdDisplayString(0, buf);

            //Write value of port 2 and 3 (old port names X2 and X3) to LCD display ((%04d means write decimal, always 4 numbers long, pad with 0's)
            //The values are from 0-1023, representing 0-2.5V voltage on input. For example, 1023=2.5V, 511=1.25V...
            //To get millivolt value in stead, use adcReadChanMv() function! For 3.3V input, see comments above.
            lcdSetCursorPosition(0, 2, 1);  //Position cursor at beginning of second line. Write whole line.
            sprintf(buf, "A2=%04d  A3=%04d", adcReadChan(ADC_CH_A2), adcReadChan(ADC_CH_A3));
            lcdDisplayString(0, buf);
        }

        /////////////////////////////////////////////////
        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 200);     //Update timer to expire in 200ms again
            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
        }
    }//end while

    return 0;
}//end main

