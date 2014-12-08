/**
 * Project Specific Defines. Are defines in projdefs.h. These are defines that
 * is the same for all target hardware. Place hardware specific defines in
 * "Configs/HWP_BOARDNAME.h" file for target board
 */
#ifndef _PROJDEFS_H_
#define _PROJDEFS_H_


//Ensure this define is uncommented for release build
//#define RELEASE_BUILD


// *********************************************************************
// ---------- Netcruzer System Configuration (nz_netcruzer.h) ----------
// *********************************************************************
#define     NZSYS_DONT_MANAGE_TICK          //Don't include nz_tick h & c files AND don't manage it(init, task)
#define     NZSYS_DONT_MANAGE_MAINLOOP      //Don't create the mainLoop() function
#define     mainLoop mainAppTask            //Use mainAppTask for main loop

#define     NZSYS_CALL_INITIALIZE_BOARD     //The initializeBoard() function will be called during Netcruzer initialization
#define     NZSYS_CALL_APPCONF_FUNCTIONS    //The cfgInit() and appConfInit() function will be called during Netcruzer initialization

//Default size of RAM User Memory. Default is 256, set to 0 to disable all "User Memory" functions
#define     USER_RAM_SIZE                       ( 256 )


// *********************************************************************
// --------- Netcruzer RTOS Configuration (from nzos_rtos.h) -----------
// *********************************************************************
#define nzosENABLE                              ( 1 )
#define nzosHEAP_SIZE                           ( 256 )
#define nzTckGetUtick()                          utick32Get()
#define nzosINCLUDE_PORT_C_FILE					(0)

// ----- Fiber Configuration -----
#define nzosFIBER_ENABLE                        ( 1 )
// ----- Memory Configuration -----
#define nzosMEM_USE1                            ( 0 )


// *********************************************************************
// ------------ DHT22 Configuration (from nz_sensDht.h) -------------
// *********************************************************************
#define SENS_DHT_TIMEOUT						( 200 )		// Timeout 100ms
#define SENS_DHT_STARTUP_READ_DELAY			( 2000 )	// Initial read delay 2 seconds (2000ms)
// TEST TEST
//#define SENS_DHT_AUTO_READ_PERIOD				( 2200 )	// Automatically read every 2.2 seconds,
#define SENS_DHT_AUTO_READ_PERIOD				( 0 )	// Automatically read every 2.2 seconds,
#define SENS_DHT_PIN_ARRAY_SIZE               ( 256 )		// Size of Pin Array read buffer, must be 4,8,16,32,64,128 or 256. Use 256 for optimum speed!
#define SENS_DHT_TYPE		                    ( 2 )		// Sensor Type, 1=DHT11, 2=DHT22
// TEST TEST
#define SENS_DHT_NUMBER_OF_SENSORS			( 2 )		// Number of Sensors


// *********************************************************************
// -------------- ADC Filtered Configuration --------------------
// *********************************************************************
//Total required memory is ADC_MAX_CHAN x ADC_FILTERS words.
#define    ADC_FILTER_STAGES  4


// *********************************************************************
// ------------ AppConfig Configuration (from nz_appConfig.h) -------------
// *********************************************************************
//Normally the CFG_STRUCT is located in the external Flash. It is placed there by the bootloader.
//If a bootloader is not used, it has to be define in code, and copied to the external Flash.
#define CFG_STRUCT_IN_CODE


// *********************************************************************
// ------- Circular Buffer Configuration (nz_circularBuffer.h) ---------
// *********************************************************************
//Specifies what circular buffer is used. Ensure only one of the following is uncommented. If none selected, nz_circularBufferStd is used
//#define    CIRBUF_USE_CIRCULAR_BUFFER_STD      //Use nz_circularBufferStd
#define    CIRBUF_USE_CIRCULAR_BUFFER_PWR2     //Use nz_circularBufferPwrs

//Optimize for size, not speed!
//#define    CIRBUF_OPTIMIZE_SIZE

//Configure buffer NOT to supports packets - define to create smaller code when packets not used.
//#define    CIRBUF_DISABLE_PACKETS

//Indicate if buffer should support large packet types. That is, a packet with a size larger than 255 bytes.
//#define    CIRBUF_DISABLE_LARGE_PACKET

#define CIRBUF_DISABLE_ALL_MACRO_FUNCTIONS


// ************************************************************
// --------------------  Cmd Configuration --------------------
// ************************************************************
//Disable the given commands. Use these defines to save code space if command is not required
//#define CMD_DISABLE_CMD_LL_AND_LK


 // *********************************************************************
 // ---------- nz_db66dev1 Configuration (from nz_db66dev1.h) -----------
 // *********************************************************************
 //All defines below that define a millisecond time value should be multiples of
 //the DB66DEV1_SERVICE_TIME_MS define (default 10ms)!
 #define DB66DEV1_DEBOUNCE_RST_MS           30      //Debounce time = 30ms default
 #define DB66DEV1_DEBOUNCE_REPEAT_MS        250     //Initial delay between repeats = 250ms default
 #define DB66DEV1_DEBOUNCE_REPEAT_TIMES     2       //2 times = 2*250ms = 500ms default
 #define DB66DEV1_DEBOUNCE_REPEAT2_MS       100     //Next delay between repeats = 100ms default
 #define DB66DEV1_DEBOUNCE_REPEAT2_TIMES    5       //5 times = 5*100ms = 500ms default
 #define DB66DEV1_DEBOUNCE_REPEAT3_MS       50      //Next delay between repeats = 50ms default
 #define DB66DEV1_DEBOUNCE_REPEAT3_TIMES    20      //20 times = 20*50ms = 1 Second
 #define DB66DEV1_DEBOUNCE_REPEAT4_MS       10      //Next delay between repeats = 10ms default

 #define DB66DEV1_SERVICE_TIME_MS           10      //Specifies how often the service routine is called, default is 10ms


// *********************************************************************
// ------------ Debounce Configuration (from nz_debounce.h) -------------
// *********************************************************************
//Define functions for reading each port's value.
//#define DEBOUNCE_GET_PORT1()    imod1_ReadButton1()
//#define DEBOUNCE_GET_PORT2()    imod1_ReadButton2()
//#define DEBOUNCE_PORT_IMOD1_BUTTON1 1
//#define DEBOUNCE_PORT_IMOD1_BUTTON2 2


// *********************************************************************
// --------------- Debug Configuration (nz_debug.h) --------------------
// *********************************************************************
#define DEBUG_USE_USBHID
//#define DEBUG_USE_UART1

//Size of Debug TX buffer, MUST BE power of 2 value! Increase size of this buffer if debug information
//is getting lost. This can be the case if the application writes debug information to the debug buffer
//faster than it can be outputted.
//#define DEBUG_TXBUF_SIZE 256
#define DEBUG_TXBUF_SIZE 1024

//Size of Debug RX buffer, MUST BE power of 2 value! Ensure it is large enought to hold largest string
//written to debug port. For example, when using the USB port for debugging, text message are sent to
//the debug port from the @ref devtools_nzUsbTerminal.
#define DEBUG_RXBUF_SIZE 32

//Uncomment this line to disable all debugging!
//#define DEBUG_LEVEL_ALLOFF

//To enable debug configuration for additional modules, add line to each of the 3 sections below with name of new module. For example in first section, add "#define DEBUG_CONF_NEWMOD 0"
#if defined (DEBUG_LEVEL_ALLOFF)
    #define DEBUG_CONF_APPCONFIG                0
    #define DEBUG_CONF_NZ_APPCONFIG             0
    #define DEBUG_CONF_CIRCULAR_BUFFER          0
    #define DEBUG_CONF_CMD                      0
    #define DEBUG_CONF_CUSTOM_HTTP_APP          0
    #define DEBUG_CONF_DHT22                    0
    #define DEBUG_CONF_HTTP2                    0
    #define DEBUG_CONF_SERPORTI2C               0
    #define DEBUG_CONF_IP                       0
    #define DEBUG_CONF_MAIN                     0
    #define DEBUG_CONF_MPFS2                    0
    #define DEBUG_CONF_PORTCONFIG               0
    #define DEBUG_CONF_SERIALPORTS              0
	#define DEBUG_CONF_SERPORTNET               0
	#define DEBUG_CONF_SERPORTUART              0
    #define DEBUG_CONF_SERPORTUSB               0
    #define DEBUG_CONF_SPIFLASH                 0
    #define DEBUG_CONF_TCP                      0
    #define DEBUG_CONF_MAINAPP                  0
#else
    #if defined (RELEASE_BUILD)
        #define DEBUG_CONF_APPCONFIG                DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_NZ_APPCONFIG             DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_CIRCULAR_BUFFER          DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_CMD                      DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_CUSTOM_HTTP_APP          DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_DHT22                    DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_HTTP2                    DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_IP                       DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_SERPORTI2C               DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_MAIN                     DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_MPFS2                    DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_PORTCONFIG               DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_SERIALPORTS              DEBUG_LEVEL_WARNING
		#define DEBUG_CONF_SERPORTNET               DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_SERPORTUSB               DEBUG_LEVEL_WARNING
		#define DEBUG_CONF_SERPORTUART              DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_SPIFLASH                 DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_TCP                      DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_MAINAPP                  DEBUG_LEVEL_WARNING
    #else
        #define DEBUG_CONF_APPCONFIG                DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_NZ_APPCONFIG             DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_CIRCULAR_BUFFER          DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_CMD                      DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_CUSTOM_HTTP_APP          DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_DHT22                    DEBUG_LEVEL_INFO
        #define DEBUG_CONF_HTTP2                    DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_IOPORTS                  DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_IP                       DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_SERPORTI2C               DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_MAIN                     DEBUG_LEVEL_INFO
        #define DEBUG_CONF_MPFS2                    DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_PORTCONFIG               DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_SERIALPORTS              DEBUG_LEVEL_WARNING
		#define DEBUG_CONF_SERPORTNET               DEBUG_LEVEL_INFO
		#define DEBUG_CONF_SERPORTUART              DEBUG_LEVEL_INFO
        #define DEBUG_CONF_SERPORTUSB               DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_SPIFLASH                 DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_TCP                      DEBUG_LEVEL_WARNING
        #define DEBUG_CONF_MAINAPP                  DEBUG_LEVEL_WARNING
    #endif
#endif


// *********************************************************************
// --------------- I2C Configuration (from nz_serI2C.h) ----------------
// *********************************************************************
#define HAS_SERPORT_I2C1


// *******************************************************
// ----------------- xEeprom Configuration -----------------
// ********************************************************
#define XEEPROM_BUFFER_SIZE      (32)


// *********************************************************************
// -------------- "Serial Data Port" Configuration --------------------
// *********************************************************************
//Dont create TX and RX buffers for "Serial Data Ports"
#define SERPORT_DONT_CREATE_TXRX_BUFS

//Dont initialize the SER_INFO structure, we will provide the serDefaultInit() function!
#define SERPORT_DONT_MANAGE_SER_INFO_INIT


// *********************************************************************
// ------------ RTC Configuration (from nz_rtc.h) -------------
// *********************************************************************
#define NZ_RTC_ENABLED                          (1) //Enable RTC
#define NZ_RTC_UNLOCK                           (1) //Unlock RTC during initialization


// *********************************************************************
// -------------- Tick Configuration ------------------
// *********************************************************************
//This is how many ticks there are per second, and will be 64 x 1000 = 64,000. The original
//Microchip TCP/IP stack had 62500.5.
#define TICKS_PER_SECOND    (64000ul)
#define NZSYS_TICK_DISABLE_VARS_INT_AND_INIT
#define	NZSYS_TICK_TMR2AND3_UTICK				( 1 )


// *********************************************************************
// -------------- USB stack hardware selection options -----------------
// *********************************************************************
//#define HAS_SERPORT_USB_CDC
#define HAS_SERPORT_USB_HID

#endif  //_PROJDEFS_H_
