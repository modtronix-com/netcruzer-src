/**
 * @example i2c1_lcd2s_txt/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to write "Hello World" to a LCD2S serial LCD display via I2C 1, and flash the system LED.
 * It uses the nz_serI2C.h and nz_serI2C.c files to write to the I2C Port using low level I2C functions.
 * <b>PLEASE NOTE</b>, it is simpler using the nz_lcd2s.h and nz_lcd2s.c functions, this example is just to show how
 * to use the LCD2S by using low level I2C functions!
 *
 * <h2>===== Required Hardware =====</h2>
 * This project can be run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>. For prototyping, we recommend
 * combining this board with a <a href="http://netcruzer.com/products-sbc66-prototype/">Prototyping Board</a>, like the PT66ECI for example. This
 * low cost prototyping board makes all the I/O ports of the SBC66 board available via marked labels on the PCB. It also provides a reset and
 * firmware button that simplifies prototyping.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/i2c/i2c1_lcd2s_txt" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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

////////// Defines /////////////////////////////
#define LCD2S_I2C_ADR       80      // Default LCD I2C address when both switches on back of LCD2S board are set to 0
#define CMD_WRITE_STRING    0x80    // Write parsed String command

////////// Function Prototypes /////////////////////////////
void initializeLcd2s(void);

/**
 * Main program entry point.
 */
int main(void)
{
    WORD tmrFlashLed = 0;   //16-bit, 1ms Timer. Used for flashing System LED
    
    nzSysInitDefault();     //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    initializeLcd2s();      //Initialize LCD2S Serial LCD display, and I2C 1 port it is connected to

    DIR_SYSLED = 0;         //Set System LED port as outputs. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output


    //Write Parsed String to LCD. See "Write Parsed String" command in LCD2S documentation(netcruzer.com/products-opto) for details.
    //The '\f' character causes "clear screen and position cusor at home position".
    i2c1BeginTransmission(LCD2S_I2C_ADR);           //Start transmission to slave at address LCD2S_I2C_ADR
    i2c1Write(CMD_WRITE_STRING);                    //Write Parsed String command
    i2c1WriteString("\fHello\nWorld");              //Write string
    i2c1EndTransmission();                          //End transmission, and start sending message in background
    if (i2c1GetStatusWait(0) != 0) {
        //ERROR! Could be that no I2C slave with given address is on bus (no I2C acknowledge received for transmission)
    }

    while(1)
    {
        nzSysTaskDefault();                     //Main netcruzer task, call in main loop.
        
        i2c1BeginTransmission(LCD2S_I2C_ADR);   //Start transmission to slave at address LCD2S_I2C_ADR
        i2c1Write(CMD_WRITE_STRING);            //Write Parsed String command
        i2c1Write('.');                         //Add a '.' to LCD display
        i2c1EndTransmission();                  //End transmission, and start sending message in background

        /////////////////////////////////////////////////
        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 500ms again
            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED
        }
    }//end while

    return 0;
}//end main

/**
 * Initialization code to initialize I2C 1 and LCD2S display connected to it
 */
void initializeLcd2s(void) {
    delay_ms(300);  //Give LCD2S time to initialize itself

    /////////////////////////////////////////////////
    //Configure LCD using "Configure Device" command (0x95). This command is only available in new V1.4 LCD Displays
    //See LCD2S documentation (http://netcruzer.com/doc?lcd2s) for details on "Configure Device" command
    //
    //Configure LCD using "Configure Device" command (0x95). This command is only available in new V1.4 LCD Displays
    //Byte 1 = 0x95
    //
    //Byte 2 (Display) of command = 0x39
    //xx1x xxxx - Interrupt pin is Open Collector output type
    //xxx1 1xxx - Backlight On, Display On
    //xxxx x001 - Cursor off, block cursor off, Cursor moves forward
    //
    //Byte 3 (Contrast and Brightness) of command = 0x77
    //01xx xxxx - Set Backlight to 100 (range is 0 to 255)
    //xx11 0111 - Set contrast to 220 (0x37 x 4)
    //
    //Byte 4 (keypad and IO) of command = 0x0f
    //0x0f Configures device for 4x4 keypad (if present), OUT1 disabled, GPIO1 to 3 disabled
    //
    //Byte 5 (Keypad and Buzzer) of command = 0x6A
    //01xx xxxx - Keypad Buzzer off
    //xx10 xxxx - Keypad repeat rate = 320ms
    //xxxx 10xx - Keypad Repeat Delay = 1 second
    //xxxx xx10 - Keypad Debounce Time = 64ms
    i2c1WriteMessage(LCD2S_I2C_ADR, (BYTE*)"\x95\x39\x77\x0F\x6a", 5);  //Write configuration data
    i2c1GetStatusWait(0);                                               //Wait till background transmission finished
}
