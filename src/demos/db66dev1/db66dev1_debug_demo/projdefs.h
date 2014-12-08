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
 // ------------ ADC Filtered Configuration (from nz_analog.h) -------------
 // *********************************************************************
 //If following line is included in projdefs.h, 3.3V supply is used for reference. Default is 2.5V external precision reference.
 #define ADC_3V3_VREF                       //Use internal 3.3V supply as reference

 //Number of filter stages for each ADC channel. Must be 2, 4 or 8. For each ADC channel, this many
 //words are needed. Total required memory is ADC_MAX_CHAN x ADC_FILTERS words.
 #define ADC_FILTER_STAGES  8



// *********************************************************************
// ------- Circular Buffer Configuration (nz_circularBuffer.h) ---------
// *********************************************************************
//Specifies what circular buffer is used. Ensure only one of the following is uncommented. If none selected, nz_circularBufferStd is used
#define    CIRBUF_USE_CIRCULAR_BUFFER_STD      //Use nz_circularBufferStd
//#define    CIRBUF_USE_CIRCULAR_BUFFER_PWR2     //Use nz_circularBufferPwrs


// *********************************************************************
// --------------- Debug Configuration (nz_debug.h) --------------------
// *********************************************************************
#define DEBUG_USE_USBHID
//#define DEBUG_USE_USBCDC
//#define DEBUG_USE_UART1

//Size of Debug TX buffer, MUST BE power of 2 value! Increase size of this buffer if debug information
//is getting lost. This can be the case if the application writes debug information to the debug buffer
//faster than it can be outputted.
#define DEBUG_TXBUF_SIZE 1024

//Size of Debug RX buffer, MUST BE power of 2 value! Ensure it is large enought to hold largest string
//written to debug port. For example, when using the USB port for debugging, text message are sent to
//the debug port from the @ref devtools_nzUsbTerminal.
#define DEBUG_RXBUF_SIZE 64

//Uncomment this line to disable all debugging!
//#define DEBUG_LEVEL_ALLOFF

//To enable debug configuration for additional modules, add line to each of the 3 sections below with name of new module. For example in first section, add "#define DEBUG_CONF_NEWMOD 0"
#if defined (DEBUG_LEVEL_ALLOFF)
    #define DEBUG_CONF_MAIN                     0
    #define DEBUG_CONF_CIRCULAR_BUFFER          0
    #define DEBUG_CONF_USB_BUFFERED             0
#else
    #if defined (RELEASE_BUILD)
        #define DEBUG_CONF_MAIN                     DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_CIRCULAR_BUFFER          DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_USB_BUFFERED             DEBUG_LEVEL_WARNING
    #else
        #define DEBUG_CONF_MAIN                     DEBUG_LEVEL_INFO
        #define DEBUG_CONF_CIRCULAR_BUFFER          DEBUG_LEVEL_INFO
        #define DEBUG_CONF_USB_BUFFERED             DEBUG_LEVEL_INFO
    #endif
#endif



// *********************************************************************
// -------------- USB Configuration (nz_serUSB.h) -----------------
// *********************************************************************
#define HAS_SERPORT_USB_HID

//If set to 1, the system LED is controlled by the nz_serUSB module
#define USB_CONTROLS_SYSLED						( 1 )

//Indicates if this application handles commands via USB. When defined, the application must implement the "nz_cmdXxx" callback functions!
//#define APP_HANDLES_USB_COMMANDS


#endif  //_PROJDEFS_H_
