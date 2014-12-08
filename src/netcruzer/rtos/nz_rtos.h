/**
 * @brief           Netcruzer RTOS Main Include file
 * @file            nz_rtos.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 & XC32 Compilers
 *
 * @section nzos_rtos_desc Description
 *****************************************
 * RTOS Functions
 * 
 * @subsection nzos_rtos_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --------- Netcruzer RTOS Configuration (from nzos_rtos.h) -----------
// *********************************************************************
#define nzosENABLE                              ( 1 )
#define nzosINT_FOR_RTOS_CUSTOM                 ( 0 )
#define nzosHEAP_SIZE                           ( 2048 )
#define nzTckGetUtick()                          utick32Get()

// ----- Thread Configuration -----
#define nzosTHREAD_ENABLE                       ( 0 )
// ----- Fiber Configuration -----
#define nzosFIBER_ENABLE                        ( 0 )
// ----- FSM Thread Configuration -----
#define nzosFSM_ENABLE                          ( 0 )
// ----- Memory Configuration -----
#define nzosMEM_USE1                            ( 1 )


 @endcode
 * 
 **********************************************************************
 * @section nzos_rtos_lic Software License Agreement
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
 * 2014-02-02, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_RTOS_H
#define NZ_RTOS_H

/////////////////////////////////////////////////
//  "RTOS" - documentation module
/////////////////////////////////////////////////
/**
@defgroup info_rtos Netcruzer RTOS
Currently the Netcruzer RTOS is very basic, and only implements @ref info_rtos_fiber "Fibers".
We plan to extend this in future releases to include light tasks called "FSM Tasks" (State Machine
Tasks) and possibly full co-operative or pre-emptive Tasks.

All RTOS file are located in the "lib/rtos" folder, and are written in a way so that they
are independent of the main Netcruzer code. Meaning it will be possible replacing it for
a different operating system like Free RTOS for example.

In the current state, with only @ref info_rtos_fiber "Fibers" implemented, it will not clash with
the majority of 3rd party RTOS implementations (Free RTOS, NTKernel...). The 3rd party RTOS can simply
be added without removing the current Netcruzer RTOS.

The code of the Netcruzer RTOS consists of two main modules, the "RTOS Timer ISR", and the
"RTOS Kernel".
The "RTOS Timer ISR" is called from the main system tick interrupt, and has the same priority.
The "RTOS Kernel" is ran as a Fiber at the lowest interrupt priority.
See the @ref info_sys_int "System Interrupts" section for details on interrupt priorities used by the system.

@section info_rtos_usage_fiber Usage when only using Fibers
One of the most useful and basic features of the Netcruzer RTOS are Fibers. They are used by many
project to add low priority processing of data stored via high priority interrupts.
To add the Netcruzer RTOS with Fiber support to a project, do the following:
- Add the following files located in "..src/netcruzer/rtos" to the MPLAB project: nzos_main.c, nzos_mem.c, nzos_fiber.c
- In projdefs.h, do any "Netcruzer RTOS" configuration required. This is done by copying the
  @ref nzos_rtos_conf "Configuration Section" from nz_rtos.h to projdefs.h, and configuring as required.

*/

#if (nzosENABLE==1)


////////// Default Defines //////////////////////
#if !defined(nzosHEAP_SIZE)
#define nzosHEAP_SIZE                           ( 2048 )
#endif

#if !defined(nzosMAIN_INT_PRIORITY)
#define nzosMAIN_INT_PRIORITY                   ( 2 )
#endif

#if !(nzosMEM1_ENABLE) && !(nzosMEM2_ENABLE) && !(nzosMEM3_ENABLE) && !(nzosMEM4_ENABLE)
#define nzosMEM3_ENABLE                         ( 1 )
#endif

#if !defined(nzosINT_FOR_RTOS_CUSTOM)
#define nzosINT_FOR_RTOS_CUSTOM					( 0 )
#endif

#if !defined(nzosINCLUDE_PORT_C_FILE)
#define nzosINCLUDE_PORT_C_FILE					( 1 )
#endif


////////// Error Codes //////////////////////////
#define NZ_RTOS_SUCCESS                         ( 0 )
#define NZ_RTOS_ERROR                           ( 0xffff )

#define NZ_THR_SUCCESS                          ( 0 )
#define NZ_THR_ERR_CREATE                       ( 0xffef )

#define NZ_FBR_SUCCESS                          ( 0 )
#define NZ_FBR_ERR_CREATE                       ( 0xffdf )
#define NZ_FBR_ERR_LEVEL_FULL                   ( 0xffde )
#define NZ_FBR_ERR_INVALID_LEVEL                ( 0xffdd )

////////// Global Variables /////////////////////
#if defined(__PIC24F__)
    #if (nzosINT_FOR_RTOS_CUSTOM==0)
        //Use I2C1 Slave interrupt
        #define NZOS_INT_KERNEL_IF                    (_SI2C1IF)
        #define NZOS_INT_KERNEL_IE                    (_SI2C1IE)
        #define NZOS_INT_KERNEL_IP                    (_SI2C1IP)

		#define NZOS_INT_KERNEL_CLEAR_IF()            (NZOS_INT_KERNEL_IF=0)
		#define NZOS_INT_KERNEL_SET_IF()              (NZOS_INT_KERNEL_IF=1)
		#define NZOS_INT_KERNEL_GET_IF()              (NZOS_INT_KERNEL_IF)
		#define NZOS_INT_KERNEL_SET_IP(prty)          (NZOS_INT_KERNEL_IP=prty)
		#define NZOS_INT_KERNEL_GET_IP()              (NZOS_INT_KERNEL_IP)
		#define NZOS_INT_KERNEL_ENABLE()              (NZOS_INT_KERNEL_IE = 1)
		#define NZOS_INT_KERNEL_DISABLE()             (NZOS_INT_KERNEL_IE = 0)
		#define NZOS_INT_KERNEL_GET_INT_ENABLE()      (NZOS_INT_KERNEL_IE)
	#endif
#elif defined(__PIC32MX__)
    //!!!!!!!!!! For PIC32MX, also consider TMR1_EXT
    #if (nzosINT_FOR_RTOS_CUSTOM==0)
        //Use Timer 3 interrupt
        #define NZOS_INT_KERNEL_CLEAR_IF()            mT3ClearIntFlag()
        #define NZOS_INT_KERNEL_SET_IF()              (IFS0SET = _IFS0_T3IF_MASK)
        #define NZOS_INT_KERNEL_GET_IF()              mT3GetIntFlag()
        #define NZOS_INT_KERNEL_SET_IP(prty)          mT3SetIntPriority(prty)
        #define NZOS_INT_KERNEL_GET_IP()              mT3GetIntPriority()
        #define NZOS_INT_KERNEL_ENABLE()              mT3IntEnable(1)
        #define NZOS_INT_KERNEL_DISNABLE()            mT3IntEnable(0)
        #define NZOS_INT_KERNEL_GET_INT_ENABLE()      mT3GetIntEnable()
    #endif
#endif


#endif  //#if (nzosENABLE==1)

#endif  //#ifndef NZ_RTOS_H
