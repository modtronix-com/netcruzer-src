/**
 * @brief           I2C Port functions
 * @file            nz_serI2C.c
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
 * 2011-06-03, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_SERI2C1_C
#define THIS_IS_A_SERPORT

#include "HardwareProfile.h"

#if defined(HAS_SERPORT_I2C)

#define USE_AND_OR /* To enable AND_OR mask setting */
#if defined(__C30__)
    #include<i2c.h>
#elif defined(__C32__)
#else
    #error "Processor or compiler not supported!"
#endif

#include "nz_circularBuffer.h"
#include "nz_serI2C.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"


//Add debugging to this file. The DEBUG_CONF_SERPORTI2C macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_SERPORTI2C)
    #define DEBUG_CONF_SERPORTI2C       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_SERPORTI2C
#include "nz_debug.h"


////////// Default Defines //////////////////////

#define I2C_USE_SHARED_FUNCTIONS

//#define I2C_DONT_SUPPORT_LISTENERS

//Number of listeners that can be registered for all I2C ports, must be 2,4 or 8.
#if !defined(I2C_LISTENERS)
#define    I2C_LISTENERS 4
#endif
#if (I2C_LISTENERS<1) || (I2C_LISTENERS>7)
#error "I2C_LISTENERS must be a value from 1 to 7!"
#endif

//I2C1 Interrupt priority, a value from 0 to 7. 0 will disable interrupt, 7 is highest priority.
#if !defined(nzINT_PRIORITY_I2C1)
#define     nzINT_PRIORITY_I2C1 3
#endif

#if !defined(I2C1_BAUD)
#define    I2C1_BAUD I2C_BAUD_100K
#endif

//I2C1 ISR timeout, default is 200ms.
#if !defined(I2C_TIMEOUT)
#define I2C_TIMEOUT 200
#endif

//I2C1 Search for STOP timeout, default is 5 Seconds.
#if !defined(I2C_SEARCH_STOP_TIMEOUT)
#define I2C_SEARCH_STOP_TIMEOUT 5000
#endif


////////// Defines //////////////////////////////

enum SM_I2C_ISR_ {
    SM_ISR_IDLE = 0,
    SM_ISR_START,
    SM_ISR_REPEATED_START,
    SM_ISR_MASTER_TX,
    SM_ISR_STOP,
    SM_ISR_MASTER_RX,
    SM_ISR_MASTER_ACK
} I2C_SM_ISR;

enum SM_I2C_TASK_ {
    SM_I2C_TASK_IDLE = 0,
    SM_I2C_TASK_SEARCH_STOP,
    SM_I2C_TASK_FOUND_MSG
} SM_I2C_TASK;

//i2cCheckBus return code
#define I2C_CHECKBUS_OK                 0   //OK
#define I2C_CHECKBUS_SCL_LOW            1   //Error, SCL low
#define I2C_CHECKBUS_INVALID_CH         2   //Invalid CH


////////// Function Prototypes //////////////////

void i2cISR (I2C_INFO* objI2C);
void serI2CTask(I2C_INFO* objI2C);
void i2cWriteMI2CIE(BYTE ch, BOOL val);
BYTE i2cCheckBus(BYTE ch);
BYTE statusMapGet(I2C_INFO* objI2C, BYTE key);
void statusMapPut(I2C_INFO* objI2C, BYTE key, BYTE status);



////////// Variables ////////////////////////////

#if defined(HAS_SERPORT_I2C1)
I2C_INFO            i2c1Info;
SERPORT_I2C_CONFIG  serportI2C1Conf;

//TX and RX "Circular Buffers". Use SERPORT_I2C1_CREATE_OWN_CIRBUFS define to use this
//module without SERPORTs. If NOT defined, this modules requires nz_serDataPorts.c
#if defined(SERPORT_I2C1_CREATE_OWN_CIRBUFS)
    CIRBUF  cbufTxI2C1;
    BYTE    bufTxI2C1[I2C1_TXBUF_SIZE];

    CIRBUF  cbufRxI2C1;
    BYTE    bufRxI2C1[I2C1_RXBUF_SIZE];
#endif
#endif

#if defined(HAS_SERPORT_I2C2)
static I2C_INFO     i2c2;
SERPORT_I2C_CONFIG  serportI2C2Conf;
#endif
#if defined(HAS_SERPORT_I2C3)
static I2C_INFO     i2c3;
SERPORT_I2C_CONFIG  serportI2C3Conf;
#endif

#if !defined(I2C_DONT_SUPPORT_LISTENERS)
static CIRBUF* i2cListeners[I2C_LISTENERS];    //Array of "Circular Buffer" pointers
#endif

#if defined(__C30__)
    volatile unsigned int* const I2CCON_ARR[] = {&I2C1CON, &I2C2CON, &I2C3CON};     //Don't use in code!!! Only use pI2CxCON()!
    #define pI2CxCON(n) (I2CCON_ARR[n])
    //#define pI2CxCON(n) ((&I2C1CON + (n*(&I2C2CON-&I2C1CON))))    //Not possible, because I2CxCON NOT evenly spaced appart!

    volatile unsigned int* const I2CSTAT_ARR[] = {&I2C1STAT, &I2C2STAT, &I2C3STAT}; //Don't use in code!!! Only use pI2CxCON()!
    #define pI2CxSTAT(n) (I2CSTAT_ARR[n])

    volatile unsigned int* const I2CTRN_ARR[] = {&I2C1TRN, &I2C2TRN, &I2C3TRN};
    volatile unsigned int* const I2CRCV_ARR[] = {&I2C1RCV, &I2C2RCV, &I2C3RCV};
#elif defined(__C32__)
    //volatile unsigned int* const I2CCON_ARR[] = {&I2C1CON, &I2C2CON, 0};            //Don't use in code!!! Only use pI2CxCON()!
    //#define pI2CxCON(n) (I2CCON_ARR[n])
    #define pI2CxCON(n) ((&I2C1CON + (n*(&I2C2CON-&I2C1CON))))    //POSSIBLE, because I2CxCON are evenly spaced appart!

    volatile unsigned int* const I2CSTAT_ARR[] = {0,0,0};                             //Don't use in code!!! Only use pI2CxCON()!
    #define pI2CxSTAT(n) (I2CSTAT_ARR[n])
    #define I2C1STATBITS __I2C1STATbits_t

    volatile unsigned int* const I2CTRN_ARR[] = {0,0,0};
    volatile unsigned int* const I2CRCV_ARR[] = {0,0,0};
#else
    #error "Processor or Compiler not supported!"
#endif

//volatile unsigned int* getI2CCON(BYTE ch) {
//    if (ch==0)
//        return &I2C1CON;
//    else if (ch==1)
//        return &I2C2CON;
//    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
//    else if (ch!=2) {
//        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ngetI2CCON() invalid ch!");
//    }
//    #endif
//
//    return &I2C3CON;
//};


/////////////////////////////////////////////////
#if defined(HAS_SERPORT_I2C1)

/**
 * I2C 1 ISR. The SEN, RSEN, PEN, RCEN and ACKEN bits (Least Significant 5 bits of
 * the I2CxCON register) and the TRSTAT bit are used to determine the current state.
 */
#if defined(__C30__)
void __attribute__((__interrupt__, no_auto_psv)) _MI2C1Interrupt (void)
#elif defined(__PIC32MX__)
void __attribute((interrupt(ipl2), vector(_I2C_1_VECTOR), nomips16)) _MI2C1Interrupt(void)
#endif
{
    
#if defined(__C30__)
    _MI2C1IF = 0;       //Clear interrupt status bit
#elif defined(__PIC32MX__)
#endif

    i2cISR(&i2c1Info);
}


/**
 * This function configures the I2C Control register and I2C Baud Rate Generator register.
 * For example:
 * config1 = (I2C_ON | I2C_7BIT_ADD );
 *
 * @preCondition: The user must configure the I2C data lines to be inputs
 * before calling this function!
 *
 * @param config1 This contains the parameter to configure the I2CCON register
 * I2C Enable bit:
 *   - I2C_ON
 *   - I2C_OFF  (Default)
 * I2C Stop in Idle Mode bit:
 *   - I2C_IDLE_STOP
 *   - I2C_IDLE_CON  (Default)
 * SCL Release Control bit (I2C Slave mode only):
 *   - I2C_CLK_REL
 *   - I2C_CLK_HLD  (Default)
 * Intelligent Peripheral Management Interface Enable. Enable receiving all addresses:
 *   - I2C_IPMI_EN
 *   - I2C_IPMI_DIS  (Default)
 * Slave Address bits, standard is 7 bits:
 *   - I2C_10BIT_ADD
 *   - I2C_7BIT_ADD  (Default)
 * Disable Slew Rate Control bit. For speeds of 400kHz and above, Slew Rate should be enabled:
 *   - I2C_SLW_DIS
 *   - I2C_SLW_EN  (Default)
 * SMBus Input Level bits. Enable I/O pin thresholds compliant with SMBus specification:
 *   - I2C_SM_EN
 *   - I2C_SM_DIS  (Default)
 * General Call Enable bit (I2C Slave mode only):
 *   - I2C_GCALL_EN
 *   - I2C_GCALL_DIS  (Default)
 * SCL Clock Stretch Enable bit (I2C Slave mode only, used in conjunction with SCL Release Control bit):
 *   - I2C_STR_EN
 *   - I2C_STR_DIS  (Default)
 *
 * @param baud Computed value for the baud rate generator, use a I2C_BAUD_XX define. For example, I2C_BAUD_400K
 *      or I2C_BAUD_100K or I2C_BAUD_1M
 */
#define i2c1Open_(config1, baud) {I2C1BRG = baud; I2C1CON = config1;}


/**
 * Initialization code.
 */
void serI2C1Init(void)
{
    WORD config;

    //Initialize whole structure with 0
    memset(&i2c1Info, 0, sizeof(i2c1Info));

    i2c1Info.pCbufTx = CIRBUF_TX_I2C1;
    i2c1Info.pCbufRx = CIRBUF_RX_I2C1;
    i2c1Info.flagsVal = 0;
    i2c1Info.flags.ch = 0;                  //I2C channel offset, channel 1 = 0
    i2c1Info.isr.bits.sm = SM_ISR_IDLE;     //ISR State machine idle
    i2c1Info.isrCnt = 0;
    i2c1Info.taskSm = SM_I2C_TASK_IDLE;     //Task State machine idle

#if defined(SERPORT_I2C1_CREATE_OWN_CIRBUFS)
    //I2C is always a Streaming type buffer, and format is "Binary, with Escape Sequence Processing"
    cbufInit(&cbufTxI2C1, bufTxI2C1, sizeof(bufTxI2C1), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
    cbufInit(&cbufRxI2C1, bufRxI2C1, sizeof(bufRxI2C1), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
#else
//    //I2C is a Streaming type buffer, and format is "Binary, with Escape Sequence Processing"
//    //!!! The following line will overwrite any formatting or type information contained in CFG_STRUCT !!!
//    cbufSetTypeAndFormat(CIRBUF_TX_I2C1, (CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING));
//    cbufSetTypeAndFormat(CIRBUF_RX_I2C1, (CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING));
#endif

    //Clear all flags
    serportI2C1Conf.flagsVal = 0;

    //Baud rate, do not configure for higher than 400kHz for boards with 400kHz RTC (SBC66EC)
    serportI2C1Conf.baud = I2C1_BAUD;

    //Configure I2C 1
    //Turn off I2C modules
    CloseI2C1();    //Disable I2C1 module if enabled previously

    //Check for locked I2C bus, I2C1 = 0
    i2cCheckBus(0);

    //I2C On, 7-Bit mode, SMBus disable
    config = (I2C_ON  | I2C_7BIT_ADD | I2C_SM_DIS);

    //For speeds LOWER (baud value larger=lower baud) than 400kbit/sec, disable slew rate
    if (serportI2C1Conf.baud > I2C_BAUD_400K) {
        config = config | I2C_SLW_DIS;
    }

    i2c1Open_(config, serportI2C1Conf.baud);  //configure I2C1

#if defined(__C30__)
    _MI2C1IF = 0;     //Clear interrupt flag

    // Configure I2C1 interrupt priority bits
    _MI2C1IP = nzINT_PRIORITY_I2C1;

    _MI2C1IE = 1; // Enable I2C 1 interrupt
#elif defined(__PIC32MX__)
#endif
}


BYTE i2c1GetStatus(BYTE adr) {
    return i2cGetStatus(&i2c1Info, adr);
}

BYTE i2c1GetStatusWait(BYTE adr) {
    #if defined I2C_USE_SHARED_FUNCTIONS
    return i2cGetStatusWait(&i2c1Info, adr);
    #else
    while(i2cIsBusy(&i2c1Info)){
        serI2CTask(&i2c1Info);
    };
    return i2cGetStatus(&i2c1Info, 0);
    #endif
}

BYTE i2c1IsBusy(void) {
    #if defined I2C_USE_SHARED_FUNCTIONS
    return i2cIsBusy(&i2c1Info);
    #else
    I2C_INFO* objI2C;
    objI2C = &i2c1Info;
    
    //Return TRUE if the ISR is NOT in the idle state!
    return (objI2C->isr.bits.sm != SM_ISR_IDLE);
    #endif
}

BYTE i2c1WriteMessage(BYTE adr, BYTE* msg, WORD size) {
    return i2cWriteMessage(&i2c1Info, adr, msg, size);
}

/**
 * Begin a transmission on I2C 1 port.
 *
 * @param adr The destination I2C address.
 *
 * @return A byte is returned, with the following meaning:
 * - 0: Success
 * - 1: Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size" to "projdefs.h" file,
 *      and increase size of buffer. Default is 128 bytes.
 * - 2: Other error.
 */
BYTE i2c1BeginTransmission(BYTE adr) {
    #if defined I2C_USE_SHARED_FUNCTIONS
    return i2cBeginTransmission(&i2c1Info, adr);
    #else
    I2C_INFO* objI2C;
    objI2C = &i2c1Info;

    objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_OK;  //Currently OK, have not finished adding whole message to buffer
    objI2C->adr = adr;                                     //Save address

    //If not enough space for at least "START, (escaped) adr, STOP", error! START and STOP require 2 bytes each (must be escaped)
    if (cbufGetFree(objI2C->pCbufTx) < 6) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2c1BeginTransmission");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return I2C_TXION_STATUS_ERR_NOSPACE;
    }

    //Put "Control Character" and START
    cbufPutByteNoCheck(objI2C->pCbufTx, cbufGetEscapeCharacter(objI2C->pCbufTx));
    cbufPutByteNoCheck(objI2C->pCbufTx, 's');

    //Put address byte
    cbufPutEscapedByte(objI2C->pCbufTx, adr);
    return 0;   //Success
    #endif
}

BYTE i2c1EndTransmission(void) {
    #if defined I2C_USE_SHARED_FUNCTIONS
    return i2cEndTransmission(&i2c1Info);
    #else
    I2C_INFO* objI2C;
    WORD_VAL w;
    objI2C = &i2c1Info;

    //If current transmission has already encountered an error, Stop, and return that error
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        return objI2C->flags.currTxionStatus;
    }

    //Put "Control Character" and STOP
    w.byte.LB = cbufGetEscapeCharacter(objI2C->pCbufTx);
    w.byte.HB = 'p';
    if (cbufPutWord(objI2C->pCbufTx, w.Val) == 0) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2c1EndTransmission");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return objI2C->flags.currTxionStatus;
    }

    //Start transmission
    serI2CTask(objI2C);

    return 0;   //Success
    #endif
}

/**
 * Add a byte to the I2C Transmit Queue (Circular Buffer)
 * @param b Byte to add to I2C Transmit Queue
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2c1Write(BYTE b) {
    #if defined I2C_USE_SHARED_FUNCTIONS
    return i2cWrite(&i2c1Info, b);
    #else
    I2C_INFO* objI2C;
    objI2C = &i2c1Info;

    //If current transmission has already encountered an error, Stop, and return that error
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        return objI2C->flags.currTxionStatus;
    }

    if (cbufPutEscapedByte(objI2C->pCbufTx, b) == 0) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2c1Write");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return objI2C->flags.currTxionStatus;
    }
    return 0;   //Success
    #endif
}


/**
 * Add given BYTE array to the I2C Transmit Queue (Circular Buffer)
 * @param arr The byte array to add to I2C Transmit Queue
 * @param size Number of bytes to add
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2c1WriteArray(BYTE* arr, BYTE size) {
    return i2cWriteArray(&i2c1Info, arr, size);
}


/**
 * Adds given NULL terminated string to the I2C Transmit Queue (Circular Buffer).
 * The NULL terminator is NOT included (not written to buffer!)
 * @param str Null terminated string to add
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2c1WriteString(const char* str) {
    return i2cWriteString(&i2c1Info, str);
}


/**
 * Adds given NULL terminated string to the I2C Transmit Queue (Circular Buffer).
 * The given string must be an ASCII String, and can contain Escape Sequences.
 *
 * For details on the format of an ASCII String with Escape Sequences,
 * see @ref nz_circularBuffer_format_ascii_esc "ASCII Format, with Escape Sequences".
 *
 * If the I2C Transmit Queue does not have enough space, a NON-Zero error code is
 * returned, and nothing is done.
 *
 * @param str Null terminated string to add, in "ASCII Format, with Escape Sequences".
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2c1WriteAsciiEscString(const char* str) {
    #if defined I2C_USE_SHARED_FUNCTIONS
    return i2cWriteAsciiEscString(&i2c1Info, str);
    #else
    if (cbufPutAsciiEscString(CIRBUF_TX_I2C1, 0, (const char*)str, ASCIIESC_ADD_S_AND_P) == 0)
        return I2C_TXION_STATUS_ERROR;

    //Start transmission
    serI2CTask(&i2c1Info);

    return 0;   //Success
    #endif
}


/**
 * Reads given number of bytes to the default Circular Receive Buffer.
 *
 * @param size Number of bytes to read.
 *
 * @return Returns 0 if request successfully added to Transmission Queue, else a
 * non-zero value is returned.
 */
BYTE i2c1Read(BYTE size) {
    #if defined I2C_USE_SHARED_FUNCTIONS
    return i2cRead(&i2c1Info, size);
    #else
    I2C_INFO* objI2C;
    objI2C = &i2c1Info;

    //If current transmission has already encountered an error, Stop, and return that error
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        return objI2C->flags.currTxionStatus;
    }

    //Check if 4 bytes available in buffer: "^rn" (n might require escaping = 2 bytes)
    if (cbufGetFree(objI2C->pCbufTx) < 4) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2c1Read");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return I2C_TXION_STATUS_ERR_NOSPACE;
    }

    //Put "Control Character" and 'r'
    cbufPutByteNoCheck(objI2C->pCbufTx, cbufGetEscapeCharacter(objI2C->pCbufTx));
    cbufPutByteNoCheck(objI2C->pCbufTx, 'r');

    //Put size byte
    cbufPutEscapedByte(objI2C->pCbufTx, size);

    return 0;   //Success
    #endif
}

BYTE i2c1ReadToArray(BYTE* buf, BYTE size) {
    return i2cReadToArray(&i2c1Info, buf, size);
}

BYTE i2c1ReadSlaveReg(BYTE adr, BYTE reg, BYTE* buf, BYTE size) {
    return i2cReadSlaveReg(&i2c1Info, adr, reg, buf, size);
}


#endif //#if defined(HAS_SERPORT_I2C1)

#define I2C1_ISR_DETAILED_DEBUG_INFO

void i2cISR(I2C_INFO* objI2C)
{
#if defined(__C30__)
    BYTE c;

    volatile unsigned int* pI2CCON;
    pI2CCON = pI2CxCON(objI2C->flags.ch);

    switch(objI2C->isr.bits.sm) {
        case SM_ISR_IDLE:
            break;
        case SM_ISR_START:     //Just finished sending start bit, next byte in buffer MUST be the address
        {
            #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c S");     //Start
            #endif
            //ERROR! Next byte in buffer is a control character!
            if (cbufGetEscapedByte(objI2C->pCbufTx, &c)) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "ERR: NoAdr");    //No address found to send after start!
                //Remove all bytes till stop character. Currently no need to do this, because
                //main task will remove all bytes till it finds a Start character
                //cbufRemoveBytes(objI2C->pCbufTx, sizeRemaining);
                //sizeRemaining = 0;

                //Reset bus, put Stop
                objI2C->isr.bits.sm=SM_ISR_STOP;
                objI2C->isr.bits.isrTxionStatus = I2C_TXION_STATUS_ERR_BUF_FRMT;  //Buffer format error
                ((I2C1CONBITS*)pI2CCON)->PEN = 1;   //Put STOP on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                break;
            }
            //I2C1TRN = c;                //Put address byte on I2C bus
            *((BYTE*)I2CTRN_ARR[objI2C->flags.ch]) = c;   //Put address byte on I2C bus
            objI2C->isrAdr = c;              //Save address
            objI2C->isrRead = 0;             //Initialize objI2C->isrRead with 0 - indicates that we are NOT reading
            objI2C->isrRdDest = 0;
            objI2C->isr.bits.sAddress = 1; //Mark that Address following START was just sent
            objI2C->isr.bits.isrTxionStatus = 0;
            #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
            DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'x');
            DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, c);
            DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');          //Print tick value
            DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, tick16Get());
            #endif

            objI2C->isr.bits.sm=SM_ISR_MASTER_TX;  //Done below when we continue to SM_ISR_MASTER_TX state
            break;
        }
        case SM_ISR_REPEATED_START: //Just finished sending REPEATED START bit
        {
            #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c RS");
            #endif
            //We sent a REPEATED start as part of a read operation. In this case we use the saved address
            if (objI2C->isrRead!=0) {
                //Put saved address on I2C bus, with R/W bit set = read
                //I2C1TRN = objI2C->isrAdr | 0x01;
                *((BYTE*)I2CTRN_ARR[objI2C->flags.ch]) = objI2C->isrAdr | 0x01;
                #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'x');
                DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, (objI2C->isrAdr|0x01));
                #endif
            }
            //The following byte MUST be the address
            else {
                //ERROR! Next byte in buffer is a control character!
                if (cbufGetEscapedByte(objI2C->pCbufTx, &c)) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: NoAdr");    //No address found to send after REPEATED START
                    //Remove all bytes till stop character. No need to do this, main task will remove all bytes till it finds a Start character.
                    //Reset bus, put Stop
                    objI2C->isr.bits.sm=SM_ISR_STOP;
                    objI2C->isr.bits.isrTxionStatus = I2C_TXION_STATUS_ERR_BUF_FRMT;  //Buffer format error
                    ((I2C1CONBITS*)pI2CCON)->PEN = 1;   //Put STOP on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                    break;
                }
                //I2C1TRN = c;                //Put byte on I2C bus
                *((BYTE*)I2CTRN_ARR[objI2C->flags.ch]) = c;   //Put byte on I2C bus
                #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'x');
                DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, c);
                #endif
            }
            objI2C->isr.bits.sm=SM_ISR_MASTER_TX;
            break;
        }
        case SM_ISR_MASTER_TX:  // Just finished sending a byte
        {
            //ERROR!!!! NO ACK received
            //if (I2C1STATbits.ACKSTAT == TRUE) {
            if (((I2C1STATBITS*)pI2CxSTAT(objI2C->flags.ch))->ACKSTAT == TRUE) {    //cast to I2C1STATBITS, because all I2CxSTATBITS vars have the same bits!
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: TX NoACK");

                //No ACK received from slave for Address or Data byte sent
                c = objI2C->isr.bits.sAddress ? I2C_TXION_STATUS_ERR_ADR_NOACK : I2C_TXION_STATUS_ERR_DAT_NOACK;

                //Remove all bytes till stop character. No need to do this, main task will remove all bytes till it finds a Start character.

                SM_ISR_MASTER_TX_PUTSTOP:
                objI2C->isr.bits.isrTxionStatus = c;
                //Reset bus, put Stop
                objI2C->isr.bits.sm=SM_ISR_STOP;
                ((I2C1CONBITS*)pI2CCON)->PEN = 1;   //Put STOP on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                goto SM_ISR_MASTER_TX_END;          //End and break;
            }

            //We just sent the address for a START (REPEATED START), and now have to switch to READING!
            if (objI2C->isrRead!=0) {
                #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c RXing");
                #endif
                ((I2C1CONBITS*)pI2CCON)->RCEN = 1;  //Read a byte (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                objI2C->isr.bits.sm=SM_ISR_MASTER_RX;
                goto SM_ISR_MASTER_TX_END;              //End and break;
            }

            //Next entry in buffer is an Escaped Byte ("Escape Character" followed by "Control Character")
            //Next byte is the Escape Character. An Escaped Character (Control Character) will follow it.
            objI2C->isr.bits.b = 0;
            if (cbufPeekEscapedByte(objI2C->pCbufTx, &c))  {
                objI2C->isr.bits.b = 1;              //Indicate c contains a 'Control Character'
                cbufRemoveBytes(objI2C->pCbufTx, 2); //c already contains byte. Remove the Escaped Byte (represented using 2 bytes in buffer)
            }
            //Next character is NOT a 'Control Character'
            else {
                //Shorthand for reading WITHOUT using 'r' control character! When address following START condition is even(READ).
                //For example: "^,s,0xDF,0x01,^,p" is the same(shorthand) as "s,0xDF,^,r,0x01,^,p" - Simulate a 'Control Character'!
                if (objI2C->isr.bits.sAddress && (objI2C->isrAdr&0x01)) {
                    #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n-(r)-");
                    #endif
                    //Simulate a 'r' control character! Don't remove anything from buffer! Next char is number of bytes to read
                    c = 'r';
                    objI2C->isr.bits.b = 1;    //Indicate c contains a 'Control Character'
                }
                //Next character must be transmitted
                else {
                    //Get & remove next character to transmit, c alread contains it!
                    //If c = "Escape Character", have to remove 2 characters !!!
                    cbufRemoveBytes(objI2C->pCbufTx, c==cbufGetEscapeCharacter(objI2C->pCbufTx)?2:1);
                    //ALTERNATIVE METHOD
                    //cbufGetEscapedByte(objI2C->pCbufTx, &c);
                }
            }

            //A 'Control Character' was read, process it:
            // - '^^' = '^' character (double '^' character represents a single '^' character)
            // - ^s = Put start on bus. A second S character (anywhere in string) before P character will cause a repeated Start condition.
            // - ^p = Put stop on bus
            // - ^rn = Read n number of bytes, where n is a value from 0-254. A value of 255 means read till slave does not ACK
            //
            // For example, { '^', 's', 0x50, 0x8A, 'H', 'e', 'l', 'l', 'o', '^', 'p' }  will put a Start condition on bus, Send 0x50 hex byte,
            // Send 0x8A hex byte, Send "Hello" string, put Stop condition on bus.
            //
            // If the buffer is in Packet mode, the Start (^s) and Stop (^p) escape sequences are not required, seeing that each
            // packet is automatically precessed by a Start Condition, and ended with a Stop Condition.
            // For example (buffer Packet type), { 0x50, 0x8A, 'H', 'e', 'l', 'l', 'o' }, will put a Start condition on bus, Send 0x50
            // hex byte, Send 0x8A hex byte, Send "Hello" string, put Stop condition on bus.
            if (objI2C->isr.bits.b==1) {
                #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-Done");
                #endif
                //Repeated Start - ALWAYS use same address as was used (and saved in objI2C->isrAdr)
                if (c=='s') {
                    ((I2C1CONBITS*)pI2CCON)->RSEN = 1;          //Put Repeated START on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                    objI2C->isr.bits.sm=SM_ISR_REPEATED_START;    //Set next state to SM_ISR_REPEATED_START
                    goto SM_ISR_MASTER_TX_END;                  //End and break;
                }
                //Last byte of message just send on I2C bus
                else if (c == 'p') {
                    c = I2C_TXION_STATUS_OK;        //Put isrTxionStatus in c
                    goto SM_ISR_MASTER_TX_PUTSTOP;  //Put STOP and end communication with I2C slave
                }
                //Read data - format "^sXX^Rnbbp" where XX=Address, n=number of bytes to read, bb=pointer
                else if (c == 'R') {
                    //There should still be at lease 5 bytes in the buffer (size, pointer, Control Character and STOP). If not, ERROR!!!
                    if (cbufGetCount(objI2C->pCbufTx)<5) {
                        c = I2C_TXION_STATUS_ERR_BUF_FRMT;  //Put isrTxionStatus in c
                        goto SM_ISR_MASTER_TX_PUTSTOP;      //Put STOP and end communication with I2C slave
                    }
                    objI2C->isr.bits.wrDstType = RD_DEST_TYPE_POINTER;  //Indicate that ic21RdDest contains BYTE pointer to write read byte to
                    //Get number of bytes to read
                    cbufGetEscapedByte(objI2C->pCbufTx, &(objI2C->isrRead));
                    //Get pointer = WORD (2 bytes)
                    cbufGetEscapedByte(objI2C->pCbufTx, ((BYTE*)&objI2C->isrRdDest)+1 );  //Get next Escaped Byte
                    cbufGetEscapedByte(objI2C->pCbufTx, ((BYTE*)&objI2C->isrRdDest) );    //Get next Escaped Byte
                    //ALTERNATIVE METHOD
                    //cbufGetEscapedByte(objI2C->pCbufTx, &c); //Get next Escaped Byte
                    //nzWordSetHighByte_MACRO(objI2C->isrRdDest, c);
                    //cbufGetEscapedByte(objI2C->pCbufTx, &c); //Get next Escaped Byte
                    //nzWordSetLowByte_MACRO(objI2C->isrRdDest, c);

                    #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c R");
                    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, objI2C->isrRead);
                    DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, objI2C->isrRdDest);
                    #endif

                    //If preceded by START with even address, start reading immediately. Else, send "Repeated START" and then start reading.
                    goto SM_ISR_MASTER_TX_RS_OR_READ;
                }
                #if !defined(I2C_DONT_SUPPORT_LISTENERS)
                //Read data - format "^sXX^Lnap" where XX=Address, n=number of bytes to read, a=OR'ed Listeners (8-bit byte)
                else if (c == 'L') {
                    //There should still be at lease 5 bytes in the buffer (size, pointer, Control Character and STOP). If not, ERROR!!!
                    if (cbufGetCount(objI2C->pCbufTx)<5) {
                        c = I2C_TXION_STATUS_ERR_BUF_FRMT;  //Put isrTxionStatus in c
                        goto SM_ISR_MASTER_TX_PUTSTOP;      //Put STOP and end communication with I2C slave
                    }

                    objI2C->isr.bits.wrDstType = RD_DEST_TYPE_LISTENERS;    //Indicate that isrRdDest contains OR'ed bits for "Circular Buffer" listeners
                    objI2C->isrRdDest = 0x01;                               //Default "Circular Buffer" = 0x01, bits 2 to 15 contain additional ones

                    //Get number of bytes to read
                    cbufGetEscapedByte(objI2C->pCbufTx, &(objI2C->isrRead));
                    //Get OR'ed Listeners (8-bit byte)
                    cbufGetEscapedByte(objI2C->pCbufTx, (BYTE*)(&(objI2C->isrRdDest)) );
                    #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-L");
                    DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, objI2C->isrRdDest);
                    #endif

                    //Check if valid listeners .....
                    //if (objI2C->isrRdDest .....) {
                    //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "....");
                    //    goto SM_ISR_MASTER_TX_PUTSTOP;      //Put STOP and end communication with I2C slave
                    //}

                    //If preceded by START with even address, start reading immediately. Else, send "Repeated START" and then start reading.
                    goto SM_ISR_MASTER_TX_RS_OR_READ;
                }
                #endif  //#if !defined(I2C_DONT_SUPPORT_LISTENERS)
                //Read data - format:
                // - Read to default "Circular Buffer": ^sXX^rnp" where XX=Address, n=number of bytes to read
                // - Read to "User Ram":                ^sXX^rn^uap" where XX=Address, n=number of bytes to read, a="User Ram" address (8-bit byte)
                // - Read to "Circular Buffer":         ^sXX^rn^ts" where XX=Address, n=number of bytes to read, 's' is the "Serial Data Port" number.
                //   Upper Cast control 'L' is used so that it can NOT be sent via Ascii (only lower cast command possible)
                else if (c == 'r') {
                    //Get number of byets to read
                    cbufGetEscapedByte(objI2C->pCbufTx, &(objI2C->isrRead));    //Setting objI2C->isrRead with NON 0 indicates that we are reading
                    #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c R");
                    DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, objI2C->isrRead);
                    #endif

                    //If 0, put STOP and end
                    if (objI2C->isrRead==0) {
                        c = I2C_TXION_STATUS_OK;        //Put isrTxionStatus in c
                        goto SM_ISR_MASTER_TX_PUTSTOP;  //Put STOP and end communication with I2C slave
                    }

                    //Initialize with default Receive Circular Buffer
                    objI2C->isr.bits.wrDstType = RD_DEST_TYPE_LISTENERS;    //Indicate that isrRdDest contains OR'ed bits for "Circular Buffer" listeners
                    objI2C->isrRdDest = 0x01;                               //Default "Circular Buffer" = 0x01, bits 2-7 are additional registered listeners

                    //Removed this "remove till STOP" below. Takes lots of time in ISR! Is done anyway in serI2CTask()
                    //Get next byte from buffer, until we remove STOP "control character"
                    //do {
                        //Get next character from buffer, should be a "control character"
                        if (cbufGetEscapedByte(objI2C->pCbufTx, &c)) {
                            #if (USER_RAM_SIZE > 0)
                            //Write reply to "user memory" with given address
                            if (c == 'u') {
                                objI2C->isr.bits.wrDstType = RD_DEST_TYPE_POINTER;  //Indicate that ic21RdDest contains BYTE pointer to write read byte to
                                //Get address of "User Memory"
                                objI2C->isrRdDest = 0;
                                cbufGetEscapedByte(objI2C->pCbufTx, (BYTE*)(&(objI2C->isrRdDest)) );
                                #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-U");
                                DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, objI2C->isrRdDest);
                                #endif

                                //Check in bounds of nzGlobals.userRam[USER_RAM_SIZE] array
                                if ((objI2C->isrRdDest+objI2C->isrRead) > USER_RAM_SIZE) {
                                    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "-OutOfBound");
                                    c = I2C_TXION_STATUS_ERR_BUF_FRMT;  //Put isrTxionStatus in c
                                    goto SM_ISR_MASTER_TX_PUTSTOP;      //Put STOP and end communication with I2C slave
                                }

                                //Get pointer to location in "User Memory" where requested bytes must be written
                                //No need to check if requested "User Memory" is within avaialable array, is done in SM_ISR_MASTER_RX state!
                                objI2C->isrRdDest = ((WORD)&nzGlobals.userRam[objI2C->isrRdDest]);
                            }
                            //Write reply to "Serial Data Port" Circular Buffer with given number
                            else if (c == 't') {
                                objI2C->isr.bits.wrDstType = RD_DEST_TYPE_CIRBUF;   //Indicate that isrRdDest contains "Serial Data Port" number
                                //Get "Serial Data Port" number in lower byte of isrRdDest. isrRdDest is a WORD, but cbufGetEscapedByte() writes lower byte only!
                                objI2C->isrRdDest = 0;
                                cbufGetEscapedByte(objI2C->pCbufTx, (BYTE*)(&(objI2C->isrRdDest)) );
                                #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-T");
                                DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, objI2C->isrRdDest);
                                #endif

                                //Check max "Serial Data Port" number
                                if (objI2C->isrRdDest > (SERPORT_ID_MAX)) {
                                    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "-OutOfBound");
                                    c = I2C_TXION_STATUS_ERR_BUF_FRMT;  //Put isrTxionStatus in c
                                    goto SM_ISR_MASTER_TX_PUTSTOP;      //Put STOP and end communication with I2C slave
                                }

                                //Get pointer to "Circular Buffer" for given "Serial Data Port"
                                // TODO! Finish this!!
                                objI2C->isrRdDest = ((WORD)&nzGlobals.userRam[objI2C->isrRdDest]);
                            }
                            #endif
                        }
                    //Removed this "remove till STOP" below. Takes lots of time in ISR! Is done anyway in serI2CTask()
                    //} while(c!='p');

                    SM_ISR_MASTER_TX_RS_OR_READ:
                    //The 'r' or 'R' control character was preceded by START (no data bytes) with Even address(READ). NO repeated START is sent! We start reading immediately
                    if (objI2C->isr.bits.sAddress && (objI2C->isrAdr&0x01)) {
                        #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c RXing");
                        #endif
                        ((I2C1CONBITS*)pI2CCON)->RCEN = 1;      //Read a byte (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                        objI2C->isr.bits.sm=SM_ISR_MASTER_RX;
                        goto SM_ISR_MASTER_TX_END;              //End and break;
                    }
                    //Else, we have to send a REPEATED START. The 'r' or 'R' control character was preceded by START and data byte(s)
                    ((I2C1CONBITS*)pI2CCON)->RSEN = 1;          //Put Repeated START on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                    objI2C->isr.bits.sm=SM_ISR_REPEATED_START;   //Set next state to SM_ISR_REPEATED_START
                    goto SM_ISR_MASTER_TX_END;                  //End and break;
                }
            }

            //Put byte on I2C bus
            #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c TXx");
            DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, c);
            #endif
            //I2C1TRN = c;
            *((BYTE*)I2CTRN_ARR[objI2C->flags.ch]) = c;

            SM_ISR_MASTER_TX_END:
            objI2C->isr.bits.sAddress = 0;
            break;
        }
        case SM_ISR_STOP:  //Just finished sending STOP bit
        {
            #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c P");
            #endif
            //Add status for current address to Address-Status Map
            statusMapPut(objI2C, objI2C->isrAdr, objI2C->isr.bits.isrTxionStatus);

            objI2C->isrCnt++;                 //Incremented for each STOP sent
            objI2C->isr.bits.sm=SM_ISR_IDLE;  //Reset state machine to idle
            break;
        }
        case SM_ISR_MASTER_RX:  //Just finished reading a byte
        {
            //c = I2C1RCV;    //Get Read byte
            c = *((BYTE*)I2CRCV_ARR[objI2C->flags.ch]);   //Get read byte
            #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c RX=x");
            DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, c);
            #endif

            //Still more to read - send ACK
            if (--objI2C->isrRead) {
                ((I2C1CONBITS*)pI2CCON)->ACKDT = 0; //Put ACK on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
            }
            //Finised reading all bytes - send NACK
            else {
                ((I2C1CONBITS*)pI2CCON)->ACKDT = 1; //Put NACK on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
            }
            objI2C->isr.bits.sm=SM_ISR_MASTER_ACK;
            ((I2C1CONBITS*)pI2CCON)->ACKEN = 1;     //Put ACK or NACK on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)

            //Received byte must be written to listeners given by bits of objI2C->isrRdDest (each bit is a listener)
            //if (objI2C->isr.bits.wrDstType==RD_DEST_TYPE_LISTENERS) {
            if (objI2C->isr.wrDstType.bit1 == 0) {
                //Bit 0 is default I2C Receive Circular Buffer
                if(objI2C->isrRdDest&0x01) {
                    if (cbufPutByte(objI2C->pCbufRx, c) == 0) {
                        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "-ERR RxBuf Full!");
                    }
                }
                #if !defined(I2C_DONT_SUPPORT_LISTENERS)
                {
                    BYTE i;
                    //Also add received byte to all other registered listeners
                    for (i=0; i<I2C_LISTENERS; i++) {
                        if( (objI2C->isrRdDest&(0x02<<i)) != 0) {
                            if (cbufPutByte(i2cListeners[i], c) == 0) {
                                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "-ERR RxBuf Full!");
                            }
                        }
                    }
                }
                #endif
            }
            //Received byte must be written to "Circular Buffer" --OR-- "BYTE* Pointer"
            else {
                //Pointer is contained in objI2C->isrRdDest
                if (objI2C->isr.wrDstType.bit0 == 0) {

                }
                //Circular Buffer Pointer is contained in objI2C->isrRdDest
                else {

                }
                *((BYTE*)objI2C->isrRdDest++) = c;
            }
            break;
        }
        case SM_ISR_MASTER_ACK: //Just finished sending an ACK or NACK.
        {
            //Just sent an ACK - this means we NOT done reading yet! Read next byte!
            if (objI2C->isrRead != 0) {
                #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c ACK");
                #endif
                ((I2C1CONBITS*)pI2CCON)->RCEN = 1;  //Read a byte (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                objI2C->isr.bits.sm=SM_ISR_MASTER_RX;
            }
            //Just sent an NACK - this means we are finished! Put STOP condition
            else {
                #ifdef I2C1_ISR_DETAILED_DEBUG_INFO
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2c NACK");
                #endif
                ((I2C1CONBITS*)pI2CCON)->PEN = 1;   //Put STOP on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                objI2C->isr.bits.isrTxionStatus = I2C_TXION_STATUS_OK;
                objI2C->isr.bits.sm=SM_ISR_STOP;
            }
            break;
        }
    }
#elif defined(__PIC32MX__)
#endif
}

//#define I2C1_TASK_DETAILED_DEBUG_INFO

/**
 * I2C Task
 */
void serI2CTask(I2C_INFO* objI2C)
{
#if defined(__C30__)
    BOOL bDone;
    BYTE c;
    BYTE count;

    volatile unsigned int* pI2CCON;
    //pI2CCON = getI2CCON(objI2C->flags.ch);
    pI2CCON = pI2CxCON(objI2C->flags.ch);

    //Use objI2C->isrRead for searchOffset to save RAM. Can be done seeing that objI2C->isrRead is only used in ISR, which is NOT running when this function is executed!
    //static BYTE searchOffset;
    #define searchOffset objI2C->isrRead

    //Use objI2C->isrAdr for escFound to save RAM. Can be done seeing that objI2C->isrAdr is only used in ISR, which is NOT running when this function is executed!
    //static BYTE escFound;
    #define escFound objI2C->isrAdr


    //Check receive "Circular Buffer" format is "Binary, with Escape Sequence Processing"
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
    if (cbufGetFormat(objI2C->pCbufRx) != BUFFORMAT_BIN_ESC) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "I2C NOT BinEsc!");
        return;
    }
    #endif

    bDone = TRUE;
    do {
    switch(objI2C->taskSm) {
        case SM_I2C_TASK_IDLE:
            //Ensure ISR is in idle state! We can not modify TXBUF if ISR is also modifying it!
            if (objI2C->isr.bits.sm != SM_ISR_IDLE) {
                if (tick16TestTmr(objI2C->timeout)) {
                    objI2C->timeout = tick16Get() + tick16ConvertFromMS(I2C_TIMEOUT);
                    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nI2C Timeout!");
                    DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');          //Print tick value
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, tick16Get());

                    i2cWriteMI2CIE(objI2C->flags.ch, 0);//Disable I2C interrupt
                    ((I2C1CONBITS*)pI2CCON)->I2CEN = 0; //Disable I2C Module (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)

                    objI2C->isr.bits.isrTxionStatus = I2C_TXION_STATUS_ERR_TIMEOUT;
                    objI2C->isr.bits.sm = SM_ISR_STOP;  //Set ISR state machine to STOP condition

                    ((I2C1CONBITS*)pI2CCON)->I2CEN = 1; //Enable I2C Module (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                    i2cWriteMI2CIE(objI2C->flags.ch, 1);//Enable I2C interrupt

                    ((I2C1CONBITS*)pI2CCON)->PEN = 1;   //Put STOP on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
                }
                return;
            }

            nzGlobals.wdtFlags.bits.serI2C = 1;     //Indicate that this module is still alive, and clear WDT

            //Check for locked I2C bus
            i2cCheckBus(objI2C->flags.ch);

            //Message must start with "Escape Character", followed by START "Control Character", remove all other data
            while (cbufHasData(objI2C->pCbufTx)) {
                //If next character in buffer is NOT the "Escape Character", remove it!
                if (cbufPeekByte(objI2C->pCbufTx) != cbufGetEscapeCharacter(objI2C->pCbufTx)) {
                    cbufRemoveByte(objI2C->pCbufTx);
                    continue;
                }

                //If second character in buffer is NOT 's' (START "Control Character"), remove both characters
                if ( (cbufGetCount(objI2C->pCbufTx) > 1) && (cbufPeekByteAt(objI2C->pCbufTx, 1) != 's') ) {
                    cbufRemoveBytes(objI2C->pCbufTx, 2);
                    continue;
                }

                //At this stage we know the first two bytes in the buffer is the "Escape Character", followed
                //by the START "Control Character". Remove them!
                cbufRemoveBytes(objI2C->pCbufTx, 2);

                #if (I2C_ALWAYS_ADD_WHOLE_MESSAGE == 1)
                    //A whole message is always added to the buffer. This will saves
                    //lots of time and processing searching buffer for stop character.
                    objI2C->taskSm = SM_I2C_TASK_FOUND_MSG;
                    bDone = FALSE;  //Not done, have to send START byte of I2C message
                    break;
                #else
                    objI2C->taskSm = SM_I2C_TASK_SEARCH_STOP;
                    //Timeout for SM_I2C_TASK_SEARCH_STOP state
                    objI2C->timeout = tick16Get() + tick16ConvertFromMS(I2C_SEARCH_STOP_TIMEOUT);
                    #ifdef I2C1_TASK_DETAILED_DEBUG_INFO
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nI2C Removed S");
                    #endif
                    bDone = FALSE;              //Not done, have to search for stop condition next
                    searchOffset = 0;           //Initiailize searching from beginning of buffer
                    escFound = 0;

                    break;                      //Break out of while() loop
                #endif
            }
            break;
#if (I2C_ALWAYS_ADD_WHOLE_MESSAGE == 0)
        case SM_I2C_TASK_SEARCH_STOP:
            bDone = TRUE;
            //Buffer empty, Nothing to do
            if (cbufIsEmpty(objI2C->pCbufTx))
                return;

            //Timeout check! If stuck in this state for longer than timeout, remove content of buffer
            //until a START 'Control Character' is found.
            if (tick16TestTmr(objI2C->timeout)) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nI2C Search STOP Timeout!");
                //By removing a single byte from the buffer (will be START), the SM_I2C_TASK_IDLE state will
                //remove all bytes until next START 'Control Character' is found.
                cbufRemoveByte(objI2C->pCbufTx);
                objI2C->taskSm = SM_I2C_TASK_IDLE;
                break;
            }

            //Peek in transmit buffer, and only start I2C communication once a whole I2C message is available. That is, [Start][Adr][Data][Stop].
            //We thus have to peek for the "Escape Character", followed by the 'p' command = end of I2C message
            count = cbufGetCount(objI2C->pCbufTx);
            while(searchOffset < count) {
                c = cbufPeekByteAt(objI2C->pCbufTx, searchOffset);
                searchOffset++;
                //Reset Timeout for each new character
                objI2C->timeout = tick16Get() + tick16ConvertFromMS(I2C_SEARCH_STOP_TIMEOUT);

                //Search for escape character
                if ((escFound==0) && (c == cbufGetEscapeCharacter(objI2C->pCbufTx))) {
                    escFound=1;
                    continue;
                }
                //Last character was Escape Character, look for 'p'
                else if (escFound==1) {
                    //Found a stop character
                    if (c == 'p') {
                        escFound = 2;    //Indicate that 'p' character was found
                        objI2C->taskSm = SM_I2C_TASK_FOUND_MSG;
                        break;
                    }
                    escFound=0;     //Reset to look for escape
                }
            }
            //If 'p' not found, return
            if (escFound != 2) {
                return;
            }
            #ifdef I2C1_TASK_DETAILED_DEBUG_INFO
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nI2C Fnd Msg");
            DEBUG_PUT_BYTE(DEBUG_LEVEL_INFO, cbufGetCount(objI2C->pCbufTx));
            #endif

            //break;        //No break, goes to next state IF 'p' (Stop) founct = have whole message
#endif  //#if (I2C_ALWAYS_ADD_WHOLE_MESSAGE == 0)
        case SM_I2C_TASK_FOUND_MSG:
            i2cWriteMI2CIE(objI2C->flags.ch, 0);      //Disable I2C interrupt

            //I2C ISR state machine is currently idle
            if (objI2C->isr.bits.sm == SM_ISR_IDLE) {
                objI2C->isr.bits.sm = SM_ISR_START;   //Set ISR state machine to START condition
                objI2C->taskSm = SM_I2C_TASK_IDLE;    //Main state machine is now idle too
                objI2C->timeout = tick16Get() + tick16ConvertFromMS(I2C_TIMEOUT); //ISR Timeout

                i2cWriteMI2CIE(objI2C->flags.ch, 1);  //Enable I2C interrupt
                //Kick start I2C ISR
                ((I2C1CONBITS*)pI2CCON)->SEN = 1;   //Put START on bus (cast to I2C1CONBITS, because all I2CnCONBITS vars have the same bits!)
            }
            //NOT idle, enable interrupt and exit! Will return later and check if idle!
            else {
                i2cWriteMI2CIE(objI2C->flags.ch, 1);  //Enable I2C interrupt
            }
            break;
    };  //switch(objI2C->taskSm)
    } while(!bDone);
#elif defined(__PIC32MX__)
#endif
}

#if !defined(I2C_DONT_SUPPORT_LISTENERS)
/**
 * Register given "Circular Buffer" as a listener. After doing this, the returned
 * "Listener ID" can be used as a parameter with certian I2C functions that return data.
 * Multiple "Listernet IDs" can be OR'ed together.
 *
 * If the given "Circular Buffer" is already registered, it will not register it
 * again. It will simply return it's "Listener ID".
 *
 * @param pBuf Pointer to "Circular Buffer"
 *
 * @return Returns "Listener ID", or 0 if error. The most likely cause of an error is
 *         that there is no more space available for registering listeners.
 */
BYTE serI2CRegisterListener(CIRBUF* pBuf) {
    BYTE i;
    BYTE listenerID = 0x02; //Start at 0x02, 0x01 is reserved for default listener

    //Add listener to first empty spot, or return ID if already registered
    for (i=0; i<I2C_LISTENERS; i++) {
        //Current location is available, add this "Circular Buffer" to this position
        if (i2cListeners[i] == 0) {
            i2cListeners[i] = pBuf;

            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nReg Lstnr = ");
            DEBUG_PUT_BYTE(DEBUG_LEVEL_INFO, listenerID);

            return listenerID;
        }

        //Alread registered
        if (i2cListeners[i] ==  pBuf) {
            return listenerID;
        }

        //Increment and loop
        listenerID = listenerID << 1;
    }

    //Error, no space left!
    return 0;
}
#endif

/**
 * Initialization code.
 */
void serI2CInit(void)
{
    #if !defined(I2C_DONT_SUPPORT_LISTENERS)
    //Initialize whole structure with 0
    memset(&i2cListeners, 0, sizeof(i2cListeners));
    #endif
}


/**
 * Enable or Disable interrupt
 *
 * @param ch Channel offset, 0=I2C 1, 1=I2C2, 2=I2C 3
 * @param val 0 or 1
 */
void i2cWriteMI2CIE(BYTE ch, BOOL val) {
#if defined(__C30__)
    if (ch==0)
        _MI2C1IE = val;
    else if (ch==1)
        _MI2C2IE = val;
    else if (ch==2)
        _MI2C3IE = val;
#elif defined(__PIC32MX__)
#endif
}


/**
 * Check I2C Bus. For example, check if any slaves are locked, and pulling SDA low.
 *
 * @param ch Channel offset, 0=I2C 1, 1=I2C2, 2=I2C 3
 *
 * @ret Returns I2C_CHECKBUS_xxx code. Returns 0 if OK, else error code:
 *      - I2C_CHECKBUS_OK
 *      - I2C_CHECKBUS_SCL_LOW = Bus still pulled low, could not fix error!
 *      - I2C_CHECKBUS_INVALID_CH = Parameter ch not valid
 */
BYTE i2cCheckBus(BYTE ch) {
    BYTE i;
    BYTE ioportSDA = IOPORT_ID_NA;
    BYTE ioportSCL = IOPORT_ID_NA;
    WORD bitadrLatSCL;
    WORD bitadrPinSDA;
    WORD bitadrTrisSCL;

    // For testing, for I2C1 only
//    i = 100;
//    DIR_SDA1 = INPUT_PIN;   //Configure SDA as input
//    while ((PIN_SDA1 == 0) && (--i!=0)) {
//        DIR_SCL1 = OUTPUT_PIN;
//        delay_us(5);    //Allow for 100kHz devices (10us = 5 high/5 low)
//        LAT_SCL1 = !LAT_SCL1;
//    }
//    LAT_SCL1 = 1;           //Ensure SCL is set
//    DIR_SCL1 = INPUT_PIN;   //Restore SCL1 to input pin
//
//    return I2C_CHECKBUS_OK; //Success

#if defined(PORT_ID_SDA1)
    if(ch==0) {
        ioportSDA = PORT_ID_SDA1;
        ioportSCL = PORT_ID_SCL1;
    }
#endif
#if defined(PORT_ID_SDA2)
    if(ch==1) {
        ioportSDA = PORT_ID_SDA2;
        ioportSCL = PORT_ID_SCL2;
    }
#endif
#if defined(PORT_ID_SDA3)
    if(ch==2) {
        ioportSDA = PORT_ID_SDA3;
        ioportSCL = PORT_ID_SCL3;
    }
#endif

    if (ioportSDA == IOPORT_ID_NA) {
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ni2cCheckBus Invalid CH!");
        return I2C_CHECKBUS_INVALID_CH;
    }

    //Get bitadr for SCL Pin
    bitadrPinSDA = portGetPIN(ioportSDA);
    bitadrLatSCL = portGetLAT(ioportSCL);
    bitadrTrisSCL = portGetTRIS(ioportSCL);
    //bitadr = TRIS_PORTID_BITADR_MAP[ioportSCL]; //Alternative method

    portSetBitadr(portGetTRIS(ioportSDA));  //Configure SDA as input - set TRIS configures as input

    //Check for locked I2C bus. This is the case if a slave is pulling the SDA line low.
    //Check if SDA is low. If it is, toggle SCK until slave releases bus. A stop condition
    //is when SDA is low, and SCK is toggled low-to-hight.
    //For details, see:
    //http://processors.wiki.ti.com/index.php/I2C_Tips#External_Slave_Device_Hanging_the_Bus_by_Holding_SDA_Low
    //Put stop condition on bus
    i = 100;
    while ((portReadBitadr(bitadrPinSDA) == 0) && (--i!=0)) {
        //Configure SCL as output - clear TRIS configures as output
        portClearBitadr(bitadrTrisSCL);

        delay_us(3);    //Allow for 100kHz devices (min 10us, we have 12us = 6 high/6 low)

        //Toggle SLC line
        //portWriteBitadr(bitadrLatSCL, !portReadBitadr(bitadrPinSCL));
        if (portReadBitadr(bitadrLatSCL))
            portClearBitadr(bitadrLatSCL);
        else
            portSetBitadr(bitadrLatSCL);

        delay_us(3);
    }

    if (i!=100) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nI2C Lockup occurred!");
    }

    //Ensure SCL is set
    portSetBitadr(bitadrLatSCL);

    //Restore SCL to input pin - set TRIS configures as input
    portSetBitadr(bitadrTrisSCL);

    return I2C_CHECKBUS_OK; //Success
}


/**
 * Gets a pointer to the I2C_INFO structure for the given bus number.
 *
 * @param i2cBus I2C Bus number. Must be an I2C bus that is available, for example
 *        if the board has an I2C 1 and I2C 2 bus, it must be 1 or 2.
 *
 * @return Returns a pointer to the I2C_INFO structure for the given bus number.
 *         Return 0 if error, or given bus number is not available.
 */
I2C_INFO* i2cGetInfoForBus(BYTE i2cBus) {
#if defined(HAS_SERPORT_I2C1)
    if (i2cBus==1)
        return &i2c1Info;
#endif
#if defined(HAS_SERPORT_I2C2)
    if (i2cBus==2)
        return &i2c2Info;
#endif
#if defined(HAS_SERPORT_I2C3)
    if (i2cBus==3)
        return &i2c3Info;
#endif
    return 0;   //ERROR
}


BYTE i2cGetStatus(I2C_INFO* objI2C, BYTE adr)
{
    //This function does NOT check if a transmission is currently in progress! If the status of a
    //current transmission should be obtained, use the i2cIsBusy() or i2cGetStatusWait() functions!
    //Check if we are currently busy transmitting or receiving!
    //if (objI2C->isr.bits.sm != SM_ISR_IDLE) {
    //    return I2C_TXION_STATUS_NOT_AVAILABLE;
    //}

    //If given address = 0, return last status
    if (adr==0) {
        //If current transmission encountered an error while adding message to "Circular Buffer",
        //the ISR would NOT have attempted to transmit it! In this case, return this error
        if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nRet last non-ISR Stat");
            return objI2C->flags.currTxionStatus;
        }

        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nRet last ISR Stat");

        //The message was transmitted by the ISR, and the status message will be in the
        //adr-status map (status cache). Get and remove it from cache.

        //Get and remove status from I2C Status Cache
        return i2cGetStatus(objI2C, objI2C->adr);

        //The status is also in isrTxionStatus. But, just returning this will NOT remove the
        //status from the status cache!
        //return  objI2C->isr.bits.isrTxionStatus;
    }

    //If current transmission encountered an error while adding message to "Circular Buffer",
    //the ISR would NOT have attempted to transmit it! In this case, we can NOT return the ISR
    //status, seeing that the ISR would not have been executed for this message!
    //Check if the given address is equal to this failed address, and if true, return status.
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        if (objI2C->adr == adr) {
            return objI2C->flags.currTxionStatus;
        }
    }

    return statusMapGet(objI2C, adr);
}

BYTE i2cGetStatusWait(I2C_INFO* objI2C, BYTE adr) {
    while(i2cIsBusy(objI2C)){
        serI2CTask(objI2C);
    };
    return i2cGetStatus(objI2C, adr);
}

#if defined I2C_USE_SHARED_FUNCTIONS
BYTE i2cIsBusy(I2C_INFO* objI2C) {
    //Return TRUE if the ISR is NOT in the idle state!
    return (objI2C->isr.bits.sm != SM_ISR_IDLE);
}
#endif


#if defined I2C_USE_SHARED_FUNCTIONS
BYTE i2cWriteMessage(I2C_INFO* objI2C, BYTE adr, BYTE* msg, WORD size) {
    objI2C->adr = adr; //Save address

    if (i2cBeginTransmission(objI2C, adr) == 0) {
    //ALTERNATIVE METHOD! i2cBeginTransmission() does size check, and writes STOP and address
//    objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_OK;  //Currently OK, have not finished adding whole message to buffer
//    //Check if there is enough space in the buffer. Just an ESTIMATE!!!! We do not take check if any
//    //bytes given in msg need escaping! This is done in i2cWriteArray() function. Add 6 bytes for address, "Escaped S" and "Escaped P", possible escape character.
//    if (cbufGetFree(objI2C->pCbufTx) > (size + 6)) {
//        //Put "Control Character" and START
//        cbufPutByteNoCheck(objI2C->pCbufTx, cbufGetEscapeCharacter(objI2C->pCbufTx));
//        cbufPutByteNoCheck(objI2C->pCbufTx, 's');
//
//        //Write address
//        cbufPutEscapedByte(objI2C->pCbufTx, objI2C->adr);

        //Write given array
        if (i2cWriteArray(objI2C, msg, size) == 0) {
            return i2cEndTransmission(objI2C);
            
            //ALTERNATIVE METHOD below, not using i2cEndTransmission(). A bit faster
            //Put "Control Character" and STOP
            //cbufPutByteNoCheck(objI2C->pCbufTx, cbufGetEscapeCharacter(objI2C->pCbufTx));
            //cbufPutByteNoCheck(objI2C->pCbufTx, 'p');
            //Start transmission
            //serI2CTask(objI2C);
        }
    }

    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nI2C ERR: No space!");
    objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
    return I2C_TXION_STATUS_ERR_NOSPACE;
}
#endif

#if defined I2C_USE_SHARED_FUNCTIONS
BYTE i2cBeginTransmission(I2C_INFO* objI2C, BYTE adr) {
    objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_OK;  //Currently OK, have not finished adding whole message to buffer
    objI2C->adr = adr;                                     //Save address

    //If not enough space for at least "START, (escaped) adr, STOP", error! START and STOP require 2 bytes each (must be escaped)
    if (cbufGetFree(objI2C->pCbufTx) < 6) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2c1BeginTransmission");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return I2C_TXION_STATUS_ERR_NOSPACE;
    }

    //Put "Control Character" and START
    cbufPutByteNoCheck(objI2C->pCbufTx, cbufGetEscapeCharacter(objI2C->pCbufTx));
    cbufPutByteNoCheck(objI2C->pCbufTx, 's');

    //Put address byte
    cbufPutEscapedByte(objI2C->pCbufTx, adr);
    return 0;   //Success
}
#endif


BYTE i2cEndTransmission(I2C_INFO* objI2C) {
    WORD_VAL w;

    //If current transmission has already encountered an error, Stop, and return that error
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        return objI2C->flags.currTxionStatus;
    }

    //Put "Control Character" and STOP
    w.byte.LB = cbufGetEscapeCharacter(objI2C->pCbufTx);
    w.byte.HB = 'p';
    if (cbufPutWord(objI2C->pCbufTx, w.Val) == 0) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2c1EndTransmission");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return objI2C->flags.currTxionStatus;
    }

    //Start transmission
    serI2CTask(objI2C);

    return 0;   //Success
}


#if defined I2C_USE_SHARED_FUNCTIONS
BYTE i2cWrite(I2C_INFO* objI2C, BYTE b) {
    //If current transmission has already encountered an error, Stop, and return that error
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        return objI2C->flags.currTxionStatus;
    }

    if (cbufPutEscapedByte(objI2C->pCbufTx, b) == 0) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2c1Write");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return objI2C->flags.currTxionStatus;
    }
    return 0;   //Success
}
#endif


#if defined I2C_USE_SHARED_FUNCTIONS
BYTE i2cWriteArray(I2C_INFO* objI2C, BYTE* arr, BYTE size) {
    BYTE i;
    WORD sizeRequired;  //Size required, including escaping characters

    //If current transmission has already encountered an error, Stop, and return that error
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        return objI2C->flags.currTxionStatus;
    }

    //Size required for msg, including size required for escaping characters. Each character that is
    //equal to the escape character (cbufGetEscapeCharacter() ) requires 2 bytes! Must be escaped!
    sizeRequired = cbufGetEscapedSizeRequired(objI2C->pCbufTx, arr, size);

    //Check if there is enough space in the buffer. Add 2 for "Escaped P" (2 bytes)
    if (cbufGetFree(objI2C->pCbufTx) < (sizeRequired + 2)) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nI2C ERR: No space!");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return I2C_TXION_STATUS_ERR_NOSPACE;
    }

    //Add all bytes, escapaing any characters as required
    for (i=0; i<size; i++) {
        //If given byte is the "Escape Character", need to put an "Escape Charcter" in front of it
        if (arr[i] == cbufGetEscapeCharacter(objI2C->pCbufTx)) {
            cbufPutByteNoCheck(objI2C->pCbufTx, cbufGetEscapeCharacter(objI2C->pCbufTx));
        }

        //Put current character
        cbufPutByteNoCheck(objI2C->pCbufTx, arr[i]);

        //ALTERNATIVE METHOD! Use cbufPutEscapedByte() function.
        //cbufPutEscapedByte(objI2C->pCbufTx, arr[i]);
    }

    return 0;   //Success
}
#endif


BYTE i2cWriteString(I2C_INFO* objI2C, const char* str) {
    //If current transmission has already encountered an error, Stop, and return that error
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        return objI2C->flags.currTxionStatus;
    }

    if (i2cWriteArray(objI2C, (BYTE*)str, strlen(str)) != 0) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2cWriteString");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return objI2C->flags.currTxionStatus;
    }
    return 0;   //Success
}


BYTE i2cWriteAsciiEscString(I2C_INFO* objI2C, const char* str) {
    if (cbufPutAsciiEscString(objI2C->pCbufTx, 0, (const char*)str, ASCIIESC_ADD_S_AND_P) == 0)
        return I2C_TXION_STATUS_ERROR;

    //Start transmission
    serI2CTask(objI2C);

    return 0;   //Success
}



#if defined I2C_USE_SHARED_FUNCTIONS
BYTE i2cRead(I2C_INFO* objI2C, BYTE size) {
    //If current transmission has already encountered an error, Stop, and return that error
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        return objI2C->flags.currTxionStatus;
    }

    //Check if 4 bytes available in buffer: "^rn" (n might require escaping = 2 bytes)
    if (cbufGetFree(objI2C->pCbufTx) < 4) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2c1Read");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return I2C_TXION_STATUS_ERR_NOSPACE;
    }

    //Put "Control Character" and 'r'
    cbufPutByteNoCheck(objI2C->pCbufTx, cbufGetEscapeCharacter(objI2C->pCbufTx));
    cbufPutByteNoCheck(objI2C->pCbufTx, 'r');

    //Put size byte
    cbufPutEscapedByte(objI2C->pCbufTx, size);

    return 0;   //Success
}
#endif


BYTE i2cReadToArray(I2C_INFO* objI2C, BYTE* buf, BYTE size) {
    //If current transmission has already encountered an error, Stop, and return that error
    if (objI2C->flags.currTxionStatus != I2C_TXION_STATUS_OK) {
        return objI2C->flags.currTxionStatus;
    }

    //This command has the format "^Rnbb", where:
    // - 'R' is the control character. Is Upper Case so it can not be created via Ascii Commands (only lower case possible)
    // - First byte is number of bytes to read
    // - Second and Third byte is pointer buffer to write read bytes
    //Use Upper Case control character to ensure it can not be repreduced via "Ascii Commands"!

    //Check if required number of bytes are available for writting
    //Get required size. Minimum is 5 bytes. Additionally n, b and b might require escaping
    //To Save code, just assume worst cast where n, b and b require escaping. Size required = 5 + 3 = 8
    if (cbufGetFree(objI2C->pCbufTx) < 8) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2cReadToArray");
        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return I2C_TXION_STATUS_ERR_NOSPACE;
    }
    //ALTERNATIVE CODE! Calculates exact size required
//    BYTE sizeRequired = 5;
//    if (size==cbufGetEscapeCharacter(objI2C->pCbufTx))
//        sizeRequired++;
//    if (nzWordGetLowByte_MACRO(((WORD)buf))==cbufGetEscapeCharacter(objI2C->pCbufTx))
//        sizeRequired++;
//    if (nzWordGetHighByte_MACRO(((WORD)buf))==cbufGetEscapeCharacter(objI2C->pCbufTx))
//        sizeRequired++;
//    if (cbufGetFree(objI2C->pCbufTx) < sizeRequired) {
//        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: i2cReadToArray");
//        objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
//        return I2C_TXION_STATUS_ERR_NOSPACE;
//    }

    //Put "Control Character" and 'R'
    cbufPutByteNoCheck(objI2C->pCbufTx, cbufGetEscapeCharacter(objI2C->pCbufTx));
    cbufPutByteNoCheck(objI2C->pCbufTx, 'R');

    //Put size byte
    cbufPutEscapedByte(objI2C->pCbufTx, size);

    //Put pointer
#if defined( __C30__ )
    cbufPutEscapedByte(objI2C->pCbufTx, nzWordGetHighByte_MACRO( ((WORD)buf) ) );
    cbufPutEscapedByte(objI2C->pCbufTx, nzWordGetLowByte_MACRO( ((WORD)buf) ) );
#elif defined( __PIC32MX__ )
	// TODO for PIC32MX - pointer is 32-bit! Can not use WORD, use either size_t or UINT!!!
    cbufPutEscapedByte(objI2C->pCbufTx, nzWordGetHighByte_MACRO( ((size_t)buf) ) );
    cbufPutEscapedByte(objI2C->pCbufTx, nzWordGetLowByte_MACRO( ((UINT)buf) ) );
#endif

    //DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\ni2cReadToArray 0x");
    //DEBUG_PUT_HEXWORD(DEBUG_LEVEL_WARNING, ((WORD)buf));

    return 0;   //Success
}

BYTE i2cReadSlaveReg(I2C_INFO* objI2C, BYTE adr, BYTE reg, BYTE* buf, BYTE size) {
    objI2C->adr = adr; //Save address

    if (i2cBeginTransmission(objI2C, adr) == 0) {
    //ALTERNATIVE METHOD! i2cBeginTransmission() does size check, and writes STOP and address
//    objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_OK;  //Currently OK, have not finished adding whole message to buffer
//    //Check if there is enough space in the buffer. Just an ESTIMATE!!!! We do not take check if any
//    //bytes given in msg need escaping! This is done in i2cWriteArray() function. Add 6 bytes for address, "Escaped S" and "Escaped P", possible escape character.
//    if (cbufGetFree(objI2C->pCbufTx) > (size + 6)) {
//        //Put "Control Character" and START
//        cbufPutByteNoCheck(objI2C->pCbufTx, cbufGetEscapeCharacter(objI2C->pCbufTx));
//        cbufPutByteNoCheck(objI2C->pCbufTx, 's');
//
//        //Write address
//        cbufPutEscapedByte(objI2C->pCbufTx, objI2C->adr);

        //No need to check reply, will have enough space. Available space check is done in i2cBeginTransmission()
        i2cWrite(objI2C, reg);     //Get Status register command

        if (i2cReadToArray(objI2C, buf, size) == 0) {
            return i2cEndTransmission(objI2C);
        }
    }

    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nI2C ERR: No space!");
    objI2C->flags.currTxionStatus  = I2C_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
    return I2C_TXION_STATUS_ERR_NOSPACE;
}

/**
 * Gets status for given key, and marks that key-value pair as available (set status to 0xff).
 * If not found, returns 0xff.
 *
 * IMPORTANT not to add or remove key-status pairs! Only statusMapPut (interrupt ISR) can
 * do that!
 *
 * @param key
 * @return
 */
BYTE statusMapGet(I2C_INFO* objI2C, BYTE key) {
    BYTE ret = 0xff;    //Not found
    WORD   c;

    //Clear bit 0 of key (I2C address). Bit 0 is NOT part of the address
    key = key & 0xfe;

    //Print current status map
//    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
//    BYTE*  pCurr;
//    pCurr = objI2C->bufStat; //Initialize with first position in buffer
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nstatusMapGet  Pre: ");
//    for(c=I2C_STATUS_BUF_SIZE ; c!=0;) {
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '=');
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');
//        c -= 2;
//    }
//    #endif

    //Search for entry with given key, and mark it as available (set status to 0xff)
    //if found.
    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nSearch");
    for(c=0; c<I2C_STATUS_BUF_SIZE; c+=2) {
        //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '.');

        //NZ_BUILTIN_DISI(0x3FFF);               //Disable all interrupts, excluding level 7
        i2cWriteMI2CIE(objI2C->flags.ch, 0);    //Disable I2C interrupt for this channel

        //Current entry is equal to current key (ignore bit 0 of adr!), use it.
        if(objI2C->bufStat[c] == key) {
            //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'F');
            ret = objI2C->bufStat[c+1];     //Get status for this key
            objI2C->bufStat[c+1] = 0xff;    //Mark current key-status pair as available

            //NZ_BUILTIN_DISI(0x0000);               //Enable interrupts
            i2cWriteMI2CIE(objI2C->flags.ch, 1);    //Enable I2C interrupt for this channel

//            //Move all pairs one position down. ONLY do this if current pair is NOT top pair!
//            if (c < (I2C_STATUS_BUF_SIZE-2)) {
//                BYTE*  pSrc;
//                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'R');
//                pCurr = &objI2C->bufStat[c];
//                pSrc = pCurr+2; //Source is one pair higher in map
//                // - Ensure size IS NOT 0
//                // - Note dst and src will be incremented by this macro!
//                //*((volatile BYTE*)(&c)) = I2C_STATUS_BUF_SIZE-2-c;
//                c = I2C_STATUS_BUF_SIZE-2-c;
//                nzMemCpy_ASM_RRR(pCurr, pSrc, c);
//                //nzMemCpy_ASM_RRR(pCurr, pSrc, I2C_STATUS_BUF_SIZE-2-c);
//                //Alternative using nzMemCpyNoCheck() or memcpy. Will NOT increment pointers!
//                //nzMemCpyNoCheck(pCurr, pSrc, c);
//                //nzMemCpy(pCurr, pSrc, c);
//            }
            break;
        }

        //NZ_BUILTIN_DISI(0x0000);               // Enable interrupts
        i2cWriteMI2CIE(objI2C->flags.ch, 1);    //Enable I2C interrupt for this channel
    }


//    if (ret == 0xff) {
//    //Print current status map
//    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
//    BYTE*  pCurr;
//    pCurr = objI2C->bufStat; //Initialize with first position in buffer
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nstatusMapGet Post: ");
//    for(c=I2C_STATUS_BUF_SIZE ; c!=0;) {
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '=');
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');
//        c -= 2;
//    }
//    #endif
//    }

    return ret;    //Not found
}

/**
 * Add given key-status pair to bottom of map, moving existing items one position
 * up (effectively removing top pair).
 *
 * DETAILS:
 * Search for key-status pair with key=given key, OR status=0xff (marked available).
 * If found:
 * - Move all lower pairs one position up, effectively removing found pair
 *
 * If NOT found:
 * - Move all map pairs one position up, effectively removing top entry
 *
 * When done moving pairs up:
 * - Add new entry to bottom of map
 * 
 * @param key
 * @param status
 * @return
 */
void statusMapPut(I2C_INFO* objI2C, BYTE key, BYTE status) {
    WORD idx;
    BYTE* volatile pCurr;
    BYTE* volatile pSrc;

    //Clear bit 0 of key (I2C address). Bit 0 is NOT part of the address
    key = key & 0xfe;

    //Print current status map
//    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
//    pCurr = objI2C->bufStat; //Initialize with first position in buffer
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nstatusMapPut  Pre: ");
//    for(idx=I2C_STATUS_BUF_SIZE ; idx!=0;) {
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '=');
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');
//        idx -= 2;
//    }
//    #endif

    /////////////////////////////////////////////////
    //Search for key-status pair with key=given key, OR status=0xff (marked available).
    //If found:
    //- Move all lower pairs one position up, effectively removing found pair
    //
    //If NOT found:
    //- Move all map pairs one position up, effectively removing top entry
    //
    //When done moving pairs up:
    //- Add new entry to bottom of map
    for(idx=0; idx<I2C_STATUS_BUF_SIZE; idx+=2) {
        //Key of current entry is equal to current key, use it.
        if(objI2C->bufStat[idx] == key) {
            break;
        }
        //Status of current entry = 0xff (marked available), break
        if (objI2C->bufStat[idx+1] == 0xff)
            break;
    }
    //Move all lower pairs one position up, effectively removing found pair. Only continue if
    //NOT first pair, seeing that first pair is overwritten in next step bellow, effectively removing it.
    if (idx!=0) {
        //If NOT found, reduce idx to second last pair.
        if (idx == I2C_STATUS_BUF_SIZE)
            idx = I2C_STATUS_BUF_SIZE-2;

        pCurr = &objI2C->bufStat[idx+1];    //Address of "status" part of key-status pair just found
        pSrc = pCurr-2;                     //Source is one pair lower in map

        nzMemCpyDecNoOut_ASM_RRR(pCurr, pSrc, idx);
        //nzMemCpyDec_ASM_RRR(pCurr, pSrc, idx);
        //nzMemCpyDecNoCheck(pCurr, pSrc, idx);
        //while(idx--!=0) {
        //    *pCurr-- = *pSrc--;
        //}
    }

    //Write given key-status pair to bottom of map
    objI2C->bufStat[0] = key;
    objI2C->bufStat[1] = status;

    //Print current status map
//    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
//    pCurr = objI2C->bufStat; //Initialize with first position in buffer
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nstatusMapPut Post: ");
//    for(idx=I2C_STATUS_BUF_SIZE ; idx!=0;) {
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '=');
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');
//        idx -= 2;
//    }
//    #endif
}

#endif  //#if defined(HAS_SERPORT_I2C)