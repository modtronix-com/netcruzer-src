/**
 * @brief           Main Netcruzer System file
 * @file            nz_netcruzer.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
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
 * 2012-08-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_NETCRUZER_C

#define INCLUDE_NETCRUZER_HEADERS
#include "HardwareProfile.h"
#include "nz_netcruzer.h"

#if !defined(NZSYS_DISABLE_STDIO_WRITE)
#include "nz_debug.h"
#include "./simio.h"
#endif

//Debounce
#include "nz_debounce.h"

//RTC
#if defined(HAS_NZ_RTC)
#include "nz_rtc.h"
#endif

//I2C includes
#if (defined(HAS_SERPORT_I2C1) || defined(HAS_SERPORT_I2C2) || defined(HAS_SERPORT_I2C3)) && !defined(NZSYS_NO_SERI2C_H_FILES)
    #include "nz_serI2C.h"
#endif

//USB includes
#if defined(HAS_SERPORT_USB)
    #include "USB\usb.h"
    #if defined(USB_IS_CDC)
        #include "USB\usb_function_cdc.h"
    #elif defined(USB_IS_HIH)
        #include "USB\usb_function_hid.h"
        #include "nz_serI2C.h"
    #endif
#endif

//Add debugging to this file. The DEBUG_LEVEL_ALLOFF macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if defined (DEBUG_LEVEL_ALLOFF)
    #define MY_DEBUG_LEVEL      0
#else
    #define MY_DEBUG_LEVEL      DEBUG_LEVEL_WARNING
#endif
#include "nz_debug.h"



/////////////////////////////////////////////////
//Include c files. This is the only place in the code that *.c files are included. Generally a bad
//practice, but simplifies project structures. No need to include all *.c files in MPLAB project
#ifndef NZSYS_DONT_INCLUDE_C_FILES
    #ifndef NZSYS_DONT_MANAGE_TICK
        #ifdef  NZSYS_ENABLE_COMPLEX_TICK
            #include "nz_tickCx.c"
        #else
            #include "nz_tick.c"
        #endif
    #endif  //#ifndef NZSYS_DONT_MANAGE_TICK

#endif  //#ifndef NZSYS_DONT_INCLUDE_C_FILES


//Defines ///////////////////////////////////////
#ifndef NZSYS_WDT_CHECK_MAINAPP_FLAG
#define NZSYS_WDT_CHECK_MAINAPP_FLAG (0)
#endif

#ifndef NETCRUZER_LCD2S_INIT_DELAY
#define NETCRUZER_LCD2S_INIT_DELAY  300
#endif

#ifndef NETCRUZER_SYSTEM_TASK_TIME
#define NETCRUZER_SYSTEM_TASK_TIME  1   //1 ms
#endif

typedef struct __attribute__((__packed__)) NZSYSTASK_INTASK_
{
    union {
        struct
        {
            unsigned int debugService : 1;
            unsigned int lcd2sTask : 1;
            unsigned int USBDeviceTasks : 1;
            unsigned int USBDeviceAttach : 1;
        } bits;
        WORD Val;
    };
} NZSYSTASK_INTASK;


// Variables ////////////////////////////////////
static NZSYSTASK_INTASK inTask;
static WORD tmrNzTask = 0;  //Netcruzer System Task
static BYTE taskCount10ms = 9;
static BYTE taskCount100ms = 99;


/**
 * Global Defined structure, located at 0x1000 in RAM
 */
#if defined(__C30__)
GLOBAL_DEFS __attribute__ ((address(0x1000))) nzGlobals;
#elif defined(__PIC32MX__)
GLOBAL_DEFS nzGlobals;
#endif



/**
 * Default initialization. Does following:
 * - Configures all ports as Digital Inputs
 * - Disables all analog port functions
 * - Initializes tick (default Tick uses Timer 3, and for 1ms).
 * - Initialize the Netcruzer RTOS if enabled
 */
void nzSysInitDefault(void)
{
    inTask.Val = 0;

    PPSUnLock;  // Unlock PPS

    #if defined(__PIC24FJ256GB210__)
        ANSA = 0x0000;
        ANSB = 0x0000;
        ANSC = 0x0000;
        ANSD = 0x0000;
        ANSE = 0x0000;
        ANSF = 0x0000;
        ANSG = 0x0000;
    #elif defined(__PIC24FJ256GB206__)
        ANSB = 0x0000;  //Port B0 to B15 are digital I/O ports
        ANSC = 0x0000;  //Port C13 and C14 are digital I/O ports
        ANSD = 0x0000;  //Port D6 and D7 are digital I/O ports
        ANSF = 0x0000;  //Port F0 is a digital I/O port
        ANSG = 0x0000;  //Port G6 to G9 are digital I/O ports
    #else
        #if defined(__C30__)
        AD1PCFG=0xffff;    //Configure I/O ports as digital
        #elif defined(__PIC32MX__)

        #endif
    #endif

    //Debugging must be initialized right at the start. At this stage, no port is required! Will write
    //add debug into to Circular Buffer, that is written out on port later on when port get's initialized.
    #if defined(NZSYS_MANAGE_DEBUG)
        debugInit();
    #endif

//    //Debugging must be initialized AS SOON AS port used by debugging has been initialized!
//    //USB HID was just inititialized above. If debugging uses USB HID, call debugInit() now
//    #if !defined(NZSYS_DEBUG_INIT_CALLED) && defined(NZSYS_MANAGE_DEBUG) && defined(HAS_USBHID_DEBUGGING)
//        #define NZSYS_DEBUG_INIT_CALLED     //Mark debugInit() has already been called
//        debugInit();
//    #endif
//
//    /////////////////////////////////////////////////
//    //LCD2S, and Debug if it uses LCD2S
//    #ifdef NZSYS_ENABLE_LCD2S_AND_I2C1
//        //Open I2C 1 port at 400kbit/second. I2C 1 is used for the LCD display (red micro match connector) on all Netcruzer SBC66 board.
//        i2c1Open(I2C_BAUD_400K);
//
//        //Initialize LCD2S display. To customize initialization, add defines from lcs2s.h (in header) to projdefs.h
//        //file. For details, see @ref lcd2s_conf. Add 300ms delay for LCD2S display to intialize itself
//        delay_ms(NETCRUZER_LCD2S_INIT_DELAY);
//        lcdInit();
//    #endif
//
//    //Debugging must be initialized AS SOON AS port used by debugging has been initialized!
//    //LCD2S was just inititialized above. If debugging uses LCD2S, call debugInit() now
//    #if !defined(NZSYS_DEBUG_INIT_CALLED) && defined(NZSYS_MANAGE_DEBUG) && defined(DEBUG_USE_LCD2S)
//        #define NZSYS_DEBUG_INIT_CALLED     //Mark debugInit() has already been called
//        debugInit();
//    #endif

    #ifndef NZSYS_DONT_MANAGE_GLOBALS
        //Global Init, must be done second
        nzSysGlobalInit();
    #endif

    //Initialize Tick Library, regardless if NZSYS_DONT_MANAGE_TICK is defined or not.
    //If NZSYS_DONT_MANAGE_TICK is defined, we don't include the c implementation file on top
    tickInit();
    tmrNzTask = tick16Get() + tick16ConvertFromMS(600); //Startup delay of 600mS
    

    //Call after Debug, Global and Tick initialization
    #if defined(NZSYS_CALL_INITIALIZE_BOARD)
    initializeBoard();      //Initialize application specific hardware
    #endif

    #if defined(NZSYS_CALL_APPCONF_FUNCTIONS)
    extern BYTE* cfgStruct;
    cfgInit((BYTE*)&cfgStruct); //Initialize Application Configuration
    appConfInit();          //Initialize AppConfig with values read from EEPROM
    #endif

    /////////////////////////////////////////////////
    //USB, and Debug if it uses USB
//    #if defined(HAS_SERPORT_USB)
//    usbInit();
//    #endif

    #if !defined(NZSYS_DONT_MANAGE_SERPORT) && defined(HAS_A_SERPORT)
    serInit();
    #endif

    #if (nzosENABLE==1)
    nzRtosInit(NZ_UTICKS_PER_MS);
    #endif

    //Call at end of initialization
    #if defined(NZSYS_MANAGE_DEBUG)
    debugAllInitDone();
    #endif

    //Initialize RTC module
    #if defined(HAS_NZ_RTC)
    rtcInit();
    #endif

    //Initialize Debounce module
    #if defined(NZ_DEBOUNCE_ENABLED)
    debounceInit();
    #endif

    //Check if a Watchdog timer reset occurred
    if (_WDTO) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nWDT Reset Occurred! (0x");
        DEBUG_PUT_HEXWORD(DEBUG_LEVEL_ERROR, DWORD_HIGH_WORD(nzGlobals.wdtFlags.Val));
        DEBUG_PUT_HEXWORD(DEBUG_LEVEL_ERROR, DWORD_LOW_WORD(nzGlobals.wdtFlags.Val));
    }

    //Enable Watchdog timer
    #if !defined(NZSYS_DONT_MANAGE_WDT)
    _SWDTEN = 1;
    #endif
}

/**
 * Default Task, must be called in main loop of function program that calls nzSysInitDefault(). Does following:
 * - Calls LCD2S task if required
 */
void nzSysTaskDefault(void)
{
    #if !defined(NZSYS_DONT_MANAGE_WDT)
        #if (NZSYS_WDT_CHECK_MAINAPP_FLAG == 1)
        if (nzGlobals.wdtFlags.bits.mainApp
        #else
        if (1
        #endif
            #if (NZSYS_WDT_CHECK_MAIN_FLAG == 1)
                && nzGlobals.wdtFlags.bits.main
            #endif
            #if defined(HAS_SERPORT_USB)
                && nzGlobals.wdtFlags.bits.serUSB
            #endif
            #if defined(HAS_SERPORT_I2C)
                && nzGlobals.wdtFlags.bits.serI2C
            #endif
            #if defined(HAS_SERPORT_SPI)
                && nzGlobals.wdtFlags.bits.serSPI
            #endif
            #if defined(HAS_SERPORT_UART)
                && nzGlobals.wdtFlags.bits.serUART
            #endif
    ) {
        nzGlobals.wdtFlags.Val = 0;
        ClrWdt();
    }
    #endif

    //Enter every NETCRUZER_SYSTEM_TASK_TIME
    if (tick16TestTmr(tmrNzTask)) {
        tick16UpdateTmrMS(tmrNzTask, NETCRUZER_SYSTEM_TASK_TIME); //Update timer to expire in 1ms (NETCRUZER_SYSTEM_TASK_TIME)

        // Every 10ms ///////////////////////////////////
        if(taskCount10ms-- == 0) {
            taskCount10ms=9;

            #if defined(HAS_SERPORT_USB_HID)
                #if defined(USB_INTERRUPT)
                if (USB_BUS_SENSE && (USBGetDeviceState() == DETACHED_STATE)) {
                    if (inTask.bits.USBDeviceAttach == FALSE) {
                        inTask.bits.USBDeviceAttach = TRUE;
                        USBDeviceAttach();
                        inTask.bits.USBDeviceAttach = FALSE;
                    }
                }
                #endif
            #endif

            // Every 20ms ///////////////////////////////////
            if (taskCount10ms&0x01) {
                #if defined(NZ_DEBOUNCE_ENABLED)
                    #if (DEBOUNCE_SERVICE_TIME!=20)
                    #error "Debounce service time not supported!"
                    #endif
                debounceService();
                #endif
            }
            /////////////////////////////////////////////////
            // Every other 20ms /////////////////////////////
            else {
			}

            // Every 100ms //////////////////////////////////
            if(taskCount100ms-- == 0) {
                taskCount100ms=9;

            }
        }

        /////////////////////////////////////////////////
        // Every 2ms ///////////////////////////////////
        if( (taskCount10ms & 0x01)==0) {
            //Debugging
            #if defined(NZSYS_MANAGE_DEBUG)
            if (inTask.bits.debugService == FALSE) {
                inTask.bits.debugService = TRUE;
                debugService();
                inTask.bits.debugService = FALSE;
            }
            #endif
        }
        /////////////////////////////////////////////////
        // Every other 2ms //////////////////////////////
        //else {
        //}

        // Every 1ms ////////////////////////////////////
    }


    #if !defined(NZSYS_DONT_MANAGE_SERPORT) && defined(HAS_A_SERPORT)
    serTask();
    #endif

//  !!!!! This is broken, fix !!!!!    
//    #ifdef NZSYS_ENABLE_LCD2S_AND_I2C1
//    if (inTask.bits.lcd2sTask == FALSE) {
//        inTask.bits.lcd2sTask = TRUE;
//        lcd2sTask();
//        inTask.bits.lcd2sTask = FALSE;
//    }
//    #endif

    #if defined(HAS_SERPORT_USB_HID)
        #if defined(USB_POLLING)
        // Check bus status and service USB interrupts.
        if (inTask.bits.USBDeviceTasks == FALSE) {
            inTask.bits.USBDeviceTasks = TRUE;
            USBDeviceTasks(); // Interrupt or polling method.  If using polling, must call
            inTask.bits.USBDeviceTasks = FALSE;
        }
        // this function periodically.  This function will take care
        // of processing and responding to SETUP transactions
        // (such as during the enumeration process when you first
        // plug in).  USB hosts require that USB devices should accept
        // and process SETUP packets in a timely fashion.  Therefore,
        // when using polling, this function should be called
        // regularly (such as once every 1.8ms or faster** [see
        // inline code comments in usb_device.c for explanation when
        // "or faster" applies])  In most cases, the USBDeviceTasks()
        // function does not take very long to execute (ex: <100
        // instruction cycles) before it returns.
        #endif
    #endif
}


/**
 * Initialize global data
 */
void nzSysGlobalInit(void) {
    DWORD dw;

    //DON't reset wdtFlags
    dw = nzGlobals.wdtFlags.Val;

    //Initialize structure with 0
    memset(&nzGlobals, 0, sizeof(nzGlobals));

    //Restore wdtFlags - is read later during initialization!
    nzGlobals.wdtFlags.Val = dw;

    //Initialize size of "User Ram"
    nzGlobals.userRamSize = USER_RAM_SIZE;
}


//If project doesn't declare a mainLoop() function, create a blank one here
#if !defined(NZSYS_DONT_MANAGE_MAINLOOP)
void mainLoop(void) {
    nzSysTaskDefault();
}
#endif

#if defined(__C30__)

#elif defined(__PIC32MX__)

#endif

#if !defined(NZSYS_DISABLE_STDIO_WRITE)
#if defined(__C30__)
int __attribute__((__weak__, __section__(".libc")))
write(int handle, void *buffer, unsigned int len) {
    #if !defined(NZSYS_DISABLE_STDIO_WRITE_USE_DEBUG) && defined(HAS_NZ_DEBUGGING)
    int i;
    #endif

    switch (handle) {
        case 0:
        case 1:
        case 2:
            #if !defined(NZSYS_DISABLE_STDIO_WRITE_USE_DEBUG) && defined(HAS_NZ_DEBUGGING)
            for (i = len; i; --i) {
                //If USB is used for Debugging, call usbTask() if TX buffer is full.
                #if defined(HAS_USBHID_DEBUGGING) || defined(HAS_USBCDC_DEBUGGING)

                if(serUSBIsSuspended() != TRUE) {
					//Wait for 4 bytes to become available
					while (cbufGetFree(CIRBUF_TX_DEBUG) < 4) {
                        //DJH - 2014059 Added this code - do more testing!
                        if((USBDeviceState < CONFIGURED_STATE) || USBIsDeviceSuspended())
                            break;
						serUSBTask();
					}
				}
                #endif
                cbufPutByteNoCheck(CIRBUF_TX_DEBUG, *(BYTE*)buffer++);
            }
            #else
            //#error "No implementation provided for stdio write()!"
            #endif
            break;
        default:
        {
            SIMIO simio;
            register PSIMIO psimio asm("w0") = &simio;

            simio.method = SIM_WRITE;
            simio.u.write.handle = handle;
            simio.u.write.buffer = buffer;
            simio.u.write.len = len;
            __asm__(".pword 0xDAC000" ::"a" (psimio) : "memory");
            len = simio.u.write.len;
            break;
        }
    }
    return (len);
}
#elif defined(__C32__)

#endif

#endif

