/**
 * @example rotary_encoder/main.c
 *
 * <h2>===== Description =====</h2>
 * This project shows how to use an incremental rotary encoder.
 * It prints out the last values read from the encoder.
 * All it does is blink the System LED every second.
 *
 * Additionally this demo also implements debugging via the USB port and the
 * <a href="http://www.netcruzer.com/usbterminal/">Netcruzer USB Terminal</a> App.
 * During the program various debug messages are written, which will be displayed
 * on the "Netcruzer USB Terminal" (running on a PC connected to the target
 * board's USB port).
 * This demo also monitors the USB port for Debug messages send to us.
 * <br>If "hi" is received, it replies with "Hello"
 * <br>Use the "Netcruzer USB Terminal" App to send and receive USB Debug messages.
 * It can be downloaded here: http://www.netcruzer.com/usbterminal/
 *
 * <h2>===== Required Hardware =====</h2>
 * The project requires a <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer board</a>
 * with an USB Port (not USB Host).
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/template/rotary_encoder" folder of the
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
 * 2014-01-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

// Includes /////////////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects
#include "nz_encIncRot.h"       //Rotary Encoder driver
#include "nz_interrupt.h"
#include <ports.h>

//Add debugging, and define debugging level. Is a DEBUG_LEVEL_xx define. Set to DEBUG_LEVEL_OFF to disable.
#define MY_DEBUG_LEVEL   DEBUG_LEVEL_INFO
#include "nz_debug.h"

// Variables ////////////////////////////////////
static WORD tmrFlashLed = 0;        //Timer for flashing system LED
ENCROT_INFO objEncRot;              //Rotary Encoder object
BYTE        bufGet;                 //Pointer to last value read from encoder buffer


/**
 * Interrupt on change ISR, called if any of the configured ports change value
 */
void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void)
{
    intOnChangeClearIF();   //Clear "interrupt on change" IF (interrupt flag)
    rotinc_task(&objEncRot);
}


/**
* Main application entry point.
*/
int main(void) {
    WORD encVal;

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    debugPutString("\nThis is a test debug message from rotary_encoder.c demo");

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs

    bufGet = 0;

    //Initialize rotary encoder for using port 8 (old port name X8) as channel A, and port 7 (old port name X7) as channel B
    rotinc_init(&objEncRot, 8, 7);

    //Configure the interrupt on change for encoder channel A (Port 8 - old port name X8) only!
    intOnChangeConfig(INT_ENABLE|INT_PRI_4);	//Enable Interrupt on chagne at Int Priority level 4
    intOnChangeClearIF();                       //Clear "interrupt on change" IF (interrupt flag)
    intOnChangeEnablePort(8);    	            //Enable "interrupt on change" for Channel A = Port 8 (old port name X8)

    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //Print out last values read by encoder. The times are in multiples of ENC_INC_ROT_RESOLUTION.
        //The default ENC_INC_ROT_RESOLUTION is 16us, so a value of 10 will be 10x16=160us
        while (bufGet != objEncRot.put) {
            //Get next encoder value, and update the last value we read from the buffer
            encVal = objEncRot.buf[bufGet];
            bufGet++;
            if (bufGet >= ENC_INC_ROT_BUFSIZE)
                bufGet = 0;

            //Encoder is being turned forward (Clock Wise)
            if (encVal & 0x8000) {
                debugPutString("\nF = ");
            }
            //Encoder is being turned backwards (Counter Clock Wise)
            else {
                debugPutString("\nB = ");
            }
            debugPutWord(encVal & 0x7fff);  //Print lower 15-bits. Bit 16 contains direction!
            //printf("\nT = %d", objEncRot.buf[bufGet]);
        }

        //Flash system LED every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 10ms again
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
            debugPutString("\nHello Mate");
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