/**
 * Project Specific Defines. Are defines in projdefs.h. These are defines that
 * is the same for all target hardware. Place hardware specific defines in
 * "Configs/HWP_BOARDNAME.h" file for target board
 */

#ifndef _PROJDEFS_H_
#define _PROJDEFS_H_


//Ensure this define is uncommented for release build
//#define RELEASE_BUILD


// *********************************************************************
// ---------- Netcruzer System Configuration (nz_netcruzer.h) ----------
// *********************************************************************



// *********************************************************************
// ------- Circular Buffer Configuration (nz_circularBuffer.h) ---------
// *********************************************************************
//Specifies what circular buffer is used. Ensure only one of the following is uncommented. If none selected, nz_circularBufferStd is used
#define    CIRBUF_USE_CIRCULAR_BUFFER_STD      //Use nz_circularBufferStd
//#define    CIRBUF_USE_CIRCULAR_BUFFER_PWR2     //Use nz_circularBufferPwrs

//Indicate if buffer should support large packet types. That is, a packet with a size larger than 255 bytes.
//#define    CIRBUF_DISABLE_LARGE_PACKET



// *********************************************************************
// --------------- Debug Configuration (nz_debug.h) --------------------
// *********************************************************************
#define NZ_USBHID_DEBUG_ENABLE
//#define NZ_USBCDC_DEBUG_ENABLE
//#define NZ_UART1_DEBUG_ENABLE

//Uncomment this line to disable all debugging!
//#define DEBUG_LEVEL_ALLOFF

//To enable debug configuration for additional modules, add line to each of the 3 sections below with name of new module. For example in first section, add "#define DEBUG_CONF_NEWMOD 0"
#if defined (DEBUG_LEVEL_ALLOFF)
    #define DEBUG_CONF_MAIN                     0
#else
    #if defined (RELEASE_BUILD)
        #define DEBUG_CONF_MAIN                     DEBUG_LEVEL_WARNING
    #else
        #define DEBUG_CONF_MAIN                     DEBUG_LEVEL_INFO
    #endif
#endif



// *********************************************************************
// -------------- USB Configuration (nz_serUSB.h) -----------------
// *********************************************************************
#define NZ_USBHID_ENABLE

//If set to 1, the system LED is controlled by the nz_serUSB module
#define USB_CONTROLS_SYSLED						( 1 )


#endif  //_PROJDEFS_H_
