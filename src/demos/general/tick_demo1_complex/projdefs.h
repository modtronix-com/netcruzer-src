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
// -------------- Tick Configuration --------------------
// *********************************************************************
//The following should be defined in the project's projdefs.h file OR on the command line
#define NZSYS_ENABLE_COMPLEX_TICK   //Indicates that the complex tick file nz_tickCx must be used, and not nz_tick


#endif  //_PROJDEFS_H_
