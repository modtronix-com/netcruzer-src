/**
 * @example pwm_demo1/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to configure a Relocatable Port as a PWM channel. The SBC66 Netcruzer boards have
 * 9 PWM channels. They can be assigned to any of the Relocatable Ports. For the location of the Relocatable
 * Ports, see http://netcruzer.com/nzdoc-sbc66-ports/
 * For this demo, connect a LED to port 6 (old port name X6), with anode to port 6 and cathode to 0V via a 1k resistor.
 * Or, the DB66DEV1 development board can be used, which has LED1 on this port. This demo
 * configures port 6 (old port name X6) as a PWM output, and will gradually increase and decrease the brightness
 * of the LED every second.
 * - At startup duty cycle of PWM output is 0
 * - During next second will gradually increase duty cycle every 10 milli seconds by 1%, until it reaches 100% (fully on)
 * - During next second will gradually decrease duty cycle every 10 milli seconds by 1%, until it reaches 0% (fully off)
 * - Repeats steps 2 and 3.
 * Additionally it will blink the System LED every second.
 *
 * <h2>===== Required Hardware =====</h2>
 * This project can be run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>. For prototyping, we recommend
 * combining this board with a <a href="http://netcruzer.com/products-sbc66-prototype/">Prototyping Board</a>, like the PT66ECI for example. This
 * low cost prototyping board makes all the I/O ports of the SBC66 board available via marked labels on the PCB. It also provides a reset and
 * firmware button that simplifies prototyping.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/pwm/pwm_demo1" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
    BYTE pwmValue = 0;          //Pwm value from 0 to 100 (0 to 100%)
    BOOL pwmDecInc = TRUE;      //Flag used to indicate if PWM duty cycle is currently decreasing (0), or increasing (1)

    //Declare all timers, and initialize with current timer value (tick16Get...() functions)
    WORD tmrFlashLed = tick16Get();     //Timer for flashing system LED
    WORD tmrAdjustPWM = tick16Get();    //Timer for flashing system LED

    nzSysInitDefault();         //Default initialization. All ports inputs. All analog features disabled. Tick 1ms

    DIR_SYSLED = 0; //Set System LED port as outputs. Setting DIR_xxx to 1 set's port to input, and 0 as output

    //Configure Port 6 (old port name X6) (LED1 on DB66DEV1) as PWM channel 2, with default frequency of 16kHz.
    //The value of the PWM2_FREQ define determines the frequency, and is 16kHz by default.
    //To change the default frequency, do one of the following:
    // - Redefine PWM2_FREQ in projdefs.h file for desired frequency (For example: #define PWM2_FREQ  PWM_FREQ_80KHZt)
    // - Call pwm2SetPeriod() with period for desired frequency
    pwm2OpenDefault(PPS_OUT_06, 0);

    while(1)
    {
        /////////////////////////////////////////////////
        //Adjust PWM value every 10ms
        if (tick16TestTmr(tmrAdjustPWM)) {
            tick16UpdateTmrMS(tmrAdjustPWM, 10);  //Update timer to expire in 10ms again

            if (pwmDecInc == TRUE)
                pwmValue++;
            else
                pwmValue--;

            pwm2SetPercent(PWM2_FREQ, pwmValue);   //Set duty cycle (in percentage) of PWM channel 2

            if ( ((pwmDecInc==FALSE)&&(pwmValue==0)) || ((pwmDecInc==TRUE)&&(pwmValue==100)) ) {
                pwmDecInc = !pwmDecInc;
            }
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
