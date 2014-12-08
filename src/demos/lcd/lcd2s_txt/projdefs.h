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


//*********************************************************************
 // ------------ LCD2S Configuration (from nz_lcd2s.h) --------------------
 //*********************************************************************
 //Define default backlight value, a value from 0-254 (0 is off)
 #define LCD2S_0_BACKLIGHT  50

 //Define default contrast value, a value from 0-254 (0 is off)
 #define LCD2S_0_CONTRAST   210


#endif  //_PROJDEFS_H_
