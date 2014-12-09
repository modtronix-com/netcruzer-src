/**
 * @brief           DB66DEV1 Driver Functions
 * @file            nz_db66dev1.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 **********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2013-09-03, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_DB66DEV1_C

#include "HardwareProfile.h"

#if defined(XBRD_DB66DEV1)

#include "nz_db66dev1.h"

/////////////////////////////////////////////////
// Defines
#if !defined(DB66DEV1_DEBOUNCE_RST_MS)
#define DB66DEV1_DEBOUNCE_RST_MS            30      //Debounce time = 30ms default
#endif
#if !defined(DB66DEV1_DEBOUNCE_REPEAT_MS)
#define DB66DEV1_DEBOUNCE_REPEAT_MS         250     //Initial delay between repeats = 250ms default
#endif
#if !defined(DB66DEV1_DEBOUNCE_REPEAT_TIMES)
#define DB66DEV1_DEBOUNCE_REPEAT_TIMES      2       //2 times = 2 x 250ms = 500ms
#endif
#if !defined(DB66DEV1_DEBOUNCE_REPEAT2_MS)
#define DB66DEV1_DEBOUNCE_REPEAT2_MS        100     //Next delay between repeats = 100ms default
#endif
#if !defined(DB66DEV1_DEBOUNCE_REPEAT2_TIMES)
#define DB66DEV1_DEBOUNCE_REPEAT2_TIMES     5       //5 times = 5 x 100ms = 500ms
#endif
#if !defined(DB66DEV1_DEBOUNCE_REPEAT3_MS)
#define DB66DEV1_DEBOUNCE_REPEAT3_MS        50      //50ms following repeats
#endif
#if !defined(DB66DEV1_DEBOUNCE_REPEAT3_TIMES)
#define DB66DEV1_DEBOUNCE_REPEAT3_TIMES     20      //20 times = 20*50ms = 1 Second
#endif
#if !defined(DB66DEV1_DEBOUNCE_REPEAT4_MS)
#define DB66DEV1_DEBOUNCE_REPEAT4_MS        10      //Next delay between repeats = 10ms default
#endif
#if !defined(DB66DEV1_SERVICE_TIME_MS)
#define DB66DEV1_SERVICE_TIME_MS            10      //Specifies how often the service routine is called, default is 10ms
#endif


/////////////////////////////////////////////////
// Local variables. Lower nibble is current state, upper nibble is latched values
static BYTE buzzOnBtnReload;  //Time in 10 x buzzer service times
static BYTE buzzCntr;         //Buzzer on counter
static BYTE buttonsValue;     //Holds debounced value of buttons
static BYTE buttonsLatch;     //Holds latched and debounced value of buttons - cleared when read by getter function
static BYTE debounceArr[4];   //A register for each button
static BYTE tmr10x;
static BYTE btnRepeated;
static BYTE serviceMask;


/////////////////////////////////////////////////
// Global variables.
BYTE db66dev1Leds0to7;

/**
 * Initializes the DB66DEV1 board
 */
void db66dev1_Init() {
    //Initialize all 8 LEDs of the DB66DEV1 board. All LED ports are configured as outputs.
    DIR_LED1 = OUTPUT_PIN;
    DIR_LED2 = OUTPUT_PIN;
    DIR_LED3 = OUTPUT_PIN;
    DIR_LED4 = OUTPUT_PIN;
    DIR_LED5 = OUTPUT_PIN;
    DIR_LED6 = OUTPUT_PIN;
    DIR_LED7 = OUTPUT_PIN;
    DIR_LED8 = OUTPUT_PIN;

    //turn all LEDs off
    db66dev1_WriteLeds(0);

    //Initialize all 4 Buttons of the DB66DEV1 board. All button ports are configured as inputs,
    // AND> all the Pull-up resistors on all button ports are enabled.
    DIR_BUTTON1 = INPUT_PIN;
    DIR_BUTTON2 = INPUT_PIN;
    DIR_BUTTON3 = INPUT_PIN;
    DIR_BUTTON4 = INPUT_PIN;
    PULLUP_BUTTON1 = 1;
    PULLUP_BUTTON2 = 1;
    PULLUP_BUTTON3 = 1;
    PULLUP_BUTTON4 = 1;

    //Buzzer
    //DIR_BUZZER = OUTPUT_PIN;

    //Initialize all decounce counters
    debounceArr[0] = DB66DEV1_DEBOUNCE_RST_MS;
    debounceArr[1] = DB66DEV1_DEBOUNCE_RST_MS;
    debounceArr[2] = DB66DEV1_DEBOUNCE_RST_MS;
    debounceArr[3] = DB66DEV1_DEBOUNCE_RST_MS;
    buttonsLatch = 0;       //Clear all latched buttons
    buttonsValue = 0;
    btnRepeated = 0;

    //Sound buzzer for 7 x 10 service periods when button pressed. So, if db66dev1_Service is called
    //every 20ms, this will sound buzzer for 4 x 10ms = 80ms
    buzzOnBtnReload = 2;
    buzzCntr = 0;

    //Initialize 10x counter
    tmr10x = 10;
}


/**
 * Sound buzzer for given time. Time is 10ms blocks.
 */
void db66dev1_Buzz(BYTE t) {
    buzzCntr = t;
}


/**
 * Gets the filtered (debounced) button state. The value returned here has already been debounced.
 *
 * @param btn Is the button number. A value from 1 - 4
 *
 * @return The current state of the requested button, 0 if not pressed, 1 if pressed.
 */
BOOL db66dev1_ReadButton(BYTE btn) {
    return (buttonsValue & (0x01 << (btn-1)))==0 ? FALSE : TRUE;
}


/**
 * Gets the latched state of the buttons. The value returned here has already been debounced.
 * The button latch is cleared after this call, so it is very important to check all buttons.
 * @return The buttons state is contained in bits 0-3
 */
BYTE db66dev1_ReadLatchedButtons() {
    BYTE ret;
    ret = buttonsLatch;
    buttonsLatch = 0;   //Clear all latched buttons
    return ret;
}


/**
 * Set all 8 LEDs of the DB66DEV1 board to the given values. Each bit in the given byte represents a LED.
 *
 * @param val Gives the values of the 8 LEDs. Bit 0 (LSB) represents LED1. Bit 7 (MSB) represents LED8.<br>
 *
 * For example:
 * - To clear all LEDs, call db66dev1_WriteLeds(0);
 * - To set first LED (binary format), call  db66dev1_WriteLeds(0b00000001);
 * - To set first LED (hex format), call  db66dev1_WriteLeds(0x01);
 * - To set first, second and fifth LED (binary format), call  db66dev1_WriteLeds(0b00010011);
 * - To set first, second and fifth LED (hex format), call  db66dev1_WriteLeds(0x13);
 */
void db66dev1_WriteLeds(BYTE leds) {
    LAT_LED1 = (leds & 0x01) == 0 ? 0 : 1;
    LAT_LED2 = (leds & 0x02) == 0 ? 0 : 1;
    LAT_LED3 = (leds & 0x04) == 0 ? 0 : 1;
    LAT_LED4 = (leds & 0x08) == 0 ? 0 : 1;
    LAT_LED5 = (leds & 0x10) == 0 ? 0 : 1;
    LAT_LED6 = (leds & 0x20) == 0 ? 0 : 1;
    LAT_LED7 = (leds & 0x40) == 0 ? 0 : 1;
    LAT_LED8 = (leds & 0x80) == 0 ? 0 : 1;
}

/**
 * Gets the current LED values. Each bit in the given byte represents a LED.
 * @return The current LED setting
 */
BYTE db66dev1_ReadLeds(void) {
    BYTE led = 0;
    if (LAT_LED1==1)
        led |= 0x01;
    if (LAT_LED2==1)
        led |= 0x02;
    if (LAT_LED3==1)
        led |= 0x04;
    if (LAT_LED4==1)
        led |= 0x08;
    if (LAT_LED5==1)
        led |= 0x10;
    if (LAT_LED6==1)
        led |= 0x20;
    if (LAT_LED7==1)
        led |= 0x40;
    if (LAT_LED8==1)
        led |= 0x80;
    return led;
}


/**
 * <b>IMPORTANT</b> - Must be called every 10ms!
 * 
 * Does processing of given button. Includes latching and debouncing
 *
 * @param btn Is the button number. A value from 0 - 4
 *
 * @param state 1 = pressed, 0 = not pressed
 */
void btnService(BYTE btn, BYTE state) {
    //Current button is pressed
    if (state) {
        debounceArr[btn]--;     //Decrement debounce count
    }
    //Current button is NOT pressed
    else {
        debounceArr[btn] = (DB66DEV1_DEBOUNCE_RST_MS/DB66DEV1_SERVICE_TIME_MS); //Reset debounce count
        buttonsValue &= (~serviceMask);                                         //Clear filtered (debounced) button bit
    }

    //Check if button has been held for debounce time yet
    if (debounceArr[btn] == 0) {
        if (btnRepeated<255) btnRepeated++;

        buttonsLatch |= serviceMask;   //Set latched button bit
        buttonsValue |= serviceMask;   //Set filtered (debounced) button bit

        //Set time until next button is detected.
        if (btnRepeated < DB66DEV1_DEBOUNCE_REPEAT_TIMES)
            debounceArr[btn] = (DB66DEV1_DEBOUNCE_REPEAT_MS/DB66DEV1_SERVICE_TIME_MS);
        else if (btnRepeated < DB66DEV1_DEBOUNCE_REPEAT2_TIMES)
            debounceArr[btn] = (DB66DEV1_DEBOUNCE_REPEAT2_MS/DB66DEV1_SERVICE_TIME_MS);
        else if (btnRepeated < DB66DEV1_DEBOUNCE_REPEAT3_TIMES)
            debounceArr[btn] = (DB66DEV1_DEBOUNCE_REPEAT3_MS/DB66DEV1_SERVICE_TIME_MS);
        else
            debounceArr[btn] = (DB66DEV1_DEBOUNCE_REPEAT4_MS/DB66DEV1_SERVICE_TIME_MS);

        //If any button has been pressed, sound buzzer
        buzzCntr = buzzOnBtnReload;
    }

    serviceMask = serviceMask << 1;
}

/**
 * Service function. Must be called every 1ms
 */
void db66dev1_Service() {

    /////////////////////////////////////////////////
    //Service buttons
    serviceMask = 0x01;   //Create mask for current button
    btnService(0, !PIN_BUTTON1);
    btnService(1, !PIN_BUTTON2);
    btnService(2, !PIN_BUTTON3);
    btnService(3, !PIN_BUTTON4);

    //If no buttons currently pressed, reset repeated
    if (PIN_BUTTON1 && PIN_BUTTON2 && PIN_BUTTON3 && PIN_BUTTON4) {
        btnRepeated = 0;                    //Clear repeated flag
    }

    /* Code for sounding buzzer each time a button is pressed. Currently not used
    if (--tmr10x == 0)
    {
        tmr10x = 10;

        //Check if buzzer must be sounded
        if (buzzCntr > 0)
        {
            LAT_BUZZ = 1;       //Buzzer on
            buzzCntr--;
        }
        else
            LAT_BUZZ = 0;       //Buzzer off
    }
    */
}

#endif  //#if defined(XBRD_DB66DEV1)