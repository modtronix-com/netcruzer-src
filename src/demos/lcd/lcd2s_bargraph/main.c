/**
 * @example lcd2s_bargraph/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to write some bar graphs to a LCD2S serial LCD display using the lcd2c.h and nz_lcd2s.c driver.
 * It writes "Example Bargraph" to first line of LCD display. Columns 1 to 8 of second line is initialized with bar
 * graphs 1 to 8 pixels high. Column 9 contains a varying height bar graph, that changes height every 100ms.
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
 * This project is located in the "src/demos/lcd/lcd2s_bargraph" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
#include "nz_serI2C.h"
#include "nz_lcd2s.h"


////////// Function Prototypes /////////////////////////////
void drawWave(BYTE count);

/**
 * Main program entry point.
 */
int main(void)
{
    BYTE count = 0;
    BYTE bgHeight;
    WORD tmrFlashLed = 0;  //16-bit, 1ms Timer. Used for flashing System LED

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms.
    nzSysInitDefault();

    DIR_SYSLED = 0; //Set System LED port as outputs. Setting DIR_xxx to 1 set's port to input, and 0 as output

    //Initialize LCD2S display. To customize initialization, add defines from lcs2s.h (in header) to projdefs.h file.
    //For details, see @ref lcd2s_conf
    delay_ms(300);          //Give LCD2S time to initialize itself
    lcdInit();

    //The following code does the following:
    //- Clear display, and write: "Bargraph Example" on first line
    //- Writes 8 bargraphs with heights from 1 to 8 on second line (column 1 to 8)
    lcdDisplayString(0, "\fBargraph Example");
    lcdLoadCustomCharSet(0, 0); //Load vertical bargraph character set
    lcdDrawVertBargraph(0, 2, 1, 1);    //Row 2, Columns 1, bargraph height = 1
    lcdDrawVertBargraph(0, 2, 2, 2);    //Row 2, Columns 2, bargraph height = 2
    lcdDrawVertBargraph(0, 2, 3, 3);    //Row 2, Columns 3, bargraph height = 3
    lcdDrawVertBargraph(0, 2, 4, 4);    //Row 2, Columns 4, bargraph height = 4
    lcdDrawVertBargraph(0, 2, 5, 5);    //Row 2, Columns 5, bargraph height = 5
    lcdDrawVertBargraph(0, 2, 6, 6);    //Row 2, Columns 6, bargraph height = 6
    lcdDrawVertBargraph(0, 2, 7, 7);    //Row 2, Columns 7, bargraph height = 7
    lcdDrawVertBargraph(0, 2, 8, 8);    //Row 2, Columns 8, bargraph height = 8

    while(1)
    {
        nzSysTaskDefault();                 //Main netcruzer task, call in main loop.

        //Enter every 200ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 200); //Update timer to expire in 200ms again
            count++;

            //Flash LED every 500ms (every time count/5 has a remainder of 0)
            if ((count%5) == 0) {
                LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
            }
            
            //Use bits 0-3, is a value from 0 to 15
            bgHeight = count & 0b00001111;

            //Cause bgHeight's value repeditively count up from 0 to 8, and down to 0 again. This causes
            //the height of the bar graph to increase from 0-8, decrease to 0, increase to 8, decrease to 0, ......
            if (bgHeight > 8)
                bgHeight = 16 - bgHeight;

            //Enable one of the followig two lines:
            // - The first line simply creates a bar graph moving up and down over time, using bgHeigh for height.
            // - The second line creates a nice looking wave effect.
            //lcdDrawVertBargraph(0, 2, 10, bgHeight);
            drawWave(count);
        }
    }//end while

    return 0;
}//end main

/**
 * Draw bar graphs in row 2, column 10 to 17, each a different height. Creates a wave effect.
 */
void drawWave(BYTE count) {
    BYTE i;
    BYTE bgHeight;

    for (i=0; i<8; i++) {
        //Use bits 0-3, is a value from 0 to 15
        bgHeight = (count+i) & 0b00001111;

        //Cause bgHeight's value repeditively count up from 0 to 8, and down to 0 again. This causes
        //the height of the bar graph to increase from 0-8, decrease to 0, increase to 8, decrease to 0, ......
        if (bgHeight > 8)
            bgHeight = 16 - bgHeight;

        //Row 2, Columns 10+i (10 to 17)
        lcdDrawVertBargraph(0, 2, 10+i, bgHeight);
    }
}
