/**
 * Project Specific Defines. Are defines in projdefs.h. These are defines that
 * is the same for all target hardware. Place hardware specific defines in
 * "Configs/HWP_BOARDNAME.h" file for target board
 */

#ifndef _PROJDEFS_H_
#define _PROJDEFS_H_


//Ensure this define is uncommented for release build
#define RELEASE_BUILD


// *********************************************************************
// ------------ I2C Configuration (from nz_serI2C.h) -------------
// *********************************************************************
//---- Enable I2C1 Serial Port ----
#define HAS_SERPORT_I2C1

//Transmit and Receive buffer sizes, for compatibility, ensure it is a power of 2 value! This can be bypassed
//by using a "non power of 2" CircularBuffer implementation for this project. See nz_CircularBuffer.h for details.
#define    I2C1_RXBUF_SIZE  32      //[-DEFAULT-]
#define    I2C1_TXBUF_SIZE  256     //[-DEFAULT-]

//I2C1 speed, can be 10K, 20K, 50K, 100K, 400K or 1M. Can also use value, for example 78 will give 200kHz.
//IMPORTANT to limit it to speed of lowest device on bus! For example, is 400kHz for RTC on SBC66EC!
#define    I2C1_BAUD I2C_BAUD_100K  //[-DEFAULT-]


//*********************************************************************
 // ------------ LCD2S Configuration (see nz_lcd2s.h) ----------------------
 //*********************************************************************
 //Define keypad for each LCD2S (maximum of 4, LCD2S_0, LCD2S_1, LCD2S_2, LCD2S_3). Can be 0 for
 //no keypad, 12 for a 3x4 keypad, or 16 for a 4x4 keypad
 #define LCD2S_0_KEYPAD     16    //LCD2S has a 4x4 keypad. Ensure OUT1 jumper on back of LCD2S is REMOVED!

 //Define default backlight value, a value from 0-254 (0 is off)
 #define LCD2S_0_BACKLIGHT  50

 //Define default contrast value, a value from 0-254 (0 is off)
 #define LCD2S_0_CONTRAST   210


#endif  //_PROJDEFS_H_
