/**
 * @example adc_indexed/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use the ADC (Analog to Digital) converter on SBC66 Netcruzer boards. The SBC66 Netcruzer boards have
 * 12 Analog input ports, port 0-5 and 30-35 (old port names X0-X5, and Y0-Y5). For details see http://netcruzer.com/nzdoc-sbc66-ports/
 * For this demo, port 1 and 4 (X1 and X4) are configured as an Analog Inputs. By default the ADC is configured for voltages between 0 - 2.5V
 * (To configure it for 3.3V inputs, see nz_analog.h file). For this demo:<br>
 * - When 1 (X1) goes above 2.0V, output 38 (Y8) is turned on (3.3V), else off.<br>
 * - When port 4's (X4) ADC's value goes above 900, output 39 (Y9) is turned on (3.3V), else off.
 * This demo uses the Indexed functions ( adcReadChanMvForIndex() ), which are faster than the non-indexed ones ( adcReadChanMv() ).
 * Additionally the System LED blinks every second.
 *
 * <h2>===== Required Hardware =====</h2>
 * This project can be run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>. For prototyping, we recommend
 * combining this board with a <a href="http://netcruzer.com/products-sbc66-prototype/">Prototyping Board</a>, like the PT66ECI for example. This
 * low cost prototyping board makes all the I/O ports of the SBC66 board available via marked labels on the PCB. It also provides a reset and
 * firmware button that simplifies prototyping.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/adc/adc_indexed" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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

//For this demo, A1 (X1) is first channel (index 0), and A4 (X4) second channel (index 1)
#define ADC_INDEX_A1 0
#define ADC_INDEX_A4 1

/**
 * Main program entry point.
 */
int main(void)
{
    WORD tmrFlashLed = 0;  //Timer for flashing system LED

    nzSysInitDefault();             //Default initialization. All ports inputs. All analog features disabled. Tick 1ms

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs
    DIR_38 =     OUTPUT_PIN;    //Set Port 38 (old port name Y8) as an output pin
    DIR_39 =     OUTPUT_PIN;    //Set Port 39 (old port name Y9) as an output pin

    //Configure port 1 and 4 (old port names X1 and X4) as ADC channels
    adcOpen(ADC_OPEN_A1 | ADC_OPEN_A4);

    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //If port 1 (X1) analog input is above 2.0V (2000mV), set 38 (old port name Y8) pin to 3.3V, else to 0V
        //This funcion convertes the ADC value (0-1023), to a millivolt value
        if (adcReadChanMvForIndex(ADC_INDEX_A1) > 2000)
            PIN_38 = 1;
        else
            PIN_38 = 0;

        //If port 4 (old port name X4) analog input value is above 900 (0-2.5V = 0-1023), set port 39 (old port name Y9)
		//pin to 3.3V, else to 0V. This function gets te ADC's 10-bit value, which has a range from 0 - 1023.
        if (adcReadChanForIndex(ADC_INDEX_A4) > 2000)
            PIN_39 = 1;
        else
            PIN_39 = 0;

        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 200);     //Update timer to expire in 200ms again
            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
        }
    }//end while

    return 0;
}//end main

