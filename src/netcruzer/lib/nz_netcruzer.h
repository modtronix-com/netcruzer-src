/**
 * @brief           Main Netcruzer System file
 * @file            nz_netcruzer.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_netcruzer_desc Description
 *****************************************
 * This module contains Netcruzer System code.
 *
 * @subsection nz_netcruzer_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ---------- Netcruzer System Configuration (nz_netcruzer.h) ----------
// *********************************************************************
//The following are NOT defined by default. Defining them will prevent the system from managing the named resources
//#define     NZSYS_DONT_MANAGE_GLOBALS       //Don't call nzSysGlobalInit() function
//#define     NZSYS_DONT_MANAGE_MAINLOOP      //Don't create the mainLoop() function
//#define     NZSYS_DONT_MANAGE_SERPORT       //Don't manage nz_serDataPorts init() and task() functions
//#define     NZSYS_DONT_MANAGE_TICK          //Don't include nz_tick h & c files AND don't manage it(init, task)
//#define     NZSYS_DONT_MANAGE_WDT           //Don't manage the Watch Dog Timer
//#define     NZ_ADC_DISABLE                  //Don't manage ADC - is enabled by default
//#define     NZ_PWM_DISABLE                  //Don't manage PWM - is enabled by default
//#define     NZ_RTC_ENABLE                   //Enable RTC - is disabled by default

 #define     NZ_APP_CONFIG_XEE_ENABLED      //Enable NZ_APP_CONFIG_XEE

//#define    NZSYS_NO_SERI2C_H_FILES         //Don't include nz_serI2C.h in nz_netcruzer.h

//#define    NZSYS_CALL_INITIALIZE_BOARD     //The initializeBoard() function will be called during Netcruzer initialization
//#define    NZSYS_CALL_APPCONF_FUNCTIONS    //The cfgInit() and appConfInit() function will be called during Netcruzer initialization

//System will NOT initialize and manuage Debugging. Is only done if a DEBUG_USE_XXX define is found (HAS_USBHID_DEBUGGING for example)
//#define     NZSYS_DONT_MANAGE_DEBUG

//#define     NZSYS_DISABLE_DEFAULT_DEBUG     //Disable the "nz_debugDefault.c" file
//#define     NZSYS_DISABLE_DEFAULT_SERUSB    //Disable the "nz_serUSB.c" file


//Indicates that *.c files should not be included from nz_netcruzer.c. Including *.c files simplifies a MPLAB project, seeing
//that all *.c files included in nz_netcruzer.c do not have to be added to the MPLAB project.
//#define     NZSYS_DONT_INCLUDE_C_FILES

//System will add and manage LCD2S and I2C 1 bus (400kbit/sec). We don't have to add any files to project.
//#define     NZSYS_ENABLE_LCD2S_AND_I2C1

//Default size of RAM User Memory. Default is 256, set to 0 to disable all "User Memory" functions
#define     USER_RAM_SIZE                       ( 256 )

//Must set "nzGlobals.wdtFlags.bits.mainApp" flag each 8 seconds to prevent WDT reset
#define     NZSYS_WDT_CHECK_MAINAPP_FLAG        ( 1 )

 @endcode
 **********************************************************************
 * @section nz_netcruzer_lic Software License Agreement
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
 * 2012-08-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NETCRUZER_H
#define NETCRUZER_H

#if (defined(__PIC24F__)) && defined(__C30__)	// Microchip C30 compiler
    #include "PPS.h"
#elif defined(__dsPIC33F__) && defined(__C30__)	// Microchip C30 compiler
#elif defined(__PIC32MX__)                      // Microchip C32 compiler
#else
	#error Unknown processor or compiler.  See Compiler.h
#endif


/////////////////////////////////////////////////
//Default Defines before any include file
#if !defined(NZ_ADC_DISABLE) && !defined(HAS_NZ_ADC)
    #define HAS_NZ_ADC
#endif
#if defined(NZ_RTC_ENABLE) && !defined(HAS_NZ_RTC)
    #define HAS_NZ_RTC
#endif
#if !defined(NZ_PWM_DISABLE) && !defined(HAS_NZ_PWM)
    #define HAS_NZ_PWM
#endif

//UART
#if defined(NZ_UART1_ENABLE) && !defined(HAS_SERPORT_UART1)
    #define HAS_SERPORT_UART1
#endif
#if defined(NZ_UART2_ENABLE) && !defined(HAS_SERPORT_UART2)
    #define HAS_SERPORT_UART2
#endif
#if defined(NZ_UART3_ENABLE) && !defined(HAS_SERPORT_UART3)
    #define HAS_SERPORT_UART3
#endif
#if defined(NZ_UART4_ENABLE) && !defined(HAS_SERPORT_UART4)
    #define HAS_SERPORT_UART4
#endif
#if defined(HAS_SERPORT_UART1) || defined(HAS_SERPORT_UART2) || defined(HAS_SERPORT_UART3) || defined(HAS_SERPORT_UART4)
    #define HAS_SERPORT_UART
#endif

//I2C
#if defined(NZ_I2C1_ENABLE) && !defined(HAS_SERPORT_I2C1)
    #define HAS_SERPORT_I2C1
#endif
#if defined(NZ_I2C2_ENABLE) && !defined(HAS_SERPORT_I2C2)
    #define HAS_SERPORT_I2C2
#endif
#if defined(NZ_I2C3_ENABLE) && !defined(HAS_SERPORT_I2C3)
    #define HAS_SERPORT_I2C3
#endif
#if (defined(HAS_SERPORT_I2C1) || defined(HAS_SERPORT_I2C2) || defined(HAS_SERPORT_I2C3))
    #define HAS_SERPORT_I2C
#endif

//SPI
#if defined(NZ_SPI1_ENABLE) && !defined(HAS_SERPORT_SPI1)
    #define HAS_SERPORT_SPI1
#endif
#if defined(NZ_SPI2_ENABLE) && !defined(HAS_SERPORT_SPI2)
    #define HAS_SERPORT_SPI2
#endif
#if defined(NZ_SPI3_ENABLE) && !defined(HAS_SERPORT_SPI3)
    #define HAS_SERPORT_SPI3
#endif
#if (defined(HAS_SERPORT_SPI1) || defined(HAS_SERPORT_SPI2) || defined(HAS_SERPORT_SPI3))
    #define HAS_SERPORT_SPI
#endif



/////////////////////////////////////////////////
//Include Files

//Basic includes required for all projects (38 seconds)
#include "nz_tick.h"
#include "nz_helpers.h"
#include "nz_ioPortDefs.h"
#include "nz_ioPorts.h"

//Netcruzer RTOS includes
#if (nzosENABLE==1)
#include "../rtos/nz_rtos.h"
#include "../rtos/nzos_main.h"
#endif


#if !defined(__INLINE_FUNCTION__)
#define __INLINE_FUNCTION__ extern inline __attribute__((always_inline))
#endif


/////////////////////////////////////////////////
//Default Defines
#ifndef DEBUG_CONF_DEFAULT
	#ifdef DEBUG_LEVEL_ALLOFF
		#define DEBUG_CONF_DEFAULT	DEBUG_LEVEL_OFF
	#else
		#define DEBUG_CONF_DEFAULT	DEBUG_LEVEL_ERROR
	#endif
#endif

#ifndef NZ_MALLOC
    #define NZ_MALLOC(size) malloc(size)
#endif

#if !defined(USER_RAM_SIZE)
//Default size of RAM User Memory
#define USER_RAM_SIZE 256
#endif

#define MAIN_LOOP_TOP()                 \
    static BOOL inMainLoop=0;           \
    nzSysTaskDefault();                 \
    if (inMainLoop == TRUE) return;     \
    inMainLoop = TRUE;

#define MAIN_LOOP_BOTTOM()              \
    inMainLoop = FALSE;


/////////////////////////////////////////////////
//Global variables
#define BUFFER_DUMMY_SIZE           2
#define NZ_GLOBALS_SERPORT_DEBUG    nzGlobals.serDebug  //Pointer to first SERPORT object
#define NZ_GLOBALS_SERPORT_FIRST    nzGlobals.serUart1   //Pointer to first SERPORT object, EXCLUDING Debug serport!
#define NZ_GLOBALS_SERPORT_LAST     nzGlobals.serUser8  //Pointer to last SERPORT object
#define SERPORT_COUNT (NZ_GLOBALS_SERPORT_LAST - NZ_GLOBALS_SERPORT_FIRST)

/**
 * Structure containing information on board status
 */
typedef struct __attribute__((aligned(2), packed))
{
    union {
        struct
        {
            unsigned int bPowerFail : 1;   //If set, we are in power fail mode. Panic!!!
            unsigned int bFill : 15;       //Fill to ensure flags take up a 16-bit WORD
        } flags;
        WORD flagsVal;
    };

    WORD        testW;
} BOARD_STATUS;

typedef struct __attribute__((__packed__)) WDT_FLAGS_
{
    union {
        struct
        {
            unsigned int main : 1;
            unsigned int mainApp : 1;
            unsigned int stackTask : 1;
            unsigned int serUART : 1;
            unsigned int serSPI : 1;
            unsigned int serI2C : 1;
            unsigned int serUDP : 1;
            unsigned int serTCP : 1;
            unsigned int serUSB : 1;
            unsigned int fl9 : 1;
            unsigned int fl10 : 1;
            unsigned int fl11 : 1;
            unsigned int fl12 : 1;
            unsigned int fl13 : 1;
            unsigned int fl14 : 1;
            unsigned int fl15 : 1;

            unsigned int app0 : 1;
            unsigned int app1 : 1;
            unsigned int app2 : 1;
            unsigned int app3 : 1;
            unsigned int app4 : 1;
            unsigned int app5 : 1;
            unsigned int app6 : 1;
            unsigned int app7 : 1;
            unsigned int app8 : 1;
            unsigned int app9 : 1;
            unsigned int app10 : 1;
            unsigned int app11 : 1;
            unsigned int app12 : 1;
            unsigned int app13 : 1;
            unsigned int app14 : 1;
            unsigned int app15 : 1;
        } bits;
        DWORD Val;
    };
} WDT_FLAGS;



/**
 * Global defines, located at 0x1000 (4096) in RAM
 */
typedef struct __attribute__((__packed__)) _GLOBAL_DEFS
{
    BOARD_STATUS    brdStatus;
    WDT_FLAGS       wdtFlags;

    WORD            userRamSize;
    BYTE            userRam[USER_RAM_SIZE];


    void*           serDebug;

    //Pointers to "Serial Data Ports" (SERPORT structures). Pointer is NULL if that "Serial Data Port"
    //does not exist.
    //!!! IMPORTANT !!! Must have same order as "Serial Data Port" defined in SER_AVAIL
    void* serUart1;  ///< Pointer to SERPORT
    void* serUart2;  ///< Pointer to SERPORT
    void* serUart3;  ///< Pointer to SERPORT
    void* serUart4;  ///< Pointer to SERPORT
    void* serSPI1;  ///< Pointer to SERPORT
    void* serSPI3;  ///< Pointer to SERPORT
    void* serI2C1;  ///< Pointer to SERPORT
    void* serI2C2;  ///< Pointer to SERPORT

    void* serI2C3;  ///< Pointer to SERPORT
    void* serUDP1;  ///< Pointer to SERPORT
    void* serUDP2;  ///< Pointer to SERPORT
    void* serUDP3;  ///< Pointer to SERPORT
    void* serTCP1;  ///< Pointer to SERPORT
    void* serTCP2;  ///< Pointer to SERPORT
    void* serTCP3;  ///< Pointer to SERPORT
    void* serUSB;   ///< Pointer to SERPORT

    void* serUser1; ///< Pointer to SERPORT
    void* serUser2; ///< Pointer to SERPORT
    void* serUser3; ///< Pointer to SERPORT
    void* serUser4; ///< Pointer to SERPORT
    void* serUser5; ///< Pointer to SERPORT
    void* serUser6; ///< Pointer to SERPORT
    void* serUser7; ///< Pointer to SERPORT
    void* serUser8; ///< Pointer to SERPORT
} GLOBAL_DEFS;


#ifndef THIS_IS_NZ_NETCRUZER_C
    extern GLOBAL_DEFS      nzGlobals;
#endif



/////////////////////////////////////////////////
//The following defines will be included with all files!
#if !defined(NZSYS_DONT_MANAGE_TICK)
#if defined(NZSYS_ENABLE_COMPLEX_TICK)
    #include "nz_tickCx.h"  //In addition to nz_tick.h, also include nz_tickCx.h
#else
    #define NZSYS_ENABLE_TICK
#endif
#endif  //#ifndef NZSYS_DONT_MANAGE_TICK

//LCD2S !!!!! This is broken, fix !!!!!
//#ifdef NZSYS_ENABLE_LCD2S_AND_I2C1
//#include "nz_i2c1.h"
//#include "nz_lcd2s.h"
//#endif


//USB "Serial Data Port"
#if defined(NZ_USBHID_ENABLE)
    #if !defined(HAS_SERPORT_USB_HID)
        #define HAS_SERPORT_USB_HID
    #endif
    #if !defined(USB_IS_HID)
        #define USB_IS_HID
    #endif    
#endif
#if defined(NZ_USBCDC_ENABLE)
    #if !defined(HAS_SERPORT_USB_CDC)
        #define HAS_SERPORT_USB_CDC
    #endif
    #if !defined(USB_IS_CDC)
        #define USB_IS_CDC
    #endif
#endif
#if defined(NZ_USBMIDI_ENABLE)
    #if !defined(HAS_SERPORT_USB_MIDI)
        #define HAS_SERPORT_USB_MIDI
    #endif
    #if !defined(USB_IS_MIDI)
        #define USB_IS_MIDI
    #endif
#endif
#if defined(HAS_SERPORT_USB_HID) || defined(HAS_SERPORT_USB_CDC) || defined(HAS_SERPORT_USB_MIDI)
    #if !defined(HAS_A_SERPORT)
        #define HAS_A_SERPORT
    #endif
    #if !defined(HAS_SERPORT_USB)
        #define HAS_SERPORT_USB
    #endif
#endif



/////////////////////////////////////////////////
//Debug - Includes required for USB
//If a known debugger is defined, define HAS_NZ_DEBUGGING. If a unknown debugger is added by
//application, then HAS_NZ_DEBUGGING should be defined by application in projdefs.h!
#if defined(NZ_USBHID_DEBUG_ENABLE) && !defined(HAS_USBHID_DEBUGGING)
    #define HAS_USBHID_DEBUGGING
#endif
#if defined(NZ_USBCDC_DEBUG_ENABLE) && !defined(HAS_USBCDC_DEBUGGING)
    #define HAS_USBCDC_DEBUGGING
#endif
#if defined(NZ_UART1_DEBUG_ENABLE) && !defined(HAS_UART1_DEBUGGING)
    #define HAS_UART1_DEBUGGING
#endif
#if defined(NZ_USBHID_DEBUG_ENABLE) || defined(NZ_USBCDC_DEBUG_ENABLE) || defined(NZ_UART1_DEBUG_ENABLE)
    #define HAS_NZ_DEBUGGING
#endif
#if !defined(NZSYS_DONT_MANAGE_DEBUG) && defined(HAS_NZ_DEBUGGING)
    #define NZSYS_MANAGE_DEBUG
#endif



/////////////////////////////////////////////////
//Other includes
#include "nz_analog.h"
#include "nz_pwm.h"



/////////////////////////////////////////////////
//Functions prototypes
void mainLoop(void);

#if defined(NZSYS_CALL_INITIALIZE_BOARD)
void initializeBoard(void);
#endif

#if defined(NZSYS_CALL_APPCONF_FUNCTIONS)
void cfgInit(BYTE* pCfgStruct);
void appConfInit(void);
#endif


/**
 * Default initialization. Does following:
 * - Configures all ports as Digital Inputs
 * - Disables all analog port functions
 * - Configures Tick to use Timer 3, and for 1ms
 */
void nzSysInitDefault(void);

/**
 * Default Task, must be called in main loop of function program that calls nzSysInitDefault(). Does following:
 * - Calls LCD2S task if required
 */
void nzSysTaskDefault(void);

/**
 * Initialize global data
 */
void nzSysGlobalInit(void);


/**
 * User Ram WORD. Use to get or set User Ram WORD.
 *
 * For example, to get the WORD at User Ram 0x42:
 * WORD val;
 * val = USER_RAM_WORD(0x42);
 *
 * For example, to set the WORD at User Ram 0xC2 to 0x0A50:
 * USER_RAM_WORD(0xC2) = 0xA50;
 */
#define USER_RAM_WORD(adr) (*((WORD*)(&nzGlobals.userRam[adr])))

/**
 * User Ram BYTE. Use to get or set User Ram BYTE.
 *
 * For example, to get the BYTE at User Ram 0x42:
 * BYTE val;
 * val = USER_RAM_BYTE(0x42);
 *
 * For example, to set the BYTE at User Ram 0xC2 to 0x0A:
 * USER_RAM_BYTE(0xC2) = 0x0A;
 */
#define USER_RAM_BYTE(adr) (nzGlobals.userRam[adr])


#if !defined(DOXY_FUNC) //Don't document in doxygen
/** DEPRECATED!!! Use nzSysInitDefault() in stead! */
#define nzsysInitDefault() nzSysInitDefault()
/** DEPRECATED!!! Use nzSysTaskDefault() in stead! */
#define nzsysTaskDefault() nzSysTaskDefault()
/** DEPRECATED!!! Use nzSysGlobalInit() in stead! */
#define nzsysGlobalInit()  nzSysGlobalInit()
#endif


#endif  //#ifndef NETCRUZER_H
