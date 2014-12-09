/**
 * @example uart_xc16_demo1/main.c
 *
 * <h2>===== Description =====</h2>
 * Project showing how to use the UARTs of a SBC66 board. This project does NOT use the
 * netcruzer nz_serUart.h and nz_serUart.c UART drivers. It uses the XC16 functions.
 * See the XC16 "PIC24F MCU Peripheral Library" (included with XC16 compiler) for details.
 *
 * Every 500ms the System LED is blinked, and a message sent on UART2.
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
 * The project requires a SBC66 Netcruzer board (http://netcruzer.com/products-sbc66-main) with
 * an USB Port (not USB Host). To access the SBC66 board's port pins, a prototype board like the
 * PT02TC, PT66EI or PT66ECI can be used. For available boards, see http://netcruzer.com/products-sbc66-prototype.
 * Or a development board, like the DB66DEV1 (http://netcruzer.com/db66dev1.html) can be used.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/template/uart_xc16_demo1" folder of the
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
 * 2014-05-03, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

// Includes /////////////////////////////////////
#include "HardwareProfile.h"    //Required for all Netcruzer projects
#define USE_AND_OR              //Required by XC16 to enable AND_OR mask setting
#include "uart.h"


//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN     DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"

// Variables ////////////////////////////////////
WORD tmrFlashLed = 0;        //Timer for flashing system LED

//Buffer for transmit data, with get (used in interrupt) and put (used in code) pointer.
BYTE Txdata[256];   //Ensure size is 256 bytes, meaning put and get pointer will automatically increment from 255 to 0!
BYTE putTxdata = 0;
BYTE getTxdata = 0;

//Buffer for receive data, with get (used in code) and put (used in interrupt) pointer.
BYTE Rxdata[256];   //Ensure size is 256 bytes, meaning put and get pointer will automatically increment from 255 to 0!
BYTE putRxdata = 0;
BYTE getRxdata = 0;

// Function Prototypes //////////////////////////
void Test_UART2_Interrupt(void);
void Test_UART2_Polling(void);

//TX interrupt ISR
void __attribute__ ((interrupt,no_auto_psv)) _U2TXInterrupt(void)
{
    //debugPutString("\nT");
    U2TX_Clear_Intr_Status_Bit;

    //If any byte available in transmit buffer, sent it
    if(putTxdata != getTxdata) {
        while(BusyUART2()); //Wait for USART to become available
        WriteUART2((unsigned int)Txdata[getTxdata++]);
    }
}

//RX interrupt ISR
void __attribute__ ((interrupt,no_auto_psv)) _U2RXInterrupt(void)
{
    //debugPutString("\nR");
    U2RX_Clear_Intr_Status_Bit;

    //If Parity, Framing or Overrun error occured, clear error flag
    if (U2STAbits.PERR || U2STAbits.FERR || U2STAbits.OERR) {
        debugPutString("\nRxErr");
        U2STAbits.PERR = U2STAbits.FERR = U2STAbits.OERR = 0;
    }

    //Save any received bytes to receive buffer
    while (DataRdyUART2())
        Rxdata[putRxdata++] = ReadUART2();
}


/**
* Main application entry point.
*/
int main(void) {

    //Default initialization. All ports inputs. All analog features disabled. Tick 1ms
    nzSysInitDefault();

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nThis is a test debug message from uart_xc16_demo1 demo");

    DIR_SYSLED = OUTPUT_PIN;    //Set System LED port as outputs

    nzDelayMs(300);

    //Initialze UART2
    iPPSOutput(PPS_OUT_36, OUT_FN_PPS_U2TX);        //Port 36 - (old port name Y6) is TX
    iPPSOutput(PPS_OUT_38, OUT_FN_PPS_U2RTS);       //Port 38 (old port name Y8) is RTS
    iPPSInput(IN_FN_PPS_U2RX, PPS_IN_37);           //Port 37 (old port name Y7) is RX
    iPPSInput(IN_FN_PPS_U2CTS, PPS_IN_39);          //Port 39 (old port name Y9) is CTS
    CloseUART2();

    //Enable UART intruupts
    ConfigIntUART2(UART_RX_INT_EN | UART_RX_INT_PR6 | UART_TX_INT_EN | UART_TX_INT_PR6);

    //UART initialized, 9600 baudrate (103=9K6 at 16MHz clock), 8bit, no parity, 1 stop bit
    OpenUART2(UART_EN, UART_TX_ENABLE, 103);

    //Main loop
    while(1)
    {
        nzSysTaskDefault();     //Main netcruzer task, call in main loop.

        //Print any data received in UART2 receive interrupt (ISR)
        while (putRxdata != getRxdata) {
            debugPutString("\nReceived 0x");
            debugPutHexByte(Rxdata[getRxdata++]);
        }

        //Enter every 500ms
        if (tick16TestTmr(tmrFlashLed)) {
            tick16UpdateTmrMS(tmrFlashLed, 500); //Update timer to expire in 10ms again

            LAT_SYSLED = !LAT_SYSLED;       //Toggle System LED

            if (LAT_SYSLED)
                Test_UART2_Interrupt();
            else
                Test_UART2_Polling();
        }
    }//end while
}

//Send message on UART using polling
void Test_UART2_Polling(void) {
    char message[] = "Hi";

    DisableIntU2TX;         //Disable TX interrupt
    DisableIntU2RX;         //Disable RX interrupt

    //Transmit data
    while(BusyUART2());     //Wail till available
    putsUART2((unsigned int *)message);
    while(BusyUART2());     //Wail till all bytes sent

    //Read all data from UART2 receive buffer
    while(DataRdyUART2()) {
        debugPutString("\nReceived 0x");
        debugPutHexByte(ReadUART2());
    }

    //Clear TX and RX interrupt flags that would have been set above
    U2TX_Clear_Intr_Status_Bit;
    U2RX_Clear_Intr_Status_Bit;
}

//Send message on UART using interrupt method
void Test_UART2_Interrupt(void) {
    char message[] = "Hello";
    BYTE i;

    EnableIntU2TX;          //Enable TX interrupt
    EnableIntU2RX;          //Enable RX interrupt

    //Write "Hello" to Txdata buffer
    for (i=0; i<5; i++)
        Txdata[putTxdata++] = message[i];

    //Set Transmit interrupt flag to start Transmit ISR, and send message
    IFS1bits.U2TXIF = 1;
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
        //Add custom debug message processing
        else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "...") == 0) {
            //..... Do something
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
}
#endif