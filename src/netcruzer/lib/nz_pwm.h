 /**
 * @brief           Simple and efficient functions for PWM configuration and usage.
 * @file            nz_pwm.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_pwm_desc Description
 *****************************************
 * Simple and efficient functions for PWM configuration and usage.
 * The SBC66 Netcruzer boards have 9 PWM channels. They can be assigned to any of the Relocatable Ports.
 * For the location of the Relocatable Ports, see http://netcruzer.com/nzdoc-sbc66-ports/
 * To use a PWM channel, first call pwmOpenDefault() function, and then pwmSetDutyCycle() and pwmSetPercent()
 * functions to set PWM duty cycle or percentage.
 *
 * When channel is configured for 16kHz (default), duty cycle percentages can be changed in 0.1% steps. For
 * example, pwmSetPercent(2, 1.5) will set PWM 2 to 1.5% duty cycle. In this mode, 0.1% is not possible, and
 * will result in a duty cycle of 0%. From 0.2% all values work as expected.
 *
 * When channel is configured for 160kHz, duty cycle percentages can be changed in 1% steps. For
 * example, pwmSetPercent(1, 5) will set PWM 1 to 5% duty cycle. In this mode, 1% is not possible, and
 * will result in a duty cycle of 0%. From 2% all values work as expected.
 *
 * When channel is configured for 1.6kHz, duty cycle percentages can be changed in 0.01% steps. For
 * example, pwmSetPercent(1, 1.42) will set PWM 1 to 1.42% duty cycle. In this mode, 0.01% is not possible, and
 * will result in a duty cycle of 0%. From 0.02% all values work as expected.
 *
 * <h2>Details</h2>
 * Each PWM module has it's own timer, implemented as a 16-bit timer. The clock for each timer is
 * configured by this module to be 16MHz, giving a 62.5ns period.
 * Each PWM channel has two registers, OCxRS (period) and OCxR (duty cycle). The PWM timer will count from 0 upwards, and
 * when equal to the value of OCxRS, will reset to 0 on next clock. For example, when OCxRS=999, it will count
 * from 0-999 (1000 steps). This determines the frequency(period) of the PWM channel, and is calculated (1/((OCxRS+1)*62.5ns)).
 * For example, a OCxRS value of 999 gives a frequency of 16kHz.
 *
 * The value of the OCxR register determines the duty cycle of the PWM. When the PWM timer resets to 0, the output
 * goes high (if OCxR!=0). It will remain high until OCxR = PWM timer, and then on the NEXT clock it will go low. This means that
 * the PWM output can NOT be configured to be high for a single clock, but minimum 2 clocks.
 * - Setting OCxR to 0, will result in a duty cycle of 0% (output permanently 0V)
 * - Setting OCxR equal to OCxRS, will result in a duty cycle of 100% (output permanently 3.3V)
 *
 * @subsection nz_pwm_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // --------------- nz_pwm Configuration (from nz_pwm.h) ----------------
 // *********************************************************************
 //#define    NZ_PWM_DISABLE                  //Don't manage PWM - PWM is enabled by default

 //Set default frequencies for PWM 1 to 9. Some predefined values are PWM_FREQ_1600HZ, PWM_FREQ_16KHZ, PWM_FREQ_80KHZ and PWM_FREQ_160KHZ
 //If using custom value, ensure it is a multiple of 100. This ensures pwmxSetPercent() functions works efficiently.
 #define PWM1_FREQ       PWM_FREQ_16KHZ     //PWM frequency is 16kHz
 #define PWM2_FREQ       PWM_FREQ_16KHZ     //PWM frequency is 16kHz
 #define PWM3_FREQ       PWM_FREQ_16KHZ     //PWM frequency is 16kHz
 #define PWM4_FREQ       PWM_FREQ_16KHZ     //PWM frequency is 16kHz
 #define PWM5_FREQ       PWM_FREQ_16KHZ     //PWM frequency is 16kHz
 #define PWM6_FREQ       PWM_FREQ_16KHZ     //PWM frequency is 16kHz
 #define PWM7_FREQ       PWM_FREQ_16KHZ     //PWM frequency is 16kHz
 #define PWM8_FREQ       PWM_FREQ_16KHZ     //PWM frequency is 16kHz
 #define PWM9_FREQ       PWM_FREQ_16KHZ     //PWM frequency is 16kHz

 @endcode
 **********************************************************************
 * @section nz_pwm_lic Software License Agreement
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
 *  - Initial version for 24FJ256GB110 at 32MHz
 *********************************************************************/
#ifndef _NZDEFAULT_PWM_H_
#define _NZDEFAULT_PWM_H_

#if defined(HAS_NZ_PWM)

#if (defined(__PIC24F__)) && defined(__C30__)	// Microchip C30 compiler
    #include "PPS.h"
#elif defined(__dsPIC33F__) && defined(__C30__)	// Microchip C30 compiler
#elif defined(__PIC32MX__)                      // Microchip C32 compiler
#else
	#error Unknown processor or compiler.  See Compiler.h
#endif


//The PWM clock is configured by this module to be 16MHz, giving a 62.5ns period
#define PWM_CLOCK_FREQ      (GetInstructionClock())
#define PWM_CLOCK_PERIOD    (1/GetInstructionClock())

typedef struct {
    WORD OCxCON1;
    WORD OCxCON2;
    WORD OCxRS;     //Period, pwm will count from 0 to OCxRS
    WORD OCxR;      //Duty Cycles
    WORD OCxTMR;
} REGS_OC_;

//For other chips, check OC1CON1 is at address 0x0190, and all OC registers follow each other from 0x0190 to 0x01E8
#if defined(__PIC24FJ128GB106__) || defined(__PIC24FJ256GB106__) || defined(__PIC24FJ256GB206__)
    extern volatile REGS_OC_ __attribute__ ((sfr(0x0190))) regs_oc_[9];
#elif defined(__PIC32MX__)                      // Microchip C32 compiler
    // TODO for PIC32MX - this code WILL NOT WORK!!!! Just here so it compiles & for testing!
    extern volatile REGS_OC_ regs_oc_[9];
#else
    #error "nz_pwm no processor defined!"
#endif

/**
 * Set PWM frequency to 320Hz, can be incremented and decremented in 0.005% steps (50,000 steps).
 * Causes PWM timer to have a range from 0-49,999 (50,000 positions)
 */
#define PWM_FREQ_320HZ  49999

/**
 * Set PWM frequency to 1.6kHz, can be incremented and decremented in 0.01% steps. Causes PWM timer
 * to have a range from 0-9999 (10000 positions)
 */
#define PWM_FREQ_1600HZ  9999

/**
 * Set PWM frequency to 3.2kHz. Duty cycle can be incremented and decremented in 0.02% steps. Causes
 * PWM timer to have a range from 0-4999 (5000 positions)
 */
#define PWM_FREQ_3200HZ   4999

/**
 * Set PWM frequency to 8kHz. Duty cycle can be incremented and decremented in 0.05% steps. Causes
 * PWM timer to have a range from 0-1999 (2000 positions)
 */
#define PWM_FREQ_8KHZ   1999

/**
 * Set PWM frequency to 16kHz. Duty cycle can be incremented and decremented in 0.1% steps. Causes
 * PWM timer to have a range from 0-999 (1000 positions)
 */
#define PWM_FREQ_16KHZ   999

/**
 * Set PWM frequency to 80kHz. Duty cycle can be incremented and decremented in 0.5% steps. Causes
 * PWM timer to have a range from 0-199 (100 positions).
 */
#define PWM_FREQ_80KHZ  199

/**
 * Set PWM frequency to 160kHz. Duty cycle can be incremented and decremented in 1% steps. Causes
 * PWM timer to have a range from 0-99 (100 positions)
 */
#define PWM_FREQ_160KHZ  99

#ifndef PWM1_FREQ
#define PWM1_FREQ       PWM_FREQ_16KHZ
#endif

#ifndef PWM2_FREQ
#define PWM2_FREQ        PWM_FREQ_16KHZ
#endif

#ifndef PWM3_FREQ
#define PWM3_FREQ       PWM_FREQ_16KHZ
#endif

#ifndef PWM4_FREQ
#define PWM4_FREQ       PWM_FREQ_16KHZ
#endif

#ifndef PWM5_FREQ
#define PWM5_FREQ       PWM_FREQ_16KHZ
#endif

#ifndef PWM6_FREQ
#define PWM6_FREQ       PWM_FREQ_16KHZ
#endif

#ifndef PWM7_FREQ
#define PWM7_FREQ       PWM_FREQ_16KHZ
#endif

#ifndef PWM8_FREQ
#define PWM8_FREQ       PWM_FREQ_16KHZ
#endif

#ifndef PWM9_FREQ
#define PWM9_FREQ       PWM_FREQ_16KHZ
#endif


/**
 * Macro for calculating the period constant required for the given frequency. It is meant
 * to be used with the pwmSetPeriod() and pwm1SetPeriod() to pwm9SetPeriod() functions.
 *
 * !!!!! IMPORTANT !!!!! <br>
 * Requested frequency MUST BE a constant value! Do not pass a variable! For example: <br>
 * PWM_CALCULATE_PERIOD_FOR_FREQUENCY(1000) <br>
 * Do NOT use with variable like PWM_CALCULATE_PERIOD_FOR_FREQUENCY(myfreq) !!!
 *
 * The returned period will result in a frequency as close as possible to the requested
 * frequency. Requested frequencies should have a value between 245 to 8,000,000 (244Hz to
 * 8MHz). The higher the frequency, the lower the PWM resolution (duty cycle steps) will be.
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
 * @param f Requested frequency to calculated a period constant for
 *
 * @return A constant period value for the requested frequency. Is used as input for the
 *      pwmSetPeriod() and pwm1SetPeriod() to pwm9SetPeriod() functions.
 */
#define PWM_CALCULATE_PERIOD_FOR_FREQUENCY(f) ((PWM_CLOCK_FREQ/f)-1)


/**
 * Configure given port as PWM 1 output. This function also configure PPS pin as a PWM output.
 * The pwm frequency is set to the value given by PWM1_FREQ define, and is 16kHz by default.
 * To change this to 80kHz for example, place following code in projdefs.h file:<br>
 * @code
 * #define PWM1_FREQ     PWM_FREQ_80KHZ
 * @endcode
 *
 * @preCondition PPS Must be unlocked! Ensure "PPSUnLock;" macro is called before calling this function!
 *
 * @param ppsOut A relocatable output port. For the location of the Relocatable Ports,
 *      see http://netcruzer.com/nzdoc-sbc66-ports/. All relocatable output ports are defined, and
 *      have the format PPS_OUT_XX, where XX is the port name. For example, to configure port 38 (old port name Y8)
 *      as the PWM 1 output, use PPS_OUT_38 for this parameter.
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
#define pwm1OpenDefault(ppsOut, inverted) {ppsOut = OUT_FN_PPS_OC1; pwmOpenDefault(1, PWM1_FREQ, inverted);}

/**
 * Configure given port as PWM 2 output. This function also configure PPS pin as a PWM output.
 * The pwm frequency is set to the value given by PWM2_FREQ define, and is 16kHz by default.
 * To change this to 80kHz for example, place following code in projdefs.h file:<br>
 * @code
 * #define PWM2_FREQ     PWM_FREQ_80KHZ
 * @endcode
 *
 * @preCondition PPS Must be unlocked! Ensure "PPSUnLock;" macro is called before calling this function!
 *
 * @param ppsOut A relocatable output port. For the location of the Relocatable Ports,
 *      see http://netcruzer.com/nzdoc-sbc66-ports/. All relocatable output ports are defined, and
 *      have the format PPS_OUT_XX, where XX is the port name. For example, to configure port 38 (old port name Y8)
 *      as the PWM 2 output, use PPS_OUT_38 for this parameter.
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
#define pwm2OpenDefault(ppsOut, inverted) {ppsOut = OUT_FN_PPS_OC2; pwmOpenDefault(2, PWM2_FREQ, inverted);}

/**
 * Configure given port as PWM 3 output. This function also configure PPS pin as a PWM output.
 * The pwm frequency is set to the value given by PWM3_FREQ define, and is 16kHz by default.
 * To change this to 80kHz for example, place following code in projdefs.h file:<br>
 * @code
 * #define PWM3_FREQ     PWM_FREQ_80KHZ
 * @endcode
 *
 * @preCondition PPS Must be unlocked! Ensure "PPSUnLock;" macro is called before calling this function!
 *
 * @param ppsOut A relocatable output port. For the location of the Relocatable Ports,
 *      see http://netcruzer.com/nzdoc-sbc66-ports/. All relocatable output ports are defined, and
 *      have the format PPS_OUT_XX, where XX is the port name. For example, to configure port 38 (old port name Y8)
 *      as the PWM 3 output, use PPS_OUT_38 for this parameter.
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
#define pwm3OpenDefault(ppsOut, inverted) {ppsOut = OUT_FN_PPS_OC3; pwmOpenDefault(3, PWM3_FREQ, inverted);}

/**
 * Configure given port as PWM 4 output. This function also configure PPS pin as a PWM output.
 * The pwm frequency is set to the value given by PWM4_FREQ define, and is 16kHz by default.
 * To change this to 80kHz for example, place following code in projdefs.h file:<br>
 * @code
 * #define PWM4_FREQ     PWM_FREQ_80KHZ
 * @endcode
 *
 * @preCondition PPS Must be unlocked! Ensure "PPSUnLock;" macro is called before calling this function!
 *
 * @param ppsOut A relocatable output port. For the location of the Relocatable Ports,
 *      see http://netcruzer.com/nzdoc-sbc66-ports/. All relocatable output ports are defined, and
 *      have the format PPS_OUT_XX, where XX is the port name. For example, to configure port 38 (old port name Y8)
 *      as the PWM 4 output, use PPS_OUT_38 for this parameter.
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
#define pwm4OpenDefault(ppsOut, inverted) {ppsOut = OUT_FN_PPS_OC4; pwmOpenDefault(4, PWM4_FREQ, inverted);}

/**
 * Configure given port as PWM 5 output. This function also configure PPS pin as a PWM output.
 * The pwm frequency is set to the value given by PWM5_FREQ define, and is 16kHz by default.
 * To change this to 80kHz for example, place following code in projdefs.h file:<br>
 * @code
 * #define PWM5_FREQ     PWM_FREQ_80KHZ
 * @endcode
 *
 * @preCondition PPS Must be unlocked! Ensure "PPSUnLock;" macro is called before calling this function!
 *
 * @param ppsOut A relocatable output port. For the location of the Relocatable Ports,
 *      see http://netcruzer.com/nzdoc-sbc66-ports/. All relocatable output ports are defined, and
 *      have the format PPS_OUT_XX, where XX is the port name. For example, to configure port 38 (old port name Y8)
 *      as the PWM 5 output, use PPS_OUT_38 for this parameter.
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
#define pwm5OpenDefault(ppsOut, inverted) {ppsOut = OUT_FN_PPS_OC5; pwmOpenDefault(5, PWM5_FREQ, inverted);}

/**
 * Configure given port as PWM 6 output. This function also configure PPS pin as a PWM output.
 * The pwm frequency is set to the value given by PWM6_FREQ define, and is 16kHz by default.
 * To change this to 80kHz for example, place following code in projdefs.h file:<br>
 * @code
 * #define PWM6_FREQ     PWM_FREQ_80KHZ
 * @endcode
 *
 * @preCondition PPS Must be unlocked! Ensure "PPSUnLock;" macro is called before calling this function!
 *
 * @param ppsOut A relocatable output port. For the location of the Relocatable Ports,
 *      see http://netcruzer.com/nzdoc-sbc66-ports/. All relocatable output ports are defined, and
 *      have the format PPS_OUT_XX, where XX is the port name. For example, to configure port 38 (old port name Y8)
 *      as the PWM 6 output, use PPS_OUT_38 for this parameter.
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
#define pwm6OpenDefault(ppsOut, inverted) {ppsOut = OUT_FN_PPS_OC6; pwmOpenDefault(6, PWM6_FREQ, inverted);}

/**
 * Configure given port as PWM 7 output. This function also configure PPS pin as a PWM output.
 * The pwm frequency is set to the value given by PWM7_FREQ define, and is 16kHz by default.
 * To change this to 80kHz for example, place following code in projdefs.h file:<br>
 * @code
 * #define PWM7_FREQ     PWM_FREQ_80KHZ
 * @endcode
 *
 * @preCondition PPS Must be unlocked! Ensure "PPSUnLock;" macro is called before calling this function!
 *
 * @param ppsOut A relocatable output port. For the location of the Relocatable Ports,
 *      see http://netcruzer.com/nzdoc-sbc66-ports/. All relocatable output ports are defined, and
 *      have the format PPS_OUT_XX, where XX is the port name. For example, to configure port 38 (old port name Y8)
 *      as the PWM 7 output, use PPS_OUT_38 for this parameter.
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
#define pwm7OpenDefault(ppsOut, inverted) {ppsOut = OUT_FN_PPS_OC7; pwmOpenDefault(6, PWM7_FREQ, inverted);}

/**
 * Configure given port as PWM 8 output. This function also configure PPS pin as a PWM output.
 * The pwm frequency is set to the value given by PWM8_FREQ define, and is 16kHz by default.
 * To change this to 80kHz for example, place following code in projdefs.h file:<br>
 * @code
 * #define PWM8_FREQ     PWM_FREQ_80KHZ
 * @endcode
 *
 * @preCondition PPS Must be unlocked! Ensure "PPSUnLock;" macro is called before calling this function!
 *
 * @param ppsOut A relocatable output port. For the location of the Relocatable Ports,
 *      see http://netcruzer.com/nzdoc-sbc66-ports/. All relocatable output ports are defined, and
 *      have the format PPS_OUT_XX, where XX is the port name. For example, to configure port 38 (old port name Y8)
 *      as the PWM 8 output, use PPS_OUT_38 for this parameter.
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
#define pwm8OpenDefault(ppsOut, inverted) {ppsOut = OUT_FN_PPS_OC8; pwmOpenDefault(8, PWM8_FREQ, inverted);}

/**
 * Configure given port as PWM 9 output. This function also configure PPS pin as a PWM output.
 * The pwm frequency is set to the value given by PWM9_FREQ define, and is 16kHz by default.
 * To change this to 80kHz for example, place following code in projdefs.h file:<br>
 * @code
 * #define PWM9_FREQ     PWM_FREQ_80KHZ
 * @endcode
 *
 * @preCondition PPS Must be unlocked! Ensure "PPSUnLock;" macro is called before calling this function!
 *
 * @param ppsOut A relocatable output port. For the location of the Relocatable Ports,
 *      see http://netcruzer.com/nzdoc-sbc66-ports/. All relocatable output ports are defined, and
 *      have the format PPS_OUT_XX, where XX is the port name. For example, to configure port 38 (old port name Y8)
 *      as the PWM 9 output, use PPS_OUT_38 for this parameter.
 *
 * @param inverted If 0, PWM output is normal. If 1, PWM output is inverted, meaning 0% duty
 *      cycle is constant 1 (3.3V), and 100% is 0V.
 */
#define pwm9OpenDefault(ppsOut, inverted) {ppsOut = OUT_FN_PPS_OC9; pwmOpenDefault(9, PWM9_FREQ, inverted);}

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
void pwmOpenDefault(BYTE chan, WORD period, BYTE inverted);


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
 * Where PWM_CLOCK_FREQ is 16,000,000 for all current boards
 *
 * For example, a period of 999 gives a PWM Period of 62.5us, and a frequency of 16kHz.
 *
 * @param period Constant used for setting the PWM Period. Can use the PWMx_FREQ defines for pre-calculated
 *      values, or the PWM_CALCULATE_PERIOD_FOR_FREQUENCY macro to get a value for a specific frequency.
 *      The resulting PWM Period is calculated (PWM_CLOCK_PERIOD is 62.5ns for all current boards):<br>
 *      PWM Period = ((period+1) * PWM_CLOCK_PERIOD
 */
#define pwm1SetPeriod(period)  pwmSetPeriod(1, period)
#define pwm2SetPeriod(period)  pwmSetPeriod(2, period)
#define pwm3SetPeriod(period)  pwmSetPeriod(3, period)
#define pwm4SetPeriod(period)  pwmSetPeriod(4, period)
#define pwm5SetPeriod(period)  pwmSetPeriod(5, period)
#define pwm6SetPeriod(period)  pwmSetPeriod(6, period)
#define pwm7SetPeriod(period)  pwmSetPeriod(7, period)
#define pwm8SetPeriod(period)  pwmSetPeriod(8, period)
#define pwm9SetPeriod(period)  pwmSetPeriod(9, period)


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
void pwmSetPeriod(BYTE chan, WORD period);


/**
 * Gets the period value of this PWM. The actual period is the returned
 * value * PWM_CLOCK_PERIOD (62.5ns for all current boards).
 *
 * @param chan PWM Channel, must be a value from 1 to 9
 *
 * @return Constant used for the period value for this PWM Channel. The actual period
 *      is the returned value * PWM_CLOCK_PERIOD (62.5ns for all current boards)
 */
WORD pwmGetPeriod(BYTE chan);


/**
 * Gets the period (OC1RS register) value for this PWM. The actual period is calculated by
 * multiplying this value by the PWM time base PWM_CLOCK_PERIOD
 */
#define pwm1GetPeriod() (OC1RS)
#define pwm2GetPeriod() (OC2RS)
#define pwm3GetPeriod() (OC3RS)
#define pwm4GetPeriod() (OC4RS)
#define pwm5GetPeriod() (OC5RS)
#define pwm6GetPeriod() (OC6RS)
#define pwm7GetPeriod() (OC7RS)
#define pwm8GetPeriod() (OC8RS)
#define pwm9GetPeriod() (OC9RS)


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
void pwmSetFreqAndDutyCycle(BYTE chan, float freq, float dc);


/**
 * Gets the frequency (in Hertz) of the given PWM channel.
 *
 * @param chan PWM Channel, must be a value from 1 to 9
 *
 * @return Returns the frequency (in Hertz) of the given PWM channel. The returned
 * frequency is a floating point value.
 */
float pwmGetFrequency(BYTE chan);


/**
 * Set the Duty Cycle constant for a PWM channel. Depending on current "pwm period" value
 * for this PWM channel, this will configure the PWM for different duty cycles.
 * When dc = 0, duty cycle is 0%. When dc = "pwm period", duty cycle will be 100%.
 * For other values, duty cycle is a ratio.
 *
 * @param dc The duty cycle, a value from 0 to "pwm period". The pwmOpenDefault() function sets the
 *      "pwm period" to PWMx_FREQ define (default PWM_FREQ_16KHZ = 999).
 */
#define pwm1SetDutyCycle(dc)  pwmSetDutyCycle(1, dc)
#define pwm2SetDutyCycle(dc)  pwmSetDutyCycle(2, dc)
#define pwm3SetDutyCycle(dc)  pwmSetDutyCycle(3, dc)
#define pwm4SetDutyCycle(dc)  pwmSetDutyCycle(4, dc)
#define pwm5SetDutyCycle(dc)  pwmSetDutyCycle(5, dc)
#define pwm6SetDutyCycle(dc)  pwmSetDutyCycle(6, dc)
#define pwm7SetDutyCycle(dc)  pwmSetDutyCycle(7, dc)
#define pwm8SetDutyCycle(dc)  pwmSetDutyCycle(8, dc)
#define pwm9SetDutyCycle(dc)  pwmSetDutyCycle(9, dc)

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
void pwmSetDutyCycle(BYTE chan, WORD dc);
//#define pwmSetDutyCycle(chan,dc)  {if(chan==1) OC1R = dc; else if(chan==2) OC2R = dc; else if .....}  //Alternative method


/**
 * !!!!! IMPORTANT !!!!! <br>
 * The given period MUST BE a constant value! Do not pass a variable! For example: <br>
 * pwm3SetPercent(PWM3_FREQ, pwmValue) or <br>
 * pwm3SetPercent(PWM_CALCULATE_PERIOD_FOR_FREQUENCY(1000), pwmValue) <br>
 * Do NOT use with variable like pwm3SetPercent(myPer, pwmValue) !!!
 *
 * Set duty cycle (in percentage) of PWM channel. Note when PWMx_FREQ is 160kHz, 1% is not possible, and will result in 0%.
 * From 2%, it works as expected. For all other PWMx_FREQ values, all percentages work as expected.
 *
 * This function works best when the default period of the PWM channel is used. That is,
 * when no pwmSetPeriod() or pwmSetFreqAndDutyCycle() functions are called after opening
 * the PWM channel! The reason is because the default pwm periods as defined by the
 * PWMx_FREQ defines are dividable by 100, and work for each 1% step.
 * 
 * @param period Constant used for setting the PWM Period. Can use the PWMx_FREQ defines for pre-calculated
 *      values, or the PWM_CALCULATE_PERIOD_FOR_FREQUENCY macro to get a value for a specific frequency.
 *      The resulting PWM Period is calculated (PWM_CLOCK_PERIOD is 62.5ns for all current boards):<br>
 *      PWM Period = ((period+1) * PWM_CLOCK_PERIOD
 *
 * @param percent The percentage to set the duty cycle to. A value from 0 to 100. Can also be a fraction, depending
 *      on PWMx_FREQ define. See nz_pwm.h description for details.
 */
#define pwm1SetPercent(period, percent)  (OC1R = ((((period+1)/100) * percent)==0) ? 0 : ((((period+1)/100) * percent)-1))
#define pwm2SetPercent(period, percent)  (OC2R = ((((period+1)/100) * percent)==0) ? 0 : ((((period+1)/100) * percent)-1))
#define pwm3SetPercent(period, percent)  (OC3R = ((((period+1)/100) * percent)==0) ? 0 : ((((period+1)/100) * percent)-1))
#define pwm4SetPercent(period, percent)  (OC4R = ((((period+1)/100) * percent)==0) ? 0 : ((((period+1)/100) * percent)-1))
#define pwm5SetPercent(period, percent)  (OC5R = ((((period+1)/100) * percent)==0) ? 0 : ((((period+1)/100) * percent)-1))
#define pwm6SetPercent(period, percent)  (OC6R = ((((period+1)/100) * percent)==0) ? 0 : ((((period+1)/100) * percent)-1))
#define pwm7SetPercent(period, percent)  (OC7R = ((((period+1)/100) * percent)==0) ? 0 : ((((period+1)/100) * percent)-1))
#define pwm8SetPercent(period, percent)  (OC8R = ((((period+1)/100) * percent)==0) ? 0 : ((((period+1)/100) * percent)-1))
#define pwm9SetPercent(period, percent)  (OC9R = ((((period+1)/100) * percent)==0) ? 0 : ((((period+1)/100) * percent)-1))


/**
 * Set the Duty Cycle % for given PWM channel. Note when PWM1_FREQ is 160kHz, 1% is not possible, and will result in 0%.
 * From 2%, it works as expected. For all other PWM1_FREQ values, all percentages work as expected.
 *
 * @param chan PWM Channel to open, must be a value from 1 to 9
 *
 * @param percent
 */
//#define pwmSetPercent(chan, percent)  {if((chan>0)&&(chan<10)) {regs_oc_[chan-1].OCxR = ((((PWM2_FREQ+1)/100) * percent)==0) ? 0 : ((((PWM2_FREQ+1)/100) * percent)-1);}}
//void pwmSetPercent(BYTE ch, WORD p);

#endif  //#if defined(HAS_NZ_PWM)
#endif
