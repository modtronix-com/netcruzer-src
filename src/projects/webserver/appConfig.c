/*********************************************************************
 * appConfig.c    - Application Configuration
 * Dependencies:    
 * Complier:        MPLAB XC16
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Application configuration.
 *
 *********************************************************************
 * File History
 *
 * 2010-11-16, David H. (DH):
 *    - Initial version
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
 *
 ********************************************************************/
#define THIS_IS_APPCONFIG_C

#include "HardwareProfile.h"
#include "appConfig.h"

#include "TCPIPConfig.h"
#include "TCPIP Stack/Helpers.h"

#include "nz_xEeprom.h"
#include "nz_xFlash.h"

//Add debugging to this file. The DEBUG_CONF_APPCONFIG macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_APPCONFIG)
    #define DEBUG_CONF_APPCONFIG        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_APPCONFIG
#include "nz_debug.h"
#include "cmd.h"

#if (XEEMEM_CFG_STRUCT_END_ADR > XEEPROM_SIZE)
    #error "CFG_STRUCT too large for External EEPROM!"
#endif

#if ( (SNMP_MAX_COMMUNITY_SUPPORT > 3) || (SNMP_COMMUNITY_MAX_LEN > 8) )
    #error "SNMP_MAX_COMMUNITY_SUPPORT must be 3 or less, and SNMP_COMMUNITY_MAX_LEN must be 8 or less!"
#endif


/////////////////////////////////////////////////
//Variables
APP_CONFIG      AppConfig;


//Place CFG_STRUCT in hex file location 0x1000000 (0x800000 x 2 = 0x1000000 byte address). Only the
//even word addresses are used to place data. For example, following code:
//
//unsigned char eeprom_space[] __attribute__((space(prog), address(0x800000))) = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde};
//
//Will create following entry in hex file:
//
//:020000040100f9
//:1000000012340000567800009abc0000de000000a8
//
CFG_STRUCT __attribute__((space(prog), address(0x800000))) cfgStructHexfile = {
    /////////////////////////////////////////////////
    //Info Settings
    .info.hdr.id = CFG_BLOCK_INFO_ID,
    .info.hdr.size = sizeof(CFG_BLOCK_INFO) / 16,
    .info.hdr.reserved = (sizeof(CFG_BLOCK_INFO) - sizeof(((CFG_BLOCK_INFO*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .info.eeConfUpdated = 0xff,
    .info.confSize = CFG_STRUCT_CONFSIZE,


    /////////////////////////////////////////////////
    //Network Settings
    .netHdr.id = CFG_BLOCK_NET_ID,
    .netHdr.size = sizeof(CFG_BLOCK_NET) / 16,
    .netHdr.reserved = (sizeof(CFG_BLOCK_NET) - sizeof(((CFG_BLOCK_NET*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .net.MyIPAddr.Val = (MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2 << 8ul | MY_DEFAULT_IP_ADDR_BYTE3 << 16ul | MY_DEFAULT_IP_ADDR_BYTE4 << 24ul),
    .net.MyMask.Val = (MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2 << 8ul | MY_DEFAULT_MASK_BYTE3 << 16ul | MY_DEFAULT_MASK_BYTE4 << 24ul),
    .net.MyGateway.Val = (MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2 << 8ul | MY_DEFAULT_GATE_BYTE3 << 16ul | MY_DEFAULT_GATE_BYTE4 << 24ul),
    .net.PrimaryDNSServer.Val = (MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2 << 8ul | MY_DEFAULT_PRIMARY_DNS_BYTE3 << 16ul | MY_DEFAULT_PRIMARY_DNS_BYTE4 << 24ul),
    .net.SecondaryDNSServer.Val = (MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2 << 8ul | MY_DEFAULT_SECONDARY_DNS_BYTE3 << 16ul | MY_DEFAULT_SECONDARY_DNS_BYTE4 << 24ul),
    .net.DefaultIPAddr.Val = (MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2 << 8ul | MY_DEFAULT_IP_ADDR_BYTE3 << 16ul | MY_DEFAULT_IP_ADDR_BYTE4 << 24ul),
    .net.DefaultMask.Val = (MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2 << 8ul | MY_DEFAULT_MASK_BYTE3 << 16ul | MY_DEFAULT_MASK_BYTE4 << 24ul),
    .net.NetBIOSName = MY_DEFAULT_HOST_NAME_,
    .net.Flags.bIsDHCPEnabled = MY_DEFAULT_DHCP_ENABLED,
    .net.Flags.bInConfigMode = 0,
    .net.MyMACAddr.v = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6},


    /////////////////////////////////////////////////
    //WiFi Settings - Currently not used. If enabled in future, this data need to be changed!!!!
    .wifiHdr.id = CFG_BLOCK_WIFI_ID,
    .wifiHdr.size = sizeof(CFG_BLOCK_WIFI) / 16,
    .wifiHdr.reserved = (sizeof(CFG_BLOCK_WIFI) - sizeof(((CFG_BLOCK_WIFI*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .wifi.MySSID = DEFAULT_WIFI_SSID,
    .wifi.SsidLength = DEFAULT_WIFI_SSID_LENGTH,
    .wifi.SecurityMode = DEFAULT_WIFI_SECURITY_MODE,
    .wifi.SecurityKey = DEFAULT_WIFI_SECURITY_KEY,
    .wifi.SecurityKeyLength = DEFAULT_WIFI_SECURITY_KEY_LENGTH,
    .wifi.WepKeyIndex = DEFAULT_WIFI_WEP_KEY_INDEX,
    .wifi.dataValid = DEFAULT_WIFI_DATA_VALID,
    .wifi.networkType = DEFAULT_WIFI_NETWORK_TYPE,
    .wifi.saveSecurityInfo = DEFAULT_WIFI_SAVED_SEC_INFO,


    /////////////////////////////////////////////////
    //SNMP Settings
    .snmpHdr.id = CFG_BLOCK_SNMP_ID,
    .snmpHdr.size = sizeof(CFG_BLOCK_SNMP) / 16,
    .snmpHdr.reserved = (sizeof(CFG_BLOCK_SNMP) - sizeof(((CFG_BLOCK_SNMP*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .snmp.readCommunity = SNMP_READ_COMMUNITIES,
    .snmp.writeCommunity = SNMP_WRITE_COMMUNITIES,
    .snmp.SnmpEngineBootRcrd = 0,


    /////////////////////////////////////////////////
    //General Settings
    .genHdr.id = CFG_BLOCK_GENERAL_ID,
    .genHdr.size = sizeof(CFG_BLOCK_GENERAL) / 16,
    .genHdr.reserved = (sizeof(CFG_BLOCK_GENERAL) - sizeof(((CFG_BLOCK_GENERAL*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .gen.username = {'a','d','m','i','n',0},
    .gen.password = {'m','i','c','r','o','c','h','i','p',0},


    /////////////////////////////////////////////////
    //Network 2 Settings
    .net2Hdr.id = CFG_BLOCK_NET2_ID,
    .net2Hdr.size = sizeof(CFG_BLOCK_NET2) / 16,
    .net2Hdr.reserved = (sizeof(CFG_BLOCK_NET2) - sizeof(((CFG_BLOCK_NET2*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .net2.Flags.bUdpCmd = 1,
    .net2.Flags.bTcpCmd = 0,
    .net2.Flags.bUdpEvents = 1,
    .net2.Flags.bTcpEvents = 0,
    .net2.udpCmdPort = DEFAULT_UDP_COMMAND_PORT,
    .net2.udpEventPort = DEFAULT_UDP_EVENT_PORT,
    .net2.tcpCmdPort = DEFAULT_TCP_COMMAND_PORT,
    .net2.tcpEventPort = DEFAULT_TCP_EVENT_PORT,


    /////////////////////////////////////////////////
    //"Serial Data Port" 0 to 7 Configuration - Following bits indicate which of "Serial Data Port" 0 to 7 are available
    .ser0to7Hdr.id = CFG_BLOCK_SER_0TO7_ID,
    .ser0to7Hdr.size = sizeof(CFG_BLOCK_SER_0TO7) / 16,
    .ser0to7Hdr.reserved = (sizeof(CFG_BLOCK_SER_0TO7) - sizeof(((CFG_BLOCK_SER_0TO7*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ser0to7.avail.bits.uart1 = 1,
    .ser0to7.avail.bits.uart2 = 1,
    .ser0to7.avail.bits.uart3 = 0,
    .ser0to7.avail.bits.uart4 = 0,
    .ser0to7.avail.bits.spi1 = 1,
    .ser0to7.avail.bits.spi3 = 1,
    .ser0to7.avail.bits.i2c1 = 1,
    .ser0to7.avail.bits.i2c2 = 1,
    .ser0to7.uart1 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.uart2 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.uart3 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.uart4 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.spi1 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.spi3 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.i2c1 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          I2C1_TXBUF_SIZE, I2C1_RXBUF_SIZE},
    .ser0to7.i2c2 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          I2C2_TXBUF_SIZE, I2C2_RXBUF_SIZE},


    /////////////////////////////////////////////////
    //"Serial Data Port" 8 to 15 Configuration - Following bits indicate which of "Serial Data Port" 8 to 15 are available
    .ser8to15Hdr.id = CFG_BLOCK_SER_8TO15_ID,
    .ser8to15Hdr.size = sizeof(CFG_BLOCK_SER_8TO15) / 16,
    .ser8to15Hdr.reserved = (sizeof(CFG_BLOCK_SER_8TO15) - sizeof(((CFG_BLOCK_SER_8TO15*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ser8to15.avail.bits.i2c3 = 1,
    .ser8to15.avail.bits.udp1 = 1,
    .ser8to15.avail.bits.udp2 = 1,
    .ser8to15.avail.bits.udp3 = 1,
    .ser8to15.avail.bits.tcp1 = 1,
    .ser8to15.avail.bits.tcp2 = 1,
    .ser8to15.avail.bits.tcp3 = 1,
    .ser8to15.avail.bits.usb = 1,
    .ser8to15.i2c3 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                           I2C2_TXBUF_SIZE, I2C2_RXBUF_SIZE},
    .ser8to15.udp1 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.udp2 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.udp3 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.tcp1 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.tcp2 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.tcp3 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.usb =  {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET,
                           32,32},   //32 TX Buffer, 32 RX buffer


    /////////////////////////////////////////////////
    //"Serial Data Port" 16 to 23 Configuration - Following bits indicate which of "Serial Data Port" 16 to 23 are available
    .ser16to23Hdr.id = CFG_BLOCK_SER_16TO23_ID,
    .ser16to23Hdr.size = sizeof(CFG_BLOCK_SER_16TO23) / 16,
    .ser16to23Hdr.reserved = (sizeof(CFG_BLOCK_SER_16TO23) - sizeof(((CFG_BLOCK_SER_16TO23*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ser16to23.avail.bits.user1 = 1,
    .ser16to23.avail.bits.user2 = 1,
    .ser16to23.avail.bits.user3 = 0,
    .ser16to23.avail.bits.user4 = 0,
    .ser16to23.avail.bits.user5 = 0,
    .ser16to23.avail.bits.user6 = 0,
    .ser16to23.avail.bits.user7 = 0,
    .ser16to23.avail.bits.user8 = 0,
    .ser16to23.user1 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user2 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user3 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user4 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user5 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user6 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user7 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user8 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer


    /////////////////////////////////////////////////
    //"Serial Data Port" 23 to 31 Configuration - Following bits indicate which of "Serial Data Port" 24 to 31 are available
    .ser24to31Hdr.id = CFG_BLOCK_SER_24TO31_ID,
    .ser24to31Hdr.size = sizeof(CFG_BLOCK_SER_24TO31) / 16,
    .ser24to31Hdr.reserved = (sizeof(CFG_BLOCK_SER_24TO31) - sizeof(((CFG_BLOCK_SER_24TO31*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ser24to31.avail.bits.res1 = 0,
    .ser24to31.avail.bits.res2 = 0,
    .ser24to31.avail.bits.res3 = 0,
    .ser24to31.avail.bits.res4 = 0,
    .ser24to31.avail.bits.res5 = 0,
    .ser24to31.avail.bits.res6 = 0,
    .ser24to31.avail.bits.res7 = 0,
    .ser24to31.avail.bits.res8 = 0,


    /////////////////////////////////////////////////
    //Microcontroller (UC) Port configuration, defines type of each port.
    // - type = Port type, is a PCFG_XXX_TYPE constant
    // - typeConf = is a PCFG_XXX_YYY constant
    // - def = Default value, valid for outputs like PWM, digital, ....
    .ucPortHdr.id = CFG_BLOCK_UC66_PORT_ID,
    .ucPortHdr.size = sizeof(CFG_BLOCK_UC66_PORT) / 16,
    .ucPortHdr.reserved = (sizeof(CFG_BLOCK_UC66_PORT) - sizeof(((CFG_BLOCK_UC66_PORT*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,

    #if defined(UC_PORT_B0_PROP)    //We have a port B0
    .ucPort.B0.din.id = UCPORT_ID_B0,
    .ucPort.B0.din.type = PCFG_DIN_TYPE,
    .ucPort.B0.din.typeConf = PCFG_DIN_3V3_IN,
    .ucPort.B0.gen.param[0] = 0,
    .ucPort.B0.gen.param[1] = 0,
    .ucPort.B0.gen.param[2] = 0,
    #else
    .ucPort.B0.din.id = UCPORT_ID_NA,
    #endif
    #if defined(UC_PORT_B1_PROP)    //We have a port B1
    .ucPort.B1.din.id = UCPORT_ID_B1,
    .ucPort.B1.din.type = PCFG_DIN_TYPE,
    .ucPort.B1.din.typeConf = PCFG_DIN_3V3_IN,
    .ucPort.B1.gen.param[0] = 0,
    .ucPort.B1.gen.param[1] = 0,
    .ucPort.B1.gen.param[2] = 0,
    #else
    .ucPort.B0.din.id = UCPORT_ID_NA,
    #endif

    /////////////////////////////////////////////////
    //IO Port (port of SBC) configuration, defines type of each port.
    .ioPortHdr.id = CFG_BLOCK_SBC66_PORT_ID,
    .ioPortHdr.size = sizeof(CFG_BLOCK_SBC66_PORT) / 16,
    .ioPortHdr.reserved = (sizeof(CFG_BLOCK_SBC66_PORT) - sizeof(((CFG_BLOCK_SBC66_PORT*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,

    .ioPort.P00.label = 0xff,


    /////////////////////////////////////////////////
    //String block with 31x8 character strings.
    .str8aHdr.id = CFG_BLOCK_STRING_31X8_ID,
    .str8aHdr.size = sizeof(CFG_BLOCK_STRING_31X8) / 16,
    .str8aHdr.reserved = (sizeof(CFG_BLOCK_STRING_31X8) - sizeof(((CFG_BLOCK_STRING_31X8*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .str8a.str8[0][0] = 0,
    .str8a.str8[1][0] = 0,
    .str8a.str8[2][0] = 0,
    .str8a.str8[3][0] = 0,
    .str8a.str8[4][0] = 0,
    .str8a.str8[5][0] = 0,
    .str8a.str8[6][0] = 0,
    .str8a.str8[7][0] = 0,
    .str8a.str8[8][0] = 0,
    .str8a.str8[9][0] = 0,
    .str8a.str8[10][0] = 0,
    .str8a.str8[11][0] = 0,
    .str8a.str8[12][0] = 0,
    .str8a.str8[13][0] = 0,
    .str8a.str8[14][0] = 0,
    .str8a.str8[15][0] = 0,
    .str8a.str8[16][0] = 0,
    .str8a.str8[17][0] = 0,
    .str8a.str8[18][0] = 0,
    .str8a.str8[19][0] = 0,
    .str8a.str8[20][0] = 0,
    .str8a.str8[21][0] = 0,
    .str8a.str8[22][0] = 0,
    .str8a.str8[23][0] = 0,
    .str8a.str8[24][0] = 0,
    .str8a.str8[25][0] = 0,
    .str8a.str8[26][0] = 0,
    .str8a.str8[27][0] = 0,
    .str8a.str8[28][0] = 0,
    .str8a.str8[29][0] = 0,
    .str8a.str8[30][0] = 0,


    /////////////////////////////////////////////////
    //Constant TABLE_BLOCKs start from here!

    /////////////////////////////////////////////////
    //Microcontroller (UC) Port properties
    .ucPortPropHdr.id = TABLE_BLOCK_UC66_PORT_ID,
    .ucPortPropHdr.size = sizeof(TABLE_BLOCK_UC66_PORT) / 16,
    .ucPortPropHdr.reserved = (sizeof(TABLE_BLOCK_UC66_PORT) - sizeof(((TABLE_BLOCK_UC66_PORT*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ucPortProp.B0.prop.Val = UC_PORT_B0_PROP,
    .ucPortProp.B1.prop.Val = UC_PORT_B1_PROP,
    .ucPortProp.B2.prop.Val = UC_PORT_B2_PROP,
    .ucPortProp.B3.prop.Val = UC_PORT_B3_PROP,
    .ucPortProp.B4.prop.Val = UC_PORT_B4_PROP,
    .ucPortProp.B5.prop.Val = UC_PORT_B5_PROP,
    .ucPortProp.B6.prop.Val = UC_PORT_B6_PROP,
    .ucPortProp.B7.prop.Val = UC_PORT_B7_PROP,
    .ucPortProp.B8.prop.Val = UC_PORT_B8_PROP,
    .ucPortProp.B9.prop.Val = UC_PORT_B9_PROP,
    .ucPortProp.B10.prop.Val = UC_PORT_B10_PROP,
    .ucPortProp.B11.prop.Val = UC_PORT_B11_PROP,
    #if defined(UC_PORT_B12_PROP)
    .ucPortProp.B12.prop.Val = UC_PORT_B12_PROP,
    #endif
    #if defined(UC_PORT_B13_PROP)
    .ucPortProp.B13.prop.Val = UC_PORT_B13_PROP,
    #endif
    #if defined(UC_PORT_B14_PROP)
    .ucPortProp.B14.prop.Val = UC_PORT_B14_PROP,
    #endif
    #if defined(UC_PORT_B15_PROP)
    .ucPortProp.B15.prop.Val = UC_PORT_B15_PROP,
    #endif

    .ucPortProp.D0.prop.Val = UC_PORT_D0_PROP,
    .ucPortProp.D1.prop.Val = UC_PORT_D1_PROP,
    .ucPortProp.D2.prop.Val = UC_PORT_D2_PROP,
    .ucPortProp.D3.prop.Val = UC_PORT_D3_PROP,
    #if defined(UC_PORT_D6_PROP)
    .ucPortProp.D6.prop.Val = UC_PORT_D6_PROP,
    #endif
    #if defined(UC_PORT_D7_PROP)
    .ucPortProp.D7.prop.Val = UC_PORT_D7_PROP,
    #endif
    #if defined(UC_PORT_D8_PROP)
    .ucPortProp.D8.prop.Val = UC_PORT_D8_PROP,
    #endif
    #if defined(UC_PORT_D9_PROP)
    .ucPortProp.D9.prop.Val = UC_PORT_D9_PROP,
    #endif
    #if defined(UC_PORT_D10_PROP)
    .ucPortProp.D10.prop.Val = UC_PORT_D10_PROP,
    #endif

    #if defined(UC_PORT_F0_PROP)
    .ucPortProp.F0.prop.Val = UC_PORT_F0_PROP,
    #endif
    #if defined(UC_PORT_F1_PROP)
    .ucPortProp.F1.prop.Val = UC_PORT_F1_PROP,
    #endif
    #if defined(UC_PORT_F3_PROP)
    .ucPortProp.F3.prop.Val = UC_PORT_F3_PROP,
    #endif
    #if defined(UC_PORT_F4_PROP)
    .ucPortProp.F4.prop.Val = UC_PORT_F4_PROP,
    #endif
    #if defined(UC_PORT_F5_PROP)
    .ucPortProp.F5.prop.Val = UC_PORT_F5_PROP,
    #endif

    .ucPortProp.G6.prop.Val = UC_PORT_G6_PROP,
    .ucPortProp.G7.prop.Val = UC_PORT_G7_PROP,
    .ucPortProp.G8.prop.Val = UC_PORT_G8_PROP,
    .ucPortProp.G9.prop.Val = UC_PORT_G9_PROP,


    /////////////////////////////////////////////////
    //IO Port (ports of SBC) properties for all "Microcontroller Ports"
    .ioPortPropHdr.id = TABLE_BLOCK_SBC66_PORT_ID,
    .ioPortPropHdr.size = sizeof(TABLE_BLOCK_SBC66_PORT) / 16,
    .ioPortPropHdr.reserved = (sizeof(TABLE_BLOCK_SBC66_PORT) - sizeof(((TABLE_BLOCK_SBC66_PORT*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    #ifdef UCPORT_24
    .ioPortProp.P24.name = {'S','0',0},
    .ioPortProp.P24.ucPortIndex = GET_UCPORT_INDEX(UCPORT_24),
    #else
    .ioPortProp.P24.name = {'N','A',0},
    .ioPortProp.P24.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_25
    .ioPortProp.P25.name = {'S','1',0},
    .ioPortProp.P25.ucPortIndex = GET_UCPORT_INDEX(UCPORT_25),
    #else
    .ioPortProp.P25.name = {'N','A',0},
    .ioPortProp.P25.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_13
    .ioPortProp.P13.name = {'S','C',0},
    .ioPortProp.P13.ucPortIndex = GET_UCPORT_INDEX(UCPORT_13),
    #else
    .ioPortProp.P13.name = {'N','A',0},
    .ioPortProp.P13.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_12
    .ioPortProp.P12.name = {'S','D',0},
    .ioPortProp.P12.ucPortIndex = GET_UCPORT_INDEX(UCPORT_12),
    #else
    .ioPortProp.P12.name = {'N','A',0},
    .ioPortProp.P12.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_20
    .ioPortProp.P20.name = {'T','0',0},
    .ioPortProp.P20.ucPortIndex = GET_UCPORT_INDEX(UCPORT_20),
    #else
    .ioPortProp.P20.name = {'N','A',0},
    .ioPortProp.P20.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_21
    .ioPortProp.P21.name = {'T','1',0},
    .ioPortProp.P21.ucPortIndex = GET_UCPORT_INDEX(UCPORT_21),
    #else
    .ioPortProp.P21.name = {'N','A',0},
    .ioPortProp.P21.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_22
    .ioPortProp.P22.name = {'T','2',0},
    .ioPortProp.P22.ucPortIndex = GET_UCPORT_INDEX(UCPORT_22),
    #else
    .ioPortProp.P22.name = {'N','A',0},
    .ioPortProp.P22.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_23
    .ioPortProp.P23.name = {'T','3',0},
    .ioPortProp.P23.ucPortIndex = GET_UCPORT_INDEX(UCPORT_23),
    #else
    .ioPortProp.P23.name = {'N','A',0},
    .ioPortProp.P23.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_26
    .ioPortProp.P26.name = {'T','4',0},
    .ioPortProp.P26.ucPortIndex = GET_UCPORT_INDEX(UCPORT_26),
    #else
    .ioPortProp.P26.name = {'N','A',0},
    .ioPortProp.P26.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_27
    .ioPortProp.P27.name = {'T','5',0},
    .ioPortProp.P27.ucPortIndex = GET_UCPORT_INDEX(UCPORT_27),
    #else
    .ioPortProp.P27.name = {'N','A',0},
    .ioPortProp.P27.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_28
    .ioPortProp.P28.name = {'T','6',0},
    .ioPortProp.P28.ucPortIndex = GET_UCPORT_INDEX(UCPORT_28),
    #else
    .ioPortProp.P28.name = {'N','A',0},
    .ioPortProp.P28.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_29
    .ioPortProp.P29.name = {'T','7',0},
    .ioPortProp.P29.ucPortIndex = GET_UCPORT_INDEX(UCPORT_29),
    #else
    .ioPortProp.P29.name = {'N','A',0},
    .ioPortProp.P29.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_00
    .ioPortProp.P00.name = {'X','0',0},
    .ioPortProp.P00.ucPortIndex = GET_UCPORT_INDEX(UCPORT_00),
    #else
    .ioPortProp.P00.name = {'N','A',0},
    .ioPortProp.P00.ucPortIndex = UCPORT_INDEX_NA,
    #endif
    
    #ifdef UCPORT_01
    .ioPortProp.P01.name = {'X','1',0},
    .ioPortProp.P01.ucPortIndex = GET_UCPORT_INDEX(UCPORT_01),
    #else
    .ioPortProp.P01.name = {'N','A',0},
    .ioPortProp.P01.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_02
    .ioPortProp.P02.name = {'X','2',0},
    .ioPortProp.P02.ucPortIndex = GET_UCPORT_INDEX(UCPORT_02),
    #else
    .ioPortProp.P02.name = {'N','A',0},
    .ioPortProp.P02.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_03
    .ioPortProp.P03.name = {'X','3',0},
    .ioPortProp.P03.ucPortIndex = GET_UCPORT_INDEX(UCPORT_03),
    #else
    .ioPortProp.P03.name = {'N','A',0},
    .ioPortProp.P03.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_04
    .ioPortProp.P04.name = {'X','4',0},
    .ioPortProp.P04.ucPortIndex = GET_UCPORT_INDEX(UCPORT_04),
    #else
    .ioPortProp.P04.name = {'N','A',0},
    .ioPortProp.P04.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_05
    .ioPortProp.P05.name = {'X','5',0},
    .ioPortProp.P05.ucPortIndex = GET_UCPORT_INDEX(UCPORT_05),
    #else
    .ioPortProp.P05.name = {'N','A',0},
    .ioPortProp.P05.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_06
    .ioPortProp.P06.name = {'X','6',0},
    .ioPortProp.P06.ucPortIndex = GET_UCPORT_INDEX(UCPORT_06),
    #else
    .ioPortProp.P06.name = {'N','A',0},
    .ioPortProp.P06.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_07
    .ioPortProp.P07.name = {'X','7',0},
    .ioPortProp.P07.ucPortIndex = GET_UCPORT_INDEX(UCPORT_07),
    #else
    .ioPortProp.P07.name = {'N','A',0},
    .ioPortProp.P07.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_08
    .ioPortProp.P08.name = {'X','8',0},
    .ioPortProp.P08.ucPortIndex = GET_UCPORT_INDEX(UCPORT_08),
    #else
    .ioPortProp.P08.name = {'N','A',0},
    .ioPortProp.P08.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_09
    .ioPortProp.P09.name = {'X','9',0},
    .ioPortProp.P09.ucPortIndex = GET_UCPORT_INDEX(UCPORT_09),
    #else
    .ioPortProp.P09.name = {'N','A',0},
    .ioPortProp.P09.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_10
    .ioPortProp.P10.name = {'X','1','0'},
    .ioPortProp.P10.ucPortIndex = GET_UCPORT_INDEX(UCPORT_10),
    #else
    .ioPortProp.P10.name = {'N','A',0},
    .ioPortProp.P10.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_11
    .ioPortProp.P11.name = {'X','1','1'},
    .ioPortProp.P11.ucPortIndex = GET_UCPORT_INDEX(UCPORT_11),
    #else
    .ioPortProp.P11.name = {'N','A',0},
    .ioPortProp.P11.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_14
    .ioPortProp.P14.name = {'X','1','2'},
    .ioPortProp.P14.ucPortIndex = GET_UCPORT_INDEX(UCPORT_14),
    #else
    .ioPortProp.P14.name = {'N','A',0},
    .ioPortProp.P14.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_15
    .ioPortProp.P15.name = {'X','1','3'},
    .ioPortProp.P15.ucPortIndex = GET_UCPORT_INDEX(UCPORT_15),
    #else
    .ioPortProp.P15.name = {'N','A',0},
    .ioPortProp.P15.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_16
    .ioPortProp.P16.name = {'X','1','4'},
    .ioPortProp.P16.ucPortIndex = GET_UCPORT_INDEX(UCPORT_16),
    #else
    .ioPortProp.P16.name = {'N','A',0},
    .ioPortProp.P16.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_17
    .ioPortProp.P17.name = {'X','1','5'},
    .ioPortProp.P17.ucPortIndex = GET_UCPORT_INDEX(UCPORT_17),
    #else
    .ioPortProp.P17.name = {'N','A',0},
    .ioPortProp.P17.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_30
    .ioPortProp.P30.name = {'Y','0',0},
    .ioPortProp.P30.ucPortIndex = GET_UCPORT_INDEX(UCPORT_30),
    #else
    .ioPortProp.P30.name = {'N','A',0},
    .ioPortProp.P30.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_31
    .ioPortProp.P31.name = {'Y','1',0},
    .ioPortProp.P31.ucPortIndex = GET_UCPORT_INDEX(UCPORT_31),
    #else
    .ioPortProp.P31.name = {'N','A',0},
    .ioPortProp.P31.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_32
    .ioPortProp.P32.name = {'Y','2',0},
    .ioPortProp.P32.ucPortIndex = GET_UCPORT_INDEX(UCPORT_32),
    #else
    .ioPortProp.P32.name = {'N','A',0},
    .ioPortProp.P32.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_33
    .ioPortProp.P33.name = {'Y','3',0},
    .ioPortProp.P33.ucPortIndex = GET_UCPORT_INDEX(UCPORT_33),
    #else
    .ioPortProp.P33.name = {'N','A',0},
    .ioPortProp.P33.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_34
    .ioPortProp.P34.name = {'Y','4',0},
    .ioPortProp.P34.ucPortIndex = GET_UCPORT_INDEX(UCPORT_34),
    #else
    .ioPortProp.P34.name = {'N','A',0},
    .ioPortProp.P34.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_35
    .ioPortProp.P35.name = {'Y','5',0},
    .ioPortProp.P35.ucPortIndex = GET_UCPORT_INDEX(UCPORT_35),
    #else
    .ioPortProp.P35.name = {'N','A',0},
    .ioPortProp.P35.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_36
    .ioPortProp.P36.name = {'Y','6',0},
    .ioPortProp.P36.ucPortIndex = GET_UCPORT_INDEX(UCPORT_36),
    #else
    .ioPortProp.P36.name = {'N','A',0},
    .ioPortProp.P36.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_37
    .ioPortProp.P37.name = {'Y','7',0},
    .ioPortProp.P37.ucPortIndex = GET_UCPORT_INDEX(UCPORT_37),
    #else
    .ioPortProp.P37.name = {'N','A',0},
    .ioPortProp.P37.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_38
    .ioPortProp.P38.name = {'Y','8',0},
    .ioPortProp.P38.ucPortIndex = GET_UCPORT_INDEX(UCPORT_38),
    #else
    .ioPortProp.P38.name = {'N','A',0},
    .ioPortProp.P38.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_39
    .ioPortProp.P39.name = {'Y','9',0},
    .ioPortProp.P39.ucPortIndex = GET_UCPORT_INDEX(UCPORT_39),
    #else
    .ioPortProp.P39.name = {'N','A',0},
    .ioPortProp.P39.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_40
    .ioPortProp.P40.name = {'Y','1','0'},
    .ioPortProp.P40.ucPortIndex = GET_UCPORT_INDEX(UCPORT_40),
    #else
    .ioPortProp.P40.name = {'N','A',0},
    .ioPortProp.P40.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_41
    .ioPortProp.P41.name = {'Y','1','1'},
    .ioPortProp.P41.ucPortIndex = GET_UCPORT_INDEX(UCPORT_41),
    #else
    .ioPortProp.P41.name = {'N','A',0},
    .ioPortProp.P41.ucPortIndex = UCPORT_INDEX_NA,
    #endif
};


#if defined(CFG_STRUCT_IN_CODE)
//const CFG_STRUCT cfgStruct = {    //Don't use const, on XC16 doesn't work any more. Compiler optimization assumes CFG_STRUCT never changes, and causes trouble. CFG_STRUCT does change!
CFG_STRUCT cfgStruct __attribute__ ((space(auto_psv))) = {
    /////////////////////////////////////////////////
    //Info Settings
    .info.hdr.id = CFG_BLOCK_INFO_ID,
    .info.hdr.size = sizeof(CFG_BLOCK_INFO) / 16,
    .info.hdr.reserved = (sizeof(CFG_BLOCK_INFO) - sizeof(((CFG_BLOCK_INFO*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .info.eeConfUpdated = 0xff,
    .info.confSize = CFG_STRUCT_CONFSIZE,


    /////////////////////////////////////////////////
    //Network Settings
    .netHdr.id = CFG_BLOCK_NET_ID,
    .netHdr.size = sizeof(CFG_BLOCK_NET) / 16,
    .netHdr.reserved = (sizeof(CFG_BLOCK_NET) - sizeof(((CFG_BLOCK_NET*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .net.MyIPAddr.Val = (MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2 << 8ul | MY_DEFAULT_IP_ADDR_BYTE3 << 16ul | MY_DEFAULT_IP_ADDR_BYTE4 << 24ul),
    .net.MyMask.Val = (MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2 << 8ul | MY_DEFAULT_MASK_BYTE3 << 16ul | MY_DEFAULT_MASK_BYTE4 << 24ul),
    .net.MyGateway.Val = (MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2 << 8ul | MY_DEFAULT_GATE_BYTE3 << 16ul | MY_DEFAULT_GATE_BYTE4 << 24ul),
    .net.PrimaryDNSServer.Val = (MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2 << 8ul | MY_DEFAULT_PRIMARY_DNS_BYTE3 << 16ul | MY_DEFAULT_PRIMARY_DNS_BYTE4 << 24ul),
    .net.SecondaryDNSServer.Val = (MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2 << 8ul | MY_DEFAULT_SECONDARY_DNS_BYTE3 << 16ul | MY_DEFAULT_SECONDARY_DNS_BYTE4 << 24ul),
    .net.DefaultIPAddr.Val = (MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2 << 8ul | MY_DEFAULT_IP_ADDR_BYTE3 << 16ul | MY_DEFAULT_IP_ADDR_BYTE4 << 24ul),
    .net.DefaultMask.Val = (MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2 << 8ul | MY_DEFAULT_MASK_BYTE3 << 16ul | MY_DEFAULT_MASK_BYTE4 << 24ul),
    .net.NetBIOSName = MY_DEFAULT_HOST_NAME_,
    .net.Flags.bIsDHCPEnabled = MY_DEFAULT_DHCP_ENABLED,
    .net.Flags.bInConfigMode = 0,
    .net.MyMACAddr.v = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6},


    /////////////////////////////////////////////////
    //WiFi Settings - Currently not used. If enabled in future, this data need to be changed!!!!
    .wifiHdr.id = CFG_BLOCK_WIFI_ID,
    .wifiHdr.size = sizeof(CFG_BLOCK_WIFI) / 16,
    .wifiHdr.reserved = (sizeof(CFG_BLOCK_WIFI) - sizeof(((CFG_BLOCK_WIFI*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .wifi.MySSID = {0},
    .wifi.SsidLength = 0,
    .wifi.SecurityMode = 0,
    .wifi.SecurityKey = {0},
    .wifi.SecurityKeyLength = 0,
    .wifi.WepKeyIndex = 0,
    .wifi.dataValid = 0,
    .wifi.networkType = 0,
    .wifi.saveSecurityInfo = 0,


    /////////////////////////////////////////////////
    //SNMP Settings
    .snmpHdr.id = CFG_BLOCK_SNMP_ID,
    .snmpHdr.size = sizeof(CFG_BLOCK_SNMP) / 16,
    .snmpHdr.reserved = (sizeof(CFG_BLOCK_SNMP) - sizeof(((CFG_BLOCK_SNMP*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .snmp.readCommunity = SNMP_READ_COMMUNITIES,
    .snmp.writeCommunity = SNMP_WRITE_COMMUNITIES,
    .snmp.SnmpEngineBootRcrd = 0,


    /////////////////////////////////////////////////
    //General Settings
    .genHdr.id = CFG_BLOCK_GENERAL_ID,
    .genHdr.size = sizeof(CFG_BLOCK_GENERAL) / 16,
    .genHdr.reserved = (sizeof(CFG_BLOCK_GENERAL) - sizeof(((CFG_BLOCK_GENERAL*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .gen.username = {'a','d','m','i','n',0},
    .gen.password = {'m','i','c','r','o','c','h','i','p',0},


    /////////////////////////////////////////////////
    //Network 2 Settings
    .net2Hdr.id = CFG_BLOCK_NET2_ID,
    .net2Hdr.size = sizeof(CFG_BLOCK_NET2) / 16,
    .net2Hdr.reserved = (sizeof(CFG_BLOCK_NET2) - sizeof(((CFG_BLOCK_NET2*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .net2.Flags.bUdpCmd = 1,
    .net2.Flags.bTcpCmd = 0,
    .net2.Flags.bUdpEvents = 1,
    .net2.Flags.bTcpEvents = 0,
    .net2.udpCmdPort = DEFAULT_UDP_COMMAND_PORT,
    .net2.udpEventPort = DEFAULT_UDP_EVENT_PORT,
    .net2.tcpCmdPort = DEFAULT_TCP_COMMAND_PORT,
    .net2.tcpEventPort = DEFAULT_TCP_EVENT_PORT,


    /////////////////////////////////////////////////
    //"Serial Data Port" 0 to 7 Configuration - Following bits indicate which of "Serial Data Port" 0 to 7 are available
    .ser0to7Hdr.id = CFG_BLOCK_SER_0TO7_ID,
    .ser0to7Hdr.size = sizeof(CFG_BLOCK_SER_0TO7) / 16,
    .ser0to7Hdr.reserved = (sizeof(CFG_BLOCK_SER_0TO7) - sizeof(((CFG_BLOCK_SER_0TO7*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ser0to7.avail.bits.uart1 = 1,
    .ser0to7.avail.bits.uart2 = 1,
    .ser0to7.avail.bits.uart3 = 0,
    .ser0to7.avail.bits.uart4 = 0,
    .ser0to7.avail.bits.spi1 = 1,
    .ser0to7.avail.bits.spi3 = 1,
    .ser0to7.avail.bits.i2c1 = 1,
    .ser0to7.avail.bits.i2c2 = 1,
    .ser0to7.uart1 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.uart2 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.uart3 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.uart4 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.spi1 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.spi3 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          32,32},   //32 TX Buffer, 32 RX buffer
    .ser0to7.i2c1 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          I2C1_TXBUF_SIZE, I2C1_RXBUF_SIZE},
    .ser0to7.i2c2 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                          I2C2_TXBUF_SIZE, I2C2_RXBUF_SIZE},


    /////////////////////////////////////////////////
    //"Serial Data Port" 8 to 15 Configuration - Following bits indicate which of "Serial Data Port" 8 to 15 are available
    .ser8to15Hdr.id = CFG_BLOCK_SER_8TO15_ID,
    .ser8to15Hdr.size = sizeof(CFG_BLOCK_SER_8TO15) / 16,
    .ser8to15Hdr.reserved = (sizeof(CFG_BLOCK_SER_8TO15) - sizeof(((CFG_BLOCK_SER_8TO15*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ser8to15.avail.bits.i2c3 = 1,
    .ser8to15.avail.bits.udp1 = 1,
    .ser8to15.avail.bits.udp2 = 1,
    .ser8to15.avail.bits.udp3 = 1,
    .ser8to15.avail.bits.tcp1 = 1,
    .ser8to15.avail.bits.tcp2 = 1,
    .ser8to15.avail.bits.tcp3 = 1,
    .ser8to15.avail.bits.usb = 1,
    .ser8to15.i2c3 = {0,0, CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_BIN_ESC | CIRBUF_TYPE_STREAMING,
                           I2C2_TXBUF_SIZE, I2C2_RXBUF_SIZE},
    .ser8to15.udp1 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.udp2 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.udp3 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.tcp1 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.tcp2 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.tcp3 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                           32,32},   //32 TX Buffer, 32 RX buffer
    .ser8to15.usb =  {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET,
                           CIRBUF_FORMAT_NONE | CIRBUF_TYPE_PACKET,
                           32,32},   //32 TX Buffer, 32 RX buffer


    /////////////////////////////////////////////////
    //"Serial Data Port" 16 to 23 Configuration - Following bits indicate which of "Serial Data Port" 16 to 23 are available
    .ser16to23Hdr.id = CFG_BLOCK_SER_16TO23_ID,
    .ser16to23Hdr.size = sizeof(CFG_BLOCK_SER_16TO23) / 16,
    .ser16to23Hdr.reserved = (sizeof(CFG_BLOCK_SER_16TO23) - sizeof(((CFG_BLOCK_SER_16TO23*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ser16to23.avail.bits.user1 = 1,
    .ser16to23.avail.bits.user2 = 1,
    .ser16to23.avail.bits.user3 = 0,
    .ser16to23.avail.bits.user4 = 0,
    .ser16to23.avail.bits.user5 = 0,
    .ser16to23.avail.bits.user6 = 0,
    .ser16to23.avail.bits.user7 = 0,
    .ser16to23.avail.bits.user8 = 0,
    .ser16to23.user1 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user2 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user3 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user4 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user5 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user6 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user7 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer
    .ser16to23.user8 = {0,0, CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING,
                             32,32},   //32 TX Buffer, 32 RX buffer


    /////////////////////////////////////////////////
    //"Serial Data Port" 23 to 31 Configuration - Following bits indicate which of "Serial Data Port" 24 to 31 are available
    .ser24to31Hdr.id = CFG_BLOCK_SER_24TO31_ID,
    .ser24to31Hdr.size = sizeof(CFG_BLOCK_SER_24TO31) / 16,
    .ser24to31Hdr.reserved = (sizeof(CFG_BLOCK_SER_24TO31) - sizeof(((CFG_BLOCK_SER_24TO31*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ser24to31.avail.bits.res1 = 0,
    .ser24to31.avail.bits.res2 = 0,
    .ser24to31.avail.bits.res3 = 0,
    .ser24to31.avail.bits.res4 = 0,
    .ser24to31.avail.bits.res5 = 0,
    .ser24to31.avail.bits.res6 = 0,
    .ser24to31.avail.bits.res7 = 0,
    .ser24to31.avail.bits.res8 = 0,


    /////////////////////////////////////////////////
    //Microcontroller (UC) Port configuration, defines type of each port.
    // - type = Port type, is a PCFG_XXX_TYPE constant
    // - typeConf = is a PCFG_XXX_YYY constant
    // - def = Default value, valid for outputs like PWM, digital, ....
    .ucPortHdr.id = CFG_BLOCK_UC66_PORT_ID,
    .ucPortHdr.size = sizeof(CFG_BLOCK_UC66_PORT) / 16,
    .ucPortHdr.reserved = (sizeof(CFG_BLOCK_UC66_PORT) - sizeof(((CFG_BLOCK_UC66_PORT*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,

    #if defined(UC_PORT_B0_PROP)    //We have a port B0
    .ucPort.B0.din.id = UCPORT_ID_B0,
    .ucPort.B0.din.type = PCFG_DIN_TYPE,
    .ucPort.B0.din.typeConf = PCFG_DIN_3V3_IN,
    .ucPort.B0.gen.param[0] = 0,
    .ucPort.B0.gen.param[1] = 0,
    .ucPort.B0.gen.param[2] = 0,
    #else
    .ucPort.B0.din.id = UCPORT_ID_NA,
    #endif
    #if defined(UC_PORT_B1_PROP)    //We have a port B1
    .ucPort.B1.din.id = UCPORT_ID_B1,
    .ucPort.B1.din.type = PCFG_DIN_TYPE,
    .ucPort.B1.din.typeConf = PCFG_DIN_3V3_IN,
    .ucPort.B1.gen.param[0] = 0,
    .ucPort.B1.gen.param[1] = 0,
    .ucPort.B1.gen.param[2] = 0,
    #else
    .ucPort.B0.din.id = UCPORT_ID_NA,
    #endif

    /////////////////////////////////////////////////
    //IO Port (ports of SBC) configuration, defines type of each port.
    .ioPortHdr.id = CFG_BLOCK_SBC66_PORT_ID,
    .ioPortHdr.size = sizeof(CFG_BLOCK_SBC66_PORT) / 16,
    .ioPortHdr.reserved = (sizeof(CFG_BLOCK_SBC66_PORT) - sizeof(((CFG_BLOCK_SBC66_PORT*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,

    .ioPort.P00.label = 0xff,


    /////////////////////////////////////////////////
    //String block with 31x8 character strings.
    .str8aHdr.id = CFG_BLOCK_STRING_31X8_ID,
    .str8aHdr.size = sizeof(CFG_BLOCK_STRING_31X8) / 16,
    .str8aHdr.reserved = (sizeof(CFG_BLOCK_STRING_31X8) - sizeof(((CFG_BLOCK_STRING_31X8*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .str8a.str8[0][0] = 0,
    .str8a.str8[1][0] = 0,
    .str8a.str8[2][0] = 0,
    .str8a.str8[3][0] = 0,
    .str8a.str8[4][0] = 0,
    .str8a.str8[5][0] = 0,
    .str8a.str8[6][0] = 0,
    .str8a.str8[7][0] = 0,
    .str8a.str8[8][0] = 0,
    .str8a.str8[9][0] = 0,
    .str8a.str8[10][0] = 0,
    .str8a.str8[11][0] = 0,
    .str8a.str8[12][0] = 0,
    .str8a.str8[13][0] = 0,
    .str8a.str8[14][0] = 0,
    .str8a.str8[15][0] = 0,
    .str8a.str8[16][0] = 0,
    .str8a.str8[17][0] = 0,
    .str8a.str8[18][0] = 0,
    .str8a.str8[19][0] = 0,
    .str8a.str8[20][0] = 0,
    .str8a.str8[21][0] = 0,
    .str8a.str8[22][0] = 0,
    .str8a.str8[23][0] = 0,
    .str8a.str8[24][0] = 0,
    .str8a.str8[25][0] = 0,
    .str8a.str8[26][0] = 0,
    .str8a.str8[27][0] = 0,
    .str8a.str8[28][0] = 0,
    .str8a.str8[29][0] = 0,
    .str8a.str8[30][0] = 0,


    /////////////////////////////////////////////////
    //Constant TABLE_BLOCKs start from here!

    /////////////////////////////////////////////////
    //Microcontroller (UC) Port properties
    .ucPortPropHdr.id = TABLE_BLOCK_UC66_PORT_ID,
    .ucPortPropHdr.size = sizeof(TABLE_BLOCK_UC66_PORT) / 16,
    .ucPortPropHdr.reserved = (sizeof(TABLE_BLOCK_UC66_PORT) - sizeof(((TABLE_BLOCK_UC66_PORT*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    .ucPortProp.B0.prop.Val = UC_PORT_B0_PROP,
    .ucPortProp.B1.prop.Val = UC_PORT_B1_PROP,
    .ucPortProp.B2.prop.Val = UC_PORT_B2_PROP,
    .ucPortProp.B3.prop.Val = UC_PORT_B3_PROP,
    .ucPortProp.B4.prop.Val = UC_PORT_B4_PROP,
    .ucPortProp.B5.prop.Val = UC_PORT_B5_PROP,
    .ucPortProp.B6.prop.Val = UC_PORT_B6_PROP,
    .ucPortProp.B7.prop.Val = UC_PORT_B7_PROP,
    .ucPortProp.B8.prop.Val = UC_PORT_B8_PROP,
    .ucPortProp.B9.prop.Val = UC_PORT_B9_PROP,
    .ucPortProp.B10.prop.Val = UC_PORT_B10_PROP,
    .ucPortProp.B11.prop.Val = UC_PORT_B11_PROP,
    #if defined(UC_PORT_B12_PROP)
    .ucPortProp.B12.prop.Val = UC_PORT_B12_PROP,
    #endif
    #if defined(UC_PORT_B13_PROP)
    .ucPortProp.B13.prop.Val = UC_PORT_B13_PROP,
    #endif
    #if defined(UC_PORT_B14_PROP)
    .ucPortProp.B14.prop.Val = UC_PORT_B14_PROP,
    #endif
    #if defined(UC_PORT_B15_PROP)
    .ucPortProp.B15.prop.Val = UC_PORT_B15_PROP,
    #endif

    .ucPortProp.D0.prop.Val = UC_PORT_D0_PROP,
    .ucPortProp.D1.prop.Val = UC_PORT_D1_PROP,
    .ucPortProp.D2.prop.Val = UC_PORT_D2_PROP,
    .ucPortProp.D3.prop.Val = UC_PORT_D3_PROP,
    #if defined(UC_PORT_D6_PROP)
    .ucPortProp.D6.prop.Val = UC_PORT_D6_PROP,
    #endif
    #if defined(UC_PORT_D7_PROP)
    .ucPortProp.D7.prop.Val = UC_PORT_D7_PROP,
    #endif
    #if defined(UC_PORT_D8_PROP)
    .ucPortProp.D8.prop.Val = UC_PORT_D8_PROP,
    #endif
    #if defined(UC_PORT_D9_PROP)
    .ucPortProp.D9.prop.Val = UC_PORT_D9_PROP,
    #endif
    #if defined(UC_PORT_D10_PROP)
    .ucPortProp.D10.prop.Val = UC_PORT_D10_PROP,
    #endif

    #if defined(UC_PORT_F0_PROP)
    .ucPortProp.F0.prop.Val = UC_PORT_F0_PROP,
    #endif
    #if defined(UC_PORT_F1_PROP)
    .ucPortProp.F1.prop.Val = UC_PORT_F1_PROP,
    #endif
    #if defined(UC_PORT_F3_PROP)
    .ucPortProp.F3.prop.Val = UC_PORT_F3_PROP,
    #endif
    #if defined(UC_PORT_F4_PROP)
    .ucPortProp.F4.prop.Val = UC_PORT_F4_PROP,
    #endif
    #if defined(UC_PORT_F5_PROP)
    .ucPortProp.F5.prop.Val = UC_PORT_F5_PROP,
    #endif

    .ucPortProp.G6.prop.Val = UC_PORT_G6_PROP,
    .ucPortProp.G7.prop.Val = UC_PORT_G7_PROP,
    .ucPortProp.G8.prop.Val = UC_PORT_G8_PROP,
    .ucPortProp.G9.prop.Val = UC_PORT_G9_PROP,


    /////////////////////////////////////////////////
    //IO Port(ports of SBC) properties for all "Microcontroller Ports"
    .ioPortPropHdr.id = TABLE_BLOCK_SBC66_PORT_ID,
    .ioPortPropHdr.size = sizeof(TABLE_BLOCK_SBC66_PORT) / 16,
    .ioPortPropHdr.reserved = (sizeof(TABLE_BLOCK_SBC66_PORT) - sizeof(((TABLE_BLOCK_SBC66_PORT*)0)->reserve)) | 0xC0 /* Ensure bits 6&7 are 1's*/,
    #ifdef UCPORT_24
    .ioPortProp.P24.name = {'S','0',0},
    .ioPortProp.P24.ucPortIndex = GET_UCPORT_INDEX(UCPORT_24),
    #else
    .ioPortProp.P24.name = {'N','A',0},
    .ioPortProp.P24.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_25
    .ioPortProp.P25.name = {'S','1',0},
    .ioPortProp.P25.ucPortIndex = GET_UCPORT_INDEX(UCPORT_25),
    #else
    .ioPortProp.P25.name = {'N','A',0},
    .ioPortProp.P25.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_13
    .ioPortProp.P13.name = {'S','C',0},
    .ioPortProp.P13.ucPortIndex = GET_UCPORT_INDEX(UCPORT_13),
    #else
    .ioPortProp.P13.name = {'N','A',0},
    .ioPortProp.P13.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_12
    .ioPortProp.P12.name = {'S','D',0},
    .ioPortProp.P12.ucPortIndex = GET_UCPORT_INDEX(UCPORT_12),
    #else
    .ioPortProp.P12.name = {'N','A',0},
    .ioPortProp.P12.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_20
    .ioPortProp.P20.name = {'T','0',0},
    .ioPortProp.P20.ucPortIndex = GET_UCPORT_INDEX(UCPORT_20),
    #else
    .ioPortProp.P20.name = {'N','A',0},
    .ioPortProp.P20.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_21
    .ioPortProp.P21.name = {'T','1',0},
    .ioPortProp.P21.ucPortIndex = GET_UCPORT_INDEX(UCPORT_21),
    #else
    .ioPortProp.P21.name = {'N','A',0},
    .ioPortProp.P21.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_22
    .ioPortProp.P22.name = {'T','2',0},
    .ioPortProp.P22.ucPortIndex = GET_UCPORT_INDEX(UCPORT_22),
    #else
    .ioPortProp.P22.name = {'N','A',0},
    .ioPortProp.P22.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_23
    .ioPortProp.P23.name = {'T','3',0},
    .ioPortProp.P23.ucPortIndex = GET_UCPORT_INDEX(UCPORT_23),
    #else
    .ioPortProp.P23.name = {'N','A',0},
    .ioPortProp.P23.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_26
    .ioPortProp.P26.name = {'T','4',0},
    .ioPortProp.P26.ucPortIndex = GET_UCPORT_INDEX(UCPORT_26),
    #else
    .ioPortProp.P26.name = {'N','A',0},
    .ioPortProp.P26.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_27
    .ioPortProp.P27.name = {'T','5',0},
    .ioPortProp.P27.ucPortIndex = GET_UCPORT_INDEX(UCPORT_27),
    #else
    .ioPortProp.P27.name = {'N','A',0},
    .ioPortProp.P27.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_28
    .ioPortProp.P28.name = {'T','6',0},
    .ioPortProp.P28.ucPortIndex = GET_UCPORT_INDEX(UCPORT_28),
    #else
    .ioPortProp.P28.name = {'N','A',0},
    .ioPortProp.P28.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_29
    .ioPortProp.P29.name = {'T','7',0},
    .ioPortProp.P29.ucPortIndex = GET_UCPORT_INDEX(UCPORT_29),
    #else
    .ioPortProp.P29.name = {'N','A',0},
    .ioPortProp.P29.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_00
    .ioPortProp.P00.name = {'X','0',0},
    .ioPortProp.P00.ucPortIndex = GET_UCPORT_INDEX(UCPORT_00),
    #else
    .ioPortProp.P00.name = {'N','A',0},
    .ioPortProp.P00.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_01
    .ioPortProp.P01.name = {'X','1',0},
    .ioPortProp.P01.ucPortIndex = GET_UCPORT_INDEX(UCPORT_01),
    #else
    .ioPortProp.P01.name = {'N','A',0},
    .ioPortProp.P01.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_02
    .ioPortProp.P02.name = {'X','2',0},
    .ioPortProp.P02.ucPortIndex = GET_UCPORT_INDEX(UCPORT_02),
    #else
    .ioPortProp.P02.name = {'N','A',0},
    .ioPortProp.P02.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_03
    .ioPortProp.P03.name = {'X','3',0},
    .ioPortProp.P03.ucPortIndex = GET_UCPORT_INDEX(UCPORT_03),
    #else
    .ioPortProp.P03.name = {'N','A',0},
    .ioPortProp.P03.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_04
    .ioPortProp.P04.name = {'X','4',0},
    .ioPortProp.P04.ucPortIndex = GET_UCPORT_INDEX(UCPORT_04),
    #else
    .ioPortProp.P04.name = {'N','A',0},
    .ioPortProp.P04.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_05
    .ioPortProp.P05.name = {'X','5',0},
    .ioPortProp.P05.ucPortIndex = GET_UCPORT_INDEX(UCPORT_05),
    #else
    .ioPortProp.P05.name = {'N','A',0},
    .ioPortProp.P05.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_06
    .ioPortProp.P06.name = {'X','6',0},
    .ioPortProp.P06.ucPortIndex = GET_UCPORT_INDEX(UCPORT_06),
    #else
    .ioPortProp.P06.name = {'N','A',0},
    .ioPortProp.P06.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_07
    .ioPortProp.P07.name = {'X','7',0},
    .ioPortProp.P07.ucPortIndex = GET_UCPORT_INDEX(UCPORT_07),
    #else
    .ioPortProp.P07.name = {'N','A',0},
    .ioPortProp.P07.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_08
    .ioPortProp.P08.name = {'X','8',0},
    .ioPortProp.P08.ucPortIndex = GET_UCPORT_INDEX(UCPORT_08),
    #else
    .ioPortProp.P08.name = {'N','A',0},
    .ioPortProp.P08.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_09
    .ioPortProp.P09.name = {'X','9',0},
    .ioPortProp.P09.ucPortIndex = GET_UCPORT_INDEX(UCPORT_09),
    #else
    .ioPortProp.P09.name = {'N','A',0},
    .ioPortProp.P09.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_10
    .ioPortProp.P10.name = {'X','1','0'},
    .ioPortProp.P10.ucPortIndex = GET_UCPORT_INDEX(UCPORT_10),
    #else
    .ioPortProp.P10.name = {'N','A',0},
    .ioPortProp.P10.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_11
    .ioPortProp.P11.name = {'X','1','1'},
    .ioPortProp.P11.ucPortIndex = GET_UCPORT_INDEX(UCPORT_11),
    #else
    .ioPortProp.P11.name = {'N','A',0},
    .ioPortProp.P11.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_14
    .ioPortProp.P14.name = {'X','1','2'},
    .ioPortProp.P14.ucPortIndex = GET_UCPORT_INDEX(UCPORT_14),
    #else
    .ioPortProp.P14.name = {'N','A',0},
    .ioPortProp.P14.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_15
    .ioPortProp.P15.name = {'X','1','3'},
    .ioPortProp.P15.ucPortIndex = GET_UCPORT_INDEX(UCPORT_15),
    #else
    .ioPortProp.P15.name = {'N','A',0},
    .ioPortProp.P15.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_16
    .ioPortProp.P16.name = {'X','1','4'},
    .ioPortProp.P16.ucPortIndex = GET_UCPORT_INDEX(UCPORT_16),
    #else
    .ioPortProp.P16.name = {'N','A',0},
    .ioPortProp.P16.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_17
    .ioPortProp.P17.name = {'X','1','5'},
    .ioPortProp.P17.ucPortIndex = GET_UCPORT_INDEX(UCPORT_17),
    #else
    .ioPortProp.P17.name = {'N','A',0},
    .ioPortProp.P17.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_30
    .ioPortProp.P30.name = {'Y','0',0},
    .ioPortProp.P30.ucPortIndex = GET_UCPORT_INDEX(UCPORT_30),
    #else
    .ioPortProp.P30.name = {'N','A',0},
    .ioPortProp.P30.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_31
    .ioPortProp.P31.name = {'Y','1',0},
    .ioPortProp.P31.ucPortIndex = GET_UCPORT_INDEX(UCPORT_31),
    #else
    .ioPortProp.P31.name = {'N','A',0},
    .ioPortProp.P31.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_32
    .ioPortProp.P32.name = {'Y','2',0},
    .ioPortProp.P32.ucPortIndex = GET_UCPORT_INDEX(UCPORT_32),
    #else
    .ioPortProp.P32.name = {'N','A',0},
    .ioPortProp.P32.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_33
    .ioPortProp.P33.name = {'Y','3',0},
    .ioPortProp.P33.ucPortIndex = GET_UCPORT_INDEX(UCPORT_33),
    #else
    .ioPortProp.P33.name = {'N','A',0},
    .ioPortProp.P33.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_34
    .ioPortProp.P34.name = {'Y','4',0},
    .ioPortProp.P34.ucPortIndex = GET_UCPORT_INDEX(UCPORT_34),
    #else
    .ioPortProp.P34.name = {'N','A',0},
    .ioPortProp.P34.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_35
    .ioPortProp.P35.name = {'Y','5',0},
    .ioPortProp.P35.ucPortIndex = GET_UCPORT_INDEX(UCPORT_35),
    #else
    .ioPortProp.P35.name = {'N','A',0},
    .ioPortProp.P35.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_36
    .ioPortProp.P36.name = {'Y','6',0},
    .ioPortProp.P36.ucPortIndex = GET_UCPORT_INDEX(UCPORT_36),
    #else
    .ioPortProp.P36.name = {'N','A',0},
    .ioPortProp.P36.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_37
    .ioPortProp.P37.name = {'Y','7',0},
    .ioPortProp.P37.ucPortIndex = GET_UCPORT_INDEX(UCPORT_37),
    #else
    .ioPortProp.P37.name = {'N','A',0},
    .ioPortProp.P37.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_38
    .ioPortProp.P38.name = {'Y','8',0},
    .ioPortProp.P38.ucPortIndex = GET_UCPORT_INDEX(UCPORT_38),
    #else
    .ioPortProp.P38.name = {'N','A',0},
    .ioPortProp.P38.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_39
    .ioPortProp.P39.name = {'Y','9',0},
    .ioPortProp.P39.ucPortIndex = GET_UCPORT_INDEX(UCPORT_39),
    #else
    .ioPortProp.P39.name = {'N','A',0},
    .ioPortProp.P39.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_40
    .ioPortProp.P40.name = {'Y','1','0'},
    .ioPortProp.P40.ucPortIndex = GET_UCPORT_INDEX(UCPORT_40),
    #else
    .ioPortProp.P40.name = {'N','A',0},
    .ioPortProp.P40.ucPortIndex = UCPORT_INDEX_NA,
    #endif

    #ifdef UCPORT_41
    .ioPortProp.P41.name = {'Y','1','1'},
    .ioPortProp.P41.ucPortIndex = GET_UCPORT_INDEX(UCPORT_41),
    #else
    .ioPortProp.P41.name = {'N','A',0},
    .ioPortProp.P41.ucPortIndex = UCPORT_INDEX_NA,
    #endif
};

void appConfInit(void) {
    WORD xeeAdr;
    WORD w;
    BYTE *p;

    /////////////////////////////////////////////////
    //Initialize AppConfig with contents of External EEPROM. At this stage, external EEPROM
    //should have valid configuration.

    //Initialize Network Settings
    p = (BYTE*)&AppConfig.MyIPAddr;     //CFG_STRUCT.net starts from AppConfig.MyIPAppr
    xeeAdr = XEEMEM_CFG_STRUCT_START_ADR + offsetof(CFG_STRUCT, net);
    //Get number of bytes to read. AppConfig does NOT contain 'reserve' array
    //w = sizeof(CFG_BLOCK_NET) -  sizeof(((CFG_BLOCK_NET*)0)->reserve);; //Number of bytes to write
    w = sizeof(((CFG_STRUCT*)0)->net) - sizeof(((CFG_STRUCT*)0)->net.reserve);
    // TODO Use xeeReadArray() function??
    xeeBeginRead(xeeAdr);
    while (w-- != 0) {
        *p++ = xeeRead();
    }
    xeeEndRead();
    FormatNetBIOSName(AppConfig.NetBIOSName);
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nNetbios Name = ");
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (const char*)AppConfig.NetBIOSName);

    //Initialize WiFi Settings. Currently NOT USED! Has NOT BEEN tested!!!!!!
    //Might need to be changed. See InitAppConfig() function in original Microchip Demo App
    //for details on what is done in working app
    #if defined(WF_CS_TRIS)
    {
        p = (BYTE*)&AppConfig.MySSID;
        xeeAdr = XEEMEM_CFG_STRUCT_START_ADR + offsetof(CFG_STRUCT, wifi);
        w = sizeof(CFG_BLOCK_WIFI); //Number of bytes to write
        xeeBeginRead(xeeAdr);
        while (w-- != 0) {
            *p++ = xeeRead();
        }
        xeeEndRead();
    }
    #endif

    //Initialize SNMP Settings. Currently NOT USED! Has NOT BEEN tested!!!!!!
    //Might need to be changed. See InitAppConfig() function in original Microchip Demo App
    //for details on what is done in working app
    #if defined(STACK_USE_SNMP_SERVER)
    {
        p = (BYTE*)&AppConfig.readCommunity;
        xeeAdr = XEEMEM_CFG_STRUCT_START_ADR + offsetof(CFG_STRUCT, snmp);
        //Get number of bytes to read. AppConfig does NOT contain 'reserve' array
        w = sizeof(CFG_BLOCK_SNMP) -  sizeof(((CFG_BLOCK_SNMP*)0)->reserve); //Number of bytes to write
        //w = sizeof(((CFG_STRUCT*)0)->snmp) - sizeof(((CFG_STRUCT*)0)->snmp.reserve);
        xeeBeginRead(xeeAdr);
        while (w-- != 0) {
            *p++ = xeeRead();
        }
        xeeEndRead();
    }
    #endif
}

#endif
