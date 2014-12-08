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
// ---------- Netcruzer System Configuration (nz_netcruzer.h) ----------
// *********************************************************************
#define     NZSYS_DONT_MANAGE_USB           //Don't manage USB


// *********************************************************************
// -------------- USB stack hardware selection options -----------------
// *********************************************************************
//Use is HID
#define USB_IS_HID
//#define USB_IS_CDC


#endif  //_PROJDEFS_H_
