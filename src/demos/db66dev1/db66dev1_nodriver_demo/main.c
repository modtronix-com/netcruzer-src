/**
 * @example db66dev1_nodriver_demo/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo contains some test code for the DB66DEV1 development daughter board.
 * It does NOT use the DB66DEV1 driver located in nz_db66dev1.h, because we want
 * to show how to control the hardware without the driver doing it all!
 * The 4 buttons on the DB66DEV1 will simply control LEDs 4 to 8 (on/off when button
 * pressed/depressed).
 * We will check the voltage on the Analog input (connected to potentiomer on DB66DEV1)
 * and if above 2.0V turn on LED1 and sound the buzzer.
 *
 * The DB66DEV1 has a potentiometer on port 3 (old port name X3). This port has an ADC (Analog to Digital)
 * converter on all SBC66 Netcruzer boards. The SBC66 Netcruzer boards have 12 Analog input ports, port 0-5
 * and 30-35 (old port names X0-X5, and Y0-Y5). For details see http://netcruzer.com/nzdoc-sbc66-ports/
 * Port 3 is configured as an Analog Inputs. By default the ADC is configured for voltages
 * between 0-2.5V, include "#define ADC_3V3_VREF" in projdefs.h for 0-3.3V range.
 *
 * The DB66DEV1 has 4 button on ports 30, 31, 32 and 33 (old port names Y0, Y1, Y2 and Y3). They are all configured as inputs.
 *
 * The DB66DEV1 has 8 LEDs on ports 6, 7, 8, 9, 26, 27, 28, 29 (Old port names X6, X7, X8, X9, T4, T5, T6 and T7).
 * These ports are all configured as outputs.
 *
 * Port 3 (old port name X3) is configured as an Analog Inputs. By default the ADC is configured for voltages between 0 - 2.5V
 * (To configure it for 3.3V inputs, see nz_analog.h file).
 *
 * Additionally the System LED blinks every second.
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with an USB Port (not USB Host), <b>AND</b> a <a href="http://netcruzer.com/db66dev1.html">DB66DEV1</a>
 * development board. The DB66DEV1 is plugged into the daughter board connector of the SBC66 board.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/db66dev1/db66dev1_nodriver_demo" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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

//Button defines
#define BUTTON1         PIN_30
#define BUTTON2         PIN_31
#define BUTTON3         PIN_32
#define BUTTON4         PIN_33
#define BUTTON1_PULLUP  PULLUP_30
#define BUTTON2_PULLUP  PULLUP_31
#define BUTTON3_PULLUP  PULLUP_32
#define BUTTON4_PULLUP  PULLUP_33
#define BUTTON1_DIR     DIR_30
#define BUTTON2_DIR     DIR_31
#define BUTTON3_DIR     DIR_32
#define BUTTON4_DIR     DIR_33

//LED define
#define LED1        LAT_06
#define LED2        LAT_07
#define LED3        LAT_08
#define LED4        LAT_09
#define LED5        LAT_26
#define LED6        LAT_27
#define LED7        LAT_28
#define LED8        LAT_29
#define LED1_DIR    DIR_06
#define LED2_DIR    DIR_07
#define LED3_DIR    DIR_08
#define LED4_DIR    DIR_09
#define LED5_DIR    DIR_26
#define LED6_DIR    DIR_27
#define LED7_DIR    DIR_28
#define LED8_DIR    DIR_29

//Other defines
#define BUZZ_PERIOD   PWM_CALCULATE_PERIOD_FOR_FREQUENCY(1000)

/**
 * Main program entry point.
 */
int main(void)
{
    WORD tmrFlashLed = 0;   //Timer for flashing system LED
    WORD tmrProcessIO = 0;  //Timer for processing I/O

    nzSysInitDefault();         //Default initialization. All ports inputs. All analog features disabled. Tick 1ms

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs

    //LED outputs
    LED1_DIR = OUTPUT_PIN;
    LED2_DIR = OUTPUT_PIN;
    LED3_DIR = OUTPUT_PIN;
    LED4_DIR = OUTPUT_PIN;
    LED5_DIR = OUTPUT_PIN;
    LED6_DIR = OUTPUT_PIN;
    LED7_DIR = OUTPUT_PIN;
    LED8_DIR = OUTPUT_PIN;

    //Configure Button ports as inputs
    BUTTON1_DIR = INPUT_PIN;
    BUTTON2_DIR = INPUT_PIN;
    BUTTON3_DIR = INPUT_PIN;
    BUTTON4_DIR = INPUT_PIN;

    //Enable pull-up resistors for all button inputs
    BUTTON1_PULLUP = 1;
    BUTTON2_PULLUP = 1;
    BUTTON3_PULLUP = 1;
    BUTTON4_PULLUP = 1;

    adcOpen(ADC_OPEN_A3);       //Configure port 3 (old port name X3) as ADC channels

    //Configure Port 34 (old port name Y4) (Buzzer on DB66DEV1) as PWM channel 1, with default frequency of 16kHz.
    //The value of the PWM1_FREQ define determines the frequency, and is 16kHz by default.
    //To change the default frequency, do one of the following:
    // - Redefine PWM1_FREQ in projdefs.h file for desired frequency (For example: #define PWM1_FREQ  PWM_FREQ_80KHZt)
    // - Call pwm1SetPeriod() with period for desired frequency
    pwm1OpenDefault(PPS_OUT_34, 0);

    //Update period of PWM to 1kHz (defined in BUZZ_PERIOD), the pwm1OpenDefault() function
    //configured it to 16kHz by defailt.
    pwm1SetPeriod( BUZZ_PERIOD );
    pwm1SetDutyCycle(0);            //Set duty cycle to 0, turns buzzer off

while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //Process I/O every 50ms
        if (tick16TestTmr(tmrProcessIO)) {
            tick16UpdateTmrMS(tmrProcessIO, 50); //Update timer to expire in 50ms again

            LED5 = !BUTTON1;    //Turn on LED 5 if button 1 pressed, and off if not
            LED6 = !BUTTON2;    //Turn on LED 6 if button 2 pressed, and off if not
            LED7 = !BUTTON3;    //Turn on LED 7 if button 3 pressed, and off if not
            LED8 = !BUTTON4;    //Turn on LED 8 if button 4 pressed, and off if not

            //If analog input is above 2.0V (2000mV), turn LED1 and Buzzer on
            if (adcReadChanMv(ADC_CH_A3) > 2000) {
                LED1 = 1;               //Turn LED on

                //Set duty cycle to 50% (period/2). This gives maximum volume on buzzer
                pwm1SetDutyCycle( BUZZ_PERIOD/2 );
            }
            else {
                LED1 = 0;               //Turn LED off
                pwm1SetDutyCycle(0);    //Set duty cycle to 0%, buzzer if off
            }
        }

        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
        }
    }//end while

    return 0;
}//end main

#if !defined(PPS_OUT_34)
#error "Can not build for this target, it does not have a reconfigurable Port 34 (old port name Y4)n! This is the case for the SBC66EC Revision 1 board. Select a different Configuration!"
#endif
