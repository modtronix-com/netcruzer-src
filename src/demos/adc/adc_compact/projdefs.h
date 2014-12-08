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
// ---------- ADC Filtered Configuration (from nz_analog.h) ------------
// *********************************************************************
//Number of filter stages for each ADC channel. Must be 2, 4 or 8. For each ADC channel, this many
//words are needed. Total required memory is ADC_MAX_CHAN x ADC_FILTERS words.
#define ADC_FILTER_STAGES  4            //[-DEFAULT-]

//Maximum number of possible ADC channels. Default value is taken from board include file, and reserves memory for
//all possible ADC channels. To reduce memory used, define this value to be equal to actual number of ADC channels used.
#define ADC_MAX_CHAN   2

//To reduce size of the nz_analog module, following defines can be added to projdefs.h
#define ADC_NO_MV_FUNCTIONS        //No millivolt functions included
#define ADC_NO_CONVERT_FUNCTIONS   //No convert functions included
#define ADC_NO_CHAN_FUNCTIONS      //Don't support any channel functions - use indexed functions


#endif  //_PROJDEFS_H_
