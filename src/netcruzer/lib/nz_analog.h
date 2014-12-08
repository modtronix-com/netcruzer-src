/**
 * @brief           Internal ADC functions
 * @file            nz_analog.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_analog_desc Description
 *****************************************
 * Functions for using the internal ADC
 *
 * @subsection nz_analog_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // ------------ ADC Filtered Configuration (from nz_analog.h) -------------
 // *********************************************************************
 //If following line is included in projdefs.h, 3.3V supply is used for reference. Default is 2.5V external precision reference.
 #define ADC_3V3_VREF                                       //Use internal 3.3V supply as reference

 //Number of filter stages for each ADC channel. Must be 2, 4 or 8. For each ADC channel, this many
 //words are needed. Total required memory is ADC_MAX_CHAN x ADC_FILTERS words.
 #define ADC_FILTER_STAGES                      ( 4 )       //[-DEFAULT-]

 //ADC Interrupt priority, a value from 0 to 7. 0 will disable interrupt, 7 is highest priority.
 #define nzINT_PRIORITY_ADC1                    ( 4 )       //[-DEFAULT-]

 //Maximum number of possible ADC channels. Default value is taken from board include file, and reserves memory for
 //all possible ADC channels. To reduce memory used, define this value to be equal to actual number of ADC channels
 //used. For example, if only using Port 1 and 2 (old port names X1 and X2) are ADC channels, change ADC_MAX_CHAN_BRD to 2.
 #define ADC_MAX_CHAN               ADC_MAX_CHAN_BRD        //[-DEFAULT-]

 //The channel mask indicates what AN channels can be used for ADC inputs. Default value is taken from board include
 //file, and is a mask for all possible ADC channels. Don't change unless you know what you are doing!
 #define ADC_CHANNEL_MASK_LOW    ADC_CHANNEL_MASK_LOW_BRD   //[-DEFAULT-]
 #define ADC_CHANNEL_MASK_HIGH   ADC_CHANNEL_MASK_HIGH_BRD  //[-DEFAULT-]

 //To reduce size of the nz_analog module, following defines can be added to projdefs.h
 #define ADC_NO_MV_FUNCTIONS                                //No millivolt functions included
 #define ADC_NO_CONVERT_FUNCTIONS                           //No convert functions included
 #define ADC_NO_CHAN_FUNCTIONS                              //Don't support any channel functions - use indexed functions
 
 @endcode
 **********************************************************************
 * @section nz_analog_lic Software License Agreement
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
#ifndef NZ_ANALOG_H
#define NZ_ANALOG_H

/**
 * Convert given millivolt value to 10-bit ADC value
 */
#define CONVERT_MV_TO_10BIT_ADC_VALUE(mv) ((WORD) ( (((float)mv)/((float)ADC_REF_MV)) * 1023.0) )

//Number of filter stages for each ADC channel. Must be 2, 4 or 8. For each ADC channel, this many
//words are needed. Total required memory is ADC_MAX_CHAN x ADC_FILTERS words.
#ifndef ADC_FILTER_STAGES
#define    ADC_FILTER_STAGES  4
#endif

//ADC Interrupt priority, a value from 0 to 7. 0 will disable interrupt, 7 is highest priority.
#ifndef nzINT_PRIORITY_ADC1
#define    nzINT_PRIORITY_ADC1 4
#endif

#ifndef ADC_MAX_CHAN
#define ADC_MAX_CHAN ADC_MAX_CHAN_BRD
#endif

//The channel mask indicates what AN channels can be used for ADC inputs
#ifndef ADC_CHANNEL_MASK_LOW
#define ADC_CHANNEL_MASK_LOW   ADC_CHANNEL_MASK_LOW_BRD
#endif

//The channel mask indicates what AN channels can be used for ADC inputs
#ifndef ADC_CHANNEL_MASK_HIGH
#define ADC_CHANNEL_MASK_HIGH  ADC_CHANNEL_MASK_HIGH_BRD
#endif


/**
 * Structure containing information on ADC channels
 */
typedef struct __attribute__((aligned(2), packed)) 
{
    BYTE        count;                      //Number of ADC channels
    BYTE        currentFilter;              //Value from 0 to (ADC_FILTER_STAGES - 1)

    union {
        struct
        {
            unsigned int bDataValid : 1;   //If set, getVarCmd() returned all requested data
            unsigned int bFill : 15;       //Fill to ensure flags take up a 16-bit WORD
        } flags;
        WORD flagsVal;
    };

    //Bits indicating enabled ADC channels. Each bit represents an ADC channel, ex: bit 0=AN0, 1=AN1, 15=AN15....
    //If ADC_CHANNEL_MASK_HIGH == 0, upper word is not used!
    // - Is initialized with ADC_CHANNEL_MASK_LOW, giving all possible ADC channels
    // - Then, in adcOpen() is masked with used channels. After adcOpen() contains only opened channels.
    DWORD_VAL    channels;

    //WORD        adcValues[ADC_MAX_CHAN];      //Filtered ADC values

    //Array of ADC filters. For each channel, ADC_FILTER_STAGES WORDs are reserved. Only used channels require
    //reserved adcFilter positions
    WORD        adcFilter[ADC_FILTER_STAGES][ADC_MAX_CHAN];

    WORD        conversionCount;                //Free running counter, get's incremented with each ISR
} ADC_INFO;


//This is used by others
#ifndef THIS_IS_NZ_ANALOG_C
extern ADC_INFO adcInfo;
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
 * The following example will enable ADC for port 1, 4 and 30 (old port names X1, X4 and Y5) ports:
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
 * @param sampleTime Value from 0 to 31. Gives sample time = given value x TAD. Don't use 0!
 */
void adcOpenExtra(DWORD adcChannels, BYTE conversionClock, BYTE sampleTime);


/**
 * Configures ADC module, and enables it for given ADC channels. By default, uses the external 2.5V precision
 * reference, meaning all inputs must be within 0 to 2.5V range. To use internal 3.3V as reference, add following
 * define to projdefs.h:
 * @code
 * #define ADC_3V3_VREF
 * @endcode
 * After calling this function, ADC conversions will be done in the background, via the ADC interrupt. To get ADC
 * values, use one of the adcGetXxx() functions.
 *
 * @param adcChannels Indicates what ADC channels are active, and included in auto scanning. Must be
 *      ORed ADC_OPEN_XX defines.<br>
 *      For example: ADC_OPEN_A1 | ADC_OPEN_A4 | ADC_OPEN_A11
 */
#define adcOpen(adcChannels) adcOpenExtra(adcChannels, 64, 31)



/**
 * This will start auto sampling and converting of configured ADC channels.
 *
 * Precondition: adcOpen() must have already been called.
 */
#define adcStart() {_AD1IF = 0; _ASAM = 1;}


/**
 * Stop current ADC conversion. All partially converted data is lost. To resume automatic
 * sampling, call adcStart()
 */
void adcStop(void);


/**
 * Gets filtered 10-bit channel value of given index. The given index must be a value from 0-n, representing
 * the index of the requested ADC channel. Each enabled channel is assigned an index from 0 to n.
 * For example, if 3 channels are enabled (ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4), index must be a value
 * from 0 to 2, representing ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4.
 *
 * This function is <b>MUCH FASTER than the matching adcReadChan() function!</b> The adcReadChan() function must
 * first calculate the index for the given channel.
 *
 * @param index Index of desired ADC channel, is a value from 0-n, representing the index of the requested
 *      ADC channel. DO NOT use ADC_CH_XX constant!
 *
 * @return Returns the given channel's 10-bit value. A WORD is returned with value from 0 - 1023.
 */
WORD adcReadChanForIndex(BYTE index);


/**
 * Gets filtered 8-bit channel value of given index. The given index must be a value from 0-n, representing
 * the index of the requested ADC channel. Each enabled channel is assigned an index from 0 to n.
 * For example, if 3 channels are enabled (ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4), index must be a value
 * from 0 to 2, representing ADC_CH_A1, ADC_CH_A3 and ADC_CH_A4.
 *
 * This function is <b>MUCH FASTER than the matching adcReadChan8Bit() function!</b> The adcGet8BitChan() function must
 * first calculate the index for the given channel.
 *
 * @param index Index of desired ADC channel, is a value from 0-n, representing the index of the requested
 *      ADC channel. DO NOT use ADC_CH_XX constant!
 *
 * @return Returns the given channel's 8-bit value. A WORD is returned with value from 0 - 1023.
 */
#define adcReadChan8BitForIndex(index) ((BYTE)(adcReadChanForIndex(index)>>2))


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
WORD adcReadChan(BYTE ch);


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
#define adcReadChan8Bit(ch) ((BYTE)(adcReadChan(ch)>>2))


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
WORD adcReadChanMvForIndex(BYTE ch);


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
WORD adcReadChanMv(BYTE ch);


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
WORD adcConvertChan(BYTE ch, BYTE times);


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
WORD adcConvertChanForIndex(BYTE index, BYTE times);


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
WORD adcConvertChanMv(BYTE ch, BYTE times);


/**
 * !!!!! IMPORTANT !!!!!
 * NOT IMPLEMENTED YET! Use adcConvertChanMv() function
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
WORD adcConvertChanMvForIndex(BYTE index, BYTE times);


/**
 * Enable ADC interrupts
 */
#define adcIntEnable()  (_AD1IE = 1)


/**
 * Disable ADC interrupts
 */
#define adcIntDisable()  (_AD1IE = 0)

/**
 * Get ADC conversion counter. Is incremented with each ISR
 */
#define adcGetConversionCount() (adcInfo.conversionCount)

/**
 * Get number of filter stages for ADC. If a ADC input changes, we have to wait for ISR to trigger this many times (adcGetConversionCount())
 * before value returned by adcReadChan() and adcReadChanMv() are valid.
 */
#define adcGetFilterStages() (ADC_FILTER_STAGES)

#endif
