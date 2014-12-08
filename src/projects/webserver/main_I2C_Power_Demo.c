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
#include "nz_serDataPorts.h"

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
static WORD vaEnable;           //V/A Enable register


/**
* Main application entry point.
*/
int main(void) {
    initMainApp();        //Initialize the main Webserver application

    tmrProcessIO = tick16Get() + tick16ConvertFromMS(600); //Startup delay of 600mS

    // Initialize "User Memory" WORD at 0x42 (are the two bytes are 0x42 and 0x43)
    USER_RAM_WORD(0x42) = 0xA0;

    //Remember current value of "User Memory" WORD at 0x42. Used to see if it is updated via Webpages
    vaEnable = USER_RAM_WORD(0x42);


    //Now that all items are initialized, begin the co-operative multitasking loop. This infinite loop will
    //continuously execute all stack-related tasks, as well as your own application's functions.  Custom
    //functions should be added at the end of this loop. Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot or part of it) and returns so that other tasks
    //can do their job. If a task needs very long time to do its job, it must be broken down into smaller pieces
    //so that other tasks can have CPU time.
    while (1) {
        mainAppTask();          //Main webserver task
        processIO();
    }
}


#define SM_PROCESS_WAIT_I2C 0x80   //Wait for I2C Transmission to DS2482 to complete
enum {
    SM_PROCESS_DELAY = 0,
    SM_PROCESS_READ_VOLTAGES,
    SM_PROCESS_READ_CURRENTS,
    SM_PROCESS_READ_TEMPERATURE,
} SM_PROCESS_IO;

/**
 * I/O functions
 */
void processIO(void) {
    static BYTE smProcess = SM_PROCESS_DELAY;
    static BYTE channel;        //Output channel V1 to V12
    static BOOL waitForI2C;
    static BYTE slaveAddr;
    static WORD timeout;
    BYTE statusI2C = 0;

    /////////////////////////////////////////////////
    //Check if "User Memory" WORD at 0x42 was updated via webpages
    if (vaEnable != USER_RAM_WORD(0x42)) {
        vaEnable = USER_RAM_WORD(0x42);     //Update with new value

        //Take any required action! For example, update register in I2C chip
        //....

        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUser RAM 0x42 updated to 0x");
        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, vaEnable);
    }

    /////////////////////////////////////////////////
    //Wait for I2C Transmission to complete, and I2C status to be available
    if (smProcess != SM_PROCESS_DELAY) {
        //Only continue once i2c 1 transmit buffer is empy. That is, all current messages in queue has been sent
        if (i2c1IsTxBufEmpty() != TRUE) {
            return;
        }

        if (waitForI2C) {
            //Check if a timeout occurred = 100ms
            if (tick16TestTmr(timeout + tick16ConvertFromMS(100))) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nI2C Timeout-");
                DEBUG_PUT_WORD(DEBUG_LEVEL_WARNING, tick16Get());
                DEBUG_PUT_CHAR(DEBUG_LEVEL_WARNING, '\n');
                i2c1EndTransmission();
                waitForI2C = FALSE;
                smProcess = SM_PROCESS_DELAY;
                return;
            }

            //If the transmission is not finished yet, return.
            if (i2c1IsBusy())
                return;

            //Get status for last I2C transmission
            statusI2C = i2c1GetStatus(0);

            //Status not available yet, I2C transmission still in progress (background via interrupt)
            if (statusI2C == I2C_TXION_STATUS_NOT_AVAILABLE) {
                //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'x');
                return;
            }

            if (statusI2C != 0) {
               DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nI2C 1 Transmit error to slave at ");
               DEBUG_PUT_BYTE(DEBUG_LEVEL_WARNING, slaveAddr);
            }
        }
    }

    //State machine
    switch(smProcess) {
        //Wait for 250ms time to expire.
        case SM_PROCESS_DELAY:
        {
            // TEST TEST
            break;
            
            if (tick16TestTmr(tmrProcessIO) == FALSE) {
                break;  //Waiting for timer to expire before continuing
            }

            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nProcess IO Begin");
            tick16UpdateTmrMS(tmrProcessIO, 1250);     //Update timer to expire in 250ms again
            channel = 0;                            //Initialize to read first channel V1
            waitForI2C = FALSE;
            smProcess = SM_PROCESS_READ_VOLTAGES;

            //For testing! Increment some User Memory variables. Will cause values on webpage to get updated!
            *((WORD*)(&nzGlobals.userRam[0x00])) += 1;
            *((WORD*)(&nzGlobals.userRam[0x02])) += 2;
            *((WORD*)(&nzGlobals.userRam[0x04])) += 3;

            //break;                                //No break, continue to read voltages below
        }
        //Read voltage of current channel
        case SM_PROCESS_READ_VOLTAGES:
        {
            //Wait for previous transmission to finish, is done in background interrupt
            if (waitForI2C) {
                waitForI2C = FALSE;      //Finished waiting for status
                //Just read last channel, next read currents
                if (++channel == 12) {
                    channel = 0;                            //Initialize to read first channel A1
                    smProcess = SM_PROCESS_READ_CURRENTS;
                    break;
                }
            }

            //Read 2 bytes for current channel "Voltage"
            //The read bytes are written to "User Ram" address 0x00 to 0x16 (0x00, 0x02, 0x04....)
            slaveAddr = 0x82 + (channel*2);
            i2c1BeginTransmission(slaveAddr);                   //Begin transmission to V1 to V12
            i2c1Write(0x02);                                    //Write register address for Voltage = 0x02
            i2c1ReadToArray(&nzGlobals.userRam[0x00+(channel*2)], 2); //Read 2 bytes from I2C slave, and save to given pointer
            i2c1EndTransmission();
            timeout = tick16Get();                                //Set Timeout
            waitForI2C = TRUE;                                  //Indicate that we are waiting for I2C to finish current transmission
            break;
        }
        //Read current of current channel
        case SM_PROCESS_READ_CURRENTS:
        {
            //Wait for previous transmission to finish, is done in background interrupt
            if (waitForI2C) {
                waitForI2C = FALSE;      //Finished waiting for status
                //Just read last channel, next read currents
                if (++channel == 12) {
                    smProcess = SM_PROCESS_READ_TEMPERATURE;
                    break;
                }
            }

            //Read 2 bytes for current channel "Current".
            //The read bytes are written to "User Ram" address 0x20 to 0x36 (0x20, 0x22, 0x24....)
            slaveAddr = 0x82+(channel*2);
            i2c1BeginTransmission(slaveAddr);                   //Begin transmission to V1 to V12
            i2c1Write(0x01);                                    //Write register address for Current = 0x01
            i2c1ReadToArray(&nzGlobals.userRam[0x20+(channel*2)], 2); //Read 2 bytes from I2C slave, and save to given pointer
            i2c1EndTransmission();
            timeout = tick16Get();                                //Set Timeout
            waitForI2C = TRUE;                                  //Indicate that we are waiting for I2C to finish current transmission
            break;
        }
        //Read temperature
        case SM_PROCESS_READ_TEMPERATURE:
        {
            //Wait for previous transmission to finish, is done in background interrupt
            if (waitForI2C) {
                waitForI2C = FALSE;      //Finished waiting for status
                //Return to Wait state!
                smProcess = SM_PROCESS_DELAY;
                break;
            }

            //Read 2 bytes for temperature. The Microchip MCP9801 I2C Temperature Monitor IC register
            //0x00 must be read.
            //The read bytes are written to "User Ram" address 0x40
            slaveAddr = 0x9E;
            i2c1BeginTransmission(slaveAddr);           //Begin transmission to
            i2c1Write(0x00);                            //Write register address = 0x00
            i2c1ReadToArray(&nzGlobals.userRam[0x40], 2);     //Read 2 bytes to given pointer
            i2c1EndTransmission();
            timeout = tick16Get();                        //Set Timeout
            waitForI2C = TRUE;                          //Indicate that we are waiting for I2C to finish current transmission
            break;
        }
    }
}
