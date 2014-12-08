/**
 * @example db66dev1_lcd2s/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo contains some test code for the DB66DEV1 development daughter board.
 * The potentiometer, connected to analog input 3 (old port name X3) controls the frequency of the
 * buzzer connected to port 34 (old port name Y4). This port is configured as a PWM output.
 * The 4 buttons on the DB66DEV1 will simply control LEDs 1 to 4 (on/off when button
 * pressed/depressed). LEDs 5 to 8 run from left to right, incremented each 100ms.
 *
 * Information is written out to a serial LCD2S type display.
 *
 * Additionally the the System LED blinks every second.
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
 * The DB66DEV1 has a buzzer on port 34 (old port name Y4).
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with an USB Port (not USB Host), a <a href="http://netcruzer.com/db66dev1.html">DB66DEV1</a>
 * development board, and a  <a href="http://netcruzer.com/lcd2s/">LCD2S</a> type serial LCD display.
 * The DB66DEV1 is plugged into the daughter board connector of the SBC66 board, and the LCD2S is plugged
 * into the red Micro Match connector on the SBC66 board.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/db66dev1/db66dev1_lcd2s" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
#include "nz_i2c1.h"
#include "nz_lcd2s.h"

/**
 * Main program entry point.
 */
int main(void)
{
    char lcdBuf[128];
    float freq;
    WORD potValue = 0, newPotValue = 0;
    BYTE ledRotate = 1;
    WORD tmrFlashLed = 0;   //Timer for flashing system LED
    WORD tmrProcessIO = 0;  //Timer for processing I/O

    nzSysInitDefault();         //Default initialization. All ports inputs. All analog features disabled. Tick 1ms

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs

    //Initialize DB66DEV1, all LEDs are output and turned off, Buttons inputs
    db66dev1_Init();

    adcOpen(ADC_OPEN_A3);       //Configure 3 (old port name X3) as ADC channels

    //Configure Port 32 (old port name Y4) (Buzzer on DB66DEV1) as PWM channel 1, with default frequency of 16kHz.
    //To change the default frequency, Redefine PWM1_FREQ in projdefs.h file for desired
    //frequency. For example: #define PWM1_FREQ  PWM_FREQ_1600HZ
    pwm1OpenDefault(PPS_OUT_34, 0);

    //Open I2C 1 port at 400kbit/second. I2C 1 is used for the LCD display (red connector) on all Netcruzer SBC66 board.
    i2c1Open(I2C_BAUD_100K);

    delay_ms(300); //Give LCD2S time to initialize itself
    lcdInit();

    //Write Parsed String to LCD. See "Write Parsed String" command in LCD2S documentation (http://netcruzer.com/doc?lcd2s)
    //for details. The '\f' character causes "clear screen and positions cursor at home position".
    //Writes "Hello" to first line, and "World!" to second line
    lcdDisplayString(0, "\fDB66DEV1 Demo");

while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //Process I/O every 50ms
        if (tick16TestTmr(tmrProcessIO)) {
            tick16UpdateTmrMS(tmrProcessIO, 100);    //Update timer to expire in 100ms again

            //Get the potentiometer analog input, will be a value from 0 to 255
            newPotValue = adcReadChan8Bit(ADC_CH_A3);

            //Only enter if potentiometer value has changed
            if (potValue!=newPotValue) {
                potValue = newPotValue;
                
                ///Multiply potValue by 10, and add 450, to get a frequency range of 450 to 3000Hz.
                freq = (potValue*10)+450;

                //Use 0.1 for duty cycle, so buzzer is not too loud (0.5 is maximum volume)
                pwmSetFreqAndDutyCycle(1, freq, 0.1);

                //If potentiometer is 0, turn buzzer off (duty cycle of 0)
                if (potValue == 0)
                    pwmSetDutyCycle(1, 0);

                //Write to second line of LCD display - \r\n = Beginning of LCD, skip to next line
                sprintf(lcdBuf, "\r\nFrequency = %04.0fHz\nADC A3 = %04u", (double)freq, potValue);

               //Write Parsed String to LCD. See "Write Parsed String" command in LCD2S documentation (http://netcruzer.com/doc?lcd2s)
                lcdDisplayString(0, lcdBuf);
            }

            //LEDs 1 to 4 are controlled by buttons 1 to 4
            LAT_LED1 = !PIN_BUTTON1;    //Turn on LED 1 if button 1 pressed, and off if not
            LAT_LED2 = !PIN_BUTTON2;    //Turn on LED 2 if button 1 pressed, and off if not
            LAT_LED3 = !PIN_BUTTON3;    //Turn on LED 3 if button 1 pressed, and off if not
            LAT_LED4 = !PIN_BUTTON4;    //Turn on LED 4 if button 1 pressed, and off if not

            //Rotate LEDs 5 to 8
            ledRotate = ledRotate << 1;
            if (ledRotate==0x10)
                ledRotate = 0x01;
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

#if !defined(PPS_OUT_34)
#error "Can not build for this target, it does not have a reconfigurable Port 34 (old port name Y4)! This is the case for the SBC66EC Revision 1 board. Select a different Configuration!"
#endif

