/**
 * @example i2c1_lcd2s_txt_key/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to write text, and check for pressed keys from a LCD2S serial LCD display via I2C.
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
 * This project is located in the "src/demos/i2c/i2c_lcd2s_hello" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
#include "HardwareProfile.h"
#include "nz_serI2C.h"            //Required for all Netcruzer projects

////////// Defines /////////////////////////////
#define LCD2S_I2C_ADR       80      // Default LCD I2C address when both switches on back of LCD2S board are set to 0
#define CMD_WRITE_STRING    0x80    // Write parsed String command
#define CMD_GET_STATUS      0xD0
#define CMD_GET_KEY         0xD1

////////// Function Prototypes /////////////////////////////
void initializeLcd2s(void);

/**
 * Main program entry point.
 */
int main(void)
{
    //Declare all timers, and initiaze with current timer value (tick16Get...() functions)
    WORD tmrCheckKey = tick16Get();     //Timer for checking if a key was pressed
    WORD tmrFlashLed = tick16Get();     //Timer for flashing system LED

    nzSysInitDefault();     //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    initializeLcd2s();      //Initialize LCD2S Serial LCD display, and I2C 1 port it is connected to

    //Set System LED port, and port 5 (old port name X5) as outputs. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output
    DIR_SYSLED = 0;

    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.
        
        /////////////////////////////////////////////////
        //Check if key was pressed every 100ms
        if (tick16TestTmr(tmrCheckKey)) {
            tick16UpdateTmrMS(tmrCheckKey, 100); //Update timer to expire in 100ms again
            BYTE status, key;

            //Read a registers from slave at address LCD2S_I2C_ADR, and save result in "status" variable
            i2c1ReadSlaveReg(LCD2S_I2C_ADR, CMD_GET_STATUS, &status, 1);
                    
            //Wait till background transmission finished, and get status (0 = OK).
            if (i2c1GetStatusWait(0) == 0) {
                //Can add code here to check free buffer space. The LCD2S has an 80 byte buffer. If sending too much
                //data to it too fast, the buffer can fill before data is processed (like written to LCD for example)
                //lcdFreeBuf = status & 0x7f;    //Get amount of free space in LCD2S receive buffer

                //Has the LCD got keys to be read
                if (status & 0x80) {
                    BYTE buf[50];
                    //Get key from LCD
                    i2c1ReadSlaveReg(LCD2S_I2C_ADR, CMD_GET_KEY, &key, 1);

                    if (i2c1GetStatusWait(0) == 0) {
                        //Print key pressed message on LCD
                        sprintf((char*)buf, "\x80\fKey Pressed\nKey Code = %c", key);
                        i2c1WriteMessage(LCD2S_I2C_ADR, buf, strlen((const char*)buf));
                    }
                }
            }
        }

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
