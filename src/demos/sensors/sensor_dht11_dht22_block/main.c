/**
 * @example sensor_dht11_dht22_block/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to communicate with the DHT11 and DHT22 1-Wire interface
 * chip, using the nz_sensDht.h and nz_sensDht.c driver. It uses non-blocking
 * (asynchronous) functions, meaning all processing is done in background
 * interrupts and fibers.
 *
 * It also flashes the system LED.
 *
 * To add DHT22 support to a project, the following must be done:
 * - Add nz_sensDht.c to the project, this is the main DHT22 driver file.
 * - The following additional files are required by nz_sensDht.c, and must be added to the project:
 *   nz_helpers.c, nz_ioPorts.c, nz_netcruzer.c
 * - Add sensDht_task() to main loop. For example:
 *     sensDht_init(36);    //Initialize using port 36 (old port name Y6) for DHT Sensor
 *       while(1) {
 *           nzSysTaskDefault();  //Main netcruzer task, call in main loop.
 *         sensDht_task();        //DHT22 Task
 *     }
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
#include "nz_sensDhtBlock.h"
#include "nz_helpersCx.h"
#include "nz_interrupt.h"

//Add debugging, and define debugging level. Is a DEBUG_LEVEL_xx define. Set to DEBUG_LEVEL_OFF to disable.
#define MY_DEBUG_LEVEL   DEBUG_LEVEL_INFO
#include "nz_debug.h"


// Defines //////////////////////////////////////

// Variables ////////////////////////////////////
TICK16      tmrFlashLed;    //Timer for flashing system LED
TICK16      tmrReadSensor;  //Timer for reading DHT sensor


/**
* Main application entry point.
*/
int main(void) {
    tmrFlashLed = tick16Get();    //Timer for flashing system LED
    tmrReadSensor = tick16Get();

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    debugPutString("\nThis is a test debug message from sensor_dht11_dht22_block");

    //Set System LED port as outputs. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output
    DIR_SYSLED = OUTPUT_PIN;

    //DHT22 Initialization. 
    //sensDht_init(IOPORT_ID_NA, 0);    //No power switch
    sensDht_init(1, 0);         //Power switch = 1 (old port name X1), not inverted
    sensDht_addSensor(7);      //Initialize port 7 (old port name X7) as a DHT sensor input
    sensDht_addSensor(8);      //Initialize port 7 (old port name X7) as a DHT sensor input

    delay_ms(120);              //Wait for DHT22 to settle

    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.
        sensDht_task();         //Main DHT Sensor task

        //Read DHT22 sensors
        if (tick16TestTmr(tmrReadSensor)) {
            //Check if DHT sensor is ready
            if (sensDht_isReady()) {
                tick16SetTmrMS(tmrReadSensor, 2000);    //Read again in 2 seconds

                //Read sensor on port 7
                if (sensDht_read(SENS_DHT_TYPE_DHT22, 7) == 0) {
                    sprintf(debugTempBuf, "\nTemperature(7)=%d.%d",
                        sensDht_getTemperatureInteger(),
                        sensDht_getTemperatureDecimal());
                    debugPutString(debugTempBuf);

                    sprintf(debugTempBuf, ", Humidity=%d.%d",
                        sensDht_getHumidityInteger(),
                        sensDht_getHumidityDecimal());
                    debugPutString(debugTempBuf);
                }
                //Error reading sensor!
                else {
                    debugPutString("\nError reading DHT22 on port 7!");
                }

                //Read sensor on port 8
                if (sensDht_read(SENS_DHT_TYPE_DHT22, 8) == 0) {
                    sprintf(debugTempBuf, "\nTemperature(8)=%d.%d",
                        sensDht_getTemperatureInteger(),
                        sensDht_getTemperatureDecimal());
                    debugPutString(debugTempBuf);

                    sprintf(debugTempBuf, ", Humidity=%d.%d",
                        sensDht_getHumidityInteger(),
                        sensDht_getHumidityDecimal());
                    debugPutString(debugTempBuf);
                }
                //Error reading sensor!
                else {
                    debugPutString("\nError reading DHT22 on port 8!");
                }
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
