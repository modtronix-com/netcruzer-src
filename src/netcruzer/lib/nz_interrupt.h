/**
 * @brief           Code for using and configuring Interrupts
 * @file            nz_interrupt.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_interrupt_desc Description
 *****************************************
 * Code for using and configuring Interrupts
 *
 * @subsection nz_interrupt_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ---------------------- ioPorts Configuration ------------------------
// *********************************************************************

 @endcode
 *
 **********************************************************************
 * @section nz_interrupt_lic Software License Agreement
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
 * 2013-11-20, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_INTERRUPT_H
#define NZ_INTERRUPT_H

/////////////////////////////////////////////////
//  "Interrupts" - documentation module
/////////////////////////////////////////////////
/**
 @defgroup info_sys_int System Interrupts
 @ingroup info_sys
 Currently the Netcruzer system uses co-operative task scheduling. It is not a real multitasking system, and
 uses state machines in tasks.

 In the future, it might be extended to a real multi taking system. Interrupt level 1 is reserved for the
 "Kernel Scheduler", and interrupt level 2 for the "Kernel Timer".
 
 Each interrupt can be assigned an interrupt level from 1 to 7 (0 is disabled). In addition, interrupts have a
 natural interrupt level. If two interrupts assigned the same interrupt level are active, the one
 with the highest natural level will be processed. For example, the natural interrupt level of the Timer 1 interrupt
 is 3, which is very high. Only the Capture/Compare Module 1 has an higher natural priority.
 The interrupt levels used are (7 has highest priority):
 
 <b>Level 7:</b> 
 - Available for user Interrupts! Only use for highest priority interrupt, that stay in ISR for a very short time!
 
 <b>Level 6:</b>
 - Available for user Interrupts! Only use for high priority interrupt, that stay in ISR for a short time!
 
 <b>Level 5:</b>
 - Available for user Interrupts! Only use for high priority interrupt, that stay in ISR for a very short time!
 - System tick (NP=3), uses TMR1. Set by nzINT_PRIORITY_TICK define in projdefs.h. Calls "Netcruzer RTOS" nzRtosTick().
 
 <b>Level 4:</b>
 - Available for user Interrupts!
 - UART1 (NP=11&12). Set by nzINT_PRIORITY_UART1 define in projdefs.h
 - Analog to Digital Converter (NP=13). Set by nzINT_PRIORITY_ADC1 define in projdefs.h
 - UART2 (NP=30&31). Set by nzINT_PRIORITY_UART2 define in projdefs.h
 - UART3 (NP=82&83). Set by nzINT_PRIORITY_UART3 define in projdefs.h
 - UART4 (NP=88&89). Set by nzINT_PRIORITY_UART4 define in projdefs.h
 
 <b>Level 3:</b>
 - Available for user Interrupts!
 - I2C 1 Master (NP=17). Set by nzINT_PRIORITY_I2C1 define in projdefs.h
 - I2C 2 Master (NP=50). Set by nzINT_PRIORITY_I2C2 define in projdefs.h
 - I2C 3 Master (NP=85). Set by nzINT_PRIORITY_I2C3 define in projdefs.h
 - USB (NP=86). Set with USB1IP in usb_hal_pic24E.h and usb_hal_dspic33E.h
 
 <b>Level 2:</b>
 - Available for user Interrupts! Use for interrupts that have very low priority, and stay long in the ISR.
 
 <b>Level 1:</b>
 - Used by "Netcruzer RTOS" for "RTOS Kernel" and "Fiber Tasks". Uses "I2C 1 Slave" (NP=16) Interrupt by default.
 
 <b>Level 0:</b>
 - Used by "Netcruzer RTOS" for Tasks and "FSM Tasks"
 
 Where (NP=n) gives natural priority. See description above for details.
*/

#include "nz_ioPorts.h"

#define USE_AND_OR	//To enable AND_OR mask setting for ports.h

#if (defined(__PIC24F__)) && defined(__C30__)	// Microchip C30 compiler
    #include <ports.h>
#elif defined(__dsPIC33F__) && defined(__C30__)	// Microchip C30 compiler
#elif defined(__PIC32MX__)                      // Microchip C32 compiler
	#include <peripheral/ports.h>
#else
	#error Unknown processor or compiler.  See Compiler.h
#endif

/////////// Default Defines /////////////////////
#if defined( __C30__ )
// Setting the priority of External or CN interrupt
#define CHANGE_INT_ON				INT_ENABLE
#define CHANGE_INT_OFF              INT_DISABLE

// Change Interrupt Priority Values
#define CHANGE_INT_PRI_7            INT_PRI_7
#define CHANGE_INT_PRI_6            INT_PRI_6
#define CHANGE_INT_PRI_5            INT_PRI_5
#define CHANGE_INT_PRI_4            INT_PRI_4
#define CHANGE_INT_PRI_3            INT_PRI_3
#define CHANGE_INT_PRI_2            INT_PRI_2
#define CHANGE_INT_PRI_1            INT_PRI_1
#define CHANGE_INT_PRI_0            INT_PRI_0
#elif defined( __PIC32MX__ )
#else
    #error "Compiler not defined"
#endif


////////// Interrupt Enable/Disable defines /////
#if defined( __C30__ )
	#define NZ_INT_DIS_PUSH()											\
	asm volatile ("push SR \n disi #3 \n mov #0xE0, W0 \n ior SR"		\
	: /* no outputs */													\
	: /* no inputs */													\
	: "w0")

	//Seems like SR can only be changed with DISICNT is NOT = 0! Use disi to set DISICNT to non 0 value.
	#define NZ_INT_EN_POP()												\
	/* Allow 2 instructions for push and pop! Compiler does not always use PUSH and POP! */	\
	asm volatile ("disi #2 \n pop %0"									\
	: "+r" (SR)	/* Outputs */											\
	: /* no inputs */)

	//NOTE! With disi #4 causes errors! With disi #5 is OK!
	//Seems like SR can only be changed with DISICNT is NOT = 0! Use disi to set DISICNT to non 0 value.
	#define NZ_INT_DIS_SAVE(save_to)													\
	asm volatile ("disi #5 \n mov SR, W0 \n mov W0, %0 \n mov #0xE0, W0 \n ior SR"		\
	: "=g" (save_to) /* Outputs */														\
	: /* no inputs */																	\
	: "w0")

	//Seems like SR can only be changed with DISICNT is NOT = 0! Use disi to set DISICNT to non 0 value.
	#define NZ_INT_EN_SAVE(save_to)										\
	asm volatile ("disi #3 \n mov %1, W0 \n mov W0, %0"					\
	: "+r" (SR)	/* Outputs */											\
	: "g" (save_to) /* Inputs */										\
	: "w0")

	/** Disable fibers */
    #if !defined(NZ_INT_DISABLE_FIBERS)
        #define NZ_INT_DISABLE_FIBERS()		NZOS_INT_KERNEL_DISABLE()
    #endif

    /** Disable fibers */
    #if !defined(NZ_INT_ENABLE_FIBERS)
        #define NZ_INT_ENABLE_FIBERS()		NZOS_INT_KERNEL_ENABLE()
    #endif

    /**
	 * Disable All Interrupt level, excluding level 7.
	 * For TMR1 and TMR2 interrupts (maybe more), if this method is used to disable interrupt,
	 * the IF is NOT set while interrupts are disabled!!!! Using NZ_SET_AND_SAVE_CPU_IPL to
	 * disabled interrupts does NOT disable these IF from getting set!
	 */
    #if !defined(NZ_INT_DISABLE_P07)
        #define NZ_INT_DISABLE_P07()			__builtin_disi(0x3FFF)	// Disable interrupts, excluding level 7
    #endif

    /** Enable All Interrupt levels */
    #if !defined(NZ_INT_ENABLE_P07)
        #define NZ_INT_ENABLE_P07()				__builtin_disi(0)		// Enable interrupts
    #endif

    /** Disable interrupt for given number of instruction cycles  */
    #if !defined(NZ_BUILTIN_DISI)
        #define NZ_BUILTIN_DISI(val)		__builtin_disi(val)		// Disable interrupts
    #endif

    /**
     * Set CPU IPL to new level, and save current level to given var.
     * All interrupts equal to AND lower than given level will be disabled.
     */
    #if !defined(NZ_SET_AND_SAVE_CPU_IPL)
        #define NZ_SET_AND_SAVE_CPU_IPL(ipl,lvl)    SET_AND_SAVE_CPU_IPL(ipl,lvl)
    #endif

	/** Restore CPU IPL to given level.  */
    #if !defined(NZ_RESTORE_CPU_IPL)
        #define NZ_RESTORE_CPU_IPL(ipl)             RESTORE_CPU_IPL(ipl)
    #endif
#elif defined( __PIC32MX__ )

	// TODO for PIC32MX - implement these macros!! Just here so it compiles.
	#define NZ_INT_DIS_PUSH()
	#define NZ_INT_EN_POP()

	/** Disable fibers */
    #if !defined(NZ_INT_DISABLE_FIBERS)
        #define NZ_INT_DISABLE_FIBERS()		//DUMMY!!! Complete for PIC32MX!!!!!!!
    #endif

    /** Disable fibers */
    #if !defined(NZ_INT_ENABLE_FIBERS)
        #define NZ_INT_ENABLE_FIBERS()		//DUMMY!!! Complete for PIC32MX!!!!!!!
    #endif

	/** Disable fibers */
    #if !defined(NZ_INT_DISABLE_FIBERS)
        #define NZ_INT_DISABLE_FIBERS()		NZOS_INT_KERNEL_DISABLE()
    #endif
    /** Disable fibers */
    #if !defined(NZ_INT_ENABLE_FIBERS)
        #define NZ_INT_ENABLE_FIBERS()		NZOS_INT_KERNEL_ENABLE()
    #endif

	/** Disable interrupt for given number of instruction cycles  */
    #if !defined(NZ_BUILTIN_DISI)
		// TODO for PIC32MX - implement these macros!! Just here so it compiles.
        #define NZ_BUILTIN_DISI(val)
    #endif
    /**
     * Set CPU IPL to new level, and save current level. A interrupt below given level
     * will be disabled.
     */
    #if !defined(NZ_SET_AND_SAVE_CPU_IPL)
        #define NZ_SET_AND_SAVE_CPU_IPL(ipl,lvl)
    #endif
    /** Restore CPU IPL to given level. */
    #if !defined(NZ_RESTORE_CPU_IPL)
        #define NZ_RESTORE_CPU_IPL(ipl)
    #endif
#else
    #error "Compiler not defined"
#endif


/**
 * @function void intOnChangeConfig(BYTE portID)
 *
 * Configure and Enable "Interrupt on Change". For example:
 *
 * @code
 * //Interrupt on change ISR
 * void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void) {
 *     intOnChangeClearIF();   //Clear "interrupt on change" IF (interrupt flag)
 *     //Do something....
 * }
 *
 * //Configure the interrupt on change for ports 7 and 8 (old port names X7 and X8)
 * enableIntOnChange(CHANGE_INT_ON|CHANGE_INT_PRI_6);	//Enable Interrupt on chagne at Int Priority level 4
 * intOnChangeClearIF();								//Clear "interrupt on change" IF (interrupt flag)
 * enableIntOnChange(7);					//Enable "interrupt on change" for Port 7
 * enableIntOnChange(8);					//Enable "interrupt on change" for Port 8
 * @endcode
 *
 * @param conf  An OR'ed combination of:
 *      - CHANGE_INT_ON    - Enable Interrupt on Change
 *      - CHANGE_INT_OFF   - Enable Interrupt on Change
 *      - CHANGE_INT_PRI_n - Where n is Interrupt priority 0-7. 0 is the lowest.
 */
#if defined(__C30__)
    #define intOnChangeConfig(conf) ConfigIntCN(conf)
#elif defined(__C32__)
	// TODO for PIC32MX - Port to PIC32MX, seems like there might be a "in on change" for each port. See ConfigIntCNA in ports.h
    //#define intOnChangeConfig(conf) ConfigIntCN(conf)
	#define intOnChangeConfig(conf)  ConfigIntCNA(conf)		//JUST FOR TESTING!!!! This only configured PORT A!!!!!!!!!!!!!!!
#else
    #error "intOnChangeConfig() not defined for this compiler!"
#endif


/**
 * @function void intOnChangeEnablePort(BYTE portID)
 *
 * Enables "Interrupt on Change" for the given IO Port (port of SBC).
 *
 * @preCondition Interrupts on change must have already been configured with
 * intOnChangeConfig() function.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 */
#if defined(__C30__)
    #define intOnChangeEnablePort(portID) portSetBitadr(portGetCNIE(portID))
#elif defined(__C32__)
    #define intOnChangeEnablePort(portID) portSetBitadr(portGetCNIE(portID))
#else
    #error "intOnChangeEnablePort() not defined for this compiler!"
#endif



/**
 * @function void intOnChangeDisablePort(BYTE portID)
 *
 * Disable "Interrupt on Change" for the given IO Port (port of SBC).
 *
 * @preCondition Interrupts on change must have already been configured with
 * intOnChangeConfig() function.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 */
#if defined(__C30__)
    #define intOnChangeDisablePort(portID) portClearBitadr(portGetCNIE(portID))
#elif defined(__C32__)
    #define intOnChangeDisablePort(portID) portClearBitadr(portGetCNIE(portID))
#else
    #error "intOnChangeDisablePort() not defined for this compiler!"
#endif


/**
 * @function void intOnChangeClearIF(void)
 *
 * Clear "Interrupt on Change" interrupt flag
 */
#if defined(__C30__)
    #define intOnChangeClearIF()  InputChange_Clear_Intr_Status_Bit
#elif defined(__C32__)
    #define intOnChangeClearIF()
#else
    #error "Processor or Compiler not supported!"
#endif


#endif  //NZ_INTERRUPT_H

