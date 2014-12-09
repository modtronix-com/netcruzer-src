 /**
 * @brief           Main Application code
 * @file            mainApp.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section mainapp_desc Description
 *****************************************
 * This module contains code for the main application.
 *
 * @subsection mainapp_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ------------ Main Application Configuration (mainapp.h) ------------------
// *********************************************************************

 @endcode
 *********************************************************************
 * @section mainapp_lic Software License Agreement
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
#ifndef MAINAPP_H
#define MAINAPP_H

//If any of the following is defined, we need to include AN0String code.
#if defined(STACK_USE_SNMP_SERVER) || defined(STACK_USE_TELNET_SERVER)
#define CALCULATE_AN0STRING
#endif
#if !defined(THIS_IS_STACK_APPLICATION)
	extern BYTE AN0String[8];
#endif

#define BAUD_RATE       (19200)		// bps

void SMTPDemo(void);
void PingDemo(void);
void SNMPTrapDemo(void);
void SNMPV2TrapDemo(void);
void GenericTCPClient(void);
void GenericTCPServer(void);


/**
 * Main Application Initialization
 */
void initMainApp(void);

/**
 * Main Application task, call in main loop
 */
void mainAppTask(void);


#endif

