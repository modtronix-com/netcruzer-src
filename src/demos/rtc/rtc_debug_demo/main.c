/**
 * @example rtc_debug_demo/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use some of the RTC function made available via the nz_rtc.h
 * and nz_rtc.c drivers. It is meant to be used together with the
 * <a href="http://www.netcruzer.com/usbterminal/">Netcruzer USB Terminal</a> App.
 * This app is used to send debug commands to, an receive debug messages from the target
 * SBC66 board.
 *
 * Using this app, various debug commands can be sent to the SBC66 board. These commands
 * are processed in the debugService() function. For example:
 * - Sending the "st" string will set the time of the RTC to 11:40:50.
 * - Sending the "t" string will request the current time and date, which will be
 *   displayed in the "Netcruzer USB Terminal" app.
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with a RTC and USB Port (not USB Host), like the <a href="http://netcruzer.com/sbc66ec.html">SBC66EC</a>
 * for example.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/template/rtc_debug_demo" folder of the
 * <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer Download</a>. To compile for
 * Netcruzer Board, open this project in MPLAB X, and select the "Project Configuration" for desired board. For
 * example "SBC66EC_R2" for the <a href="http://netcruzer.com/sbc66ec.html">SBC66EC</a> Revision 2 board.
 * For details <a target="_blank" href="http://netcruzer.com/nz/doc/building_projects/">click here</a>
 *
 * <h2>===== Programming Board =====</h2>
 * After compiling (build), the board can be programmed via the <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb">
 * USB Bootloader</a> or a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_prog">PIC Programmer</a>.
 * USB Programming is simplified when using the SBC board together with a
 * <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb_pt66">Prototype Board</a>.
 *
 * <h2>===== File History =====</h2>
 * 2014-01-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

// Includes /////////////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects

//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN     DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"
#include "nz_rtc.h"

// Variables ////////////////////////////////////
static WORD tmrFlashLed = 0;        //Timer for flashing system LED


/**
* Main application entry point.
*/
int main(void) {

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nThis is a test debug message from rtc_debug_demo demo");

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs

    rtcUnlock();                //Enable modifying RTC time and date registers

    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

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
        //'l' - Lock
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "l") == 0) {
            debugPutString("\nRTC Lock");
            rtcLock();          //Lock RTC, disabling time and date modification functions
        }
        //'ul' - Unlock
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "ul") == 0) {
            debugPutString("\nRTC Unlock");
            rtcUnlock();          //Unlock RTC, enabling time and date modification functions
        }
        //'sec' - Reply with RTC Seconds
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "sec") == 0) {
            debugPutString("\nSec = ");
            debugPutByte(rtcGetSec());
        }
        //'st' - Set time to fixed value
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "st") == 0) {
            RTC_TIME tm;
            tm.hour = 11;
            tm.min = 40;
            tm.sec = 50;
            rtcSetTime(&tm);
            debugPutString("\nSet time to 11:40:50");
        }
        //'std' - Set time and date to fixed value
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "std") == 0) {
            RTC_TIME_AND_DATE td = {8,15,30,25,12,10,0};    //8:15;30 25 Dec 2010
            rtcSetTimeAndDate(&td);
            debugPutString("\nSet time to 8:15:30, and date to 25 Dec 2010");
        }
        //'ram' - Test user ram
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "ram") == 0) {
            BYTE ram1, ram2;
            //Set RTC User RAM 1 and 2 to 0x56 and 0xAB
            rtcSetRAM1(0x56);
            rtcSetRAM2(0xAB);

            //Read RTC User RAM 1 and 2 to 0x56 and 0xAB
            ram1 = rtcGetRAM1();
            ram2 = rtcGetRAM2();

            //Confirm values are correct.
            if ((ram1 == 0x56) && (ram2 == 0xAB))
                debugPutString("\nRAM OK!");
            else
                debugPutString("\nRAM Error!");
        }
        //'t' - Print current time and date
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t") == 0) {
            RTC_TIME_AND_DATE td;
            rtcGetTimeAndDate(&td, 0);  //Get time and date in standard binary format
            printf("\nTime: %02d:%02d:%02d, Date: %02d-%02d-%d", td.hour, td.min, td.sec, td.day, td.month, td.year+2000);
        }
        //'tt' - Print current time
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "tt") == 0) {
            RTC_TIME tm;
            rtcGetTime(&tm, 0); //Get time in standard binary format
            printf("\nTime: %02d:%02d:%02d", tm.hour, tm.min, tm.sec);
        }
        //'tbcd' - Print current time
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "tbcd") == 0) {
            RTC_TIME tm;
            rtcGetTime(&tm, 1); //Get time in BCD format
            debugPutString("\nTime: ");
            debugPutChar('0' + (tm.hour>>4));   //Upper nibble of hour - is in BCD format
            debugPutChar('0' + (tm.hour&0x0f)); //Lower nibble of hour - is in BCD format
            debugPutChar(':');
            debugPutChar('0' + (tm.min>>4));    //Upper nibble of minute - is in BCD format
            debugPutChar('0' + (tm.min&0x0f));  //Lower nibble of minute - is in BCD format
            debugPutChar(':');
            debugPutChar('0' + (tm.sec>>4));    //Upper nibble of second - is in BCD format
            debugPutChar('0' + (tm.sec&0x0f));  //Lower nibble of second - is in BCD format
        }
        //'td' - Print current date
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "td") == 0) {
            RTC_DATE dt;
            rtcGetDate(&dt, 0); //Get date in standard binary format
            printf("\nDate: %02d-%02d-%d", dt.day, dt.month, dt.year+2000);
        }
        //Add custom debug message processing
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "...") == 0) {
            //..... Do something
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif