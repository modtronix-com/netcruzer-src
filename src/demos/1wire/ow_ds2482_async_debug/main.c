/**
 * @example ow_ds2482_async_debug/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to communicate with the DS2482 1-Wire interface chip, using the
 * nz_ds2482.h and nz_ds2482.c driver. It uses non-blocking (asynchronous) DS2482
 * functions. These functions are more complex to implement than the blocking
 * version, but never cause delays blocking all other code from executing.
 * All blocking function names end with "WATE" (WAiT and Error processing). For example,
 * the blocking version of the owReadByte() function is owReadByteWATE().
 *
 * It also flashes the system LED. To add DS2482 support to a project, the following must be done:
 * - Add nz_ds2482.c to the project, this is the main DS2482 driver file.
 * - The following additional files are required by nz_ds2482.c, and must be added to the project:
 *   nz_circularBufferPwr2.c, nz_helpers.c, nz_netcruzer.c and nz_serI2C.c
 * - Add "HAS_SERPORT_I2C1" to projdefs.h file (assuming the DS2482 is on I2C 1).
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
 * This project is located in the "src/demos/1wire/ow_ds2482_async_debug" folder of the
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

//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN     DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"


// Defines //////////////////////////////////////
#define DS18B20_PARASITIC_POWER

// Variables ////////////////////////////////////
DS2482_INFO objDS2482;
WORD tmrFlashLed;   //Timer for flashing system LED
#define DS2482 (&objDS2482)

// Function Prototypes //////////////////////////
BYTE ds18b20_task(DS2482_INFO* pObj);

/**
* Main application entry point.
*/
int main(void) {
    tmrFlashLed = tick16Get();    //Timer for flashing system LED

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nThis is a test debug message from ow_ds2482_async_debug");

    //Set System LED port as outputs. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output
    DIR_SYSLED = OUTPUT_PIN;

    //DS2482 Initialization
    ds2482_init(DS2482, 1, DS2482_ADDRESS_00); //Initialize DS2482, using I2C 1, and DS2482 AD0 and AD1 = 0
    
    delay_ms(20);   //Wait for DS18B20 to settle

    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        ds18b20_task(DS2482);   //Read and display Temperature using non-blocking functions
        
        ds2482_task(DS2482);    //DS2482 Task

        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED; //Toggle System LED
        }
    }//end while
}


/**
 * Search a 1-Wire Network for all DS18B20 devices, and for each one display it's
 * Temperature. As and example, the
 *
 * @param pObj Pointer to DS2482 Object.
 *
 * @return 0 if done, else 1
 */
BYTE ds18b20_task(DS2482_INFO* pObj) {
    #define MAX_DEVICES 4
    static BYTE deviceAdr[MAX_DEVICES][8];
    static BYTE get[10];
    static BYTE devices;    //Count how many devices found
    static WORD_VAL temp;
    static WORD tmrDelay;
    static BYTE iRd=0;
    static BYTE idx=0;
    double fTemp;

    typedef enum SM_PRINT_DS18B20_ {
        SM_PRTDS_IDLE = 0,
        SM_PRTDS_SEARCH,
        SM_PRTDS_SEARCH_DONE,
        SM_PRTDS_NEXT_DEVICE,
        SM_PRTDS_NEXT_DEVICE_SELECT,
        SM_PRTDS_NEXT_DEVICE_START_CONVERSION,
        SM_PRTDS_NEXT_DEVICE_START_CONVERSION_DELAY,
        SM_PRTDS_NEXT_DEVICE_SELECT2,
        SM_PRTDS_NEXT_DEVICE_RD_SCRATCH_PAD,
        SM_PRTDS_READ_DS18B20_BYTES,
        SM_PRTDS_READ_DS18B20_BYTES2,
        SM_PRTDS_DONE,
    } SM_PRINT_DS18B20;
    static SM_PRINT_DS18B20 sm_prtds = SM_PRTDS_IDLE;
    
    #define SM_PRTDS_WAIT_FOR_TXION 0x80    //Wait for DS2482 Transmission - use upper bit 7
    #define SM_PRTDS_SM_MASK        0x7F    //Mask out all bits above

    
    /////////////////////////////////////////////////
    //Wait for current DS2482 Transmission to finish, and get status
    if (sm_prtds & SM_PRTDS_WAIT_FOR_TXION) {
        //DS2482 is still busy, return!
        if (ds2482_isBusy(pObj) == TRUE)
            return 1;                   //Return not done - waiting

        //Check if last transmission status is an Error!
        if (ds2482_getStatus(pObj) != 0) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nprintAllAsync_DS18B20 Error!");
            sm_prtds = SM_PRTDS_IDLE;   //Return to idle state
            return 0;                   //Return done
        }
        //Status was just received! Clear SM_TASK_WAIT_1WB flag
       sm_prtds = sm_prtds & ~SM_PRTDS_WAIT_FOR_TXION;
    }

    //State machine
    switch (sm_prtds & SM_PRTDS_SM_MASK) {
        case SM_PRTDS_IDLE:
            //Reset search
            devices = 0;
            owSearchReset(pObj);
            sm_prtds++; //Increment to next state
            //break;    //NO BREAK! Fall thru to next state
        case SM_PRTDS_SEARCH:
            //Search for devices, and store the addresses of all DS18B20 devices in deviceAdr[][]
            if (owSearch(pObj) != 0)
                goto PRINT_ALL_ASYNC_DS18B20_ERROR;
            //Increment to next state, and set SM_PRTDS_WAIT_FOR_TXION flag (wait for Txion)
            sm_prtds = SM_PRTDS_SEARCH_DONE | SM_PRTDS_WAIT_FOR_TXION;
            break;
        case SM_PRTDS_SEARCH_DONE:
            //A 1-Wire device was found (owGetAdr64() returns owSearch() result)
            if (owGetAdr64(pObj) != 0) {
                //Is it a DS18B20, the LSB of the ROM Address
                if (pObj->adr64[0] == 0x28) {
                    //Device found, save and print it's address
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDS18B20 Found: ");
                    for (iRd=0; iRd<8; iRd++) {
                        //Save and display address
                        deviceAdr[devices][iRd]=pObj->adr64[iRd];
                        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, pObj->adr64[iRd]);
                    }
                    //Check if deviceAdr[][] array is full, and break out of loop
                    if (devices == (MAX_DEVICES-1)) {
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nFound maximum supported number of devices");
                        break;
                    }
                    devices++;
                }
                else {
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nFound unsupported device ");
                    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
                    for (iRd=0; iRd<8; iRd++) {
                        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, pObj->adr64[iRd]);
                    }
                    #endif
                }
                
                //Return to "Search State", and search for next device
                sm_prtds = SM_PRTDS_SEARCH;
                break;
            }

            //All 1-Wire devices have been found, increment to next state
            idx = 0;    //Reset to first device index
            sm_prtds++; //Increment to next state
            break;
        case SM_PRTDS_NEXT_DEVICE:
            //Print temperature for all DS18B20 devices found
            if(idx++ < devices) {
                //Reset 1-Wire Net, and wait till done
                if (owReset(pObj) != 0)
                    goto PRINT_ALL_ASYNC_DS18B20_ERROR;
                //Increment to next state, and set SM_PRTDS_WAIT_FOR_TXION flag (wait for Txion)
                sm_prtds = SM_PRTDS_NEXT_DEVICE_SELECT | SM_PRTDS_WAIT_FOR_TXION;
                break;
            }
            //Done! Printed temperature for all devices!
            tmrDelay = tick16Get();
            sm_prtds = SM_PRTDS_DONE;
            break;
        case SM_PRTDS_NEXT_DEVICE_SELECT:
            //Select 1-Wire Net, and wait till done
            if (owSelect(pObj, &deviceAdr[idx-1][0]) != 0)
                goto PRINT_ALL_ASYNC_DS18B20_ERROR;
            //Increment to next state, and set SM_PRTDS_WAIT_FOR_TXION flag (wait for Txion)
            sm_prtds = SM_PRTDS_NEXT_DEVICE_START_CONVERSION | SM_PRTDS_WAIT_FOR_TXION;
            break;
        case SM_PRTDS_NEXT_DEVICE_START_CONVERSION:
            //Start Conversion (0x44), and wait(in next state) till done
            //If the DS18B20 is in "parasite power" mode (2 wires, no Vcc), than the "Strong
            //pull-up" on the 1-Wire network must be enabled after sending the conversion
            //command. This has to stay active until the conversion is done.
            #if defined(DS18B20_PARASITIC_POWER)
            if (owWriteBytePower(pObj, 0x44) != 0) {
            #else
            if (owWriteByte(pObj, 0x44) != 0) {
            #endif
                goto PRINT_ALL_ASYNC_DS18B20_ERROR;
            }

            //Increment to next state, and set SM_PRTDS_WAIT_FOR_TXION flag (wait for Txion)
            sm_prtds = SM_PRTDS_NEXT_DEVICE_START_CONVERSION_DELAY | SM_PRTDS_WAIT_FOR_TXION;
            tmrDelay = tick16Get();
            break;
        case SM_PRTDS_NEXT_DEVICE_START_CONVERSION_DELAY:
            //Wait delay required after issuing a "Start Conversion" command till the
            //depends on the bit resolution setting of the DS18B20. The possible delays
            //are:
            // - 94ms for 9 bit resolution
            // - 188ms for 10 bit resolution
            // - 375ms for 11 bit resolution
            // - 750ms for 12 bit resolution
            //Seeing that we have not read the Configuration register to determine the
            //resolution, assume worst case of 12 bit resolution
            //
            //To decrease this delay:
            // - Reduce resolution of DS18B20 devices
            // - Supply the DS18B20 with their own power. In this case I think we can
            //   move on to the next DS18B20 while this one is converting. After issuing
            //   a "Start Conversion" command to all, poll them to see when done.
            if (tick16TestTmr(tmrDelay + tick16ConvertFromMS(750))) {
                //Reset 1-Wire Net, and wait till done
                if (owReset(pObj) != 0)
                    goto PRINT_ALL_ASYNC_DS18B20_ERROR;
                //Increment to next state, and set SM_PRTDS_WAIT_FOR_TXION flag (wait for Txion)
                sm_prtds = SM_PRTDS_NEXT_DEVICE_SELECT2 | SM_PRTDS_WAIT_FOR_TXION;
                break;
            }
            break;
        case SM_PRTDS_NEXT_DEVICE_SELECT2:
            //Select 1-Wire Net, and wait till done
            if (owSelect(pObj, &deviceAdr[idx-1][0]) != 0)
                goto PRINT_ALL_ASYNC_DS18B20_ERROR;
            //Increment to next state, and set SM_PRTDS_WAIT_FOR_TXION flag (wait for Txion)
            sm_prtds = SM_PRTDS_NEXT_DEVICE_RD_SCRATCH_PAD | SM_PRTDS_WAIT_FOR_TXION;
            break;
        case SM_PRTDS_NEXT_DEVICE_RD_SCRATCH_PAD:
            //Read Scratch Pad (0xBE), and wait till done
            if (owWriteByte(pObj, 0xBE) != 0)
                goto PRINT_ALL_ASYNC_DS18B20_ERROR;
            //Increment to next state, and set SM_PRTDS_WAIT_FOR_TXION flag (wait for Txion)
            sm_prtds = SM_PRTDS_READ_DS18B20_BYTES | SM_PRTDS_WAIT_FOR_TXION;
            iRd = 0;
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nScratchPAD DATA = 0x");
            break;
        case SM_PRTDS_READ_DS18B20_BYTES:
            //Read next byte of DS18B20 scratch memory
            if (owReadByte(pObj) != 0)
                goto PRINT_ALL_ASYNC_DS18B20_ERROR;
            //Increment to next state, and set SM_PRTDS_WAIT_FOR_TXION flag (wait for Txion)
            sm_prtds = SM_PRTDS_READ_DS18B20_BYTES2 | SM_PRTDS_WAIT_FOR_TXION;
            break;
        case SM_PRTDS_READ_DS18B20_BYTES2:
            //Get byte just read
            get[iRd] = owGetByte(pObj);
            DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, get[iRd]);
            DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');
            //Are there more byte to read? Read all 9 bytes of DS18B20 scratch memory
            if(iRd++ < 9) {
                sm_prtds = SM_PRTDS_READ_DS18B20_BYTES;
                break;
            }

            //Done reading whole "Scratch Pad" memory! Print temperature for device!
            temp.byte.LB = get[0];
            temp.byte.HB = get[1];
            fTemp = (temp.Val & 0x07f0)>>4;
            fTemp += 0.0625 * (temp.byte.LB & 0x0f);
            printf("\nTempC= %f degrees C\n", fTemp); // print temp. C

            //Process next DS18B20 device, loop back to SM_PRTDS_NEXT_DEVICE state
            sm_prtds = SM_PRTDS_NEXT_DEVICE;
            break;
        case SM_PRTDS_DONE:
            //Wait 1000ms and print all temperatures again
            if (tick16TestTmr(tmrDelay + tick16ConvertFromMS(1000))) {
                idx = 0;    //Reset to first device index
                sm_prtds = SM_PRTDS_NEXT_DEVICE;
            }
            break;
    }

    //If state = SM_PRTDS_DONE, return 0 (done). Else, not done, return 1;
    return (sm_prtds!=SM_PRTDS_DONE);

    PRINT_ALL_ASYNC_DS18B20_ERROR:
    sm_prtds = SM_PRTDS_DONE;
    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nprintAllAsync_DS18B20 Error!");
    return 0;   //Done
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
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHello");
        }
        //'rst' - Send reset command to DS2482 chip
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "rst") == 0) {
            ds2482_reset(DS2482);
        }
        //'1w-rst' - Send a 1-Wire reset command to DS2482 chip
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "owRst") == 0) {
            //Send "1-Wire Reset" command
            owReset(DS2482);

            //Wait till done, and get status
            if (ds2482_getStatusWait(DS2482) != 0) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nowReset() Error!");
            }
        }
        //Add custom debug message processing
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "...") == 0) {
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nReceived unknown debug message");
            //..... Do something
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif
