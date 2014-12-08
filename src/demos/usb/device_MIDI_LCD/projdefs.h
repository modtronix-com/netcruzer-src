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
// ------------------ Circular Buffer Configuration --------------------
// *********************************************************************
//Specifies what circular buffer is used. Ensure only one of the following is uncommented. If none selected, nz_circularBufferStd is used
#define    CIRBUF_USE_CIRCULAR_BUFFER_STD      //Use nz_circularBufferStd
//#define    CIRBUF_USE_CIRCULAR_BUFFER_PWR2     //Use nz_circularBufferPwrs

//Optimize for size, not speed!
#define    CIRBUF_OPTIMIZE_SIZE


// *********************************************************************
// --------------- I2C Configuration (from nz_serI2C.h) ----------------
// *********************************************************************
//---- Enable I2C1 as a "Serial Data Port" ----
#define HAS_SERPORT_I2C1


// *********************************************************************
// -------------- USB stack hardware selection options -----------------
// *********************************************************************
//Defined in usb_config.h
//#define USB_IS_CDC
//#define USB_IS_HID
#define USB_IS_MIDI


#endif  //_PROJDEFS_H_
