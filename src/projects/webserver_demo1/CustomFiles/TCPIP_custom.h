/**
 * Project Specific TCP/IP Defines. Overwrites values defined in SBC TCPIP_xx.h files.
 */
#ifndef _TCPIP_CUSTOM_H_
#define _TCPIP_CUSTOM_H_

// =======================================================================
//   Defines not present in TCPIP_xxx.h file
// =======================================================================
#define MY_DEFAULT_DHCP_ENABLED         (1)



// =======================================================================
//   Select TCPIP_xxx.h file (created by config tool)
// =======================================================================
//To replace board's default TCPIP_xx.h file, uncomment following code
#define DONT_INCLUDE_BOARD_TCPIP_FILE
#include "CustomFiles/TCPIP_custom_SBC66EC-R2.h"

#endif  //_TCPIP_CUSTOM_H_
