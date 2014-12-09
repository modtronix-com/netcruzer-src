/**
 * @brief           Functions for DHT11 and DHT22 Temperature and Humidity Sensors
 * @file            nz_sensDhtBlock.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_sensorDhtBlock_desc Description
 *****************************************
 * Functions for reading the DHT11 and DHT22 temperature and humidity values.
 * This is a blocking version of the DHT library, meaning code is blocked for about
 * 5ms while a read takes place. The "nz_sensDht.h" and "nz_sensDht.c" is the
 * non blocking version.
 *
 * @subsection nz_sensorDhtBlock_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --------- DHT Sensor Configuration (from nz_sensDhtBlock.h) -----------
// *********************************************************************
#define SENS_DHT_BLOCK_ENABLE                               // Enable

// Initial read delay 2 seconds (2000ms)
#define SENS_DHT_STARTUP_READ_DELAY           ( 5000 )      //[-DEFAULT-]

// Power down reset time in mS
#define SENS_DHT_POWER_RST_TIME               ( 2000 )      //[-DEFAULT-]

// Delay after reset till read allowed
#define SENS_DHT_RESET_READ_DELAY             ( 2000 )      //[-DEFAULT-]

 @endcode
 *
 *
 * @subsection nz_sensorDhtBlock_usage Usage
 *****************************************
 * To use this module, the following must be done:
 * - Include nz_sensDhtBlock.h in the c file it is used in.
 * - Add nz_sensDhtBlock.c to the MPLAB project, this is the main DHT22 driver file.
 * - The following additional files are required by nz_sensDhtBlock.c, and must be added to the project:
 *   nz_helpers.c, nz_ioPorts.c, nz_netcruzer.c
 * - Add sensDht_task() to main loop. For example:
 *      sensDht_init(36);        //Initialize using port 36 (old port name Y6) for DHT Sensor
 *      while(1) {
 *          nzSysTaskDefault();  //Main netcruzer task, call in main loop.
 *          sensDht_task();        //DHT22 Task
 *     }
 * - In projdefs.h, do any DHT22 or other configuration required. This is done by copying the
 *   configuration section from the *.h files to projdefs.h. Nothing required, defaults will work!
 * - All done! Can now use DHT22.h driver functions!
 *
 * For details, see DHT22 and DHT11 demos in "../src/demos/sensors/" folder of Netcruzer Download.
 *
 **********************************************************************
 * @section nz_sensorDhtBlock_lic Software License Agreement
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
#ifndef NZ_SENS_DHT_BLOCK_H
#define NZ_SENS_DHT_BLOCK_H

#if defined(SENS_DHT_BLOCK_ENABLE)

////////// Defines //////////////////////////////
#define SENS_DHT_TYPE_DHT11       ( 1 )
#define SENS_DHT_TYPE_DHT22       ( 2 )

typedef struct __attribute__((__packed__)) SENS_DHT_
{
    BYTE    data[5];
    BYTE    sm;

    union {
        struct
        {
            unsigned char type              : 4;    //Sensor type
            unsigned char error             : 1;
            unsigned char hasData           : 1;    //Set if sensor data has been read
            unsigned char pwrSwtInverted    : 1;    //Power switch is inverted
            unsigned char fill7to7          : 1;
        } flags;
        BYTE flagsVal;
    };

    BYTE    fill;               //Fill required to get WORDs below on word boundaries
    WORD    tmrDelay;           //Timer for read
    WORD    bitadrPwrSwt;       //bitadr for power switch LAT register, or BITADR_NA if not used

} DHT_SENSOR;


// DHT Sensor Error code
typedef enum
{
    SENS_DHT_ERR_NONE = 0,          // No Error
    SENS_DHT_ERR_BUSY,              // Module is currencly busy
    SENS_DHT_ERR_INVALID_PORT,      // Invalid port
    SENS_DHT_ERR_PORT_LOW,          // Port is low, but should be high
    SENS_DHT_ERR_NO_RESPONCE,       // No responce from DHT sensors
    SENS_DHT_ERR_CRC                // No responce from DHT sensors
} SENS_DHT_ERROR;


////////// Functions ////////////////////////////


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
BYTE sensDht_init(WORD startupDelay, BYTE portPowerSwitch, BOOL powerSwitchInverted);


/**
 * Call this function for each sensor. Configures IO Port (port of SBC) for the DHT
 * Sensor as an input, with pull-up resistor enabled.
 *
 * @param portIdSens The "Port ID" connected to the sensor pin of the DHT22. Is a
 *             number from 0 to 41, also known as the "Netcruzer Port Name".
 */
void sensDht_addSensor(BYTE portIdSens);


/**
 * Checks if the DHT sensor module is ready.
 *
 * @return Returns 1 if Ready, else 0.
 */
BOOL sensDht_isReady();


/**
 * Main DHT Task, called in task context (not ISR or Thread context).
 * Does NOT have to be called too often, once every couple of 100ms will
 * be fine. But, constant calls are also OK.
 */
void sensDht_task();


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
 * @return Returns 0 if success, else error code.
 */
BYTE sensDht_read(BYTE type, BYTE portIdSens);


/**
 * Get the last read Humidity * 10. For example, a returned value of 503 is 50.3%
 * humidity.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The Humidity * 10
 */
WORD sensDht_getHumidityX10(void);


/**
 * Get the integer (whole) part of the current Humidity. For example, if the current
 * humidity is 50.3, 50 will be returned.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The integer (hole) part of the current Humidity
 */
WORD sensDht_getHumidityInteger(void);


/**
 * Get the decimal (fraction) part of the current Humidity. For example, if the current
 * humidity is 50.3, this function will be returne 3.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The decimal (fraction) part of the current Humidity
 */
WORD sensDht_getHumidityDecimal(void);


/**
 * Get the current Temperature * 10. For example, a returned value of 203 is 20.3
 * Degrees Celcuis.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The Temperature * 10
 */
WORD sensDht_getTemperatureX10(void);


/**
 * Get the integer (whole) part of the current Temperature. For example, if the current
 * humidity is 50.3, 50 will be returned.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The integer (hole) part of the current Temperature
 */
WORD sensDht_getTemperatureInteger(void);


/**
 * Get the decimal (fraction) part of the current Temperature. For example, if the current
 * humidity is 50.3, this function will be returne 3.
 *
 * @preCondition The sensDht_read() function must be called before calling this
 * function!
 *
 * @return The decimal (fraction) part of the current Temperature
 */
WORD sensDht_getTemperatureDecimal(void);


/**
 * Gets the status of the last transmission.
 * @return The status of the last transmission, 0=Success, else error code
 */
#define sensDht_getStatus()    (dhtInfo.status)

#endif  //#if defined(SENS_DHT_BLOCK_ENABLE)
#endif  //#ifndef NZ_SENS_DHT_BLOCK_H
