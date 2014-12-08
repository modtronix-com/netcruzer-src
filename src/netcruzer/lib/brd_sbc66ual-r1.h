/**
 * @brief           Contains hardware specific defines for SBC66UAL Revision 1 board.
 * @file            brd_sbc66ual-r1.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section sbc66ual_r1_desc Description
 *****************************************
 * This module includes hardware specific defines for SBC66EC Revision 2 board.
 *
 **********************************************************************
 * @section sbc66ual_r1_lic Software License Agreement
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
 * 2014-07-24, David H. (DH):
 *    - Changed WDT from 32 to 8 seconds
 * 2012-08-06, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef SBC66UAL_R1_H
#define SBC66UAL_R1_H


/**
 * Set configuration fuses to default values. If this is done in app, define CONFIGURATION_FUSES_SET
 * to prevent them being set here again.
 */
#if defined(THIS_IS_MAIN_FILE)
    #if defined(__PIC24FJ128GB106__)

        #if !defined(CONFIGURATION_FUSES_SET)
            #define CONFIGURATION_FUSES_SET

            //Are not defined in p24FJ128GB106
            #ifndef DEBUG_ON
            #define DEBUG_ON              0x77FF
            #endif
            #ifndef DEBUG_OFF
            #define DEBUG_OFF             0x7FFF
            #endif

            // - JTAG Port Disabled (JTAGEN_OFF)
            // - General Segment Code Protect = Code protection is enabled (GCP_OFF)
            // - General Segment Write Protect Disabled (GWRP_OFF)
            // - Emulator functions are shared with PGEC2/PGED2 (ICS_PGx2)
            //Watchdog timer can be enabled in software via SWDTEN bit. Resets every 8 seconds
            // - Watchdog Timer Disabled (FWDTEN_OFF)
            // - WDT Prescaler = 32 (FWPSA_PR32)
            // - Watchdog Timer Postscaler = 1:32,768 (WDTPS_PS32768)
            _CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx2 & FWDTEN_OFF & FWPSA_PR32 & WDTPS_PS8192)
            //_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx2 & FWDTEN_OFF & FWPSA_PR32 & WDTPS_PS4096)

            // - Two Speed Start-up = off (IESO_OFF)
            // - 96MHz PLL Prescaler = Divide by 4, for 16MHz input (PLLDIV_DIV4)
            // - Disable 96MHz PLL = not disabled, don't add anthing to _CONFIG2
            // - Oscillator Selection = Primary oscillator (XT, HS, EC) w/ PLL (FNOSC_PRIPLL)
            // - Clock switching and clock monitor = Both disabled (FCKSM_CSDCMD)
            // - OSCO/RC15 function = OSCO or Fosc/2 (OSCIOFNC_OFF)
            // - RP Register Protection = Unlimited Writes To RP Registers (IOL1WAY_OFF)
            // - Disable Internal USB Voltage Regulator = Disabled (DISUVREG_OFF)
            // - Oscillator Selection = HS oscillator (POSCMOD_HS)
            _CONFIG2( IESO_OFF & PLLDIV_DIV4 & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF & IOL1WAY_OFF & DISUVREG_OFF & POSCMOD_HS)

            // - Last page(at the top of program memory) and Flash configuration words are not protected (WPCFG_WPCFGDIS)
            // - Segment Write Protection = Segmented code protection disabled (WPDIS_WPDIS)
            _CONFIG3(WPCFG_WPCFGDIS & WPDIS_WPDIS)        //Disable erase/write protect of all memory regions.
        #endif
    #else
            #error "Not configured for valid CPU"
    #endif
#endif // Prevent more than one set of config fuse definitions

//System clock = Fosc. Instruction clock = Fcy = Fosc/2 (in datasheet)
#if !defined(CLOCK_FREQ)
        #define CLOCK_FREQ              (32000000ul)
        #define GetSystemClock()        (32000000ul)      // Hz
        #define GetInstructionClock()   (GetSystemClock()/2)
        #define GetPeripheralClock()    (GetSystemClock()/2)
#endif



// *********************************************************************
// ----------- Daughter Board Connector Defines for SBC66ZL ------------
// *********************************************************************
#define PORT_ID_MAX     41  //ID of highest used port ID

#define UCPORT_00       B0
#define UCPORT_ID_00    UCPORT_ID_B0
#define PIN_00          _RB0
#define PIN_00_BITADR   ((volatile WORD)(/*Bit0=0x0nnn*/0x02CA/*PORTB=0x02CA*/ ))
#define LAT_00          _LATB0
#define LAT_00_BITADR   ((volatile WORD)(/*Bit0=0x0nnn*/0x02CC/*LATB=0x02CC*/ ))
#define DIR_00          _TRISB0
#define TRIS_00_BITADR  ((volatile WORD)(/*Bit0=0x0nnn*/0x02C8/*TRISB=0x02C8*/ ))
#define PULLUP_00       _CN2PUE
#define PULLDOWN_00      _CN2PDE
#define PPS_OUT_00      _RP0R
#define PPS_IN_00       0

#define UCPORT_01       B1
#define UCPORT_ID_01    UCPORT_ID_B1
#define PIN_01          _RB1
#define PIN_01_BITADR   ((volatile WORD)(/*Bit1=0x1nnn*/0x12CA/*PORTB=0x02CA*/ ))
#define LAT_01          _LATB1
#define LAT_01_BITADR   ((volatile WORD)(/*Bit1=0x1nnn*/0x12CC/*LATB=0x02CC*/ ))
#define DIR_01          _TRISB1
#define TRIS_01_BITADR  ((volatile WORD)(/*Bit1=0x1nnn*/0x12C8/*TRISB=0x02C8*/ ))
#define PULLUP_01       _CN3PUE
#define PULLDOWN_01      _CN3PDE
#define PPS_OUT_01      _RP1R
#define PPS_IN_01       1

#define UCPORT_02       B2
#define UCPORT_ID_02    UCPORT_ID_B2
#define PIN_02          _RB2
#define PIN_02_BITADR   ((volatile WORD)(/*Bit2=0x2nnn*/0x22CA/*PORTB=0x02CA*/ ))
#define LAT_02          _LATB2
#define LAT_02_BITADR   ((volatile WORD)(/*Bit2=0x2nnn*/0x22CC/*LATB=0x02CC*/ ))
#define DIR_02          _TRISB2
#define TRIS_02_BITADR  ((volatile WORD)(/*Bit2=0x2nnn*/0x22C8/*TRISB=0x02C8*/ ))
#define PULLUP_02       _CN4PUE
#define PULLDOWN_02      _CN4PDE
#define PPS_OUT_02      _RP13R
#define PPS_IN_02       13

#define UCPORT_03       B3
#define UCPORT_ID_03    UCPORT_ID_B3
#define PIN_03          _RB3
#define PIN_03_BITADR   ((volatile WORD)(/*Bit3=0x3nnn*/0x32CA/*PORTB=0x02CA*/ ))
#define LAT_03          _LATB3
#define LAT_03_BITADR   ((volatile WORD)(/*Bit3=0x3nnn*/0x32CC/*LATB=0x02CC*/ ))
#define DIR_03          _TRISB3
#define TRIS_03_BITADR  ((volatile WORD)(/*Bit3=0x3nnn*/0x32C8/*TRISB=0x02C8*/ ))
#define PULLUP_03       _CN5PUE
#define PULLDOWN_03      _CN5PDE

#define UCPORT_04       B4
#define UCPORT_ID_04    UCPORT_ID_B4
#define PIN_04          _RB4
#define PIN_04_BITADR   ((volatile WORD)(/*Bit4=0x4nnn*/0x42CA/*PORTB=0x02CA*/ ))
#define LAT_04          _LATB4
#define LAT_04_BITADR   ((volatile WORD)(/*Bit4=0x4nnn*/0x42CC/*LATB=0x02CC*/ ))
#define DIR_04          _TRISB4
#define TRIS_04_BITADR  ((volatile WORD)(/*Bit4=0x4nnn*/0x42C8/*TRISB=0x02C8*/ ))
#define PULLUP_04       _CN6PUE
#define PULLDOWN_04      _CN6PDE
#define PPS_OUT_04      _RP28R
#define PPS_IN_04       28

#define UCPORT_05       B5
#define UCPORT_ID_05    UCPORT_ID_B5
#define PIN_05          _RB5
#define PIN_05_BITADR   ((volatile WORD)(/*Bit5=0x5nnn*/0x52CA/*PORTB=0x02CA*/ ))
#define LAT_05          _LATB5
#define LAT_05_BITADR   ((volatile WORD)(/*Bit5=0x5nnn*/0x52CC/*LATB=0x02CC*/ ))
#define DIR_05          _TRISB5
#define TRIS_05_BITADR  ((volatile WORD)(/*Bit5=0x5nnn*/0x52C8/*TRISB=0x02C8*/ ))
#define PULLUP_05       _CN7PUE
#define PULLDOWN_05      _CN7PDE
#define PPS_OUT_05      _RP18R
#define PPS_IN_05       18

#define UCPORT_06       G6
#define UCPORT_ID_06     UCPORT_ID_G6
#define PIN_06          _RG6
#define PIN_06_BITADR   ((volatile WORD)(/*Bit6=0x6nnn*/0x62F2/*PORTG=0x02F2*/ ))
#define LAT_06          _LATG6
#define LAT_06_BITADR   ((volatile WORD)(/*Bit6=0x6nnn*/0x62F4/*LATG=0x02F4*/ ))
#define DIR_06          _TRISG6
#define TRIS_06_BITADR  ((volatile WORD)(/*Bit6=0x6nnn*/0x62F0/*TRISG=0x02F0*/ ))
#define PULLUP_06       _CN8PUE
#define PULLDOWN_06      _CN8PDE
#define PPS_OUT_06      _RP21R
#define PPS_IN_06       21

#define UCPORT_07       G7
#define UCPORT_ID_07    UCPORT_ID_G7
#define PIN_07          _RG7
#define PIN_07_BITADR   ((volatile WORD)(/*Bit7=0x7nnn*/0x72F2/*PORTG=0x02F2*/ ))
#define LAT_07          _LATG7
#define LAT_07_BITADR   ((volatile WORD)(/*Bit7=0x7nnn*/0x72F4/*LATG=0x02F4*/ ))
#define DIR_07          _TRISG7
#define TRIS_07_BITADR  ((volatile WORD)(/*Bit7=0x7nnn*/0x72F0/*TRISG=0x02F0*/ ))
#define PULLUP_07       _CN9PUE
#define PULLDOWN_07      _CN9PDE
#define PPS_OUT_07      _RP26R
#define PPS_IN_07       26

#define UCPORT_08       G8
#define UCPORT_ID_08    UCPORT_ID_G8
#define PIN_08          _RG8
#define PIN_08_BITADR   ((volatile WORD)(/*Bit8=0x8nnn*/0x82F2/*PORTG=0x02F2*/ ))
#define LAT_08          _LATG8
#define LAT_08_BITADR   ((volatile WORD)(/*Bit8=0x8nnn*/0x82F4/*LATG=0x02F4*/ ))
#define DIR_08          _TRISG8
#define TRIS_08_BITADR  ((volatile WORD)(/*Bit8=0x8nnn*/0x82F0/*TRISG=0x02F0*/ ))
#define PULLUP_08       _CN10PUE
#define PULLDOWN_08      _CN10PDE
#define PPS_OUT_08      _RP19R
#define PPS_IN_08       19

#define UCPORT_09       G9
#define UCPORT_ID_09    UCPORT_ID_G9
#define PIN_09          _RG9
#define PIN_09_BITADR   ((volatile WORD)(/*Bit9=0x9nnn*/0x92F2/*PORTG=0x02F2*/ ))
#define LAT_09          _LATG9
#define LAT_09_BITADR   ((volatile WORD)(/*Bit9=0x9nnn*/0x92F4/*LATG=0x02F4*/ ))
#define DIR_09          _TRISG9
#define TRIS_09_BITADR  ((volatile WORD)(/*Bit9=0x9nnn*/0x92F0/*TRISG=0x02F0*/ ))
#define PULLUP_09       _CN11PUE
#define PULLDOWN_09      _CN11PDE
#define PPS_OUT_09      _RP27R
#define PPS_IN_09       27

#define UCPORT_10       E2
#define UCPORT_ID_10    UCPORT_ID_E2
#define PIN_10          _RE2
#define PIN_10_BITADR   ((volatile WORD)(/*Bit2=0x2nnn*/0x22E2/*PORTE=0x02E2*/ ))
#define LAT_10          _LATE2
#define LAT_10_BITADR   ((volatile WORD)(/*Bit2=0x2nnn*/0x22E4/*LATE=0x02E4*/ ))
#define DIR_10          _TRISE2
#define TRIS_10_BITADR  ((volatile WORD)(/*Bit2=0x2nnn*/0x22E0/*TRISE=0x02E0*/ ))
#define PULLUP_10       _CN60PUE
#define PULLDOWN_10      _CN60PDE

#define UCPORT_11       E3
#define UCPORT_ID_11    UCPORT_ID_E3
#define PIN_11          _RE3
#define PIN_11_BITADR   ((volatile WORD)(/*Bit3=0x3nnn*/0x32E2/*PORTE=0x02E2*/ ))
#define LAT_11          _LATE3
#define LAT_11_BITADR   ((volatile WORD)(/*Bit3=0x3nnn*/0x32E4/*LATE=0x02E4*/ ))
#define DIR_11          _TRISE3
#define TRIS_11_BITADR  ((volatile WORD)(/*Bit3=0x3nnn*/0x32E0/*TRISE=0x02E0*/ ))
#define PULLUP_11       _CN61PUE
#define PULLDOWN_11      _CN61PDE

#define UCPORT_12       D9
#define UCPORT_ID_12     UCPORT_ID_D9
#define PIN_12           _RD9
#define PIN_12_BITADR   ((volatile WORD)(/*Bit9=0x9nnn*/0x92DA/*PORTD=0x02DA*/ ))
#define LAT_12           _LATD9
#define LAT_12_BITADR   ((volatile WORD)(/*Bit9=0x9nnn*/0x92DC/*LATD=0x02DC*/ ))
#define DIR_12           _TRISD9
#define TRIS_12_BITADR  ((volatile WORD)(/*Bit9=0x9nnn*/0x92D8/*TRISD=0x02D8*/ ))
#define PULLUP_12        _CN54PUE
#define PULLDOWN_12       _CN54PDE
#define PPS_OUT_12       _RP4R
#define PPS_IN_12        4

#define UCPORT_13       D10
#define UCPORT_ID_13     UCPORT_ID_D10
#define PIN_13           _RD10
#define PIN_13_BITADR   ((volatile WORD)(/*Bit10=0xAnnn*/0xA2DA/*PORTD=0x02DA*/ ))
#define LAT_13           _LATD10
#define LAT_13_BITADR   ((volatile WORD)(/*Bit10=0xAnnn*/0xA2DC/*LATD=0x02DC*/ ))
#define DIR_13           _TRISD10
#define TRIS_13_BITADR  ((volatile WORD)(/*Bit10=0xAnnn*/0xA2D8/*TRISD=0x02D8*/ ))
#define PULLUP_13        _CN55PUE
#define PULLDOWN_13       _CN55PDE
#define PPS_OUT_13       _RP3R
#define PPS_IN_13        3

//#define UCPORT_14     0  //Dont define is not available!
#define UCPORT_ID_14    UCPORT_ID_NA
#define PIN_14_BITADR   BITADR_NA
#define LAT_14_BITADR   BITADR_NA
#define TRIS_14_BITADR  BITADR_NA

//#define UCPORT_15     0  //Dont define is not available!
#define UCPORT_ID_15    UCPORT_ID_NA
#define PIN_15_BITADR   BITADR_NA
#define LAT_15_BITADR   BITADR_NA
#define TRIS_15_BITADR  BITADR_NA

//#define UCPORT_16     0  //Dont define is not available!
#define UCPORT_ID_16    UCPORT_ID_NA
#define PIN_16_BITADR   BITADR_NA
#define LAT_16_BITADR   BITADR_NA
#define TRIS_16_BITADR  BITADR_NA

//#define UCPORT_17     0  //Dont define is not available!
#define UCPORT_ID_17    UCPORT_ID_NA
#define PIN_17_BITADR   BITADR_NA
#define LAT_17_BITADR   BITADR_NA
#define TRIS_17_BITADR  BITADR_NA

//#define UCPORT_18     0  //Dont define is not available!
#define UCPORT_ID_18    UCPORT_ID_NA
#define PIN_18_BITADR   BITADR_NA
#define LAT_18_BITADR   BITADR_NA
#define TRIS_18_BITADR  BITADR_NA

//#define UCPORT_19     0  //Dont define is not available!
#define UCPORT_ID_19    UCPORT_ID_NA
#define PIN_19_BITADR   BITADR_NA
#define LAT_19_BITADR   BITADR_NA
#define TRIS_19_BITADR  BITADR_NA

#define UCPORT_20       B6
#define UCPORT_ID_20    UCPORT_ID_B6
#define PIN_20          _RB6
#define PIN_20_BITADR   ((volatile WORD)(/*Bit6=0x6nnn*/0x62CA/*PORTB=0x02CA*/ ))
#define LAT_20          _LATB6
#define LAT_20_BITADR   ((volatile WORD)(/*Bit6=0x6nnn*/0x62CC/*LATB=0x02CC*/ ))
#define DIR_20          _TRISB6
#define TRIS_20_BITADR  ((volatile WORD)(/*Bit6=0x6nnn*/0x62C8/*TRISB=0x02C8*/ ))
#define PULLUP_20       _CN24PUE
#define PULLDOWN_20      _CN24PDE
#define PPS_OUT_20      _RP6R
#define PPS_IN_20       6

#define UCPORT_21       B7
#define UCPORT_ID_21    UCPORT_ID_B7
#define PIN_21          _RB7
#define PIN_21_BITADR   ((volatile WORD)(/*Bit7=0x7nnn*/0x72CA/*PORTB=0x02CA*/ ))
#define LAT_21          _LATB7
#define LAT_21_BITADR   ((volatile WORD)(/*Bit7=0x7nnn*/0x72CC/*LATB=0x02CC*/ ))
#define DIR_21          _TRISB7
#define TRIS_21_BITADR  ((volatile WORD)(/*Bit7=0x7nnn*/0x72C8/*TRISB=0x02C8*/ ))
#define PULLUP_21       _CN25PUE
#define PULLDOWN_21      _CN25PDE
#define PPS_OUT_21      _RP7R
#define PPS_IN_21       7

//Port 22 can be MCLR (RST) or E4
#define UCPORT_22       E4
#define UCPORT_ID_22    UCPORT_ID_E4
#define PIN_22          _RE4
#define PIN_22_BITADR   ((volatile WORD)(/*Bit4=0x4nnn*/0x42E2/*PORTE=0x02E2*/ ))
#define LAT_22          _LATE4
#define LAT_22_BITADR   ((volatile WORD)(/*Bit4=0x4nnn*/0x42E4/*LATE=0x02E4*/ ))
#define DIR_22          _TRISE4
#define TRIS_22_BITADR  ((volatile WORD)(/*Bit4=0x4nnn*/0x42E0/*TRISE=0x02E0*/ ))
#define PULLUP_22       _CN62PUE
#define PULLDOWN_22      _CN62PDE

#define UCPORT_23       E5
#define UCPORT_ID_23    UCPORT_ID_E5
#define PIN_23          _RE5
#define PIN_23_BITADR   ((volatile WORD)(/*Bit5=0x5nnn*/0x52E2/*PORTE=0x02E2*/ ))
#define LAT_23          _LATE5
#define LAT_23_BITADR   ((volatile WORD)(/*Bit5=0x5nnn*/0x52E4/*LATE=0x02E4*/ ))
#define DIR_23          _TRISE5
#define TRIS_23_BITADR  ((volatile WORD)(/*Bit5=0x5nnn*/0x52E0/*TRISE=0x02E0*/ ))
#define PULLUP_23       _CN63PUE
#define PULLDOWN_23      _CN63PDE

#define UCPORT_ID_24     UCPORT_ID_E6
#define PIN_24           _RE6
#define PIN_24_BITADR   ((volatile WORD)(/*Bit6=0x6nnn*/0x62E2/*PORTE=0x02E2*/ ))
#define LAT_24           _LATE6
#define LAT_24_BITADR   ((volatile WORD)(/*Bit6=0x6nnn*/0x62E4/*LATE=0x02E4*/ ))
#define DIR_24           _TRISE6
#define TRIS_24_BITADR  ((volatile WORD)(/*Bit6=0x6nnn*/0x62E0/*TRISE=0x02E0*/ ))
#define PULLUP_24        _CN64PUE
#define PULLDOWN_24       _CN64PDE

#define UCPORT_ID_25     UCPORT_ID_E7
#define PIN_25           _RE7
#define PIN_25_BITADR   ((volatile WORD)(/*Bit7=0x7nnn*/0x72E2/*PORTE=0x02E2*/ ))
#define LAT_25           _LATE7
#define LAT_25_BITADR   ((volatile WORD)(/*Bit7=0x7nnn*/0x72E4/*LATE=0x02E4*/ ))
#define DIR_25           _TRISE7
#define TRIS_25_BITADR  ((volatile WORD)(/*Bit7=0x7nnn*/0x72E0/*LAT0=0x02E0*/ ))
#define PULLUP_25        _CN65PUE
#define PULLDOWN_25       _CN65PDE

#define UCPORT_26       F1
#define UCPORT_ID_26    UCPORT_ID_F1
#define PIN_26          _RF1
#define PIN_26_BITADR   ((volatile WORD)(/*Bit1=0x1nnn*/0x12EA/*PORTF=0x02EA*/ ))
#define LAT_26          _LATF1
#define LAT_26_BITADR   ((volatile WORD)(/*Bit1=0x1nnn*/0x12EC/*LATF=0x02EC*/ ))
#define DIR_26          _TRISF1
#define TRIS_26_BITADR  ((volatile WORD)(/*Bit1=0x1nnn*/0x12E8/*TRISF=0x02E8*/ ))
#define PULLUP_26       _CN69PUE
#define PULLDOWN_26      _CN69PDE

#define UCPORT_27       F3
#define UCPORT_ID_27    UCPORT_ID_F3
#define PIN_27          _RF3
#define PIN_27_BITADR   ((volatile WORD)(/*Bit3=0x3nnn*/0x32EA/*PORTF=0x02EA*/ ))
#define LAT_27          _LATF3
#define LAT_27_BITADR   ((volatile WORD)(/*Bit3=0x3nnn*/0x32EC/*LATF=0x02EC*/ ))
#define DIR_27          _TRISF3
#define TRIS_27_BITADR  ((volatile WORD)(/*Bit3=0x3nnn*/0x32E8/*TRIS=0x02E8*/ ))
#define PULLUP_27       _CN71PUE
#define PULLDOWN_27      _CN71PDE
#define PPS_OUT_27      _RP16R
#define PPS_IN_27       16

#define UCPORT_28       F4
#define UCPORT_ID_28    UCPORT_ID_F4
#define PIN_28          _RF4
#define PIN_28_BITADR   ((volatile WORD)(/*Bit4=0x4nnn*/0x42EA/*PORTF=0x02EA*/ ))
#define LAT_28          _LATF4
#define LAT_28_BITADR   ((volatile WORD)(/*Bit4=0x4nnn*/0x42EC/*LATF=0x02EC*/ ))
#define DIR_28          _TRISF4
#define TRIS_28_BITADR  ((volatile WORD)(/*Bit4=0x4nnn*/0x42E8/*TRISF=0x02E8*/ ))
#define PULLUP_28       _CN17PUE
#define PULLDOWN_28      _CN17PDE
#define PPS_OUT_28      _RP10R
#define PPS_IN_28       10

#define UCPORT_29       F5
#define UCPORT_ID_29    UCPORT_ID_F5
#define PIN_29          _RF5
#define PIN_29_BITADR   ((volatile WORD)(/*Bit5=0x5nnn*/0x52EA/*PORTF=0x02EA*/ ))
#define LAT_29          _LATF5
#define LAT_29_BITADR   ((volatile WORD)(/*Bit5=0x5nnn*/0x52EC/*LATF=0x02EC*/ ))
#define DIR_29          _TRISF5
#define TRIS_29_BITADR  ((volatile WORD)(/*Bit5=0x5nnn*/0x52E8/*TRISF=0x02E8*/ ))
#define PULLUP_29       _CN18PUE
#define PULLDOWN_29      _CN18PDE
#define PPS_OUT_26      _RP2R
#define PPS_IN_29       2

#define UCPORT_30       B8
#define UCPORT_ID_30     UCPORT_ID_B8
#define PIN_30           _RB8
#define PIN_30_BITADR   ((volatile WORD)(/*Bit8=0x8nnn*/0x82CA/*PORTB=0x02CA*/ ))
#define LAT_30           _LATB8
#define LAT_30_BITADR   ((volatile WORD)(/*Bit8=0x8nnn*/0x82CC/*LATB=0x02CC*/ ))
#define DIR_30           _TRISB8
#define TRIS_30_BITADR  ((volatile WORD)(/*Bit8=0x8nnn*/0x82C8/*TRISB=0x02C8*/ ))
#define PULLUP_30        _CN26PUE
#define PULLDOWN_30       _CN26PDE
#define PPS_OUT_30       _RP8R
#define PPS_IN_30        8

#define UCPORT_31       B9
#define UCPORT_ID_31     UCPORT_ID_B9
#define PIN_31           _RB9
#define PIN_31_BITADR   ((volatile WORD)(/*Bit9=0x9nnn*/0x92CA/*PORTB=0x02CA*/ ))
#define LAT_31           _LATB9
#define LAT_31_BITADR   ((volatile WORD)(/*Bit9=0x9nnn*/0x92CC/*LATB=0x02CC*/ ))
#define DIR_31           _TRISB9
#define TRIS_31_BITADR  ((volatile WORD)(/*Bit9=0x9nnn*/0x92C8/*TRISB=0x02C8*/ ))
#define PULLUP_31        _CN27PUE
#define PULLDOWN_31       _CN27PDE
#define PPS_OUT_31       _RP9R
#define PPS_IN_31        9

#define UCPORT_32       B10
#define UCPORT_ID_32     UCPORT_ID_B10
#define PIN_32           _RB10
#define PIN_32_BITADR   ((volatile WORD)(/*Bit10=0xAnnn*/0xA2CA/*PORTB=0x02CA*/ ))
#define LAT_32           _LATB10
#define LAT_32_BITADR   ((volatile WORD)(/*Bit10=0xAnnn*/0xA2CC/*LATB=0x02CC*/ ))
#define DIR_32           _TRISB10
#define TRIS_32_BITADR  ((volatile WORD)(/*Bit10=0xAnnn*/0xA2C8/*TRISB=0x02C8*/ ))
#define PULLUP_32        _CN28PUE
#define PULLDOWN_32       _CN28PDE

#define UCPORT_33       B11
#define UCPORT_ID_33     UCPORT_ID_B11
#define PIN_33           _RB11
#define PIN_33_BITADR   ((volatile WORD)(/*Bit11=0xBnnn*/0xB2CA/*PORTB=0x02CA*/ ))
#define LAT_33           _LATB11
#define LAT_33_BITADR   ((volatile WORD)(/*Bit11=0xBnnn*/0xB2CC/*LATB=0x02CC*/ ))
#define DIR_33           _TRISB11
#define TRIS_33_BITADR  ((volatile WORD)(/*Bit11=0xBnnn*/0xB2C8/*TRISB=0x02C8*/ ))
#define PULLUP_33        _CN29PUE
#define PULLDOWN_33       _CN29PDE

#define UCPORT_34       B12
#define UCPORT_ID_34     UCPORT_ID_B14
#define PIN_34           _RB14
#define PIN_34_BITADR   ((volatile WORD)(/*Bit14=0xEnnn*/0xE2CA/*PORTB=0x02CA*/ ))
#define LAT_34           _LATB14
#define LAT_34_BITADR   ((volatile WORD)(/*Bit14=0xEnnn*/0xE2CC/*LATB=0x02CC*/ ))
#define DIR_34           _TRISB14
#define TRIS_34_BITADR  ((volatile WORD)(/*Bit14=0xEnnn*/0xE2C8/*TRISB=0x02C8*/ ))
#define PULLUP_34        _CN32PUE
#define PULLDOWN_34       _CN32PDE
#define PPS_OUT_34       _RP14R
#define PPS_IN_34        14

#define UCPORT_35       B15
#define UCPORT_ID_35     UCPORT_ID_B15
#define PIN_35           _RB15
#define PIN_35_BITADR   ((volatile WORD)(/*Bit15=0xFnnn*/0xF2CA/*PORTB=0x02CA*/ ))
#define LAT_35           _LATB15
#define LAT_35_BITADR   ((volatile WORD)(/*Bit15=0xFnnn*/0xF2CC/*LATB=0x02CC*/ ))
#define DIR_35           _TRISB15
#define TRIS_35_BITADR  ((volatile WORD)(/*Bit15=0xFnnn*/0xF2C8/*TRISB=0x02C8*/ ))
#define PULLUP_35        _CN12PUE
#define PULLDOWN_35       _CN12PDE
#define PPS_OUT_35       _RP29R
#define PPS_IN_35        29

#define UCPORT_36       D0
#define UCPORT_ID_36     UCPORT_ID_D0
#define PIN_36           _RD0
#define PIN_36_BITADR   ((volatile WORD)(/*Bit0=0x0nnn*/0x02DA/*PORTD=0x02DA*/ ))
#define LAT_36           _LATD0
#define LAT_36_BITADR   ((volatile WORD)(/*Bit0=0x0nnn*/0x02DC/*LATD=0x02DC*/ ))
#define DIR_36           _TRISD0
#define TRIS_36_BITADR  ((volatile WORD)(/*Bit0=0x0nnn*/0x02D8/*TRISD=0x02D8*/ ))
#define PULLUP_36        _CN49PUE
#define PULLDOWN_36       _CN49PDE
#define PPS_OUT_36       _RP11R
#define PPS_IN_36        11

#define UCPORT_37       D1
#define UCPORT_ID_37     UCPORT_ID_D1
#define PIN_37           _RD1
#define PIN_37_BITADR   ((volatile WORD)(/*Bit1=0x1nnn*/0x12DA/*PORTD=0x02DA*/ ))
#define LAT_37           _LATD1
#define LAT_37_BITADR   ((volatile WORD)(/*Bit1=0x1nnn*/0x12DC/*LATD=0x02DC*/ ))
#define DIR_37           _TRISD1
#define TRIS_37_BITADR  ((volatile WORD)(/*Bit1=0x1nnn*/0x12D8/*TRISD=0x02D8*/ ))
#define PULLUP_37        _CN50PUE
#define PULLDOWN_37       _CN50PDE
#define PPS_OUT_37       _RP24R
#define PPS_IN_37        24

#define UCPORT_38       D2
#define UCPORT_ID_38     UCPORT_ID_D2
#define PIN_38           _RD2
#define PIN_38_BITADR   ((volatile WORD)(/*Bit2=0x2nnn*/0x22DA/*PORTD=0x02DA*/ ))
#define LAT_38           _LATD2
#define LAT_38_BITADR   ((volatile WORD)(/*Bit2=0x2nnn*/0x22DC/*LATD=0x02DC*/ ))
#define DIR_38           _TRISD2
#define TRIS_38_BITADR  ((volatile WORD)(/*Bit2=0x2nnn*/0x22D8/*TRISD=0x02D8*/ ))
#define PULLUP_38        _CN51PUE
#define PULLDOWN_38       _CN51PDE
#define PPS_OUT_38       _RP23R
#define PPS_IN_38        23

#define UCPORT_39       D3
#define UCPORT_ID_39     UCPORT_ID_D3
#define PIN_39           _RD3
#define PIN_39_BITADR   ((volatile WORD)(/*Bit3=0x3nnn*/0x32DA/*PORTD=0x02DA*/ ))
#define LAT_39           _LATD3
#define LAT_39_BITADR   ((volatile WORD)(/*Bit3=0x3nnn*/0x32DC/*LATD=0x02DC*/ ))
#define DIR_39           _TRISD3
#define TRIS_39_BITADR  ((volatile WORD)(/*Bit3=0x3nnn*/0x32D8/*TRISD=0x02D8*/ ))
#define PULLUP_39        _CN52PUE
#define PULLDOWN_39       _CN52PDE
#define PPS_OUT_39       _RP22R
#define PPS_IN_39        22

#define UCPORT_40       D6
#define UCPORT_ID_40    UCPORT_ID_D6
#define PIN_40           _RD6
#define PIN_40_BITADR   ((volatile WORD)(/*Bit6=0x6nnn*/0x62DA/*PORTD=0x02DA*/ ))
#define LAT_40           _LATD6
#define LAT_40_BITADR   ((volatile WORD)(/*Bit6=0x6nnn*/0x62DC/*LATD=0x02DC*/ ))
#define DIR_40           _TRISD6
#define TRIS_40_BITADR  ((volatile WORD)(/*Bit6=0x6nnn*/0x62D8/*TRISD=0x02D8*/ ))
#define PULLUP_40        _CN15PUE
#define PULLDOWN_40       _CN15PDE

#define UCPORT_41       D7
#define UCPORT_ID_41    UCPORT_ID_D7
#define PIN_41           _RD7
#define PIN_41_BITADR   ((volatile WORD)(/*Bit7=0x7nnn*/0x72DA/*PORTD=0x02DA*/ ))
#define LAT_41           _LATD7
#define LAT_41_BITADR   ((volatile WORD)(/*Bit7=0x7nnn*/0x72DC/*LATD=0x02DC*/ ))
#define DIR_41           _TRISD7
#define TRIS_41_BITADR  ((volatile WORD)(/*Bit7=0x7nnn*/0x72D8/*TRISD=0x02D8*/ ))
#define PULLUP_41        _CN16PUE
#define PULLDOWN_41       _CN16PDE

//#define UCPORT_42     0  //Dont define is not available!
#define UCPORT_ID_42    UCPORT_ID_NA
#define PIN_42_BITADR   BITADR_NA
#define LAT_42_BITADR   BITADR_NA
#define TRIS_42_BITADR  BITADR_NA

//#define UCPORT_43     0  //Dont define is not available!
#define UCPORT_ID_43    UCPORT_ID_NA
#define PIN_43_BITADR   BITADR_NA
#define LAT_43_BITADR   BITADR_NA
#define TRIS_43_BITADR  BITADR_NA

//#define UCPORT_44     0  //Dont define is not available!
#define UCPORT_ID_44    UCPORT_ID_NA
#define PIN_44_BITADR   BITADR_NA
#define LAT_44_BITADR   BITADR_NA
#define TRIS_44_BITADR  BITADR_NA

//#define UCPORT_45     0  //Dont define is not available!
#define UCPORT_ID_45    UCPORT_ID_NA
#define PIN_45_BITADR   BITADR_NA
#define LAT_45_BITADR   BITADR_NA
#define TRIS_45_BITADR  BITADR_NA


//I2C ports
#define PORT_ID_SDA1     12
#define UCPORT_SDA1      UCPORT_12
#define UCPORT_ID_SDA1   UCPORT_ID_12
#define PIN_SDA1         PIN_12
#define PIN_SDA1_BITADR  PIN_12_BITADR
#define LAT_SDA1         LAT_12
#define LAT_SDA1_BITADR  LAT_12_BITADR
#define DIR_SDA1         DIR_12
#define TRIS_SDA1_BITADR TRIS_12_BITADR
#define PULLUP_SDA1      PULLUP_12
#define PULLDOWN_SDA1    PULLDOWN_12

#define PORT_ID_SCL1     13
#define UCPORT_SCL1      UCPORT_13
#define UCPORT_ID_SCL1   UCPORT_ID_13
#define PIN_SCL1         PIN_13
#define PIN_SCL1_BITADR  PIN_13_BITADR
#define LAT_SCL1         LAT_13
#define LAT_SCL1_BITADR  LAT_13_BITADR
#define DIR_SCL1         DIR_13
#define TRIS_SCL1_BITADR TRIS_13_BITADR
#define PULLUP_SCL1      PULLUP_13
#define PULLDOWN_SCL1    PULLDOWN_13

#define PORT_ID_SDA2     28
#define UCPORT_SDA2      UCPORT_28
#define UCPORT_ID_SDA2   UCPORT_ID_28
#define PIN_SDA2         PIN_28
#define PIN_SDA2_BITADR  PIN_28_BITADR
#define LAT_SDA2         LAT_28
#define LAT_SDA2_BITADR  LAT_28_BITADR
#define DIR_SDA2         DIR_28
#define TRIS_SDA2_BITADR TRIS_28_BITADR
#define PULLUP_SDA2      PULLUP_28
#define PULLDOWN_SDA2    PULLDOWN_28

#define PORT_ID_SCL2     29
#define UCPORT_SCL2      UCPORT_29
#define UCPORT_ID_SCL2   UCPORT_ID_29
#define PIN_SCL2         PIN_29
#define PIN_SCL2_BITADR  PIN_29_BITADR
#define LAT_SCL2         LAT_29
#define LAT_SCL2_BITADR  LAT_29_BITADR
#define DIR_SCL2         DIR_29
#define TRIS_SCL2_BITADR TRIS_29_BITADR
#define PULLUP_SCL2      PULLUP_29
#define PULLDOWN_SCL2    PULLDOWN_29

#define PORT_ID_SDA3     25
#define UCPORT_SDA3      UCPORT_25
#define UCPORT_ID_SDA3   UCPORT_ID_25
#define PIN_SDA3         PIN_25
#define PIN_SDA3_BITADR  PIN_25_BITADR
#define LAT_SDA3         LAT_25
#define LAT_SDA3_BITADR  LAT_25_BITADR
#define DIR_SDA3         DIR_25
#define TRIS_SDA3_BITADR TRIS_25_BITADR
#define PULLUP_SDA3      PULLUP_25
#define PULLDOWN_SDA3    PULLDOWN_25

#define PORT_ID_SCL3     24
#define UCPORT_SCL3      UCPORT_24
#define UCPORT_ID_SCL3   UCPORT_ID_24
#define PIN_SCL3         PIN_24
#define PIN_SCL3_BITADR  PIN_24_BITADR
#define LAT_SCL3         LAT_24
#define LAT_SCL3_BITADR  LAT_24_BITADR
#define DIR_SCL3         DIR_24
#define TRIS_SCL3_BITADR TRIS_24_BITADR
#define PULLUP_SCL3      PULLUP_24
#define PULLDOWN_SCL3    PULLDOWN_24



// *********************************************************************
// ---------------- Analog Netcruzer Port Names --------------------
// *********************************************************************
#define PIN_A0          PIN_00
#define PIN_A0_BITADR   PIN_00_BITADR
#define LAT_A0          LAT_00
#define LAT_A0_BITADR   LAT_00_BITADR
#define DIR_A0          DIR_00
#define TRIS_A0_BITADR  TRIS_00_BITADR
#define PULLUP_A0       PULLUP_00
#define PULLDOWN_A0      PULLDOWN_00

#define PIN_A1          PIN_01
#define PIN_A1_BITADR   PIN_01_BITADR
#define LAT_A1          LAT_01
#define LAT_A1_BITADR   LAT_01_BITADR
#define DIR_A1          DIR_01
#define TRIS_A1_BITADR  TRIS_01_BITADR
#define PULLUP_A1       PULLUP_01
#define PULLDOWN_A1      PULLDOWN_01

#define PIN_A2          PIN_02
#define PIN_A2_BITADR   PIN_02_BITADR
#define LAT_A2          LAT_02
#define LAT_A2_BITADR   LAT_02_BITADR
#define DIR_A2          DIR_02
#define TRIS_A2_BITADR  TRIS_02_BITADR
#define PULLUP_A2       PULLUP_02
#define PULLDOWN_A2      PULLDOWN_02

#define PIN_A3          PIN_03
#define PIN_A3_BITADR   PIN_03_BITADR
#define LAT_A3          LAT_03
#define LAT_A3_BITADR   LAT_03_BITADR
#define DIR_A3          DIR_03
#define TRIS_A3_BITADR  TRIS_03_BITADR
#define PULLUP_A3       PULLUP_03
#define PULLDOWN_A3      PULLDOWN_03

#define PIN_A4          PIN_04
#define PIN_A4_BITADR   PIN_04_BITADR
#define LAT_A4          LAT_04
#define LAT_A4_BITADR   LAT_04_BITADR
#define DIR_A4          DIR_04
#define TRIS_A4_BITADR  TRIS_04_BITADR
#define PULLUP_A4       PULLUP_04
#define PULLDOWN_A4      PULLDOWN_04

#define PIN_A5          PIN_05
#define PIN_A5_BITADR   PIN_05_BITADR
#define LAT_A5          LAT_05
#define LAT_A5_BITADR   LAT_05_BITADR
#define DIR_A5          DIR_05
#define TRIS_A5_BITADR  TRIS_05_BITADR
#define PULLUP_A5       PULLUP_05
#define PULLDOWN_A5      PULLDOWN_05

#define PIN_A6          PIN_30
#define PIN_A6_BITADR   PIN_30_BITADR
#define LAT_A6          LAT_30
#define LAT_A6_BITADR   LAT_30_BITADR
#define DIR_A6          DIR_30
#define TRIS_A6_BITADR  TRIS_30_BITADR
#define PULLUP_A6       PULLUP_30
#define PULLDOWN_A6      PULLDOWN_30

#define PIN_A7          PIN_31
#define PIN_A7_BITADR   PIN_31_BITADR
#define LAT_A7          LAT_31
#define LAT_A7_BITADR   LAT_31_BITADR
#define DIR_A7          DIR_31
#define TRIS_A7_BITADR  TRIS_31_BITADR
#define PULLUP_A7       PULLUP_31
#define PULLDOWN_A7      PULLDOWN_31

#define PIN_A8          PIN_32
#define PIN_A8_BITADR   PIN_32_BITADR
#define LAT_A8          LAT_32
#define LAT_A8_BITADR   LAT_32_BITADR
#define DIR_A8          DIR_32
#define TRIS_A8_BITADR  TRIS_32_BITADR
#define PULLUP_A8       PULLUP_32
#define PULLDOWN_A8      PULLDOWN_32

#define PIN_A9          PIN_33
#define PIN_A9_BITADR   PIN_33_BITADR
#define LAT_A9          LAT_33
#define LAT_A9_BITADR   LAT_33_BITADR
#define DIR_A9          DIR_33
#define TRIS_A9_BITADR  TRIS_33_BITADR
#define PULLUP_A9       PULLUP_33
#define PULLDOWN_A9      PULLDOWN_33

#define PIN_A10          PIN_34
#define PIN_A10_BITADR   PIN_34_BITADR
#define LAT_A10          LAT_34
#define LAT_A10_BITADR   LAT_34_BITADR
#define DIR_A10          DIR_34
#define TRIS_A10_BITADR  TRIS_34_BITADR
#define PULLUP_A10       PULLUP_34
#define PULLDOWN_A10      PULLDOWN_34

#define PIN_A11          PIN_35
#define PIN_A11_BITADR   PIN_35_BITADR
#define LAT_A11          LAT_35
#define LAT_A11_BITADR   LAT_35_BITADR
#define DIR_A11          DIR_35
#define TRIS_A11_BITADR  TRIS_35_BITADR
#define PULLUP_A11       PULLUP_35
#define PULLDOWN_A11      PULLDOWN_35


                    
// *********************************************************************
// ---------------- Old to New Netcruzer Port Names --------------------
// *********************************************************************
#define UCPORT_SC       UCPORT_13
#define UCPORT_ID_SC    UCPORT_ID_13
#define PIN_SC          PIN_13
#define PIN_SC_BITADR   PIN_13_BITADR
#define LAT_SC          LAT_13
#define LAT_SC_BITADR   LAT_13_BITADR
#define DIR_SC          DIR_13
#define TRIS_SC_BITADR  TRIS_13_BITADR
#define PULLUP_SC       PULLUP_13
#define PULLDOWN_SC      PULLDOWN_13
#define PPS_OUT_SC      PPS_OUT_13
#define PPS_IN_SC       PPS_IN_13

#define UCPORT_SD       UCPORT_12
#define UCPORT_ID_SD    UCPORT_ID_12
#define PIN_SD          PIN_12
#define PIN_SD_BITADR   PIN_12_BITADR
#define LAT_SD          LAT_12
#define LAT_SD_BITADR   LAT_12_BITADR
#define DIR_SD          DIR_12
#define TRIS_SD_BITADR  TRIS_12_BITADR
#define PULLUP_SD       PULLUP_12
#define PULLDOWN_SD      PULLDOWN_12
#define PPS_OUT_SD      PPS_OUT_12
#define PPS_IN_SD       PPS_IN_12

#define UCPORT_S0       UCPORT_24
#define UCPORT_ID_S0    UCPORT_ID_24
#define PIN_S0          PIN_24
#define PIN_S0_BITADR   PIN_24_BITADR
#define LAT_S0          LAT_24
#define LAT_S0_BITADR   LAT_24_BITADR
#define DIR_S0          DIR_24
#define TRIS_S0_BITADR  TRIS_24_BITADR
#define PULLUP_S0       PULLUP_24
#define PULLDOWN_S0      PULLDOWN_24
#define PPS_OUT_S0      PPS_OUT_24
#define PPS_IN_S0       PPS_IN_24

#define UCPORT_S1       UCPORT_25
#define UCPORT_ID_S1    UCPORT_ID_25
#define PIN_S1          PIN_25
#define PIN_S1_BITADR   PIN_25_BITADR
#define LAT_S1          LAT_25
#define LAT_S1_BITADR   LAT_25_BITADR
#define DIR_S1          DIR_25
#define TRIS_S1_BITADR  TRIS_25_BITADR
#define PULLUP_S1       PULLUP_25
#define PULLDOWN_S1      PULLDOWN_25
#define PPS_OUT_S1      PPS_OUT_25
#define PPS_IN_S1       PPS_IN_25

#define UCPORT_T0       UCPORT_20
#define UCPORT_ID_T0    UCPORT_ID_20
#define PIN_T0          PIN_20
#define PIN_T0_BITADR   PIN_20_BITADR
#define LAT_T0          LAT_20
#define LAT_T0_BITADR   LAT_20_BITADR
#define DIR_T0          DIR_20
#define TRIS_T0_BITADR  TRIS_20_BITADR
#define PULLUP_T0       PULLUP_20
#define PULLDOWN_T0      PULLDOWN_20
#define PPS_OUT_T0      PPS_OUT_20
#define PPS_IN_T0       PPS_IN_20

#define UCPORT_T1       UCPORT_21
#define UCPORT_ID_T1    UCPORT_ID_21
#define PIN_T1          PIN_21
#define PIN_T1_BITADR   PIN_21_BITADR
#define LAT_T1          LAT_21
#define LAT_T1_BITADR   LAT_21_BITADR
#define DIR_T1          DIR_21
#define TRIS_T1_BITADR  TRIS_21_BITADR
#define PULLUP_T1       PULLUP_21
#define PULLDOWN_T1      PULLDOWN_21
#define PPS_OUT_T1      PPS_OUT_21
#define PPS_IN_T1       PPS_IN_21

#define UCPORT_T2       UCPORT_22
#define UCPORT_ID_T2    UCPORT_ID_22
#define PIN_T2          PIN_22
#define PIN_T2_BITADR   PIN_22_BITADR
#define LAT_T2          LAT_22
#define LAT_T2_BITADR   LAT_22_BITADR
#define DIR_T2          DIR_22
#define TRIS_T2_BITADR  TRIS_22_BITADR
#define PULLUP_T2       PULLUP_22
#define PULLDOWN_T2      PULLDOWN_22
#define PPS_OUT_T2      PPS_OUT_22
#define PPS_IN_T2       PPS_IN_22

#define UCPORT_T3       UCPORT_23
#define UCPORT_ID_T3    UCPORT_ID_23
#define PIN_T3          PIN_23
#define PIN_T3_BITADR   PIN_23_BITADR
#define LAT_T3          LAT_23
#define LAT_T3_BITADR   LAT_23_BITADR
#define DIR_T3          DIR_23
#define TRIS_T3_BITADR  TRIS_23_BITADR
#define PULLUP_T3       PULLUP_23
#define PULLDOWN_T3      PULLDOWN_23
#define PPS_OUT_T3      PPS_OUT_23
#define PPS_IN_T3       PPS_IN_23

#define UCPORT_T4       UCPORT_26
#define UCPORT_ID_T4    UCPORT_ID_26
#define PIN_T4          PIN_26
#define PIN_T4_BITADR   PIN_26_BITADR
#define LAT_T4          LAT_26
#define LAT_T4_BITADR   LAT_26_BITADR
#define DIR_T4          DIR_26
#define TRIS_T4_BITADR  TRIS_26_BITADR
#define PULLUP_T4       PULLUP_26
#define PULLDOWN_T4      PULLDOWN_26
#define PPS_OUT_T4      PPS_OUT_26
#define PPS_IN_T4       PPS_IN_26

#define UCPORT_T5       UCPORT_27
#define UCPORT_ID_T5    UCPORT_ID_27
#define PIN_T5          PIN_27
#define PIN_T5_BITADR   PIN_27_BITADR
#define LAT_T5          LAT_27
#define LAT_T5_BITADR   LAT_27_BITADR
#define DIR_T5          DIR_27
#define TRIS_T5_BITADR  TRIS_27_BITADR
#define PULLUP_T5       PULLUP_27
#define PULLDOWN_T5      PULLDOWN_27
#define PPS_OUT_T5      PPS_OUT_27
#define PPS_IN_T5       PPS_IN_27

#define UCPORT_T6       UCPORT_28
#define UCPORT_ID_T6    UCPORT_ID_28
#define PIN_T6          PIN_28
#define PIN_T6_BITADR   PIN_28_BITADR
#define LAT_T6          LAT_28
#define LAT_T6_BITADR   LAT_28_BITADR
#define DIR_T6          DIR_28
#define TRIS_T6_BITADR  TRIS_28_BITADR
#define PULLUP_T6       PULLUP_28
#define PULLDOWN_T6      PULLDOWN_28
#define PPS_OUT_T6      PPS_OUT_28
#define PPS_IN_T6       PPS_IN_28

#define UCPORT_T7       UCPORT_29
#define UCPORT_ID_T7    UCPORT_ID_29
#define PIN_T7          PIN_29
#define PIN_T7_BITADR   PIN_29_BITADR
#define LAT_T7          LAT_29
#define LAT_T7_BITADR   LAT_29_BITADR
#define DIR_T7          DIR_29
#define TRIS_T7_BITADR  TRIS_29_BITADR
#define PULLUP_T7       PULLUP_29
#define PULLDOWN_T7      PULLDOWN_29
#define PPS_OUT_T7      PPS_OUT_29
#define PPS_IN_T7       PPS_IN_29

#define UCPORT_X0       UCPORT_00
#define UCPORT_ID_X0    UCPORT_ID_00
#define PIN_X0          PIN_00
#define PIN_X0_BITADR   PIN_00_BITADR
#define LAT_X0          LAT_00
#define LAT_X0_BITADR   LAT_00_BITADR
#define DIR_X0          DIR_00
#define TRIS_X0_BITADR  TRIS_00_BITADR
#define PULLUP_X0       PULLUP_00
#define PULLDOWN_X0      PULLDOWN_00
#define PPS_OUT_X0      PPS_OUT_00
#define PPS_IN_X0       PPS_IN_00

#define UCPORT_X1       UCPORT_01
#define UCPORT_ID_X1    UCPORT_ID_01
#define PIN_X1          PIN_01
#define PIN_X1_BITADR   PIN_01_BITADR
#define LAT_X1          LAT_01
#define LAT_X1_BITADR   LAT_01_BITADR
#define DIR_X1          DIR_01
#define TRIS_X1_BITADR  TRIS_01_BITADR
#define PULLUP_X1       PULLUP_01
#define PULLDOWN_X1      PULLDOWN_01
#define PPS_OUT_X1      PPS_OUT_01
#define PPS_IN_X1       PPS_IN_01

#define UCPORT_X2       UCPORT_02
#define UCPORT_ID_X2    UCPORT_ID_02
#define PIN_X2          PIN_02
#define PIN_X2_BITADR   PIN_02_BITADR
#define LAT_X2          LAT_02
#define LAT_X2_BITADR   LAT_02_BITADR
#define DIR_X2          DIR_02
#define TRIS_X2_BITADR  TRIS_02_BITADR
#define PULLUP_X2       PULLUP_02
#define PULLDOWN_X2      PULLDOWN_02
#define PPS_OUT_X2      PPS_OUT_02
#define PPS_IN_X2       PPS_IN_02

#define UCPORT_X3       UCPORT_03
#define UCPORT_ID_X3    UCPORT_ID_03
#define PIN_X3          PIN_03
#define PIN_X3_BITADR   PIN_03_BITADR
#define LAT_X3          LAT_03
#define LAT_X3_BITADR   LAT_03_BITADR
#define DIR_X3          DIR_03
#define TRIS_X3_BITADR  TRIS_03_BITADR
#define PULLUP_X3       PULLUP_03
#define PULLDOWN_X3      PULLDOWN_03
#define PPS_OUT_X3      PPS_OUT_03
#define PPS_IN_X3       PPS_IN_03

#define UCPORT_X4       UCPORT_04
#define UCPORT_ID_X4    UCPORT_ID_04
#define PIN_X4          PIN_04
#define PIN_X4_BITADR   PIN_04_BITADR
#define LAT_X4          LAT_04
#define LAT_X4_BITADR   LAT_04_BITADR
#define DIR_X4          DIR_04
#define TRIS_X4_BITADR  TRIS_04_BITADR
#define PULLUP_X4       PULLUP_04
#define PULLDOWN_X4      PULLDOWN_04
#define PPS_OUT_X4      PPS_OUT_04
#define PPS_IN_X4       PPS_IN_04

#define UCPORT_X5       UCPORT_05
#define UCPORT_ID_X5    UCPORT_ID_05
#define PIN_X5          PIN_05
#define PIN_X5_BITADR   PIN_05_BITADR
#define LAT_X5          LAT_05
#define LAT_X5_BITADR   LAT_05_BITADR
#define DIR_X5          DIR_05
#define TRIS_X5_BITADR  TRIS_05_BITADR
#define PULLUP_X5       PULLUP_05
#define PULLDOWN_X5      PULLDOWN_05
#define PPS_OUT_X5      PPS_OUT_05
#define PPS_IN_X5       PPS_IN_05

#define UCPORT_X6       UCPORT_06
#define UCPORT_ID_X6    UCPORT_ID_06
#define PIN_X6          PIN_06
#define PIN_X6_BITADR   PIN_06_BITADR
#define LAT_X6          LAT_06
#define LAT_X6_BITADR   LAT_06_BITADR
#define DIR_X6          DIR_06
#define TRIS_X6_BITADR  TRIS_06_BITADR
#define PULLUP_X6       PULLUP_06
#define PULLDOWN_X6      PULLDOWN_06
#define PPS_OUT_X6      PPS_OUT_06
#define PPS_IN_X6       PPS_IN_06

#define UCPORT_X7       UCPORT_07
#define UCPORT_ID_X7    UCPORT_ID_07
#define PIN_X7          PIN_07
#define PIN_X7_BITADR   PIN_07_BITADR
#define LAT_X7          LAT_07
#define LAT_X7_BITADR   LAT_07_BITADR
#define DIR_X7          DIR_07
#define TRIS_X7_BITADR  TRIS_07_BITADR
#define PULLUP_X7       PULLUP_07
#define PULLDOWN_X7      PULLDOWN_07
#define PPS_OUT_X7      PPS_OUT_07
#define PPS_IN_X7       PPS_IN_07

#define UCPORT_X8       UCPORT_08
#define UCPORT_ID_X8    UCPORT_ID_08
#define PIN_X8          PIN_08
#define PIN_X8_BITADR   PIN_08_BITADR
#define LAT_X8          LAT_08
#define LAT_X8_BITADR   LAT_08_BITADR
#define DIR_X8          DIR_08
#define TRIS_X8_BITADR  TRIS_08_BITADR
#define PULLUP_X8       PULLUP_08
#define PULLDOWN_X8      PULLDOWN_08
#define PPS_OUT_X8      PPS_OUT_08
#define PPS_IN_X8       PPS_IN_08

#define UCPORT_X9       UCPORT_09
#define UCPORT_ID_X9    UCPORT_ID_09
#define PIN_X9          PIN_09
#define PIN_X9_BITADR   PIN_09_BITADR
#define LAT_X9          LAT_09
#define LAT_X9_BITADR   LAT_09_BITADR
#define DIR_X9          DIR_09
#define TRIS_X9_BITADR  TRIS_09_BITADR
#define PULLUP_X9       PULLUP_09
#define PULLDOWN_X9      PULLDOWN_09
#define PPS_OUT_X9      PPS_OUT_09
#define PPS_IN_X9       PPS_IN_09

#define UCPORT_X10       UCPORT_10
#define UCPORT_ID_X10    UCPORT_ID_10
#define PIN_X10          PIN_10
#define PIN_X10_BITADR   PIN_10_BITADR
#define LAT_X10          LAT_10
#define LAT_X10_BITADR   LAT_10_BITADR
#define DIR_X10          DIR_10
#define TRIS_X10_BITADR  TRIS_10_BITADR
#define PULLUP_X10       PULLUP_10
#define PULLDOWN_X10      PULLDOWN_10
#define PPS_OUT_X10      PPS_OUT_10
#define PPS_IN_X10       PPS_IN_10

#define UCPORT_X11       UCPORT_11
#define UCPORT_ID_X11    UCPORT_ID_11
#define PIN_X11          PIN_11
#define PIN_X11_BITADR   PIN_11_BITADR
#define LAT_X11          LAT_11
#define LAT_X11_BITADR   LAT_11_BITADR
#define DIR_X11          DIR_11
#define TRIS_X11_BITADR  TRIS_11_BITADR
#define PULLUP_X11       PULLUP_11
#define PULLDOWN_X11      PULLDOWN_11
#define PPS_OUT_X11      PPS_OUT_11
#define PPS_IN_X11       PPS_IN_11

#define UCPORT_Y0       UCPORT_30
#define UCPORT_ID_Y0    UCPORT_ID_30
#define PIN_Y0          PIN_30
#define PIN_Y0_BITADR   PIN_30_BITADR
#define LAT_Y0          LAT_30
#define LAT_Y0_BITADR   LAT_30_BITADR
#define DIR_Y0          DIR_30
#define TRIS_Y0_BITADR  TRIS_30_BITADR
#define PULLUP_Y0       PULLUP_30
#define PULLDOWN_Y0      PULLDOWN_30
#define PPS_OUT_Y0      PPS_OUT_30
#define PPS_IN_Y0       PPS_IN_30

#define UCPORT_Y1       UCPORT_31
#define UCPORT_ID_Y1    UCPORT_ID_31
#define PIN_Y1          PIN_31
#define PIN_Y1_BITADR   PIN_31_BITADR
#define LAT_Y1          LAT_31
#define LAT_Y1_BITADR   LAT_31_BITADR
#define DIR_Y1          DIR_31
#define TRIS_Y1_BITADR  TRIS_31_BITADR
#define PULLUP_Y1       PULLUP_31
#define PULLDOWN_Y1      PULLDOWN_31
#define PPS_OUT_Y1      PPS_OUT_31
#define PPS_IN_Y1       PPS_IN_31

#define UCPORT_Y2       UCPORT_32
#define UCPORT_ID_Y2    UCPORT_ID_32
#define PIN_Y2          PIN_32
#define PIN_Y2_BITADR   PIN_32_BITADR
#define LAT_Y2          LAT_32
#define LAT_Y2_BITADR   LAT_32_BITADR
#define DIR_Y2          DIR_32
#define TRIS_Y2_BITADR  TRIS_32_BITADR
#define PULLUP_Y2       PULLUP_32
#define PULLDOWN_Y2      PULLDOWN_32
#define PPS_OUT_Y2      PPS_OUT_32
#define PPS_IN_Y2       PPS_IN_32

#define UCPORT_Y3       UCPORT_33
#define UCPORT_ID_Y3    UCPORT_ID_33
#define PIN_Y3          PIN_33
#define PIN_Y3_BITADR   PIN_33_BITADR
#define LAT_Y3          LAT_33
#define LAT_Y3_BITADR   LAT_33_BITADR
#define DIR_Y3          DIR_33
#define TRIS_Y3_BITADR  TRIS_33_BITADR
#define PULLUP_Y3       PULLUP_33
#define PULLDOWN_Y3      PULLDOWN_33
#define PPS_OUT_Y3      PPS_OUT_33
#define PPS_IN_Y3       PPS_IN_33

#define UCPORT_Y4       UCPORT_34
#define UCPORT_ID_Y4    UCPORT_ID_34
#define PIN_Y4          PIN_34
#define PIN_Y4_BITADR   PIN_34_BITADR
#define LAT_Y4          LAT_34
#define LAT_Y4_BITADR   LAT_34_BITADR
#define DIR_Y4          DIR_34
#define TRIS_Y4_BITADR  TRIS_34_BITADR
#define PULLUP_Y4       PULLUP_34
#define PULLDOWN_Y4      PULLDOWN_34
#define PPS_OUT_Y4      PPS_OUT_34
#define PPS_IN_Y4       PPS_IN_34

#define UCPORT_Y5       UCPORT_35
#define UCPORT_ID_Y5    UCPORT_ID_35
#define PIN_Y5          PIN_35
#define PIN_Y5_BITADR   PIN_35_BITADR
#define LAT_Y5          LAT_35
#define LAT_Y5_BITADR   LAT_35_BITADR
#define DIR_Y5          DIR_35
#define TRIS_Y5_BITADR  TRIS_35_BITADR
#define PULLUP_Y5       PULLUP_35
#define PULLDOWN_Y5      PULLDOWN_35
#define PPS_OUT_Y5      PPS_OUT_35
#define PPS_IN_Y5       PPS_IN_35

#define UCPORT_Y6       UCPORT_36
#define UCPORT_ID_Y6    UCPORT_ID_36
#define PIN_Y6          PIN_36
#define PIN_Y6_BITADR   PIN_36_BITADR
#define LAT_Y6          LAT_36
#define LAT_Y6_BITADR   LAT_36_BITADR
#define DIR_Y6          DIR_36
#define TRIS_Y6_BITADR  TRIS_36_BITADR
#define PULLUP_Y6       PULLUP_36
#define PULLDOWN_Y6      PULLDOWN_36
#define PPS_OUT_Y6      PPS_OUT_36
#define PPS_IN_Y6       PPS_IN_36

#define UCPORT_Y7       UCPORT_37
#define UCPORT_ID_Y7    UCPORT_ID_37
#define PIN_Y7          PIN_37
#define PIN_Y7_BITADR   PIN_37_BITADR
#define LAT_Y7          LAT_37
#define LAT_Y7_BITADR   LAT_37_BITADR
#define DIR_Y7          DIR_37
#define TRIS_Y7_BITADR  TRIS_37_BITADR
#define PULLUP_Y7       PULLUP_37
#define PULLDOWN_Y7      PULLDOWN_37
#define PPS_OUT_Y7      PPS_OUT_37
#define PPS_IN_Y7       PPS_IN_37

#define UCPORT_Y8       UCPORT_38
#define UCPORT_ID_Y8    UCPORT_ID_38
#define PIN_Y8          PIN_38
#define PIN_Y8_BITADR   PIN_38_BITADR
#define LAT_Y8          LAT_38
#define LAT_Y8_BITADR   LAT_38_BITADR
#define DIR_Y8          DIR_38
#define TRIS_Y8_BITADR  TRIS_38_BITADR
#define PULLUP_Y8       PULLUP_38
#define PULLDOWN_Y8      PULLDOWN_38
#define PPS_OUT_Y8      PPS_OUT_38
#define PPS_IN_Y8       PPS_IN_38

#define UCPORT_Y9       UCPORT_39
#define UCPORT_ID_Y9    UCPORT_ID_39
#define PIN_Y9          PIN_39
#define PIN_Y9_BITADR   PIN_39_BITADR
#define LAT_Y9          LAT_39
#define LAT_Y9_BITADR   LAT_39_BITADR
#define DIR_Y9          DIR_39
#define TRIS_Y9_BITADR  TRIS_39_BITADR
#define PULLUP_Y9       PULLUP_39
#define PULLDOWN_Y9      PULLDOWN_39
#define PPS_OUT_Y9      PPS_OUT_39
#define PPS_IN_Y9       PPS_IN_39

#define UCPORT_Y10       UCPORT_40
#define UCPORT_ID_Y10    UCPORT_ID_40
#define PIN_Y10          PIN_40
#define PIN_Y10_BITADR   PIN_40_BITADR
#define LAT_Y10          LAT_40
#define LAT_Y10_BITADR   LAT_40_BITADR
#define DIR_Y10          DIR_40
#define TRIS_Y10_BITADR  TRIS_40_BITADR
#define PULLUP_Y10       PULLUP_40
#define PULLDOWN_Y10      PULLDOWN_40
#define PPS_OUT_Y10      PPS_OUT_40
#define PPS_IN_Y10       PPS_IN_40

#define UCPORT_Y11       UCPORT_41
#define UCPORT_ID_Y11    UCPORT_ID_41
#define PIN_Y11          PIN_41
#define PIN_Y11_BITADR   PIN_41_BITADR
#define LAT_Y11          LAT_41
#define LAT_Y11_BITADR   LAT_41_BITADR
#define DIR_Y11          DIR_41
#define TRIS_Y11_BITADR  TRIS_41_BITADR
#define PULLUP_Y11       PULLUP_41
#define PULLDOWN_Y11      PULLDOWN_41
#define PPS_OUT_Y11      PPS_OUT_41
#define PPS_IN_Y11       PPS_IN_41



// *********************************************************************
// --------------------------- Other IO Defines ------------------------
// *********************************************************************
#define PIN_SYSLED      _RB6
#define LAT_SYSLED      _LATB6
#define DIR_SYSLED      _TRISB6
#define PULLUP_SYSLED   _CN24PUE
#define PULLDOWN_SYSLED  _CN24PDE



// *********************************************************************
// -------------------- Some general CPU Defines -----------------------
// *********************************************************************
#define PWM_COUNT       9   //Hardware PWM channels



// *********************************************************************
// ---------------------------- Analog Defines -------------------------
// *********************************************************************
#define ADC_REF_MV       2500   //ADC External Reference in MV
#define ADC_MAX_CHAN_BRD 12     //Maximum number of possible ADC channels, B0-B5, B8-B11, B14, B15 (rev2+)

//The channel mask indicates what AN channels can be used for ADC inputs
#define ADC_CHANNEL_MASK_LOW_BRD  0b1100111100111111    //AN0-AN5, AN8-AN11, AN14-AN15
#define ADC_CHANNEL_MASK_HIGH_BRD 0                     //Only has 16 ADC channels

//Index for selected analog input in adcFilter[] and adcValue[] arrays. Some analog inputs do not have space assigned for them
#define ADC_CH_A0       0   //AN0
#define ADC_CH_A1       1   //AN1
#define ADC_CH_A2       2   //AN2
#define ADC_CH_A3       3   //AN3
#define ADC_CH_A4       4   //AN4
#define ADC_CH_A5       5   //AN5
#define ADC_CH_A6       8   //AN8
#define ADC_CH_A7       9   //AN9
#define ADC_CH_A8       10  //AN10
#define ADC_CH_A9       11  //AN11
#define ADC_CH_A10       14  //AN14
#define ADC_CH_A11       15  //AN15

#define ADC_OPEN_A0     0x0001  //AN0
#define ADC_OPEN_A1     0x0002  //AN1
#define ADC_OPEN_A2     0x0004  //AN2
#define ADC_OPEN_A3     0x0008  //AN3
#define ADC_OPEN_A4     0x0010  //AN4
#define ADC_OPEN_A5     0x0020  //AN5
#define ADC_OPEN_A6     0x0100  //AN8
#define ADC_OPEN_A7     0x0200  //AN9
#define ADC_OPEN_A8     0x0400  //AN10
#define ADC_OPEN_A9     0x0800  //AN11
#define ADC_OPEN_A10     0x4000  //AN14
#define ADC_OPEN_A11     0x8000  //AN15


// *********************************************************************
// ------------------ Define ports for this board ----------------------
// *********************************************************************
#define UC_PORT_B0_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B1_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B2_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B3_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B4_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B5_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B6_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B7_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B8_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B9_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B10_PROP    (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B11_PROP    (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B14_PROP    (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_B15_PROP    (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_ANALOG_IN | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)

#define UC_PORT_D0_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V | UCPORT_PROP_INT)
#define UC_PORT_D1_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)
#define UC_PORT_D2_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)
#define UC_PORT_D3_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)
#define UC_PORT_D6_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_D7_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_D9_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V | UCPORT_PROP_SDA)
#define UC_PORT_D10_PROP    (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V | UCPORT_PROP_SCL)

#define UC_PORT_E2_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)
#define UC_PORT_E3_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)
#define UC_PORT_E5_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)
#define UC_PORT_E6_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)
#define UC_PORT_E7_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)

#define UC_PORT_F0_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_F1_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL |                   UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)
#define UC_PORT_F3_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V)
#define UC_PORT_F4_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V | UCPORT_PROP_SDA)
#define UC_PORT_F5_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN | UCPORT_PROP_5V | UCPORT_PROP_SCL)
    //G6, F7,G8,G9
#define UC_PORT_G6_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_G7_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_G8_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)
#define UC_PORT_G9_PROP     (UCPORT_PROP_DIG_IN | UCPORT_PROP_DIG_OUT | UCPORT_PROP_OPEN_COL | UCPORT_PROP_RP | UCPORT_PROP_PULLUP | UCPORT_PROP_PULLDOWN)


// *********************************************************************
// --------------------- spiFlash Configuration ------------------------
// *********************************************************************
#define SPI_FLASH_SECTOR_SIZE       (4096ul)
//#define SPI_FLASH_PAGE_SIZE       (0ul)           // SST has no page boundary requirements
#define SPI_FLASH_PAGE_SIZE         (256ul)         // Winbond FLASH has 256 byte pages
#define SPI_FLASH_SIZE              (0x400000ul)    // Flash is 4MByte



// *********************************************************************
// ----------------- SPI External FLASH and EEPROM ---------------------
// *********************************************************************
// To use SST (SST25VF016B) flash, SPI_FLASH_SECTOR_SIZE is 4096, and define SPI_FLASH_PAGE_SIZE as 0.
// To use Winbond flash, SPI_FLASH_SECTOR_SIZE is 4096, and define SPI_FLASH_PAGE_SIZE as 256.
#define SPIFLASH_CS_TRIS        (TRISBbits.TRISB13)
#define SPIFLASH_CS_IO          (LATBbits.LATB13)

// External EEPROM I/O pins
#define EEPROM_CS_TRIS          (TRISBbits.TRISB12)
#define EEPROM_CS_IO            (LATBbits.LATB12)

// To use Winbond flash, SPI_FLASH_SECTOR_SIZE is 4096, and define SPI_FLASH_PAGE_SIZE as 256.
#define SPIFLASH_SPI_IF         (IFS2bits.SPI2IF)
#define SPIFLASH_SSPBUF         (SPI2BUF)
//#define SPIFLASH_SPICON1        (SPI2CON1)
//#define SPIFLASH_SPICON1bits    (SPI2CON1bits)
//#define SPIFLASH_SPICON2        (SPI2CON2)
#define SPIFLASH_SPISTAT        (SPI2STAT)
#define SPIFLASH_SPISTATbits    (SPI2STATbits)

// External EEPROM
#define EEPROM_SPI_IF           (IFS2bits.SPI2IF)
#define EEPROM_SSPBUF           (SPI2BUF)
//#define EEPROM_SPICON1          (SPI2CON1)
//#define EEPROM_SPICON1bits      (SPI2CON1bits)
//#define EEPROM_SPICON2          (SPI2CON2)
#define EEPROM_SPISTAT          (SPI2STAT)
#define EEPROM_SPISTATbits      (SPI2STATbits)



// *********************************************************************
// ----------- SPI External Mememory Bus Defines for SBC66UAL -----------
// *********************************************************************
#define SPIMEM_SPI_IF           (IFS2bits.SPI2IF)
#define SPIMEM_SSPBUF           (SPI2BUF)
#define SPIMEM_SPICON1          (SPI2CON1)
#define SPIMEM_SPICON1bits      (SPI2CON1bits)
#define SPIMEM_SPICON2          (SPI2CON2)
#define SPIMEM_SPISTAT          (SPI2STAT)
#define SPIMEM_SPISTATbits      (SPI2STATbits)

// These defines are for the SPI bus used for the external FLASH and EEPROM
#define PIN_SPIMEM_SDO          _RD8
#define LAT_SPIMEM_SDO          _LATD8
#define DIR_SPIMEM_SDO          _TRISD8
#define PULLUP_SPIMEM_SDO       _CN53PUE
#define PULLDOWN_SPIMEM_SDO      _CN53PDE
#define PPS_OUT_SPIMEM_SDO      OUT_PIN_PPS_RP2

#define PIN_SPIMEM_SCK          _RD11
#define LAT_SPIMEM_SCK          _LATD11
#define DIR_SPIMEM_SCK          _TRISD11
#define PULLUP_SPIMEM_SCK       _CN56PUE
#define PULLDOWN_SPIMEM_SCK      _CN56PDE
#define PPS_OUT_SPIMEM_SCK      OUT_PIN_PPS_RP12

#define PIN_SPIMEM_SDI          _RC14
#define LAT_SPIMEM_SDI          _LATC14
#define DIR_SPIMEM_SDI          _TRISC14
#define PULLUP_SPIMEM_SDI       _CN0PUE
#define PULLDOWN_SPIMEM_SDI      _CN0PDE
#define PPS_IN_SPIMEM_SDI       IN_PIN_PPS_RPI37



// *********************************************************************
// -------------- USB stack hardware selection options -----------------
// *********************************************************************
//This section is the set of definitions required by the MCHPFSUSB framework. These definitions
//tell the firmware what mode it is running in, and are required by every application developed
//with this revision of the MCHPFSUSB framework.

//#define USE_SELF_POWER_SENSE_IO
//#define tris_self_power       TRISAbits.TRISA2    // Input
#define self_power              1

//#define USE_USB_BUS_SENSE_IO
//#define tris_usb_bus_sense    TRISBbits.TRISB5    // Input
#define USB_BUS_SENSE           U1OTGSTATbits.SESVD



// *******************************************************
// ----------------- xEeprom Configuration -----------------
// ********************************************************
#define XEEPROM_SIZE            (8192)      //Default EEPROM is 25LC640 = 64kBits = 8kBytes)
#define XEEPROM_PAGE_SIZE       (32)


/**
 * Get the UCPORT_PROP offset in CFG_STRUCT for the given "Microcontroller Port".
 * The returned value can be used as the offset parameter for cfgGetXxx and cfgSetXxx functions.
 * For example:
 * UCPORT_PROP ucportProp;
 * cfgGetDfltArray(GET_UCPORT_PROP_OFFSET(UCPORT_01), (BYTE *)&ucportProp, sizeof(ucportProp));
 *
 * @param ucPort Is the element name in the TABLE_BLOCK_UC66_PORT structures.
 *          Use the UCPORT_xx defines, for example UCPORT_05
 */
#define GET_UCPORT_PROP_OFFSET(ucPort) (offsetof(CFG_STRUCT, ucPortProp) + ((offsetof(TABLE_BLOCK_UC66_PORT, ucPort)/sizeof(UCPORT_PROP)) * sizeof(UCPORT_PROP)) )

/**
 * Get the UCPORT_CONFIG offset in CFG_STRUCT for the given "Microcontroller Port".
 * The returned value can be used as the offset parameter for cfgGetXxx and cfgSetXxx functions.
 * For example:
 * UCPORT_CONFIG ucportConfig;
 * cfgGetArray(GET_UCPORT_CONFIG_OFFSET(UCPORT_01), (BYTE *)&ucportConfig, sizeof(ucportConfig));
 *
 * @param ucPortVal Is the element name in the CFG_BLOCK_UC66_PORT structures.
 *          Use the UCPORT_xx defines, for example UCPORT_05
 */
#define GET_UCPORT_CONFIG_OFFSET(ucPortVal) (offsetof(CFG_STRUCT, ucPort) + ((offsetof(CFG_BLOCK_UC66_PORT, ucPortVal)/sizeof(UCPORT_CONFIG)) * sizeof(UCPORT_CONFIG)) )

#endif