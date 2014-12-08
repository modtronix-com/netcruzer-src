/**
 * @brief           Serial Data Port code. Each "Serial Data Port" consists of two Circular Buffers, one for
 *                  transmit and one for receive.
 * @file            serialPorts.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 *********************************************************************
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
 * 2012-08-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_SERIAL_PORTS_C

#include "HardwareProfile.h"
#include "appConfig.h"

#include <stdlib.h>     //for malloc, free
#include <string.h>     //for memset

#include "nz_helpers.h"
#include "nz_serDataPorts.h"
#include "myDebug.h"

//Add debugging to this file. The DEBUG_CONF_SERIALPORTS macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_SERIALPORTS)
    #define DEBUG_CONF_SERIALPORTS       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_SERIALPORTS
#include "nz_debug.h"


////////// Variables ////////////////////////////
extern BYTE serportDummyBuf[BUFFER_DUMMY_SIZE];

#if defined(HAS_SERPORT_DEBUG)
    //Create TX and RX buffers
    BYTE bufTxDebug[DEBUG_TXBUF_SIZE];
    BYTE bufRxDebug[DEBUG_RXBUF_SIZE];
#endif



/**
 * Default Initialization. Allocate required memory for "Serial Data Ports".
 */
void serDefaultInit(void) {
    BYTE* p;
    SER_CONFIG serConf;
    BYTE mask;
    BYTE index;
    WORD offset=0;
    WORD totalBufSize;
    SERPORT*  pSerport;
    SERPORT** ppSerport;

    //Inlitialize serInfo with 0
    memset(&serInfo, 0, sizeof(serInfo));

    /////////////////////////////////////////////////
    //Get 'ser.avail' specifying what "Serial Data Ports" are available. A "Serial Data Port" is
    //enabled if it is enabled in the 'ser.avail' structure
    // TODO -  Replace this with cfgGetStruct() once implemented!
    cfgGetDfltStruct(ser0to7.avail, serInfo.block.avail_0to7);
    cfgGetDfltStruct(ser8to15.avail, serInfo.block.avail_8to15);
    cfgGetDfltStruct(ser16to23.avail, serInfo.block.avail_16to23);
    cfgGetDfltStruct(ser24to31.avail, serInfo.block.avail_24to31);
    serInfo.count = nzBitCountArr(((BYTE*)&serInfo.avail), sizeof(serInfo.avail));

    /////////////////////////////////////////////////
    //Set all SERPORT pointers in GLOBAL_DEFS to NULL, including DEBUG serport
    ppSerport = (SERPORT**)(&NZ_GLOBALS_SERPORT_DEBUG);
    do {
        *ppSerport = NULL;
    } while(ppSerport++ != (SERPORT**)(&NZ_GLOBALS_SERPORT_LAST));


    /////////////////////////////////////////////////
    //Initialize all defined SERPORT pointers in nzGlobals.
    #if defined(HAS_SERPORT_DEBUG)
        serports.debug.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        nzGlobals.serDebug = (SERPORT*)&serports.debug;
    #endif
    #if defined(HAS_SERPORT_UART1)
        serports.uart1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UART);
        nzGlobals.serUart1 = (SERPORT*)&serports.uart1;
    #endif
    #if defined(HAS_SERPORT_UART2)
        serports.uart2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UART);
        nzGlobals.serUart2 = (SERPORT*)&serports.uart2;
    #endif
    #if defined(HAS_SERPORT_UART3)
        serports.uart3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UART);
        nzGlobals.serUart3 = (SERPORT*)&serports.uart3;
    #endif
    #if defined(HAS_SERPORT_UART4)
        serports.uart4.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UART);
        nzGlobals.serUart4 = (SERPORT*)&serports.uart4;
    #endif
    #if defined(HAS_SERPORT_SPI1)
        serports.spi1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SPI);
        nzGlobals.serSPI1 = (SERPORT*)&serports.spi1;
    #endif
    #if defined(HAS_SERPORT_SPI3)
        serports.spi3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SPI);
        nzGlobals.serSPI3 = (SERPORT*)&serports.spi3;
    #endif
    #if defined(HAS_SERPORT_I2C1)
        serports.i2c1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_I2C);
        nzGlobals.serI2C1 = (SERPORT*)&serports.i2c1;
    #endif
    #if defined(HAS_SERPORT_I2C2)
        serports.i2c2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_I2C);
        nzGlobals.serI2C2 = (SERPORT*)&serports.i2c2;
    #endif


    #if defined(HAS_SERPORT_I2C3)
        serports.i2c3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_I2C);
        nzGlobals.serI2C3 = (SERPORT*)&serports.i2c3;
    #endif
    #if defined(HAS_SERPORT_UDP1)
        serports.udp1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UDP);
        nzGlobals.serUDP1 = (SERPORT*)&serports.udp1;
    #endif
    #if defined(HAS_SERPORT_UDP2)
        serports.udp2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UDP);
        nzGlobals.serUDP2 = (SERPORT*)&serports.udp2;
    #endif
    #if defined(HAS_SERPORT_UDP3)
        serports.udp3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_UDP);
        nzGlobals.serUDP3 = (SERPORT*)&serports.udp3;
    #endif
    #if defined(HAS_SERPORT_TCP1)
        serports.tcp1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_TCP);
        nzGlobals.serTCP1 = (SERPORT*)&serports.tcp1;
    #endif
    #if defined(HAS_SERPORT_TCP2)
        serports.tcp2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_TCP);
        nzGlobals.serTCP2 = (SERPORT*)&serports.tcp2;
    #endif
    #if defined(HAS_SERPORT_TCP3)
        serports.tcp3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_TCP);
        nzGlobals.serTCP3 = (SERPORT*)&serports.tcp3;
    #endif
    #if defined(HAS_SERPORT_USB)
        serports.usb.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_USBHID);
        nzGlobals.serUSB = (SERPORT*)&serports.usb;
    #endif


    #if defined(HAS_SERPORT_USER1)
        serports.user1.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        nzGlobals.serUser1 = (SERPORT*)&serports.user1;
    #endif
    #if defined(HAS_SERPORT_USER2)
        serports.user2.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        nzGlobals.serUser2 = (SERPORT*)&serports.user2;
    #endif
    #if defined(HAS_SERPORT_USER3)
        serports.user3.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        nzGlobals.serUser3 = (SERPORT*)&serports.user3;
    #endif
    #if defined(HAS_SERPORT_USER4)
        serports.user4.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        nzGlobals.serUser4 = (SERPORT*)&serports.user4;
    #endif
    #if defined(HAS_SERPORT_USER5)
        serports.user5.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        nzGlobals.serUser5 = (SERPORT*)&serports.user5;
    #endif
    #if defined(HAS_SERPORT_USER6)
        serports.user6.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        nzGlobals.serUser6 = (SERPORT*)&serports.user6;
    #endif
    #if defined(HAS_SERPORT_USER7)
        serports.user7.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        nzGlobals.serUser7 = (SERPORT*)&serports.user7;
    #endif
    #if defined(HAS_SERPORT_USER8)
        serports.user8.flagVal = (SERPORT_STAT_ENABLED | SERPORT_TYPE_SOFTWARE);
        nzGlobals.serUser8 = (SERPORT*)&serports.user8;
    #endif


    /////////////////////////////////////////////////
    //Configure Debug SERPORT, is NOT done below! (Uses NZ_GLOBALS_SERPORT_FIRST and NOT NZ_GLOBALS_SERPORT_DEBUG)
    #if defined(DEBUG_USE_STREAMING)
        //USB TX and RX buffer initialization. Configure as normal Circular Buffer (streaming, nor formatting)
        cbufInitDefault(CIRBUF_TX_DEBUG, bufTxDebug, DEBUG_TXBUF_SIZE);
        cbufInitDefault(CIRBUF_RX_DEBUG, bufRxDebug, DEBUG_RXBUF_SIZE);
    #else
        //USB TX and RX buffer initialization. Configure as Packet (not large packet) Circular Buffer
        cbufInit(CIRBUF_TX_DEBUG, bufTxDebug, DEBUG_TXBUF_SIZE, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);
        cbufInit(CIRBUF_RX_DEBUG, bufRxDebug, DEBUG_RXBUF_SIZE, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);
    #endif


    /////////////////////////////////////////////////
    //Configure all SERPORT objects. Use pointers in nzGlobals. Pointer will be NULL if SERPORT is
    //not defined in code. Following is done:
    // - Get size of Tx and Rx buffers, defined in cfgStruct
    // - Initialize CIRBUF object for Tx and Rx buffers in SERPORT
    // - Initialize CIRBUF flags
    mask = 0x01;
    index = 0;
    totalBufSize = 0;
    //offset = offsetof(CFG_STRUCT, ser0to7.uart1);  //Get offset of first SER_CONFIG structure. 'ser.uart1' is first SER_CONFIG structure, use as start address
    ppSerport = (SERPORT**)(&NZ_GLOBALS_SERPORT_FIRST);
    do {
        //First time we access this CFG_BLOCK
        if (mask==0x01) {
            if (index==0)
                offset = offsetof(CFG_STRUCT, ser0to7.uart1);  //Get offset of first SER_CONFIG structure. 'ser.uart1' is first SER_CONFIG structure, use as start address
            else if (index==1)
                offset = offsetof(CFG_STRUCT, ser8to15.i2c3);   //Get offset of first SER_CONFIG structure in ser8to15
            else if (index==2)
                offset = offsetof(CFG_STRUCT, ser16to23.user1); //Get offset of first SER_CONFIG structure in ser16to23
            else
                offset = offsetof(CFG_STRUCT, ser24to31.res1);  //Get offset of first SER_CONFIG structure in ser24to31
        }
        pSerport = *ppSerport;
        //If "Serial Data Port" is enabled (pSerport != NULL)
        if (pSerport != NULL) {
            pSerport->cbufTx.put = pSerport->cbufTx.get = 0;
            pSerport->cbufRx.put = pSerport->cbufRx.get = 0;

            //If "Serial Data Port" is enabled via SER_CONFIG structure
            if ( (serInfo.avail[index] & mask) != 0) {
                // TODO - Replace this with cfgGetArray() once implemented!
                //Get current buffer's SER_CONFIG structure.
                cfgGetDfltArray(offset, ((BYTE*)&serConf), sizeof(serConf));

                //Has already been done in code above for each SERPORT. All SERPORT objects are enabled by default above. Disable them in "else" below if required
                //pSerport->flags = (SERPORT_STAT_ENABLED);

                //Get configuration settings from SER_CONFIG structure
                pSerport->strId = serConf.strId;

                //Initialize buffer. Pass NULL as buffer at this stage. All buffers are assigned in "Allocate space..." code below
                cbufInit(&pSerport->cbufTx, NULL, serConf.txSize, serConf.txType);
                cbufInit(&pSerport->cbufRx, NULL, serConf.rxSize, serConf.rxType);

                totalBufSize += (serConf.txSize + serConf.rxSize);
            }
            //"Serial Data Port" is disabled, initialize as dummy "Serial Data Port"
            else {
                pSerport->flagBits.bEnabled = 0;        //SERPORT_STAT_ENABLE flag is cleared (disabled)

                //Initialize "Serial Data Port" as dummy "Serial Data Port". In case a function uses it (does not check pSerport->flags if it is enabled)
                pSerport->strId = 0;
                pSerport->cbufTx.maxOffset = pSerport->cbufRx.maxOffset = 1;      //Dummy "Serial Data Port" has size of 2, so maxOffset = 1
                //Initialize buffer as CIRBUF type, and streaming
                pSerport->cbufTx.flagVal = pSerport->cbufRx.flagVal = CIRBUF_FLAGS_POWER2 | CIRBUF_TYPE_STREAMING;
                pSerport->cbufTx.buf = &serportDummyBuf[0];
                pSerport->cbufRx.buf = &serportDummyBuf[0];
            }
        }

        offset += sizeof(SER_CONFIG);                   //Update offset to point to next SER_CONFIG structure.
        //offset += sizeof(cfgStruct.ser0to15.uart1);    //Update offset to point to next SER_CONFIG structure.

        //Rotate mask bit one position left
        if (mask==0x80) {
            mask = 0x01;
            index++;
        }
        else mask = mask << 1;
    } while(ppSerport++ != (SERPORT**)(&NZ_GLOBALS_SERPORT_LAST));


    /////////////////////////////////////////////////
    //Allocate space for all buffers contained in the (enabled) "Serial Data Ports"
    //"Serial Data Ports" that are disabled are assigned to dummy "Serial Data Port" above
    if ((p = (BYTE *)NZ_MALLOC(totalBufSize)) != NULL) {
        #if defined(DEBUGGING_ENABLED)
        debugErrorFlags.bits.bSerPortMemAllocFail = 0;
        #endif
        ppSerport = (SERPORT**)(&NZ_GLOBALS_SERPORT_FIRST);
        do {
            pSerport = *ppSerport;
            //If "Serial Data Port" is enabled (pSerport != NULL)
            if (pSerport != NULL) {
                //If "Serial Data Port" enable flag is set (enabled via SER_CONFIG structure)
                if (pSerport->flagBits.bEnabled) {
                    pSerport->cbufTx.buf = p;
                    p += (pSerport->cbufTx.maxOffset + 1);
                    pSerport->cbufRx.buf = p;
                    p += (pSerport->cbufRx.maxOffset + 1);
                }
            }
        } while(ppSerport++ != (SERPORT**)(&NZ_GLOBALS_SERPORT_LAST));
    }
    else {
        #if defined(DEBUGGING_ENABLED)
        //ERROR - could not allocate enough memory!
        debugErrorFlags.bits.bSerPortMemAllocFail = 1;
        #endif
    }
}



/**
 * Indicates if given buffer is active. A buffer is active when bit is set in 'ser.avail' structure in cfgStruct.
 *
 * CONDITION: serDefaultInit() must have already been called. This will load configuration from cfgStruct to serInfo.avail.
 *
 * @param bufId The buffer ID of the requested bus. Is a BUFID_XXX variable
 *
 * @return Returns 0 if not active, else 1
 */
//BYTE serIsActive(BYTE bufId) {
//    BYTE mask;
//    BYTE index;
//
//    if (bufId >= BUF_COUNT)
//        return 0;
//
//    index = bufId/8;
//
//    mask = 0x01 << (bufId % 8);
//
//    if ( (serInfo.avail[index] & mask) == 0)
//        return 0;
//
//    return 1;
//}


