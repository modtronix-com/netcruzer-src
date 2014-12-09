 /**
 * @example demos/DB66DEV1_Demo1/main.c
 *
 * <h2>===== Description =====</h2>
 * This demo shows how to use the <a href="http://netcruzer.com/sbc66ec.html/">SBC66EC</a> (SBC66ECL) board together with
 * the <a href="http://netcruzer.com/db66dev1/">DB66DEV1</a> Development daughter board, and a DHT22 Sensor connector to
 * an IO Port(port of SBC). Can also be configured for DHT11 in projdefs.h file. For this demo, the DHT22 is connected to
 * port 36 (old port name Y6) of the SBC66 board. To use a different port, replace all lines of code in this file that
 * contain PIN_36 and 36 with the new port. For example PIN_05 and 5 to use port 5. To enable multiple DHT sensors,
 * edit the SENS_DHT_NUMBER_OF_SENSORS define in the projdefs.h file.
 *
 * The temperature and humidity values are stored in "User Memory" locations 0,1,2 and 3. Their values can be displayed
 * on a web page, or requested via a command using the "User Memory" tag. For details, see "User Memory" section on this page:
 * http://www.netcruzer.com/project-webserver-tags
 *
 * <h2>===== Required Hardware =====</h2>
 * This demo requires the following hardware:
 * - The <a href="http://netcruzer.com/sbc66ec.html/">SBC66EC</a> Single Board Computer. It can however easily be compiled
 *    to run on any of our <a href="http://netcruzer.com/products-sbc66-main/">SBC66 Netcruzer boards</a>
 * - The <a href="http://netcruzer.com/db66dev1/">DB66DEV1</a> Development daughter board.
 * - A DHT11 or DHT22 Temperature and Humidity sensor
 *
 * <h2>===== Building Project =====</h2>
 * This demo is part of the main Webserver project, and is located in the "src/projects/webserver/demos/DB66DEV1_Demo1" folder of the
 * <a target="_blank" href="http://netcruzer.com/nzdownload/">Netcruzer Download</a>. To compile, open the Webserver project
 * in MPLAB X, and select the "Project Configuration" that includes "_DB66DEV1_Demo1" in it's name. For example, to build firmware
 * for the <a href="http://netcruzer.com/sbc66ec.html">SBC66EC</a> board, select "SBC66EC_DB66DEV1_Demo1".
 * For details <a target="_blank" href="http://netcruzer.com/nz/doc/building_projects/">click here</a>
 *
 * <h2>===== Programming Board =====</h2>
 * After compiling (build), the board can be programmed via the <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb">
 * USB Bootloader</a> or a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_prog">PIC Programmer</a>. USB Programming is
 * simplified when using the SBC board together with a <a target="_blank" href="http://netcruzer.com/nz/doc/update_firmware_usb_pt66">Prototype Board</a>.
 *
 * <h2>===== File History =====</h2>
 * 2012-10-28, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

#if defined(SENS_DHT22_DEMO1)

//Netcruzer include files
#include "HardwareProfile.h"
#include "main.h"
#include "mainApp.h"
#include "nz_interrupt.h"
#include "nz_serDataPorts.h"
#include "nz_db66dev1.h"
#include "nz_ioPorts.h"
#include "nz_sensDht.h"
#include "cmd.h"
#include "nzos_fiber.h"


//Add debugging. DEBUG_CONF_MAIN macro sets debugging to desired level (defined in projdefs.h)
#if !defined(DEBUG_CONF_MAIN)
    #define DEBUG_CONF_MAIN        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_MAIN
#include "nz_debug.h"

#undef XBRD_DB66DEV1_R1

// Function Prototypes //////////////////////////
void processIO(void);
WORD processCmd(char * name, char * value, void* param1, WORD flags);
WORD processTag(BYTE* tag, WORD ref, BYTE* dest, BYTE user);


// Variables ////////////////////////////////////
static WORD	tmrProcessIO = 0;		//Timer definining ProcessIO rate (16-bit, 1ms Timer)
static WORD tmrServiceDB66DEV1 = 0; //Timer for checking button and Potentiometer inputs
FIBER_TCB	fbrTcbDHT22;			//DHT22 Fiber TCB


/**
 * Interrupt on change ISR, called if any of the configured ports change value
 */
void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void)
{
	WORD utick15bit_8us;
    intOnChangeClearIF();           //Clear "interrupt on change" IF (interrupt flag)

	//If "Interrupt on Change" is configured for inputs other than DHT Sensors, a check will be
	//required here to ensure a DHT sensor caused this interrupt! Not required for this example.
	//NZ_INT_DIS_PUSH();		//Disable interrupts - not required if this ISR has a high priority!

	utick15bit_8us = tick16Get_8us_noDisi();		//Get 8us time

	//The sensDht_isr_MACRO() Macro must be called in interrupt. Will defer processing to Fiber
	sensDht_isr_MACRO(utick15bit_8us);
	//NZ_INT_EN_POP();		//Enable interrupt

	//Schedule the DHT22 fiber (calls the sensDht_fbrTask() function) to run in lower interrupt priority.
	nzFbrSchedule(&fbrTcbDHT22);
}


/**
* Main application entry point.
*/
int main(void) {
    initMainApp();        //Initialize the main Webserver application

    tmrProcessIO = tick16Get() + tick16ConvertFromMS(600); //Startup delay of 600mS

    db66dev1_Init();
	
	//Create the DHT Sensor fiber! This will be executed in a lower interrupt priority than the main
	//DHT Sensor "interrupt on change" interrupt. The fiber will call the sensDht_fbrTask() function.
	nzFbrCreate(2, TRUE, sensDht_fbrTask, &fbrTcbDHT22);

    //DHT22 Initialization. First sensor is given with sensDht_init(). Use sensDht_addSensor() to add additional sensors.
    sensDht_init(36, 3000, IOPORT_ID_NA, 0);    //Initialize DHT22, using port 36 (old port name Y6), startup delay 3 seconds

    //Configure the interrupt on change for DHT22 on port 36 (old port name Y6)!
    //IMPORTANT!!! Use equal or higher priority than is used for System Tick = 4 by default.
    #if (nzINT_PRIORITY_TICK >= 6)
    #error "Ensure Tick priority is smaller or equal to Interrupt On Change priority"
    #endif
	intOnChangeConfig(CHANGE_INT_ON|CHANGE_INT_PRI_6);	//Enable Interrupt on change at Int Priority level 6 (high)
    intOnChangeClearIF();								//Clear "interrupt on change" IF (interrupt flag)
    intOnChangeEnablePort(36);				//Enable "interrupt on change" for Channel A = Port 36 (old port name Y6)

    //Add "User Command" listener, will call cmdProcessing() function
    cmdAddCmdListener(&processCmd);

    //Add "User Command" listener, will call cmdProcessing() function
    cmdAddTagListener(&processTag);

    //Now that all items are initialized, begin the co-operative multitasking loop. This infinite loop will
    //continuously execute all stack-related tasks, as well as your own application's functions.  Custom
    //functions should be added at the end of this loop. Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot or part of it) and returns so that other tasks
    //can do their job. If a task needs very long time to do its job, it must be broken down into smaller pieces
    //so that other tasks can have CPU time.
    while (1) {
        mainAppTask();      //Main webserver task
        processIO();

		//!!!!! FOLLOWING CODE IS ONLY ENABLED WHEN USING DB66DEV1 DEVELOPMENT BOARD !!!!!
        //Service DB66DEV1 every 10ms
		#if defined(XBRD_DB66DEV1_R1)
        if (tick16TestTmr(tmrServiceDB66DEV1)) {
            tick16UpdateTmrMS(tmrServiceDB66DEV1, 10);   //Update timer to expire in 10ms again
            db66dev1_Service();						//DB66DEV1 service routine. Must be called each 10ms (default, can be changed)
		}
		#endif
	}
}


/**
 * I/O functions
 */
void processIO(void) {
	sensDht_task();	//DHT22 Task

    //Enter every 500ms
    if (tick16TestTmr(tmrProcessIO)) {
        tick16UpdateTmrMS(tmrProcessIO, 500); //Update timer to expire in 500ms again

		if (sensDht_hasNewTemperature(0)) {
			//Store Temperature integer and decimal in User Ram 0x00 and 0x01
			nzGlobals.userRam[0] = (BYTE)sensDht_getTemperatureInteger(0);
			nzGlobals.userRam[1] = (BYTE)sensDht_getTemperatureDecimal(0);
			//Print it out to debug terminal
			debugPutString("\nTemperature = ");
			debugPutByte(nzGlobals.userRam[0]);
			debugPutChar('.');
			debugPutByte(nzGlobals.userRam[1]);
			//Avoid using sprintf and printf in project! Increases total memory by over 3KBytes!!!!
			//sprintf(debugTempBuf, "\nT: %d.%d",
			//		sensDht_getTemperatureInteger(0),
			//		sensDht_getTemperatureDecimal(0));
			//debugPutString(debugTempBuf);
		}
		if (sensDht_hasNewHumidity(0)) {
			//Store Humidity integer and decimal in User Ram 0x02 and 0x03
			nzGlobals.userRam[2] = (BYTE)sensDht_getHumidityInteger(0);
			nzGlobals.userRam[3] = (BYTE)sensDht_getHumidityDecimal(0);
			debugPutString("\nHumidity = ");
			debugPutByte(nzGlobals.userRam[2]);
			debugPutChar('.');
			debugPutByte(nzGlobals.userRam[3]);
			//Avoid using sprintf and printf in project! Increases total memory by over 3KBytes!!!!
			//sprintf(debugTempBuf, "\nRH: %d.%d",
			//		sensDht_getHumidityInteger(0),
			//		sensDht_getHumidityDecimal(0));
			//debugPutString(debugTempBuf);
		}


		//!!!!! FOLLOWING CODE IS ONLY ENABLED WHEN USING DB66DEV1 DEVELOPMENT BOARD !!!!!
		/////////////////////////////////////////////////
        //Rotate LEDs 26 to 29 (old port names T4 to T7) using "Port ID"
		#if defined(XBRD_DB66DEV1_R1)
		{
		    static BYTE ledPortID = 26;
			static WORD portBitadr = LAT_06_BITADR;

			portWriteBitadr(portBitadr, 0);

			//Rotate LEDs 26 to 29 (old port names T4 to T7) using "Port ID"
			if (ledPortID++ == 29)
				ledPortID = 26;
			portBitadr = portGetLAT(ledPortID);

			//Set bitAdr
			portWriteBitadr(portBitadr, 1);
		}
		#endif
    }
}

/**
 * Callback if a "User Command" is received.
 *
 * @param name The name part of the name-value command (NULL terminated string)
 *
 * @param value The value part of the name-value command (NULL terminated string)
 *
 * @param param1 A parameter passed to this function.
 *        If flags bit 7 (flags&0x80) = 0, this is the name (and path) of the calling file, for example "config/ports.htm".
 *        If flags bit 7 = 1, this contains a CIRBUF pointer for reply to be written to.
 *
 * @param flags Bits 0-3 is the user level, is a USER_XX constant.
 *              Bit 7 is "PARAM_IS_CIRBUF" flag. When set, param contains a CIRBUF pointer.
 *              Bits 8-15 is param1 specific
 */
WORD processCmd(char * name, char * value, void* param1, WORD flags) {
    //Received user command "testCmd"
    if (memcmp(&name[0], "xMyCmd", 6)==0) {
        //Do something.......
    }

    return 0;
}

/**
 * Callback if a "User Tag" is received.
 *
 * @param tag Pointer to string containing tag
 *
 * @param ref Reference used by this function to determine if it is the first call, or
 *      if it should continue from last return. If 0, this is the first call.
 *
 * @param dest The destination to write data to, has size of CMDGETTAG_VAL_SIZE
 *
 * @param user The user level required to process this request. Is a USER_XX constant
 *
 * Return Bit 0-14 = ref, bit 15 = more data. Bytes added to dest = ((returned bits 0-14) - (ref parameter)).
 *          If returned value = given 'ref parameter', no tag was returned
 */
WORD processTag(BYTE* tag, WORD ref, BYTE* dest, BYTE user) {
    //Received user tag "testTag"
    if (memcmp(&tag[0], "testTag", 7)==0) {
        //Return a value.....return 'hi' for this example
        strcpy((char*)dest, "hi");
        return ref+2;   //Return length of returned value = 2 for "hi"
    }

    return ref;
}

#endif  //#if defined(SENS_DHT22_DEMO1)