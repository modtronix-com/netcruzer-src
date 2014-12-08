/**
 * Project Specific Defines. Are defines in projdefs.h. These are defines that
 * is the same for all target hardware. Place hardware specific defines in
 * "Configs/HWP_BOARDNAME.h" file for target board
 */

#ifndef _PROJDEFS_H_
#define _PROJDEFS_H_


//Ensure this define is uncommented for release build
#define RELEASE_BUILD

 // *********************************************************************
 // --------------- nz_pwm Configuration (from nz_pwm.h) ----------------
 // *********************************************************************
 //Set default frequencies for PWM 1. Some predefined values are PWM_FREQ_1600HZ, PWM_FREQ_16KHZ, PWM_FREQ_80KHZ and PWM_FREQ_160KHZ
 //If using custom value, ensure it is a multiple of 100. This ensures pwmxSetPercent() functions works efficiently.
 #define PWM2_FREQ       PWM_FREQ_1600HZ     //PWM frequency is 1.6kHz


#endif  //_PROJDEFS_H_
