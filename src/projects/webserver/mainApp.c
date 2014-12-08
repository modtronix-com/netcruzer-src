 /**
 * @brief           Main Application code
 * @file            mainApp.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 **********************************************************************
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
 * 2010-11-10, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_WEBSERVER_C
#define THIS_IS_STACK_APPLICATION

//Netcruzer include files
#include "HardwareProfile.h"

//XC16 Compiler and Microchip include files
#include <PPS.h>
#include "USB/usb.h"            //Includes headers for Microchip USB
#include "TCPIP Stack/TCPIP.h"  //Includes headers for Microchip TCPIP Stack

//Remaining defines
#include "mainApp.h"
#include "nz_netcruzer.h"
#include "nz_analog.h"
#include "nz_pt66ei24p.h"

#if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
#include "TCPIP Stack/ZeroconfLinkLocal.h"
#endif
#if defined(STACK_USE_ZEROCONF_MDNS_SD)
#include "TCPIP Stack/ZeroconfMulticastDNS.h"
#endif

//Add debugging to this file. The DEBUG_CONF_MAINAPP macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_MAINAPP)
    #define DEBUG_CONF_MAINAPP        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAINAPP
#include "nz_debug.h"


// Variables  ///////////////////////////////////
typedef struct __attribute__((__packed__)) MAINAPPTASK_INTASK_
{
    union {
        struct
        {
            unsigned int processIoMainApp : 1;
            unsigned int wfUsePowerSaveFunctions : 1;
            unsigned int newIP : 1;
            unsigned int serUsbProcessCmd : 1;
            unsigned int MicrochipDemo : 1;
            unsigned int StackTask : 1;
            unsigned int StackApplications : 1;
            unsigned int ZeroconfLLProcess : 1;
            unsigned int mDNSProcess : 1;
            unsigned int GenericTCPClient : 1;
            unsigned int GenericTCPServer : 1;
            unsigned int SNMP : 1;
        } bits;
        DWORD Val;
    };
} MAINAPPTASK_INTASK;
static MAINAPPTASK_INTASK inTask;

WORD sysLedMask;    //Used for LED flashing pattern. Has 1 set bit, and it rotated left every 50ms.
WORD sysLedPattern; //Used for blinking system LED
WORD tmrStartupDelay;   //Startup delay
WORD tmrMainAppTask = 0;   //Timer definining LED flash rate (16-bit, 1ms Timer)
#if defined(CALCULATE_AN0STRING)
BYTE AN0String[8];
#endif


// Function Prototypes  /////////////////////////
static void processIoMainApp(void);
static void expansionInit(void);
static void LEDTask(void);
void blinkUSBStatus(void);
#if defined(WF_CS_TRIS)
static void WF_Connect(void);
#endif
void debugPrintIP(void);    //Defined in myDebug.h
extern void serUsbProcessCmd(void);


// Defines  /////////////////////////////////////
// Used for Wi-Fi assertions
#define WF_MODULE_NUMBER   WF_MODULE_MAIN_DEMO


/**
 * Interrupt Service Routines
 */
void _ISR __attribute__((__no_auto_psv__)) _AddressError(void) {
    Nop(); Nop();
}


/**
 * Stack Error
 */
void _ISR __attribute__((__no_auto_psv__)) _StackError(void) {
    Nop(); Nop();
}


/**
 * Comparator ISR, triggered by power fail detect
 */
void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void) {
    WORD i;

    // Deassert all chip select lines to save power
    /*
#if defined(SPIRAM_CS_TRIS)
        SPIRAM_CS_IO = 1;
        SPIRAM_CS_TRIS = 0;
#endif
#if defined(SPIFLASH_CS_TRIS)
        SPIFLASH_CS_IO = 1;
        SPIFLASH_CS_TRIS = 0;
#endif
#if defined(WF_CS_TRIS)
        WF_CS_IO = 1;
        WF_CS_TRIS = 0;
#endif
     */

    //Ensure all I/O ports are off, configure them all for input
    /*
    DIR_20 = 1;
    DIR_21 = 1;
    DIR_26 = 1;
    DIR_27 = 1;
    DIR_28 = 1;
    DIR_29 = 1;
    DIR_00 = 1;
    DIR_01 = 1;
    DIR_02 = 1;
    DIR_03 = 1;
    DIR_04 = 1;
    DIR_05 = 1;
    DIR_06 = 1;
    DIR_07 = 1;
    DIR_08 = 1;
    DIR_09 = 1;
    DIR_10 = 1;
    DIR_11 = 1;
    DIR_30 = 1;
    DIR_31 = 1;
    DIR_32 = 1;
    DIR_33 = 1;
    DIR_34 = 1;
    DIR_35 = 1;
    DIR_36 = 1;
    DIR_37 = 1;
    DIR_38 = 1;
    DIR_39 = 1;
    DIR_40 = 1;
    DIR_41 = 1;
     */

    /*
    MACPowerDown();

    //Power down all peripherals
    PMD1 = 0xffff;
    PMD2 = 0xffff;
    PMD3 = 0xffff;
    PMD4 = 0xffff;
    PMD5 = 0xffff;
    PMD6 = 0xffff;


    //mPWRMGNT_GotoSleepMode();
    asm ( " PWRSAV #0 " );
     */

    //Check if COUT is still active
    nzGlobals.brdStatus.flags.bPowerFail = 0;
    if (CM3CONbits.COUT == 1) {
        for (i = 0; i < 100; i++) {
        } //Delay about 400 instructions, for 16MIPs = 25us
        //Check if COUT is still active
        if (CM3CONbits.COUT == 1) {
            //Take action, this is a power failure!!!!!!!!!!
            nzGlobals.brdStatus.flags.bPowerFail = 1;
        }
    }

    _CMIF = 0;              //Clear interrupt flag
    CM3CONbits.CEVT = 0;    //This bit must be cleared to enable more interrupts
}


/**
 * Main Application Initialization
 */
void initMainApp(void) {
    inTask.Val = 0;
    
    #if defined(HAS_SERPORT_USB_CDC)
    char buf[32];
    #endif

    #if !defined(RELEASE_BUILD) && defined(PT66EI_24P_LEDS)
    BYTE leds = 0x01;      // For PT66EI-24PI test
    #endif

    nzSysInitDefault();     //Netcruzer System Init - MUST BE BEFORE initializeBoard()!
    
    //Configure Port 1, 2 and 3 (old port names X1, X2 and X3) as ADC channels
    adcOpen(ADC_OPEN_A1 | ADC_OPEN_A2 | ADC_OPEN_A3);

    expansionInit();        //Initialize any expansion boards used

    #if defined(STACK_USE_MPFS2)
    MPFSInit();
    #endif

    StackInit();            //Initialize core stack layers (MAC, ARP, TCP, UDP) and application modules (HTTP, SNMP, etc.)

    #if defined(WF_CS_TRIS)
    WF_Connect();
    #endif

    #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
    ZeroconfLLInitialize();
    #endif

    #if defined(STACK_USE_ZEROCONF_MDNS_SD)
    mDNSInitialize(MY_DEFAULT_HOST_NAME);
    mDNSServiceRegister(
            (const char *) "DemoWebServer", // base name of the service
            "_http._tcp.local", // type of the service
            80, // TCP or UDP port, at which this service is available
            ((const BYTE *) "path=/index.htm"), // TXT info
            1, // auto rename the service when if needed
            NULL, // no callback function
            NULL // no application context
            );

    mDNSMulticastFilterRegister();
    #endif

    tmrStartupDelay = tick16Get() + tick16ConvertFromMS(500); //Startup delay of 500mS
}


/**
 * Main Application tasks, call in main loop
 */
void mainAppTask(void) {
    static DWORD dwLastIP = 0;
    static BYTE taskCount10ms=9;
    static BYTE taskCount100ms=99;


    //Startup delay of 500ms
    if ((tmrStartupDelay!=-1) && tick16TestTmr(tmrStartupDelay)) {
        tmrStartupDelay = -1;       //Don't trigger this code again! Only a one time start-up event!
        //Remove this when comparitor input has been ported for SBC66ECL too
        #if defined(BRD_SBC66EC_R1) || defined(BRD_SBC66EC_R2)
            CM3CONbits.CEVT = 0;    //This bit must be cleared to enable more interrupts
            _CMIF = 0;              //Clear interrupt flag
            _CMIE = 1;              //Comparator interrupt enable
        #endif
    }
    

    //Main Appliation Task every 1ms
    if (tick16TestTmr(tmrMainAppTask)) {
        tick16UpdateTmrMS(tmrMainAppTask, 1);  //Update timer to expire in 1ms again

        nzGlobals.wdtFlags.bits.mainApp = 1;	//Has to be set each 8 seconds to prevent WDT reset

        /////////////////////////////////////////////////
        // Every 10ms ///////////////////////////////////
        if(taskCount10ms-- == 0) {
            taskCount10ms=9;

            //Process I/O every 10ms
            if (inTask.bits.processIoMainApp == FALSE) {
                inTask.bits.processIoMainApp = TRUE;
                processIoMainApp();
                inTask.bits.processIoMainApp = FALSE;
            }

            #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
            BOOL PsPollEnabled;
            static BOOL psConfDone = FALSE;

            if (inTask.bits.wfUsePowerSaveFunctions == FALSE) {
                inTask.bits.wfUsePowerSaveFunctions = TRUE;
                wdtTask.bits.wfUsePowerSaveFunctions = TRUE;

                if (!psConfDone && WFisConnected()) {
                    PsPollEnabled = (MY_DEFAULT_PS_POLL == WF_ENABLED);
                    if (!PsPollEnabled) {
                        //Disable low power (PS-Poll) mode
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Disable PS-Poll\r\n");
                        WF_PsPollDisable();
                    } else {
                        /Enable low power (PS-Poll) mode
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Enable PS-Poll\r\n");
                        WF_PsPollEnable(TRUE);
                    }
                    psConfDone = TRUE;
                }

                inTask.bits.wfUsePowerSaveFunctions = FALSE;
            }
            #endif


            /////////////////////////////////////////////////
            // Every 20ms ///////////////////////////////////
            if (taskCount10ms&0x01) {
                //IP address has changed (ex: due to DHCP lease change)! Write new IP to debug, and Announce service
                if (inTask.bits.newIP == FALSE) {
                    inTask.bits.newIP = TRUE;
                    if (dwLastIP != AppConfig.MyIPAddr.Val) {
                        dwLastIP = AppConfig.MyIPAddr.Val;

                        //sprintf(debugTempBuf, "\nNew IP Address: %d.%d.%d.%d", AppConfig.MyIPAddr.v[0], AppConfig.MyIPAddr.v[1], AppConfig.MyIPAddr.v[2], AppConfig.MyIPAddr.v[3]);
                        //debugPutString((const char *) debugTempBuf);
                        debugPutString("\nNew IP Address: ");
                        debugPrintIP();

                        #if defined(STACK_USE_ANNOUNCE)
                        AnnounceIP();
                        #endif

                        #if defined(STACK_USE_ZEROCONF_MDNS_SD)
                        mDNSFillHostRecord();
                        #endif
                    }
                    inTask.bits.newIP = FALSE;
                }
            }
            /////////////////////////////////////////////////
            // Every other 20ms /////////////////////////////
            //else {
            //}


            /////////////////////////////////////////////////
            // Every 100ms //////////////////////////////////
            if(taskCount100ms-- == 0) {
                taskCount100ms=9;

                //LED Flash code every 100ms
                {
                    LEDTask();

                    #if !defined(RELEASE_BUILD) && defined(PT66EI_24P_LEDS)
                    //Test - Run leds on PT66EI-24P
                    /*
                    pt66ei24p_WriteLeds(leds);
                    if (leds==0x01)
                        leds=0x80;
                    else if (leds == 0x02)
                        leds = 0x40;
                    else if (leds == 0)
                        leds = 0x80;
                    else
                        leds = leds >> 2;
                    */
                    #endif
                }
            }
            /////////////////////////////////////////////////
            // Every other 100ms ////////////////////////////
			//else {
			//}
        }

        /////////////////////////////////////////////////
        // Every 2ms ///////////////////////////////////
        if( (taskCount10ms & 0x01)==0) {

        }
        /////////////////////////////////////////////////
        // Every other 2ms //////////////////////////////
        //else {
        //}


        /////////////////////////////////////////////////
        // Every 1ms ////////////////////////////////////
    }

    //Don't use inTask.bits.xxx to disable recursive calls. These checks are done in nzSysTaskDefault()
    nzSysTaskDefault(); //Netcruzer System task

    if (inTask.bits.serUsbProcessCmd == FALSE) {
        inTask.bits.serUsbProcessCmd = TRUE;
        serUsbProcessCmd();
        inTask.bits.serUsbProcessCmd = FALSE;
    }

    //This task performs normal stack task including checking for incoming packet, type of packet and
    //calling appropriate stack entity to process it.
    if (inTask.bits.StackTask == FALSE) {
        inTask.bits.StackTask = TRUE;
        StackTask();
        inTask.bits.StackTask = FALSE;
    }

    // This tasks invokes each of the core stack application tasks
    if (inTask.bits.StackApplications == FALSE) {
        inTask.bits.StackApplications = TRUE;
        StackApplications();
        inTask.bits.StackApplications = FALSE;
    }
    
    #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
    if (inTask.bits.ZeroconfLLProcess == FALSE) {
        inTask.bits.ZeroconfLLProcess = TRUE;
        ZeroconfLLProcess();
        inTask.bits.ZeroconfLLProcess = FALSE;
    }
    #endif

    #if defined(STACK_USE_ZEROCONF_MDNS_SD)
    if (inTask.bits.mDNSProcess == FALSE) {
        inTask.bits.mDNSProcess = TRUE;
        mDNSProcess();
        // Use this function to exercise service update function
        // HTTPUpdateRecord();
        inTask.bits.mDNSProcess = FALSE;
    }
    #endif

    //Process application specific tasks here. For this demo app, this will include the Generic TCP client
    //and servers, and the SNMP, Ping, and SNMP Trap demos.  Following that, we will process any IO from
    // the inputs on the board itself. Any custom modules or processing you need to do should go here.
    #if defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE)
    if (inTask.bits.GenericTCPClient == FALSE) {
        inTask.bits.GenericTCPClient = TRUE;
        GenericTCPClient();
        inTask.bits.GenericTCPClient = FALSE;
    }
    #endif

    #if defined(STACK_USE_GENERIC_TCP_SERVER_EXAMPLE)
    if (inTask.bits.GenericTCPServer == FALSE) {
        inTask.bits.GenericTCPServer = TRUE;
        GenericTCPServer();
        inTask.bits.GenericTCPServer = FALSE;
    }
    #endif

    if (inTask.bits.MicrochipDemo == FALSE) {
        inTask.bits.MicrochipDemo = TRUE;
        #if defined(STACK_USE_SMTP_CLIENT)
        SMTPDemo();
        #endif

        #if defined(STACK_USE_ICMP_CLIENT)
        PingDemo();
        #endif
    }

    #if defined(STACK_USE_SNMP_SERVER) && !defined(SNMP_TRAP_DISABLED)
    if (inTask.bits.SNMP == FALSE) {
        inTask.bits.SNMP = TRUE;
        //User should use one of the following SNMP demo
        //This routine demonstrates V1 or V2 trap formats with one variable binding.
        SNMPTrapDemo();
        inTask.bits.MicrochipDemo = FALSE;
        #if defined(SNMP_STACK_USE_V2_TRAP) || defined(SNMP_V1_V2_TRAP_WITH_SNMPV3)
        //This routine provides V2 format notifications with multiple (3) variable bindings
        //User should modify this routine to send v2 trap format notifications with the required varbinds.
        //SNMPV2TrapDemo();
        #endif
        if (gSendTrapFlag)
            SNMPSendTrap();
        inTask.bits.SNMP = FALSE;
    }
    #endif
}


/**
 * Task for flashing system LED. Called each 100ms
 */
static void LEDTask(void) {
    sysLedMask = sysLedMask << 1;

    if (sysLedMask == 0)
        sysLedMask = 0x0001;

    if (sysLedPattern & sysLedMask)
        LAT_SYSLED = 1;  // Turn the LED on
    else
        LAT_SYSLED = 0;  // Turn the LED off
}


/**
 * blinkUSBStatus turns on and off LEDs corresponding to the USB device state.
 * Note: USBDeviceState is declared and updated in usb_device.c.
 */
void blinkUSBStatus(void) {
    //USB Suspended
    if(USBIsDeviceSuspended()) {
        //sysLedPattern = 0b0001111100010101;     //1 long, 3 short flash
    }
    else {
        //If USB is ready and plugged in, 1 long and 2 short flases. Else, 1 long and 1 short flash
        if(USBDeviceState == CONFIGURED_STATE)
            sysLedPattern = 0b0000111110000101;     //1 long, 2 short flash
        else
            sysLedPattern = 0b0000011111000001;     //1 long, 1 short flash
    }
}


/**
 * Processes I/O
 */
static void processIoMainApp(void) {
#if defined(CALCULATE_AN0STRING)
    //Use AN1 value for AN0String, is shown on web page as an example
    uitoa((WORD) adcReadChan(ADC_CH_A3), AN0String);
#endif
}


/**
 * Hardware initialization
 */
void initializeBoard(void) {
	
    // LEDs
    DIR_SYSLED = OUTPUT_PIN;
    LAT_SYSLED = 0;

    sysLedMask = 0x0001;
    sysLedPattern = 0x0f0f;         //Blink

    //Set all ports as Digital I/Os.
    #if defined(__PIC24F__)

    CLKDIVbits.RCDIV = 0; // Set 1:1 8MHz FRC postscalar

    /////////////////////////////////////////////////
    //Configure ADC /////////////////////////////
    // Set all ports as digital ports, 0=Digital, 1=Analog
    #if defined(__PIC24FJ256GB210__)
        ANSA = 0x0000;
        ANSB = 0x0000;
        ANSC = 0x0000;
        ANSD = 0x0000;
        ANSE = 0x0000;
        ANSF = 0x0000;
        ANSG = 0x0000;
    #elif defined(__PIC24FJ256GB206__)
        ANSB = 0x0000;
        ANSC = 0x0000;
        ANSD = 0x0000;
        ANSF = 0x0000;
        ANSG = 0x0000;
    #elif defined(__PIC24FJ128GB106__)
        //All ports digital ports
        AD1PCFGL = 0xffff;
        AD1PCFGH = 0xffff;
    #else
        AD1PCFG0 = 0xffff;
    #endif
    #endif

    /////////////////////////////////////////////////
    //Configure Comparator voltage sense input for power fail sense
    _CMIP = 6; //Set interrupt priority very high = 6 (7 is the highest)
    #if defined(BRD_SBC66EC_R1) || defined(BRD_SBC66EC_R2) || defined(BRD_SBC66EC_R3)
        //RD6 is Power Fail Sense input
        ANSD |= 0x0040; //Configure RD6 for Analog function, will be comparitor input

        //Configure Comparator Voltage Reference module. For values, see excel spreadsheed in ../netcruzer/doc/comparator voltage reference.xls file

        //Voltage Fail sense is connected to 5V via a resistor voltage divider. We want interrupt to be triggered when voltage
        //falls below 4.8V. After the voltage divider: 5V=1.437V, 4.9V=1.408V, 4.8V=1.38V, 4.78V=1.374V, 4.74V=1.354V, 4.38V=1.25V
        //With 2.5V reference selected, reference voltage can be configured for 1.354V and 1.25V, this corresponds to 4.74V and 4.38V VCC.
        //With 3.3V reference selected, reference voltage is configured for 1.374V, this corresponds to 4.78V
        //Configure for 1.354V
        //xxxx xxxx xxxx 1010 - CVR is set to 10, this gives a voltage of 1.374
        //xxxx xxxx xxx0 xxxx - CVRSS selects Avdd as source = 3.3V
        //xxxx xxxx xx1x xxxx - CVRR sets range to 0 to 0.625 CVRSRC
        //xxxx xxxx x0xx xxxx - CVROE voltage level is disconnected from the CVREF pin
        //xxxx xxxx 1xxx xxxx - CVREN Comparator Voltage Reference Enable
        //xxxx xx00 xxxx xxxx - CVREFM Not used, bits are DON'T CARE
        //xxxx x0xx xxxx xxxx - CVREFP The CVR (4-bit DAC) within this module provides the reference voltage to the comparators
        CVRCON = 0x00AA;

        //xxxx xxxx xxxx xx00 - CCH Inverting input of the comparator connects to the CXINB pin
        //xxxx xxxx xxx1 xxxx - CREF Non-inverting input connects to the internal CVREF voltage
        //xxxx xxxx 11xx xxxx - EVPOL Trigger/Event/Interrupt generated on any change of the comparator output
        //xxxx xxx0 xxxx xxxx - COUT, read only bit. Contains comparator output
        //xxxx xx0x xxxx xxxx - CEVT, Comparator Event bit. Comparator event occurred; subsequent triggers and interrupts are disabled until the bit is cleared
        //xx0x xxxx xxxx xxxx - CPOL, Comparator output is not inverted
        //x0xx xxxx xxxx xxxx - COE Comparator output is internal only, and is NOT present on the CxOUT pin
        //1xxx xxxx xxxx xxxx - CEN Comparator enable
        CM3CON = 0x80D0; //Comparator to pin

        _CMIE = 0; //Comparator interrupt is only enabled after system is running for a while

        _CMIF = 0; //Clear interrupt flag

        //This bit must be cleared to enable more interrupts
        CM3CONbits.CEVT = 0;

        #if !defined(RELEASE_BUILD)
        // For testing, make port 28 (old port name T6) (RF4) (RP10) comparator 3 output. Can connecto to scope to see if it works
        //DIR_28 = 0;     //
        //iPPSOutput(OUT_PIN_PPS_RP10, OUT_FN_PPS_C3OUT);    //Assign RP12 to SCK2 (output)
        #endif
    #elif defined(BRD_SBC66ECL_R2) || defined(BRD_SBC66ECL_R3)
        //Configure Comparator Voltage Reference module. For values, see excel spreadsheed in ../netcruzer/doc/comparator voltage reference.xls file

        //Voltage Fail sense is connected to 5V via a resistor voltage divider. We want interrupt to be triggered when voltage
        //falls below 4.8V. After the voltage divider: 5V=1.437V, 4.9V=1.408V, 4.8V=1.38V, 4.78V=1.374V, 4.74V=1.354V, 4.38V=1.25V
        //With 2.5V reference selected, reference voltage can be configured for 1.354V and 1.25V, this corresponds to 4.74V and 4.38V VCC.
        //With 3.3V reference selected, reference voltage is configured for 1.374V, this corresponds to 4.78V
        //Configure for 1.354V
        //xxxx xxxx xxxx 1010 - CVR is set to 10, this gives a voltage of 1.374
        //xxxx xxxx xxx0 xxxx - CVRSS selects Avdd as source = 3.3V
        //xxxx xxxx xx1x xxxx - CVRR sets range to 0 to 0.625 CVRSRC
        //xxxx xxxx x0xx xxxx - CVROE voltage level is disconnected from the CVREF pin
        //xxxx xxxx 1xxx xxxx - CVREN Comparator Voltage Reference Enable
        //xxxx xx00 xxxx xxxx - CVREFM Not used, bits are DON'T CARE
        //xxxx x0xx xxxx xxxx - CVREFP The CVR (4-bit DAC) within this module provides the reference voltage to the comparators
        CVRCON = 0x00AA;

        //xxxx xxxx xxxx xx00 - CCH Inverting input of the comparator connects to the CXINB pin
        //xxxx xxxx xxx1 xxxx - CREF Non-inverting input connects to the internal CVREF voltage
        //xxxx xxxx 11xx xxxx - EVPOL Trigger/Event/Interrupt generated on any change of the comparator output
        //xxxx xxx0 xxxx xxxx - COUT, read only bit. Contains comparator output
        //xxxx xx0x xxxx xxxx - CEVT, Comparator Event bit. Comparator event occurred; subsequent triggers and interrupts are disabled until the bit is cleared
        //xx0x xxxx xxxx xxxx - CPOL, Comparator output is not inverted
        //x0xx xxxx xxxx xxxx - COE Comparator output is internal only, and is NOT present on the CxOUT pin
        //1xxx xxxx xxxx xxxx - CEN Comparator enable
        CM3CON = 0x80D0; //Comparator to pin

        _CMIE = 0; //Comparator interrupt is only enabled after system is running for a while

        _CMIF = 0; //Clear interrupt flag

        //This bit must be cleared to enable more interrupts
        CM3CONbits.CEVT = 0;

        #if !defined(RELEASE_BUILD)
        // For testing, make port 28 (old port name T6) (RF4) (RP10) comparator 3 output. Can connecto to scope to see if it works
        //DIR_28 = 0;
        //iPPSOutput(OUT_PIN_PPS_RP10, OUT_FN_PPS_C3OUT);    //Assign RP12 to SCK2 (output)
        #endif
    #elif defined(BRD_DIN24I)
    #else
    #error "Board has to be defined"
    #endif

    /////////////////////////////////////////////////
    //Initialize SPI bus for EEPROM and FLASH
    // Deassert all chip select lines so there isn't any problem with initialization order.
    #if defined(EEPROM_CS_TRIS)
    EEPROM_CS_IO = 1;
    EEPROM_CS_TRIS = 0;
    #endif
    #if defined(SPIRAM_CS_TRIS)
    SPIRAM_CS_IO = 1;
    SPIRAM_CS_TRIS = 0;
    #endif
    #if defined(SPIFLASH_CS_TRIS)
    SPIFLASH_CS_IO = 1;
    SPIFLASH_CS_TRIS = 0;
    #endif
    #if defined(WF_CS_TRIS)
    WF_CS_IO = 1;
    WF_CS_TRIS = 0;
    #endif

    //#if defined(BRD_SBC66EC_R1) || defined(BRD_SBC66EC_R2) || defined(BRD_SBC66ECL_R2)
    //PPSUnLock;  // Unlock PPS, called at top of nzSysInitDefault()!
    //Configure External EEPROM and FLASH to use SPI2. They share the same SPI bus
    iPPSOutput(PPS_OUT_SPIMEM_SCK, OUT_FN_PPS_SCK2OUT);     //Assign SCK2 (output)
    iPPSOutput(PPS_OUT_SPIMEM_SDO, OUT_FN_PPS_SDO2);        //Assign SDO2 (output)
    iPPSInput(IN_FN_PPS_SDI2, PPS_IN_SPIMEM_SDI);           //Assign SDI2 (input)
    //#endif

    //Set SPI Memory bus (Used by FLASH and EEPROM)
    DIR_SPIMEM_SCK = 0;    // Set SCK pin as an output
    DIR_SPIMEM_SDI = 1;    // Set SDI pin is an input
    DIR_SPIMEM_SDO = 0;    // Set SDO pin as an output

    #if defined(__PIC24F__) || defined(__PIC24FK__)
        //SPI Clock = Fcy/2 = 16MHz/2 = 8MHz
        //xxxx xxxx xxxx xx11 - Primary Prescale 1:1
        //xxxx xxxx xxx1 10xx - Secondary prescale 2:1
        //xxxx xxxx xx1x xxxx - Master Mode
        //xxxx xxxx x0xx xxxx - Clock Idle is low level
        //xxxx xxxx 0xxx xxxx - SS Slave Select Disable
        //xxxx xxx1 xxxx xxxx - Serial output data changes on clock falling edge (active clock state to Idle clock state)
        //xxxx xx0x xxxx xxxx - Input data sampled at middle of data output time
        //xxxx x0xx xxxx xxxx - Communication is byte-wide (8 bits)
        //xxxx 0xxx xxxx xxxx - SDOx pin is controlled by the module
        //xxx0 xxxx xxxx xxxx - Internal SPI clock is enabled
        SPIMEM_SPICON1 = 0x013b;
        //SPIMEM_SPICON1 = 0x0133;    //SPI Clock = 4Mbit/sec, Primary prescaler 1:1, Secondary prescale 4:1
        //SPIMEM_SPICON1 = 0x013a;    //SPI Clock = 2Mbit/sec, Primary prescaler 4:1, Secondary prescale 2:1
        //SPIMEM_SPICON1 = 0x0132;    //SPI Clock = 1Mbit/sec, Primary prescaler 4:1, Secondary prescale 4:1
        //SPIMEM_SPICON1 = 0x0138;    //Clock = 125Kbit/sec, Primary prescaler 64:1, Secondary prescale 2:1
    #else
        #error "SPIMEM not supported for this hardware!"
    #endif

    SPIMEM_SPICON2 = 0;
    SPIMEM_SPISTAT = 0;    // clear SPI
    SPIMEM_SPISTATbits.SPIEN = 1;

    #if defined(SPIRAM_CS_TRIS)
    xramInit();
    #endif
    #if defined(EEPROM_CS_TRIS)
    xeeInit();
    #endif
    #if defined(SPIFLASH_CS_TRIS)
    xflashInit();
    #endif
}


#if defined(WF_CS_TRIS)
/**
 * Connects to an 802.11 network. Customize this function as needed for your application.
 */
static void WF_Connect(void) {
    UINT8 ConnectionProfileID;
    UINT8 channelList[] = MY_DEFAULT_CHANNEL_LIST;

    /* create a Connection Profile */
    WF_CPCreate(&ConnectionProfileID);

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set SSID (");
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, AppConfig.MySSID);
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, ")\r\n");

    WF_CPSetSsid(ConnectionProfileID,
            AppConfig.MySSID,
            AppConfig.SsidLength);

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Network Type\r\n");
    WF_CPSetNetworkType(ConnectionProfileID, MY_DEFAULT_NETWORK_TYPE);

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Scan Type\r\n");
    WF_CASetScanType(MY_DEFAULT_SCAN_TYPE);

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Channel List\r\n");
    WF_CASetChannelList(channelList, sizeof (channelList));

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set list retry count\r\n");
    // The Retry Count parameter tells the WiFi Connection manager how many attempts to make when trying
    // to connect to an existing network.  In the Infrastructure case, the default is to retry forever so that
    // if the AP is turned off or out of range, the radio will continue to attempt a connection until the
    // AP is eventually back on or in range.  In the Adhoc case, the default is to retry 3 times since the
    // purpose of attempting to establish a network in the Adhoc case is only to verify that one does not
    // initially exist.  If the retry count was set to WF_RETRY_FOREVER in the AdHoc mode, an AdHoc network
    // would never be established.  The constants MY_DEFAULT_LIST_RETRY_COUNT_ADHOC and
    // MY_DEFAULT_LIST_RETRY_COUNT_INFRASTRUCTURE have been created specifically for the June 2011 MAL release.
#if defined(EZ_CONFIG_STORE)
    if (AppConfig.networkType == WF_ADHOC)
        WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_ADHOC);
    else
        WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_INFRASTRUCTURE);
#else
#if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
    WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_ADHOC);
#else
    WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_INFRASTRUCTURE);
#endif
#endif


    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Event Notify\r\n");
    WF_CASetEventNotificationAction(MY_DEFAULT_EVENT_NOTIFICATION_LIST);

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Beacon Timeout\r\n");
    WF_CASetBeaconTimeout(40);

    /* Set Security */
#if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_OPEN)
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Security (Open)\r\n");
#elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40)
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Security (WEP40)\r\n");
#elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104)
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Security (WEP104)\r\n");
#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_KEY
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Security (WPA with key)\r\n");
#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_KEY
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Security (WPA2 with key)\r\n");
#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_PASS_PHRASE
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Security (WPA with pass phrase)\r\n");
#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_PASS_PHRASE
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Security (WPA2 with pass phrase)\r\n");
#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_KEY
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Security (WPA with key, auto-select)\r\n");
#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Set Security (WPA with pass phrase, auto-select)\r\n");
#endif /* MY_DEFAULT_WIFI_SECURITY_MODE */

    WF_CPSetSecurity(ConnectionProfileID,
            AppConfig.SecurityMode,
            AppConfig.WepKeyIndex, /* only used if WEP enabled */
            AppConfig.SecurityKey,
            AppConfig.SecurityKeyLength);
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "Start WiFi Connect\r\n");
    WF_CMConnect(ConnectionProfileID);
}
#endif /* WF_CS_TRIS */


/**
 * Power is failing. Go to low power mode
 */
static void expansionInit(void) {
    #if !defined(RELEASE_BUILD)
    // Test PT24ei board
    //pt66ei24p_Init();
    //pt66ei24p_WriteLeds(0x37);
    //pt66ei24p_WriteLeds(0b00000001);
    #endif
}
