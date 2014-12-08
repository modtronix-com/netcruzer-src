/*********************************************************************
 * main.c         - Netcruzer Webserver Main File
 * Dependencies:
 * Complier:        MPLAB XC16
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Main file for Netcruzer Webserver Project.
 *
 *********************************************************************
 * File History
 *
 * 2010-11-10, David H. (DH):
 *    - Initial version, copied from Microchip MAL web server
 *
 *********************************************************************
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
 *
 ********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

//Netcruzer include files
#include "HardwareProfile.h"
#include "main.h"
#include "mainApp.h"

//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"

// Function Prototypes //////////////////////////
void processIO(void);

// Variables ////////////////////////////////////
static WORD tmrProcessIO = 0;   //Timer for Process IO rate


/**
* Main application entry point.
*/
int main(void) {
    initMainApp();        //Initialize the main Webserver application

    tmrProcessIO = tick16Get() + tick16ConvertFromMS(600); //Startup delay of 600mS

    //Now that all items are initialized, begin the co-operative multitasking loop. This infinite loop will
    //continuously execute all stack-related tasks, as well as your own application's functions.  Custom
    //functions should be added at the end of this loop. Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot or part of it) and returns so that other tasks
    //can do their job. If a task needs very long time to do its job, it must be broken down into smaller pieces
    //so that other tasks can have CPU time.
    while (1) {
        mainAppTask();    //Main webserver task
        processIO();
    }
}


/**
 * I/O functions
 */
void processIO(void) {

    if (tick16TestTmr(tmrProcessIO) == FALSE) {
        tick16UpdateTmrMS(tmrProcessIO, 250);            //Update timer to expire in 250ms again
        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nPI");  //Output debug message each cycle

    }
}