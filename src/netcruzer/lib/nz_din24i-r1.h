/**
 * @brief           Contains hardware specific defines for DIN24I Revision 1 board.
 * @file            nz_din24i-r1.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section nz_din24i_r1_desc Description
 *****************************************
 * This module includes hardware specific defines for DIN24I Revision 1 board.
 *
 **********************************************************************
 * @section nz_din24i_r1_lic Software License Agreement
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
 * 2012-08-06, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef DIN24I_R1_H
#define DIN24I_R1_H


/**
 * Set configuration fuses to default values. If this is done in app, define CONFIGURATION_FUSES_SET
 * to prevent them being set here again.
 */
#if defined(THIS_IS_MAIN_FILE)
    #if defined(__PIC24FJ256GB110__)

        #if !defined(CONFIGURATION_FUSES_SET)
            #define CONFIGURATION_FUSES_SET

            //Are not defined in p24FJ128GB106
            #define DEBUG_ON              0x77FF
            #define DEBUG_OFF             0x7FFF

            // - JTAG Port Disabled (JTAGEN_OFF)
            // - General Segment Code Protect = Code protection is enabled (GCP_OFF)
            // - General Segment Write Protect Disabled (GWRP_OFF)
            // - Emulator functions are shared with PGEC2/PGED2 (ICS_PGx2)
            // - Watchdog Timer Disabled (FWDTEN_OFF)
            // - WDT Prescaler = 32 (FWPSA_PR32)
            // - Watchdog Timer Postscaler = 1:32,768 (WDTPS_PS32768)
            _CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx2 & FWDTEN_OFF) 

            // - Two Speed Start-up = off (IESO_OFF)
            // - 96MHz PLL Prescaler = No Divide, for 4MHz input (PLLDIV_NODIV)
            // - Oscillator Selection = Primary oscillator (XT, HS, EC) w/ PLL (FNOSC_PRIPLL)
            // - Clock switching and clock monitor = Both disabled (FCKSM_CSDCMD)
            // - OSCO/RC15 function = RC15 (OSCIOFNC_ON)
            // - RP Register Protection = Unlimited Writes To RP Registers (IOL1WAY_OFF)
            // - Disable Internal USB Voltage Regulator = Disabled (DISUVREG_OFF)
            // - Oscillator Selection = External Clock (POSCMOD_EC)
            _CONFIG2(IESO_OFF & PLLDIV_NODIV & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_ON & IOL1WAY_OFF & DISUVREG_OFF & POSCMOD_EC)

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
        #define GetPeripheralClock()    GetInstructionClock()
#endif



// *********************************************************************
// ----------- Daughter Board Connector Defines for SBC66ECL ------------
// *********************************************************************



// *********************************************************************
// --------------------------- Other IO Defines ------------------------
// *********************************************************************
#define PIN_SYSLED      _RB6
#define LAT_SYSLED      _LATB6
#define DIR_SYSLED      _TRISB6
#define PULLUP_SYSLED   _CN24PUE
#define PULLDOWN_SYSLED  _CN24PDE



// *********************************************************************
// ---------------------------- Analog Defines -------------------------
// *********************************************************************
#define ADC_REF_MV      2500    //ADC External Reference in MV

//PIC24FJ256GB110 has 14 ADC channels
//AN0-AN5 (B0-B5), AN8-AN15 (B8-B15)
#define ADC_MAX_CHAN_BRD   14
//The channel mask indicates what AN channels can be used for ADC inputs
#define ADC_CHANNEL_MASK_LOW_BRD  0b1100111100111111    //AN0-AN5, AN8-AN11, AN14-AN15
#define ADC_CHANNEL_MASK_HIGH_BRD 0                     //Only has 16 ADC channels

//PIC24FJ256GB210 has 24 ADC channels, can use following ones
//AN0-AN5 (B0-B5), AN8-AN15 (B8-B15), AN16 (C4), AN19 (G8), AN20 (G9), AN21 (E9), AN22 (A6)
//#define ADC_MAX_CHAN_BRD   19
//The channel mask indicates what AN channels can be used for ADC inputs
//#define ADC_CHANNEL_MASK_LOW_BRD  0b1111111100111111    //AN0-AN5, AN8-AN15
//#define ADC_CHANNEL_MASK_HIGH_BRD 0b0000000001111001    //AN16, AN19, AN20, AN21, AN22

//Index for selected analog input in adcFilter[] and adcValue[] arrays. Some analog inputs do not have space assigned for them
//TODO - Replace with port names! For example ADC_CH_A0 in stead of ADC_CH_AN0
#define ADC_CH_AN0      0
#define ADC_CH_AN1      1
#define ADC_CH_AN2      2
#define ADC_CH_AN3      3
#define ADC_CH_AN4      4
#define ADC_CH_AN5      5
#define ADC_CH_AN8      6
#define ADC_CH_AN9      7
#define ADC_CH_AN10     8
#define ADC_CH_AN11     9
#define ADC_CH_AN12     10
#define ADC_CH_AN13     11
#define ADC_CH_AN14     12
#define ADC_CH_AN15     13
#define ADC_CH_AN16     14
#define ADC_CH_AN19     15
#define ADC_CH_AN20     16
#define ADC_CH_AN21     17
#define ADC_CH_AN22     18

#define ADC_OPEN_AN0    0x0001  //AN0
#define ADC_OPEN_AN1    0x0002  //AN1
#define ADC_OPEN_AN2    0x0004  //AN2
#define ADC_OPEN_AN3    0x0008  //AN3
#define ADC_OPEN_AN4    0x0010  //AN4
#define ADC_OPEN_AN5    0x0020  //AN5
#define ADC_OPEN_AN8    0x0100  //AN8
#define ADC_OPEN_AN9    0x0200  //AN9
#define ADC_OPEN_AN10   0x0400  //AN10
#define ADC_OPEN_AN11   0x0800  //AN11
#define ADC_OPEN_AN12   0x1000  //AN12
#define ADC_OPEN_AN13   0x2000  //AN13
#define ADC_OPEN_AN14   0x4000  //AN14
#define ADC_OPEN_AN15   0x8000  //AN15




// *********************************************************************
// ------------------ Define ports for this board ----------------------
// *********************************************************************



// *********************************************************************
// ------------------------- LEDs and Buttons --------------------------
// *********************************************************************



// *********************************************************************
// ---------------- Network ENC624J600 WiFi I/O pins -------------------
// *********************************************************************



// *********************************************************************
// ---------------- Network MRF24WB0M WiFi I/O pins --------------------
// *********************************************************************



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
// ----------- SPI External Mememory Bus Defines for SBC66ECL -----------
// *********************************************************************



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

#endif
