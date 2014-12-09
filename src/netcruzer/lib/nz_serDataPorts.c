/**
 * @brief           Functions for "Serial Data Ports" (SDP)
 * @file            nz_serDataPorts.c
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
#define THIS_IS_NZ_SERDATA_PORTS_C

#include "HardwareProfile.h"
#include "nz_serDataPorts.h"
#include "nz_helpers.h"

#if defined(HAS_A_SERPORT)

//Add debugging to this file. The DEBUG_CONF_SERPORT macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_SERDATA_PORTS)
    #define DEBUG_CONF_SERDATA_PORTS       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_SERDATA_PORTS
#include "nz_debug.h"
#include "TCPIP Stack/SMTP.h"


////////// Defines //////////////////////////////
enum SERPORT_INDEX_ENUM {
    SERPORT_INDEX_FIRST = -1,
    #if defined(HAS_NZ_DEBUGGING)
    SERPORT_INDEX_DEBUG,
    #endif
    #if defined(HAS_SERPORT_UART1)
    SERPORT_INDEX_UART1,
    #endif
    #if defined(HAS_SERPORT_UART2)
    SERPORT_INDEX_UART2,
    #endif
    #if defined(HAS_SERPORT_UART3)
    SERPORT_INDEX_UART3,
    #endif
    #if defined(HAS_SERPORT_UART4)
    SERPORT_INDEX_UART4,
    #endif
    #if defined(HAS_SERPORT_SPI1)
    SERPORT_INDEX_SPI1,
    #endif
    #if defined(HAS_SERPORT_SPI3)
    SERPORT_INDEX_SPI3,
    #endif
    #if defined(HAS_SERPORT_I2C1)
    SERPORT_INDEX_I2C1,
    #endif
    #if defined(HAS_SERPORT_I2C2)
    SERPORT_INDEX_I2C2,
    #endif

    #if defined(HAS_SERPORT_I2C3)
    SERPORT_INDEX_I2C3,
    #endif
    #if defined(HAS_SERPORT_UDP1)
    SERPORT_INDEX_UDP1,
    #endif
    #if defined(HAS_SERPORT_UDP2)
    SERPORT_INDEX_UDP2,
    #endif
    #if defined(HAS_SERPORT_UDP3)
    SERPORT_INDEX_UDP3,
    #endif
    #if defined(HAS_SERPORT_TCP1)
    SERPORT_INDEX_TCP1,
    #endif
    #if defined(HAS_SERPORT_TCP2)
    SERPORT_INDEX_TCP2,
    #endif
    #if defined(HAS_SERPORT_TCP3)
    SERPORT_INDEX_TCP3,
    #endif
    #if defined(HAS_SERPORT_USB)
    SERPORT_INDEX_USB,
    #endif

    #if defined(HAS_SERPORT_USER1)
    SERPORT_INDEX_USER1,
    #endif
    #if defined(HAS_SERPORT_USER2)
    SERPORT_INDEX_USER2,
    #endif
    #if defined(HAS_SERPORT_USER3)
    SERPORT_INDEX_USER3,
    #endif
    #if defined(HAS_SERPORT_USER4)
    SERPORT_INDEX_USER4,
    #endif
    #if defined(HAS_SERPORT_USER5)
    SERPORT_INDEX_USER5,
    #endif
    #if defined(HAS_SERPORT_USER6)
    SERPORT_INDEX_USER6,
    #endif
    #if defined(HAS_SERPORT_USER7)
    SERPORT_INDEX_USER7,
    #endif
    #if defined(HAS_SERPORT_USER8)
    SERPORT_INDEX_USER8,
    #endif

    SERPORT_INDEX_SIZE
};

////////// Variables ////////////////////////////
static BYTE inTaskArr[(SERPORT_INDEX_SIZE/8)+1];

//Bus info structure
SER_INFO    serInfo;
SERPORTS    serports;

//Dummy CIRBUF object
BYTE        serportDummyBuf[BUFFER_DUMMY_SIZE];     //Buffer used for Tx and Rx buffer of dummy "Serial Data Port"
SERPORT     serportDummy;


//Following are all "Serial Data Port" (SERPORT structures)
//!!! IMPORTANT !!! Must have same order as "Serial Data Port" defined in SER_AVAIL_0TO15 and SER_AVAIL_16TO31
#if defined(HAS_NZ_DEBUGGING)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxDebug[DEBUG_TXBUF_SIZE];
    BYTE bufRxDebug[DEBUG_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_UART1)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxUart1[UART1_TXBUF_SIZE];
    BYTE bufRxUart1[UART1_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_UART2)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxUart2[UART2_TXBUF_SIZE];
    BYTE bufRxUart2[UART2_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_UART3)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxUart3[UART3_TXBUF_SIZE];
    BYTE bufRxUart3[UART3_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_UART4)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxUart4[UART4_TXBUF_SIZE];
    BYTE bufRxUart4[UART4_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_SPI1)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxSpi1[SPI1_TXBUF_SIZE];
    BYTE bufRxSpi1[SPI1_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_SPI3)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxSpi3[SPI3_TXBUF_SIZE];
    BYTE bufRxSpi3[SPI3_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_I2C1)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxI2C1[I2C1_TXBUF_SIZE];
    BYTE bufRxI2C1[I2C1_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_I2C2)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxI2C2[I2C2_TXBUF_SIZE];
    BYTE bufRxI2C2[I2C2_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_I2C3)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxI2C3[I2C3_TXBUF_SIZE];
    BYTE bufRxI2C3[I2C3_RXBUF_SIZE];
    #endif
#endif
#if defined(HAS_SERPORT_UDP1)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_UDP2)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_UDP3)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_TCP1)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_TCP2)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_TCP3)
#endif
#if defined(HAS_SERPORT_USB)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    BYTE bufTxUSB[USB_TXBUF_SIZE];
    BYTE bufRxUSB[USB_RXBUF_SIZE];
    #endif
#endif


#if defined(HAS_SERPORT_USER1)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_USER2)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_USER3)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_USER4)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_USER5)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_USER6)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_USER7)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif
#if defined(HAS_SERPORT_USER8)
    //Create TX and RX buffers
    #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
    #error "Complete this!"
    #endif
#endif

/**
 * Initialization code.
 */
void serInit(void) {
    memset(inTaskArr, 0, sizeof(inTaskArr));

    //Initialize dummy "Serial Data Port"
    serportDummy.flagVal = 0;
    serportDummy.strId = 0;
    serportDummy.cbufTx.flagVal = CIRBUF_FLAGS_POWER2 | CIRBUF_TYPE_STREAMING;  //Initialize buffer as CIRBUF type, and streaming
    serportDummy.cbufTx.maxOffset = 1;         //Dummy buffer size is 2
    serportDummy.cbufTx.put = 0;
    serportDummy.cbufTx.get = 0;
    serportDummy.cbufTx.buf = serportDummyBuf;
    serportDummy.cbufRx.flagVal = CIRBUF_FLAGS_POWER2 | CIRBUF_TYPE_STREAMING;  //Initialize buffer as CIRBUF type, and streaming
    serportDummy.cbufRx.maxOffset = 1;         //Dummy buffer size is 2
    serportDummy.cbufRx.put = 0;
    serportDummy.cbufRx.get = 0;
    serportDummy.cbufRx.buf = serportDummyBuf;

    #if defined(HAS_SERPORT_UART1) || defined(HAS_SERPORT_UART2) || defined(HAS_SERPORT_UART3) || defined(HAS_SERPORT_UART4)
    serUartInit();
    #endif
    #if defined(HAS_SERPORT_I2C1) || defined(HAS_SERPORT_I2C2) || defined(HAS_SERPORT_I2C3)
    serI2CInit();
    #endif

    //FIRST do default Initialization
    serDefaultInit();


    /////////////////////////////////////////////////
    //NEXT Call init functions for all serports for additional initialization
    SERPORT_DEBUG_INIT();

    SERPORT_UART1_INIT();
    SERPORT_UART2_INIT();
    SERPORT_UART3_INIT();
    SERPORT_UART4_INIT();
    SERPORT_SPI1_INIT();
    SERPORT_SPI3_INIT();
    SERPORT_I2C1_INIT();
    SERPORT_I2C2_INIT();

    SERPORT_I2C3_INIT();
    SERPORT_UDP1_INIT();
    SERPORT_UDP2_INIT();
    SERPORT_UDP3_INIT();
    SERPORT_TCP1_INIT();
    SERPORT_TCP2_INIT();
    SERPORT_TCP3_INIT();
    SERPORT_USB_INIT();

    SERPORT_USER1_INIT();
    SERPORT_USER2_INIT();
    SERPORT_USER3_INIT();
    SERPORT_USER4_INIT();
    SERPORT_USER5_INIT();
    SERPORT_USER6_INIT();
    SERPORT_USER7_INIT();
    SERPORT_USER8_INIT();
}


/**
 * Must be called every couple of ms
 *
 */
void serTask(void) {
    static BYTE smSerTask = 0;
    BYTE mask;
    BYTE offset;

    //Increment smSerTask, reset if max
    if (++smSerTask >= SERPORT_INDEX_SIZE)
        smSerTask=0;

    //Get offset and mask
    offset = smSerTask/8;
    mask = 0x01 << (smSerTask&0x07);

    //Ensure not currently in this task, for re-entrant use of this function
    if ( (inTaskArr[offset] & mask) != 0) {
        return;
    }

    //Set bit in inTaskArr[] to indicate we are currently in this task
    inTaskArr[offset] = inTaskArr[offset] | mask;

    switch(smSerTask) {
        #if defined(HAS_NZ_DEBUGGING)
        case SERPORT_INDEX_DEBUG:
            SERPORT_DEBUG_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_UART1)
        case SERPORT_INDEX_UART1:
            SERPORT_UART1_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_UART2)
        case SERPORT_INDEX_UART2:
            SERPORT_UART2_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_UART3)
        case SERPORT_INDEX_UART3:
            SERPORT_UART3_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_UART4)
        case SERPORT_INDEX_UART4:
            SERPORT_UART4_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_SPI1)
        case SERPORT_INDEX_SPI1:
            SERPORT_SPI1_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_SPI3)
        case SERPORT_INDEX_SPI3:
            SERPORT_SPI3_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_I2C1)
        case SERPORT_INDEX_I2C1:
            SERPORT_I2C1_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_I2C2)
        case SERPORT_INDEX_I2C2:
            SERPORT_I2C2_TASK();
            break;
        #endif

        #if defined(HAS_SERPORT_I2C3)
        case SERPORT_INDEX_I2C3:
            SERPORT_I2C3_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_UDP1)
        case SERPORT_INDEX_UDP1:
            SERPORT_UDP1_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_UDP2)
        case SERPORT_INDEX_UDP2:
            SERPORT_UDP2_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_UDP3)
        case SERPORT_INDEX_UDP3:
            SERPORT_UDP3_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_TCP1)
        case SERPORT_INDEX_TCP1:
            SERPORT_TCP1_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_TCP2)
        case SERPORT_INDEX_TCP2:
            SERPORT_TCP2_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_TCP3)
        case SERPORT_INDEX_TCP3:
            SERPORT_TCP3_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_USB)
        case SERPORT_INDEX_USB:
            SERPORT_USB_TASK();
            break;
        #endif

        #if defined(HAS_SERPORT_USER1)
        case SERPORT_INDEX_USER1:
            SERPORT_USER1_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_USER2)
        case SERPORT_INDEX_USER2:
            SERPORT_USER2_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_USER3)
        case SERPORT_INDEX_USER3:
            SERPORT_USER3_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_USER4)
        case SERPORT_INDEX_USER4:
            SERPORT_USER4_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_USER5)
        case SERPORT_INDEX_USER5:
            SERPORT_USER5_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_USER6)
        case SERPORT_INDEX_USER6:
            SERPORT_USER6_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_USER7)
        case SERPORT_INDEX_USER7:
            SERPORT_USER7_TASK();
            break;
        #endif
        #if defined(HAS_SERPORT_USER8)
        case SERPORT_INDEX_USER8:
            SERPORT_USER8_TASK();
            break;
        #endif

    }

    //Clear bit in inTaskArr[] to indicate we are currently finished this task
    inTaskArr[offset] = inTaskArr[offset] & ~mask;
}


#if !defined(SERPORT_DONT_MANAGE_SER_INFO_INIT)
/**
 * Allocate required memory for "Serial Data Ports".
 */
void serDefaultInit(void) {
    SERPORT** ppSerport;

    //Inlitialize serInfo with 0
    memset(&serInfo, 0, sizeof(serInfo));

    /////////////////////////////////////////////////
    //Set all SERPORT pointers in GLOBAL_DEFS to NULL, including DEBUG serport
    ppSerport = (SERPORT**)(&NZ_GLOBALS_SERPORT_DEBUG);
    do {
        *ppSerport = NULL;
    } while(ppSerport++ != (SERPORT**)(&NZ_GLOBALS_SERPORT_LAST));


    /////////////////////////////////////////////////
    //Initialize all defined SERPORT pointers in nzGlobals.
    #if defined(HAS_NZ_DEBUGGING)
        nzGlobals.serDebug = (SERPORT*)&serports.debug;
    #endif
    #if defined(HAS_SERPORT_UART1)
        nzGlobals.serUart1 = (SERPORT*)&serports.uart1;
    #endif
    #if defined(HAS_SERPORT_UART2)
        nzGlobals.serUart2 = (SERPORT*)&serports.uart2;
    #endif
    #if defined(HAS_SERPORT_UART3)
        nzGlobals.serUart3 = (SERPORT*)&serports.uart3;
    #endif
    #if defined(HAS_SERPORT_UART4)
        nzGlobals.serUart4 = (SERPORT*)&serports.uart4;
    #endif
    #if defined(HAS_SERPORT_SPI1)
        nzGlobals.serSPI1 = (SERPORT*)&serports.spi1;
    #endif
    #if defined(HAS_SERPORT_SPI3)
        nzGlobals.serSPI3 = (SERPORT*)&serports.spi3;
    #endif
    #if defined(HAS_SERPORT_I2C1)
        nzGlobals.serI2C1 = (SERPORT*)&serports.i2c1;
    #endif
    #if defined(HAS_SERPORT_I2C2)
        nzGlobals.serI2C2 = (SERPORT*)&serports.i2c2;
    #endif

    #if defined(HAS_SERPORT_I2C3)
        nzGlobals.serI2C3 = (SERPORT*)&serports.i2c3;
    #endif
    #if defined(HAS_SERPORT_UDP1)
        nzGlobals.serUDP1 = (SERPORT*)&serports.udp1;
    #endif
    #if defined(HAS_SERPORT_UDP2)
        nzGlobals.serUDP2 = (SERPORT*)&serports.udp2;
    #endif
    #if defined(HAS_SERPORT_UDP3)
        nzGlobals.serUDP3 = (SERPORT*)&serports.udp3;
    #endif
    #if defined(HAS_SERPORT_TCP1)
        nzGlobals.serTCP1 = (SERPORT*)&serports.tcp1;
    #endif
    #if defined(HAS_SERPORT_TCP2)
        nzGlobals.serTCP2 = (SERPORT*)&serports.tcp2;
    #endif
    #if defined(HAS_SERPORT_TCP3)
        nzGlobals.serTCP3 = (SERPORT*)&serports.tcp3;
    #endif
    #if defined(HAS_SERPORT_USB)
        nzGlobals.serUSB = (SERPORT*)&serports.usb;
    #endif

    #if defined(HAS_SERPORT_USER1)
        nzGlobals.serUser1 = (SERPORT*)&serports.user1;
    #endif
    #if defined(HAS_SERPORT_USER2)
        nzGlobals.serUser2 = (SERPORT*)&serports.user2;
    #endif
    #if defined(HAS_SERPORT_USER3)
        nzGlobals.serUser3 = (SERPORT*)&serports.user3;
    #endif
    #if defined(HAS_SERPORT_USER4)
        nzGlobals.serUser4 = (SERPORT*)&serports.user4;
    #endif
    #if defined(HAS_SERPORT_USER5)
        nzGlobals.serUser5 = (SERPORT*)&serports.user5;
    #endif
    #if defined(HAS_SERPORT_USER6)
        nzGlobals.serUser6 = (SERPORT*)&serports.user6;
    #endif
    #if defined(HAS_SERPORT_USER7)
        nzGlobals.serUser7 = (SERPORT*)&serports.user7;
    #endif
    #if defined(HAS_SERPORT_USER8)
        nzGlobals.serUser8 = (SERPORT*)&serports.user8;
    #endif


    /////////////////////////////////////////////////
    //Initialize all defined SERPORT structures to 0, including DEBUG serport
    ppSerport = (SERPORT**)(&NZ_GLOBALS_SERPORT_DEBUG);
    do {
        if (*ppSerport != NULL) {
            //Inlitialize SERPORT structure with all 0
            memset(*ppSerport, 0, sizeof(SERPORT));
        }
    } while(ppSerport++ != (SERPORT**)(&NZ_GLOBALS_SERPORT_LAST));


    /////////////////////////////////////////////////
    //Initialize all defined SERPORT pointers in nzGlobals
    #if defined(HAS_NZ_DEBUGGING)
        serports.debug.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        cbufInit( &((SERPORT*)nzGlobals.serDebug)->cbufTx, bufTxDebug, sizeof(bufTxDebug), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);
        cbufInit( &((SERPORT*)nzGlobals.serDebug)->cbufRx, bufRxDebug, sizeof(bufRxDebug), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);
        #endif
    #endif
    #if defined(HAS_SERPORT_UART1)
        serInfo.block.avail_0to7.bits.uart1 = 1;
        serports.uart1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UART);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        cbufInit( &((SERPORT*)nzGlobals.serUart1)->cbufTx, bufTxUart1, sizeof(bufTxUart1), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
        cbufInit( &((SERPORT*)nzGlobals.serUart1)->cbufRx, bufRxUart1, sizeof(bufRxUart1), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
        #endif
    #endif
    #if defined(HAS_SERPORT_UART2)
        serInfo.block.avail_0to7.bits.uart2 = 1;
        serports.uart2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UART);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        cbufInit( &((SERPORT*)nzGlobals.serUart2)->cbufTx, bufTxUart2, sizeof(bufTxUart2), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
        cbufInit( &((SERPORT*)nzGlobals.serUart2)->cbufRx, bufRxUart2, sizeof(bufRxUart2), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
        #endif
    #endif
    #if defined(HAS_SERPORT_UART3)
        serInfo.block.avail_0to7.bits.uart3 = 1;
        serports.uart3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UART);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        cbufInit( &((SERPORT*)nzGlobals.serUart3)->cbufTx, bufTxUart3, sizeof(bufTxUart3), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
        cbufInit( &((SERPORT*)nzGlobals.serUart3)->cbufRx, bufRxUart3, sizeof(bufRxUart3), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
        #endif
    #endif
    #if defined(HAS_SERPORT_UART4)
        serInfo.block.avail_0to7.bits.uart4 = 1;
        serports.uart4.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UART);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        cbufInit( &((SERPORT*)nzGlobals.serUart4)->cbufTx, bufTxUart4, sizeof(bufTxUart4), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
        cbufInit( &((SERPORT*)nzGlobals.serUart4)->cbufRx, bufRxUart4, sizeof(bufRxUart4), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
        #endif
    #endif
    #if defined(HAS_SERPORT_SPI1)
        serInfo.block.avail_0to7.bits.spi1 = 1;
        serports.spi1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SPI);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_SPI3)
        serInfo.block.avail_0to7.bits.spi3 = 1;
        serports.spi3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SPI);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_I2C1)
        serInfo.block.avail_0to7.bits.i2c1 = 1;
        serports.i2c1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_I2C);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        cbufInit( &((SERPORT*)nzGlobals.serI2C1)->cbufTx, bufTxI2C1, sizeof(bufTxI2C1), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
        cbufInit( &((SERPORT*)nzGlobals.serI2C1)->cbufRx, bufRxI2C1, sizeof(bufRxI2C1), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
        #endif
    #endif
    #if defined(HAS_SERPORT_I2C2)
        serInfo.block.avail_0to7.bits.i2c2 = 1;
        serports.i2c2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_I2C);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        cbufInit( &((SERPORT*)nzGlobals.serI2C2)->cbufTx, bufTxI2C2, sizeof(bufTxI2C2), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
        cbufInit( &((SERPORT*)nzGlobals.serI2C2)->cbufRx, bufRxI2C2, sizeof(bufRxI2C2), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
        #endif
    #endif

    #if defined(HAS_SERPORT_I2C3)
        serInfo.block.avail_8to15.bits.i2c3 = 1;
        serports.i2c3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_I2C);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        cbufInit( &((SERPORT*)nzGlobals.serI2C3)->cbufTx, bufTxI2C3, sizeof(bufTxI2C3), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
        cbufInit( &((SERPORT*)nzGlobals.serI2C3)->cbufRx, bufRxI2C3, sizeof(bufRxI2C3), CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING);
        #endif
    #endif
    #if defined(HAS_SERPORT_UDP1)
        serInfo.block.avail_8to15.bits.udp1 = 1;
        serports.udp1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UDP);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_UDP2)
        serInfo.block.avail_8to15.bits.udp2 = 1;
        serports.udp2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UDP);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_UDP3)
        serInfo.block.avail_8to15.bits.udp3 = 1;
        serports.udp3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UDP);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_TCP1)
        serInfo.block.avail_8to15.bits.tcp1 = 1;
        serports.tcp1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_TCP);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_TCP2)
        serInfo.block.avail_8to15.bits.tcp2 = 1;
        serports.tcp2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_TCP);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_TCP3)
        serInfo.block.avail_8to15.bits.tcp3 = 1;
        serports.tcp3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_TCP);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_USB)
        serInfo.block.avail_8to15.bits.usb = 1;
        serports.usb.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_USBHID);

        //Create TX and RX buffers
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        cbufInit(&((SERPORT*)nzGlobals.serUSB)->cbufTx, bufTxUSB, sizeof(bufTxUSB), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);
        cbufInit(&((SERPORT*)nzGlobals.serUSB)->cbufRx, bufRxUSB, sizeof(bufRxUSB), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);
        #endif
    #endif


    #if defined(HAS_SERPORT_USER1)
        serInfo.block.avail_16to23.bits.user1 = 1;
        serports.user1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_USER2)
        serInfo.block.avail_16to23.bits.user2 = 1;
        serports.user2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_USER3)
        serInfo.block.avail_16to23.bits.user3 = 1;
        serports.user3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_USER4)
        serInfo.block.avail_16to23.bits.user4 = 1;
        serports.user4.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_USER5)
        serInfo.block.avail_16to23.bits.user5 = 1;
        serports.user5.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_USER6)
        serInfo.block.avail_16to23.bits.user6 = 1;
        serports.user6.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_USER7)
        serInfo.block.avail_16to23.bits.user7 = 1;
        serports.user7.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif
    #if defined(HAS_SERPORT_USER8)
        serInfo.block.avail_16to23.bits.user8 = 1;
        serports.user8.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        //Create TX and RX buffers, initialize with defaults
        #if !defined(SERPORT_DONT_CREATE_TXRX_BUFS)
        #error "complete code!"
        #endif
    #endif

    //Count how many "Serial Data Ports" are available
    serInfo.count = nzBitCountArr(((BYTE*)&serInfo.avail), sizeof(serInfo.avail));
}
#endif  //#if !defined(SERPORT_DONT_MANAGE_SER_INFO_INIT)

#endif  //#if defined(HAS_A_SERPORT)