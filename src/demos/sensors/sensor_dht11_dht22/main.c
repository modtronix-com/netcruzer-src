/**
 * @example sensor_dht11_dht22/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to communicate with the DHT11 and DHT22 1-Wire interface
 * chip, using the nz_sensDht.h and nz_sensDht.c driver. It uses non-blocking
 * (asynchronous) functions, meaning all processing is done in background
 * interrupts and fibers.
 *
 * By default it is configured for DHT22 sensors. This is configured via the
 * SENS_DHT_TYPE define in the projdefs.h file for DHT11 or DHT22 sensor types.
 *
 * It also flashes the system LED.
 * To add DHT22 support to a project, the following must be done:
 * - Add nz_sensDht.c to the project, this is the main DHT22 driver file.
 * - The project must have "Netcruzer RTOS Fibers" enabled, see @ref info_rtos_usage_fiber.
 * - The following additional files are required by nz_sensDht.c, and must be added to the project:
 *   nz_helpers.c, nz_ioPorts.c, nz_netcruzer.c
 * - Add sensDht_task() to main loop. For example:
 *     sensDht_init(36);    //Initialize using port 36 (old port name Y6) for DHT Sensor
 *       while(1) {
 *           nzSysTaskDefault();  //Main netcruzer task, call in main loop.
 *         sensDht_task();        //DHT22 Task
 *     }
 * - In code, configure interrupt for pin used by DHT Sensor. In the ISR for this interrupt, call
 *   sensDht_isr_MACRO() macro. See demo below for example.
 * - In projdefs.h, do any DHT22 or other configuration required. This is done by copying the
 *   configuration section from the *.h files to projdefs.h. Nothing required, defaults will work!
 * - All done! Can now use DHT22.h driver functions!
 *
 * This demo can be used with any
 * <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with an USB Port (not USB Host). It uses USB Commands and Debug Messages sent
 * and received via the <a href="http://www.netcruzer.com/usbterminal/">Netcruzer USB Terminal</a>
 * app to communicate with board.
 *
 * Additionally this demo also implements debugging via the USB port and the
 * <a href="http://www.netcruzer.com/usbterminal/">Netcruzer USB Terminal</a> App.
 * During the program various debug messages are written, which will be displayed
 * on the "Netcruzer USB Terminal" (running on a PC connected to the target
 * board's USB port).
 * This demo also monitors the USB port for Debug messages send to us.
 * <br>- If "hi" is received, it replies with "Hello"
 * <br>Use the "Netcruzer USB Terminal" App to send and receive USB Debug messages.
 * It can be downloaded here: http://www.netcruzer.com/usbterminal/
 *
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with an USB Port (not USB Host), like the
 * <a href="http://netcruzer.com/sbc66zl/">SBC66ZL</a> for example.
 * Additionally a <a href="http://www.netcruzer.com/sens-dht11.html">DHT11</a>
 * or <a href="http://www.netcruzer.com/sens-dht22.html">DHT22</a> Sensor has to be
 * connected to any of the SBC66 port pins. 
 *
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/sensors/sensor_dht11_dht22" folder of the
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
 * 2014-02-10, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

// Includes /////////////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects
#include "nz_sensDht.h"
#include "nz_helpersCx.h"
#include "nz_interrupt.h"
#include "nzos_fiber.h"

//Add debugging, and define debugging level. Is a DEBUG_LEVEL_xx define. Set to DEBUG_LEVEL_OFF to disable.
#define MY_DEBUG_LEVEL   DEBUG_LEVEL_INFO
#include "nz_debug.h"


// Defines //////////////////////////////////////

// Variables ////////////////////////////////////
WORD        tmrFlashLed;    //Timer for flashing system LED
FIBER_TCB   fbrTcbDHT22;    //DHT22 Fiber TCB


/**
 * Interrupt on change ISR, called if any of the configured ports change value
 */
void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void)
{
    WORD utick15bit_8us;
    intOnChangeClearIF();           //Clear "interrupt on change" IF (interrupt flag)

    //If "Interrupt on Change" is configured for inputs other than DHT Sensors, a check will be
    //required here to ensure a DHT sensor caused this interrupt! Not required for this example.
    //NZ_INT_DIS_PUSH();        //Disable interrupts - not required if this ISR has a high priority!

    utick15bit_8us = tick16Get_8us_noDisi();        //Get 8us time

    //The sensDht_isr_MACRO() Macro must be called in interrupt. Will defer processing to Fiber
    sensDht_isr_MACRO(utick15bit_8us);
    //NZ_INT_EN_POP();        //Enable interrupt

    //Schedule the DHT22 fiber (calls the sensDht_fbrTask() function) to run in lower interrupt priority.
    nzFbrSchedule(&fbrTcbDHT22);
}

/**
* Main application entry point.
*/
int main(void) {
    BYTE i;
    tmrFlashLed = tick16Get();    //Timer for flashing system LED

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    //Create the DHT Sensor fiber! This will be executed in a lower interrupt priority than the main
    //DHT Sensor "interrupt on change" interrupt. The fiber will call the sensDht_fbrTask() function.
    nzFbrCreate(2, TRUE, sensDht_fbrTask, &fbrTcbDHT22);

    debugPutString("\nThis is a test debug message from sensor_dht11_dht22");

    //Set System LED port as outputs. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output
    DIR_SYSLED = OUTPUT_PIN;

    //DHT22 Initialization. First sensor is given with sensDht_init(). Use sensDht_addSensor() to add additional sensors.
    sensDht_init(7, 3000, IOPORT_ID_NA, 0);     //Initialize DHT22, using port 7 (old port name X7), startup delay 3 seconds
    sensDht_addSensor(1, 8);                    //Add sensor on IO Port 8 (old port name Y8) (sensor index 1)

    //Configure the "interrupt on change" for pin that has a DHT Sensor - Ports 7 & 8 (old port names X7 & X8)
    //IMPORTANT!!! Use equal or higher priority than is used for System Tick = 4 by default.
    #if (nzINT_PRIORITY_TICK >= 6)
    #error "Ensure Tick priority is smaller or equal to Interrupt On Change priority"
    #endif
    intOnChangeConfig(CHANGE_INT_ON|CHANGE_INT_PRI_6);  //Enable Interrupt on change at Int Priority level 6 (high)

    intOnChangeClearIF();       //Clear "interrupt on change" IF (interrupt flag)
    intOnChangeEnablePort(7);   //Enable "interrupt on change" for each sensor
    intOnChangeEnablePort(8);   //Enable "interrupt on change" for each sensor

    delay_ms(120);              //Wait for DHT22 to settle

    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        sensDht_task();         //DHT22 Task

        //Check if new data available for any of the sensors
        for(i=0; i<SENS_DHT_NUMBER_OF_SENSORS; i++) {
            if (sensDht_hasNewTemperature(i)) {
                sprintf(debugTempBuf, "\nT%d: %d.%d",
                    i,
                    sensDht_getTemperatureInteger(i),
                    sensDht_getTemperatureDecimal(i));
                debugPutString(debugTempBuf);
            }
            if (sensDht_hasNewHumidity(i)) {
                sprintf(debugTempBuf, "\nRH%d: %d.%d",
                    i,
                    sensDht_getHumidityInteger(i),
                    sensDht_getHumidityDecimal(i));
                debugPutString(debugTempBuf);
            }
        }

        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED; //Toggle System LED

            //Enable this code if SENS_DHT_AUTO_READ_PERIOD is 0. No auto reading!
            //if (sensDht_isIdle() == TRUE) {
            //    sensDht_startRead();
            //}
        }
    }//end while
}


#if defined(DEBUGGING_ENABLED)
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
        //Add custom debug message processing
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "...") == 0) {
            debugPutString("\nReceived unknown debug message");
            //..... Do something
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif
