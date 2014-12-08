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
// ---------- Netcruzer Configuration (from nz_netcruzer.h) ---------------
// *********************************************************************
//Add this define to add I2C 1 bus at 400kbit/seconds, and add LCD2S support to project
#define  NZSYS_ENABLE_LCD2S_AND_I2C1

#endif  //_PROJDEFS_H_
