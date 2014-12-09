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
// ------------ ADC Filtered Configuration (from nz_analog.h) -------------
// *********************************************************************
//If following line is included in projdefs.h, 3.3V supply is used for reference. Default is 2.5V external precision reference.
#define ADC_3V3_VREF                       //Use internal 3.3V supply as reference

//Number of filter stages for each ADC channel. Must be 2, 4 or 8. For each ADC channel, this many
//words are needed. Total required memory is ADC_MAX_CHAN x ADC_FILTERS words.
#define ADC_FILTER_STAGES  8

// *********************************************************************
// ------------ Debounce Configuration (from nz_debounce.h) -------------
// *********************************************************************
//Define functions for reading each port's value.
#define NZ_DEBOUNCE_ENABLED
#define DEBOUNCE_GET_PORT1()    (!PIN_32)
#define DEBOUNCE_GET_PORT2()    (!PIN_33)


#endif  //_PROJDEFS_H_
