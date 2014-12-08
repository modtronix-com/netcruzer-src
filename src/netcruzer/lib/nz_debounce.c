/**
 * @brief           Debounce Driver Functions
 * @file            nz_debounce.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 **********************************************************************
 * Software License Agreement
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
 * 2013-09-03, David H (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_DEBOUNCE_C

#include "HardwareProfile.h"
#include "nz_debounce.h"

#if defined(NZ_DEBOUNCE_ENABLE)

//Add debugging to this file. The DEBUG_CONF_DEBOUNCE macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_DEBOUNCE)
    #define DEBUG_CONF_DEBOUNCE       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_DEBOUNCE
#include "nz_debug.h"


#ifndef DEBOUNCE_RST
#define DEBOUNCE_RST 40
#endif

#ifndef DEBOUNCE_REPEAT
#define DEBOUNCE_REPEAT 280         //first repeat after 280ms
#endif

#ifndef DEBOUNCE_REPEAT_TIMES
#define DEBOUNCE_REPEAT_TIMES 2     //2 times = 2 x 280ms = 560ms
#endif

#ifndef DEBOUNCE_REPEAT2
#define DEBOUNCE_REPEAT2 160        //160ms following repeats
#endif

#ifndef DEBOUNCE_REPEAT2_TIMES
#define DEBOUNCE_REPEAT2_TIMES 10   //5 times = 10 x 160ms = 1.6 Second
#endif

#ifndef DEBOUNCE_REPEAT3
#define DEBOUNCE_REPEAT3 80         //100ms following repeats
#endif

#ifndef DEBOUNCE_REPEAT3_TIMES
#define DEBOUNCE_REPEAT3_TIMES 20   //20 times = 20 x 80ms = 1.6 Second
#endif

#ifndef DEBOUNCE_REPEAT4
#define DEBOUNCE_REPEAT4 40         //40ms following repeats
#endif


// Defines //////////////////////////////////////


// Local variables //////////////////////////////
static BYTE portsLatch;                     //Holds latched and debounced value of ports - cleared when read by getter function
static BYTE debounceArr[DEBOUNCE_PORTS];    //A register for each port
static BYTE btnRepeated;


// Function Prototypes //////////////////////////
BYTE portService(BYTE btn, BYTE state);


/**
 * Initialization code
 */
void debounceInit() {
    BYTE i;

    //Initialize all decounce counters
    for (i = 0; i < DEBOUNCE_PORTS; i++) {
        debounceArr[i] = DEBOUNCE_RST/DEBOUNCE_SERVICE_TIME;
    }

    portsLatch = 0;       //Clear all latched ports

    portsLatch = 0;       //All ports off

    btnRepeated = 0;
}


/**
 * Service function. Must be called every Must be called each DEBOUNCE_SERVICE_TIME
 */
void debounceService() {
    BYTE noPortsActive = 1;

    /////////////////////////////////////////////////
    //Service ports
    #if defined(DEBOUNCE_GET_PORT1)
    if (portService(0, DEBOUNCE_GET_PORT1()))
        noPortsActive = 0;   //A port is active
    #endif
    #if defined(DEBOUNCE_GET_PORT2)
    if (portService(1, DEBOUNCE_GET_PORT2()))
        noPortsActive = 0;   //A port is active
    #endif
    #if defined(DEBOUNCE_GET_PORT3)
    if (portService(2, DEBOUNCE_GET_PORT3()))
        noPortsActive = 0;   //A port is active
    #endif
    #if defined(DEBOUNCE_GET_PORT4)
    if (portService(3, DEBOUNCE_GET_PORT4()))
        noPortsActive = 0;   //A port is active
    #endif
    #if defined(DEBOUNCE_GET_PORT5)
    if (portService(4, DEBOUNCE_GET_PORT5()))
        noPortsActive = 0;   //A port is active
    #endif
    #if defined(DEBOUNCE_GET_PORT6)
    if (portService(5, DEBOUNCE_GET_PORT6()))
        noPortsActive = 0;   //A port is active
    #endif
    #if defined(DEBOUNCE_GET_PORT7)
    if (portService(6, DEBOUNCE_GET_PORT7()))
        noPortsActive = 0;   //A port is active
    #endif
    #if defined(DEBOUNCE_GET_PORT8)
    if (portService(7, DEBOUNCE_GET_PORT8()))
        noPortsActive = 0;   //A port is active
    #endif

    //If no ports currently active, reset repeated
    if (noPortsActive) {
        btnRepeated = 0;                    //Clear repeated flag
    }

}

/**
 * Gets the latched state of all ports. The value returned here has already been debounced.
 * The port latch is cleared after this call, so it is very important to process all
 * returned values! To get a single port's value, use the debounceGetLatchedPort()
 * function.
 *
 * @return The ports state is contained in bits 0-1
 */
BYTE debounceGetLatchedPorts() {
    BYTE ret;
    ret = portsLatch;
    portsLatch = 0;    //Clear all latched ports
    return ret;
}


/**
 * Returns the latched state of the requested port. The value returned here has
 * been debounced. The port latch is cleared after this call!
 * It will only be set again after the port return to 0, and back to 1 again.
 *
 * @param port Is the port number. A value from 1 - 8
 *
 * @return 1 if port is active, else 0.
 */
BYTE debounceGetLatchedPort(BYTE port) {
    BYTE ret;
    BYTE mask;

    port--;                 //Convert from 1-8 to 0-7

    //Check given port value
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_WARNING)
    if (port>7) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "debounceGetLatchedPort() invalid port");
        return;
    }
    #endif

    mask = 0x01 << port;    //Create mask for current port

    ret = ((portsLatch & mask)==0) ? 0 : 1;

    portsLatch = portsLatch & ~mask;    //Clear given port's latched value

    return ret;
}


/**
 * Does processing of each port. Includes latching and debouncing.
 * Must be called each DEBOUNCE_SERVICE_TIME
 *
 * @param port Is the port number. A value from 0 - 7
 *
 * @param state 1 = active, 0 = not active
 *
 * @return Returns state
 */
BYTE portService(BYTE port, BYTE state) {
    BYTE mask;

    mask = 0x01 << port;    //Create mask for current port

    //Current port is active
    if (state) {
        debounceArr[port]--;     //Decrement debounce count
    }
    //Current port is NOT active
    else {
        debounceArr[port] = DEBOUNCE_RST/DEBOUNCE_SERVICE_TIME;    //Reset debounce count
    }

    //Check if port has been active for debounce time yet
    if (debounceArr[port] == 0) {
        if (btnRepeated<255) btnRepeated++;
        
        portsLatch |= mask;

        //Set time until next port is detected.
        if (btnRepeated < DEBOUNCE_REPEAT_TIMES)
            debounceArr[port] = DEBOUNCE_REPEAT/DEBOUNCE_SERVICE_TIME;
        else if (btnRepeated < DEBOUNCE_REPEAT2_TIMES)
            debounceArr[port] = DEBOUNCE_REPEAT2/DEBOUNCE_SERVICE_TIME;
        else if (btnRepeated < DEBOUNCE_REPEAT3_TIMES)
            debounceArr[port] = DEBOUNCE_REPEAT3/DEBOUNCE_SERVICE_TIME;
        else
            debounceArr[port] = DEBOUNCE_REPEAT4/DEBOUNCE_SERVICE_TIME;
    }

    return state;
}

#endif  //#if defined(NZ_DEBOUNCE_ENABLE)

