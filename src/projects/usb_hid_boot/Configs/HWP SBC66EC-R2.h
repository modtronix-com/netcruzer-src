/********************************************************************
 FileName:      HWP - SBC66EC.h
 Dependencies:  See INCLUDES section
 Processor:     PIC24FJ256GB210
 Hardware:      PIC24FJ256GB210 PIM
 Compiler:      Microchip C30
 Company:       Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company”) for its PIC® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
 20??-??-??
	- 2.6a  Initial revision
 2014-07-24, David H. (DH):
    - Changed WDT from 32 to 8 seconds
********************************************************************/

#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

//#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "nz_genericTypeDefs.h"


// Set configuration fuses (but only once)
#if defined(THIS_IS_BOOTLOADER_APPLICATION)
	#if defined(__PIC24FJ256GB206__)

    //Are not defined in p24FJ256GB206
    #define DEBUG_ON              0x77FF
    #define DEBUG_OFF             0x7FFF

        // - External Clock Mode (SCLKI), use RC13 as digital IO (SOSCSEL_EC)
        // - Segment Write Protection Disable (WPDIS_WPDIS)
        // - Write Protect Configuration Page Select Disabled (WPCFG_WPCFGDIS)
        // - Segment Write Protection End Page Select = Write Protect from page 0 to WPFP (WPEND_WPSTARTMEM)
        _CONFIG3( SOSCSEL_EC & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPSTARTMEM)

        // - Two Speed Start-up = off (IESO_OFF)
        // - 96MHz PLL Prescaler = No Divide, for 4MHz input (PLLDIV_NODIV)
        // - Disable 96MHz PLL = not disabled, don't add anthing to _CONFIG2
        // - Oscillator Selection = Primary oscillator (XT, HS, EC) w/ PLL (FNOSC_PRIPLL)
        // - Clock switching and clock monitor = Both disabled (FCKSM_CSDCMD)
        // - OSCO/RC15 function = RC15 (OSCIOFNC_ON)
        // - RP Register Protection = Unlimited Writes To RP Registers (IOL1WAY_OFF)
        // - Oscillator Selection = External Clock (POSCMOD_EC)
        _CONFIG2( IESO_OFF & PLLDIV_NODIV & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_ON & IOL1WAY_OFF & POSCMOD_EC)

        // - JTAG Port Disabled (JTAGEN_OFF)
        // - General Segment Code Protect = Code protection is enabled (GCP_OFF)
        // - General Segment Write Protect Disabled (GWRP_OFF)
        // - Emulator functions are shared with PGEC2/PGED2 (ICS_PGx2)
        //Watchdog timer resets every 8 seconds
        // - Watchdog Timer Disabled (FWDTEN_OFF)
        // - WDT Prescaler = 32 (FWPSA_PR32) = 1ms
        // - Watchdog Timer Postscaler = 1:4,096 (WDTPS_PS4096)
        _CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx2 & FWDTEN_OFF & FWPSA_PR32 & WDTPS_PS8192)
        //_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx2 & FWDTEN_OFF & FWPSA_PR32 & WDTPS_PS4096)
	#else
		#error "Not configured for valid CPU"
	#endif
#endif // Prevent more than one set of config fuse definitions

    // *********************************************************************
    // ----------- SPI External Mememory Bus Defines for SBC66EC -----------
    // *********************************************************************
    // These defines are for the SPI bus used for the external
    // memory chips like FLASH, EEPROM and SRAM. Use SPI2
    #define PPS_OUT_SPIMEM_SDO      OUT_PIN_PPS_RP2
    #define PPS_OUT_SPIMEM_SCK      OUT_PIN_PPS_RP12
    #define PPS_IN_SPIMEM_SDI       IN_PIN_PPS_RPI37


    // *********************************************************************
    // ----------------- External EEPROM Configuration ---------------------
    // *********************************************************************
    #define EEPROM_CS_TRIS	(TRISCbits.TRISC15)
    #define EEPROM_CS_IO	(LATCbits.LATC15)
    #define EEPROM_SCK_TRIS	(_TRISD11)
    #define EEPROM_SDI_TRIS	(_TRISC14)
    #define EEPROM_SDO_TRIS	(_TRISD8)
    #define EEPROM_SDI_IO	(PORTCbits.RC14)
    #define EEPROM_SPI_IF	(IFS2bits.SPI2IF)
    #define EEPROM_SSPBUF	(SPI2BUF)
    #define EEPROM_SPICON1	(SPI2CON1)
    #define EEPROM_SPICON1bits	(SPI2CON1bits)
    #define EEPROM_SPICON2	(SPI2CON2)
    #define EEPROM_SPISTAT	(SPI2STAT)
    #define EEPROM_SPISTATbits	(SPI2STATbits)



    // *********************************************************************
    // ----------------- External FLASH Configuration ----------------------
    // *********************************************************************
    #define SPIFLASH_CS_TRIS        (TRISCbits.TRISC13)
    #define SPIFLASH_CS_IO          (LATCbits.LATC13)
    #define SPIFLASH_SCK_TRIS       (TRISDbits.TRISD11)
    #define SPIFLASH_SDI_TRIS       (TRISCbits.TRISC14)
    #define SPIFLASH_SDO_TRIS       (TRISDbits.TRISD8)
    #define SPIFLASH_SDI_IO         (PORTCbits.RC14)
    #define SPIFLASH_SPI_IF         (IFS2bits.SPI2IF)
    #define SPIFLASH_SSPBUF         (SPI2BUF)
    #define SPIFLASH_SPICON1        (SPI2CON1)
    #define SPIFLASH_SPICON1bits    (SPI2CON1bits)
    #define SPIFLASH_SPICON2        (SPI2CON2)
    #define SPIFLASH_SPISTAT        (SPI2STAT)
    #define SPIFLASH_SPISTATbits    (SPI2STATbits)



    // *******************************************************************
    // --------------- USB stack hardware selection options --------------
    // *******************************************************************
    //This section is the set of definitions required by the MCHPFSUSB
    //  framework.  These definitions tell the firmware what mode it is
    //  running in, and where it can find the results to some information
    //  that the stack needs.
    //These definitions are required by every application developed with
    //  this revision of the MCHPFSUSB framework.  Please review each
    //  option carefully and determine which options are desired/required
    //  for your application.

    //#define USE_SELF_POWER_SENSE_IO
    //#define tris_self_power     TRISAbits.TRISA2    // Input
    #define self_power          1

    //#define USE_USB_BUS_SENSE_IO
    //#define tris_usb_bus_sense  TRISBbits.TRISB5    // Input
    #define USB_BUS_SENSE       U1OTGSTATbits.SESVD 
   
    //Uncomment this to make the output HEX of this project 
    //   to be able to be bootloaded using the HID bootloader
    //#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER	

    //If the application is going to be used with the HID bootloader
    //  then this will provide a function for the application to 
    //  enter the bootloader from the application (optional)
    #if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
        #define EnterBootloader() __asm__("goto 0x400")
    #endif   



    // *********************************************************************
    // -------------- Application specific definitions --------------------
    // *********************************************************************

    // Board definition
    #define CLOCK_FREQ 32000000
    
    // LED
    #define mInitAllLEDs()      TRISBbits.TRISB6 = 0
    
    #define mLED_1              LATBbits.LATB6
    #define mLED_2              
    #define mLED_3              
    #define mLED_4              

    #define mGetLED_1()         mLED_1
    #define mGetLED_2()         1
    #define mGetLED_3()         1
    #define mGetLED_4()         1
    
    #define mLED_1_On()         mLED_1 = 1;
    #define mLED_2_On()
    #define mLED_3_On()
    #define mLED_4_On()
    
    #define mLED_1_Off()        mLED_1 = 0;
    #define mLED_2_Off()
    #define mLED_3_Off()
    #define mLED_4_Off()
    
    #define mLED_1_Toggle()     mLED_1 = !mLED_1;
    #define mLED_2_Toggle()
    #define mLED_3_Toggle()
    #define mLED_4_Toggle()

    #define mSetLED_1(in)       mLED_1 = in 
    #define mSetLED_2(in)
    #define mSetLED_3(in)
    #define mSetLED_4(in)
    
    // I/O pin definitions
    #define INPUT_PIN 1
    #define OUTPUT_PIN 0

#endif
