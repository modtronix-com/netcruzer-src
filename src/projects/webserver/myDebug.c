/**
 * @brief           Implementation of debug API as defined in nz_debug.h
 * @file            myDebug.c
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
#define THIS_IS_MY_DEBUG_C

#include "string.h"

#include "HardwareProfile.h"
#include "appConfig.h"

//Ensure nz_debug.h is after myDebug.h.
#include "myDebug.h"
#include "nz_debug.h"
#include "nz_analog.h"
#include "nz_xFlash.h"

//#include "TCPIP Stack/ENCX24J600.h"
#include "TCPIP Stack/Helpers.h"

#include "doxygen/src_examples/examples.h"

//If "Power of 2" buffer used, size is power of 2 -AND- equal to or larger than CIRBUF_MIN_SIZE
#if defined(CIRBUF_USE_CIRCULAR_BUFFER_PWR2)
    //Ensure size is power of 2 -AND- equal to or larger than CIRBUF_MIN_SIZE
    #if ( ((DEBUG_TXBUF_SIZE & ~(DEBUG_TXBUF_SIZE-1))!=DEBUG_TXBUF_SIZE) || (DEBUG_TXBUF_SIZE < CIRBUF_MIN_SIZE))
    #error "DEBUG_TXBUF_SIZE is not power of 2, AND larger or equal to CIRBUF_MIN_SIZE!"
    #endif

    //Ensure size is power of 2 -AND- equal to or larger than CIRBUF_MIN_SIZE
    #if ( ((DEBUG_RXBUF_SIZE & ~(DEBUG_RXBUF_SIZE-1))!=DEBUG_RXBUF_SIZE) || (DEBUG_RXBUF_SIZE < CIRBUF_MIN_SIZE))
    #error "DEBUG_RXBUF_SIZE is not power of 2, AND larger or equal to CIRBUF_MIN_SIZE!"
    #endif
#endif


////////// Variables //////////////////
//Debug variables and flags
DEBUG_ERROR_FLAGS   debugErrorFlags  = {.val = 0};  //Do NOT initialize in debugInit() function! Need it done by compiler before any application code!
DEBUG_VARS          debugVars;
char debugTempBuf[DEBUG_TEMP_BUF_SIZE];


////////// Function prototypes //////////////////
void debugPrintIP(void);
#if (HAS_DEMO_DEBUG_HANDLER == 1)
void demoDebugService(void);
#endif


#if defined(HAS_SERPORT_DEBUG)

////////// Defines //////////////////////////////
#if !defined(HAS_DEMO_DEBUG_HANDLER)
    #define HAS_DEMO_DEBUG_HANDLER              (0)
#endif

//TX and RX "Circular Buffers". Use SERPORT_DEBUG_CREATE_OWN_CIRBUFS define to use this
//module without SERPORTs. If NOT defined, this modules requires nz_serDataPorts.c
#if defined(SERPORT_DEBUG_CREATE_OWN_CIRBUFS)
    CIRBUF  cbufTxDebug;
    BYTE    bufTxDebug[DEBUG_TXBUF_SIZE];   //Buffer used for Debug Tx

    CIRBUF  cbufRxDebug;
    BYTE    bufRxDebug[DEBUG_RXBUF_SIZE];   //Buffer used for Debug Rx
#endif


/**
 * Initialize debug
 */
void debugInit(void) {
    //!!!!! IMPORTANT !!!!!
    //This gets done right at the start of initialization! Don't rely on ANY other code being initialize
    //at this stage! Only make the buffers available so that any debug information can be added to the
    //buffers. At a later stage, when the debug port (USB, RS232...) has been initialized, it will be
    //sent to the configured debug port

    #if defined(SERPORT_DEBUG_CREATE_OWN_CIRBUFS)
    #if defined(DEBUG_USE_STREAMING)
        //USB TX and RX buffer initialization. Configure as normal Circular Buffer (streaming, nor formatting)
        cbufInitDefault(&cbufTxDebug, bufTxDebug, DEBUG_TXBUF_SIZE);
        cbufInitDefault(&cbufRxDebug, bufRxDebug, DEBUG_RXBUF_SIZE);
    #else
        //USB TX and RX buffer initialization. Configure as Packet (not large packet) Circular Buffer
        cbufInit(&cbufTxDebug,
                bufTxDebug,
                DEBUG_TXBUF_SIZE,
                CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);

        cbufInit(&cbufRxDebug,
                cbufRxDebug,
                DEBUG_RXBUF_SIZE,
                CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET);
    #endif
    #endif  //#if defined(SERPORT_DEBUG_CREATE_OWN_CIRBUFS)
}

/**
 * Call once all system initialization is done. Normally just before main system loop.
 */
void debugAllInitDone(void) {
}

#if !defined(DISABLE_WEBSERVER_DEFAULT_DEBUGSERVICE)
/**
 * Debug service routine
 */
void debugService(void) {
    //char cbuf[64];
    WORD debugStrLen;
    char* pData;
    char firstChar; //First character of Debug string.

#if (HAS_DEMO_DEBUG_HANDLER == 1)
    demoDebugService();
#endif

#if defined(DEBUG_USE_STREAMING)
    #error "Processing streaming (non-Packet) debug messages has not been implemented!"
#else
    //Check if a debug message was received
    if (cbufHasWholePacket(CIRBUF_RX_DEBUG)) {

        //Using first character of debug messages speeds things up when there are lots of commands! Can be removed!
        firstChar = cbufPeekPacketByte(CIRBUF_RX_DEBUG);

        if (firstChar=='a') {
            #if !defined(RELEASE_BUILD)
            //'anv' = Analog Values = Print Analog Inputs AN1 to AN3 values
            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "anv") == 0) {

                //sprintf(debugTempBuf, "\nAN1=%04d, AN2=%04d, AN3=%04d", adcReadChan(ADC_CH_A1), adcReadChan(ADC_CH_A2), adcReadChan(ADC_CH_A3));
                cbufPutString(CIRBUF_TX_DEBUG, "\nAN1=");
                cbufPutWord(CIRBUF_TX_DEBUG, adcReadChan(ADC_CH_A1));
                cbufPutString(CIRBUF_TX_DEBUG, " AN2=");
                cbufPutWord(CIRBUF_TX_DEBUG, adcReadChan(ADC_CH_A2));
                cbufPutString(CIRBUF_TX_DEBUG, " AN3=");
                cbufPutWord(CIRBUF_TX_DEBUG, adcReadChan(ADC_CH_A3));
            }
            //'anon' = ADC Converter ON
            else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "anon") == 0) {
                _AD1IF = 0;     //Clear interrupt flag
                _AD1IE = 1;     // Enable A/D conversion interrupt
                _ADON = 1;      //Turn ADC converter on
                _ASAM = 1;      //Kick start
                cbufPutString(CIRBUF_TX_DEBUG, "\nADC ON");
            }
            //'anoff' = ADC Converter OFF
            else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "anoff") == 0) {
                _AD1IE = 0;     // Disable A/D conversion interrupt
                _ADON = 0;      //Turn ADC converter off
                cbufPutString(CIRBUF_TX_DEBUG, "\nADC OFF");
            }
            #endif  //#if !defined(RELEASE_BUILD)
        }
        else if (firstChar=='c') {
            //'ca0' = Config A0
            /*
            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "ca0") == 0) {
                UCPORT_CONFIG pconf;

                cfgGetDfltStruct(ucPort.B0, pconf);
                sprintf(debugTempBuf, "\nPrtB0.flags.val=%x, prtB0.type=%x", pconf.flags.val, (pconf.type<<8)&&pconf.typeConf);
                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
            }
            */
            //The code below is used to return info for the CFG_BLOCK_XXX structures. It also checks that each CFG_BLOCK_XXX
            //structure  is a 32 byte multiple.
            #if !defined(RELEASE_BUILD)
            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "cfgInfo") == 0) {
//                #define SZCBH sizeof(CFG_BLOCK_HDR)
//                //sprintf(debugTempBuf, "\noffsetof(CFG_STRUCT, ser.avail) = %d", offsetof(CFG_STRUCT, ser.avail));
//                //cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                //sprintf(debugTempBuf, "\noffsetof(CFG_STRUCT, ser.avail) = %d", offsetof(CFG_STRUCT, ser.avail));
//                //cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                cbufPutString(CIRBUF_TX_DEBUG, "\nInfo for CFG_BLOCK_xxx structures.");
//
//                sprintf(debugTempBuf, "\nCFG_STRUCT size = %d", CFG_STRUCT_CONFSIZE);
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//
//                sprintf(debugTempBuf, "\nCFG_STRUCT_INFO size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_INFO), sizeof(CFG_BLOCK_INFO) - sizeof(((CFG_BLOCK_INFO*)0)->reserve), (((sizeof(CFG_BLOCK_INFO))%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_INFO))%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_NET size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_NET)+SZCBH, sizeof(CFG_BLOCK_NET) - sizeof(((CFG_BLOCK_NET*)0)->reserve), (((sizeof(CFG_BLOCK_NET)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_NET)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_WIFI size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_WIFI)+SZCBH, sizeof(CFG_BLOCK_WIFI) - sizeof(((CFG_BLOCK_WIFI*)0)->reserve), (((sizeof(CFG_BLOCK_WIFI)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_WIFI)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_SNMP size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_SNMP)+SZCBH, sizeof(CFG_BLOCK_SNMP) - sizeof(((CFG_BLOCK_SNMP*)0)->reserve), (((sizeof(CFG_BLOCK_SNMP)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_SNMP)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_GENERAL size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_GENERAL)+SZCBH, sizeof(CFG_BLOCK_GENERAL) - sizeof(((CFG_BLOCK_GENERAL*)0)->reserve), (((sizeof(CFG_BLOCK_GENERAL)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_GENERAL)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_NET2 size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_NET2)+SZCBH, sizeof(CFG_BLOCK_NET2) - sizeof(((CFG_BLOCK_NET2*)0)->reserve), (((sizeof(CFG_BLOCK_NET2)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_NET2)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_SER_0TO7 size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_SER_0TO7)+SZCBH, sizeof(CFG_BLOCK_SER_0TO7) - sizeof(((CFG_BLOCK_SER_0TO7*)0)->reserve), (((sizeof(CFG_BLOCK_SER_0TO7)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_SER_0TO7)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_SER_8TO15 size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_SER_8TO15)+SZCBH, sizeof(CFG_BLOCK_SER_8TO15) - sizeof(((CFG_BLOCK_SER_8TO15*)0)->reserve), (((sizeof(CFG_BLOCK_SER_8TO15)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_SER_8TO15)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_SER_16TO23 size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_SER_16TO23)+SZCBH, sizeof(CFG_BLOCK_SER_16TO23) - sizeof(((CFG_BLOCK_SER_16TO23*)0)->reserve), (((sizeof(CFG_BLOCK_SER_16TO23)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_SER_16TO23)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_SER_24TO31 size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_SER_24TO31)+SZCBH, sizeof(CFG_BLOCK_SER_24TO31) - sizeof(((CFG_BLOCK_SER_24TO31*)0)->reserve), (((sizeof(CFG_BLOCK_SER_24TO31)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_SER_24TO31)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_UC66_PORT size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_UC66_PORT)+SZCBH, sizeof(CFG_BLOCK_UC66_PORT) - sizeof(((CFG_BLOCK_UC66_PORT*)0)->reserve), (((sizeof(CFG_BLOCK_UC66_PORT)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_UC66_PORT)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_SBC66_PORT size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_SBC66_PORT)+SZCBH, sizeof(CFG_BLOCK_SBC66_PORT) - sizeof(((CFG_BLOCK_SBC66_PORT*)0)->reserve), (((sizeof(CFG_BLOCK_SBC66_PORT)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_SBC66_PORT)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nCFG_BLOCK_STRING_31X8 size = %d, used %d, add %d filling", sizeof(CFG_BLOCK_STRING_31X8)+SZCBH, sizeof(CFG_BLOCK_STRING_31X8) - sizeof(((CFG_BLOCK_STRING_31X8*)0)->reserve), (((sizeof(CFG_BLOCK_STRING_31X8)+SZCBH)%32)==0) ? 0 : 32-((sizeof(CFG_BLOCK_STRING_31X8)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//
//                /* TABLE_BLOCKs do not have to be on boundaries
//                sprintf(debugTempBuf, "\nTABLE_BLOCK_UC66_PORT size = %d, used %d, add %d filling", sizeof(TABLE_BLOCK_UC66_PORT)+SZCBH, sizeof(TABLE_BLOCK_UC66_PORT) - sizeof(((TABLE_BLOCK_UC66_PORT*)0)->reserve), (((sizeof(TABLE_BLOCK_UC66_PORT)+SZCBH)%32)==0) ? 0 : 32-((sizeof(TABLE_BLOCK_UC66_PORT)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                sprintf(debugTempBuf, "\nTABLE_BLOCK_SBC66_PORT size = %d, used %d, add %d filling", sizeof(TABLE_BLOCK_SBC66_PORT)+SZCBH, sizeof(TABLE_BLOCK_SBC66_PORT) - sizeof(((TABLE_BLOCK_SBC66_PORT*)0)->reserve), (((sizeof(TABLE_BLOCK_SBC66_PORT)+SZCBH)%32)==0) ? 0 : 32-((sizeof(TABLE_BLOCK_SBC66_PORT)+SZCBH)%32) );
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                */
            }
            #endif
        }
        else if (firstChar=='e') {
            #if !defined(RELEASE_BUILD)
            debugStrLen = cbufGetContiguousPacket(CIRBUF_RX_DEBUG, (BYTE**)&pData);
            #if defined(INCLUDE_EXAMPLES)
            //'exnn_d', where nn is example number from 01 to 99, and d is number from 0 to 9
            if (debugStrLen==6 && pData[1]=='x' && pData[4]=='_') {
                BYTE doThis=pData[5];
                //ex01_n
                if (pData[2]=='0' && pData[3]=='1') {
                    ex01_SerportI2C1Send(doThis);

                    cbufPutString(CIRBUF_TX_DEBUG, "\nCalling ex01_SerportI2C1Send(");
                    cbufPutByte(CIRBUF_TX_DEBUG, doThis);
                    cbufPutByte(CIRBUF_TX_DEBUG, ')');
                }
            }
            #endif
            #endif
        }
        else if (firstChar=='h') {
            #if !defined(RELEASE_BUILD)
            //"hi" Debug message received - reply with "Hello" on a new line
            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "hi") == 0) {
                cbufPutString(CIRBUF_TX_DEBUG, "\nHello");
            }
            #endif
        }
        else if (firstChar=='i') {
            //"ip" Debug message received - reply with our IP address
            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "ip") == 0) {
                //If not using any printf functions anywhere in project, to total code size is much less
                //sprintf(debugTempBuf, "\nIP Address: %d.%d.%d.%d", AppConfig.MyIPAddr.v[0], AppConfig.MyIPAddr.v[1], AppConfig.MyIPAddr.v[2], AppConfig.MyIPAddr.v[3]);
                //cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
                debugPutString("\nIP Address: ");
                debugPrintIP();
            }
            //'invalidate xeeprom'
            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "invalidate xeeprom") == 0) {
                cfgInvalidateXeeprom();
                cbufPutString(CIRBUF_TX_DEBUG, "\nDone");
            }
        }
        else if (firstChar=='t') {
            #if !defined(RELEASE_BUILD)
            //'t1'
            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t1") == 0) {
                //Write message to slave at address 0x50 on I2C 1 bus, using using i2c1WriteMessage()
                BYTE bufMsg[] = {0x80, 0x0C, 'H', 'i', ' ', 'T', 'h', 'e', 'r', 'e'};
                i2c1WriteMessage(0x50, bufMsg, strlen((const char*)bufMsg));
                i2c1GetStatusWait(0);  //Wait for transmission to finish so we can get status information
                debugPutString("\nTXION Status(0x50) = ");
                debugPutByte(i2c1GetStatus(0x50));
            }
            //'t2'
            else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t2") == 0) {
                //Write a message to I2C 1 at address 0x50.
                i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
                i2c1Write(0x80);                //Write 0x80
                i2c1Write(0x0C);                //Write 0x0C
                i2c1WriteString("Gday");        //Write "Gday" string
                i2c1EndTransmission();          //End transmission, and start sending message in background
                i2c1GetStatusWait(0);           //Wait till background transmission finished
                debugPutString("\nTXION Status(0) = ");
                debugPutByte(i2c1GetStatus(0)); //Get Status of transmission
            }
            //'t3'
            else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t3") == 0) {
                //Write a message to I2C 1 at address 0x50.
                i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
                i2c1Write(0xD0);                //Write 0xD0 = Status register
                i2c1Read(1);                    //Read 1 byte
                i2c1EndTransmission();          //End transmission, and start sending message in background
                i2c1GetStatusWait(0);           //Wait till background transmission finished
                if (i2c1GetStatus(0)==0) {
                    if (i2c1HasData()) {
                        debugPutString("\nRead: ");
                        while (i2c1HasData()) {
                            debugPutByte(i2c1GetByte());
                            debugPutChar(',');
                        }
                    }
                }
                else {
                    debugPutString("\nTXION ERROR!");
                }
            }
            //'t4'
            else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t4") == 0) {
                //Write a message to I2C 1 at address 0xDE (RTC).
                i2c1BeginTransmission(0xDE);    //Start transmission to slave at address 0xDE
                i2c1Write(0x12);                //Write 0x12 = User RAM
                i2c1Read(2);                    //Read 2 bytes
                i2c1EndTransmission();          //End transmission, and start sending message in background
                //Wait till transmission finished (background via interrupt), and get status
                if (i2c1GetStatusWait(0)==0) {
                    if (i2c1HasData()) {
                        debugPutString("\nRead: ");
                        while (i2c1HasData()) {
                            debugPutByte(i2c1GetByte());
                            debugPutChar(',');
                        }
                    }
                }
                else {
                    debugPutString("\nTXION ERROR!");
                }
            }
            //'t5'
            else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t5") == 0) {
                BYTE buf[8];
                //Write a message to I2C 1 at address 0xDE (RTC).
                i2c1BeginTransmission(0xDE);    //Start transmission to slave at address 0xDE
                i2c1Write(0x12);                //Write 0x12 = User RAM address
                i2c1ReadToArray(buf, 2);       //Read 2 bytes to buf
                i2c1EndTransmission();          //End transmission, and start sending message in background
                //Wait till transmission finished (background via interrupt), and get status
                if (i2c1GetStatusWait(0)==0) {
                    debugPutString("\nRead: ");
                    debugPutByte(buf[0]);
                    debugPutChar(',');
                    debugPutByte(buf[1]);
                }
                else {
                    debugPutString("\nTXION ERROR!");
                }
            }
            //'t6'
            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t6") == 0) {
                //Write "ASCII String, with Escape Sequences" to I2C 1 port
                i2c1WriteAsciiEscString("50800C'Hello'0A'World'");
                if (i2c1GetStatusWait(0)!=0) {
                    debugPutString("\nTXION ERROR!");
                }
            }
            //'t7'
            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t7") == 0) {
                //Write "ASCII String, with Escape Sequences" to I2C 1 port
                i2c1WriteAsciiEscString("50800C'GDay'0A'Mate!'");
                i2c1WriteAsciiEscString("6080A7");
                if (i2c1GetStatusWait(0)!=0) {
                    debugPutString("\nTXION ERROR!");
                }
            }
            #endif

//            #if !defined(RELEASE_BUILD)
//            //'t1' = Test 1
//            if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t1") == 0) {
//                //
//                cbufEmpty(CIRBUF_TX_I2C1);
//                cbufPutString(CIRBUF_TX_DEBUG, "\nEmpty CIRBUF_TX_I2C1");
//
//                //Test cfgSaveArray() and cfgGetArray() functions. A structure in CFG_STRUCT is changed. Next cfgSaveArray() is called.
//                //This saves copy of changed structure in CFG_COPY(external EEPROM), and actual structure in CFG_STRUCT(external EEPROM).
//                //The cfgGetArray() function reads current value from CFG_STRUCT, which should now contain the updated value.
//                /*
//                UCPORT_CONFIG pconf;
//                pconf.type = 0x12;
//                pconf.id = 0x34;
//                cfgSaveArray(offsetof(CFG_STRUCT, ucPort.B0), (BYTE *)&pconf, sizeof(pconf));
//                cfgGetArray(offsetof(CFG_STRUCT, ucPort.B0), (BYTE *)&pconf, sizeof(pconf));
//                sprintf(debugTempBuf, "\ntype=%x, id=%x", pconf.type, pconf.id);
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//
//                pconf.type = 0xab;
//                pconf.id = 0xcd;
//                cfgSaveArray(offsetof(CFG_STRUCT, ucPort.B0), (BYTE *)&pconf, sizeof(pconf));
//                cfgGetArray(offsetof(CFG_STRUCT, ucPort.B0), (BYTE *)&pconf, sizeof(pconf));
//                sprintf(debugTempBuf, "\ntype=%x, id=%x", pconf.type, pconf.id);
//                cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
//                */
//            }
//            //'t2'
//            else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t2") == 0) {
//                /*
//                if (cfgStruct.info.eeConfUpdated == 0xff) {
//                    cbufPutString(CIRBUF_TX_DEBUG, "Is 0xff");
//
//                    //Write 0 to given address in FLASH
//                    nzFlashWriteConst(__builtin_tbladdress(&cfgStruct.info.eeConfUpdated), 0);
//                    if (cfgStruct.info.eeConfUpdated == 0xff) {
//                        cbufPutString(CIRBUF_TX_DEBUG, "STILL 0xff!");
//                    }
//                    else {
//                        cbufPutString(CIRBUF_TX_DEBUG, "NOW 0!");
//                    }
//                }
//                else {
//                    cbufPutString(CIRBUF_TX_DEBUG, "Not 0xff");
//                }
//                */
//            }
//            //'t3'
//            else if (cbufPacketStrcmp(CIRBUF_RX_DEBUG, "t3") == 0) {
//                BYTE index;
//
//                //Get UCPORT_PROP for a IO Port (port of SBC) 1 (old port name X1), using UCPORT_xx defines
//                UCPORT_PROP ucportProp;
//                cfgGetDfltArray(GET_UCPORT_PROP_OFFSET(UCPORT_01), (BYTE *)&ucportProp, sizeof(ucportProp));
//                debugPutString("\nUCPORT_PROP.prop 1 (offset=0x");
//                debugPutHexWord(GET_UCPORT_PROP_OFFSET(UCPORT_01));
//                debugPutString(")=");
//                debugPutHexWord(ucportProp.prop.w[0]);
//
//                //Get UCPORT_CONFIG for a IO Port (port of SBC) 1 (old port name X1), using UCPORT_xx defines
//                UCPORT_CONFIG ucportConfig;
//                cfgGetArray(GET_UCPORT_CONFIG_OFFSET(UCPORT_01), (BYTE *)&ucportConfig, sizeof(ucportConfig));
//                debugPutString("\nUCPORT_CONFIG.type 1, offset=0x");
//                debugPutHexWord(GET_UCPORT_CONFIG_OFFSET(UCPORT_01));
//                debugPutString(", type=0x");
//                debugPutHexWord(ucportConfig.gen.type);
//
//                //Get UCPORT_PROP for a IO Port (port of SBC) 1 (old port name X1), using the "Port ID"
//                //UCPORT_PROP ucportProp;
//                index = ucportGetIndex(1);
//                if (index != UCPORT_INDEX_NA) {
//                    cfgGetDfltArray(offsetof(CFG_STRUCT, ucPortProp) + (index * sizeof(UCPORT_PROP)), (BYTE *)&ucportProp, sizeof(ucportProp));
//                    debugPutString("\nUCPORT_PROP.prop 1 (index=0x");
//                    debugPutHexWord(index);
//                    debugPutString(", offset=0x");
//                    debugPutHexWord(offsetof(CFG_STRUCT, ucPortProp) + (index * sizeof(UCPORT_PROP)));
//                    debugPutString(")=");
//                    debugPutHexWord(ucportProp.prop.w[0]);
//                }
//
//                //Get UCPORT_CONFIG for a IO Port (port of SBC) 1 (old port name X1), using the "Port ID"
//                //UCPORT_CONFIG ucportConfig;
//                index = ucportGetIndex(1);
//                if (index != UCPORT_INDEX_NA) {
//                    cfgGetArray(offsetof(CFG_STRUCT, ucPort) + (index * sizeof(UCPORT_CONFIG)), (BYTE *)&ucportConfig, sizeof(ucportConfig));
//                    debugPutString("\nUCPORT_CONFIG.type 1, offset=0x");
//                    debugPutHexWord(offsetof(CFG_STRUCT, ucPort) + (index * sizeof(UCPORT_CONFIG)));
//                    debugPutString(", type=0x");
//                    debugPutHexWord(ucportConfig.gen.type);
//                }
//            }
//            #endif
        }

        //Remove received packet. If it was not processed above, it is lost!
        cbufRemovePacket(CIRBUF_RX_DEBUG);
    }
#endif
}
#endif  //#if !defined(DISABLE_WEBSERVER_DEFAULT_DEBUGSERVICE)


/**
 * Write a single character to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 *
 * @param c Character to add to the buffer
 */
void debugPutChar(CHAR c)
{
    cbufPutByte(CIRBUF_TX_DEBUG, c);
}

/**
 * Write BYTE in decimal format to the debug output. For example, debugPutByte(100) will
 * write the string "100" to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 *
 * @param b Byte to add to the buffer
 */
void debugPutByte(BYTE b)
{
    cbufPutByteAscii(CIRBUF_TX_DEBUG, b);
}


/**
 * Write WORD in decimal format to the debug output. For example, debugPutWord(100) will
 * write the string "100" to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 *
 * @param w Word to add to the buffer
 */
void debugPutWord(WORD w) {
    cbufPutWordAscii(CIRBUF_TX_DEBUG, w);
}


/**
 * Adds given number of bytes to debug port. If no more space in debug port
 * buffer, function returns without doing anything.
 *
 * @param pArr Pointer to source BYTE array
 * @param size Number of bytes to copy
 */
void debugPutArray(const BYTE* pArr, WORD size)
{
    cbufPutArray(CIRBUF_TX_DEBUG, pArr, size);
}


/**
 * Send given NULL terminated string to debug port. If no more space in debug port
 * buffer, function returns without doing anything.
 *
 * @param str Null terminated string to add
 */
void debugPutString(const char * str) {
    cbufPutString(CIRBUF_TX_DEBUG, str);
}


/**
 * Write the given BYTE as a 2-character "ASCII Formatted Hex" string to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 * @param b Byte to write
 */
void debugPutHexByte(BYTE b) {
    cbufPutByteAsciiHex(CIRBUF_TX_DEBUG, b);
}


/**
 * Write the given WORD as a 4-character "ASCII Formatted Hex" string to the debug output.
 * If no more space in debug port buffer, function returns without doing anything.
 * @param w Word to write
 */
void debugPutHexWord(WORD w) {
    cbufPutWordAsciiHex(CIRBUF_TX_DEBUG, w);
}


void debugPutHexLine(DWORD adr, BYTE* buf, BYTE len) {
    BYTE i;

    //Print Address - 11 bytes buffer space required
    cbufPutByte(CIRBUF_TX_DEBUG, '\n');
    debugPutHexWord( ((WORD)(adr>>16)) );
    debugPutHexWord(((WORD)adr));
    cbufPutByte(CIRBUF_TX_DEBUG, ':');
    cbufPutByte(CIRBUF_TX_DEBUG, ' ');

    //Print line, maximum 16 bytes
    i=0;
    while (i<len) {
        cbufPutByte(CIRBUF_TX_DEBUG, btohexa_high(buf[i]));
        cbufPutByte(CIRBUF_TX_DEBUG, btohexa_low(buf[i]));
        if (len > ++i)
            cbufPutByte(CIRBUF_TX_DEBUG, ' ');
    }
}


/**
 * Prints the contents of the FLASH to the debug port.
 * @param address
 * @param length
 */
void debugPrintFlash(DWORD address, WORD length) {
    BYTE buf[16];
    BYTE len;

    len = 16 - (((BYTE)address) & 0x0f);
    if (length < len)
        len = length;

    while ((len != 0) && (cbufGetFree(CIRBUF_TX_DEBUG) > (11 + len*2))) {
        xflashReadArray(address, buf, len);

        debugPutHexLine(address, buf, len);

        address += len;
        length -= len;
        len = 16;
        if (length < len)
            len = length;
    }
}

/**
 * Print IP address to debug port
 */
void debugPrintIP(void) {
    debugPutByte(AppConfig.MyIPAddr.v[0]);
    debugPutChar('.');
    debugPutByte(AppConfig.MyIPAddr.v[1]);
    debugPutChar('.');
    debugPutByte(AppConfig.MyIPAddr.v[2]);
    debugPutChar('.');
    debugPutByte(AppConfig.MyIPAddr.v[3]);
}

#endif  //#if defined(HAS_SERPORT_DEBUG)


