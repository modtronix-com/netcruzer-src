/**
 * @example lcd2s_txt_key/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to write text, and read keys from a LCD2S serial LCD display using the lcd2c.h and nz_lcd2s.c driver.
 * To use this demo, a LCD2S serial LCD display with a keypad connected to it is required.
 * It also flashes the system LED. To add LCD2S support to a project, the following must be done:
 * - Add nz_lcd2s.c to the project, this is the main LCD2S driver file.
 * - The following additional files are required by lcd2s.c, and must be added to the project:
 *   nz_circularBufferPwr2.c, nz_helpers.c, nz_netcruzer.c and nz_serI2C.c
 * - Add "HAS_SERPORT_I2C1" to projdefs.h file.
 * - In code, initialize LCD2S. Ensure to delay 300ms from power up. For example:
 *      delay_ms(300);
 *      lcdInit();
 * - In projdefs.h, do any LCD2S, I2C or other configuration required. This is done by copying the
 *   configuration section from the *.h files to projdefs.h. Nothing required, defaults will work!
 * - All done! Can now use lcd2c.h driver functions! For example, to write to LCD2S:
 *      lcdDisplayString(0, "\fHello\nWorld, Again!");

 *
 * <h2>===== Required Hardware =====</h2>
 * This project can be run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>. For prototyping, we recommend
 * combining this board with a <a href="http://netcruzer.com/products-sbc66-prototype/">Prototyping Board</a>, like the PT66ECI for example. This
 * low cost prototyping board makes all the I/O ports of the SBC66 board available via marked labels on the PCB. It also provides a reset and
 * firmware button that simplifies prototyping.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/lcd/lcd2s_txt_key" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
#include <stdio.h>                      //Required for sprintf()

#include "HardwareProfile.h"            //Required for all Netcruzer projects
#include "nz_serI2C.h"
#include "nz_lcd2s.h"


/**
 * Main program entry point.
 */
int main(void)
{
    char buf[40];
    WORD tmrFlashLed = 0;  //16-bit, 1ms Timer. Used for flashing System LED

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms.
    nzSysInitDefault();

    DIR_SYSLED = 0; //Set System LED port as outputs. Setting DIR_xxx to 1 set's port to input, and 0 as output

    //Initialize LCD2S display. To customize initialization, add defines from lcs2s.h (in header) to projdefs.h file.
    //or details, see @ref lcd2s_conf
    delay_ms(300);          //Give LCD2S time to initialize itself
    lcdInit();

    //Write Parsed String to LCD. See "Write Parsed String" command in LCD2S documentation (http://netcruzer.com/doc?lcd2s)
    //or details. The '\f' character causes "clear screen and position cusor at home position".
    lcdDisplayString(0, "\fKeypad Demo\nWaiting for key!");

    while(1)
    {
        nzSysTaskDefault();                     //Main netcruzer task, call in main loop.
        
        //When a keyboard is connected to the LCD2S, lcd2sTask() must be called in main system loop.
        lcd2sTask();

        //Check if there are any keys in buffer (of keypad 0), and if so, get it and display key code on LCD.
        if (kpadHasKeys(0)) {
            BYTE keyCode = kpadGetKey(0);
            sprintf(buf, "\x80\fKey Pressed\nKey Code = %c", keyCode);
            lcdDisplayString(0, buf);
        }

        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
        }
    }//end while

    return 0;
}//end main
