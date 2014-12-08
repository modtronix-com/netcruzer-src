/**
 * @brief           Functions for DHT22 Temperature and Humidity Sensor.
 * @file            nz_sensDhtBlock.c
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
#define THIS_IS_NZ_DHT22_BLOCK_C

#include "HardwareProfile.h"
#include "nz_sensDhtBlock.h"
#include "nz_interrupt.h"

//Add debugging to this file. The DEBUG_CONF_DHT22 macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_DHT22)
    #define DEBUG_CONF_DHT22       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_DHT22
#include "nz_debug.h"

////////// Default Defines //////////////////////

#if !defined(SENS_DHT_SUPPORT_DHT22)
#define SENS_DHT_SUPPORT_DHT22                  ( 1 )
#endif

#if !defined(SENS_DHT_SUPPORT_DHT11)
#define SENS_DHT_SUPPORT_DHT11                  ( 0 )
#endif

#if !defined(SENS_DHT_STARTUP_READ_DELAY)
#define SENS_DHT_STARTUP_READ_DELAY             ( 5000 )    // 5 Seconds
#endif

#if !defined(SENS_DHT_RD_DLY)
#define SENS_DHT_RD_DLY                         ( 35 )      //Delay 35uS before read
#endif

#if !defined(SENS_DHT_POWER_RST_TIME)
#define SENS_DHT_POWER_RST_TIME                 ( 2000 )    //Power down reset time in mS
#endif

#if !defined(SENS_DHT_RESET_READ_DELAY)
#define SENS_DHT_RESET_READ_DELAY               ( 2000 )    //Delay after reset till read allowed
#endif


////////// Function Prototypes //////////////////


////////// Variables ////////////////////////////
DHT_SENSOR    sens;


enum SM_SENSDHT_ {
    SM_SENSDHT_IDLE = 0,
	SM_SENSDHT_POWERUP_RESET,
	SM_SENSDHT_RESET_DELAY
} SM_SENSDHT;


/**
 * Inititializes the DHT Sensor module.
 *
 * The positive supply for all DHT sensors can by supplied via a port. If the
 * "portPowerSwitch" parameter is not IOPORT_ID_NA, than it gives the IO
 * Port used to supply power to the DHT sensors.
 * If "powerSwitchInverted" is FALSE, this port is set to 1 to power DHT sensors.
 * If "powerSwitchInverted" is TRUE, this port is set to 0 to power DHT sensors.
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
 * @return Returns 0 if success, else error code.
 */
BYTE sensDht_init(WORD startupDelay, BYTE portPowerSwitch, BOOL powerSwitchInverted) {

    sens.flags.pwrSwtInverted = powerSwitchInverted;
    sens.bitadrPwrSwt = portGetLAT(portPowerSwitch);

    //Enable power switch
    if (sens.bitadrPwrSwt != BITADR_NA) {
        //Enable power
        if (powerSwitchInverted==TRUE)
            portWriteBitadr(sens.bitadrPwrSwt, 0);
        else
            portWriteBitadr(sens.bitadrPwrSwt, 1);

        //Configure port as output
        portConfig(portPowerSwitch, PTYPE_DOUT);
        //if (portConfig(portIdSens, PTYPE_DOUT) != 0) {
        //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: sensDht_addSensor() DOUT");
        //}
    }

    //Start reset delay - delay required before first read
    sens.sm = SM_SENSDHT_RESET_DELAY;

    if (startupDelay==0)
        startupDelay = SENS_DHT_STARTUP_READ_DELAY;
    sens.tmrDelay = tick16Get() + tick16ConvertFromMS(startupDelay);

    return 0;
}

/**
 * Configures IO Port (port of SBC) for the DHT Sensor as an input, with pull-up
 * resistor enabled.
 *
 * @param portIdSens The "Port ID" connected to the sensor pin of the DHT22. Is a
 *             number from 0 to 41, also known as the "Netcruzer Port Name".
 */
void sensDht_addSensor(BYTE portIdSens) {
    //Configure port as input, with pull-up resistors enabled
    if (portConfig(portIdSens, PTYPE_DIN | PCFG_DIN_PULLUP) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: sensDht_addSensor() DIN");
    }

    //Start reset delay - delay required before first read
    sens.sm = SM_SENSDHT_RESET_DELAY;
    sens.tmrDelay = tick16Get() + tick16ConvertFromMS(SENS_DHT_STARTUP_READ_DELAY);
}

/**
 * Wait for pin to go high or low
 *
 * @param value 0 to wait for 0, 1 to wait for 1
 * @param portIdSens Port to check
 * @param t Time in us to wait
 *
 * @return 0 = Error, 1 if OK
 */
WORD sensDht_waitForPin(BOOL state, WORD bitadrPin, WORD t) {
    WORD utickStart = utick16Get();

    while(portReadBitadr(bitadrPin) == (state==0)?1:0)  {
        if (utick16GetElapsedUS(utickStart) > t) {
            return 0;
        }
    }

    return 1;
}


/**
 * Checks if the DHT sensor module is ready.
 *
 * @return Returns 1 if Ready, else 0.
 */
BOOL sensDht_isReady() {
    return (sens.sm == SM_SENSDHT_IDLE);
}


/**
 * Main DHT Task, called in task context (not ISR or Thread context).
 * Does NOT have to be called too often, once every couple of 100ms will
 * be fine. But, constant calls are also OK.
 */
void sensDht_task() {
    //State machine
    switch(sens.sm) {
        //Idle
        case SM_SENSDHT_IDLE:
			break;
        //Busy with power down - power up reset
        case SM_SENSDHT_POWERUP_RESET:

            //Return if delay not finished
            if (tick16TestTmr(sens.tmrDelay) == FALSE) {
                return;
            }

            //Turn power switch back on
            portWriteBitadr(sens.bitadrPwrSwt, (sens.flags.pwrSwtInverted==TRUE)?0:1);
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDHT power reset");

            sens.sm = SM_SENSDHT_RESET_DELAY;
            sens.tmrDelay = tick16Get() + tick16ConvertFromMS(SENS_DHT_RESET_READ_DELAY);

			//Set next state, Delay given time, and check for Timeout (sens.timeout is already set)
            sens.sm = SM_SENSDHT_RESET_DELAY;
            break;
        //Delay required after power reset
        case SM_SENSDHT_RESET_DELAY:

            //Return if delay not finished
            if (tick16TestTmr(sens.tmrDelay) == FALSE) {
                return;
            }

            //Set next state, Delay given time, and check for Timeout (sens.timeout is already set)
            sens.sm = SM_SENSDHT_IDLE;
            break;
    }
    return;
}


/**
 * Reads the temperature and humidity for a DHT11 or DHT22 sensor. After calling this
 * function, the temperature and humidity values can be retrieved with the sensDht_getXxx
 * functions.
 *
 * @param type Type of sensor, is a SENS_DHT_TYPE_XXX define, like SENS_DHT_TYPE_DHT22
 * for example.
 *
 * @param portIdSens The "Port ID" connected to the sensor pin of the DHT22. Is a
 *             number from 0 to 41, also known as the "Netcruzer Port Name".
 *
 * @return Returns 0 if success, else a SENS_DHT_ERROR(enum) error code.
 *      - 0 = Success
 */
BYTE sensDht_read(BYTE type, BYTE portIdSens) {
    BYTE i;
    WORD w;
    WORD bitadrSens;
    BYTE err;
    WORD savedIpl;

    //Only continue if idle
    if (sens.sm != SM_SENSDHT_IDLE)
        return SENS_DHT_ERR_BUSY;

    if ((bitadrSens = portGetPIN(portIdSens)) == BITADR_NA) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nsensDht_read() - invalid port!");
        return SENS_DHT_ERR_INVALID_PORT;
    }

    //Confirm port is high.
    if (portReadBitadr(bitadrSens) == 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nsensDht_read() - port is low!");
        return SENS_DHT_ERR_PORT_LOW;
    }

    sens.flags.type = type;

    //Send start signal = 0v. Must be minimum 1ms for the DHT22, or 18ms for the DHT11 sensor
    portConfig(portIdSens, PTYPE_DOUT);
    portWriteBitadr(bitadrSens, 0);
#if (SENS_DHT_SUPPORT_DHT22 == 1)
    if (type==SENS_DHT_TYPE_DHT22) {
        nzDelayMs(2);
    }
#endif
#if (SENS_DHT_SUPPORT_DHT11 == 1)
    if (type==SENS_DHT_TYPE_DHT11) {
        nzDelayMs(20);
    }
#endif
    //Disable all interrupts lower than system tick. This will NOT disable the system tick!
    //Do this AFTER 2 or 20mS delay above, and before ending start signal below!
    NZ_SET_AND_SAVE_CPU_IPL(savedIpl, nzINT_PRIORITY_TICK-1);

    //End start signal
    portWriteBitadr(bitadrSens, 1);
    portConfig(portIdSens, PTYPE_DIN | PCFG_DIN_PULLUP);

    // Wait for LOW responce from sensor = low pulse 80uS. Is sent 20-40uS after our start signal
    if (sensDht_waitForPin(0, bitadrSens, 100) == 0) {
        NZ_RESTORE_CPU_IPL(savedIpl);   //Restore interrupts

        //Toggle power - turn power off
        portWriteBitadr(sens.bitadrPwrSwt, (sens.flags.pwrSwtInverted==TRUE)?1:0);

        sens.sm = SM_SENSDHT_POWERUP_RESET;
        sens.tmrDelay = tick16Get() + tick16ConvertFromMS(SENS_DHT_POWER_RST_TIME);
        
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT(");
        DEBUG_PUT_BYTE(DEBUG_LEVEL_WARNING, portIdSens);
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, ") no responce");

        err=SENS_DHT_ERR_NO_RESPONCE;   //Error, no responce received from sensor!
        goto DHT22_SENS_READ_ERROR;
    }

//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nDHT Rcv: ");

    //Read 40 bits from sensor
    for(i=0; i<41; i++) {
        //Wait for sensor pin to go high. Will stay high 26-28uS for 0, and 70uS for 1
        if (sensDht_waitForPin(1, bitadrSens, 100) == 0) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT Err3");
            err=SENS_DHT_ERR_PORT_LOW;      //Error, DHT sensor holding bus low
            goto DHT22_SENS_READ_ERROR;
        }

        //First high-low pulse is responce from sensor. Is responce to our "Start" pulse. NOT first bit!
        if (i!=0) {
            BOOL bit;
            //Delay 50uS and measure pin. For low, pin stays high 26-28us, and for high 70uS.
            //So, after 50uS should the right time to measure pin
            nzDelayUs(SENS_DHT_RD_DLY);

            bit = portReadBitadr(bitadrSens);

            sens.data[(i-1)/8] <<= 1;
            sens.data[(i-1)/8] |= (bit==0)?0:0x01;

//            DEBUG_PUT_BYTE(DEBUG_LEVEL_INFO, portReadBitadr(bitadrSens));
//            DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ',');
        }

        //Wait for low pulse from slave
        if (sensDht_waitForPin(0, bitadrSens, 100) == 0) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT Err4");
            err=SENS_DHT_ERR_NO_RESPONCE;   //Error, no responce from DHT sensor
            goto DHT22_SENS_READ_ERROR;
        }
    }

    //Check CRC
    w = 0;
    for (i=0; i<4; i++) {
        w += sens.data[i];
    }
    //Check for CRC Error
    if (sens.data[4] != (w&0xff)) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT CRC Error!");
        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[4]);
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "!=");
        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, w&0xff);
        err=SENS_DHT_ERR_CRC;       //CRC Error
        goto DHT22_SENS_READ_ERROR;
    }

//    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT =");
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[0]);
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[1]);
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[2]);
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[3]);
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[4]);


    //Save data to sens.data[]
#if (SENS_DHT_SUPPORT_DHT22 == 1)
    if (type==SENS_DHT_TYPE_DHT22) {
        //Swap bytes 1&2 and 2&3
        i = sens.data[0];
        sens.data[0] = sens.data[1];
        sens.data[1] = i;
        i = sens.data[2];
        sens.data[2] = sens.data[3];
        sens.data[3] = i;
    }
#endif
#if (SENS_DHT_SUPPORT_DHT11 == 1)
    //if (type==SENS_DHT_TYPE_DHT11) {
        //No swapping of data required for the DHT11
    //}
#endif

    //After all bits are sent, bus is released by sensor, and goes high
    if (sensDht_waitForPin(1, bitadrSens, 100) == 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT Err5");
        err=SENS_DHT_ERR_PORT_LOW;  //Error, DHT sensor holding bus low
        goto DHT22_SENS_READ_ERROR;
    }

    NZ_RESTORE_CPU_IPL(savedIpl);   //Restore interrupts

//    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nDHT =");
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[0]);
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[1]);
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[2]);
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[3]);
//    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, sens.data[4]);

    sens.flags.error = 0;
    return 0;   //Success

    //ERROR! Set error flag, delay 2ms
DHT22_SENS_READ_ERROR:
    NZ_RESTORE_CPU_IPL(savedIpl);   //Restore interrupts
    //Ensure pin is input (high impedance)
    portConfigDir(portIdSens, 1);   //Configure port as input
    //nzDelayMs(2);
    sens.flags.error = 1;   //Error
    return err;
}


/**
 * Get the current Humidity * 10. For example, a returned value of 503 is 50.3%
 * humidity.
 *
 * @return The Humidity * 10
 */
WORD sensDht_getHumidityX10(void) {
    if (sens.flags.error != 0)
        return 0;

#if (SENS_DHT_SUPPORT_DHT22 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT22) {
        return ((WORD)*((WORD*)&(sens.data[0])));
    }
#endif
#if (SENS_DHT_SUPPORT_DHT11 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT11) {
        return (((WORD*)&(sens.data[0]))[0] * 10);
    }
#endif

    return 0;
}


/**
 * Get the integer (whole) part of the current Humidity. For example, if the current
 * humidity is 50.3, 50 will be returned.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The integer (hole) part of the current Humidity
 */
WORD sensDht_getHumidityInteger(void) {
    if (sens.flags.error != 0)
        return 0;

#if (SENS_DHT_SUPPORT_DHT22 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT22) {
        return (((WORD)*((WORD*)&(sens.data[0])))/10);
    }
#endif
#if (SENS_DHT_SUPPORT_DHT11 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT11) {
        return (sens.data[0]);
    }
#endif

    return 0;
}


/**
 * Get the decimal (fraction) part of the current Humidity. For example, if the current
 * humidity is 50.3, this function will be returne 3.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The decimal (fraction) part of the current Humidity
 */
WORD sensDht_getHumidityDecimal(void) {
    if (sens.flags.error != 0)
        return 0;

#if (SENS_DHT_SUPPORT_DHT22 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT22) {
        return (((WORD)*((WORD*)&(sens.data[0])))%10);
    }
#endif
#if (SENS_DHT_SUPPORT_DHT11 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT11) {
        return (sens.data[1]);
    }
#endif

    return 0;
}


/**
 * Get the current Temperature * 10. For example, a returned value of 203 is 20.3
 * Degrees Celcuis.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The Temperature * 10
 */
WORD sensDht_getTemperatureX10(void) {
    if (sens.flags.error != 0)
        return 0;

#if (SENS_DHT_SUPPORT_DHT22 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT22) {
        return ((WORD)*((WORD*)&(sens.data[2])));
    }
#endif
#if (SENS_DHT_SUPPORT_DHT11 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT11) {
        return (((WORD*)&(sens.data[2]))[0] * 10);
    }
#endif

    return 0;
}


/**
 * Get the integer (whole) part of the current Temperature. For example, if the current
 * humidity is 50.3, 50 will be returned.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The integer (hole) part of the current Temperature
 */
WORD sensDht_getTemperatureInteger(void) {
    if (sens.flags.error != 0)
        return 0;

#if (SENS_DHT_SUPPORT_DHT22 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT22) {
        return (((WORD)*((WORD*)&(sens.data[2])))/10);
    }
#endif
#if (SENS_DHT_SUPPORT_DHT11 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT11) {
        return (sens.data[2]);
    }
#endif
    return 0;
}


/**
 * Get the decimal (fraction) part of the current Temperature. For example, if the current
 * humidity is 50.3, this function will be returne 3.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The decimal (fraction) part of the current Temperature
 */
WORD sensDht_getTemperatureDecimal(void) {
    if (sens.flags.error != 0)
        return 0;

#if (SENS_DHT_SUPPORT_DHT22 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT22) {
        return (((WORD)*((WORD*)&(sens.data[2])))%10);
    }
#endif
#if (SENS_DHT_SUPPORT_DHT11 == 1)
    if (sens.flags.type == SENS_DHT_TYPE_DHT11) {
        return (sens.data[3]);
    }
#endif

    return 0;
}


