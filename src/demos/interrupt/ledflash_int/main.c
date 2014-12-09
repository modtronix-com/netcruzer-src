/**
 * @example ledflash_int/main.c
 *
 * <h2>===== Description =====</h2>
 * This module contains example code for blinking the System LED. Most(currently all) Netcruzer SBC boards have a System LED, and can be used
 * for testing this program. The LED is blinked by using the Timer 1 interrupt. Timer 1 is set to interrupt every 1 second, causing the
 * Timer 1 Interrupt Service Routine (ISR) to be called. The ISR simply toggles the System LED. The line "PR2 = 62500;" sets the Timer 1
 * interrupt period. Changing this value will change the blink rate. For example, changing this line to "PR2 = 6250;" will blink LED every 100ms.
 *
 * <h2>===== Required Hardware =====</h2>
 * This project can be run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>. For prototyping, we recommend
 * combining this board with a <a href="http://netcruzer.com/products-sbc66-prototype/">Prototyping Board</a>, like the PT66ECI for example. This
 * low cost prototyping board makes all the I/O ports of the SBC66 board available via marked labels on the PCB. It also provides a reset and
 * firmware button that simplifies prototyping.
 *
 * <h2>===== Building Project =====</h2>
 * This project is located in the "src/demos/interrupt/ledflash_int" folder of the <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer
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
#define THIS_IS_MAIN_FILE

/////////////////////////////////////////////////
// Include Files
#include "HardwareProfile.h"            //Required for all Netcruzer projects

//Dummy tick_val and tickInit() function
volatile DWORD_VAL tick_val;
void tickInit(void) {}

/**
 * Timer 1 Interrupt Service Routine (ISR). This function is executed each time a Timer 1 interrupt
 * occurs.
 */
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void)
{
    // Toggle the System LED
    PIN_SYSLED = !PIN_SYSLED;

    IFS0bits.T1IF = 0;    //Clear interrupt status bit
}

/**
 * Main program entry point
 */
int main(void)
{
    nzSysInitDefault(); //Default initialization. All ports as inputs. All analog functions disabled


    /////////////////////////////////////////////////
    //Initialize Timer 1
    //For 32MHz Fosc (16 MIPS), timer increments every 16us
    //T1CONbits.TCKPS = 3;    // 1:256 prescale
    //xxxx xxxx xxxx xx0x - Clock source = Fosc/2 = 16MHz
    //xxxx xxxx xx11 xxxx - Input clock prescaler = 256
    //xxxx xxxx x0xx xxxx - Gated time accumulation disabled
    T1CON = 0x0030;
    //PR2 = 0xFFFF;         //Free running timer
    //PR2 = 62500;          //Timer interrupts every 1 second
    //PR2 = 31250;          //Timer interrupts every 500ms
    //PR2 = 6250;             //Timer interrupts every 100ms
    PR2 = 3125;           //Timer interrupts every 50ms
    //PR2 = 1250;           //Timer interrupts every 20ms
    //PR2 = 625;            //Timer interrupts every 10 ms

    TMR1 = 0;               // Clear counter
    IPC0bits.T1IP = 2;      // Interrupt priority 2 (low)
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    T1CONbits.TON = 1;      // Start timer

    //Set System LED port as an output. Setting a DIR_xxx bit to 1 set's corresponding port to input, and 0 as output
    DIR_SYSLED = 0;

    //Loop forever. Our code is interrupt based. The _T1Interrupt() ISR is called in background each time
    //an interrupt occurs
    while (1) {}

    return 0;
}

