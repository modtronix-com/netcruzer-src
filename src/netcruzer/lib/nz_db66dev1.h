/**
 * @brief           Contains hardware specific defines for DB66DEV1 Revision 1 board.
 * @file            nz_db66dev1.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section db66dev1_desc Description
 *****************************************
 * This module includes hardware specific defines for DB66DEV1 Revision 1 board.
 *
 * @subsection nz_db66dev1_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // ---------- nz_db66dev1 Configuration (from nz_db66dev1.h) -----------
 // *********************************************************************
 //All defines below that define a millisecond time value should be multiples of
 //the DB66DEV1_SERVICE_TIME_MS define (default 10ms)!
 #define DB66DEV1_DEBOUNCE_RST_MS           30      //Debounce time = 30ms default
 #define DB66DEV1_DEBOUNCE_REPEAT_MS        250     //Initial delay between repeats = 250ms default
 #define DB66DEV1_DEBOUNCE_REPEAT_TIMES     2       //2 times = 2*250ms = 500ms default
 #define DB66DEV1_DEBOUNCE_REPEAT2_MS       100     //Next delay between repeats = 100ms default
 #define DB66DEV1_DEBOUNCE_REPEAT2_TIMES    5       //5 times = 5*100ms = 500ms default
 #define DB66DEV1_DEBOUNCE_REPEAT3_MS       50      //Next delay between repeats = 50ms default
 #define DB66DEV1_DEBOUNCE_REPEAT3_TIMES    20      //20 times = 20*50ms = 1 Second
 #define DB66DEV1_DEBOUNCE_REPEAT4_MS       10      //Next delay between repeats = 10ms default

 #define DB66DEV1_SERVICE_TIME_MS           10      //Specifies how often the service routine is called, default is 10ms
 
 @endcode
 *
 * @subsection nz_db66dev1_usage Usage
 *****************************************
 * To use the DB66DEV1 in a project, the following must be done:
 * - Copy the @ref nz_db66dev1_conf "Configuration" section above (from nz_db66dev1.h)
 *   to the projdefs.h file.
 * - In this "Configuration" section, change any default values if required (if default
 *   values should be used, define is not required in projdefs.h).
 * - Add nz_db66dev1.c to the MPLAB project.
 * - Include nz_db66dev1.h in the c file it is used in.
 * - In HardwareProfile.h, add a define for this board:
 @code
//Define any expansion boards, is a XBRD_xxx define (see nz_board.h for supported boards).
#define XBRD_DB66DEV1_R1    //This project uses the DB66DEV1 expansion daughter board
 @endcode
 *
 **********************************************************************
 * @section db66dev1_lic Software License Agreement
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
 * 2012-08-06, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef DB66DEV1_H
#define DB66DEV1_H

#if defined(XBRD_DB66DEV1)

/////////////////////////////////////////////////
// Buttons
#define PIN_BUTTON1     PIN_30		//Old port name = Y0
#define PIN_BUTTON2     PIN_31		//Old port name = Y1
#define PIN_BUTTON3     PIN_32		//Old port name = Y2
#define PIN_BUTTON4     PIN_33		//Old port name = Y3

#define PULLUP_BUTTON1  PULLUP_30	//Old port name = Y0
#define PULLUP_BUTTON2  PULLUP_31	//Old port name = Y1
#define PULLUP_BUTTON3  PULLUP_32	//Old port name = Y2
#define PULLUP_BUTTON4  PULLUP_33	//Old port name = Y3

#define DIR_BUTTON1     DIR_30		//Old port name = Y0
#define DIR_BUTTON2     DIR_31		//Old port name = Y1
#define DIR_BUTTON3     DIR_32		//Old port name = Y2	
#define DIR_BUTTON4     DIR_33		//Old port name = Y3


/////////////////////////////////////////////////
// Buttons
#define DIR_BUZZER      DIR_30		//Old port name = Y0


/////////////////////////////////////////////////
// LEDs
#define DIR_LED1 DIR_06				//Port 6 - old port name = X6
#define DIR_LED2 DIR_07				//Port 7 - old port name = X7
#define DIR_LED3 DIR_08				//Port 8 - old port name = X8
#define DIR_LED4 DIR_09				//Port 9 - old port name = X9
#define DIR_LED5 DIR_26				//Port 26 - old port name = T4
#define DIR_LED6 DIR_27				//Port 27 - old port name = T5
#define DIR_LED7 DIR_28				//Port 28 - old port name = T6
#define DIR_LED8 DIR_29				//Port 29 - old port name = T7

#define LAT_LED1 LAT_06				//Port 6 - old port name = X6
#define LAT_LED2 LAT_07				//Port 7 - old port name = X7
#define LAT_LED3 LAT_08				//Port 8 - old port name = X8
#define LAT_LED4 LAT_09				//Port 9 - old port name = X9
#define LAT_LED5 LAT_26				//Port 26 - old port name = T4
#define LAT_LED6 LAT_27				//Port 27 - old port name = T5
#define LAT_LED7 LAT_28				//Port 28 - old port name = T6
#define LAT_LED8 LAT_29				//Port 29 - old port name = T7

/**
 * @param val Value to set led 1 to. 0=off, 1=on
 * Set LED 1 to the given value
 */
#define db66dev1_WriteLed1(val) (LAT_LED1 = (val))

/**
 * @param val Value to set led 1 to. 0=off, 1=on
 * Set LED 2 to the given value
 */
#define db66dev1_WriteLed2(val) (LAT_LED2 = (val))

/**
 * @param val Value to set led 1 to. 0=off, 1=on
 * Set LED 3 to the given value
 */
#define db66dev1_WriteLed3(val) (LAT_LED3 = (val))

/**
 * @param val Value to set led 1 to. 0=off, 1=on
 * Set LED 4 to the given value
 */
#define db66dev1_WriteLed4(val) (LAT_LED4 = (val))

/**
 * @param val Value to set led 1 to. 0=off, 1=on
 * Set LED 5 to the given value
 */
#define db66dev1_WriteLed5(val) (LAT_LED5 = (val))

/**
 * @param val Value to set led 1 to. 0=off, 1=on
 * Set LED 6 to the given value
 */
#define db66dev1_WriteLed6(val) (LAT_LED6 = (val))

/**
 * @param val Value to set led 1 to. 0=off, 1=on
 * Set LED 7 to the given value
 */
#define db66dev1_WriteLed7(val) (LAT_LED7 = (val))

/**
 * @param val Value to set led 1 to. 0=off, 1=on
 * Set LED 8 to the given value
 */
#define db66dev1_WriteLed8(val) (LAT_LED8 = (val))

/**
 * Initializes the DB66DEV1 board
 */
void db66dev1_Init();


/**
 * Sound buzzer for given time. Time is 10ms blocks.
 */
void db66dev1_Buzz(BYTE t);


/**
 * Gets the filtered (debounced) button state. The value returned here has already been debounced.
 *
 * @param btn Is the button number. A value from 1 - 4
 *
 * @return The current state of the requested button, 0 if not pressed, 1 if pressed.
 */
BOOL db66dev1_ReadButton(BYTE btn);


/**
 * Gets the latched state of the buttons. The value returned here has already been debounced.
 * The button latch is cleared after this call, so it is very important to check all buttons.
 * @return The buttons state is contained in bits 0-3
 */
BYTE db66dev1_ReadLatchedButtons();


/**
 * Set all 8 LEDs of the DB66DEV1 board to the given values. Each bit in the given byte represents a LED.
 *
 * @param leds Gives the values of the 8 LEDs. Bit 0 (LSB) represents LED1. Bit 7 (MSB) represents LED8.<br>
 *
 * For example:
 * - To clear all LEDs, call db66dev1_WriteLeds(0);
 * - To set first LED (binary format), call  db66dev1_WriteLeds(0b00000001);
 * - To set first LED (hex format), call  db66dev1_WriteLeds(0x01);
 * - To set first, second and fifth LED (binary format), call  db66dev1_WriteLeds(0b00010011);
 * - To set first, second and fifth LED (hex format), call  db66dev1_WriteLeds(0x13);
 */
void db66dev1_WriteLeds(BYTE leds);


/**
 * Gets the current LED values. Each bit in the given byte represents a LED.
 * @return The current LED setting
 */
BYTE db66dev1_ReadLeds(void);



/**
 * Service function. Must be called every 1ms
 */
void db66dev1_Service();

#endif  //#if defined(XBRD_DB66DEV1)

#endif
