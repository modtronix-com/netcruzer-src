/**
 * @brief           General Purpose ADC (Analog to Digital Converter) function, with interrupt functionality
 * @file            nz_analog.c
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
 * 2011-06-03, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_ANALOG_C
#define USE_AND_OR

#include <string.h>

#include "HardwareProfile.h"

#include "nz_analog.h"

//Global variables
ADC_INFO    adcInfo;        // physical I2C address


/**
 * ADC ISR
 */
#if defined( __C30__ )
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt (void) {
    WORD* pSrc;
    WORD* pSrcLast;
    WORD* pDst;
    WORD mask=0x0001;

    //Increment every ISR. Seeing that in worst case ISR shouldn't be called more than once per 100us, will only overflow after 100us x 6.5 seconds
    adcInfo.conversionCount++;

    //_ASAM = 0;     //Clear "Auto Sample" bit. Must be done so we can start it again

    pDst = &adcInfo.adcFilter[adcInfo.currentFilter][0];    //Address of first destination byte
    //The line below brings error, use line below that. Should not bring error, seeing that "unsigned int" and WORD (unsigned short), are both a value from 0- 65535
    //pSrc = ((unsigned int *)(&ADC1BUF0));                 //Address of first source byte
    pSrc = ((WORD *)(&ADC1BUF0));                           //Address of first source byte
    pSrcLast = ((WORD *)pSrc) + adcInfo.count;              //Address of last source byte
#if (ADC_CHANNEL_MASK_HIGH == 0)
    while (pSrc != pSrcLast) {
        //Is channel pointed to by current pDst an active ADC channel
        // - If so, copy data from ADC1BUFX to pDst
        if ((adcInfo.channels.w[0] & mask) != 0) {
            *pDst++ = *pSrc++;
        }
        mask = mask << 1;
    }
#else
    #error "ADC ISR not implemented for CPU with more than 16 channels"
#endif

    if (++adcInfo.currentFilter == ADC_FILTER_STAGES) {
        adcInfo.currentFilter = 0;

        //All adcFilter[] positions have been filled with data.
        //Set bDataValid flag indicating ADC now contains a valid value. Functions like adcGetCh()
        //and adcGetChMV() can now be called, and a valid value will be returned.
        adcInfo.flags.bDataValid = 1;
    }

    //_ASAM = 1;    //Start next conversion sequence
    _AD1IF = 0;     //Clear interrupt flag
}
#elif defined( __PIC32MX__ )
void __attribute((interrupt(ipl2), vector(_ADC_VECTOR), nomips16)) _ADC1Interrupt(void) {

    // Get correct code for ADC!!!!!!!!!
    //IFS0CLR = _IFS0_T1IF_MASK;  //Clear interrupt flag
}
#else
#error "Compiler not defined"
#endif


/**
 * Configures ADC module. By default, uses the external 2.5V precision reference, meaning all inputs must be within
 * 0 to 2.5V range. To use internal 3.3V as reference, add following define to projdefs.h:
 * @code
 * #define ADC_3V3_VREF
 * @endcode
 * This function configures the ADC to interrupt after all ADC pins have been converted (one ADC cycle). For each ADC
 * cycle, all ADC channels are converted, and their results placed in the ADC1BUF0 to ADC1BUFF registers.
 *
 * The conversionClock parameter sets the TAD (number of Tcy) of the ADC. Converting each channel consists
 * of the sampling, followed by converting. The converting takes 12 x TAD. TAD = Tcy x conversionClock (150nS for 16MHz clock)
 * The sampleTime sets how many TAD is used for samping. Should be at least 1uS.
 * Total time per channel is:
 * = (12 x TAD) + (sampleTime x TAD)
 * = (12 x (Tcy x conversionClock))  +  (sampleTime x (Tcy x conversionClock))
 *
 * For example, for a SBC66 boards (32Mhz clock, Tcy=62.5ns) with conversionClock=2 and sampleTime=10, TAD = 150ns (2xTcy)
 * = (12 x TAD)  +  (sampleTime x TAD)  =  (12 x 150ns)  +  (10 x 150ns)
 * = 1.8us (conversion time) + 1.5us (sample time) = 3.3us per channel
 * For this example, if configured for 4 channels, interrupt will be generated each 3.3x4 = 13.2us!
 *
 * Keep in mind than an interrupt is generated each time all configured channels are converted. This could
 * be a big overhead if the conversionClock and sampleTime parameters are set too small!
 *
 * The following example will enable ADC for Port 1, 4 and 30 (old port names X1, X4 and Y5) ports:
 * adcOpen(ADC_OPEN_A1 | ADC_OPEN_A4 | ADC_OPEN_A11, 64, 31);
 * This example sets TAD to be 64 x 62.5ns = 4us. This gives a conversion time of 12 x TAD = 48us.
 * This example sets sample time to be 31 x TAD = 124us. Sample and conversion time per channel:
 * = (sample time) + (conversion time) = 172us per channel.
 * For 3 channels in this example interrupt is generated every 3 x 172 = 516us.
 *
 * @param adcChannels Indicates what ADC channels are active, and included in auto scanning. Must be
 *      ORed ADC_OPEN_XX defines. For example: ADC_OPEN_A1 | ADC_OPEN_A4 | ADC_OPEN_A11
 *
 * @param conversionClock Value from 1 to 64, that is used for TAD. Number of Tcy (Instruction Cycle Time) to use
 *        for one TAD. One TAD must be at lease 150nS long.
 *
 * @refExternal When 0, reference is internal 3.3V supply. Else, reference is precision 2.5V external reference.
 *
 * @param sampleTime Value from 0 to 31. Gives sample time = given value x TAD. Don't use 0!
 */
void adcOpenExtra(DWORD adcChannels, BYTE conversionClock, BYTE sampleTime) {
#if defined( __C30__ )
    WORD mask;
    BYTE firstAdcChan;
    BYTE i,j;
    BYTE chanCount = 0;                 //Number of ports configured as ADC channels

    _AD1IE = 0;             //Disable A/D conversion interrupt
    _ADON = 0;              //Turn ADC converter off

    adcInfo.flagsVal = 0;               //Clear all flags
    adcInfo.currentFilter = 0;
    adcInfo.conversionCount = 0;

    //Ensure only channels that are possible for this hardware are defined in adcInfo.channels. Mask out all others
    adcInfo.channels.Val = adcChannels;     //Save ADC channels
    adcInfo.channels.w[0] &= ADC_CHANNEL_MASK_LOW;
    adcInfo.channels.w[1] &= ADC_CHANNEL_MASK_HIGH;

    //Clear all ADC filter values
    for (i=0; i<ADC_MAX_CHAN; i++) {
        for (j=0; j<ADC_FILTER_STAGES; j++)
            adcInfo.adcFilter[j][i] = 0;
    }

    //Get channel of first ADC pin. Is a value from 0-15.
    mask = 0x0001;
    firstAdcChan = 0xff;
#if (ADC_CHANNEL_MASK_HIGH == 0)
    for (i=0; i<16; i++) {
        //Check if channel indicated by current mask is active
        if ( (adcInfo.channels.w[0] & mask) != 0) {
            //If we have already reached our maximum configured channels, clear all the rest
            if (chanCount == ADC_MAX_CHAN) {
                //Clear current channel. Is not used any more, only first ADC_MAX_CHAN are used!
                adcInfo.channels.w[0] &= ~mask;
            }
            //Increment channel count, and mark if first channel
            else {
                chanCount++;
                if (firstAdcChan==0xff) {
                    firstAdcChan = i;
                }
            }
        }
        mask = mask << 1;
    }
#else
    j = 0;
    for (i=0; i<32; i++) {
        //Is ADC pin indicated by mask active
        if ( (adcInfo.channels.w[j] & mask) != 0) {
            chanCount++;
            if (firstAdcChan==0xff) {
                firstAdcChan = i;
            }
        }

        //Prepare for checking second WORD (bits 17 to 31 of adcChanels)
        if (i==15) {
            mask = 0x0001;
            j = 1;      //Point to second word
        }
        else {
            mask = mask << 1;
        }
    }
#endif

    adcInfo.count = chanCount;  //Store number of channels

    //Configure ADC channels. A 0 = ADC channel, 1 = digital I/O
    #if defined(__PIC24FJ128GB106__) || defined(__PIC24FJ256GB106__) || defined(__PIC24FJ256GB110__)
        //These PICs have a AD1PCFGL register to select what Ports are Digital and what Analog
        AD1PCFGL = ~adcInfo.channels.w[0];    //0=Analog mode, 1=Digital mode
    #elif defined(__PIC24FJ256GB206__)
        //These PICs have ANSx registers to select what Ports are Digital and what Analog
        //1=Analog mode, 0=Digital mode
        ANSB = adcInfo.channels.w[0];  //B0 - B15 = AN0 - AN15
    #elif defined(__PIC24FJ256GB210__)
        // TODO add support for channels 16 to 23!!!!!!!!!!!!!!
        //These PICs have ANSx registers to select what Ports are Digital and what Analog
        // TODO Implement AN16 to AN23 configuration
        //1=Analog mode, 0=Digital mode
        ANSA = 0x0000;                  //A6, A7 = AN22, AN23
        ANSB = adcInfo.channels.w[0];   //B0 - B15 = AN0 - AN15
        ANSC = 0x0000;                  //C4 = AN16
        ANSE = 0x0000;                  //E9 = AN21
        ANSG = 0x0000;                  //G6 - G9 = AN17 - AN20
    #else
        #error "ADC channels not configured for selected CPU type!"
    #endif

    //0xxx xxxx xxxx xxxx - A/D Converter is off
    //xx0x xxxx xxxx xxxx - Continue module operation in Idle mode
    //xxxx xx00 xxxx xxxx - Data output format is Integer (0000 00dd dddd dddd)
    //xxxx xxxx 111x xxxx - Auto Conversion, Internal counter ends sampling and starts conversion
    //xxxx xxxx xxxx x0xx - ASAM = Auto Sampling = Don't set now, will start sampling as soon as set!!!
    AD1CON1 = 0x00E0;

    _AD1IF = 0;                     //Clear interrupt flag

    _AD1IP = nzINT_PRIORITY_ADC1;     // Configure A/D interrupt priority bits (AD1IP<2:0>)

    //Configure ADC Channels to be included in scan
    AD1CSSL = adcInfo.channels.w[0];
#if (ADC_CHANNEL_MASK_HIGH != 0)
    #error "ADC ISR not implemented for CPU with more than 16 channels"
    //Add support for PIC with more than 16 ADC channels
    //???? = adcInfo.channels.w[1];
#endif

    //There are no ADC ports, return
    if (adcInfo.channels.Val == 0) {
        return;
    }

    //001x xxxx xxxx xxxx - VR+ = External Vref pin, VR- is Analog ground AVdd
    //xxxx x1xx xxxx xxxx - Auto scan inputs
    //xxxx xxxx xx.. ..xx - SMPI set in next line = Sample/Convert Sequences Per Interrupt Selection bits
    //xxxx xxxx xxxx xx0x - Buffer configured as one 16-word buffer (ADC1BUFn<15:0>)
    //xxxx xxxx xxxx xxx0 - Always use MAX A for ADC inputs
    #ifdef ADC_3V3_VREF
    AD1CON2 = 0x0400;   //Internal VRef
    #else
    AD1CON2 = 0x2400;   //External VRef
    #endif

    _SMPI = chanCount-1;    //Set number of Sample/Convert Sequences before interrupt is triggered

    if (conversionClock>64) conversionClock=64;
    if (conversionClock!=0) conversionClock--;  //Convert from 1-255 to 0-254

    //xxxx xxxx xxnn nnnn - A/D Conversion Clock (75nS min) = TAD = conversionClock x Tcy
    //xxxn nnnn xxxx xxxx - Auto-Sample Time = sampleTime parameter = sampleTime x TAD
    //0xxx xxxx xxxx xxxx - ADC clock derived from system clock
    AD1CON3 = ((WORD)conversionClock) | (( ((WORD)sampleTime)&0x001f ) << 8);

    //xxxx xxxx 0xxx xxxx - Channel 0 negative input is VR- = AVss
    AD1CHS0 = 0x0000;

    //Set selected ADC channel, these bits are IGNORED when scanning is enabled!
    //_CH0SA = firstAdcChan;

    _AD1IE = 1;     // Enable A/D conversion interrupt

    _ADON = 1;      //Turn ADC converter on

    adcStart();     //This will start auto sampling and converting of configured ADC channels. Is done in background interrupt.

#elif defined( __PIC32MX__ )
    
#else
#error "Compiler not defined"
#endif
}


#ifndef ADC_NO_CHAN_FUNCTIONS   //Don't support any channel functions, only ones to use ADC channel's index
/**
 * Convert the given channel to an index pointing to the ADC buffer.
 *
 * @param chan A channel value from 0-15 (can be more for 80 and 100 pin PIC chips)
 *
 * @return Returns index into ADC buffer for this channel. If 0xff, given channel is not enabled!
 */
BYTE adcChanToIndex(BYTE chan) {
    WORD mask;
    BYTE i = 0;
    BYTE index = 0;
    BOOL chanIsEnabled;

    mask = 0x0001;

#if (ADC_CHANNEL_MASK_HIGH == 0)
    while(1) {
        chanIsEnabled = FALSE;
        //Check if channel indicated by current mask is open
        if ( (adcInfo.channels.w[0] & mask) != 0) {
            chanIsEnabled = TRUE;
            index++;
        }
        mask = mask << 1;

        //i currently points to given channel. If it is not enabled, return 0xff
        if (i++ == chan) {
            //If given channel is not enabled, return 0xff
            if (chanIsEnabled==FALSE) {
                return 0xff;
            }
            break;
        }
    }
#else
    #error "adcChanToIndex() not implemented for CPU with more than 16 channels yet!"
#endif

    //At this stage, index should be 1 greater than index into buffer. If 0, given channel
    //was not found - return 0xff
    if (index == 0) {
        return 0xff;
    }

    return index-1;
}
#endif


#ifndef ADC_NO_MV_FUNCTIONS     //No millivolt functions
/**
 * Gets filtered value of given channel in mV. The given index must be a value from 0-n, representing
 * the index of the requested ADC channel. Each enabled channel is assigned an index from 0 to n.
 * For example, if 3 channels are enabled (ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4), index must be a value
 * from 0 to 2, representing ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4.
 *
 * This function is <b>MUCH FASTER than the matching adcReadChanMv() function!</b> The adcReadChanMv() function must
 * first calculate the index for the given channel.
 *
 * @param index Index of desired ADC channel, is a value from 0-n, representing the index of the requested
 *      ADC channel.
 *
 * @return Requested channel value in mV.
 */
WORD adcReadChanMvForIndex(BYTE index) {
    #ifdef ADC_3V3_VREF
    return ((WORD) ((3300.0/1023.0) * ((float)adcReadChan(index))) );
    #else
    return ((WORD) ((((float)ADC_REF_MV)/1023.0) * ((float)adcReadChanForIndex(index))) );
    #endif
}
#endif

#ifndef ADC_NO_MV_FUNCTIONS     //No millivolt functions
#ifndef ADC_NO_CHAN_FUNCTIONS
/**
 * Gets filtered value of given channel in mV. The given channel (ch parameter) must
 * be a ADC_CH_xx constant! These constants are defined as values from 0-15, representing AN0 to AN15
 * of the Microcontroller.
 *
 * @param ch The ADC channel. For example, for a CPU with 16 ADC channels(AN0 to AN15), will be a value
 *           from 0-15. Can use ADC_CH_XX constants.
 *
 * @return Requested channel value in mV.
 */
WORD adcReadChanMv(BYTE ch) {
    #ifdef ADC_3V3_VREF
    return ((WORD) ((3300.0/1023.0) * ((float)adcReadChan(ch))) );
    #else
    return ((WORD) ((((float)ADC_REF_MV)/1023.0) * ((float)adcReadChan(ch))) );
    #endif
}
#endif
#endif


/**
 * Gets filtered 10-bit value of given index. The given index must be a value from 0-n, representing
 * the index of the requested ADC channel. Each enabled channel is assigned an index from 0 to n.
 * For example, if 3 channels are enabled (ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4), index must be a value
 * from 0 to 2, representing ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4.
 *
 * This function is <b>MUCH FASTER than the matching adcReadChan() function!</b> The adcReadChan() function must
 * first calculate the index for the given channel.
 *
 * @param index Index of desired ADC channel, is a value from 0-n, representing the index of the requested
 *      ADC channel.
 *
 * @return Returns the given channel's 10-bit value. A WORD is returned with value from 0 - 1023.
 */
WORD adcReadChanForIndex(BYTE index) {
#if (ADC_FILTER_STAGES == 2)
  return ( (adcInfo.adcFilter[0][index] + adcInfo.adcFilter[1][index]) / 2);
#elif (ADC_FILTER_STAGES == 4)
  return ( (adcInfo.adcFilter[0][index]
          + adcInfo.adcFilter[1][index]
          + adcInfo.adcFilter[2][index]
          + adcInfo.adcFilter[3][index]
          + 2 /* Ensure rounding above .5 to next integer */) / 4);
#elif (ADC_FILTER_STAGES == 8)
  return ( (adcInfo.adcFilter[0][index]
          + adcInfo.adcFilter[1][index]
          + adcInfo.adcFilter[2][index]
          + adcInfo.adcFilter[3][index]
          + adcInfo.adcFilter[4][index]
          + adcInfo.adcFilter[5][index]
          + adcInfo.adcFilter[6][index]
          + adcInfo.adcFilter[7][index] + 4/* Ensure rounding above .5 to next integer */) / 8);
#else
  #error "ADC_FILTER_STAGES must be 2, 4 or 8"
#endif
}


#ifndef ADC_NO_CHAN_FUNCTIONS   //Don't support any channel functions, only ones to use ADC channel's index
/**
 * Gets filtered 10-bit value of given channel. The given channel (ch parameter) must
 * be a ADC_CH_xx constant! These constants are defined as values from 0-15, representing AN0 to AN15
 * of the Microcontroller.
 *
 * @param ch The ADC channel. For example, for a CPU with 16 ADC channels(AN0 to AN15), will be a value
 *           from 0-15. Can use ADC_CH_XX constants.
 *
 * @return Returns the given channel's 10-bit value. A WORD is returned with value from 0 - 1023.
 */
WORD adcReadChan(BYTE ch) {
    BYTE index;

    //Convert given channel to index in ADC channel array
    if ((index = adcChanToIndex(ch)) == 0xff) {
        return 0;   //Given channel not enabled!
    }

    return adcReadChanForIndex(index);
}
#endif


#ifndef ADC_NO_CHAN_FUNCTIONS       //Don't support any channel functions, only ones to use ADC channel's index
#ifndef ADC_NO_GET_FIL_CHAN_8BIT
/**
 * Gets filtered 8-bit value of given channel. The given channel (ch parameter) must
 * be a ADC_CH_xx constant! These constants are defined as values from 0-15, representing AN0 to AN15
 * of the Microcontroller.
 *
 * @param ch The ADC channel. For example, for a CPU with 16 ADC channels(AN0 to AN15), will be a value
 *           from 0-15. Can use ADC_CH_XX constants.
 *
 * @return Returns the given channel's 8-bit value. A BYTE is returned with value from 0 - 255.
 */
//BYTE adcReadChan8Bit(BYTE ch) {
//    BYTE index;
//
//    //Convert given channel to index in ADC channel array
//    if ((index = adcChanToIndex(ch)) == 0xff) {
//        return 0;   //Given channel not enabled!
//    }
//
//    return ((BYTE)(adcReadChanForIndex(index)>>2));
//}
#endif
#endif


#ifndef ADC_NO_MV_FUNCTIONS     //No millivolt functions
#ifndef ADC_NO_CHAN_FUNCTIONS
/**
 * Convert given channel given number of times, and return average. The ADC IRS is teporarity stopped.
 * Convert given channel given number of times, and return average. The ADC IRS is teporarity stopped.
 * The given channel (ch parameter) must be a ADC_CH_xx constant! These constants are defined as values
 * from 0-15, representing AN0 to AN15 of the Microcontroller.
 *
 * Precondition: The given channel must have been configured as an ADC channel via adcOpen() function.

 * @param ch The ADC channel. For example, for a CPU with 16 ADC channels(AN0 to AN15), will be a value
 *           from 0-15. Can use ADC_CH_XX constants.
 *
 * @param times How many times to convert given channel. Must be 2, 4 or 8.
 *
 * @return Returns the average value of conversions. Result is a 10 bit value.
 */
WORD adcConvertChan(BYTE ch, BYTE times) {
    // TODO
    // Check if AD1CSSL has to be changed???? It might be ignored when CSCNA = 0

    //In this function, we will disable interrupts and scanning of given channels. We will select a single
    //channel (CH0SA bits select CH) to convert multiple times.

    BYTE i;
    WORD* ADC16Ptr;
    WORD config2, config3;
    WORD ADCValue = 0;

#if defined( __C30__ )
    _AD1IE = 0;     // Disable A/D conversion interrupt
    _ADON = 0;      //Turn ADC converter off
#elif defined( __PIC32MX__ )

#else
#error "Compiler not defined"
#endif

    if ((times!=2) && (times!=4) && (times!=8))
        times = 4;

    if (ch > 15)
        ch = 0;

    //Save current values
    config2 = AD1CON2;
    config3 = AD1CON3;

    //===== AD1CON1 =====
    //AD1CON1 should already have this value, NO NEED TO SET IT AGAIN!
    //AD1CON1 = 0x00E0; // SSRC bit = 111 implies internal counter ends sampling and starts converting.

    //===== AD1CON2 =====
    _CSCNA = 0;             //Disable Auto scan
    //Set number of Sample/Convert Sequences before interrupt is triggered. Get it to do one more
    //than requested. This is done seeing that first sample seems to often be not accurate. Below we
    //don't use ADC1BUF0, start from ADC1BUF1;
    _SMPI = times+1;

    //===== AD1CON3 =====
    //Ensure TAD is at least 75ns (minimum in datasheet), and sample time more than 1us
#if (GetSystemClock() == 32000000ul)  //Tcy = 62.5ns
    //xxxx xxxx xx00 0001 - A/D Conversion Clock (TAD) = 2 x Tcy = 125ns
    //xxx0 1010 xxxx xxxx - Auto-Sample Time = 10 x TAD = 1.25us
    //0xxx xxxx xxxx xxxx - ADC clock derived from system clock
    AD1CON3 = 0x0A01;
#else
    // TODO for PIC32MX!!!!!!!!
    //#error "No code for current clock"
#endif

    //===== AD1CHS =====
    //_CH0NA = 0;       //Select VR- for S/H- Input - SHOULD ALREADY BE IN THIS STATE
    _CH0SA = ch;     //Set selected ADC channel, these bits are IGNORED when scanning is enabled again at end of this function!

    //===== AD1CSSL =====
    // TODO add support for channels 16 to 23!!!!!!!!!!!!!!
    AD1CSSL = 0;    //Do not scan any channels.

    //The line below brings error, use line below that. Should not bring error, seeing that "unsigned int" and WORD (unsigned short), are both a value from 0- 65535
    //ADC16Ptr = (WORD*)&ADC1BUF1;  //initialize ADC1BUF pointer. Don't use ADC1BUF0, start at ADC1BUF1. ADC1BUF0 seem to often not be accurate
    ADC16Ptr = (WORD*)((unsigned int *)(&ADC1BUF1));

#if defined( __C30__ )
    _ADON = 1;              //Turn ADC converter on
    _AD1IF = 0;             //Clear interrupt flag
    _ASAM = 1;              //Kick start
    while (!_AD1IF){};      // conversion done?
    _ASAM = 0;
#elif defined( __PIC32MX__ )
#else
#error "Compiler not defined"
#endif

    for (i = 0; i < times; i++) // average the ADC value
    {
        ADCValue = ADCValue + *ADC16Ptr++;
    }
    //Add times/2 to final value. This ensures any value more than .5 gets rounded to next value below.
    if (times > 2)
        ADCValue += (times>>1);

    ADCValue = ADCValue / times;

    //Restore AD1CON2 and AD1CON3
    AD1CON2 = config2;
    AD1CON3 = config3;

    //Restore ADC Channels to be included in scan
    // TODO add support for channels 16 to 23!!!!!!!!!!!!!!
    AD1CSSL = adcInfo.channels.w[0];


#if defined( __C30__ )
    _AD1IF = 0;     //Clear interrupt flag
    _AD1IE = 1;     // Enable A/D conversion interrupt
    _ADON = 1;      //Turn ADC converter on
    _ASAM = 1;      //Kick start
#elif defined( __PIC32MX__ )
#else
#error "Compiler not defined"
#endif

    return ADCValue;
}
#endif
#endif


#ifndef ADC_NO_MV_FUNCTIONS     //No millivolt functions
#ifndef ADC_NO_CONVERT_FUNCTIONS
/**
 * !!!!! IMPORTANT !!!!!
 * NOT IMPLEMENTED YET! Use adcConvertChanMv() functions
 *
 * Convert given channel given number of times, and return average. The ADC IRS is teporarity stopped.
 * The given index must be a value from 0-n, representing the index of the requested ADC channel.
 * Each enabled channel is assigned an index from 0 to n.
 * For example, if 3 channels are enabled (ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4), index must be a value
 * from 0 to 2, representing ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4.
 *
 * Precondition: The given channel must have been configured as an ADC channel via adcOpen() function.
 *
 * @param index Index of desired ADC channel, is a value from 0-n, representing the index of the requested
 *      ADC channel. DO NOT use ADC_CH_XX constant!
 *
 * @param times How many times to convert given channel. Must be 2, 4 or 8.
 *
 * @return Returns the average value of conversions. Result is a 10 bit value.
 */
WORD adcConvertChanForIndex(BYTE index, BYTE times) {
    // TOTO - create adcIndexToChan() function
    //return adcConvertChan(adcIndexToChan(index), times);
    return 0;   //Will remove this line once implemented
}
#endif
#endif


#ifndef ADC_NO_MV_FUNCTIONS     //No millivolt functions
#ifndef ADC_NO_CONVERT_FUNCTIONS
/**
 * Convert given channel given number of times, and return average. The ADC IRS is teporarity stopped.
 * The given channel (ch parameter) must be a ADC_CH_xx constant! These constants are defined as values
 * from 0-15, representing AN0 to AN15 of the Microcontroller.
 *
 * Precondition: The given channel must have been configured as an ADC channel via adcOpen() function.

 * @param ch The ADC channel. For example, for a CPU with 16 ADC channels(AN0 to AN15), will be a value
 *           from 0-15. Can use ADC_CH_XX constants.
 *
 * @param times How many times to convert given channel. Must be 2, 4 or 8.
 *
 * @return Returns the average value of conversions. Result is a 10 bit value.
 */
WORD adcConvertChanMv(BYTE ch, BYTE times) {
    #ifdef ADC_3V3_VREF
    return ((WORD) ((3300.0/1023.0) * ((float)adcConvertChan(ch, times))) );
    #else
    return ((WORD) ((((float)ADC_REF_MV)/1023.0) * ((float)adcConvertChan(ch, times))) );
    #endif
}
#endif
#endif


#ifndef ADC_NO_MV_FUNCTIONS         //No millivolt functions
#ifndef ADC_NO_CONVERT_FUNCTIONS
/**
 * !!!!! IMPORTANT !!!!!
 * NOT IMPLEMENTED YET! Use adcConvertChanMv() functions
 *
 * Convert given channel given number of times, and return average. The ADC IRS is teporarity stopped.
 * The given index must be a value from 0-n, representing the index of the requested ADC channel.
 * Each enabled channel is assigned an index from 0 to n.
 * For example, if 3 channels are enabled (ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4), index must be a value
 * from 0 to 2, representing ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4.
 *
 * Precondition: The given channel must have been configured as an ADC channel via adcOpen() function.
 *
 * @param index Index of desired ADC channel, is a value from 0-n, representing the index of the requested
 *      ADC channel. DO NOT use ADC_CH_XX constant!
 *
 * @param times How many times to convert given channel. Must be 2, 4 or 8.
 *
 * @return Returns the average value of conversions. Result is a 10 bit value.
 */
WORD adcConvertChanMvForIndex(BYTE index, BYTE times) {
    // TOTO - create adcIndexToChan() function
    //return adcConvertChanMv(adcIndexToChan(index), times);
    return 0;   //Will remove this line once implemented
}
#endif
#endif


/**
 * Stop current ADC conversion. All partially converted data is lost.
 */
void adcStop(void) {


}
