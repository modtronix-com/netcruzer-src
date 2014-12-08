 /**
 * @brief           Serial LCD Display handler.
 * @file            nz_lcd2s.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 * @ingroup         mod_sys_lcd2s
 *
 *********************************************************************
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
 * 2007-07-16, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/
#define THIS_IS_NZ_LCD2S_C

#define INCLUDE_NETCRUZER_HEADERS
#include "HardwareProfile.h"

#include "nz_lcd2s.h"
#include "nz_serI2C.h"
#include "nz_tick.h"


//Define default backlight value, a value from 0-254 (0 is off, -1 is don't change)
#ifndef LCD2S_0_BACKLIGHT
#define LCD2S_0_BACKLIGHT  -1   //Don't change is default
#endif
#ifndef LCD2S_1_BACKLIGHT
#define LCD2S_1_BACKLIGHT  -1   //Don't change is default
#endif
#ifndef LCD2S_2_BACKLIGHT
#define LCD2S_2_BACKLIGHT  -1   //Don't change is default
#endif
#ifndef LCD2S_3_BACKLIGHT
#define LCD2S_3_BACKLIGHT  -1   //Don't change is default
#endif

//Define default contrast value, a value from 0-254 (0 is off, -1 is don't change)
#ifndef LCD2S_0_CONTRAST
#define LCD2S_0_CONTRAST   -1   //Don't change is default
#endif
#ifndef LCD2S_1_CONTRAST
#define LCD2S_1_CONTRAST   -1   //Don't change is default
#endif
#ifndef LCD2S_2_CONTRAST
#define LCD2S_2_CONTRAST   -1   //Don't change is default
#endif
#ifndef LCD2S_3_CONTRAST
#define LCD2S_3_CONTRAST   -1   //Don't change is default
#endif

//Define default keypad
#ifndef LCD2S_0_KEYPAD
#define LCD2S_0_KEYPAD   -1   //Don't change is default
#endif
#ifndef LCD2S_1_KEYPAD
#define LCD2S_1_KEYPAD   -1   //Don't change is default
#endif
#ifndef LCD2S_2_KEYPAD
#define LCD2S_2_KEYPAD   -1   //Don't change is default
#endif
#ifndef LCD2S_3_KEYPAD
#define LCD2S_3_KEYPAD   -1   //Don't change is default
#endif

//Define default Keypad Buzzer Period for each LCD2S in ms (16ms increments).
//Use -1 for no change.
#ifndef LCD2S_0_BUZZ_PERIOD_MS
#define LCD2S_0_BUZZ_PERIOD_MS  -1   //Don't change is default
#endif
#ifndef LCD2S_1_BUZZ_PERIOD_MS
#define LCD2S_1_BUZZ_PERIOD_MS  -1   //Don't change is default
#endif
#ifndef LCD2S_2_BUZZ_PERIOD_MS
#define LCD2S_2_BUZZ_PERIOD_MS  -1   //Don't change is default
#endif
#ifndef LCD2S_3_BUZZ_PERIOD_MS
#define LCD2S_3_BUZZ_PERIOD_MS  -1   //Don't change is default
#endif

//Define default Key Repeat Rated for each LCD2S in ms (16ms increments).
//Use -1 for no change.
#ifndef LCD2S_0_KEY_REPEAT_RATE_MS
#define LCD2S_0_KEY_REPEAT_RATE_MS  -1   //Don't change is default
#endif
#ifndef LCD2S_1_KEY_REPEAT_RATE_MS
#define LCD2S_1_KEY_REPEAT_RATE_MS  -1   //Don't change is default
#endif
#ifndef LCD2S_2_KEY_REPEAT_RATE_MS
#define LCD2S_2_KEY_REPEAT_RATE_MS  -1   //Don't change is default
#endif
#ifndef LCD2S_3_KEY_REPEAT_RATE_MS
#define LCD2S_3_KEY_REPEAT_RATE_MS  -1   //Don't change is default
#endif

//Define default Key Repeat Delay for each LCD2S in ms (16ms increments).
//Use -1 for no change.
#ifndef LCD2S_0_KEY_REPEAT_DELAY_MS
#define LCD2S_0_KEY_REPEAT_DELAY_MS -1   //Don't change is default
#endif
#ifndef LCD2S_1_KEY_REPEAT_DELAY_MS
#define LCD2S_1_KEY_REPEAT_DELAY_MS -1   //Don't change is default
#endif
#ifndef LCD2S_2_KEY_REPEAT_DELAY_MS
#define LCD2S_2_KEY_REPEAT_DELAY_MS -1   //Don't change is default
#endif
#ifndef LCD2S_3_KEY_REPEAT_DELAY_MS
#define LCD2S_3_KEY_REPEAT_DELAY_MS -1   //Don't change is default
#endif

//Define default Key Debounce time for each LCD2S in ms (8ms increments).
//Use -1 for no change. Must be a value between 0 to 120ms.
#ifndef LCD2S_0_KEY_DEBOUNCE_TIME_MS
#define LCD2S_0_KEY_DEBOUNCE_TIME_MS -1   //Don't change is default
#endif
#ifndef LCD2S_1_KEY_DEBOUNCE_TIME_MS
#define LCD2S_1_KEY_DEBOUNCE_TIME_MS -1   //Don't change is default
#endif
#ifndef LCD2S_2_KEY_DEBOUNCE_TIME_MS
#define LCD2S_2_KEY_DEBOUNCE_TIME_MS -1   //Don't change is default
#endif
#ifndef LCD2S_3_KEY_DEBOUNCE_TIME_MS
#define LCD2S_3_KEY_DEBOUNCE_TIME_MS -1   //Don't change is default
#endif

//Get pointer to I2C_INFO for selected I2C bus to use
#if (LCD2S_USE_I2C_BUS == 1)
    #if !defined(HAS_SERPORT_I2C1)
    #error "I2C 1 is not enabled for this project! Add HAS_SERPORT_I2C1 to the projdefs.h file!"
    #endif
    I2C_INFO* pInfo = I2C1_INFO;
#elif (LCD2S_USE_I2C_BUS == 2)
    #if !defined(HAS_SERPORT_I2C2)
    #error "I2C 2 is not enabled for this project! Add HAS_SERPORT_I2C2 to the projdefs.h file!"
    #endif
    I2C_INFO* pInfo = I2C2_INFO;
#elif (LCD2S_USE_I2C_BUS == 3)
    #if !defined(HAS_SERPORT_I2C3)
    #error "I2C 3 is not enabled for this project! Add HAS_SERPORT_I2C3 to the projdefs.h file!"
    #endif
    I2C_INFO* pInfo = I2C3_INFO;
#endif


//Keypad buffer - contains received keypad data
#define KPAD_BUF_SIZE_MASK (KPAD_BUF_SIZE-1)
static BYTE kpadBuf[LCD2S_MAX_NUMBER][KPAD_BUF_SIZE];
BYTE kpadBufGet[LCD2S_MAX_NUMBER];    //get index
BYTE kpadBufPut[LCD2S_MAX_NUMBER];    //put index


/////////////////////////////////////////////////
//Function Prototypes
BYTE lcdGetByte(BYTE lcdNum, BYTE cmd, BYTE* val);

#define LCD2S_DELAY_INIT_FOR_EACH_CMD() delay_ms(5)

/**
 * Initialise the serint module. After calling this function, the serEnable() function must be
 * called to enable the serial port.
 */
void lcdInit(void) {
    BYTE i;
    BOOL i2cError;

    i2cError = TRUE;
    
    //Empty all keypad buffers
    for (i=0; i<LCD2S_MAX_NUMBER; i++) {
        kpadBufGet[i] = 0;
        kpadBufPut[i] = 0;
    }

    /////////////////////////////////////////////////
    //Configure LCD at base address
#if ((LCD2S_0_BACKLIGHT!=-1) || (LCD2S_0_CONTRAST!=-1) || (LCD2S_0_KEYPAD!=-1) || (LCD2S_0_BUZZ_PERIOD_MS!=-1) || (LCD2S_0_KEY_REPEAT_RATE_MS!=-1) || (LCD2S_0_KEY_REPEAT_DELAY_MS!=-1) || (LCD2S_0_KEY_DEBOUNCE_TIME_MS!=-1))
    i2cError = TRUE;
    do {
        #if (LCD2S_0_BACKLIGHT != -1)
        //Set backlight
// TEST TEST
//        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_SET_BACKLIGHT, LCD2S_0_BACKLIGHT) != 0)
//            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        #if (LCD2S_0_CONTRAST != -1)
        //Set contrast
// TEST TEST
//        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_SET_CONTRAST, LCD2S_0_CONTRAST) != 0)
//            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

// TEST TEST
//        #if (LCD2S_0_KEYPAD == 16)
//        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x00) != 0)       //4x4 keypad
//            break;  //Error
//        LCD2S_DELAY_INIT_FOR_EACH_CMD();
//        #elif (LCD2S_0_KEYPAD == 12)
//        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x01) != 0)       //4x3 keypad
//            break;  //Error
//        LCD2S_DELAY_INIT_FOR_EACH_CMD();
//        #elif (LCD2S_0_KEYPAD == 8)
//        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x03) != 0)       //4x2 keypad
//            break;  //Error
//        LCD2S_DELAY_INIT_FOR_EACH_CMD();
//        #elif (LCD2S_0_KEYPAD == 4)
//        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x07) != 0)       //4x1 keypad
//            break;  //Error
//        LCD2S_DELAY_INIT_FOR_EACH_CMD();
//        #elif (LCD2S_0_KEYPAD == 0)
//        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x0f) != 0)       //no keypad, all I/Os are inputs
//            break;  //Error
//        LCD2S_DELAY_INIT_FOR_EACH_CMD();
//        #endif

// TEST TEST
//        //Keypad buzzer
//        #if (LCD2S_0_BUZZ_PERIOD_MS != -1)
//        if (i2cWriteSlaveByte(LCD2S_I2C_ADR), LCD2S_CMD_REMEMBER) != 0)
//            break;  //Error
//        i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_SET_KEYPAD_BUZZER_PERIOD, LCD2S_0_BUZZ_PERIOD_MS/16);
//        LCD2S_DELAY_INIT_FOR_EACH_CMD();
//        #endif
//
//        //Key Repeat Rate
//        #if (LCD2S_0_KEY_REPEAT_RATE_MS != -1)
//        if (i2cWriteSlaveByte(LCD2S_I2C_ADR), LCD2S_CMD_REMEMBER) != 0)
//            break;  //Error
//        i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_SET_KEYPAD_REPEAT_RATE, LCD2S_0_KEY_REPEAT_RATE_MS/16);
//        LCD2S_DELAY_INIT_FOR_EACH_CMD();
//        #endif
//
//        //Keypad Repeat delay
//        #if (LCD2S_0_KEY_REPEAT_DELAY_MS != -1)
//        if (i2cWriteSlaveByte(LCD2S_I2C_ADR), LCD2S_CMD_REMEMBER) != 0)
//            break;  //Error
//        i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_SET_KEYPAD_REPEAT_DELAY, LCD2S_0_KEY_REPEAT_DELAY_MS/16);
//        LCD2S_DELAY_INIT_FOR_EACH_CMD();
//        #endif
//
//        //Keypad Debounce time
//        #if (LCD2S_0_KEY_DEBOUNCE_TIME_MS != -1)
//        if (i2cWriteSlaveByte(LCD2S_I2C_ADR), LCD2S_CMD_REMEMBER) != 0)
//            break;  //Error
//        i2cWriteSlaveBytes(LCD2S_I2C_ADR, LCD2S_CMD_SET_KEYPAD_DEBOUNCE_TIME, LCD2S_0_KEY_DEBOUNCE_TIME_MS/8);
//        LCD2S_DELAY_INIT_FOR_EACH_CMD();
//        #endif

        i2cError = FALSE;
        break;
    } while(0);
    if (i2cError == TRUE) {
// TEST TEST
//        //Error, could be that LCD is not present!
//        i2c1PutStopAndWait();    //Reset I2C bus
    }
#endif


    /////////////////////////////////////////////////
    //Configure LCD at base address + 2
#if ((LCD2S_1_BACKLIGHT!=-1) || (LCD2S_1_CONTRAST!=-1) || (LCD2S_1_KEYPAD!=-1) || (LCD2S_1_BUZZ_PERIOD_MS!=-1) || (LCD2S_1_KEY_REPEAT_RATE_MS!=-1) || (LCD2S_1_KEY_REPEAT_DELAY_MS!=-1) || (LCD2S_1_KEY_DEBOUNCE_TIME_MS!=-1))
    i2cError = TRUE;
    do {
        #if (LCD2S_1_BACKLIGHT != -1)
        //Set backlight
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_SET_BACKLIGHT, LCD2S_1_BACKLIGHT) != 0)
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        #if (LCD2S_1_CONTRAST != -1)
        //Set contrast
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_SET_CONTRAST, LCD2S_1_CONTRAST) != 0)
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        #if (LCD2S_1_KEYPAD == 16)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x00) != 0)       //4x4 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_1_KEYPAD == 12)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x01) != 0)       //4x3 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_1_KEYPAD == 8)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x03) != 0)       //4x2 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_1_KEYPAD == 4)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x07) != 0)       //4x1 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_1_KEYPAD == 0)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x0f) != 0)       //no keypad, all I/Os are inputs
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Keypad buzzer
        #if (LCD2S_1_BUZZ_PERIOD_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+2), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_SET_KEYPAD_BUZZER_PERIOD, LCD2S_1_BUZZ_PERIOD_MS/16);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Key Repeat Rate
        #if (LCD2S_1_KEY_REPEAT_RATE_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+2), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_SET_KEYPAD_REPEAT_RATE, LCD2S_1_KEY_REPEAT_RATE_MS/16);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Keypad Repeat delay
        #if (LCD2S_1_KEY_REPEAT_DELAY_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+2), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_SET_KEYPAD_REPEAT_DELAY, LCD2S_1_KEY_REPEAT_DELAY_MS/16);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Keypad Debounce time
        #if (LCD2S_1_KEY_DEBOUNCE_TIME_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+2), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+2, LCD2S_CMD_SET_KEYPAD_DEBOUNCE_TIME, LCD2S_1_KEY_DEBOUNCE_TIME_MS/8);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        i2cError = FALSE;
        break;
    } while(0);
    if (i2cError == TRUE) {
        //Error, could be that LCD is not present!
        i2c1PutStopAndWait();    //Reset I2C bus
    }
#endif


    /////////////////////////////////////////////////
    //Configure LCD at base address + 4
#if ((LCD2S_2_BACKLIGHT!=-1) || (LCD2S_2_CONTRAST!=-1) || (LCD2S_2_KEYPAD!=-1) || (LCD2S_2_BUZZ_PERIOD_MS!=-1) || (LCD2S_2_KEY_REPEAT_RATE_MS!=-1) || (LCD2S_2_KEY_REPEAT_DELAY_MS!=-1) || (LCD2S_2_KEY_DEBOUNCE_TIME_MS!=-1))
    i2cError = TRUE;
    do {
        #if (LCD2S_2_BACKLIGHT != -1)
        //Set backlight
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_SET_BACKLIGHT, LCD2S_2_BACKLIGHT) != 0)
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        #if (LCD2S_2_CONTRAST != -1)
        //Set contrast
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_SET_CONTRAST, LCD2S_2_CONTRAST) != 0)
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        #if (LCD2S_2_KEYPAD == 16)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x00) != 0)       //4x4 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_2_KEYPAD == 12)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x01) != 0)       //4x3 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_2_KEYPAD == 8)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x03) != 0)       //4x2 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_2_KEYPAD == 4)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x07) != 0)       //4x1 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_2_KEYPAD == 0)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x0f) != 0)       //no keypad, all I/Os are inputs
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Keypad buzzer
        #if (LCD2S_2_BUZZ_PERIOD_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+4), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_SET_KEYPAD_BUZZER_PERIOD, LCD2S_2_BUZZ_PERIOD_MS/16);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Key Repeat Rate
        #if (LCD2S_2_KEY_REPEAT_RATE_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+4), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_SET_KEYPAD_REPEAT_RATE, LCD2S_2_KEY_REPEAT_RATE_MS/16);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Keypad Repeat delay
        #if (LCD2S_2_KEY_REPEAT_DELAY_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+4), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_SET_KEYPAD_REPEAT_DELAY, LCD2S_2_KEY_REPEAT_DELAY_MS/16);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Keypad Debounce time
        #if (LCD2S_2_KEY_DEBOUNCE_TIME_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+4), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+4, LCD2S_CMD_SET_KEYPAD_DEBOUNCE_TIME, LCD2S_2_KEY_DEBOUNCE_TIME_MS/8);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        i2cError = FALSE;
        break;
    } while(0);
    if (i2cError == TRUE) {
        //Error, could be that LCD is not present!
        i2c1PutStopAndWait();    //Reset I2C bus
    }
#endif

    /////////////////////////////////////////////////
    //Configure LCD at base address + 6
#if ((LCD2S_3_BACKLIGHT!=-1) || (LCD2S_3_CONTRAST!=-1) || (LCD2S_3_KEYPAD!=-1) || (LCD2S_3_BUZZ_PERIOD_MS!=-1) || (LCD2S_3_KEY_REPEAT_RATE_MS!=-1) || (LCD2S_3_KEY_REPEAT_DELAY_MS!=-1) || (LCD2S_3_KEY_DEBOUNCE_TIME_MS!=-1))
    i2cError = TRUE;
    do {
        #if (LCD2S_3_BACKLIGHT != -1)
        //Set backlight
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_SET_BACKLIGHT, LCD2S_3_BACKLIGHT) != 0)
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        #if (LCD2S_3_CONTRAST != -1)
        //Set contrast
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_SET_CONTRAST, LCD2S_3_CONTRAST) != 0)
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        #if (LCD2S_3_KEYPAD == 16)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x00) != 0)       //4x4 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_3_KEYPAD == 12)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x01) != 0)       //4x3 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_3_KEYPAD == 8)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x03) != 0)       //4x2 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_3_KEYPAD == 4)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x07) != 0)       //4x1 keypad
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #elif (LCD2S_3_KEYPAD == 0)
        if (i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_CONFIG_KEYPAD_AND_IO, 0x0f) != 0)       //no keypad, all I/Os are inputs
            break;  //Error
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Keypad buzzer
        #if (LCD2S_3_BUZZ_PERIOD_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+6), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_SET_KEYPAD_BUZZER_PERIOD, LCD2S_3_BUZZ_PERIOD_MS/16);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Key Repeat Rate
        #if (LCD2S_3_KEY_REPEAT_RATE_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+6), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_SET_KEYPAD_REPEAT_RATE, LCD2S_3_KEY_REPEAT_RATE_MS/16);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Keypad Repeat delay
        #if (LCD2S_3_KEY_REPEAT_DELAY_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+6), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_SET_KEYPAD_REPEAT_DELAY, LCD2S_3_KEY_REPEAT_DELAY_MS/16);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        //Keypad Debounce time
        #if (LCD2S_3_KEY_DEBOUNCE_TIME_MS != -1)
        if (i2cWriteSlaveByte(LCD2S_I2C_ADR+6), LCD2S_CMD_REMEMBER) != 0)
            break;  //Error
        i2cWriteSlaveBytes(LCD2S_I2C_ADR+6, LCD2S_CMD_SET_KEYPAD_DEBOUNCE_TIME, LCD2S_3_KEY_DEBOUNCE_TIME_MS/8);
        LCD2S_DELAY_INIT_FOR_EACH_CMD();
        #endif

        i2cError = FALSE;
        break;
    } while(0);
    if (i2cError == TRUE) {
        //Error, could be that LCD is not present!
        i2c1PutStopAndWait();    //Reset I2C bus
    }
#endif

}


/**
 * LCD2S Task. Must be regulary called by main program (NOT IN ISR!). Does NOT have to be called by
 * main program if LCD2S keyboard support is not required. This function does the following:
 * - Check if a button was pressed on the LCD2S keypad, and if so, gets it (use kpadGetKey() to get it).
 */
void lcd2sTask(void) {
    static WORD tLcd = 0;
    static BYTE lcdNum=0; //Keypad to service
    BYTE c;

    //Enter every 40ms
    if (tick16TestTmr(tLcd)) {
        tick16UpdateTmrMS(tLcd, 40);     //Update timer to expire in 40ms again

        //Empty all keypad buffers
        //Get status byte from LCD
        if (lcdGetByte(lcdNum, LCD2S_CMD_GET_STATUS, &c) == 0) {
            //Has the LCD got keys to be read
            if (c & 0x80) {
                //Check if more space left in buffer - last byte of buffer can not be used, because that is when put=get, which indicates empty
                if ( ((kpadBufPut[lcdNum]+1)&KPAD_BUF_SIZE_MASK) != kpadBufGet[lcdNum]) {
                    //Get key from LCD
                    lcdGetByte(lcdNum, LCD2S_CMD_GET_KEY, &c);

                    //Add byte to kpadBuf, and increment put pointer
                    kpadBuf[lcdNum][ kpadBufPut[lcdNum]++ ] = c;
                    kpadBufPut[lcdNum] &= KPAD_BUF_SIZE_MASK;
                }
            }
        }

        //Increment keypad to service
        if (++lcdNum >= LCD2S_MAX_NUMBER) {
            lcdNum = 0; //Reset to first keypad
        }
    }
}


/**
 * Get a single byte from the LCD display.
 *
 * @param lcddNum The number of the lcd to read status byte from. Is a number from 0-n.
 *        If, for example, the system is configured for 4 keypads, it is a number from 0-3.
 *
 * @param cmd LCD command for byte to get, for example 0xD0 is status byte, 0xD1 is key, 0xD3 is GPIO bytes.....
 *
 * @param val Read byte is returned in this parameter
 *
 * @return Returns 0 if OK, 1 if no ACK received
 */
BYTE lcdGetByte(BYTE lcdNum, BYTE cmd, BYTE* val) {
//    //Read LCD status byte
//    i2cWaitForIdle();
//
//    i2cPutStartAndWait();       //Write I2C Start Condition, and wait for it to complete
//
//    while(1) {
//        //Write module address, lcdNum is multiplied by two because LSB bit is Read/Write bit! Must be 0 for write
//        i2cWriteByteAndWait(LCD2S_I2C_ADR + (lcdNum*2));
//
//        //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
//        if(i2cWasAckReceived() == 0) break;
//
//        //Write get status byte command
//        i2cWriteByteAndWait(cmd);
//
//        //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
//        if(i2cWasAckReceived() == 0) break;
//
//        //Send repeated I2C start condition
//        i2cRestartAndWait();
//
//        //Send module address, and read/write bit (LSB bit) set - this is a read message
//        i2cWriteByteAndWait(LCD2S_I2C_ADR + 1 + (lcdNum*2));
//
//        //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
//        if(i2cWasAckReceived() == 0) break;
//
//        //Shift in byte from I2C port
//        i2cReadByte(val, 0);     //ACK each byte except for last one. Only one byte, so no ACK
//
//        i2cPutStopAndWait();    //Write I2C Stop condition, and wait for it to complete
//
//        return 0;   //Return OK
//    }
//
//    //Error, reset I2C bus.
//    i2cPutStopAndWait();    //Reset I2C bus
    return 1;               //Return error
}


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
BYTE kpadGetKey(BYTE kpadNum) {
    BYTE ret;
    
    //Check kpadNum is a valid number
    if (kpadNum >= LCD2S_MAX_NUMBER)
        return 0;

    //Read byte from kpadBuf, and increment get pointer
    ret = kpadBuf[kpadNum][ kpadBufGet[kpadNum]++ ];
    kpadBufGet[kpadNum] &= KPAD_BUF_SIZE_MASK;
    return ret;
}


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
INT8 lcdDisplayChar(BYTE lcdNum, char c) {
// TEST TEST
//    return i2cWriteSlaveBytes(LCD2S_I2C_ADR + (lcdNum*2), LCD2S_CMD_WRITE_STRING, c);
    return 0;
}


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
INT8 lcdDisplayCharHex(BYTE lcdNum, BYTE c) {
    INT8 ret;
    if ((ret = lcdDisplayChar(lcdNum,  (c > 0x9f) ? ((c>>4)&0x0f)+0x57 : ((c>>4)&0x0f)+'0')) == 0) {
        return lcdDisplayChar(lcdNum,  ((c&0x0f)>9) ? (c&0x0f)+0x57 : (c&0x0f)+'0' );
    }

    return ret;
}


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
INT8 lcdPutCmd(BYTE lcdNum, BYTE cmd)  {
// TEST TEST
//    return i2cWriteSlaveByte(LCD2S_I2C_ADR + (lcdNum*2), cmd);
    return 0;
}


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
INT8 lcdPutCmd1Param(BYTE lcdNum, BYTE cmd, BYTE param1)  {
// TEST TEST
//    return i2cWriteSlaveBytes(LCD2S_I2C_ADR + (lcdNum*2), cmd, param1);
    return 0;
}


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
INT8 lcdPutCmd2Param(BYTE lcdNum, BYTE cmd, BYTE param1, BYTE param2) {
// TEST TEST
//    BYTE arr[3] = {cmd, param1, param2};
//    return i2cWriteSlaveBlock(LCD2S_I2C_ADR + (lcdNum*2), arr, 3);
    return 0;
}


/**
 * Writes given array to the LCD Display. For example, to create custom character use following
 * command. This example uses 0x03 for all 8 bytes of custom character, the last 9 is the length of anonymous array.
 *
 * lcdPutArray(0, const BYTE[] = {LCD2S_CMD_DEFINE_CUSTOM_CHAR, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03}, 9);
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 * @param buf       Buffer containing data to send to LCD Display
 * @param len       Number of bytes to send from given buffer
 *
 * @return 0 if OK, else error
 */
INT8 lcdPutArray(BYTE lcdNum, BYTE* buf, BYTE len) {
// TEST TEST
//    return i2cWriteSlaveBlock(LCD2S_I2C_ADR + (lcdNum*2), buf, len);
    return 0;
}


/**
 * Display the given string on the LCD Display. It is appended to the current LCD
 * Displayed string. The following bytes have special meanings:
 * - 0x0a ('\n') = Go to beginning of next line.
 * - 0x0c ('\f') = Clear display and go to beginning of first line.
 * - 0x0d ('\r') = Go to beginning of first line
 * - 0x08 ('\b') = Cursor left
 * - 0x09 ('\t') = Cursor Right
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
INT8 lcdDisplayString(BYTE lcdNum, char* s) {
//
//    return i2cWriteSlaveRegString(LCD2S_I2C_ADR + (lcdNum*2), LCD2S_CMD_WRITE_STRING, s);
return 0;
}


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
 * @param s     Null terminated string to write out to the LCD Display
 *
 * @return 0 if OK, else error
 */
INT8 lcdSetStartupScreen(BYTE lcdNum, BYTE row, char* string) {
    BYTE buf[21];
    BYTE i;
    char c;

    buf[0] = row;

    for (i=1; i<=20; i++) {
        c = *string++;
        if (c == 0)
            break;
        buf[i] = c;
    }

// TEST TEST
//    return i2cWriteSlaveRegBlock(LCD2S_I2C_ADR + (lcdNum*2), LCD2S_CMD_SET_STARTUP_SCREEN, buf, i);
    return 0;
}


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
INT8 lcdDrawVertBargraph(BYTE lcdNum, BYTE row, BYTE col, BYTE height) {
    BYTE buf[3];
    buf[0] = row;
    buf[1] = col;
    buf[2] = height;

// TEST TEST
//    return i2cWriteSlaveRegBlock(LCD2S_I2C_ADR + (lcdNum*2), LCD2S_CMD_DRAW_VERT_BAR_GRAPH, buf, 3);
    return 0;
}








