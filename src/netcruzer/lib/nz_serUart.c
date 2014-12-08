/**
 * @brief           UART Port functions
 * @file            nz_serUart.c
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
#define THIS_IS_NZ_SERUART1_C
#define THIS_IS_A_SERPORT

//////////////////// RS-485 and Master/Slave Protocols /////////////////////////
/* For future development of this module. To support RS-485 and Master/Slave
 Protocols, a "Circular Buffer" with Escape Sequences could be used. This has quite
 a bit of overhead. A simpler way will be to add a "port address" and "count" variable
 to UART_INFO structure. Only a single RS-485 message can be sent at a time.
 - Add serUartReadSlave() to send message, and read data from slave.
 - When sending a message, the "port address" and "count" variables are set.
 - The ISR will automatically set the "port address" pin when starting to send message.
 - The ISR will decrement "count" reg after each byte. When 0, will reset "port address"

 Can create array of "port address" and "count" variables to allow multiple message to
 be sent without having to wait for previous message to have completed.
*/
////////////////////////////////////////////////////////////////////////////////

#include "HardwareProfile.h"

#if defined(HAS_SERPORT_UART)

#if defined(__C30__)
#define USE_AND_OR  // To enable AND_OR mask setting
#include "uart.h"
#elif defined(__C32__)
#else
    #error "Processor or compiler not supported!"
#endif

#include "nz_circularBuffer.h"
#include "nz_serUart.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"


//Add debugging to this file. The DEBUG_CONF_SERPORTUART macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_SERPORTUART)
    #define DEBUG_CONF_SERPORTUART       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_SERPORTUART
#include "nz_debug.h"


////////// Default Defines //////////////////////

#define UART_USE_SHARED_FUNCTIONS

//#define UART_DONT_SUPPORT_LISTENERS

//Number of listeners that can be registered for all UART ports, must be 2,4 or 8.
#if !defined(UART_LISTENERS)
#define    UART_LISTENERS 4
#endif
#if (UART_LISTENERS<1) || (UART_LISTENERS>7)
#error "UART_LISTENERS must be a value from 1 to 7!"
#endif

//UART1 Interrupt priority, a value from 0 to 7. 0 will disable interrupt, 7 is highest priority.
#if !defined(nzINT_PRIORITY_UART1)
#define     nzINT_PRIORITY_UART1 4
#endif

#if !defined(UART1_BAUD)
#define UART1_BAUD UART_BAUD_9K6
#endif

//UART1 ISR timeout, default is 200ms.
#if !defined(UART_TIMEOUT)
#define UART_TIMEOUT 500
#endif

//UART1 Search for STOP timeout, default is 5 Seconds.
#if !defined(UART_SEARCH_STOP_TIMEOUT)
#define UART_SEARCH_STOP_TIMEOUT 5000
#endif

#if !defined(UART_USE_FORMATTED_TXBUFFER)
#define UART_USE_FORMATTED_TXBUFFER 0
#endif


////////// Defines //////////////////////////////

enum SM_UART_ISR_ {
    SM_ISR_IDLE = 0,
    SM_ISR_TXING,
    SM_ISR_STOP
} UART_SM_ISR;

enum SM_UART_TASK_ {
    SM_UART_TASK_IDLE = 0,
} SM_UART_TASK;



////////// Function Prototypes //////////////////

void uartRxISR (UART_INFO* objUART);
void uartTxISR (UART_INFO* objUART);
void serUartTask(UART_INFO* objUART);
void serUartKickstart(UART_INFO* objUART);
void uartWriteUartIE(BYTE ch, BOOL val);
BYTE uartStatusMapGet(UART_INFO* objUART, BYTE key);
void uartStatusMapPut(UART_INFO* objUART, BYTE key, BYTE status);



////////// Variables ////////////////////////////

#if defined(HAS_SERPORT_UART1)
UART_INFO            uart1Info;
SERPORT_UART_CONFIG  serportUART1Conf;

//TX and RX "Circular Buffers". Use SERPORT_UART1_CREATE_OWN_CIRBUFS define to use this
//module without SERPORTs. If NOT defined, this modules requires nz_serDataPorts.c
#if defined(SERPORT_UART1_CREATE_OWN_CIRBUFS)
    CIRBUF  cbufTxUART1;
    BYTE    bufTxUART1[UART1_TXBUF_SIZE];

    CIRBUF  cbufRxUART1;
    BYTE    bufRxUART1[UART1_RXBUF_SIZE];
#endif
#endif

#if defined(HAS_SERPORT_UART2)
static UART_INFO     uart2;
SERPORT_UART_CONFIG  serportUART2Conf;
#endif
#if defined(HAS_SERPORT_UART3)
static UART_INFO     uart3;
SERPORT_UART_CONFIG  serportUART3Conf;
#endif
#if defined(HAS_SERPORT_UART4)
static UART_INFO     uart4;
SERPORT_UART_CONFIG  serportUART4Conf;
#endif

#if !defined(UART_DONT_SUPPORT_LISTENERS)
static CIRBUF* uartListeners[UART_LISTENERS];    //Array of "Circular Buffer" pointers
#endif

#if defined(__C30__)
    volatile unsigned int* const UxMODE_ARR[] = {&U1MODE, &U2MODE, &U3MODE, &U4MODE};         //Don't use in code!!! Only use pUxMODE()!
    #define pUxMODE(n) (UxMODE_ARR[n])
    //#define pUxMODE(n) ((&U1MODE + (n*(&U2MODE-&U1MODE))))    //Not possible, because UxMODE NOT evenly spaced appart!
    //#define pUxMODEbits(n) ((U1MODEBITS*)(UxMODE_ARR[n])

    volatile unsigned int* const UxSTA_ARR[] = {&U1STA, &U2STA, &U3STA, &U4STA};              //Don't use in code!!! Only use pUxSTA()!
    #define pUxSTA(n) (UxSTA_ARR[n])
    volatile unsigned int* const UxTXREG_ARR[] = {&U1TXREG, &U2TXREG, &U3TXREG, &U4TXREG};    //Don't use in code!!! Only use pUxTXREG()!
    #define pUxTXREG(n) (UxTXREG_ARR[n])
    volatile unsigned int* const UxRXREG_ARR[] = {&U1RXREG, &U2RXREG, &U3RXREG, &U4RXREG};    //Don't use in code!!! Only use pUxRXREG()!
    #define pUxRXREG(n) (UxRXREG_ARR[n])
#elif defined(__C32__)
    //volatile unsigned int* const UxMODE_ARR[] = {&U1MODE, &U2MODE, &U3MODE, &U4MODE};       //Don't use in code!!! Only use pUxMODE()!
    //#define pUxMODE(n) (UxMODE_ARR[n])
    #define pUxMODE(n) ((&U1MODE + (n*(&U2MODE-&U1MODE))))    //POSSIBLE!!! Because UxMODE evenly spaced appart!
    #define U1STABITS __U1STAbits_t

    //volatile unsigned int* const UxSTA_ARR[] = {&U1STA, &U2STA, &U3STA, &U4STA};              //Don't use in code!!! Only use pUxSTA()!
    //#define pUxSTA(n) (UxSTA_ARR[n])
    #define pUxSTA(n) ((&U1STA + (n*(&U2STA-&U1STA))))    //POSSIBLE!!! Because UxSTA evenly spaced appart!

    //volatile unsigned int* const UxTXREG_ARR[] = {&U1TXREG, &U2TXREG, &U3TXREG, &U4TXREG};    //Don't use in code!!! Only use pUxTXREG()!
    //#define pUxTXREG(n) (UxTXREG_ARR[n])
    #define pUxTXREG(n) ((&U1TXREG + (n*(&U2TXREG-&U1TXREG))))  //POSSIBLE!!! Because UxTXREG evenly spaced appart!

    //volatile unsigned int* const UxRXREG_ARR[] = {&U1RXREG, &U2RXREG, &U3RXREG, &U4RXREG};    //Don't use in code!!! Only use pUxRXREG()!
    //#define pUxRXREG(n) (UxRXREG_ARR[n])
    #define pUxRXREG(n) ((&U1RXREG + (n*(&U2RXREG-&U1RXREG))))  //POSSIBLE!!! Because UxRXREG evenly spaced appart!
#else
    #error "Processor or Compiler not supported!"
#endif


/**
 * Receive interrupt routine
 * @param objUART
 */
void uartRxISR(UART_INFO* objUART) {
    char c;

    /////////////////////////////////////////////////
    // Check for errors /////////////////////////////
    //Check for Overun (OERR bit 1), or Framming (FERR bit 2) or Parity (PERR bit 3) Errors
    //Any of these errors will cause a Receive interrupt.
    if ((*pUxSTA(objUART->flags.ch) & 0x000E) != 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nUART");
        DEBUG_PUT_BYTE(DEBUG_LEVEL_WARNING, objUART->flags.ch+1);
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, " ERR = 0x");
        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_WARNING, *pUxSTA(objUART->flags.ch) & 0x000E);

        //if(((U1STABITS*)pUxSTA(objUART->flags.ch))->FERR==1) {
        //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nFERR1!");
        //    ((U1STABITS*)pUxSTA(objUART->flags.ch))->FERR = 0;
        //}
        //if(((U1STABITS*)pUxSTA(objUART->flags.ch))->OERR==1) {
        //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nOERR1!");
        //    ((U1STABITS*)pUxSTA(objUART->flags.ch))->OERR = 0;
        //}
        //if(((U1STABITS*)pUxSTA(objUART->flags.ch))->PERR==1) {
        //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nPERR1!");
        //    ((U1STABITS*)pUxSTA(objUART->flags.ch))->PERR = 0;
        //}

        //((U1STABITS*)pUxSTA(objUART->flags.ch))->OERR = 0;      //cast to U1STABITS possible, because all UxSTABITS have the same bits!
        //((U1STABITS*)pUxSTA(objUART->flags.ch))->FERR = 0;      //cast to U1STABITS possible, because all UxSTABITS have the same bits!
        //((U1STABITS*)pUxSTA(objUART->flags.ch))->PERR = 0;      //cast to U1STABITS possible, because all UxSTABITS have the same bits!
        *((WORD*)pUxSTA(objUART->flags.ch)) &= ~0x000E;         //Clear OERR, FERR and PERR bits
    }


    /////////////////////////////////////////////////
    // Get next byte from Rx Buffer /////////////////
#if defined(__C30__)
    //While there are more bytes in the Receive buffer (URXDA = 1)
    while ((((U1STABITS*)pUxSTA(objUART->flags.ch))->URXDA == TRUE)) {    //cast to U1STABITS possible, because all UxSTABITS have the same bits!
        c = *pUxRXREG(objUART->flags.ch);   //Get next byte
#elif defined(__PIC32MX__)
    //While there are more bytes in the Receive buffer (URXDA = 1)
    while ((((U1STABITS*)pUxSTA(objUART->flags.ch))->URXDA == TRUE)) {    //cast to U1STABITS possible, because all UxSTABITS have the same bits!
        c = *pUxRXREG(objUART->flags.ch);   //Get next byte
#endif
        cbufPutByte(objUART->pCbufRx, c);
    }
}

/**
 * Transmit interrupt routine
 * @param objUART
 */
void uartTxISR(UART_INFO* objUART) {
    switch(objUART->isr.bits.sm) {
        case SM_ISR_IDLE:
            break;
        case SM_ISR_TXING:
        {
            //Is there data to transmit
            if (cbufHasData(objUART->pCbufTx)) {
                //Send next byte in buffer
                *pUxTXREG(objUART->flags.ch) = cbufGetByte(objUART->pCbufTx);
            }
            else {
                //Finished sending all data, return to idle state
                objUART->isr.bits.sm = SM_ISR_IDLE;
            }
            break;
        }
    }
}


/////////////////////////////////////////////////
#if defined(HAS_SERPORT_UART1)

/**
 * UART 1 Transmit ISR. For PIC32 this is the Transmit and Receive ISR!
 */
#if defined(__C30__)
void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt (void)
{
    IFS0bits.U1TXIF = 0;    //Clear interrupt flag
    uartTxISR(&uart1Info);
}

/**
 * UART 1 Receive ISR. For PIC32 this is no Receive ISR!
 * PIC32MX Only has a single ISR for TX and RX. Do Nothing!!
 */
void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt (void)
{
    IFS0bits.U1RXIF = 0;    //Clear interrupt flag
    uartRxISR(&uart1Info);
}
#elif defined(__PIC32MX__)
void __attribute((interrupt(ipl6), vector(_UART1_VECTOR), nomips16)) U1Interrupt(void)
{
	if(IFS1bits.U1RXIF) {
        IFS1bits.U1RXIF = 0;        //Clear interrupt flag
		uartTxISR(&uart1Info);
    }
	if(IEC1bits.U1TXIE)
	{
		if(IFS1bits.U1TXIF) {
            IFS1bits.U1TXIF = 0;    //IFS1bits.U1TXIF = 0;
			uartRxISR(&uart1Info);
        }
	}
}
#endif


/**
 * Initialization code.
 */
void serUart1Init(void) {
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nserUart1Init()");

    //Initialize whole structure with 0
    memset(&uart1Info, 0, sizeof(uart1Info));

    uart1Info.pCbufTx = CIRBUF_TX_UART1;
    uart1Info.pCbufRx = CIRBUF_RX_UART1;
    uart1Info.flagsVal = 0;
    uart1Info.flags.ch = 0;                 //UART channel offset, channel 1 = 0
    uart1Info.isr.bits.sm = SM_ISR_IDLE;    //ISR State machine idle
    uart1Info.taskSm = SM_UART_TASK_IDLE;   //Task State machine idle

#if defined(SERPORT_UART1_CREATE_OWN_CIRBUFS)
    //UART is always a Streaming type buffer, and format is "Binary, with Escape Sequence Processing"
    cbufInit(&cbufTxUART1, bufTxUART1, sizeof(bufTxUART1), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
    cbufInit(&cbufRxUART1, bufRxUART1, sizeof(bufRxUART1), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
#else
//    //UART is a Streaming type buffer, and format is "Binary, with Escape Sequence Processing"
//    //!!! The following line will overwrite any formatting or type information contained in CFG_STRUCT !!!
//    cbufSetTypeAndFormat(CIRBUF_TX_UART1, (CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING));
//    cbufSetTypeAndFormat(CIRBUF_RX_UART1, (CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING));
#endif

    //Clear all flags
    serportUART1Conf.flagsVal = 0;

    //Baud rate, do not configure for higher than 400kHz for boards with 400kHz RTC (SBC66EC)
    serportUART1Conf.baud = UART1_BAUD;

    //Turn off UART modules
    CloseUART1();    //Disable UART1 module if enabled previously


    //Enable UART TX and RX interrupts
#if (nzINT_PRIORITY_UART1==1)
    ConfigIntUART1(UART_RX_INT_EN | UART_RX_INT_PR1 | UART_TX_INT_EN | UART_TX_INT_PR1);
#elif (nzINT_PRIORITY_UART1==2)
    ConfigIntUART1(UART_RX_INT_EN | UART_RX_INT_PR2 | UART_TX_INT_EN | UART_TX_INT_PR2);
#elif (nzINT_PRIORITY_UART1==3)
    ConfigIntUART1(UART_RX_INT_EN | UART_RX_INT_PR3 | UART_TX_INT_EN | UART_TX_INT_PR3);
#elif (nzINT_PRIORITY_UART1==4)
    ConfigIntUART1(UART_RX_INT_EN | UART_RX_INT_PR4 | UART_TX_INT_EN | UART_TX_INT_PR4);
#elif (nzINT_PRIORITY_UART1==5)
    ConfigIntUART1(UART_RX_INT_EN | UART_RX_INT_PR5 | UART_TX_INT_EN | UART_TX_INT_PR5);
#elif (nzINT_PRIORITY_UART1==6)
    ConfigIntUART1(UART_RX_INT_EN | UART_RX_INT_PR6 | UART_TX_INT_EN | UART_TX_INT_PR6);
#elif (nzINT_PRIORITY_UART1==7)
    ConfigIntUART1(UART_RX_INT_EN | UART_RX_INT_PR7 | UART_TX_INT_EN | UART_TX_INT_PR7);
#else
    #error "nzINT_PRIORITY_UART1 not supported"
#endif

    //UART enable/disable:
    //  - UART_EN
    //  - UART_DIS
    //UART Idle mode operation:
    //  - UART_IDLE_CON
    //  - UART_IDLE_STOP
    //UART Wake-up on Start
    //  - UART_EN_WAKE
    //  - UART_DIS_WAKE
    //UART Loopback mode enable/disable
    //  - UART_EN_LOOPBACK
    //  - UART_DIS_LOOPBACK
    //Input to Capture module
    //  - UART_EN_ABAUD
    //  - UART_DIS_ABAUD
    //Parity and data bits select
    //  - UART_NO_PAR_9BIT
    //  - UART_ODD_PAR_8BIT
    //  - UART_EVEN_PAR_8BIT
    //  - UART_NO_PAR_8BIT
    //Number of Stop bits
    //  - UART_2STOPBITS
    //  - UART_1STOPBIT
    //IrDA encoder and decoder enable/disable
    //  - UART_IrDA_ENABLE
    //  - UART_IrDA_DISABLE
    //Mode Selection
    //  - UART_MODE_SIMPLEX
    //  - UART_MODE_FLOW
    //Enable bits
    //  - UART_UEN_11
    //  - UART_UEN_10
    //  - UART_UEN_01
    //  - UART_UEN_00
    //Receive Polarity Inversion bit
    //  - UART_UXRX_IDLE_ZERO
    //  - UART_UXRX_IDLE_ONE
    //High Baudrate Enable
    //  - UART_BRGH_FOUR
    //  - UART_BRGH_SIXTEEN
    //U1MODE = UART_EN | UART_IDLE_CON |

    //UART initialized, 9600 Buad, 8bit, no parity, 1 stop bit, High BAUD rate
#if (USER_CONFIGURES_UART1==0)
    OpenUART1(UART_EN | UART_NO_PAR_8BIT | UART_1STOPBIT | UART_UEN_00 | UART_BRGH_FOUR , UART_TX_ENABLE, UART_BAUD_9K6);
#endif

#if defined(__C30__)
    //_MUART1IF = 0;     //Clear interrupt flag

    // Configure UART1 interrupt priority bits
    //_MUART1IP = nzINT_PRIORITY_UART1;

    //_MUART1IE = 1; // Enable UART 1 interrupt
#elif defined(__PIC32MX__)
#endif
}


/**
 * Task
 */
void serUart1Task(void) {
    serUartTask(&uart1Info);
}


BYTE serUart1IsBusy(void) {
    #if defined UART_USE_SHARED_FUNCTIONS
    return serUartIsBusy(&uart1Info);
    #else
    UART_INFO* objUART;
    objUART = &uart1Info;
    
    //Return TRUE if the ISR is NOT in the idle state!
    return (objUART->isr.bits.sm != SM_ISR_IDLE);
    #endif
}


/**
 * Add a byte to the UART Transmit Queue (Circular Buffer)
 * @param b Byte to add to UART Transmit Queue
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE serUart1Write(BYTE b) {
    #if defined UART_USE_SHARED_FUNCTIONS
    return serUartWrite(&uart1Info, b);
    #else
    UART_INFO* objUART;
    objUART = &uart1Info;

    //If current transmission has already encountered an error, Stop, and return that error
    if (objUART->flags.currTxionStatus != UART_TXION_STATUS_OK) {
        return objUART->flags.currTxionStatus;
    }

    if (cbufPutByte(objUART->pCbufTx, b) == 0) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: serUart1Write");
        objUART->flags.currTxionStatus  = UART_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return objUART->flags.currTxionStatus;
    }
    return 0;   //Success
    #endif
}


/**
 * Add given BYTE array to the UART Transmit Queue (Circular Buffer)
 * @param arr The byte array to add to UART Transmit Queue
 * @param size Number of bytes to add
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE serUart1WriteArray(BYTE* arr, BYTE size) {
    return serUartWriteArray(&uart1Info, arr, size);
}


/**
 * Adds given NULL terminated string to the UART Transmit Queue (Circular Buffer).
 * The NULL terminator is NOT included (not written to buffer!)
 * @param str Null terminated string to add
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE serUart1WriteString(const char* str) {
    return serUartWriteString(&uart1Info, str);
}

#endif //#if defined(HAS_SERPORT_UART1)


#define UART1_ISR_DETAILED_DEBUG_INFO
//#define UART1_TASK_DETAILED_DEBUG_INFO


/**
 * Uart Task
 */
void serUartTask(UART_INFO* objUART) {
    BOOL bDone;

    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
    //Check receive "Circular Buffer" has no format
    if (cbufGetFormat(objUART->pCbufRx) != BUFFORMAT_NONE) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nUART Cirbuf is formatted!");
        return;
    }
    //Check receive "Circular Buffer" format is "Binary, with Escape Sequence Processing"
    //if (cbufGetFormat(objUART->pCbufRx) != BUFFORMAT_BIN_ESC) {
    //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nUART NOT BinEsc!");
    //    return;
    //}
    #endif

    bDone = TRUE;
    do {
    switch(objUART->taskSm) {
        case SM_UART_TASK_IDLE:
            //Ensure ISR is in idle state! We can not modify TXBUF if ISR is also modifying it!
            if (objUART->isr.bits.sm != SM_ISR_IDLE) {
                if (tick16TestTmr(objUART->timeout)) {
                    objUART->timeout = tick16Get() + tick16ConvertFromMS(UART_TIMEOUT);
                    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nUART Timeout!");
                    DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '-');          //Print tick value
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, tick16Get());

//                    uartWriteUartIE(objUART->flags.ch, 0);  //Disable UART interrupt
//                    ((U1MODEBITS*)pUxMODE)->UARTEN = 0;     //Disable UART Module (cast to U1MODEBITS, because all UxMODEBITS vars have the same bits!)
//
//                    objUART->isr.bits.isrTxionStatus = UART_TXION_STATUS_ERR_TIMEOUT;
//                    objUART->isr.bits.sm = SM_ISR_STOP;     //Set ISR state machine to STOP condition
//
//                    ((U1MODEBITS*)pUxMODE)->UARTEN = 1;     //Enable UART Module (cast to U1MODEBITS, because all UxMODEBITS vars have the same bits!)
//                    uartWriteUartIE(objUART->flags.ch, 1);  //Enable UART interrupt
                }
                return;
            }

            nzGlobals.wdtFlags.bits.serUart = 1;     //Indicate that this module is still alive, and clear WDT

            //Is there data to transmit
            if (cbufHasData(objUART->pCbufTx)) {
                objUART->isr.bits.sm = SM_ISR_TXING;    //Set ISR state machine to START condition
                objUART->timeout = tick16Get() + tick16ConvertFromMS(UART_TIMEOUT); //ISR Timeout

                //Send next byte in buffer
                *pUxTXREG(objUART->flags.ch) = cbufGetByte(objUART->pCbufTx);

                //uartWriteUartIE(objUART->flags.ch, 1);  //Enable UART interrupt

                //Kick start UART ISR
                //IFS0bits.U1TXIF = 1;    //Set interrupt flag
            }
            break;
    };  //switch(objUART->taskSm)
    } while(!bDone);
}


/**
 * Start Uart transmission. ONLY CALL if TX buffer has data!!!
 */
void serUartKickstart(UART_INFO* objUART) {
    //Ensure ISR is in idle state! We can not modify TXBUF if ISR is also modifying it!
    if (objUART->isr.bits.sm == SM_ISR_IDLE) {
        //No need to check, this function is ONLY CALLED WHEN THERE IS DATA!!! Is there data to transmit
        //if (cbufHasData(objUART->pCbufTx)) {
            objUART->isr.bits.sm = SM_ISR_TXING;    //Set ISR state machine to START condition
            objUART->timeout = tick16Get() + tick16ConvertFromMS(UART_TIMEOUT); //ISR Timeout

            //Send next byte in buffer
            *pUxTXREG(objUART->flags.ch) = cbufGetByte(objUART->pCbufTx);
        //}
    }
}


#if !defined(UART_DONT_SUPPORT_LISTENERS)
/**
 * Register given "Circular Buffer" as a listener. After doing this, the returned
 * "Listener ID" can be used as a parameter with certian UART functions that return data.
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
BYTE serUartRegisterListener(CIRBUF* pBuf) {
    BYTE i;
    BYTE listenerID = 0x02; //Start at 0x02, 0x01 is reserved for default listener

    //Add listener to first empty spot, or return ID if already registered
    for (i=0; i<UART_LISTENERS; i++) {
        //Current location is available, add this "Circular Buffer" to this position
        if (uartListeners[i] == 0) {
            uartListeners[i] = pBuf;

            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nReg Lstnr = ");
            DEBUG_PUT_BYTE(DEBUG_LEVEL_INFO, listenerID);

            return listenerID;
        }

        //Alread registered
        if (uartListeners[i] ==  pBuf) {
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
void serUartInit(void)
{
    #if !defined(UART_DONT_SUPPORT_LISTENERS)
    //Initialize whole structure with 0
    memset(&uartListeners, 0, sizeof(uartListeners));
    #endif
}


/**
 * Enable or Disable TX and RX interrupts.
 *
 * @param ch Channel offset, 0=UART 1, 1=UART2, 2=UART 3
 * @param val 0 or 1
 */
void uartWriteUartIE(BYTE ch, BOOL val) {
#if defined(__C30__)
    if (ch==0) {
        IEC0bits.U1RXIE = val;
        IEC0bits.U1TXIE = val;
    }
    else if (ch==1)  {
        IEC1bits.U2RXIE = val;
        IEC1bits.U2TXIE = val;
    }
    else if (ch==2) {
        IEC5bits.U3RXIE = val;
        IEC5bits.U3TXIE = val;
    }
    else if (ch==3) {
        IEC5bits.U4RXIE = val;
        IEC5bits.U4TXIE = val;
    }
#elif defined(__PIC32MX__)
#endif
}


/**
 * Gets a pointer to the UART_INFO structure for the given bus number.
 *
 * @param uartBus UART Bus number. Must be an UART bus that is available, for example
 *        if the board has an UART 1 and UART 2 bus, it must be 1 or 2.
 *
 * @return Returns a pointer to the UART_INFO structure for the given bus number.
 *         Return 0 if error, or given bus number is not available.
 */
UART_INFO* uartGetInfoForBus(BYTE uartBus) {
#if defined(HAS_SERPORT_UART1)
    if (uartBus==1)
        return &uart1Info;
#endif
#if defined(HAS_SERPORT_UART2)
    if (uartBus==2)
        return &uart2Info;
#endif
#if defined(HAS_SERPORT_UART3)
    if (uartBus==3)
        return &uart3Info;
#endif
#if defined(HAS_SERPORT_UART4)
    if (uartBus==4)
        return &uart4Info;
#endif
    return 0;   //ERROR
}


#if defined UART_USE_SHARED_FUNCTIONS
BYTE serUartIsBusy(UART_INFO* objUART) {
    //Return TRUE if the ISR is NOT in the idle state!
    return (objUART->isr.bits.sm != SM_ISR_IDLE);
}
#endif


#if defined UART_USE_SHARED_FUNCTIONS
BYTE serUartWrite(UART_INFO* objUART, BYTE b) {
    //If current transmission has already encountered an error, Stop, and return that error
    if (objUART->flags.currTxionStatus != UART_TXION_STATUS_OK) {
        return objUART->flags.currTxionStatus;
    }

    #if (UART_USE_FORMATTED_TXBUFFER == 0)
    if (cbufPutByte(objUART->pCbufTx, b) == 0) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: serUart1Write");
        objUART->flags.currTxionStatus  = UART_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return objUART->flags.currTxionStatus;
    }
    #else   //Buffer is formatted
    if (cbufPutEscapedByte(objUART->pCbufTx, b) == 0) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: serUart1Write");
        objUART->flags.currTxionStatus  = UART_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return objUART->flags.currTxionStatus;
    }
    #endif

    serUartKickstart(objUART); //Call task to kick start transmission

    return 0;   //Success
}
#endif


#if defined UART_USE_SHARED_FUNCTIONS
BYTE serUartWriteArray(UART_INFO* objUART, BYTE* arr, BYTE size) {

    //If current transmission has already encountered an error, Stop, and return that error
    if (objUART->flags.currTxionStatus != UART_TXION_STATUS_OK) {
        return objUART->flags.currTxionStatus;
    }

    #if (UART_USE_FORMATTED_TXBUFFER == 0)
    cbufPutArray(objUART->pCbufTx, arr, size);
    #else   //Buffer is formatted
    BYTE i;
    WORD sizeRequired;  //Size required, including escaping characters
    //Size required for msg, including size required for escaping characters. Each character that is
    //equal to the escape character (cbufGetEscapeCharacter() ) requires 2 bytes! Must be escaped!
    sizeRequired = cbufGetEscapedSizeRequired(objUART->pCbufTx, arr, size);

    //Check if there is enough space in the buffer. Add 2 for "Escaped P" (2 bytes)
    if (cbufGetFree(objUART->pCbufTx) < (sizeRequired + 2)) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nUART ERR: No space!");
        objUART->flags.currTxionStatus  = UART_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return UART_TXION_STATUS_ERR_NOSPACE;
    }

    //Add all bytes, escapaing any characters as required
    for (i=0; i<size; i++) {
        //If given byte is the "Escape Character", need to put an "Escape Charcter" in front of it
        if (arr[i] == cbufGetEscapeCharacter(objUART->pCbufTx)) {
            cbufPutByteNoCheck(objUART->pCbufTx, cbufGetEscapeCharacter(objUART->pCbufTx));
        }

        //Put current character
        cbufPutByteNoCheck(objUART->pCbufTx, arr[i]);

        //ALTERNATIVE METHOD! Use cbufPutEscapedByte() function.
        //cbufPutEscapedByte(objUART->pCbufTx, arr[i]);
    }
    #endif

    serUartKickstart(objUART); //Call task to kick start transmission

    return 0;   //Success
}
#endif


BYTE serUartWriteString(UART_INFO* objUART, const char* str) {
    //If current transmission has already encountered an error, Stop, and return that error
    if (objUART->flags.currTxionStatus != UART_TXION_STATUS_OK) {
        return objUART->flags.currTxionStatus;
    }

    if (serUartWriteArray(objUART, (BYTE*)str, strlen(str)) != 0) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: serUartWriteString");
        objUART->flags.currTxionStatus  = UART_TXION_STATUS_ERR_NOSPACE; //No space in buffer to add message
        return objUART->flags.currTxionStatus;
    }

    //Not required, already called in serUartWriteArray() above
    //serUartKickstart(objUART); //Call task to kick start transmission

    return 0;   //Success
}


/**
 * Gets status for given key, and marks that key-value pair as available (set status to 0xff).
 * If not found, returns 0xff.
 *
 * IMPORTANT not to add or remove key-status pairs! Only uartStatusMapPut (interrupt ISR) can
 * do that!
 *
 * @param key
 * @return
 */
BYTE uartStatusMapGet(UART_INFO* objUART, BYTE key) {
    BYTE ret = 0xff;    //Not found
    WORD   c;

    //Clear bit 0 of key (UART address). Bit 0 is NOT part of the address
    key = key & 0xfe;

    //Print current status map
//    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
//    BYTE*  pCurr;
//    pCurr = objUART->bufStat; //Initialize with first position in buffer
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nuartStatusMapGet  Pre: ");
//    for(c=UART_STATUS_BUF_SIZE ; c!=0;) {
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
    for(c=0; c<UART_STATUS_BUF_SIZE; c+=2) {
        //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '.');

        //NZ_BUILTIN_DISI(0x3FFF);               //Disable all interrupts, excluding level 7
        uartWriteUartIE(objUART->flags.ch, 0);    //Disable UART interrupt for this channel

        //Current entry is equal to current key (ignore bit 0 of adr!), use it.
        if(objUART->bufStat[c] == key) {
            //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'F');
            ret = objUART->bufStat[c+1];     //Get status for this key
            objUART->bufStat[c+1] = 0xff;    //Mark current key-status pair as available

            //NZ_BUILTIN_DISI(0x0000);               //Enable interrupts
            uartWriteUartIE(objUART->flags.ch, 1);    //Enable UART interrupt for this channel

//            //Move all pairs one position down. ONLY do this if current pair is NOT top pair!
//            if (c < (UART_STATUS_BUF_SIZE-2)) {
//                BYTE*  pSrc;
//                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, 'R');
//                pCurr = &objUART->bufStat[c];
//                pSrc = pCurr+2; //Source is one pair higher in map
//                // - Ensure size IS NOT 0
//                // - Note dst and src will be incremented by this macro!
//                //*((volatile BYTE*)(&c)) = UART_STATUS_BUF_SIZE-2-c;
//                c = UART_STATUS_BUF_SIZE-2-c;
//                nzMemCpy_ASM_RRR(pCurr, pSrc, c);
//                //nzMemCpy_ASM_RRR(pCurr, pSrc, UART_STATUS_BUF_SIZE-2-c);
//                //Alternative using nzMemCpyNoCheck() or memcpy. Will NOT increment pointers!
//                //nzMemCpyNoCheck(pCurr, pSrc, c);
//                //nzMemCpy(pCurr, pSrc, c);
//            }
            break;
        }

        //NZ_BUILTIN_DISI(0x0000);               // Enable interrupts
        uartWriteUartIE(objUART->flags.ch, 1);    //Enable UART interrupt for this channel
    }


//    if (ret == 0xff) {
//    //Print current status map
//    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
//    BYTE*  pCurr;
//    pCurr = objUART->bufStat; //Initialize with first position in buffer
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nuartStatusMapGet Post: ");
//    for(c=UART_STATUS_BUF_SIZE ; c!=0;) {
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
void uartStatusMapPut(UART_INFO* objUART, BYTE key, BYTE status) {
    WORD idx;
    BYTE* volatile pCurr;
    BYTE* volatile pSrc;

    //Clear bit 0 of key (UART address). Bit 0 is NOT part of the address
    key = key & 0xfe;

    //Print current status map
//    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
//    pCurr = objUART->bufStat; //Initialize with first position in buffer
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nuartStatusMapPut  Pre: ");
//    for(idx=UART_STATUS_BUF_SIZE ; idx!=0;) {
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
    for(idx=0; idx<UART_STATUS_BUF_SIZE; idx+=2) {
        //Key of current entry is equal to current key, use it.
        if(objUART->bufStat[idx] == key) {
            break;
        }
        //Status of current entry = 0xff (marked available), break
        if (objUART->bufStat[idx+1] == 0xff)
            break;
    }
    //Move all lower pairs one position up, effectively removing found pair. Only continue if
    //NOT first pair, seeing that first pair is overwritten in next step bellow, effectively removing it.
    if (idx!=0) {
        //If NOT found, reduce idx to second last pair.
        if (idx == UART_STATUS_BUF_SIZE)
            idx = UART_STATUS_BUF_SIZE-2;

        pCurr = &objUART->bufStat[idx+1];    //Address of "status" part of key-status pair just found
        pSrc = pCurr-2;                     //Source is one pair lower in map

        nzMemCpyDecNoOut_ASM_RRR(pCurr, pSrc, idx);
        //nzMemCpyDec_ASM_RRR(pCurr, pSrc, idx);
        //nzMemCpyDecNoCheck(pCurr, pSrc, idx);
        //while(idx--!=0) {
        //    *pCurr-- = *pSrc--;
        //}
    }

    //Write given key-status pair to bottom of map
    objUART->bufStat[0] = key;
    objUART->bufStat[1] = status;

    //Print current status map
//    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
//    pCurr = objUART->bufStat; //Initialize with first position in buffer
//    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nuartStatusMapPut Post: ");
//    for(idx=UART_STATUS_BUF_SIZE ; idx!=0;) {
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '=');
//        DEBUG_PUT_HEXBYTE(DEBUG_LEVEL_INFO, *pCurr++);
//        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');
//        idx -= 2;
//    }
//    #endif
}

#endif  //#if defined(HAS_SERPORT_UART)