/**
 * @brief           Interrupt
 * @file            interrupt.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section interrupt_desc Description
 *****************************************
 * This module contains code for interrupts
 *
 * @subsection interrupt_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --------------- Interrupt Configuration (interrupt.h) --------------------
// *********************************************************************
#define INT_LVL_TEST	4

 @endcode
 *********************************************************************
 * @section interrupt_lic Software License Agreement
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
 * 2010-11-16, David H. (DH):
 *  - Initial version
 *********************************************************************/
#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

//#include "nz_serDataPorts.h"

/////////////////////////////////////////////////
//  Interrupt - documentation module
/////////////////////////////////////////////////
/** @addtogroup proj_webserver_int
<p>Some code run in ISRs (interrupt service routines). It is important to know what code is implemented in ISRs,
and what the ISR level is. There are 7 interrupt levels, with 7 being the highest.

Each interrupt can be assigned an interrupt level from 1 to 7 (0 is disabled). In addition, interrupts have a
natural interrupt level. If two interrupts assigned the same interrupt level are active, the one
with the highest natural level will be processed. For example, the natural interrupt level of the Timer 1 interrupt
is 3, which is very high. Only the Capture/Compare Module 1 has an higher natural priority.
The interrupt levels used are (7 has highest priority):

The interrupt levels used are (7 has highest priority):
- Level 7: 
- Level 6: 
- Level 5:
	- System tick (NP=3), uses TMR1. Set by nzINT_PRIORITY_TICK define in projdefs.h
- Level 4:
	- UART1 (NP=11&12). Set by nzINT_PRIORITY_UART1 define in projdefs.h
	- Analog to Digital Converter (NP=13). Set by nzINT_PRIORITY_ADC1 define in projdefs.h
	- UART2 (NP=30&31). Set by nzINT_PRIORITY_UART2 define in projdefs.h
	- UART3 (NP=82&83). Set by nzINT_PRIORITY_UART3 define in projdefs.h
	- UART4 (NP=88&89). Set by nzINT_PRIORITY_UART4 define in projdefs.h
- Level 3:
	- I2C 1 Master (NP=17). Set by nzINT_PRIORITY_I2C1 define in projdefs.h
	- I2C 2 Master (NP=50). Set by nzINT_PRIORITY_I2C2 define in projdefs.h
	- I2C 3 Master (NP=85). Set by nzINT_PRIORITY_I2C3 define in projdefs.h
	- USB (NP=86). Set with USB1IP in usb_hal_pic24E.h and usb_hal_dspic33E.h
- Level 2: 
    - Reserved for future "RTOS Kernel Timer Interrupt"
- Level 1: 
    - Reserved for future "RTOS Kernel" and "RTOS Fibre"
- Level 0: 
    - Reserved for future "RTOS User Threads"
	
Where (NP=n) gives natural priority. See description above for details.

*/
/* Not documented yet! To document, add to section above!
@section proj_webserver_int_levels Interrupt Levels
*/


#endif	//#ifndef _INTERRUPT_H_

