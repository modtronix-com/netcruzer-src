/**
 * @brief           Functions for "Serial Data Ports"(SDP)
 * @file            nz_serDataPorts.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_serDataPorts_desc Description
 *****************************************
 * Functions for "Serial Data Ports"(SDP)
 * 
 * @subsection nz_serDataPorts_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ------ Serial Data Port Configuration (from nz_serDataPorts.h) ------
// *********************************************************************
//Dont create TX and RX buffers for "Serial Data Ports"
#define SERPORT_DONT_CREATE_TXRX_BUFS

//Dont initialize the SER_INFO structure, we will provide the serDefaultInit() function!
#define SERPORT_DONT_MANAGE_SER_INFO_INIT

 @endcode
 *
 **********************************************************************
 * @section nz_serDataPorts_lic Software License Agreement
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
 * 2014-01-17, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_SERDATAPORTS_H
#define NZ_SERDATAPORTS_H

/////////////////////////////////////////////////
//  "Serial Data Ports" - documentation module
/////////////////////////////////////////////////
/**
@defgroup info_serport Serial Data Ports
Serial Data Ports (SDP) is a term used in the Netcruzer Library for many different modules that use data serially. All serial communication
ports, like the UARTs, I2C port, SPI Port, UDP Port, TCP Ports... are referred to as "Serial Data Port". There are also a couple of General
purpose "Serial Data Port" that can be used in user applications. All "Serial Data Port" use @ref info_cirbuf "Circular Buffers" for
transferring data.

@section info_serport_num Serial Data Port Numbers
All Serial Data Ports are assigned numbers:
<table class="doxtable">
<tr><td style="padding-right: 50px;">
- @b 0 = Default Debug Port
- @b 1 = UART 1
- @b 2 = UART 2
- @b 3 = UART 3
- @b 4 = UART 4
- @b 5 = SPI 1
- @b 6 = SPI 3
- @b 7 = I2C 1
- @b 8 = I2C 2
- @b 9 = I2C 3
- @b 10(a) = UDP 1
- @b 11(b) = UDP 2
- @b 12(c) = UDP 3
</td><td>
- @b 13(d) = TCP 1
- @b 14(e) = TCP 2
- @b 15(f) = TCP 3
- @b 16(g) = USB HID Port 1
- @b 17(h) = User 1 General Purpose Serial Data Port
- @b 18(i) = User 2 General Purpose Serial Data Port
- @b 19(j) = User 3 General Purpose Serial Data Port
- @b 20(k) = User 4 General Purpose Serial Data Port
- @b 21(l) = User 5 General Purpose Serial Data Port
- @b 22(m) = User 6 General Purpose Serial Data Port
- @b 23(n) = User 7 General Purpose Serial Data Port
- @b 24(o) = User 8 General Purpose Serial Data Port
</td>
</tr></table>
*/


//"Serial Data Ports" contain circular buffers, this include is required
#include "nz_circularBuffer.h"


/////////////////////////////////////////////////
////////// Defines //////////////////////////////
/////////////////////////////////////////////////
#define SERPORT_ID_DEBUG    0   ///< Default debug "Serial Data Port"

#define SERPORT_ID_UART1     1
#define SERPORT_ID_UART2     2
#define SERPORT_ID_UART3     3
#define SERPORT_ID_UART4     4
#define SERPORT_ID_SPI1     5
#define SERPORT_ID_SPI3     6
#define SERPORT_ID_I2C1     7
#define SERPORT_ID_I2C2     8

#define SERPORT_ID_I2C3     9
#define SERPORT_ID_UDP1     10
#define SERPORT_ID_UDP2     11
#define SERPORT_ID_UDP3     12
#define SERPORT_ID_TCP1     13
#define SERPORT_ID_TCP2     14
#define SERPORT_ID_TCP3     15
#define SERPORT_ID_USB1     16

#define SERPORT_ID_USER1    17
#define SERPORT_ID_USER2    18
#define SERPORT_ID_USER3    19
#define SERPORT_ID_USER4    20
#define SERPORT_ID_USER5    21
#define SERPORT_ID_USER6    22
#define SERPORT_ID_USER7    23
#define SERPORT_ID_USER8    24

#define SERPORT_ID_MAX      SERPORT_ID_USER8

/**
 * The number of circular buffers. Each "Serial Data Port" consists of a TX and RX circular buffer.
 * There are thus BUF_COUNT/2 "Serial Data Ports"
 */
//#define BUF_COUNT (SERPORT_ID_MAX)


/**
 * @section info_serport_bufid Serial Data Port Buffers
 * @addtogroup info_serport
 *
 * Each "Serial Data Port" has a transmit and receive buffer. They are identified by their "Serial Data Port Buffer ID"
 * number.
 * - The "Serial Data Port Buffer ID" for the transmit buffer = SERPORT_ID * 2
 * - The "Serial Data Port Buffer ID" for the receive buffer = (SERPORT_ID * 2) + 1
 *
 * For example, the "Serial Data Port ID" for I2C 1 is 7. It's transmit buffer will have the "Serial Data Port Buffer ID"
 * of 14 (7*2), and it's receive buffer will be 15.
 * - Must match sequence of bits in SER_AVAIL structures
 * - Must match order of members in CFG_BLOCK_SER. First 'uart1', then 'uart2' .....
 */
//1 I2C, 3 UDP, 3 TCP and one USB buffer
#define SERPORT_TXBUFID_UART1    (SERPORT_ID_UART1*2)
#define SERPORT_RXBUFID_UART1    ((SERPORT_ID_UART1*2)+1)
#define SERPORT_TXBUFID_UART2    (SERPORT_ID_UART2*2)
#define SERPORT_RXBUFID_UART2    ((SERPORT_ID_UART2*2)+1)
#define SERPORT_TXBUFID_UART3    (SERPORT_ID_UART3*2)
#define SERPORT_RXBUFID_UART3    ((SERPORT_ID_UART3*2)+1)
#define SERPORT_TXBUFID_UART4    (SERPORT_ID_UART4*2)
#define SERPORT_RXBUFID_UART4    ((SERPORT_ID_UART4*2)+1)
#define SERPORT_TXBUFID_SPI1    (SERPORT_ID_SPI1*2)
#define SERPORT_RXBUFID_SPI1    ((SERPORT_ID_SPI1*2)+1)
#define SERPORT_TXBUFID_SPI3    (SERPORT_ID_SPI3*2)
#define SERPORT_RXBUFID_SPI3    ((SERPORT_ID_SPI3*2)+1)
#define SERPORT_TXBUFID_I2C1    (SERPORT_ID_I2C1*2)
#define SERPORT_RXBUFID_I2C1    ((SERPORT_ID_I2C1*2)+1)
#define SERPORT_TXBUFID_I2C2    (SERPORT_ID_I2C2*2)
#define SERPORT_RXBUFID_I2C2    ((SERPORT_ID_I2C2*2)+1)
//1 I2C, 3 UDP, 3 TCP and one USB buffer
#define SERPORT_TXBUFID_I2C3    (SERPORT_ID_I2C3*2)
#define SERPORT_RXBUFID_I2C3    ((SERPORT_ID_I2C3*2)+1)
#define SERPORT_TXBUFID_UDP1    (SERPORT_ID_UDP1*2)
#define SERPORT_RXBUFID_UDP1    ((SERPORT_ID_UDP1*2)+1)
#define SERPORT_TXBUFID_UDP2    (SERPORT_ID_UDP2*2)
#define SERPORT_RXBUFID_UDP2    ((SERPORT_ID_UDP2*2)+1)
#define SERPORT_TXBUFID_UDP3    (SERPORT_ID_UDP3*2)
#define SERPORT_RXBUFID_UDP3    ((SERPORT_ID_UDP3*2)+1)
#define SERPORT_TXBUFID_TCP1    (SERPORT_ID_TCP1*2)
#define SERPORT_RXBUFID_TCP1    ((SERPORT_ID_TCP1*2)+1)
#define SERPORT_TXBUFID_TCP2    (SERPORT_ID_TCP2*2)
#define SERPORT_RXBUFID_TCP2    ((SERPORT_ID_TCP2*2)+1)
#define SERPORT_TXBUFID_TCP3    (SERPORT_ID_TCP3*2)
#define SERPORT_RXBUFID_TCP3    ((SERPORT_ID_TCP3*2)+1)
#define SERPORT_TXBUFID_USB1    (SERPORT_ID_USB1*2)
#define SERPORT_RXBUFID_USB1    ((SERPORT_ID_USB1*2)+1)
//8 User buffers
#define SERPORT_TXBUFID_USER1    (SERPORT_ID_USER1*2)
#define SERPORT_RXBUFID_USER1    ((SERPORT_ID_USER1*2)+1)
#define SERPORT_TXBUFID_USER2    (SERPORT_ID_USER2*2)
#define SERPORT_RXBUFID_USER2    ((SERPORT_ID_USER2*2)+1)
#define SERPORT_TXBUFID_USER3    (SERPORT_ID_USER3*2)
#define SERPORT_RXBUFID_USER3    ((SERPORT_ID_USER3*2)+1)
#define SERPORT_TXBUFID_USER4    (SERPORT_ID_USER4*2)
#define SERPORT_RXBUFID_USER4    ((SERPORT_ID_USER4*2)+1)
#define SERPORT_TXBUFID_USER5    (SERPORT_ID_USER5*2)
#define SERPORT_RXBUFID_USER5    ((SERPORT_ID_USER5*2)+1)
#define SERPORT_TXBUFID_USER6    (SERPORT_ID_USER6*2)
#define SERPORT_RXBUFID_USER6    ((SERPORT_ID_USER6*2)+1)
#define SERPORT_TXBUFID_USER7    (SERPORT_ID_USER7*2)
#define SERPORT_RXBUFID_USER7    ((SERPORT_ID_USER7*2)+1)
#define SERPORT_TXBUFID_USER8    (SERPORT_ID_USER8*2)
#define SERPORT_RXBUFID_USER8    ((SERPORT_ID_USER8*2)+1)


/** "Serial Data Port" Types */
enum SERPORT_TYPE_UART {
    SERPORT_TYPE_UNDEFINED=0,
    SERPORT_TYPE_SOFTWARE,
    SERPORT_TYPE_UART,
    SERPORT_TYPE_USART,
    SERPORT_TYPE_SPI,
    SERPORT_TYPE_I2C,
    SERPORT_TYPE_USBHID,
    SERPORT_TYPE_UDP,
    SERPORT_TYPE_TCP,
    SERPORT_TYPE_CAN,
    SERPORT_TYPE_MIWI,
    SERPORT_TYPE_ZIGBEE,
    SERPORT_TYPE_BLUETOOTH,
    SERPORT_TYPE_MAX
};

//#if (SERPORT_TYPE_MAX >= sizeof(SERPORT.flagBits.type))
#if (SERPORT_TYPE_MAX >= 32)
#error "Size of SERPORT.flagBits.type too small for all SERPORT_TYPES!"
#endif

/** This SERPORT is enabled */
#define SERPORT_STAT_ENABLED 0x8000

//SERPORT is defined in nz_netcruzer.h
///**
// * "Serial Data Port" structure, TX and RX "Circular Buffers" (CIRBUF structures).
// */
typedef struct __attribute__((__packed__)) _SERPORT
{
    union {
        struct
        {
            unsigned int type : 5;          ///< "Serial Data Port" Type, is a SERPORT_TYPE_xx define
            unsigned int fill5_7 : 3;       ///< Reserved, might add to type if required
            unsigned int typeInfo : 3;      ///< Additional info for "Serial Data Port" Type
            unsigned int fill11_14 : 4;     ///< Reserved
            unsigned int bEnabled : 1;      ///< "Serial Data Port" Enabled. Must match SERPORT_STAT_ENABLED!
        } flagBits;
        struct
        {
            BYTE    LB;                 ///< Lower Byte, contains Buffer Type and Format
            BYTE    HB;                 ///< Upper Byte
        } flagBytes;
        WORD flagVal;
    };

    CIRBUF cbufTx;      ///< TX Circular Buffer, use bufXxx functions
    CIRBUF cbufRx;      ///< RX Circular Buffer, use bufXxx functions

    BYTE   fill;        ///< Reserved
    BYTE   strId;
} SERPORT;


typedef struct SERPORTS_ {
#if defined(HAS_SERPORT_DEBUG)
    SERPORT    debug;
#endif

#if defined(HAS_SERPORT_UART1)
    SERPORT    uart1;
#endif
#if defined(HAS_SERPORT_UART2)
    SERPORT    uart2;
#endif
#if defined(HAS_SERPORT_UART2)
    SERPORT    uart3;
#endif
#if defined(HAS_SERPORT_UART4)
    SERPORT    uart4;
#endif
#if defined(HAS_SERPORT_SPI1)
    SERPORT    spi1;
#endif
#if defined(HAS_SERPORT_SPI3)
    SERPORT    spi3;
#endif
#if defined(HAS_SERPORT_I2C1)
    SERPORT    i2c1;
#endif
#if defined(HAS_SERPORT_I2C2)
    SERPORT    i2c2;
#endif
#if defined(HAS_SERPORT_I2C3)
    SERPORT    i2c3;
#endif
#if defined(HAS_SERPORT_UDP1)
    SERPORT    udp1;
#endif
#if defined(HAS_SERPORT_UDP2)
    SERPORT    udp2;
#endif
#if defined(HAS_SERPORT_UDP3)
    SERPORT    udp3;
#endif
#if defined(HAS_SERPORT_TCP1)
    SERPORT    tcp1;
#endif
#if defined(HAS_SERPORT_TCP2)
    SERPORT    tcp2;
#endif
#if defined(HAS_SERPORT_TCP3)
    SERPORT    tcp3;
#endif
#if defined(HAS_SERPORT_USB)
    SERPORT    usb;
#endif
#if defined(HAS_SERPORT_USER1)
    SERPORT    user1;
#endif
#if defined(HAS_SERPORT_USER2)
    SERPORT    user2;
#endif
#if defined(HAS_SERPORT_USER3)
    SERPORT    user3;
#endif
#if defined(HAS_SERPORT_USER4)
    SERPORT    user4;
#endif
#if defined(HAS_SERPORT_USER5)
    SERPORT    user5;
#endif
#if defined(HAS_SERPORT_USER6)
    SERPORT    user6;
#endif
#if defined(HAS_SERPORT_USER7)
    SERPORT    user7;
#endif
#if defined(HAS_SERPORT_USER8)
    SERPORT    user8;
#endif
} SERPORTS;


/**
 * Structure indicating what "Serial Data Ports" 0 to 7 are available. Each SDP consists of a TX and RX circular buffer.
 */
typedef union __attribute__((__packed__)) {
    BYTE val;

    struct {
         unsigned char uart1:1;   //---- ---x = Serial 1
         unsigned char uart2:1;   //---- --x- = Serial 2
         unsigned char uart3:1;   //---- -x-- = Serial 3
         unsigned char uart4:1;   //---- x--- = Serial 4
         unsigned char spi1:1;   //---x ---- = SPI 1
         unsigned char spi3:1;   //--x- ---- = SPI 3
         unsigned char i2c1:1;   //-x-- ---- = I2C 1
         unsigned char i2c2:1;   //x--- ---- = I2C 2
    } bits;
} SER_AVAIL_0TO7;


/**
 * Structure indicating what "Serial Data Port" 8 to 15 are available. Each SDP consists of a TX and RX circular buffer.
 */
typedef union __attribute__((__packed__)) {
    BYTE val;

    struct {
         unsigned char i2c3:1;   //---- ---x = I2C 3
         unsigned char udp1:1;   //---- --x- = UDP 1
         unsigned char udp2:1;   //---- -x-- = UDP 2
         unsigned char udp3:1;   //---- x--- = UDP 3
         unsigned char tcp1:1;   //---x ---- = TCP 1
         unsigned char tcp2:1;   //--x- ---- = TCP 2
         unsigned char tcp3:1;   //-x-- ---- = TCP 3
         unsigned char usb:1;    //x--- ---- = USB CDC 1
    } bits;
} SER_AVAIL_8TO15;


/**
 * Structure indicating what "Serial Data Ports" 16 to 23 are available. Each SDP consists of a TX and RX circular buffer.
 */
typedef union __attribute__((__packed__)) {
    BYTE val;

    struct {
         unsigned char user1:1;  //---- ---x = User 1
         unsigned char user2:1;  //---- --x- = User 2
         unsigned char user3:1;  //---- -x-- = User 3
         unsigned char user4:1;  //---- x--- = User 4
         unsigned char user5:1;  //---x ---- = User 5
         unsigned char user6:1;  //--x- ---- = User 6
         unsigned char user7:1;  //-x-- ---- = User 7
         unsigned char user8:1;  //x--- ---- = Debug
    } bits;
} SER_AVAIL_16TO23;


/**
 * Structure indicating what "Serial Data Ports" 24 to 31 are available. Each SDP consists of a TX and RX circular buffer.
 */
typedef union __attribute__((__packed__)) {
    BYTE val;

    struct {
         unsigned char res1:1;   //---- ---x = Reserved
         unsigned char res2:1;   //---- --x- = Reserved
         unsigned char res3:1;   //---- -x-- = Reserved
         unsigned char res4:1;   //---- x--- = Reserved
         unsigned char res5:1;   //---x ---- = Reserved
         unsigned char res6:1;   //--x- ---- = Reserved
         unsigned char res7:1;   //-x-- ---- = Reserved
         unsigned char res8:1;   //x--- ---- = Reserved
    } bits;
} SER_AVAIL_24TO31;


/**
 * Structure containing information for all "Serial Data Ports"
 *
 */
typedef struct __attribute__((aligned(2), packed)) _SER_INFO
{
    BYTE                count;      //Number of "Serial Data Ports".
    BYTE                padd1;      //Padding

    union {
        BYTE avail[4];
        struct
        {
            SER_AVAIL_0TO7      avail_0to7;
            SER_AVAIL_8TO15     avail_8to15;
            SER_AVAIL_16TO23    avail_16to23;
            SER_AVAIL_24TO31    avail_24to31;
        } block;
    };

    BYTE*               buf;        //Pointer to allocated memory space used for all CIRBUF buffers
} SER_INFO;



/////////////////////////////////////////////////
////////// Configuration Data ///////////////////
/////////////////////////////////////////////////

/**
 * "Serial Data Port" Config. Each "Serial Data Port" consists of a TX and RX "Circular Buffer".
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 *
 * The txType and rxType variables gives the type of the TX and RX "Circular Buffers".
 * It is an OR'ed combination of CIRBUF_TYPE_XXX and CIRBUF_FORMAT_XXX defines.
 * - Bit 0-1 = CIRBUF.flagTF.type = CIRBUF_TYPE_XXX define
 * - Bit 2-4 = CIRBUF.flagTF.format = CIRBUF_FORMAT_XXX define
 */
//typedef struct __attribute__((aligned(2), packed)) _SER_CONFIG
typedef struct __attribute__((__packed__))
{
    BYTE    flags;  ///< Currently only gives "Serial Data Port" type, is a SER_BUFTYPE_xx define
    BYTE    strId;  ///< Buffer string ID, if buffer has a name
    BYTE    txType; ///< Tx buffer type and flags, see description on top for details.
    BYTE    rxType; ///< Rx buffer type and flags, a CIRBUF_TYPE_xxx constant
    WORD    txSize; ///< TX Circular Buffer size. Actual usable size is 1 less. For "Power of 2" buffers, this must be power of 2 value, like 4,8,16,32....
    WORD    rxSize; ///< RX Circular Buffer size. Actual usable size is 1 less. For "Power of 2" buffers, this must be power of 2 value, like 4,8,16,32....
} SER_CONFIG;


/**
 * UART SERPORT Config.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    BYTE    conf[4];
} SER_CONFIG_UART;


/**
 * SPI SERPORT Config.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    BYTE    conf[4];
} SER_CONFIG_SPI;

/**
 * I2C SERPORT Config.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    BYTE    conf[4];
} SER_CONFIG_I2C;


/**
 * USB SERPORT Config.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    BYTE    conf[4];
} SER_CONFIG_USB;


/**
 * UDP SERPORT Config.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    BYTE    conf[4];
} SER_CONFIG_UDP;


/**
 * TCP SERPORT Config.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    BYTE    conf[4];
} SER_CONFIG_TCP;


/**
 * "Serial Data Port" 0 to 7 Settings.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    SER_AVAIL_0TO7     avail;  ///< Bits indicate what "Serial Data Ports" are enabled
    BYTE                res;

    SER_CONFIG          uart1;
    SER_CONFIG          uart2;
    SER_CONFIG          uart3;
    SER_CONFIG          uart4;
    SER_CONFIG          spi1;
    SER_CONFIG          spi3;
    SER_CONFIG          i2c1;
    SER_CONFIG          i2c2;

    SER_CONFIG_UART     ser1Conf;
    SER_CONFIG_UART     ser2Conf;
    SER_CONFIG_UART     ser3Conf;
    SER_CONFIG_UART     ser4Conf;
    SER_CONFIG_SPI      spi1Conf;
    SER_CONFIG_SPI      spi3Conf;
    SER_CONFIG_I2C      i2c1Conf;
    SER_CONFIG_I2C      i2c2Conf;

    BYTE            reserve[26];        ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_SER_0TO7;


/**
 * "Serial Data Port" 8 to 15 Settings.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    SER_AVAIL_8TO15     avail;  ///< Bits indicate what "Serial Data Ports" are enabled
    BYTE                res;

    SER_CONFIG          i2c3;
    SER_CONFIG          udp1;
    SER_CONFIG          udp2;
    SER_CONFIG          udp3;
    SER_CONFIG          tcp1;
    SER_CONFIG          tcp2;
    SER_CONFIG          tcp3;
    SER_CONFIG          usb;

    SER_CONFIG_I2C      i2c3Conf;
    SER_CONFIG_UDP      udp1Conf;
    SER_CONFIG_UDP      udp2Conf;
    SER_CONFIG_UDP      udp3Conf;
    SER_CONFIG_TCP      tcp1Conf;
    SER_CONFIG_TCP      tcp2Conf;
    SER_CONFIG_TCP      tcp3Conf;
    SER_CONFIG_USB      usbConf;

    BYTE            reserve[26];        ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_SER_8TO15;


/**
 * "Serial Data Port" 16 to 23 Settings.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    SER_AVAIL_16TO23    avail;  ///< Bits indicate what "Serial Data Ports" are enabled
    BYTE                res;

    SER_CONFIG          user1;
    SER_CONFIG          user2;
    SER_CONFIG          user3;
    SER_CONFIG          user4;
    SER_CONFIG          user5;
    SER_CONFIG          user6;
    SER_CONFIG          user7;
    SER_CONFIG          user8;

    BYTE                user1Conf[4];
    BYTE                user2Conf[4];
    BYTE                user3Conf[4];
    BYTE                user4Conf[4];
    BYTE                user5Conf[4];
    BYTE                user6Conf[4];
    BYTE                user7Conf[4];
    BYTE                user8Conf[4];

    BYTE            reserve[26];        ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_SER_16TO23;


/**
 * "Serial Data Port" 24 to 31 Settings.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    SER_AVAIL_24TO31    avail;  ///< Bits indicate what "Serial Data Ports" are enabled
    BYTE                res;

    SER_CONFIG          res1;
    SER_CONFIG          res2;
    SER_CONFIG          res3;
    SER_CONFIG          res4;
    SER_CONFIG          res5;
    SER_CONFIG          res6;
    SER_CONFIG          res7;
    SER_CONFIG          res8;

    BYTE            reserve[26];        ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_SER_24TO31;



/////////////////////////////////////////////////
// "Serial Data Port" includes and Macros
/////////////////////////////////////////////////

#if defined(HAS_SERPORT_DEBUG)
    #define SERPORT_DEBUG_INIT() debugInit()
    #define SERPORT_DEBUG_TASK() debugService()
#else
    #define SERPORT_DEBUG_INIT()
    #define SERPORT_DEBUG_TASK()
#endif
#if defined(HAS_SERPORT_UART1) || defined(HAS_SERPORT_UART2) || defined(HAS_SERPORT_UART3) || defined(HAS_SERPORT_UART4)
    #include "nz_serUart.h"
#endif
#if defined(HAS_SERPORT_UART1)
    #define SERPORT_UART1_INIT() serUart1Init()
    #define SERPORT_UART1_TASK() serUart1Task()
#else
    #define SERPORT_UART1_INIT()
    #define SERPORT_UART1_TASK()
#endif

#if defined(HAS_SERPORT_UART2)
    #define SERPORT_UART2_INIT() serUart2Init()
    #define SERPORT_UART2_TASK() serUart2Task()
#else
    #define SERPORT_UART2_INIT()
    #define SERPORT_UART2_TASK()
#endif

#if defined(HAS_SERPORT_UART3)
    #define SERPORT_UART3_INIT() serUart3Init()
    #define SERPORT_UART3_TASK() serUart3Task()
#else
    #define SERPORT_UART3_INIT()
    #define SERPORT_UART3_TASK()
#endif

#if defined(HAS_SERPORT_UART4)
    #define SERPORT_UART4_INIT() serUart4Init()
    #define SERPORT_UART4_TASK() serUart4Task()
#else
    #define SERPORT_UART4_INIT()
    #define SERPORT_UART4_TASK()
#endif

#if defined(HAS_SERPORT_SPI1) || defined(HAS_SERPORT_SPI3) || defined(HAS_SERPORT_SPI4)
    #include "serSPI.h"
#endif
#if defined(HAS_SERPORT_SPI1)
    #define SERPORT_SPI1_INIT() serSPI1Init()
    #define SERPORT_SPI1_TASK() serSPI1Task()
#else
    #define SERPORT_SPI1_INIT()
    #define SERPORT_SPI1_TASK()
#endif

#if defined(HAS_SERPORT_SPI3)
    #define SERPORT_SPI3_INIT() serSPI3Init()
    #define SERPORT_SPI3_TASK() serSPI3Task()
#else
    #define SERPORT_SPI3_INIT()
    #define SERPORT_SPI3_TASK()
#endif

#if defined(HAS_SERPORT_I2C1) || defined(HAS_SERPORT_I2C2) || defined(HAS_SERPORT_I2C3)
    #include "nz_serI2C.h"
#endif
#if defined(HAS_SERPORT_I2C1)
    #define SERPORT_I2C1_INIT() serI2C1Init()
    #define SERPORT_I2C1_TASK() serI2CTask(&i2c1Info)
#else
    #define SERPORT_I2C1_INIT()
    #define SERPORT_I2C1_TASK()
#endif

#if defined(HAS_SERPORT_I2C2)
    #define SERPORT_I2C2_INIT() serI2C2Init()
    #define SERPORT_I2C2_TASK() serI2CTask(&i2c2Info)
#else
    #define SERPORT_I2C2_INIT()
    #define SERPORT_I2C2_TASK()
#endif

#if defined(HAS_SERPORT_I2C3)
    #define SERPORT_I2C3_INIT() serI2C3Init()
    #define SERPORT_I2C3_TASK() serI2CTask(&i2c3Info)
#else
    #define SERPORT_I2C3_INIT()
    #define SERPORT_I2C3_TASK()
#endif

#if defined(HAS_SERPORT_UDP1) || defined(HAS_SERPORT_UDP2) || defined(HAS_SERPORT_UDP3)
    #include "nz_serUDP.h"
#endif
#if defined(HAS_SERPORT_UDP1)
    #define SERPORT_UDP1_INIT() serUDP1Init()
    #define SERPORT_UDP1_TASK() serUDP1Task()
#else
    #define SERPORT_UDP1_INIT()
    #define SERPORT_UDP1_TASK()
#endif

#if defined(HAS_SERPORT_UDP2)
    #define SERPORT_UDP2_INIT() serUDP2Init()
    #define SERPORT_UDP2_TASK() serUDP2Task()
#else
    #define SERPORT_UDP2_INIT()
    #define SERPORT_UDP2_TASK()
#endif

#if defined(HAS_SERPORT_UDP3)
    #define SERPORT_UDP3_INIT() serUDP3Init()
    #define SERPORT_UDP3_TASK() serUDP3Task()
#else
    #define SERPORT_UDP3_INIT()
    #define SERPORT_UDP3_TASK()
#endif

#if defined(HAS_SERPORT_TCP1) || defined(HAS_SERPORT_TCP2) || defined(HAS_SERPORT_TCP3)
    #include "nz_serUDP.h"
#endif
#if defined(HAS_SERPORT_TCP1)
    #define SERPORT_TCP1_INIT() serTCP1Init()
    #define SERPORT_TCP1_TASK() serTCP1Task()
#else
    #define SERPORT_TCP1_INIT()
    #define SERPORT_TCP1_TASK()
#endif

#if defined(HAS_SERPORT_TCP2)
    #define SERPORT_TCP2_INIT() serTCP2Init()
    #define SERPORT_TCP2_TASK() serTCP2Task()
#else
    #define SERPORT_TCP2_INIT()
    #define SERPORT_TCP2_TASK()
#endif

#if defined(HAS_SERPORT_TCP3)
    #define SERPORT_TCP3_INIT() serTCP3Init()
    #define SERPORT_TCP3_TASK() serTCP3Task()
#else
    #define SERPORT_TCP3_INIT()
    #define SERPORT_TCP3_TASK()
#endif

#if defined(HAS_SERPORT_USB)
    #include "nz_serUSB.h"
    #define SERPORT_USB_INIT() serUSBInit()
    #define SERPORT_USB_TASK() serUSBTask()
#else
    #define SERPORT_USB_INIT()
    #define SERPORT_USB_TASK()
#endif


#if defined(HAS_SERPORT_USER1) || defined(HAS_SERPORT_USER2) || defined(HAS_SERPORT_USER3) || defined(HAS_SERPORT_USER4) || defined(HAS_SERPORT_USER5) || defined(HAS_SERPORT_USER6) || defined(HAS_SERPORT_USER7) || defined(HAS_SERPORT_USER8)
    #include "nz_serUDP.h"
#endif
#if defined(HAS_SERPORT_USER1)
    #define SERPORT_USER1_INIT() serUser1Init()
    #define SERPORT_USER1_TASK() serUser1Task()
#else
    #define SERPORT_USER1_INIT()
    #define SERPORT_USER1_TASK()
#endif
#if defined(HAS_SERPORT_USER2)
    #define SERPORT_USER2_INIT() serUser2Init()
    #define SERPORT_USER2_TASK() serUser2Task()
#else
    #define SERPORT_USER2_INIT()
    #define SERPORT_USER2_TASK()
#endif
#if defined(HAS_SERPORT_USER3)
    #define SERPORT_USER3_INIT() serUser3Init()
    #define SERPORT_USER3_TASK() serUser3Task()
#else
    #define SERPORT_USER3_INIT()
    #define SERPORT_USER3_TASK()
#endif
#if defined(HAS_SERPORT_USER4)
    #define SERPORT_USER4_INIT() serUser4Init()
    #define SERPORT_USER4_TASK() serUser4Task()
#else
    #define SERPORT_USER4_INIT()
    #define SERPORT_USER4_TASK()
#endif
#if defined(HAS_SERPORT_USER5)
    #define SERPORT_USER5_INIT() serUser5Init()
    #define SERPORT_USER5_TASK() serUser5Task()
#else
    #define SERPORT_USER5_INIT()
    #define SERPORT_USER5_TASK()
#endif
#if defined(HAS_SERPORT_USER6)
    #define SERPORT_USER6_INIT() serUser6Init()
    #define SERPORT_USER6_TASK() serUser6Task()
#else
    #define SERPORT_USER6_INIT()
    #define SERPORT_USER6_TASK()
#endif
#if defined(HAS_SERPORT_USER7)
    #define SERPORT_USER7_INIT() serUser7Init()
    #define SERPORT_USER7_TASK() serUser7Task()
#else
    #define SERPORT_USER7_INIT()
    #define SERPORT_USER7_TASK()
#endif
#if defined(HAS_SERPORT_USER8)
    #define SERPORT_USER8_INIT() serUser8Init()
    #define SERPORT_USER8_TASK() serUser8Task()
#else
    #define SERPORT_USER8_INIT()
    #define SERPORT_USER8_TASK()
#endif



/////////////////////////////////////////////////
// Make all "Serial Data Ports" available externally
/////////////////////////////////////////////////

#if !defined(THIS_IS_NZ_SERDATA_PORTS_C)
    extern SER_INFO     serInfo;

    extern SERPORTS     serports;
#endif



/////////////////////////////////////////////////
// Define TX and RX "Circular Buffers" for all "Serial Data Ports"
/////////////////////////////////////////////////
#define CIRBUF_TX_DEBUG    (&serports.debug.cbufTx)
#define CIRBUF_RX_DEBUG    (&serports.debug.cbufRx)

#define CIRBUF_TX_UART1     (&serports.uart1.cbufTx)
#define CIRBUF_RX_UART1     (&serports.uart1.cbufRx)
#define CIRBUF_TX_UART2     (&serports.uart2.cbufTx)
#define CIRBUF_RX_UART2     (&serports.uart2.cbufRx)
#define CIRBUF_TX_UART3     (&serports.uart3.cbufTx)
#define CIRBUF_RX_UART3     (&serports.uart3.cbufRx)
#define CIRBUF_TX_UART4     (&serports.uart4.cbufTx)
#define CIRBUF_RX_UART4     (&serports.uart4.cbufRx)
#define CIRBUF_TX_SPI1     (&serports.spi1.cbufTx)
#define CIRBUF_RX_SPI1     (&serports.spi1.cbufRx)
#define CIRBUF_TX_SPI3     (&serports.spi3.cbufTx)
#define CIRBUF_RX_SPI3     (&serports.spi3.cbufRx)
#define CIRBUF_TX_I2C1     (&serports.i2c1.cbufTx)
#define CIRBUF_RX_I2C1     (&serports.i2c1.cbufRx)
#define CIRBUF_TX_I2C2     (&serports.i2c2.cbufTx)
#define CIRBUF_RX_I2C2     (&serports.i2c2.cbufRx)

#define CIRBUF_TX_I2C3     (&serports.i2c3.cbufTx)
#define CIRBUF_RX_I2C3     (&serports.i2c3.cbufRx)
#define CIRBUF_TX_UDP1     (&serports.udp1.cbufTx)
#define CIRBUF_RX_UDP1     (&serports.udp1.cbufRx)
#define CIRBUF_TX_UDP2     (&serports.udp2.cbufTx)
#define CIRBUF_RX_UDP2     (&serports.udp2.cbufRx)
#define CIRBUF_TX_UDP3     (&serports.udp3.cbufTx)
#define CIRBUF_RX_UDP3     (&serports.udp3.cbufRx)
#define CIRBUF_TX_TCP1     (&serports.tcp1.cbufTx)
#define CIRBUF_RX_TCP1     (&serports.tcp1.cbufRx)
#define CIRBUF_TX_TCP2     (&serports.tcp2.cbufTx)
#define CIRBUF_RX_TCP2     (&serports.tcp2.cbufRx)
#define CIRBUF_TX_TCP3     (&serports.tcp3.cbufTx)
#define CIRBUF_RX_TCP3     (&serports.tcp3.cbufRx)
#define CIRBUF_TX_USB      (&serports.usb.cbufTx)
#define CIRBUF_RX_USB      (&serports.usb.cbufRx)

#define CIRBUF_TX_USER1    (&serports.user1.cbufTx)
#define CIRBUF_RX_USER1    (&serports.user1.cbufRx)
#define CIRBUF_TX_USER2    (&serports.user2.cbufTx)
#define CIRBUF_RX_USER2    (&serports.user2.cbufRx)
#define CIRBUF_TX_USER3    (&serports.user3.cbufTx)
#define CIRBUF_RX_USER3    (&serports.user3.cbufRx)
#define CIRBUF_TX_USER4    (&serports.user4.cbufTx)
#define CIRBUF_RX_USER4    (&serports.user4.cbufRx)
#define CIRBUF_TX_USER5    (&serports.user5.cbufTx)
#define CIRBUF_RX_USER5    (&serports.user5.cbufRx)
#define CIRBUF_TX_USER6    (&serports.user6.cbufTx)
#define CIRBUF_RX_USER6    (&serports.user6.cbufRx)
#define CIRBUF_TX_USER7    (&serports.user7.cbufTx)
#define CIRBUF_RX_USER7    (&serports.user7.cbufRx)
#define CIRBUF_TX_USER8    (&serports.user8.cbufTx)
#define CIRBUF_RX_USER8    (&serports.user8.cbufRx)


/**
 * Initialization code.
 */
void serInit(void);


/**
 * Allocate required memory for "Serial Data Ports".
 */
void serDefaultInit(void);


/**
 * Must be called every couple of ms
 *
 */
void serTask(void);

/**
 * Gets the "Circular Buffer" for the given @ref info_serport_bufid "Serial Data Port Buffer ID".
 *
 * @param bufid A @ref info_serport_bufid "Serial Data Port Buffer ID".
 */
#define getCirbufForSerportID(bufid) ( (bufid&0x01) ? ( &((SERPORT*)(NZ_GLOBALS_SERPORT_FIRST + (bufid/2)))->cbufTx) : ( &((SERPORT*)(NZ_GLOBALS_SERPORT_FIRST + (bufid/2)))->cbufRx) )


#define serIsEnabled(pSerport) ( ((pSerport->flags & SERPORT_STAT_ENABLED) != 0) && (pSerport!=NULL) )


#endif  //#ifndef NZ_SERDATAPORTS_H
