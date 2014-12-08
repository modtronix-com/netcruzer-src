/**
 * @brief           UART Port 1 functions
 * @file            nz_serUart.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_seruart_desc Description
 *****************************************
 * Functions for communicating with UART Ports. It is interrupt based, and all processing
 * is handled in the background by interrupt service routines. Internally @ref nz_circularBuffer_desc "Circular Buffers"
 * are used, allowing multiple messages to be written to the queue.
 * - @ref nz_seruart_details "Dedicated UART functions" can be used for communicating with UART ports.
 * - Alternatively messages can be directly written to, and read from the underlying @ref nz_seruart_cirbuf "Circular Buffer".
 * - @ref nz_seruart_ascii "ASCII Commands" are also supported if human readable, ASCIII commands are required.
 * 
 * @subsection nz_seruart_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --------------- UART Configuration (from nz_serUart.h) ----------------
// *********************************************************************
//To save space, this will remove all code to do with Listeners
//#define UART_DONT_SUPPORT_LISTENERS

//Number of listeners that can be registered for all UART ports, a value from 1 to 7.
#define     UART_LISTENERS          (4)             //[-DEFAULT-]

//---- Enable UART1 as a "Serial Data Port" ----
#define     HAS_SERPORT_UART1
#define     USER_CONFIGURES_UART1   (0)             //[-DEFAULT-]

//UART1 Interrupt priority, a value from 0 to 7. 0 will disable interrupt, 7 is highest priority.
#define     nzINT_PRIORITY_UART1    (4)             //[-DEFAULT-]

//Transmit and Receive buffer sizes, for compatibility, ensure it is a power of 2 value! This can be bypassed
//by using a "non power of 2" CircularBuffer implementation for this project. See nz_CircularBuffer.h for details.
#define     UART1_RXBUF_SIZE        (64)            //[-DEFAULT-]
#define     UART1_TXBUF_SIZE        (256)           //[-DEFAULT-]

//UART1 speed, can be 1K2, 2K4, 4K8, 9K6, 19K2, 38K4, 56K, 115K, 250K. Can also use value, for example 12 will give 300kbps.
#define     UART1_BAUD              (UART_BAUD_9K6) //[-DEFAULT-]

//Uncomment this to define your own UART 1 TX and RX "Circular Buffers"
//#define     UART1_DONT_CREATE_CIRBUF

//UART1 ISR timeout, default is 200ms. This is maximum time allowed from START till STOP
#define     UART1_TIMEOUT           (200)           //[-DEFAULT-]

//---- Enable UART2 as a "Serial Data Port" ----
#define     HAS_SERPORT_UART2
#define     USER_CONFIGURES_UART1   (0)             //[-DEFAULT-]

//UART2 Interrupt priority, a value from 0 to 7. 0 will disable interrupt, 7 is highest priority.
#define     nzINT_PRIORITY_UART2    (4)             //[-DEFAULT-]

//Transmit and Receive buffer sizes, for compatibility, ensure it is a power of 2 value! This can be bypassed
//by using a "non power of 2" CircularBuffer implementation for this project. See nz_CircularBuffer.h for details.
#define     UART2_RXBUF_SIZE        (64)            //[-DEFAULT-]
#define     UART2_TXBUF_SIZE        (256)           //[-DEFAULT-]
 @endcode
 *
 * 
 * @subsection nz_seruart_usage Usage
 *****************************************
 * To use this module, the following must be done:
 * - Ensure a "Circular Buffer" has been added to the project, for details @ref nz_circularBuffer_usage "see Circular Buffer Usage".
 * - Copy the @ref nz_seruart_conf "Configuration" section above (from nz_serUart.h)
 *   to the projdefs.h file.
 * - In this "Configuration" section, enable required UART ports. For example to enable UART 1, ensure following is uncommented:
 @code
 #define HAS_SERPORT_UART1
 @endcode
 * - In this "Configuration" section, change any default values if required (if default
 *   values should be used, define is not required in projdefs.h).
 * - Add nz_serUart.c and nz_serDataPorts.c to the MPLAB project.
 * - Include nz_serUart.h in the c file it is used in.
 * - <b>All DONE!</b> The functions defined in this file can now be used in the project.
 * 
 **********************************************************************
 * @section nz_seruart1_lic Software License Agreement
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


/////////////////////////////////////////////////
//  UART - documentation module
/////////////////////////////////////////////////
/** @defgroup nz_seruart_intro UART Serial Data Ports
 @ingroup info_serport
 Netcruzer boards have 4 UART ports. They are all interrupt based, meaning once
 a message is written to the queue, it is processed in the background by an interrupt
 service routine.

  @section nz_seruart_details Dedicated UART functions
  --- Add documentation ---
 
  @section nz_seruart_ascii ASCII Commands
  --- Add documentation ---

  @section nz_seruart_cirbuf Circular Buffer
  --- Add documentation ---
*/


#ifndef NZ_SERUART_H
#define NZ_SERUART_H

#include "nz_circularBuffer.h"


////////// Default Defines //////////////////////

#if (CLOCK_FREQ==32000000ul)
#ifndef UART_BAUD_1K2
#define UART_BAUD_1K2   3332    //0.01% error
#endif
#ifndef UART_BAUD_2K4
#define UART_BAUD_2K4   1666    //0.01% error
#endif
#ifndef UART_BAUD_4K8
#define UART_BAUD_4K8   832
#endif
#ifndef UART_BAUD_9K6
#define UART_BAUD_9K6   416 //0.07% error
#endif
#ifndef UART_BAUD_19K2
#define UART_BAUD_19K2  207 //0.16% error
#endif
#ifndef UART_BAUD_38K4
#define UART_BAUD_38K4  103 //0.16% error
#endif
#ifndef UART_BAUD_56K
#define UART_BAUD_56K   70  //0.6% error
#endif
#ifndef UART_BAUD_115K
#define UART_BAUD_115K  34  //0.62% error
#endif
#ifndef UART_BAUD_250K
#define UART_BAUD_250K  15  //0% error
#endif

#else
#error "UART does not support this CLOCK_FREQ"
#endif


//Transmit and Receive buffer sizes
#if !defined(UART1_RXBUF_SIZE)
#define    UART1_RXBUF_SIZE  64
#endif
#if !defined(UART1_TXBUF_SIZE)
#define    UART1_TXBUF_SIZE  256
#endif
#if !defined(UART2_RXBUF_SIZE)
#define    UART2_RXBUF_SIZE  32
#endif
#if !defined(UART2_TXBUF_SIZE)
#define    UART2_TXBUF_SIZE  128
#endif
#if !defined(UART3_RXBUF_SIZE)
#define    UART3_RXBUF_SIZE  32
#endif
#if !defined(UART3_TXBUF_SIZE)
#define    UART3_TXBUF_SIZE  128
#endif


////////// Defines //////////////////////////////
//UART_TXION_STATUS_xx defines, 0=OK, 1=Added to Buffer, 0x80-0x8F=Error, 0xFF=Status Not Available
#define UART_TXION_STATUS_OK            0    //Currently OK, have not finished adding whole message to buffer
#define UART_TXION_STATUS_ADDED_TO_BUF  1    //Successfully added whole message to the buffer (from start to stop)
#define UART_TXION_STATUS_ERROR         4    //No space in buffer to add message
#define UART_TXION_STATUS_ERR_NOSPACE   5    //No space in buffer to add message
#define UART_TXION_STATUS_ERR_ADR_NOACK 6    //Slave did not ACK address we sent
#define UART_TXION_STATUS_ERR_DAT_NOACK 7    //Slave did not ACK data we sent
#define UART_TXION_STATUS_ERR_BUF_FRMT  8    //Buffer format error
#define UART_TXION_STATUS_ERR_TIMEOUT   9    //Timeout
#define UART_TXION_STATUS_NOT_AVAILABLE 0xFF //Status not available

//UART "Serial Data Port"
#if (defined(HAS_SERPORT_UART1) || defined(HAS_SERPORT_UART2) || defined(HAS_SERPORT_UART3) || defined(HAS_SERPORT_UART4))
    #if !defined(HAS_A_SERPORT)
    #define HAS_A_SERPORT
    #endif
#endif


////////// Configuration Checks /////////////////

//If "Power of 2" buffer used, size must by a power of 2
#if defined(CIRBUF_USE_CIRCULAR_BUFFER_PWR2)
#if defined(HAS_SERPORT_UART1)
    //Ensure size is power of 2
    #if ((UART1_TXBUF_SIZE & ~(UART1_TXBUF_SIZE-1))!=UART1_TXBUF_SIZE)
    #error "UART1 UART1_TXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif

    //Ensure size is power of 2
    #if ((UART1_RXBUF_SIZE & ~(UART1_RXBUF_SIZE-1))!=UART1_RXBUF_SIZE)
    #error "UART1 UART1_RXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif
#endif
#if defined(HAS_SERPORT_UART2)
    //Ensure size is power of 2
    #if ((UART2_TXBUF_SIZE & ~(UART2_TXBUF_SIZE-1))!=UART2_TXBUF_SIZE)
    #error "UART2 UART2_TXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif

    //Ensure size is power of 2
    #if ((UART2_RXBUF_SIZE & ~(UART2_RXBUF_SIZE-1))!=UART2_RXBUF_SIZE)
    #error "UART2 UART2_RXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif
#endif
#if defined(HAS_SERPORT_UART3)
    //Ensure size is power of 2
    #if ((UART3_TXBUF_SIZE & ~(UART3_TXBUF_SIZE-1))!=UART3_TXBUF_SIZE)
    #error "UART3 UART3_TXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif

    //Ensure size is power of 2
    #if ((UART3_RXBUF_SIZE & ~(UART3_RXBUF_SIZE-1))!=UART3_RXBUF_SIZE)
    #error "UART3 UART3_RXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif
#endif
#if defined(HAS_SERPORT_UART4)
    //Ensure size is power of 2
    #if ((UART4_TXBUF_SIZE & ~(UART4_TXBUF_SIZE-1))!=UART4_TXBUF_SIZE)
    #error "UART4 UART4_TXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif

    //Ensure size is power of 2
    #if ((UART4_RXBUF_SIZE & ~(UART4_RXBUF_SIZE-1))!=UART4_RXBUF_SIZE)
    #error "UART4 UART4_RXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif
#endif
#endif



/**
 * Structure containing UART Serport Flags
 */
typedef struct __attribute__((aligned(2), packed))
{
    union {
        struct
        {
            unsigned int bFill : 16;        //Fill to ensure flags take up a 16-bit WORD
        } flags;
        WORD flagsVal;
    };

    /// Configures UART1. This is the value for ??? register of PIC, see PIC datasheet for details.
    // Not all values for UART1CON are possible, only following can be set:
    WORD    config1;

    ///Baud rate. This is the value for ??? register of PIC, see PIC datasheet for details. Some values are:
    //???=9.6kbits/sec
    WORD    baud;

} SERPORT_UART_CONFIG;

//DON'T change! Code that checks wrDstType.bit0 and wrDstType.bit1 assumes there values!
#define RD_DEST_TYPE_LISTENERS 0
#define RD_DEST_TYPE_POINTER   2
#define RD_DEST_TYPE_CIRBUF    3

typedef struct __attribute__((__packed__)) UART_ISR_SMFLAGS_
{
    union {
        struct
        {
            unsigned int sm : 4;                ///< ISR State
            unsigned int isrTxionStatus : 1;
            unsigned int fill12 : 11;
        } bits;
        WORD val;                           ///< Bits 0-3 = State, bits 4-7 = Flags
    };
} UART_ISR_SMFLAGS;

//Must be a power of two value: 4,8,16,32,64
#define UART_STATUS_BUF_SIZE 8

typedef struct UART_INFO_
//typedef struct __attribute__((__packed__)) UART_INFO_
//typedef struct __attribute__((aligned(2), packed)) UART_INFO_
{
    //Buffer containing status. Two bytes are used for each entry, the LSB=Address (0=empty), and MSB=Status.
    BYTE            bufStat[UART_STATUS_BUF_SIZE];
    BYTE            adr;        //Used by non-ISR functions to remember current slave address
    BYTE            taskSm;     //ISR State machine. 0 is idle.
    BYTE            isrAdr;     //Used in ISR - address of slave
    BYTE            isrRead;    //Used in ISR - number of bytes to to read
    BYTE            type;       //Protocol Type - 0=standard streaming serial port, 1=RS485
    
    WORD            isrRdDest;  //Destination for read, meaning depends on value of uart1.isr.bits.wrDstType
                                // - When 0, bits 0-15 indicates which "Circular Buffer" listeners are destinations (0=default)
                                // - When 1, contains pointer to destination BYTE array
    WORD            timeout;    //Used in task to check for a timeout
    CIRBUF*         pCbufTx;
    CIRBUF*         pCbufRx;
    UART_ISR_SMFLAGS isr;        //ISR State machine AND flags.
    union {
        struct
        {
            BYTE ch;
            BYTE currTxionStatus;
        } flags;
        WORD flagsVal;
    };
} UART_INFO;



/////////////////////////////////////////////////
//UART Functions for all ports

/**
 * Initialization code.
 */
void serUartInit(void);

/**
 * Service given UART "Serial Data Port"
 * 
 * @param objUART Pointer to UART_INFO structure for UART Bus to service
 */
void serUartTask(UART_INFO* objUART);


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
BYTE serUartRegisterListener(CIRBUF* pBuf);


/**
 * Gets a pointer to the UART_INFO structure for the given bus number.
 *
 * @param uartBus UART Bus number. Must be an UART bus that is available, for example
 *        if the board has an UART 1 and UART 2 bus, it must be 1 or 2.
 *
 * @return Returns a pointer to the UART_INFO structure for the given bus number.
 *         Return 0 if error, or given bus number is not available.
 */
UART_INFO* uartGetInfoForBus(BYTE uartBus);


/**
 * Check if the UART port is currently Transmitting or Receiving. This is done in the background
 * via the UART interrupt. Messages can still be added if true. This function is mainly
 * required to wait for the current Transmission to finish, so the status can be read.
 *
 * @param objUART The UART bus to request status info from. Use UARTx_INFO defines,
 *        for example UART1_INFO = UART 1.
 *
 * @return Returns TRUE if the UART port is currently Transmitting or Receiving, else FALSE.
 */
BYTE serUartIsBusy(UART_INFO* objUART);

/**
 * Add a byte to the UART Transmit Queue (Circular Buffer).
 *
 * @param objUART The UART bus to request status info from. Use UARTx_INFO defines, for example UART1_INFO = UART 1.
 * @param b Byte to add to UART Transmit Queue
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE serUartWrite(UART_INFO* objUART, BYTE b);


/**
 * Add given BYTE array to the UART Transmit Queue (Circular Buffer), Escaping any
 * characters requiring it.
 *
 * @param objUART The UART bus to request status info from. Use UARTx_INFO defines,
 *        for example UART1_INFO = UART 1.
 * @param arr The byte array to add to UART Transmit Queue
 * @param size Number of bytes to add
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE serUartWriteArray(UART_INFO* objUART, BYTE* arr, BYTE size);


/**
 * Adds given NULL terminated string to the UART Transmit Queue (Circular Buffer).
 * The NULL terminator is NOT included (not written to buffer!)
 *
 * @param objUART The UART bus to request status info from. Use UARTx_INFO defines,
 *        for example UART1_INFO = UART 1.
 * @param str Null terminated string to add
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE serUartWriteString(UART_INFO* objUART, const char* str);


/**
 * !!!!! For future use !!!!!
 * Will be used with Master/Slave serial protocols where we can request data from
 * a slave. For this, "Circular Buffers", must be formatted with escape sequences
 *
 * Reads given number of bytes to the default Circular Receive Buffer.
 *
 * Once bytes have been read, use the uart1GetXxx functions to get them.
 * See serUart1Read() for an example of doing this.
 *
 * @param objUART The UART bus to request status info from. Use UARTx_INFO defines,
 *        for example UART1_INFO = UART 1.
 * @param size Number of bytes to read.
 *
 * @return Returns 0 if request successfully added to Transmission Queue, else a
 * non-zero value is returned.
 */
//BYTE serUartRead(UART_INFO* objUART, BYTE size);


/**
 * !!!!! For future use !!!!!
 * Will be used with Master/Slave serial protocols where we can request data from
 * a slave. For this, "Circular Buffers", must be formatted with escape sequences
 *
 * Reads given number of bytes to the given buffer.
 *
 * @param objUART The UART bus to request status info from. Use UARTx_INFO defines,
 *        for example UART1_INFO = UART 1.
 * @param buf Pointer to buffer to write read bytes to.
 * @param size Number of bytes to read.
 *
 * @return Returns 0 if request successfully added to Transmission Queue, else a
 * non-zero value is returned.
 */
//BYTE serUartReadToArray(UART_INFO* objUART, BYTE* buf, BYTE size);


/////////////////////////////////////////////////
//UART 1
#if defined(HAS_SERPORT_UART1)

#if !defined(SERPORT_UART1_CREATE_OWN_CIRBUFS)
#include "nz_serDataPorts.h"
#endif

//This is used by others
#ifndef THIS_IS_NZ_SERUART1_C
extern UART_INFO uart1Info;
#endif
#define UART1_INFO &uart1Info

//TX and RX "Circular Buffers". Use SERPORT_UART1_CREATE_OWN_CIRBUFS define to use this
//module without SERPORTs. If NOT defined, this modules requires nz_serDataPorts.c
#if defined(SERPORT_UART1_CREATE_OWN_CIRBUFS)
    //This is used by others
    #ifndef THIS_IS_NZ_SERUART1_C
    extern CIRBUF cbufTxUART1;
    extern CIRBUF cbufRxUART1;
    #endif

    #define CIRBUF_TX_UART1 &cbufTxUART1
    #define CIRBUF_RX_UART1 &cbufRxUART1
#endif


/**
 * Initialization code.
 */
void serUart1Init(void);


/**
 * Task
 */
void serUart1Task(void);


/**
 * Check if the UART port is currently Transmitting or Receiving. This is done in the background
 * via the UART interrupt. Messages can still be added if true. This function is mainly
 * required to wait for the current Transmission to finish.
 *
 * @return Returns TRUE if the UART port is currently Transmitting or Receiving, else FALSE.
 */
BYTE serUart1IsBusy(void);



/**
 * Add a byte to the UART Transmit Queue (Circular Buffer)
 *
 * @param b Byte to add to UART Transmit Queue
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE serUart1Write(BYTE b);


/**
 * Add given BYTE array to the UART Transmit Queue (Circular Buffer)
 *
 * @param arr The byte array to add to UART Transmit Queue
 * @param size Number of bytes to add
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE serUart1WriteArray(BYTE* arr, BYTE size);


/**
 * Adds given NULL terminated string to the UART Transmit Queue (Circular Buffer).
 * The NULL terminator is NOT included (not written to buffer!)
 *
 * @param str Null terminated string to add
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE serUart1WriteString(const char* str);


/**
 * Get the available space in the Transmit Queue(Circular Buffer) for adding data.
 */
#define serUart1GetTxBufFree()  cbufGetFree(CIRBUF_TX_UART1)


/**
 * Empty the Transmit Queue(Circular Buffer). Any data contained in the buffer is lost.
 */
#define serUart1EmptyTxBuf() cbufEmpty(CIRBUF_TX_UART1)


/**
 * Checks if the Transmit Queue(Circular Buffer)is empty.
 *
 * @return Returns true if the given buffer is empty. Else, returns false.
 */
#define serUart1IsTxBufEmpty()  cbufIsEmpty(CIRBUF_TX_UART1)


/**
 * Checks if the given transmit buffer is full - no more space for adding data.
 *
 * @return Returns true if the given buffer is full. Else, returns false.
 */
#define serUart1IsTxBufFull()   cbufIsFull(CIRBUF_TX_UART1)


/**
 * Returns number of bytes available in the Receive Queue(Circular Buffer) for
 * reading. They can be read with the serUart1Read... functions.
 *
 * @return Returns number of bytes available in the Receive Queue(Circular Buffer)
 * for reading.
 */
#define serUart1GetRxBufCount() cbufGetCount(CIRBUF_RX_UART1)


/**
 * Returns TRUE if there are bytes available in the Receive Queue(Circular Buffer) for
 * reading. They can be read with the serUart1Read... functions.
 *
 * @return TRUE if there are bytes in the Receive Queue(Circular Buffer) for reading.
 */
#define serUart1HasData() cbufHasData(CIRBUF_RX_UART1)


/**
 * Checks if the Receive Queue(Circular Buffer)is empty. If not, the contents can
 * be read with the serUart1Read... functions.
 *
 * @return Returns true if buffer is empty. Else, returns false.
 */
#define serUart1IsRxBufEmpty()  cbufIsEmpty(CIRBUF_RX_UART1)


/**
 * Reads a single byte from the Receive Queue(Circular Buffer). 
 
 * @preCondition uart1GetRxBufCount() or uart1IsRxBufEmpty() must have been called to
 * confirm there is data!
 *
 * @return Returns 0 if OK, else error code
 */
#define serUart1Read() cbufGetByte(CIRBUF_RX_UART1)
#define serUart1GetByte() cbufGetByte(CIRBUF_RX_UART1)


/**
 * Reads requested number of bytes from Receive Queue(Circular Buffer), and removes
 * them from the queue. The actual number of bytes read is returned.
 * If returned value is less than requested value, all bytes were read.
 *
 * @preCondition uart1GetRxBufCount() or uart1IsRxBufEmpty() must have been called to
 * confirm there is data!
 *
 * @return Returns number of bytes read. If less then size, there were not more bytes
 * in buffer.
 */
#define serUart1ReadArray(pDstArr, size) cbufGetArray(CIRBUF_RX_UART1, pDstArr, size)
#define serUart1GetArray(pDstArr, size) cbufGetArray(CIRBUF_RX_UART1, pDstArr, size)

#endif  //#if defined(HAS_SERPORT_UART1)

#endif  //#ifndef NZ_SERUART_H
