 /**
 * @brief           Simple and efficient functions for PWM configuration and usage.
 * @file            nz_pwm.c
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
 * 2010-06-06, David Hosken (DH):
 *    - Initial version for 24FJ256GB110 at 32MHz
 *********************************************************************/
#define THIS_IS_NZ_PWM_C

#define INCLUDE_NETCRUZER_HEADERS
#include "HardwareProfile.h"

#include "nz_pwm.h"


#if defined(__PIC24FJ128GB106__) || defined(__PIC24FJ256GB106__) || defined(__PIC24FJ256GB206__)
#elif defined(__PIC32MX__)                      // Microchip C32 compiler
    // Dummy!!! Remove this! In nz_pwm.h regs_oc_ must be defined to use CPU sfr!
    volatile REGS_OC_ regs_oc_[9];
#else
    #error "nz_pwm no processor defined!"
#endif

/**
 * This function should generally not be used, and is used by the pwmxOpenDefault() macros. The
 * pwmxOpenDefault() macros will also configure PPS, this function does not(see below for details)!
 *
 * The resulting PWM Period is calculated: <br>
 * PWM Period = ((period+1) * PWM_CLOCK_PERIOD <br>
 *
 * Where PWM_CLOCK_PERIOD is 62.5ns for all current boards
 *
 * In addition to calling this function, the relocatable output port (PPS) must be configured as a
 * PWM output. Any relocatable output port can be used. For the location of the Relocatable Ports,
 * see http://netcruzer.com/nzdoc-sbc66-ports/
 * For Netcruzer projects, all relocatable ports have the format PPS_OUT_XX, where XX is the port name.
 * For example, to configure port 38 (old port name Y8) as the PWM 1 output (OC1 - Output Compare 1), use following code:<br>
 * iPPSOutput(PPS_OUT_38,OUT_FN_PPS_OC1);    //Assign PWM 1 to (relocatable) port 38 (old port name Y8)
 *
 * @param chan PWM Channel to open, must be a value from 1 to 9
 *
 * @param period Constant used for setting the PWM Period. Can use the PWMx_FREQ defines for pre-calculated
 *      values, or the PWM_CALCULATE_PERIOD_FOR_FREQUENCY macro to get a value for a specific frequency.
 *      The resulting PWM Period is calculated (PWM_CLOCK_PERIOD is 62.5ns for all current boards):<br>
 *      PWM Period = ((period+1) * PWM_CLOCK_PERIOD
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
void pwmOpenDefault(BYTE chan, WORD period, BYTE inverted) {
#if defined(__PIC24FJ128GB106__) || defined(__PIC24FJ256GB106__) || defined(__PIC24FJ256GB206__)
    //Output Compare registers for PIC24FJ128GB106 and PIC24FJ256GB206 families
    typedef struct {
        WORD OCxCON1;
        WORD OCxCON2;
        WORD OCxRS;
        WORD OCxR;
        WORD OCxTMR;
    } REGS_OC;

    REGS_OC* regsOC;

    chan--; //Change from 1-9 to 0-8

    //Check if valid channel given, must be 0 to (PWM_COUNT - 1)
    if (chan >= PWM_COUNT) {
        return;
    }

    regsOC = ((REGS_OC*)(&OC1CON1 + (chan * 5)));

    //OCxR determines duty cycle.
    //regsOC->OCxR = 500ul;     //Duty cycle 5%
    regsOC->OCxR = 0;           //Duty cycle 0%

    //OCxRS determines period. When SYNCSEL<4:0> = 0x1F, then OCxTMR is reset when OCxRS = OCxTMR
    //For Tcy of 16Mhz, period = ((1/16,000,000) * value). For example is 16kHz for value of 1000
    //regsOC->OCxRS = 1000ul;   //OCxTMR counts from 0 to 999 = 16kHz
    regsOC->OCxRS = period;

    //xxxx xxxx xxxx x110 - Edge aligned PWM mode (Output set when OCxTMR=0, cleared when OCxTMR = OCxR )
    //xxx1 11xx xxxx xxxx - Use peripheral clock as source = Tcy = 16MHz for SBC66EC, SBC66ECL...
    regsOC->OCxCON1 = 0x1c06;

    if (inverted) {
        //xxxx xxxx xxx1 1111 - This OC Module is Trigger/Synchronization Source
        //xxx1 xxxx xxxx xxxx - PWM2 (Contrast) output inverted
        regsOC->OCxCON2 = 0x101f;
    }
    else {
        //xxxx xxxx xxx1 1111 - This OC Module is Trigger/Synchronization Source
        //xxx0 xxxx xxxx xxxx - PWM1 (Backlight) output NOT inverted
        regsOC->OCxCON2 = 0x001f;
    }
#elif defined(__PIC32MX__)                      // Microchip C32 compiler
    // TODO for PIC32MX
#else
    #error "nz_pwm no processor defined!"
#endif
}


/**
 * Sets the period of PWM channel. Can use the PWMx_FREQ defines for pre-calculated
 * values, or the PWM_CALCULATE_PERIOD_FOR_FREQUENCY macro to get a value for a
 * specific frequency.
 *
 * The resulting PWM Period is calculated: <br>
 * PWM Period = ((period+1) * PWM_CLOCK_PERIOD <br>
 *
 * Where PWM_CLOCK_PERIOD is 62.5ns for all current boards
 *
 * The resulting PWM Frequency is calculated:<br>
 * PWM Frequency = PWM_CLOCK_FREQ / (period+1)
 *
 * Where PWM_CLOCK_FREQ is 16,000,000 for all current boards.
 *
 * For example, a period of 999 gives a PWM Period of 62.5us, and a frequency of 16kHz.
 *
 * @param chan PWM Channel, must be a value from 1 to 9
 *
 * @param period Constant used for setting the PWM Period. Can use the PWMx_FREQ defines for pre-calculated
 *      values, or the PWM_CALCULATE_PERIOD_FOR_FREQUENCY macro to get a value for a specific frequency.
 *      The resulting PWM Period is calculated (PWM_CLOCK_PERIOD is 62.5ns for all current boards):<br>
 *      PWM Period = ((period+1) * PWM_CLOCK_PERIOD
 */
void pwmSetPeriod(BYTE chan, WORD period) {
    chan--; //Change from 1-9 to 0-8

    //Check valid channel given
    if(chan < PWM_COUNT) {
        regs_oc_[chan].OCxRS = period;
    }
}


/**
 * Gets the period value of this PWM. The actual period is the returned
 * value * PWM_CLOCK_PERIOD (62.5ns for all current boards).
 *
 * @param chan PWM Channel, must be a value from 1 to 9
 *
 * @return Constant used for the period value for this PWM Channel. The actual period
 *      is the returned value * PWM_CLOCK_PERIOD (62.5ns for all current boards)
 */
WORD pwmGetPeriod(BYTE chan) {
    chan--; //Change from 1-9 to 0-8

    //Check valid channel given
    if(chan < PWM_COUNT) {
        return regs_oc_[chan].OCxRS;
    }
    return 0;   //Error, chan is not in the range 0 to 10
}


/**
 * Sets the PWM frequency (in Hertz) and duty cycle (fraction). The given frequency and
 * duty cycle are both floating point values. This function <b>IS MUCH</b> slower then
 * the other pwm functions like pwmSetPeriod() and pwmSetDutyCycle(), and should only
 * be used for convenience!
 * For speed, efficiency and smaller code use pwmSetPeriod() and pwmSetDutyCycle() functions!
 *
 * The actual frequency is calculated to be as close as possible to the requested frequency,
 * and should have a value between 245 to 8,000,000 (244Hz to 8MHz). The higher the frequency,
 * the lower the PWM resolution (duty cycle steps) will be.
 *
 * The pwmGetFrequency() function can be used to get the actual frequency. The lower the requested
 * frequency, the finer the requested duty cycle can be. The amount of possible duty cycle steps
 * are calculated:  <br>
 * Duty Cycle Steps = 16MHz / requested frequency  <br>
 * For example, a requested frequency of 16kHz, will result in 1000 possible duty cycle steps.
 * Choosing a high frequency of 1MHz for example, will result in only 16!
 *
 * Possible frequency will have a period that is dividable by 62.5ns. To check if a requested
 * frequencies is possible, check if it can be obtained by the formula:
 * Frequency = 1 / ((n+1) * 62.5ns)
 *
 * @param chan PWM Channel, must be a value from 1 to 9
 *
 * @param freq Requested PWM frequency, a floating point value from 245 to 8000000.
 *
 * @param dc Requested PWM duty cycle, a fraction from 0 to 1. For
 *      example, 0.5 for 50% duty cycle.
 */
void pwmSetFreqAndDutyCycle(BYTE chan, float freq, float dc) {
    chan--; //Change from 1-9 to 0-8
    float fper;

    if (freq < 245)
        freq = 245.0;

    //Check valid channel given
    if(chan < PWM_COUNT) {
        //Set PWM period. The PWM frequency is calculated (PWM_CLOCK_FREQ / (period+1) )
        //Add 0.5 to compensate for rounding error! For example, 16000000/501 = 31.936,
        //which will be converted to 31 when cast to a WORD. We make it 31.936+0.5=32.436
        fper = (((float)PWM_CLOCK_FREQ) /freq ) + 0.5;

        regs_oc_[chan].OCxRS =  ((WORD)fper) - 1;

        regs_oc_[chan].OCxR = (WORD)(dc * ((float)regs_oc_[chan].OCxRS) );
    }
}


/**
 * Gets the frequency (in Hertz) of the given PWM channel.
 *
 * @param chan PWM Channel, must be a value from 1 to 9
 *
 * @return Returns the frequency (in Hertz) of the given PWM channel. The returned
 * frequency is a floating point value.
 */
float pwmGetFrequency(BYTE chan) {
    chan--; //Change from 1-9 to 0-8

    //Check valid channel given
    //if(chan < PWM_COUNT) {
        //Add 0.5 to compensate for rounding
        return ( (((float)PWM_CLOCK_FREQ) / ((float)(regs_oc_[chan].OCxRS) + 1.0) ) + 0.5 );
    //}
}



/**
 * Set the Duty Cycle for given PWM channel. Depending on current "pwm period" value for this PWM channel, this
 * will configure the PWM for different duty cycles. When dc = 0, duty cycle is 0%. When dc = "pwm period", duty cycle
 * will be 100%. For other values, duty cycle is a ratio.
 *
 * @param chan PWM Channel, must be a value from 1 to 9
 *
 * @param dc The duty cycle, a value from 0 to "pwm period". The pwmOpenDefault() function sets the
 *      "pwm period" to PWMx_FREQ define (default PWM_FREQ_16KHZ = 999).
 */
void pwmSetDutyCycle(BYTE chan, WORD dc) {
    chan--; //Change from 1-9 to 0-8

    //Check valid channel given
    if(chan < PWM_COUNT) {
        regs_oc_[chan].OCxR = dc;
    }
}
