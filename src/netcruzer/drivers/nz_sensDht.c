/**
 * @brief           Functions for DHT22 Temperature and Humidity Sensor
 * @file            nz_sensDht.c
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
 * 2014-01-02, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_DHT22_C

#include "HardwareProfile.h"

//Disable all code if SENS_DHT_NUMBER_OF_SENSORS = 0
#if (SENS_DHT_NUMBER_OF_SENSORS > 0)

#if !defined(SENS_DHT_ENABLE)
    #define SENS_DHT_ENABLE
#endif

#include "nz_sensDht.h"
#include "nz_interrupt.h"

//Add debugging to this file. The DEBUG_CONF_DHT22 macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_DHT22)
    #define DEBUG_CONF_DHT22       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_DHT22
#include "nz_debug.h"

////////// Default Defines //////////////////////
#if !defined(SENS_DHT_TIMEOUT)
#define SENS_DHT_TIMEOUT                        ( 100 )     // 100ms timeout
#endif

#if !defined(SENS_DHT_STARTUP_READ_DELAY)
#define SENS_DHT_STARTUP_READ_DELAY             ( 5000 )    // 5 Seconds
#endif

#if !defined(SENS_DHT_AUTO_READ_PERIOD)
#define SENS_DHT_AUTO_READ_PERIOD                ( 5000 )    // Automatically read every 5 seconds (5000ms)
#endif

#if !((SENS_DHT_AUTO_READ_PERIOD == 0) || (SENS_DHT_AUTO_READ_PERIOD > 2099))
#error "SENS_DHT_AUTO_READ_PERIOD must be larger than or equal to 2100 (2.1 seconds)"
#endif


enum SM_SENSDHT_ {
    SM_SENSDHT_IDLE = 0,
    SM_SENSDHT_START,
    SM_SENSDHT_START_FINISHED,
    SM_SENSDHT_WAIT_RESPONCE,
    SM_SENSDHT_WAIT_PREPARE,
    SM_SENSDHT_READ_DATA,
    SM_SENSDHT_READ_FINISHED,
} SM_SENSDHT;


////////// Function Prototypes //////////////////


////////// Variables ////////////////////////////
//static DHT_SENSOR* pSens;    //Current sensor
DHT_SENSOR*      pSens;        //Current sensor
SENS_DHT_INFO dhtInfo;

////////// Defines //////////////////////////////
#define DHTSENS_TASK_WAIT_FOR_DELAY   0x80   //Wait for I2C Transmission to DS2482 to complete
#define DHTSENS_TASK_CHECK_TIMEOUT    0x40   //Wait for I2C Transmission to DS2482 to complete
#define DHTSENS_TASK_SM_MASK          0x3F    //Mask out all bits above


/**
 * DHT Sensor Initialization. This function also sets the IO Port (port of SBC) for the
 * first DHT Sensor. If there is more than 1 sensor (SENS_DHT_NUMBER_OF_SENSORS > 1),
 * the sensDht_setPort() function has to be called for each one to set it's IO Port
 * (port of SBC)!
 *
 * The positive supply for all DHT sensors can by supplied via a port. If the
 * "portPowerSwitch" parameter is not IOPORT_ID_NA, than it gives the IO
 * Port used to supply power to the DHT sensors.
 * If "powerSwitchInverted" is FALSE, this port is set to 1 to power DHT sensors.
 * If "powerSwitchInverted" is TRUE, this port is set to 0 to power DHT sensors.
 *
 * @param portIdSens The "Port ID" connected to the sensor pin of the DHT22. Is a
 *             number from 0 to 41, also known as the "Netcruzer Port Name".
 *
 * @param startupDelay Time to wait before first read. When a system has multiple DHT22 sensors,
 *            ensure each one has a different startup delay! Use 0 to use default value.
 *
 * @param portPowerSwitch The "Port ID" connected to the sensor pin of the DHT22.
 * Is a number from 0 to 41, also known as the "Netcruzer Port Name".
 * Use IOPORT_ID_NA is there is no power switch.
 *
 * @param powerSwitchInverted Indicates if the power switch is inverted (port set to
 * 0 to power sensors).
 *
 * @return The status of the Initialization, 0=Success, else error code
 */
BYTE sensDht_init(BYTE portIdSens, WORD startupDelay, BYTE portPowerSwitch, BOOL powerSwitchInverted) {
    //Initialize with 0
    memset(&dhtInfo, 0, sizeof(SENS_DHT_INFO));

    if (portConfig(portIdSens, PTYPE_DIN | PCFG_DIN_PULLUP) != 0) {
        goto DHT22_INIT_ERROR;
    }

    dhtInfo.bitadrPwrSwt = portGetLAT(portPowerSwitch);
    dhtInfo.flags.pwrSwtInverted = powerSwitchInverted;

    //Enable power switch
    if (dhtInfo.bitadrPwrSwt != BITADR_NA) {
        //Enable power
        if (powerSwitchInverted==TRUE)
            portWriteBitadr(dhtInfo.bitadrPwrSwt, 0);
        else
            portWriteBitadr(dhtInfo.bitadrPwrSwt, 1);

        //Configure port as output
        portConfig(portPowerSwitch, PTYPE_DOUT);
        //if (portConfig(portIdSens, PTYPE_DOUT) != 0) {
        //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: sensDht_addSensor() DOUT");
        //}
    }

    dhtInfo.sens[0].portID = portIdSens;            //Save sensor pin's IO Port ID (ports of SBC)
    dhtInfo.sens[0].flags.initialized = 1;            //Mark as initialized
    /* Not required, we check if DHT_SENSOR.flags.initialized bit is set to see if sensor has been initialized
    #if (SENS_DHT_NUMBER_OF_SENSORS > 1)
    for (i=1; i<SENS_DHT_NUMBER_OF_SENSORS; i++) {
        dhtInfo.sens[i].portID = IOPORT_ID_NA;
    }
    #endif
    */

    //Wait SENS_DHT_STARTUP_READ_DELAY before first read
    if (startupDelay==0)
        dhtInfo.tmrDelay = tick16Get() + tick16ConvertFromMS(SENS_DHT_STARTUP_READ_DELAY);
    else
        dhtInfo.tmrDelay = tick16Get() + tick16ConvertFromMS(startupDelay);
    dhtInfo.sm = SM_SENSDHT_START | DHTSENS_TASK_WAIT_FOR_DELAY;

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDHT22 Init Success");
    return 0;   //Return Success

    DHT22_INIT_ERROR:
    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nDHT22 Init Error");
    return 1;   //Return ERROR
}

/**
 * DHT22 Fiber Task. Is called from the pin interrupt connected to the sensor
 * input.
 * 
 */
void sensDht_fbrTask() {
    static WORD usTime;
    WORD newTick;
    BYTE pinState;
    WORD oldTick;

     //While there is data to read
    while (dhtInfo.pinGet != dhtInfo.pinPut) {
        newTick = dhtInfo.pinArr[dhtInfo.pinGet];   //Save new tick. MSB contains pin value!
        #if (SENS_DHT_PIN_ARRAY_SIZE==256)
            dhtInfo.pinGet++;
        #else
            dhtInfo.pinGet = (dhtInfo.pinGet + 1) & (SENS_DHT_PIN_ARRAY_SIZE-1);
        #endif
        pinState = (newTick & 0x8000) ? 1 : 0;  //Is pin set or cleared
        oldTick = dhtInfo.tickLastPinChange;
        newTick &= 0x7fff;                      //Clear MSB = pin state
        dhtInfo.tickLastPinChange = newTick;

        //Ignore all high pin states, EXCEPT when during SM_SENSDHT_READ_FINISHED state!
        //In SM_SENSDHT_READ_FINISHED state, we are waiting for bus to go high!
        if ((pinState==1) && ((dhtInfo.sm & DHTSENS_TASK_SM_MASK) != SM_SENSDHT_READ_FINISHED)) {
            continue;
        }

        usTime = (newTick - oldTick) & 0x7fff;  //Mask out MSB = pin state
        usTime *= 8;    //Get uS

//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '(');
//        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, newTick);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '-');
//        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, oldTick);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '=');
//        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, usTime);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'p');
//        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, dhtInfo.pinPut);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'g');
//        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, dhtInfo.pinGet);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ')');


        //If main task is waiting for delay, we do not process any data. Return!
        if (dhtInfo.sm & DHTSENS_TASK_WAIT_FOR_DELAY) {
            return;
        }

        /////////////////////////////////////////////////
        //State machine
        switch(dhtInfo.sm & DHTSENS_TASK_SM_MASK) {
            //Response has been received. This is first low pulse after we sent "Start Signal" low pulse
            case SM_SENSDHT_WAIT_RESPONCE:
                //Response received = 0V
                //if (pinState == 0) {  //Not needed, always 0! Above we check if pinState=1, we "continue"
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDHT22 Response, T=");
                    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, usTime);

                    //Wait for "Prepare" signal from sensor, and check for Timeout (dhtInfo.timeout is already set)
                    dhtInfo.sm = SM_SENSDHT_WAIT_PREPARE | DHTSENS_TASK_CHECK_TIMEOUT;
                //}
                //else {
                //    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDHT22 NO Response");
                //}
                break;
            //Low pulse of first bit received. From now on the duration of the high part of each pulse indicates "0" or "1"
            case SM_SENSDHT_WAIT_PREPARE:
                //Response received = 0V
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDHT22 Prepared, T=");
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, usTime);
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDHT22 Data, T=");

                //Read data from sensor, and check for Timeout (dhtInfo.timeout is already set)
                dhtInfo.sm = SM_SENSDHT_READ_DATA | DHTSENS_TASK_CHECK_TIMEOUT;
                break;
            case SM_SENSDHT_READ_DATA:
                //Response received = 0V
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, usTime);
                //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '(');
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, oldTick);
                //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '-');
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, newTick);
                //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ')');

                if (usTime < 10) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nDHT22 Read Error!");
                    goto DHT22_FBR_TASK_ERROR;
                }

                if (usTime > 256) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n>256(");
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, newTick);
                    DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '-');
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, oldTick);
                    DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '=');
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, usTime);
                    DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'p');
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, dhtInfo.pinPut);
                    DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'g');
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, dhtInfo.pinGet);
                    DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ')');

                    goto DHT22_FBR_TASK_ERROR;
                }

                //Shift 1 position left, and set LSB if we read a '1'
                dhtInfo.data[dhtInfo.cnt/8] <<= 1;
                if (usTime > 50) {        //Read a 1
                    dhtInfo.data[dhtInfo.cnt/8] |= 0x01;
                }
                dhtInfo.cnt++;

                //Did we just read the last value?
                if (dhtInfo.cnt == 40) {
                    //Use pinState & usTime as temp registers!
                    usTime = 0;
                    for (pinState=0; pinState<4; pinState++) {
                        usTime += dhtInfo.data[pinState];
                    }
                    //Check for CRC Error
                    if (dhtInfo.data[4] != (usTime&0xff)) {
                        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nDHT22 CRC Error!!!!!!!!!!!!!!");
                        goto DHT22_FBR_TASK_ERROR;
                    }
                    //debugPutString("\nRHT22: ");
                    //debugPutHexByte(dhtInfo.data[0]);    debugPutChar(' ');
                    //debugPutHexByte(dhtInfo.data[1]);    debugPutChar(' ');
                    //debugPutHexByte(dhtInfo.data[2]);    debugPutChar(' ');
                    //debugPutHexByte(dhtInfo.data[3]);    debugPutChar(' ');
                    //debugPutHexByte(dhtInfo.data[4]);

                    //Save data to dhtInfo.sens[]
                    #if (SENS_DHT_TYPE == 2)        //DHT22
                        //Swap byts 1&2 and 2&3
                        *((WORD*)&(pSens->data[0])) = WORD_FROM_BYTES(dhtInfo.data[0], dhtInfo.data[1]);
                        *((WORD*)&(pSens->data[2])) = WORD_FROM_BYTES(dhtInfo.data[2], dhtInfo.data[3]);
                    #elif (SENS_DHT_TYPE == 1)    //DHT11
                        //No swapping of data required for the DHT11
                        *((WORD*)&(pSens->data[0])) = WORD_FROM_BYTES(dhtInfo.data[1], dhtInfo.data[0]);
                        *((WORD*)&(pSens->data[2])) = WORD_FROM_BYTES(dhtInfo.data[3], dhtInfo.data[2]);
                    #endif

                    //Sensor still has to pull bus high. Also check for Timeout (dhtInfo.timeout is already set)
                    dhtInfo.sm = SM_SENSDHT_READ_FINISHED | DHTSENS_TASK_CHECK_TIMEOUT;
                }
                break;
            //Sensor sets bus high after last (40th) bit
            case SM_SENSDHT_READ_FINISHED:
            {
                WORD bitadr;

                pSens->flags.error = FALSE;            //Success!
                pSens->flags.newHumidity = TRUE;
                pSens->flags.newTemperature = TRUE;

                //Set port to 1, and configure as output. Makes it more immune to noise!
                portWrite(pSens->portID, 1);               //Set port to 1
                bitadr = portGetTRIS(pSens->portID);    //Get bitadr for TRIS
                NZ_INT_DISABLE_FIBERS();                      //Disable "RTOS Kernel" and "RTOS Fibers"
                portWriteBitadr_MACRO(bitadr, 0);             //Configure port as output. Will set output to 1
                NZ_INT_ENABLE_FIBERS();                       //Enable "RTOS Kernel" and "RTOS Fibers"

                //Reset to idle state!
                dhtInfo.sm = SM_SENSDHT_IDLE;
                break;
            }
        }
    }
    return;

    //ERROR! Set error flag, delay 1200ms
DHT22_FBR_TASK_ERROR:
    //Ensure pin is input (high impedance)
    portConfigDir(pSens->portID, 1);   //Configure port as input
    dhtInfo.tmrDelay = tick16Get() + tick16ConvertFromMS(1200);
    dhtInfo.sm = SM_SENSDHT_IDLE | DHTSENS_TASK_WAIT_FOR_DELAY;
    pSens->flags.error = 1;   //Error
    return;
}


/**
 * Main DHT Task, called in task context (not ISR or Thread context).
 */
void sensDht_task() {
    WORD bitadr;

    /////////////////////////////////////////////////
    //Waiting for delay to expire
    if (dhtInfo.sm & DHTSENS_TASK_WAIT_FOR_DELAY) {
        //Return if delay not finished
        if (tick16TestTmr(dhtInfo.tmrDelay) == FALSE) {
            return;
        }
        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDelay Done!");
        //Clear wait bit
        dhtInfo.sm = dhtInfo.sm & ~DHTSENS_TASK_WAIT_FOR_DELAY;
    }

    /////////////////////////////////////////////////
    //Check for timeout
    if (dhtInfo.sm & DHTSENS_TASK_CHECK_TIMEOUT) {
        //Timeout occurred!
        if (tick16TestTmr(dhtInfo.timeout + tick16ConvertFromMS(SENS_DHT_TIMEOUT))) {
            //Timeout! Reset!
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT22 Timeout!");
            goto DHTSENS_TASK_ERROR;
        }
    }

    //Only handle SM_SENSDHT_IDLE to SM_SENSDHT_START_FINISHED states, sensDht_fbrTask() handles the rest!
    //Wait for any previous messages to be processed.
    if ((dhtInfo.sm & DHTSENS_TASK_SM_MASK) > SM_SENSDHT_START_FINISHED)
        return;

    /////////////////////////////////////////////////
    //State machine
    switch(dhtInfo.sm & DHTSENS_TASK_SM_MASK) {
        //Idle
        case SM_SENSDHT_IDLE:
            //If the last sensor was just read, only continue if "auto reading" is enabled
            if (dhtInfo.currSens == (SENS_DHT_NUMBER_OF_SENSORS-1)) {
                //If auto reading enabled
                if (SENS_DHT_AUTO_READ_PERIOD != 0) {
                    dhtInfo.tmrDelay = tick16Get() + tick16ConvertFromMS(SENS_DHT_AUTO_READ_PERIOD);
                    dhtInfo.sm = SM_SENSDHT_START | DHTSENS_TASK_WAIT_FOR_DELAY;
                }
            }
            //If this is NOT the last sensor, read next sensor with small 2ms delay
            else {
                dhtInfo.tmrDelay = tick16Get() + 2;
                dhtInfo.sm = SM_SENSDHT_START | DHTSENS_TASK_WAIT_FOR_DELAY;
            }
            break;
        //Send start pulse
        case SM_SENSDHT_START:
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDHT22 Start");
            if (++dhtInfo.currSens == SENS_DHT_NUMBER_OF_SENSORS) {
                dhtInfo.currSens = 0;
            }
            pSens = &dhtInfo.sens[dhtInfo.currSens];                //Get pointer to current sensor
            if (pSens->flags.initialized == 0) {
                //Invalid IO Port ID (port of SBC)!
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT22 Sensor ");
                DEBUG_PUT_WORD(DEBUG_LEVEL_WARNING, dhtInfo.currSens);
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, " not initialized!");
                goto DHTSENS_TASK_ERROR;
            }
            //Check IO Port ID (port of SBC) is valid
            pSens->flagsCycle.clearEachCycle = 0;                    //Clear all "cycle" flags
            dhtInfo.bitadrPin = portGetPIN(pSens->portID);    //Save sensor pinadr for current sensor
            #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
            if (dhtInfo.bitadrPin == BITADR_NA) {
                //Invalid IO Port ID (port of SBC)!
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT22 IO Port ID Invalid!");
                goto DHTSENS_TASK_ERROR;
            }
            #endif
            #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
            if (dhtInfo.pinGet != dhtInfo.pinPut) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nPUT != GET");
            }
            #endif
            //Reset PUT and GET pointer! Will only get here once all previous messages have been processed! So GET should be = PUT!
            dhtInfo.pinGet = dhtInfo.pinPut = 0;
            //Reset dhtInfo.data, is used to store 40 bit we read from sensor
            dhtInfo.cnt = 0;
            memset(dhtInfo.data, 0, sizeof(dhtInfo.data));
            //Set Timeout
            dhtInfo.timeout = tick16Get();
            //Send low part of Start Pulse. Must be min 1ms for DHT22, and 18ms for DHT11
            portWrite(pSens->portID, 0);                //Set port to 0
            bitadr = portGetTRIS(pSens->portID);    //Get bitadr for TRIS
            //Configuring port as output, will change it's state from 1 to 0. This will trigger
            //an interrupt, which will start the DHT22 Fiber, meaning the following line will only
            //be executed on return from the fiber. They set pObj variables that could be needed
            //in the Fiber! To prevent this, disable Fibers.
            NZ_INT_DISABLE_FIBERS();                //Disable "RTOS Kernel" and "RTOS Fibers"
            portWriteBitadr_MACRO(bitadr, 0);        //Configure port as output. Will pull output low, and trigger ISR!
            //Replaced by portGetTRIS() and portWriteBitadr_MACRO().
            //portConfigDir(pSens->portID, 0);

            //Wait 20ms. DON'T use tick16UpdateTmrMS!! Will cause wrong(too short) delay if interrupt occurred in code above!
            //between tick16TestTmr() and sending start pulse!
            dhtInfo.tmrDelay =  tick16Get() + tick16ConvertFromMS(20);

            //Set next state, Delay given time, and check for Timeout (dhtInfo.timeout is already set)
            dhtInfo.sm = SM_SENSDHT_START_FINISHED | DHTSENS_TASK_WAIT_FOR_DELAY | DHTSENS_TASK_CHECK_TIMEOUT;
            NZ_INT_ENABLE_FIBERS();    //Enable "RTOS Kernel" and "RTOS Fibers"
            break;
        //Finished sending start pulse
        case SM_SENSDHT_START_FINISHED:
            //Reset DHT pin to high impedance! The DHT will now send a response in about 50uS
            bitadr = portGetTRIS(pSens->portID);    //Get bitadr for TRIS

            //Prevent DHT22 Fiber from executing after pin is set with portWriteBitadr_MACRO().
            NZ_INT_DISABLE_FIBERS();                //Disable "RTOS Kernel" and "RTOS Fibers"
            portWriteBitadr_MACRO(bitadr, 1);        //Configure port as input so we can read sensor!
            //Replaced by portGetTRIS() and portWriteBitadr_MACRO().
            //portConfigDir(pSens->portID, 1);

            //Set next state, and check for Timeout (dhtInfo.timeout is already set)
            dhtInfo.sm = SM_SENSDHT_WAIT_RESPONCE | DHTSENS_TASK_CHECK_TIMEOUT;
            NZ_INT_ENABLE_FIBERS();   //Enable "RTOS Kernel" and "RTOS Fibers"
            break;
    }
    return;

    //ERROR! Set error flag, delay 1200ms
DHTSENS_TASK_ERROR:
    NZ_INT_DISABLE_FIBERS();                //Disable "RTOS Kernel" and "RTOS Fibers"
    //Ensure pin is input (high impedance)

    portConfigDir(pSens->portID, 1);    //Configure port as input

    dhtInfo.tmrDelay = tick16Get() + tick16ConvertFromMS(1200);
    dhtInfo.sm = SM_SENSDHT_IDLE | DHTSENS_TASK_WAIT_FOR_DELAY;;
    pSens->flags.error = 1;   //Error
    NZ_INT_ENABLE_FIBERS();   //Enable "RTOS Kernel" and "RTOS Fibers"
    return;
}


/**
 * Set the IO Port(port of SBC) for the DHT Sensor at the given index. This function only has to
 * be called when there are more than 1 DHT sensors. This is the case when
 * SENS_DHT_NUMBER_OF_SENSORS is larger than 1. When there is only 1 sensor, it's
 * IO Port(port of SBC) is set with the sensDht_init() function.
 *
 * @param portIdSens The "Port ID" connected to the sensor pin of the DHT22. Is a
 *             number from 0 to 41, also known as the "Netcruzer Port Name".
 * @param idxSensor The index of the target sensor, is a value from 0 to n
 */
void sensDht_addSensor(BYTE idxSensor, BYTE portIdSens) {
    if (idxSensor < SENS_DHT_NUMBER_OF_SENSORS) {
        dhtInfo.sens[idxSensor].portID = portIdSens;        //Save sensor pin's IO Port ID (port of SBC)
        dhtInfo.sens[idxSensor].flags.initialized = 1;        //Mark as initialized
    }
}


/**
 * Start reading the DHT22 temperature and humidity in the background.
 */
void sensDht_startRead() {
    //Ensure currently in idle state
    if ((dhtInfo.sm & DHTSENS_TASK_SM_MASK) == SM_SENSDHT_IDLE) {
        dhtInfo.tmrDelay = tick16Get() + tick16ConvertFromMS(SENS_DHT_AUTO_READ_PERIOD);
        dhtInfo.sm = SM_SENSDHT_START | DHTSENS_TASK_WAIT_FOR_DELAY;
    }
}


/**
 * Indicates if the DHT22 is currently idle. This is the case if all sensors
 * have been read, and NOT configured for Auto reading (SENS_DHT_AUTO_READ_PERIOD = 0).
 * If this function returns true, the sensDht_startRead() function can be used
 * to start reading all sensors again.
 *
 * @return 1 DHT22 is currently idle,
 *         0 DHT22 is busy
 */
BOOL sensDht_isIdle() {
    //If:
    // - Currently in IDLE state
    // - All sensors have been read (dhtInfo.currSens = last sensor)
    return ( ((dhtInfo.sm & DHTSENS_TASK_SM_MASK) == SM_SENSDHT_IDLE)
          && (dhtInfo.currSens == (SENS_DHT_NUMBER_OF_SENSORS-1)) );
}

#endif  //#if (SENS_DHT_NUMBER_OF_SENSORS > 0)