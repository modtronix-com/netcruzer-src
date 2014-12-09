/**
 * @example ow_ds2482_demo1_debug/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to communicate with the DS2482 1-Wire interface chip, using the
 * nz_ds2482.h and nz_ds2482.c driver. It uses blocking (synchronous) DS2482 functions.
 * These functions are simpler to implement than the "non blocking"
 * version, but are more complicated to implement. All blocking function names end
 * with "WATE" (Wait and Error processing). For example, the blocking version of
 * the owReadByte() function is owReadByteWATE().
 *
 * It also flashes the system LED. To add DS2482 support to a project, the following must be done:
 * - Add nz_ds2482.c to the project, this is the main DS2482 driver file.
 * - The following additional files are required by nz_ds2482.c, and must be added to the project:
 *   nz_circularBufferPwr2.c, nz_helpers.c, nz_netcruzer.c and nz_serI2C.c
 * - Add "NZ_I2C1_ENABLE" to projdefs.h file (assuming the DS2482 is on I2C 1).
 * - In code, create a DS2482_INFO structure for each DS2482, and initialize. For example:
 *     DS2482_INFO objDS2482;
 *     ds2482_init(&objDS2482, 1, DS2482_ADDRESS_00); //Initialize using I2C 1, and DS2482 AD0 and AD1 = 0
 * - In projdefs.h, do any DS2482, I2C or other configuration required. This is done by copying the
 *   configuration section from the *.h files to projdefs.h. Nothing required, defaults will work!
 * - All done! Can now use ds2482.h driver functions! For example, to write a byte to the 1-Wire Network:
 *     owWriteByteWATE(&objDS2482, 0xCC);    //Write Skip ROM = 0xCC
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
 * Additionally a DS2482 1-Wire to I2C Bridge chip has to be connected to the I2C 1 port of
 * the SBC66 board. We recommend using one of our
 * <a href="http://www.netcruzer.com/products-sbc66-prototype">Prototype Boards</a> with an iMod port (
 * <a href="http://www.netcruzer.com/pt66eci.html">PT66ECI</a> for example), and our
 * <a href="http://www.netcruzer.com/products-imod-1wire">im1WP</a> 1-Wire DS2482 Interface Module.
 *
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/1wire/ow_ds2482_demo1_debug" folder of the
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
 * 2013-10-31, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

// Includes /////////////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects
#include "nz_ow2482.h"
#include "nz_serI2C.h"
#include "nz_helpers.h"

//Add debugging, and define debugging level. Is a DEBUG_LEVEL_xx define. Set to DEBUG_LEVEL_OFF to disable.
#define MY_DEBUG_LEVEL   DEBUG_LEVEL_INFO
#include "nz_debug.h"

// Variables ////////////////////////////////////
DS2482_INFO objDS2482;
WORD tmrFlashLed;   //Timer for flashing system LED

// Function Prototypes //////////////////////////
void printAll_DS18B20(DS2482_INFO* pObj, BOOL parasitePower);

/**
* Main application entry point.
*/
int main(void) {
    tmrFlashLed = tick16Get();    //Timer for flashing system LED

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    debugPutString("\nThis is a test debug message from ow_ds2482_demo1_debug");

    //Set System LED port as outputs. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output
    DIR_SYSLED = OUTPUT_PIN;

    delay_ms(10);

    //Initialize DS2482, using I2C 1, and DS2482 AD0 and AD1 = 0
    if (ds2482_init(&objDS2482, 1, DS2482_ADDRESS_00) == 0) {

        delay_ms(12);   //Wait for DS18B20 to settle

        //Search the 1-Wire Network for all DS18B20 devices, and for each one display it's Temperature.
        printAll_DS18B20(&objDS2482, TRUE);
    }
    else {
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDS2482 Initialization Error!");
    }


    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

         //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED; //Toggle System LED
        }
    }//end while
}


/**
 * Search a 1-Wire Network for all DS18B20 devices, and for each one display it's
 * Temperature.
 *
 * @param pObj Pointer to DS2482 Object.
 */
void printAll_DS18B20(DS2482_INFO* pObj, BOOL parasitePower) {
    #define MAX_DEVICES 4
    BYTE deviceAdr[MAX_DEVICES][8];
    BYTE get[10];
    WORD_VAL temp;
    BYTE i, idx;
    BYTE devices = 0;   //Count how many devices found
    double fTemp;

    //Reset search
    owSearchReset(pObj);

    debugPutString("\nSearching for DS18B20 devices");

    //Call this prior to using xxxWATE() functions to reset the status
    ds2482_resetStatus(pObj);

    //Search for devices, and store the addresses of all DS18B20 devices in deviceAdr[][]
    while (owSearchWATE(pObj) != 0) {
        //Is it a DS18B20, the LSB of the ROM Address
        if (pObj->adr64[0] == 0x28) {
            //Device found, save and print it's address
            debugPutString("\nDS18B20 Found: ");
            for (i=0; i<8; i++) {
                //Save addres
                deviceAdr[devices][i]=pObj->adr64[i];
                debugPutHexByte(pObj->adr64[i]);
            }
            //Check if deviceAdr[][] array is full, and break out of loop
            if (++devices == MAX_DEVICES) {
                debugPutString("\nFound maximum supported number of devices");
                break;
            }
        }
        else {
            debugPutString("\nFound unsupported device");
        }
    }

    //Print temperature for all DS18B20 devices found
    for(idx=0; idx < devices; idx++) {
        //Prepare for using xxxWATE() functions - reset any status error code
        ds2482_resetStatus(pObj);

        //Reset 1-Wire Net, and wait till done
        owResetWATE(pObj);

        //Selectt 1-Wire Net, and wait till done
        owSelectWATE(pObj, &deviceAdr[idx][0]);

        //If the DS18B20 is in "parasite power" mode (2 wires, no Vcc), than the "Strong
        //pull-up" on the 1-Wire network must be enabled after sending the conversion
        //command. This has to stay active until the conversion is done
        if (parasitePower==TRUE) {
            owWriteBytePowerWATE(pObj, 0x44);  //Start Conversion
        }
        else {
            owWriteByteWATE(pObj, 0x44);  //Start Conversion
        }

        //The delay required after issuing a "Start Conversion" command till the
        //depends on the bit resolution setting of the DS18B20. The possible delays
        //are:
        // - 94ms for 9 bit resolution
        // - 188ms for 10 bit resolution
        // - 375ms for 11 bit resolution
        // - 750ms for 12 bit resolution
        //Seeing that we have not read the Configuration register to determine the
        //resolution, assume worst case of 12 bit resolution
        delay_ms(750);

        owResetWATE(pObj);
        owSelectWATE(pObj, &deviceAdr[idx][0]);
        owWriteByteWATE(pObj, 0xBE);  //Read Scratch Pad

        if (ds2482_getStatus(pObj) != 0) {
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nprintDS18B20 ERROR!");
            return;
        }

        debugPutString("\nScratchPAD DATA = 0x");
        for (i = 0; i < 9; i++) {
            get[i] = owReadByteWATE(pObj);
            debugPutHexByte(get[i]);
            debugPutChar(' ');
        }

        temp.byte.LB = get[0];
        temp.byte.HB = get[1];
        fTemp = (temp.Val & 0x07f0)>>4;
        fTemp += 0.0625 * (temp.byte.LB & 0x0f);
        printf("\nTempC= %f degrees C", fTemp); // print temp. C
    }
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
        //'rst' - Send reset command to DS2482 chip
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "rst") == 0) {
            ds2482_reset(&objDS2482);
        }
        //'1w-rst' - Send a 1-Wire reset command to DS2482 chip
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "owRst") == 0) {
            owResetWATE(&objDS2482);
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
