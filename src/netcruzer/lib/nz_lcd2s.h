/**
 * @brief           Serial LCD Display handler.
 * @file            nz_lcd2s.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_lcd2s_desc Description
 *****************************************
 * This module implements a driver for the LCD2S range of serial LCD Displays.
 * To add LCD2S support to a project, do one of the following:
 *
 * ----- Option 2 -----
 * - Add nz_lcd2s.c to the project, this is the main LCD2S driver file.
 * - The following additional files are required by nz_lcd2s.c, and must be added to the project:
 *   nz_circularBufferPwr2.c, nz_helpers.c, nz_netcruzer.c and nz_serI2C.c
 * - Add "HAS_SERPORT_I2C1" to projdefs.h file.
 * - In code, initialize LCD2S. Ensure to delay 300ms from power up. For example:
 *      delay_ms(300);
 *      lcdInit();
 * - In projdefs.h, do any LCD2S, I2C or other configuration required. This is done by copying the
 *   configuration section from the *.h files to projdefs.h. Nothing required, defaults will work!
 * - In code, call lcd2sTask() repetitively in main loop
 * - Can now use LCD2S functions to write to LCD and perform other tasks. Some examples:
 *      lcdDisplayString(0, "\fHello\nWorld, Again!");      //Write parsed string to LCD display
 *      lcdDisplayChar(0, 'x');                             //Display 'x' at current cursor position
 *
  * ----- Option 1 (Add LCD support via nz_netcruzer.c) -----
 * - Add nz_netcruzer.c to project
 * - Add "#define    NZSYS_ENABLE_LCD2S_AND_I2C1" to projdefs.h
 * - In code, call nzSysInitDefault() once at initialization stage
 * - In code, call nzSysTaskDefault() repetitively in main loop
 * - Can now use LCD2S functions to write to LCD and perform other tasks. Some examples:
 *      lcdDisplayString(0, "\fHello\nWorld, Again!");      //Write parsed string to LCD display
 *      lcdDisplayChar(0, 'x');                             //Display 'x' at current cursor position
 *
 * @subsection nz_lcd2s_conf LCD2S Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // ------------ LCD2S Configuration (from nz_lcd2s.h) ---------------------
 // *********************************************************************
 //Define the I2C address for the LCD display
 #define LCD2S_I2C_ADR                  LCD2S_DEFAULT_I2C_ADR   //[-DEFAULT-]

 //Define the default I2C address for the LCD display
 #define LCD2S_DEFAULT_I2C_ADR          (80ul)  //[-DEFAULT-]

 //Define which I2C bus the LCD2S should use, 1 is default. Is I2C 1 for all SBC66 Netcruzer boards
 #define LCD2S_USE_I2C_BUS              ( 1 )   //[-DEFAULT-]

 //Define number of serial LCD displays supported, 1 is default
 #define LCD2S_MAX_NUMBER               ( 1 )   //[-DEFAULT-]

 //Define size of keypad buffer for each serial LCD display, must be 8,16,32 or 64
 #define KPAD_BUF_SIZE                  ( 8ul ) //[-DEFAULT-]

 //Define keypad for each LCD2S. Can be 0 for no keypad, 4 for 1x4 keypad, 8 for 2x4 keypad,
 //12 for a 3x4 keypad, or 16 for a 4x4 keypad. A value of -1 = no change.
 #define LCD2S_0_KEYPAD                 ( 0 )   //[-DEFAULT-] No keypad
 //#define LCD2S_0_KEYPAD               ( 12 )  //LCD2S has a 3x4 keypad
 //#define LCD2S_0_KEYPAD               ( 16 )  //LCD2S has a 4x4 keypad. Ensure OUT1 jumper on back of LCD2S is REMOVED!

 //Define default backlight for each LCD2S, a value from 0-254 (0 is off, -1 is don't change)
 #define LCD2S_0_BACKLIGHT              ( -1 )  //[-DEFAULT-]

 //Define default contrast for each LCD2S, a value from 0-254 (0 is off, -1 is don't change)
 #define LCD2S_0_CONTRAST               ( -1 )  //[-DEFAULT-]

 //Define default Keypad Buzzer Period for each LCD2S in ms (16ms increments). Use -1 for no change.
 #define LCD2S_0_BUZZ_PERIOD_MS         ( -1 )  //[-DEFAULT-] No Change

 //Define default Key Repeat Rated for each LCD2S in ms (16ms increments). Use -1 for no change.
 #define LCD2S_0_KEY_REPEAT_RATE_MS     ( -1 )  //[-DEFAULT-] No Change

 //Define default Key Repeat Delay for each LCD2S in ms (16ms increments). Use -1 for no change.
 #define LCD2S_0_KEY_REPEAT_DELAY_MS    ( -1 )  //[-DEFAULT-] No Change

 //Define default Key Debounce time for each LCD2S in ms (8ms increments). Use -1 for no change.
 #define LCD2S_0_KEY_DEBOUNCE_TIME_MS   ( -1 )  //[-DEFAULT-] No Change

 @endcode
 **********************************************************************
 * @section nz_lcd2s_lic Software License Agreement
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
 * 2007-07-16, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/

#ifndef _LCD2S_H_
#define _LCD2S_H_

/////////////////////////////////////////////////
//Global defines
#ifndef LCD2S_I2C_ADR
#define LCD2S_I2C_ADR  80ul  //LCD2S I2C Address
#endif

//Default is a single LCD2S display on I2C bus
#ifndef LCD2S_MAX_NUMBER
#define LCD2S_MAX_NUMBER 1
#endif

//Default buffer of 8 for each LCD2S keypad buffer
#ifndef KPAD_BUF_SIZE
#define KPAD_BUF_SIZE  8ul
#endif

//Use I2C bus 1 by default
#ifndef LCD2S_USE_I2C_BUS
#define LCD2S_USE_I2C_BUS  1
#endif


//LCD2S Commands
/** Cursor moves backwards, no parameters */
#define LCD2S_CMD_CURSOR_MOVES_BACKWARDS    0x01

/** Cursor moves forward, no parameters */
#define LCD2S_CMD_CURSOR_MOVES_FORWARD      0x09

/** Blink Block cursor off, No parameters */
#define LCD2S_CMD_BLINK_BLOCK_CURSOR_OFF    0x10

/** Underline cursor on, No parameters */
#define LCD2S_CMD_UNDERLINE_CURSOR_ON       0x19

/** Display on, No parameters */
#define LCD2S_CMD_DISPLAY_ON                0x1A

/** Display off, No parameters */
#define LCD2S_CMD_DISPLAY_OFF               0x12

/** Configure interrupt pin as a push-pull output (not open collector), No parameters */
#define LCD2S_CMD_INTERRUPT_PIN_PUSH_PULL   0x22

/** Backlight on, No parameters */
#define LCD2S_CMD_BACKLIGHT_ON              0x28

/** Backlight off, No parameter */
#define LCD2S_CMD_BACKLIGHT_OFF             0x20

/** OUT1 off, No parameterss */
#define LCD2S_CMD_OUT1_OFF                  0x30

/** OUT2 off, No parameterss */
#define LCD2S_CMD_OUT2_OFF                  0x31

/** OUT1 on, No parameterss */
#define LCD2S_CMD_OUT1_ON                   0x38

/** OU21 on, No parameterss */
#define LCD2S_CMD_OUT2_ON                   0x39

/** GPIO1 off, No parameterss */
#define LCD2S_CMD_GPIO1_OFF                 0x40

/** GPIO2 off, No parameterss */
#define LCD2S_CMD_GPIO2_OFF                 0x41

/** GPIO1 on, No parameterss */
#define LCD2S_CMD_GPIO1_ON                  0x48

/** GPIO2 on, No parameterss */
#define LCD2S_CMD_GPIO2_ON                  0x49

/** Write string, string to follow */
#define LCD2S_CMD_WRITE_STRING              0x80

/** Set Backlight, byte to follow (0-255) */
#define LCD2S_CMD_SET_BACKLIGHT             0x81

/** Set contrast, byte to follow (0-254) */
#define LCD2S_CMD_SET_CONTRAST              0x82

/** Move cursor right, no parameters */
#define LCD2S_CMD_MOVE_CURSOR_RIGHT         0x83

/** Move cursor left, no parameters */
#define LCD2S_CMD_MOVE_CURSOR_LEFT          0x84

/** Shift display right, No parameters */
#define LCD2S_CMD_SHIFT_DISPLAY_RIGHT       0x85

/** Shift display left, No parameters */
#define LCD2S_CMD_SHIFT_DISPLAY_LEFT        0x86

/** Shift display up, No parameters */
#define LCD2S_CMD_SHIFT_DISPLAY_UP          0x85

/** Shift display down, No parameters */
#define LCD2S_CMD_SHIFT_DISPLAY_DOWN        0x86

/** Set cursor address, byte to follow (0-254) */
#define LCD2S_CMD_SET_CURSOR_ADDRESS        0x89 

/** Set cursor address, next byte is row (1-n), next byte is column (1-n) */
#define LCD2S_CMD_SET_CURSOR_POSITION       0x8A

/** Clear Display, No parameters */
#define LCD2S_CMD_CLEAR_DISPLAY             0x8C

/** Remember next command, No parameters */
#define LCD2S_CMD_REMEMBER                  0x8D

/** Set I2C base address, next byte is character set code (0=vert bar graph, 1=nrw v.bar graph, 2=larg num) */
#define LCD2S_CMD_LOAD_CUSTOM_CHAR_SET      0x8E

/** Write large number string, string to follow with numbers (0 1 2 3 4 5 6 7 8 9 . : space) */
#define LCD2S_CMD_WRITE_LARGE_NUM_STING     0x8F

/** Set startup screen for a single line. Next byte is line number (1-4), followed by string (message for that line */
#define LCD2S_CMD_SET_STARTUP_SCREEN        0x90

/** Set I2C base address, next byte is address */
#define LCD2S_CMD_SET_I2C_BASE_ADR          0x91

/** Set I2C base address, next byte is adr (0-7), next 8 bytes are pixels */
#define LCD2S_CMD_DEFINE_CUSTOM_CHAR        0x92

/** Draw vert. bar graph, next byte is row (1-n), next byte is column (1-n), next byte is height (0-8) */
#define LCD2S_CMD_DRAW_VERT_BAR_GRAPH       0x93

/** Draw tall vert. bar graph (span 2 rows), next byte is row (1-n), next byte is column (1-n), next byte is height (0-8) */
#define LCD2S_CMD_DRAW_TALL_VERT_BAR_GRAPH  0x94

/** Configure device, followed by 4 bytes */
#define LCD2S_CMD_CONFIG_DEVICE             0x95

/** Set keypad repeat delay, byte to follow is 16ms multiples time (1-100 = 16-1600ms) */
#define LCD2S_CMD_SET_KEYPAD_REPEAT_DELAY   0xA0

/** Set keypad repeat rate, byte to follow is 16ms multiples time (1-100 = 16-1600ms) */
#define LCD2S_CMD_SET_KEYPAD_REPEAT_RATE    0xA1

/** Set keypad buzzer period, byte to follow is 16ms multiples time (1-100 = 16-1600ms) */
#define LCD2S_CMD_SET_KEYPAD_BUZZER_PERIOD  0xA2

/** Set maximum brightness, byte to follow (1-255) */
#define LCD2S_CMD_MAX_BACKLIGHT             0xA3    

/** Get status. Next byte read is status. Bit 7 indicates if keypad buffer contains data (keys). Bits 0-6 = receive buffer space available. */
#define LCD2S_CMD_GET_STATUS                0xD0

/** Read next key from buffer. Next byte read is key */
#define LCD2S_CMD_GET_KEY                   0xD1

/** Read GPIO1, GPIO2 and GPIO3. Next byte read is values in bits 0 - 2 */
#define LCD2S_CMD_GET_GPIO1_TO_GPIO3        0xD3

/** Configure keypad and IO, byte to follow */
#define LCD2S_CMD_CONFIG_KEYPAD_AND_IO      0xE0

/** Set keypad debounce time, byte to follow is 8ms multiples time (1-15 = 8-120ms) */
#define LCD2S_CMD_SET_KEYPAD_DEBOUNCE_TIME  0xE1

/** Set OUT1 and OUT2, byte to follow is value (bit 0=OUT1, bit 1=OUT2) */
#define LCD2S_CMD_SET_OUT1_AND_OUT2         0xE2

/** Configure GPIO1, byte to follow */
#define LCD2S_CMD_CONFIG_GPIO1              0xE3

/** Configure GPIO1, byte to follow */
#define LCD2S_CMD_CONFIG_GPIO2              0xE4



/////////////////////////////////////////////////
//Global data
extern BYTE kpadBufGet[LCD2S_MAX_NUMBER];    //get index
extern BYTE kpadBufPut[LCD2S_MAX_NUMBER];    //put index


/////////////////////////////////////////////////
//Function prototypes


/**
 * Called in the USART TX ISR
 */
void lcdInit(void);


/**
 * Service this module
 */
void lcd2sTask(void);


/**
 * Are there any keys in the keypad buffer.
 * @return 1 if true, else 0
 */
#define kpadHasKeys(kpadNum) (kpadBufPut[kpadNum] != kpadBufGet[kpadNum])


/**
 * Get the next key from the keypad buffer. Before calling this function, the
 * kpadHasKeys() function should be called to check if there are any
 * keys to be read from the keypad buffer.
 *
 * @param kpadNum The number of the keypad you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 keypads, it is a number from 0-3.
 *
 * @return Returns next key code from the keypad buffer.
 */
BYTE kpadGetKey(BYTE kpadNum);


/**
 * Writes a command with no parameters to LCD Display. For example, to turn off the backlight:
 * lcdPutCmd(0, LCD2S_CMD_BACKLIGHT_OFF);
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param cmd The command to send the LCD display, is a LCD2S_CMD_XX define.
 *
 * @return 0 if OK, else error
 */
INT8 lcdPutCmd(BYTE lcdNum, BYTE cmd);


/**
 * Writes a command with 1 parameter to LCD Display. For example, to set backlight to 100:
 * lcdPutCmd1Param(0, LCD2S_CMD_SET_BACKLIGHT, 100);
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param cmd The command to send the LCD display, is a LCD2S_CMD_XX define.
 *
 * @param param1 Parameter to send after command
 *
 * @return 0 if OK, else error
 */
INT8 lcdPutCmd1Param(BYTE lcdNum, BYTE cmd, BYTE param1);


/**
 * Writes a command with 2 parameter to LCD Display. For example, to set cursor to row 1, column 5:
 * lcdPutCmd2Param(0, LCD2S_CMD_SET_CURSOR_POSITION, 1, 5);
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param cmd The command to send the LCD display, is a LCD2S_CMD_XX define.
 *
 * @param param1 First parameter to send after command
 *
 * @param param2 Second parameter to send after command
 *
 * @return 0 if OK, else error
 */
INT8 lcdPutCmd2Param(BYTE lcdNum, BYTE cmd, BYTE param1, BYTE param2);


/**
 * Writes given number of characters from given buffer to the LCD Display.
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 * @param buf       Buffer containing data to send to LCD Display
 * @param len       Number of bytes to send from given buffer
 *
 * @return 0 if OK, else error
 */
INT8 lcdPutArray(BYTE lcdNum, BYTE* buf, BYTE len);


/**
 * Display the given character on the LCD Display.
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param c Character to write out on the serial port
 *
 * @return  Returns 0 if OK, else returns a non-zero value if error
 */
INT8 lcdDisplayChar(BYTE lcdNum, char c);


/**
 * Display the given byte in two byte ASCII hex on the LCD Display. It is appended to the
 * current LCD Display. For example, if c=11, then "0B" will be sent to the LCD Display
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param c Byte to display
 *
 * @return  Returns 0 if OK, else returns a non-zero value if error
 */
INT8 lcdDisplayCharHex(BYTE lcdNum, BYTE c);


/**
 * Display the given string on the LCD Display. It is appended to the current LCD
 * Displayed string. The following bytes have special meanings:
 * @code
 * - 0x0a ('\n') = Go to beginning of next line.
 * - 0x0c ('\f') = Clear display and go to beginning of first line.
 * - 0x0d ('\r') = Go to beginning of first line
 * - 0x08 ('\b') = Cursor left
 * - 0x09 ('\t') = Cursor Right
 * @endcode
 *
 * For example, the following command will clear the display,write "Hello" at beginning of first
 * line, and "World!" at the beginning of second line:
 * lcdDisplayString(0, "\fHello\nWorld, Again!");
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param s     Null terminated string to write out to the LCD Display
 *
 * @return 0 if OK, else error
 */
INT8 lcdDisplayString(BYTE lcdNum, char* s);


/**
 * Sets a single line of the startup screen. The startup screen is displayed when the LCD is switched on.
 *
 * The row parameter of this command gives the LCD row that the following string is for. For a two line
 * display (16 by 2 lines for example), the row parameter can be 1 or 2. For a 4 line display (20 by 4 lines
 * for example), the row parameter can be 1, 2, 3 or 4.
 *
 * The string parameter of this command should be a maximum of 20 characters long. For a 2 by 16 line display,
 * the string will be 16 characters long. For a 4 by 20 line display the string will be 20 characters long.
 *
 * For example, to set line one of a 2 by 16 line display to ?This is a very? and line two to ?cool LCD Display?, the following two
 * commands have to be sent to the LCD2S:
 * @code
 * lcdSetStartupScreen(0, 1, "This is a very  ");  //For a 2x16 line LCD, ensure is 16 characters long - add to spaces to end!
 * lcdSetStartupScreen(0, 2, "cool LCD Display");
 * @endcode
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param row The row this line is meant for, a value from 1-n.
 *
 * @param string Null terminated string to write out to the LCD Display
 *
 * @return 0 if OK, else error
 */
INT8 lcdSetStartupScreen(BYTE lcdNum, BYTE row, char* string);


/**
 * Draws a vertical bar graph in the space of a single character position. Before using this
 * command, a vertical bar graph character set has to be loaded with the lcdLoadCustomCharSet()
 * function.
 *
 * The row and column parameters gives the location of the bar graph, and are values from 1 to
 * the maximum row and column count of the display used.
 *
 * The height parameter gives the height of the bar graph to draw, and can have a range from 0 to 8.
 * If 0, the given location is cleared. For a value between 1-8 a vertical bar graph is drawn in the
 * given cell with that height.
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param row The row this bargraph will be displayed in, a value from 1-n.
 *
 * @param col The column this bargraph will be displayed in, a value from 1-n.
 *
 * @param height The height, a value from 0-8.
 *
 * @return 0 if OK, else error
 */
INT8 lcdDrawVertBargraph(BYTE lcdNum, BYTE row, BYTE col, BYTE height);


/**
 * Set position of cursor.
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param row The row to place the cursor at, is a value from 1-n
 *
 * @param col The column to place the cursor at, is a value from 1-n
 *
 * @return 0 if OK, else error
 */
#define lcdSetCursorPosition(lcdNum, row, col) lcdPutCmd2Param(lcdNum, LCD2S_CMD_SET_CURSOR_POSITION, row, col)

/**
 * Set backlight brightness, is a value from 0-255.
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param value The new backlight brightness
 *
 * @return 0 if OK, else error
 */
#define lcdSetBacklight(lcdNum, value) lcdPutCmd1Param(lcdNum, LCD2S_CMD_SET_BACKLIGHT, value)

/**
 * Set display contrast, is a value from 0-254.
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param value The new display contrast
 *
 * @return 0 if OK, else error
 */
#define lcdSetContrast(lcdNum, value) lcdPutCmd1Param(lcdNum, LCD2S_CMD_SET_CONTRAST, value)

/**
 * This command is used to load the 8 custom characters with a set of 8 predefined characters.
 * Some commands, like the bar graph commands, require that a specific custom character set be loaded
 * prior to it's use. The following custom character sets are defined:
 *
 * - Character Set Code 0 (Vertical Bar Graph): Defines character set for displaying numbers that are
 *   3 character high, and 2 character wide. This character set has to be loaded prior to using the ?Write
 *   Large Number String to LCD? command.
 *
 * - Character Set Code 1 (Vertical Bar Graph Narrow): Defines character set for displaying numbers that
 *   are 3 character high, and 2 character wide. This character set has to be loaded prior to using the
 *   ?Write Large Number String to LCD? command.
 *
 * - Character Set Code 2 (Large 3x2 Number Character Set): Defines character set for displaying numbers
 *   that are 3 character high, and 2 character wide. This character set has to be loaded prior to using the
 *   ?Write Large Number String to LCD? command.
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param value The code for the custom character set to load, for current version a value from 0 to 2.
 *
 * @return 0 if OK, else error
 */
#define lcdLoadCustomCharSet(lcdNum, value) lcdPutCmd1Param(lcdNum, LCD2S_CMD_LOAD_CUSTOM_CHAR_SET, value)

/**
 * This command is used to define a custom character. See LCD2S documentation for details.
 * The LCD2S has space for 8 custom characters. Each custom character is 5 pixels wide by 8 pixels high.
 * This command is used to define one of these 8 custom characters.
 *
 * The address parameter indicates which custom character this command is defining, and must have a value from
 * 0 to 7, where 0 is for the first custom character, 1 for the second custom character, up to 7 which is
 * for the eighth character.
 *
 * Following the adr parameter are 8 bytes that define the custom character. Bits 0 to 4 of each byte will
 * each define a pixel of the character. When the bit is 1, the pixel is on. When the bit is 0, the pixel
 * is off. The first byte defines the pixels for the top row, the second byte defines the pixels for the
 * second row, up to the eighth byte which defines the pixels for the last row.
 *
 * For example:
 * @code
 *  BYTE buf[] = {
 *      LCD2S_CMD_DEFINE_CUSTOM_CHAR,   //First byte must be command
 *      0,                              //Second byte is address of custom char to follow, a value from 0 to 7
 *      0x55,                           //1st byte of custom character
 *      0x55,                           //2nd byte of custom character
 *      0x55,                           //3rd byte of custom character
 *      0x55,                           //4th byte of custom character
 *      0x55,                           //5th byte of custom character
 *      0x55,                           //6th byte of custom character
 *      0x55,                           //7th byte of custom character
 *      0x55                            //8th byte of custom character
 *  };
 *  lcdDefineCustomChar(0, buf);
 * @endcode
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param buf Block containing 10 bytes. First byte is LCD2S_CMD_DEFINE_CUSTOM_CHAR, second byte is address. Following 8 bytes is character
 *
 * @return 0 if OK, else error
 */
#define lcdDefineCustomChar(lcdNum, buf) lcdPutArray(lcdNum, buf, 10)


#endif    //_LCD2S_H_




