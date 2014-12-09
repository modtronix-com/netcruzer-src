/**
 * @brief           Functions for DHT11 and DHT22 Temperature and Humidity Sensors
 * @file            nz_sensDht.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_sensorDht_desc Description
 *****************************************
 * Functions for reading the DHT11 and DHT22 temperature and humidity values.
 * The SENS_DHT_STARTUP_READ_DELAY value will define the initial delay before reading
 * the first values, and is 5 seconds by default.
 *
 * The SENS_DHT_AUTO_READ_PERIOD value will define how long to delay between reading
 * the sensor. The default is 5000ms, meaning the temperature and humidity will
 * automatically be read every 5 seconds. If this value is set to 0, automatic read
 * are disabled. In this case, the sensDht_startRead() function has to be called to
 * start the reading process, which is performed in the background.
 *
 * @subsection nz_sensorDht_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ------------ DHT Sensor Configuration (from nz_sensDht.h) -------------
// *********************************************************************
// Timeout 100ms
#define SENS_DHT_TIMEOUT                        ( 100 )     //[-DEFAULT-]

// Initial read delay 2 seconds (2000ms)
#define SENS_DHT_STARTUP_READ_DELAY             ( 5000 )    //[-DEFAULT-]

// Automatically read every 5 seconds (5000ms), set to 0 to disable auto read!
#define SENS_DHT_AUTO_READ_PERIOD               ( 5000 )    //[-DEFAULT-]

// Size of Pin Array read buffer, must be 4,8,16,32,64,128 or 256. Use 256 for optimum speed!
#define SENS_DHT_PIN_ARRAY_SIZE                 ( 128 )     //[-DEFAULT-]

// Sensor Type, 1=DHT11, 2=DHT22
#define SENS_DHT_TYPE                           ( 2 )       //[-DEFAULT-]
// Number of Sensors, set to 0 to disable DHT Sensor module
#define SENS_DHT_NUMBER_OF_SENSORS              ( 1 )       //[-DEFAULT-]


 @endcode
 *
 *
 * @subsection nz_sensorDht_usage Usage
 *****************************************
 * To use this module, the following must be done:
 * - Include nz_sensDht.h in the c file it is used in.
 * - Add nz_sensDht.c to the MPLAB project, this is the main DHT22 driver file.
 * - The project must have "Netcruzer RTOS Fibers" enabled, see @ref info_rtos_usage_fiber.
 * - The following additional files are required by nz_sensDht.c, and must be added to the project:
 *   nz_helpers.c, nz_ioPorts.c, nz_netcruzer.c
 * - Add sensDht_task() to main loop. For example:
 *     sensDht_init(36);        //Initialize using port 36 (old port name Y6) for DHT Sensor
 *       while(1) {
 *           nzSysTaskDefault();  //Main netcruzer task, call in main loop.
 *         sensDht_task();        //DHT22 Task
 *     }
 * - In code, configure interrupt for pin used by DHT Sensor. In the ISR for this interrupt, call
 *   sensDht_isr_MACRO() macro.
 * - In projdefs.h, do any DHT22 or other configuration required. This is done by copying the
 *   configuration section from the *.h files to projdefs.h. Nothing required, defaults will work!
 * - All done! Can now use DHT22.h driver functions!
 *
 * For details, see DHT22 and DHT11 demos in "../src/demos/sensors/" folder of Netcruzer Download.
 *
 **********************************************************************
 * @section nz_sensorDht_lic Software License Agreement
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
#ifndef NZ_SENS_DHT_H
#define NZ_SENS_DHT_H

// Number of Sensors, default is 1
#if !defined(SENS_DHT_NUMBER_OF_SENSORS)
#define SENS_DHT_NUMBER_OF_SENSORS              ( 0 )
#endif

//Disable all code if SENS_DHT_NUMBER_OF_SENSORS = 0
#if (SENS_DHT_NUMBER_OF_SENSORS > 0)

////////// Default Defines //////////////////////
#if !defined(SENS_DHT_PIN_ARRAY_SIZE)
#define SENS_DHT_PIN_ARRAY_SIZE               ( 256 )
#endif

// Sensor Type, 1=DHT11, 2=DHT22
#if !defined(SENS_DHT_TYPE)
#define SENS_DHT_TYPE                            ( 2 )
#endif

#if (SENS_DHT_PIN_ARRAY_SIZE!=4) && (SENS_DHT_PIN_ARRAY_SIZE!=8) && (SENS_DHT_PIN_ARRAY_SIZE!=16) && (SENS_DHT_PIN_ARRAY_SIZE!=32) && (SENS_DHT_PIN_ARRAY_SIZE!=64) & (SENS_DHT_PIN_ARRAY_SIZE!=128) && (SENS_DHT_PIN_ARRAY_SIZE!=256)
#error "SENS_DHT_PIN_ARRAY_SIZE must be 4,8,16,32,64,128 or 256!"
#endif

#if (SENS_DHT_NUMBER_OF_SENSORS >= 40)
#error "SENS_DHT_NUMBER_OF_SENSORS must be less than 40!"
#endif

////////// Defines //////////////////////////////

typedef struct __attribute__((__packed__)) SENS_DHT_
{
    BYTE    data[4];
    BYTE    portID;

    //Flags, ensure 0 is default value! Are reset to 0 at new cycle
    union {
        struct
        {
            //Bottom 4 bits are cleared each cycle
            unsigned char error             : 1;
            unsigned char newTemperature    : 1;
            unsigned char newHumidity       : 1;
            unsigned char fill3             : 1;
            //Initialized during initialization
            unsigned char fill4to6          : 3;
            unsigned char initialized       : 1;
        } flags;
        struct
        {
            unsigned char clearEachCycle    : 4;
            unsigned char fill4to7          : 4;
        } flagsCycle;
        BYTE flagsVal;
    };
} DHT_SENSOR;


//typedef struct SENS_DHT_INFO_
typedef struct __attribute__((aligned(2), packed)) SENS_DHT_INFO_
//typedef struct __attribute__((__packed__)) SENS_DHT_INFO_
{
    WORD    timeout;
    WORD    tmrDelay;       //Timer for read
    WORD    tickLastPinChange;
    WORD    bitadrPin;      //bitadr for pin of current DHT sensor
    WORD    pinArr[SENS_DHT_PIN_ARRAY_SIZE];

    WORD    pinPut;
    WORD    pinGet;
    WORD    bitadrPwrSwt;   //bitadr for power switch LAT register, or BITADR_NA if not used
    BYTE    data[5];        //Data for current pin. Is copied to sens[].data once done
    BYTE    cnt;            //Temporary counters
    BYTE    sm;
    BYTE    currSens;         //Index of current sensor, is a value from 0 to (SENS_DHT_NUMBER_OF_SENSORS-1)
    union {
        struct
        {
            unsigned char pwrSwtInverted    : 1;    //Power switch is inverted
            unsigned char fill1to7          : 7;
        } flags;
        BYTE flagsVal;
    };
    BYTE    fill;            //Fill required to get on WORD boundary

    //A DHT_SENSOR structure for each sensor
    DHT_SENSOR    sens[SENS_DHT_NUMBER_OF_SENSORS];

} SENS_DHT_INFO;

extern SENS_DHT_INFO dhtInfo;
//extern SENS_DHT_INFO* pObj;

////////// Functions ////////////////////////////

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
BYTE sensDht_init(BYTE portIdSens, WORD startupDelay, BYTE portPowerSwitch, BOOL powerSwitchInverted);


/**
 * DHT Sensor Task
 */
void sensDht_task();


/**
 * DHT Sensor ISR Task. Is called from the pin interrupt connected to the sensor input.
 * Must be very fast. Just save current state. sensDht_fbrTask() will process it.
 *
 * For example:
 * @code
 * void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void) {
 *     WORD utick15bit_8us;
 *     intOnChangeClearIF();  //Clear "interrupt on change" IF (interrupt flag)
 *
 *     //Get 8us time, and clear MSB (bit 15). Is used to store value of pin
 *     utick15bit_8us = tick16Get_8us_noDisi() & 0x7fff;
 *     sensDht_isr_MACRO(PIN_05, utick15bit_8us);
 *
 *     nzFbrSchedule(&fbrTcbDHT22);
 * }
 * @endcode
 */
#define sensDht_isr_MACRO(tick15bit_8us)                                                                        \
    tick15bit_8us = tick15bit_8us & 0x7fff;                                                                            \
    /* Save value to be read later. Time is stored in lower 15-bits, and pin value in bit 15 */                        \
    if ((portReadBitadr_MACRO(dhtInfo.bitadrPin))!=0)                                                                \
        tick15bit_8us |= 0x8000;                                                                                    \
    dhtInfo.pinArr[dhtInfo.pinPut] = tick15bit_8us;                                                                    \
    /* Increment, and clear first bit outside of range. Not required for SENS_DHT_PIN_ARRAY_SIZE=256! */            \
    if (SENS_DHT_PIN_ARRAY_SIZE==256)                                                                                \
        dhtInfo.pinPut++;                                                                                            \
    else                                                                                                            \
        dhtInfo.pinPut = (dhtInfo.pinPut+1) & (~(SENS_DHT_PIN_ARRAY_SIZE));                                        \
    /* ALTENATIVE METHOD - might use more code because not sigle clear bit instruction like above! */                \
    /* dhtInfo.pinPut = (dhtInfo.pinPut + 1) & (SENS_DHT_PIN_ARRAY_SIZE-1); */                                    \
    /* ALTERNATIVE single line for all lines above */                                                                \
    /* dhtInfo.pinArr[dhtInfo.pinPut&(SENS_DHT_PIN_ARRAY_SIZE-1)] = (pin==0)?utick15bit:(utick15bit|0x8000); */    \


/**
 * DHT Sensor Fiber Task. Must be very fast. Just save current state. sensDht_task() will
 * process it.
 */
void sensDht_fbrTask();


/**
 * Set the IO Port (port of SBC) for the DHT Sensor at the given index. This function only
 * has to be called when there are more than 1 DHT sensors. This is the case when
 * SENS_DHT_NUMBER_OF_SENSORS is larger than 1. When there is only 1 sensor, it's
 * IO Port (port of SBC) is set with the sensDht_init() function.
 *
 * @param portIdSens The "Port ID" connected to the sensor pin of the DHT22. Is a
 *             number from 0 to 41, also known as the "Netcruzer Port Name".
 * @param idxSensor The index of the target sensor, is a value from 0 to n
 */
void sensDht_addSensor(BYTE idxSensor, BYTE portIdSens);


/**
 * Start reading the DHT Sensor temperature and humidity in the background.
 */
void sensDht_startRead();

/**
 * Indicated if new Humidity data is available. This flag is ALWAYS reset to false
 * when calling this function!
 *
 * @param idxSensor The index of the target sensor, is a value from 0 to n
 */
BOOL __INLINE_FUNCTION__ sensDht_hasNewHumidity(BYTE idxSensor) {
    BOOL ret;
    ret = dhtInfo.sens[idxSensor].flags.newHumidity;
    dhtInfo.sens[idxSensor].flags.newHumidity = FALSE;
    return ret;
}

/**
 * Indicated if new Temperature data is available. This flag is ALWAYS reset to false
 * when calling this function!
 *
 * @param idxSensor The index of the target sensor, is a value from 0 to n
 */
BOOL __INLINE_FUNCTION__ sensDht_hasNewTemperature(BYTE idxSensor) {
    BOOL ret;
    ret = dhtInfo.sens[idxSensor].flags.newTemperature;
    dhtInfo.sens[idxSensor].flags.newTemperature = FALSE;
    return ret;
}


/**
 * Get the current Humidity * 10. For example, a returned value of 503 is 50.3%
 * humidity.
 *
 * @param idxSensor The index of the target sensor, is a value from 0 to n
 *
 * @return The Humidity * 10
 */
#if (SENS_DHT_TYPE == 2)        //DHT22
#define sensDht_getHumidityX10(idxSensor) ((WORD)*((WORD*)&(dhtInfo.sens[idxSensor].data[0])))
#elif (SENS_DHT_TYPE == 1)    //DHT11
#define sensDht_getHumidityX10(idxSensor) (((WORD*)&(dhtInfo.sens[].data[0]))[0] * 10)
#endif


/**
 * Get the integer (whole) part of the current Humidity. For example, if the current
 * humidity is 50.3, 50 will be returned.
 *
 * @param idxSensor The index of the target sensor, is a value from 0 to n
  *
 * @return The integer (hole) part of the current Humidity
 */
#if (SENS_DHT_TYPE == 2)        //DHT22
#define sensDht_getHumidityInteger(idxSensor) (((WORD)*((WORD*)&(dhtInfo.sens[idxSensor].data[0])))/10)
#elif (SENS_DHT_TYPE == 1)    //DHT11
#define sensDht_getHumidityInteger(idxSensor) (dhtInfo.sens[idxSensor].data[0])
#endif


/**
 * Get the decimal (fraction) part of the current Humidity. For example, if the current
 * humidity is 50.3, this function will be returne 3.
 *
 * @param idxSensor The index of the target sensor, is a value from 0 to n
 *
 * @return The decimal (fraction) part of the current Humidity
 */
#if (SENS_DHT_TYPE == 2)        //DHT22
#define sensDht_getHumidityDecimal(idxSensor) (((WORD)*((WORD*)&(dhtInfo.sens[idxSensor].data[0])))%10)
#elif (SENS_DHT_TYPE == 1)    //DHT11
#define sensDht_getHumidityDecimal(idxSensor) (dhtInfo.sens[idxSensor].data[1])
#endif


/**
 * Get the current Temperature * 10. For example, a returned value of 203 is 20.3
 * Degrees Celcuis.
 *
 * @param idxSensor The index of the target sensor, is a value from 0 to n
 *
 * @return The Temperature * 10
 */
#if (SENS_DHT_TYPE == 2)        //DHT22
#define sensDht_getTemperatureX10(idxSensor) ((WORD)*((WORD*)&(dhtInfo.sens[idxSensor].data[2])))
#elif (SENS_DHT_TYPE == 1)    //DHT11
#define sensDht_getTemperatureX10(idxSensor) (((WORD*)&(dhtInfo.sens[idxSensor].data[2]))[0] * 10)
#endif


/**
 * Get the integer (whole) part of the current Temperature. For example, if the current
 * humidity is 50.3, 50 will be returned.
 *
 * @param idxSensor The index of the target sensor, is a value from 0 to n
 *
 * @return The integer (hole) part of the current Temperature
 */
#if (SENS_DHT_TYPE == 2)        //DHT22
#define sensDht_getTemperatureInteger(idxSensor) (((WORD)*((WORD*)&(dhtInfo.sens[idxSensor].data[2])))/10)
#elif (SENS_DHT_TYPE == 1)    //DHT11
#define sensDht_getTemperatureInteger(idxSensor) (dhtInfo.sens[idxSensor].data[2])
#endif


/**
 * Get the decimal (fraction) part of the current Temperature. For example, if the current
 * humidity is 50.3, this function will be returne 3.
 *
 * @param idxSensor The index of the target sensor, is a value from 0 to n
 *
 * @return The decimal (fraction) part of the current Temperature
 */
#if (SENS_DHT_TYPE == 2)        //DHT22
#define sensDht_getTemperatureDecimal(idxSensor) (((WORD)*((WORD*)&(dhtInfo.sens[idxSensor].data[2])))%10)
#elif (SENS_DHT_TYPE == 1)    //DHT11
#define sensDht_getTemperatureDecimal(idxSensor) (dhtInfo.sens[idxSensor].data[3])
#endif


/**
 * Indicates if the DHT22 is currently idle. This is the case if all sensors
 * have been read, and NOT configured for Auto reading (SENS_DHT_AUTO_READ_PERIOD = 0).
 * If this function returns true, the sensDht_startRead() function can be used
 * to start reading all sensors again.
 *
 * @return 1 DHT22 is currently idle,
 *         0 DHT22 is busy
 */
BOOL sensDht_isIdle();

/**
 * Gets the status of the last transmission.
 * @return The status of the last transmission, 0=Success, else error code
 */
#define sensDht_getStatus()    (dhtInfo.status)


#endif    //#if (SENS_DHT_NUMBER_OF_SENSORS > 0)

#endif  //#ifndef NZ_SENS_DHT_H
