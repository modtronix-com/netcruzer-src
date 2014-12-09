/**
 * @example db66dev1_debug_demo/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to:
 * - Read the value of a potentiometer connected to an analog input
 * - Read buttons and control LEDs on the DB66DEV1 development board (using
 *   DB66DEV1 driver located in nz_db66dev1.h)
 * - Output a frequency with varying duty cycles to a PWM channel. The PWM output
 *   is connected to a buzzer, and the duty cycle is used to set the volume.
 *
 * This demo uses a <a href="http://netcruzer.com/db66dev1.html">DB66DEV1</a> development
 * daughter board. It has 4 buttons, 8 LEDs, a potentiometer and a buzzer.
 *
 * The potentiometer is used to set the frequency of the buzzer to a value between
 * 255 to 2795 Hz.
 *
 * Buttons 1 and 2 are used to set the volume to a level between 0 to 8. The volume
 * is represented on the LEDs, with volume 0=all LEDs of, and volume 8 all LEDs on.
 *
 * The PWM duty cycle is set to a value between 0 to 0.33 depending on the current
 * volume setting.
 *
 * Additionally this demo also implements debugging via the USB port and the
 * <a href="http://www.netcruzer.com/usbterminal/">Netcruzer USB Terminal</a> App.
 * During the program various debug messages are written, which will be displayed
 * on the "Netcruzer USB Terminal" (running on a PC connected to the target
 * board's USB port).
 * This demo also monitors the USB port for Debug messages send to us.
 * <br>- If "hi" is received, it replies with "Hello"
 * <br>- If "adc" is received, it replies with the analog value of the potentiometer
 * <br>Use the "Netcruzer USB Terminal" App to send and receive USB Debug messages.
 * It can be downloaded here: http://www.netcruzer.com/usbterminal/
 *
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with an USB Port (not USB Host), <b>AND</b> a <a href="http://netcruzer.com/db66dev1.html">DB66DEV1</a>
 * development board. The DB66DEV1 is plugged into the daughter board connector of the SBC66 board.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/db66dev1/db66dev1_debug_demo" folder of the
 * <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer Download</a>. To compile for
 * Netcruzer Board, open this project in MPLAB X, and select the "Project Configuration" for desired board. For
 * example "SBC66ZL_R1" for the <a href="http://netcruzer.com/sbc66zl.html">SBC66ZL</a> Revision 1 board.
 * For details <a target="_blank" href="http://netcruzer.com/nz/doc/building_projects/">click here</a>
 *
 * <h2>===== Programming Board =====</h2>
 * After compiling (build), the board can be programmed via the <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb">
 * USB Bootloader</a> or a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_prog">PIC Programmer</a>.
 * USB Programming is simplified when using the SBC board together with a
 * <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb_pt66">Prototype Board</a>.
 *
 * <h2>===== File History =====</h2>
 * 2013-07-22, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

// Includes /////////////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects
#include "nz_db66dev1.h"

//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN     DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"

// Variables ////////////////////////////////////
static WORD potValue = 0, newPotValue = 0;
static BYTE volume = 0, newVolume = 0;
static BYTE buttons;
static WORD tmrFlashLed = 0;        //Timer for flashing system LED
static WORD tmrServiceDB66DEV1 = 0; //Timer for checking button and Potentiometer inputs


/**
* Main application entry point.
*/
int main(void) {

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nThis is a test debug message from db66dev1 demo");

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs

    //Initialize DB66DEV1, all LEDs are output and turned off, Buttons inputs
    db66dev1_Init();

    //Configure port 3 (old port name X3) as ADC channel, is connected to Potentiometer on DB66DEV1
    adcOpen(ADC_OPEN_A3);

    //Congigure port 30 (old port name Y4) (Buzzer on DB66DEV1) as PWM channel 1. 
    pwm1OpenDefault(PPS_OUT_34, 0);

    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //Service DB66DEV1 every 10ms
        if (tick16TestTmr(tmrServiceDB66DEV1)) {
            tick16UpdateTmrMS(tmrServiceDB66DEV1, 10);   //Update timer to expire in 10ms again

            db66dev1_Service();     //DB66DEV1 service routine. Must be called each 10ms (default, can be changed)

            //Get latched button value. This function will clear the latched value of all buttons!
            buttons = db66dev1_ReadLatchedButtons();

            //Was button 2 pressed, increase volume
            if(buttons & 0x02) {
                //Increase volume, maximum level is 8
                if (newVolume < 8)
                    newVolume++;
            }
            //Was button 1 pressed, decrease volume
            if(buttons & 0x01) {
                //Decrease volume, minimum level is 0
                if (newVolume != 0)
                    newVolume--;
            }

            //Get the potentiometer analog input, will be a value from 0 to 255
            newPotValue = adcReadChan8BitForIndex(0);    //Use index function

            //Only enter if volume of potentiometer value has changed
            if ( (potValue!=newPotValue) || (newVolume!=volume) ) {
                WORD pwmPeriod;
                float dcRatio;      //Duty Cycle ratio
                potValue = newPotValue;
                volume = newVolume;

                //Update LEDs with new volume level.
                //For volume=0, all LEDs are off, for volume=8 all LEDS are on
                db66dev1_WriteLeds(0xff >> (8-volume));

                //Calculate period with forumula: 0xffff - (potValue * 250) <br>
                //This gives a range of 1785 to 65535, which gives a frequency range
                //of 244 to 8963Hz. For pwmSetPeriod(), the frequency is calculated:
                //PWM Frequency = 16000000 / (period+1)
                pwmPeriod = 0xffff - (potValue * 250);
                pwmSetPeriod(1, pwmPeriod);                     //Set PWM1 period with calculated value
                dcRatio = (((float)(volume*volume)) / 192.0);   //Duty cycle range from 0 to 0.33 (volume*volume gives exponential curve)
                pwmSetDutyCycle(1,((float)pwmPeriod)*dcRatio);  //Set duty cycle to 25% (one quarter)
                printf("\nSetting f=%.0fHz and dc=%.2f", (double)pwmGetFrequency(1), (double)dcRatio);  //Print frequency and duty cycle to Debug Terminal

                //ALTERNATIVE METHOD, COMMENTED BY DEFAULT!
                //Set frequency and duty cycle.
                //Frequency can have a minimum value of 245. Range is 245 to 2795 Hz
                //Duty cycle has a range from 0 to 0.33. Use exponential curve (volume*volume)
                //pwmSetFreqAndDutyCycle(1, (newPotValue*10)+245, dcRatio );
                //printf("\nSetting f=%uHz and dc=%.2f", (newPotValue*10)+245, dcRatio);  //Print frequency and duty cycle to Debug Terminal
            }
        }

        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 100); //Update timer to expire in 10ms again
            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
        }
    }//end while
}

#if defined(HAS_NZ_DEBUGGING)
/**
 * Debug service routine. This function is required when debugging is enabled. It is
 * called from the system! We have to process any received debug message.
 */
void debugService(void) {
    //Check if a debug message was received
    if (cbufHasWholePacket(CIRBUF_RX_DEBUG)) {
        //'hi' - Reply with "Hello" on a new line
        if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "hi") == 0) {
            debugPutString("\nHello");
        }
        //"adc' - Print Potentiometer ADC value
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "adc") == 0) {
            debugPutString("\nADC = ");
            debugPutWord(adcReadChan8BitForIndex(0));
            //printf("\nADC = %d", adcReadChan8BitForIndex(0));  //Alternative method
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif