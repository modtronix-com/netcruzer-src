/**
 * @brief           Contains code for debouncing noisy ditital inputs, like buttons.
 * @file            nz_debounce.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section nz_debounce_desc Description
 *****************************************
 * Functions for debouncing noisy digital inputs, like buttons for example.
 * See @ref nz_debounce_usage for details how to use it.
 * This module is automatically enabled in the system as soon as any DEBOUNCE_GET_PORTn
 * defines are added to the projdefs.h file (as described in "Usage" section).
 * 
 * @subsection nz_debounce_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ------------ Debounce Configuration (from nz_debounce.h) -------------
// *********************************************************************
//Define functions for reading each port's value. ALWAYS define from 1 and up, with NO gaps!
//For example, if using 3 ports, use first 3 (1,2 and 3), AND NOT 1,2 and 4!
#define DEBOUNCE_GET_PORT1()
#define DEBOUNCE_GET_PORT2()
#define DEBOUNCE_GET_PORT3()
#define DEBOUNCE_GET_PORT4()
#define DEBOUNCE_GET_PORT5()
#define DEBOUNCE_GET_PORT6()
#define DEBOUNCE_GET_PORT7()
#define DEBOUNCE_GET_PORT8()

//Define debounce times. Must be multiples of 20ms
#define DEBOUNCE_RST            ( 40 )      //40ms debounce [-DEFAULT-]
#define DEBOUNCE_REPEAT         ( 280 )     //280ms first repeat [-DEFAULT-]
#define DEBOUNCE_REPEAT_TIMES   ( 2 )       //2 times = 2 x 280ms = 560ms [-DEFAULT-]
#define DEBOUNCE_REPEAT2        ( 160 )     //100ms following repeats [-DEFAULT-]
#define DEBOUNCE_REPEAT2_TIMES  ( 10 )      //10 times = 10 x 160ms = 1.6 Second [-DEFAULT-]
#define DEBOUNCE_REPEAT3        ( 80 )      //80ms [-DEFAULT-]
#define DEBOUNCE_REPEAT3_TIMES  ( 20 )      //20 times = 20 x 80ms = 1.6 Seconds [-DEFAULT-]
#define DEBOUNCE_REPEAT4        ( 40 )      //60ms [-DEFAULT-]
 @endcode
 *
 *
 * @subsection nz_debounce_usage Usage
 *****************************************
 * To use this module, the following must be done:
 * - Copy the @ref nz_debounce_conf "Configuration" section above (from nz_debounce.h)
 *   to the projdefs.h file.
 * - In this "Configuration" section, change any default values if required (if default
 *   values should be used, define is not required in projdefs.h).
 * - Complete the DEBOUNCE_GET_PORTn defines for all ports that should be debounced.
 * - Add nz_debounce.c to the MPLAB project.
 * - Include nz_debounce.h in the c file it is used in.
 * - <b>All DONE!</b> The functions defined in this file can now be used in the project.
 *
 *
 * @subsection nz_debounce_examples Examples
 ***************************************** 
 * An example how to use this module to debounce buttons connected to ports 4 and 7 (old port names X4 and X7).
 * First, the following must be added to the projdefs.h file:
@code
#define DEBOUNCE_GET_PORT1()    PIN_04
#define DEBOUNCE_GET_PORT2()    PIN_07
@endcode
 * After doing this, this module will automatically be enabled in the system (debounceService()
 * is called every 20ms). We can now use the debounceGetLatchedPort() function to get
 * the debounced port values. For example:
@code
    DIR_04 = INPUT_PIN; //Configure port 4 (old port name X4) as an input
    PULLDOWN_04 = TRUE;  //Enable pull-down resistor on port 4 (old port name X4)

    //Get debounced port 1
    if(debounceGetLatchedPort(1)) {
        //... Do something
    }
@endcode
 * If a button is attached to pin 4 (old port name X4), it should be tied to 3.3V via a resistor(10k for
 * example). And, the internal pull-down resistor of port 4 (X4) should be enabled.
 *
 **********************************************************************
 * @section nz_debounce_lic Software License Agreement
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
 * 2013-10-28, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_DEBOUNCE_H
#define NZ_DEBOUNCE_H


//Check for gaps
#if defined(DEBOUNCE_GET_PORT8) && (!defined(DEBOUNCE_GET_PORT1)||!defined(DEBOUNCE_GET_PORT2)||!defined(DEBOUNCE_GET_PORT3)||!defined(DEBOUNCE_GET_PORT4)||!defined(DEBOUNCE_GET_PORT5)||!defined(DEBOUNCE_GET_PORT6)||!defined(DEBOUNCE_GET_PORT7))
#error "DEBOUNCE_GET_PORT8 is defined, but not DEBOUNCE_GET_PORT1 to DEBOUNCE_GET_PORT7! ALWAYS define DEBOUNCE_GET_PORTs from 1 and up, with NO gaps!"
#endif
#if defined(DEBOUNCE_GET_PORT7) && (!defined(DEBOUNCE_GET_PORT1)||!defined(DEBOUNCE_GET_PORT2)||!defined(DEBOUNCE_GET_PORT3)||!defined(DEBOUNCE_GET_PORT4)||!defined(DEBOUNCE_GET_PORT5)||!defined(DEBOUNCE_GET_PORT6))
#error "DEBOUNCE_GET_PORT7 is defined, but not DEBOUNCE_GET_PORT1 to DEBOUNCE_GET_PORT6! ALWAYS define DEBOUNCE_GET_PORTs from 1 and up, with NO gaps!"
#endif
#if defined(DEBOUNCE_GET_PORT6) && (!defined(DEBOUNCE_GET_PORT1)||!defined(DEBOUNCE_GET_PORT2)||!defined(DEBOUNCE_GET_PORT3)||!defined(DEBOUNCE_GET_PORT4)||!defined(DEBOUNCE_GET_PORT5))
#error "DEBOUNCE_GET_PORT6 is defined, but not DEBOUNCE_GET_PORT1 to DEBOUNCE_GET_PORT5! ALWAYS define DEBOUNCE_GET_PORTs from 1 and up, with NO gaps!"
#endif
#if defined(DEBOUNCE_GET_PORT5) && (!defined(DEBOUNCE_GET_PORT1)||!defined(DEBOUNCE_GET_PORT2)||!defined(DEBOUNCE_GET_PORT3)||!defined(DEBOUNCE_GET_PORT4))
#error "DEBOUNCE_GET_PORT5 is defined, but not DEBOUNCE_GET_PORT1 to DEBOUNCE_GET_PORT4! ALWAYS define DEBOUNCE_GET_PORTs from 1 and up, with NO gaps!"
#endif
#if defined(DEBOUNCE_GET_PORT4) && (!defined(DEBOUNCE_GET_PORT1)||!defined(DEBOUNCE_GET_PORT2)||!defined(DEBOUNCE_GET_PORT3))
#error "DEBOUNCE_GET_PORT4 is defined, but not DEBOUNCE_GET_PORT1 to DEBOUNCE_GET_PORT3! ALWAYS define DEBOUNCE_GET_PORTs from 1 and up, with NO gaps!"
#endif
#if defined(DEBOUNCE_GET_PORT3) && (!defined(DEBOUNCE_GET_PORT1)||!defined(DEBOUNCE_GET_PORT2))
#error "DEBOUNCE_GET_PORT3 is defined, but not DEBOUNCE_GET_PORT1 to DEBOUNCE_GET_PORT2! ALWAYS define DEBOUNCE_GET_PORTs from 1 and up, with NO gaps!"
#endif
#if defined(DEBOUNCE_GET_PORT2) && (!defined(DEBOUNCE_GET_PORT1))
#error "DEBOUNCE_GET_PORT2 is defined, but not DEBOUNCE_GET_PORT1! ALWAYS define DEBOUNCE_GET_PORTs from 1 and up, with NO gaps!"
#endif

//Define how many deoubnce ports there are
#if defined(DEBOUNCE_GET_PORT8)
#define DEBOUNCE_PORTS  8
#elif defined(DEBOUNCE_GET_PORT7)
#define DEBOUNCE_PORTS  7
#elif defined(DEBOUNCE_GET_PORT6)
#define DEBOUNCE_PORTS  6
#elif defined(DEBOUNCE_GET_PORT5)
#define DEBOUNCE_PORTS  5
#elif defined(DEBOUNCE_GET_PORT4)
#define DEBOUNCE_PORTS  4
#elif defined(DEBOUNCE_GET_PORT3)
#define DEBOUNCE_PORTS  3
#elif defined(DEBOUNCE_GET_PORT2)
#define DEBOUNCE_PORTS  2
#elif defined(DEBOUNCE_GET_PORT1)
#define DEBOUNCE_PORTS  1
#else
#define DEBOUNCE_PORTS  0
#endif

#if (DEBOUNCE_PORTS > 0) && !defined(NZ_DEBOUNCE_ENABLE)
#define NZ_DEBOUNCE_ENABLE
#endif

#if defined(NZ_DEBOUNCE_ENABLE)

// Defines //////////////////////////////////////
#define DEBOUNCE_SERVICE_TIME   20  //Debounce serice time in MS


// Variables ////////////////////////////////////

/**
 * Initialization
 */
void debounceInit();


/**
 * Service function. Must be called every 20ms
 */
void debounceService();

/**
 * Gets the latched state of all ports. The value returned here has already been debounced.
 * The port latch is cleared after this call, so it is very important to check all ports.
 *
 * @return The ports state is contained in bits 0-1
 */
BYTE debounceGetLatchedPorts();


/**
 * Returns the latched state of the requested port. The value returned here has
 * been debounced. The port latch is cleared after this call!
 * It will only be set again after the port return to 0, and back to 1 again.
 *
 * @param port Is the port number. A value from 1 - 8
 *
 * @return 1 if port is active, else 0.
 */
BYTE debounceGetLatchedPort(BYTE port);

#endif  //#if defined(NZ_DEBOUNCE_ENABLE)

#endif
