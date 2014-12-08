/**
 * @brief           Defines for  I/O ports
 * @file            nz_ioPortDefs.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_ioPortDefs_desc Description
 *****************************************
 * Defines for I/O ports
 *
 * @subsection nz_ioPortDefs_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ---------------------- ioPorts Configuration ------------------------
// *********************************************************************

 @endcode
 *
 **********************************************************************
 * @section nz_ioPortDefs_lic Software License Agreement
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
 * 2013-11-20, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef IO_PORT_DEFS_H
#define IO_PORT_DEFS_H

/////////// Defines /////////////////////////////
#define IOPORT_ID_NA   	0xff    //Port not available (use 0xff = erased flash)
#define UCPORT_ID_NA    0xff    //UCPort not available (use 0xff = erased flash)
#define UCPORT_INDEX_NA 0xff    //UCPort Index not available (use 0xff = erased flash)
#define BITADR_NA       0xffff  //Bit Address not available (use 0xffff = erased flash)

//Define addresses for CNPD1 and CNPU6 for different CPUs
#if defined(__PIC24FJ256GB206__) || defined(__PIC24FJ256GB208__) || defined(__PIC24FJ256GB210__)
#define CNPD1_ADR   0x0056
#define CNPU6_ADR   0x0078
#elif defined(__PIC24FJ128GB106__) || defined(__PIC24FJ128GB108__) || defined(__PIC24FJ128GB110__)
#define CNPD1_ADR   0x0054
#define CNPU6_ADR   0x0076
#endif


/////////////////////////////////////////////////
//Port type rules (UCPORT_CONFIG.type)
// - Bits 8-15 is a PCFG_XXX_TYPE constant (or bits 8-15 of PTYPE_XXX), giving type of the pin
//   ~ 0-0x7F are inputs
//   ~ 0x80-0xFF are ouputs
// - Bit 0-7 gives details about the type
//For Output types:
// - Bit 7 specifies it is an open collector output
//For Input types:
// - Bit 7 specifies pull-up resistor is enabled
// - Bit 6 specifies pull-down resistor is enabled

#define PCFG_NA_TYPE            0xff    // Port Not available (use 0xff = erased flash)

/////////////////////////////////////////////////
//Inputs Types

//Flags for all input types
#define PCFG_IN_FL_PULLUP       0x80    // Pull-up enabled Flag (can be ORed with other PCFG_DIN_xxx defines)
#define PCFG_IN_FL_PULLDOWN     0x40    // Pull-down enabled Flag (can be ORed with other PCFG_DIN_xxx defines)

//Port Config, H = bits 8-15, L = bits 0-7. For example, a 2.5V, 10-bit ADC input has has type code = 0x0100. A 12-bit variant would be 0x0110
#define PCFG_ADC_TYPE           0x01    // ADC Input - ##### TYPE #####
#define PTYPE_ADC               0x0100
#define PCFG_ADC_PULLUP         PCFG_IN_FL_PULLUP       // Pull-up enabled
#define PCFG_ADC_PULLDOWN       PCFG_IN_FL_PULLDOWN     // Pull-down enabled
#define PCFG_ADC_2V5_10BIT      0x00    // 10-bit ADC port, using external 2.5V reference
#define PCFG_ADC_3V3_10BIT      0x01    // 10-bit ADC port, using internal 3.3V Avcc as reference
#define PCFG_ADC_2V5_12BIT      0x10    // 12-bit ADC port, using external 2.5V reference
#define PCFG_ADC_3V3_12BIT      0x11    // 12-bit ADC port, using internal 3.3V Avcc as reference

#define PCFG_DIN_TYPE           0x02    // Digital input - ##### TYPE #####
#define PTYPE_DIN               0x0200  // Digital input - ##### TYPE #####
#define PCFG_DIN_PULLUP         PCFG_IN_FL_PULLUP       // Pull-up enabled
#define PCFG_DIN_PULLDOWN       PCFG_IN_FL_PULLDOWN     // Pull-down enabled
#define PCFG_DIN_3V3_IN         0x00    // 3V3 Digital input
#define PCFG_DIN_5V_IN          0x01    // 5V Digital input

/////////////////////////////////////////////////
//UART Receive Types
#define PCFG_UART_RX_TYPE       0x03    // UART Input - ##### TYPE #####
#define PTYPE_UART_IN           0x0300
#define PCFG_UART_PULLUP        PCFG_IN_FL_PULLUP       // Pull-up enabled
#define PCFG_UART_PULLDOWN      PCFG_IN_FL_PULLDOWN     // Pull-down enabled
//Bits 0-3 gives UART number, can be 1-8
#define PCFG_UART_NUMER_1       0x00    // UART 1
#define PCFG_UART_NUMER_2       0x01    // UART 2
#define PCFG_UART_NUMER_3       0x02    // UART 3
#define PCFG_UART_NUMER_4       0x03    // UART 4
#define PCFG_UART_NUMER_5       0x04    // UART 5
#define PCFG_UART_NUMER_6       0x05    // UART 6
#define PCFG_UART_NUMER_7       0x06    // UART 7
#define PCFG_UART_NUMER_8       0x07    // UART 8

/////////////////////////////////////////////////
//USART Input Types
#define PCFG_USART_RX_TYPE      0x04    // USART Input - ##### TYPE #####
#define PTYPE_USART_RX          0x0400
#define PCFG_USART_PULLUP       PCFG_IN_FL_PULLUP       // Pull-up enabled
#define PCFG_USART_PULLDOWN     PCFG_IN_FL_PULLDOWN     // Pull-down enabled
//Bits 0-3 gives USART number, can be 1-8
#define PCFG_USART_NUMER_1      0x00    // USART 1
#define PCFG_USART_NUMER_2      0x01    // USART 2
#define PCFG_USART_NUMER_3      0x02    // USART 3
#define PCFG_USART_NUMER_4      0x03    // USART 4
#define PCFG_USART_NUMER_5      0x04    // USART 5
#define PCFG_USART_NUMER_6      0x05    // USART 6
#define PCFG_USART_NUMER_7      0x06    // USART 7
#define PCFG_USART_NUMER_8      0x07    // USART 8

/////////////////////////////////////////////////
//SPI Input Types
#define PCFG_SPI_SDI_TYPE       0x05    // SPI Data Input - ##### TYPE #####
#define PTYPE_SPI_SDI           0x0500
#define PCFG_SPI_SS_TYPE        0x06    // SPI Slave Select Input - ##### TYPE #####
#define PTYPE_SPI_SS            0x0600
#define PCFG_SPI_PULLUP         PCFG_IN_FL_PULLUP       // Pull-up enabled
#define PCFG_SPI_PULLDOWN       PCFG_IN_FL_PULLDOWN     // Pull-down enabled
//Bits 0-3 gives SPI number, can be 1-8
#define PCFG_SPI_NUMBER_1       0x00    // SPI 1
#define PCFG_SPI_NUMBER_2       0x01    // SPI 2
#define PCFG_SPI_NUMBER_3       0x02    // SPI 3
#define PCFG_SPI_NUMBER_4       0x03    // SPI 4
#define PCFG_SPI_NUMBER_5       0x04    // SPI 5
#define PCFG_SPI_NUMBER_6       0x05    // SPI 6
#define PCFG_SPI_NUMBER_7       0x06    // SPI 7
#define PCFG_SPI_NUMBER_8       0x07    // SPI 8



/////////////////////////////////////////////////
//Outputs Types

//Flags for all output types
#define PCFG_OUT_FL_OPEN_COLL   0x80    // Open Collector enabled Flag (can be ORed with other PCFG_xxx defines)

//Outputs
#define PCFG_AOUT_TYPE          0x81    // Analog Output - ##### TYPE #####
#define PTYPE_AOUT              0x8100
#define PCFG_AOUT_3V3           0x00    // 3V3 Digital output
#define PCFG_AOUT_3V3_OC        0x01    // 3V3 Open Collector output. NOT 5V tolerant
#define PCFG_AOUT_5V            0x02    // 5V Digital output (push-pull)
#define PCFG_AOUT_5V_OC         0x03    // 5V tolerant Open Collector output

#define PCFG_DOUT_TYPE          0x82    // Digital Out - ##### TYPE #####
#define PTYPE_DOUT              0x8200
#define PCFG_DOUT_3V3           0x00    // 3V3 Digital output
#define PCFG_DOUT_3V3_OC        0x01    // 3V3 Open Collector output. NOT 5V tolerant
#define PCFG_DOUT_5V            0x02    // 5V Digital output (push-pull)
#define PCFG_DOUT_5V_OC         0x03    // 5V tolerant Open Collector output

#define PCFG_PWM_TYPE           0x83    // PWM Output - ##### TYPE #####
#define PTYPE_PWM               0x8300
#define PCFG_PWM_3V3_10BIT      0x00    // 3V3 Digital output, 10 bit
#define PCFG_PWM_3V3_OC_10BIT   0x01    // 3V3 Open COllector output, 10 bit. NOT 5V tolerant
#define PCFG_PWM_5V_10BIT       0x02    // 5V Digital output (push-pull), 10 bit
#define PCFG_PWM_5V_OC_10BIT    0x03    // 5V tolerant Open Collector output, 10 bit
#define PCFG_PWM_3V3_12BIT      0x10    // 3V3 Digital output, 12 bit
#define PCFG_PWM_3V3_OC_12BIT   0x11    // 3V3 Open COllector output, 12 bit. NOT 5V tolerant
#define PCFG_PWM_5V_12BIT       0x12    // 5V Digital output (push-pull), 12 bit
#define PCFG_PWM_5V_OC_12BIT    0x13    // 5V tolerant Open Collector output, 12 bit

#define PCFG_VREFOUT_TYPE       0x84    // High impedance Reference output, for all PICs looked at this is RB10 pin - ##### TYPE #####
#define PTYPE_VREFOUT           0x8400
#define PCFG_VREFOUT_xxx        0x00    // Reference voltage is set to ?V

/////////////////////////////////////////////////
//UART Output Types
#define PCFG_UART_OUT_TYPE      0x85    // UART Output - ##### TYPE #####
#define PTYPE_UART_OUT          0x8500
//Bits 0-3 gives UART number, can be 1-8. Use PCFG_UART_NUMBER_X defines

/////////////////////////////////////////////////
//USART Output Types
#define PCFG_USART_OUT_TYPE    0x86    // USART Output - ##### TYPE #####
#define PTYPE_USART_OUT        0x8600
//Bits 0-3 gives USART number, can be 1-8. Use PCFG_USART_NUMBER_X defines

/////////////////////////////////////////////////
//SPI Output Types
#define PCFG_SPI_SDO_TYPE       0x87    // SPI Data Output - ##### TYPE #####
#define PTYPE_SPI_SDO           0x8700
#define PCFG_SPI_CLK_TYPE       0x88    // SPI Clock Output - ##### TYPE #####
#define PTYPE_SPI_CLK           0x8800
//Bits 0-3 gives SPI number, can be 1-8.  Use PCFG_SPI_NUMBER_X defines

/////////////////////////////////////////////////
//I2C Types
#define PCFG_I2C_SDA_TYPE       0x89    // I2C Serial Data - ##### TYPE #####
#define PTYPE_I2C_SDA           0x8900
#define PCFG_I2C_SCL_TYPE       0x8A    // I2C Serial Clock - ##### TYPE #####
#define PTYPE_I2C_SCL           0x8A00
//Bits 0-3 gives I2C number, can be 1-8.
#define PCFG_I2C_NUMBER_1       0x00    // I2C 1
#define PCFG_I2C_NUMBER_2       0x01    // I2C 2
#define PCFG_I2C_NUMBER_3       0x02    // I2C 3
#define PCFG_I2C_NUMBER_4       0x03    // I2C 4
#define PCFG_I2C_NUMBER_5       0x04    // I2C 5
#define PCFG_I2C_NUMBER_6       0x05    // I2C 6
#define PCFG_I2C_NUMBER_7       0x06    // I2C 7
#define PCFG_I2C_NUMBER_8       0x07    // I2C s8


extern const WORD TRIS_PORTID_BITADR_MAP[];

extern const BYTE PORTID_UCPORT_MAP[];

/** Get number of UCPORT_CONFIG structures. This is the number of Microcontroller (UC) ports that can be configured */
#define getUcPortNumber() ((sizeof(CFG_BLOCK_UC66_PORT) - sizeof(((CFG_BLOCK_UC66_PORT*)0)->reserve)) / sizeof(UCPORT_CONFIG))
#define UC_PORT_NUMBER    ((sizeof(CFG_BLOCK_UC66_PORT) - sizeof(((CFG_BLOCK_UC66_PORT*)0)->reserve)) / sizeof(UCPORT_CONFIG))

/**
 * Get index of given port in CFG_BLOCK_UC66_PORT. This is used for various tables
 *
 * @param ele The "Microcontroller Port". Can use a literal constant, like "B5" for example.
 *            Or, can use the UCPORT_xx defines, for example UCPORT_05
 */
#define GET_UCPORT_INDEX(ele)    (offsetof(CFG_BLOCK_UC66_PORT, ele) / sizeof(UCPORT_CONFIG))


/** Microcontroller (UC) Port IDs */
typedef enum _UCPORT_CONFIG_ID
{
    UCPORT_ID_A0 = 0, UCPORT_ID_A1, UCPORT_ID_A2, UCPORT_ID_A3, UCPORT_ID_A4, UCPORT_ID_A5, UCPORT_ID_A6, UCPORT_ID_A7,
    UCPORT_ID_A8, UCPORT_ID_A9, UCPORT_ID_A10, UCPORT_ID_A11, UCPORT_ID_A12, UCPORT_ID_A13, UCPORT_ID_A14, UCPORT_ID_A15,

    UCPORT_ID_B0 = 16, UCPORT_ID_B1, UCPORT_ID_B2, UCPORT_ID_B3, UCPORT_ID_B4, UCPORT_ID_B5, UCPORT_ID_B6, UCPORT_ID_B7,
    UCPORT_ID_B8, UCPORT_ID_B9, UCPORT_ID_B10, UCPORT_ID_B11, UCPORT_ID_B12, UCPORT_ID_B13, UCPORT_ID_B14, UCPORT_ID_B15,

    UCPORT_ID_C0 = 32, UCPORT_ID_C1, UCPORT_ID_C2, UCPORT_ID_C3, UCPORT_ID_C4, UCPORT_ID_C5, UCPORT_ID_C6, UCPORT_ID_C7,
    UCPORT_ID_C8, UCPORT_ID_C9, UCPORT_ID_C10, UCPORT_ID_C11, UCPORT_ID_C12, UCPORT_ID_C13, UCPORT_ID_C14, UCPORT_ID_C15,

    UCPORT_ID_D0 = 48, UCPORT_ID_D1, UCPORT_ID_D2, UCPORT_ID_D3, UCPORT_ID_D4, UCPORT_ID_D5, UCPORT_ID_D6, UCPORT_ID_D7,
    UCPORT_ID_D8, UCPORT_ID_D9, UCPORT_ID_D10, UCPORT_ID_D11, UCPORT_ID_D12, UCPORT_ID_D13, UCPORT_ID_D14, UCPORT_ID_D15,

    UCPORT_ID_E0 = 64, UCPORT_ID_E1, UCPORT_ID_E2, UCPORT_ID_E3, UCPORT_ID_E4, UCPORT_ID_E5, UCPORT_ID_E6, UCPORT_ID_E7,
    UCPORT_ID_E8, UCPORT_ID_E9, UCPORT_ID_E10, UCPORT_ID_E11, UCPORT_ID_E12, UCPORT_ID_E13, UCPORT_ID_E14, UCPORT_ID_E15,

    UCPORT_ID_F0 = 80, UCPORT_ID_F1, UCPORT_ID_F2, UCPORT_ID_F3, UCPORT_ID_F4, UCPORT_ID_F5, UCPORT_ID_F6, UCPORT_ID_F7,
    UCPORT_ID_F8, UCPORT_ID_F9, UCPORT_ID_F10, UCPORT_ID_F11, UCPORT_ID_F12, UCPORT_ID_F13, UCPORT_ID_F14, UCPORT_ID_F15,

    UCPORT_ID_G0 = 96, UCPORT_ID_G1, UCPORT_ID_G2, UCPORT_ID_G3, UCPORT_ID_G4, UCPORT_ID_G5, UCPORT_ID_G6, UCPORT_ID_G7,
    UCPORT_ID_G8, UCPORT_ID_G9, UCPORT_ID_G10, UCPORT_ID_G11, UCPORT_ID_G12, UCPORT_ID_G13, UCPORT_ID_G14, UCPORT_ID_G15,
} UCPORT_CONFIG_ID;

#define UCPORT_ID_MAX UCPORT_ID_G15

typedef struct __attribute__((__packed__)) _PCFG_DIN
{
    BYTE    id;
    BYTE    type;
    BYTE    typeConf;
    BYTE    param[2];
    struct
    {
        unsigned char bFill : 6;            ///< Fill to ensure flags take up a 8-bit WORD
        unsigned char bPulldown : 1;        ///< Pull-Down resistor is enabled
        unsigned char bPullup : 1;          ///< Pull-Up resistor is enabled
    } flags;
} UCPORT_CONFIG_DIN;

typedef struct __attribute__((__packed__)) _PCFG_DOUT
{
    BYTE    id;
    BYTE    type;
    BYTE    typeConf;
    BYTE    param[2];
    struct
    {
        unsigned char bFill : 6;            ///< Fill to ensure flags take up a 8-bit WORD
        unsigned char bOpenCollector : 1;   ///< Open Collector
        unsigned char bDefault : 1;         ///< Default value if this port is a digital output. For other types, default value is defined elsewhere.
    } flags;
} UCPORT_CONFIG_DOUT;

/**
 * Microcontroller (UC) Port Config. Is used to configure a port of the CPU
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef union __attribute__((__packed__)) _UCPORT_CONFIG
{
    BYTE arr[6];

    //General structure, param[0] is typeConf
    struct {
        BYTE    id;         ///< Microcontroller Port ID. Can NOT BE CHANGED! Port A0-A15=0-15, B0-B15=16-31, C0-C15=32-47, D0-D15=48-63, E0-E15=64-79, F0-F15=80-95, G0-G15=96-111
        BYTE    type;       ///< Hardware Type
        BYTE    typeConf;   ///< Type configuration
        //Additional parameters, for example default value of a output port, pull-up ad pull-down resistors, open collector output....
        BYTE    param[3];
    } gen;

    UCPORT_CONFIG_DOUT dout;    //Digital output = PCFG_DOUT_TYPE

    UCPORT_CONFIG_DIN din;      //Digital input = PCFG_DIN_TYPE
} UCPORT_CONFIG;


/**
 * IO Port Config (of SBC board). Is used to configure the IO ports, like Daughter Board Connector pins, FFC Connector Pins....
 */
typedef struct __attribute__((__packed__))
{
    BYTE    label;      ///< Port Label, is a "String Table" index value. Is 0xff if nothing (unprogrammed Flash = 0xff)
} IOPORT_CONFIG;


/**
 * Microcontroller (UC) Port Configuration for boards. Must contain
 * ALL configurable ports.
 * Ensure all elements, and order of elements is EXACTLY the same as in the TABLE_BLOCK_UC66_PORT structure!
 *
 * To read this structure from configuration data, modify following sample code:
 * @code
 * //Get UCPORT_CONFIG for port 1, using UCPORT_xx defines (creates smaller code!)
 * UCPORT_CONFIG ucportConf;
 * cfgGetArray(GET_UCPORT_CONFIG_OFFSET(UCPORT_01), (BYTE *)&ucportConf, sizeof(ucportConf));
 *
 * //Get UCPORT_CONFIG for a "Port ID"
 * UCPORT_CONFIG ucportConfig;
 * index = ucportGetIndex(1);
 * if (index != UCPORT_INDEX_NA) {
 *     cfgGetArray(offsetof(CFG_STRUCT, ucPort) + (index * sizeof(UCPORT_CONFIG)), (BYTE *)&ucportConfig, sizeof(ucportConfig));
 * }
 * @endcode
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    UCPORT_CONFIG       B0;
    UCPORT_CONFIG       B1;
    UCPORT_CONFIG       B2;
    UCPORT_CONFIG       B3;
    UCPORT_CONFIG       B4;
    UCPORT_CONFIG       B5;
    UCPORT_CONFIG       B6;
    UCPORT_CONFIG       B7;
    UCPORT_CONFIG       B8;
    UCPORT_CONFIG       B9;
    UCPORT_CONFIG       B10;
    UCPORT_CONFIG       B11;
    UCPORT_CONFIG       B12;
    UCPORT_CONFIG       B13;
    UCPORT_CONFIG       B14;
    UCPORT_CONFIG       B15;

    UCPORT_CONFIG       D0;
    UCPORT_CONFIG       D1;
    UCPORT_CONFIG       D2;
    UCPORT_CONFIG       D3;
    UCPORT_CONFIG       D6;
    UCPORT_CONFIG       D7;
    UCPORT_CONFIG       D8;
    UCPORT_CONFIG       D9;
    UCPORT_CONFIG       D10;

    UCPORT_CONFIG       E2;
    UCPORT_CONFIG       E3;
    UCPORT_CONFIG       E4;
    UCPORT_CONFIG       E5;
    UCPORT_CONFIG       E6;
    UCPORT_CONFIG       E7;

    UCPORT_CONFIG       F0;
    UCPORT_CONFIG       F1;
    UCPORT_CONFIG       F3;
    UCPORT_CONFIG       F4;
    UCPORT_CONFIG       F5;

    UCPORT_CONFIG       G6;
    UCPORT_CONFIG       G7;
    UCPORT_CONFIG       G8;
    UCPORT_CONFIG       G9;

    BYTE                reserve[12];    ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_UC66_PORT;


/**
 * IO Port Configuration (ports of SBC).
 */
typedef struct __attribute__((__packed__))
{
    IOPORT_CONFIG  P00;
    IOPORT_CONFIG  P01;
    IOPORT_CONFIG  P02;
    IOPORT_CONFIG  P03;
    IOPORT_CONFIG  P04;
    IOPORT_CONFIG  P05;
    IOPORT_CONFIG  P06;
    IOPORT_CONFIG  P07;
    IOPORT_CONFIG  P08;
    IOPORT_CONFIG  P09;
    IOPORT_CONFIG  P10;
    IOPORT_CONFIG  P11;
    IOPORT_CONFIG  P12;
    IOPORT_CONFIG  P13;
    IOPORT_CONFIG  P14;
    IOPORT_CONFIG  P15;
    IOPORT_CONFIG  P16;
    IOPORT_CONFIG  P17;
    IOPORT_CONFIG  P18;
    IOPORT_CONFIG  P19;
    IOPORT_CONFIG  P20;
    IOPORT_CONFIG  P21;
    IOPORT_CONFIG  P22;
    IOPORT_CONFIG  P23;
    IOPORT_CONFIG  P24;
    IOPORT_CONFIG  P25;
    IOPORT_CONFIG  P26;
    IOPORT_CONFIG  P27;
    IOPORT_CONFIG  P28;
    IOPORT_CONFIG  P29;
    IOPORT_CONFIG  P30;
    IOPORT_CONFIG  P31;
    IOPORT_CONFIG  P32;
    IOPORT_CONFIG  P33;
    IOPORT_CONFIG  P34;
    IOPORT_CONFIG  P35;
    IOPORT_CONFIG  P36;
    IOPORT_CONFIG  P37;
    IOPORT_CONFIG  P38;
    IOPORT_CONFIG  P39;
    IOPORT_CONFIG  P40;
    IOPORT_CONFIG  P41;

    BYTE            reserve[18];    ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_SBC66_PORT;



/// Microcontroller (UC) port property flags. Specifies what the hardware is capable of.
#define UCPORT_PROP_NA_VAL      0xffffffff      ///< Not defined (use 0xff = erased flash)
#define UCPORT_PROP_NA          0x80000000      ///< When set, all 32-bits are invalid = not available!
#define UCPORT_PROP_DIG_IN      0x00000001      ///< Digital Input
#define UCPORT_PROP_DIG_OUT     0x00000002      ///< Digital Output
#define UCPORT_PROP_ANALOG_IN   0x00000004      ///< Analog Input
#define UCPORT_PROP_OPEN_COL    0x00000008      ///< Open Collector
#define UCPORT_PROP_RP          0x00000010      ///< Remappable input/output pin
#define UCPORT_PROP_RPI         0x00000020      ///< Remappable input pin
#define UCPORT_PROP_PULLUP      0x00000040      ///< Pull-Up
#define UCPORT_PROP_PULLDOWN    0x00000080      ///< Pull-Down
#define UCPORT_PROP_5V          0x00000100      ///< 5V tolerant
#define UCPORT_PROP_SDA         0x00000200      ///< I2C Data
#define UCPORT_PROP_SCL         0x00000400      ///< I2C Clock
#define UCPORT_PROP_INT         0x00000800      ///< Interrupt Input

//#define UCPORT_PROP_ANALOG_OUT  0x00000000      ///< Analog Output
//#define UCPORT_PROP_INT         0x00000000      ///< Interrupt Input
//#define UCPORT_PROP_T1CK        0x00000000      ///< Timer 3 Clock

/**
 * Microcontroller (UC) Port Property. Is used to give properties of the CPU ports
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    DWORD_VAL   prop;   ///< Port Property. Is a combination of UCPORT_PROP_xxx constants
} UCPORT_PROP;

/** IO Port properties (ports of SBC) */
typedef struct __attribute__((__packed__))
{
    // Contains the index for (UCPORT_INDEX_NA = not available):
    // - The UCPORT_PROP element in the TABLE_BLOCK_UC66_PORT structure
    // - The UCPORT_CONFIG element in the CFG_BLOCK_UC66_PORT structure
    // To get the UCPORT_CONFIG structure in CFG_BLOCK_UC66_PORT (cfgStruct.ucPort) for this
    // IO Port, use the following code:
    //  UCPORT_CONFIG ucport
    //  if (index != 0xff)
    //      cfgGetDfltArray(offsetof(CFG_STRUCT, ucPort) + (sizeof(UCPORT_CONFIG) * index), ((BYTE*)&ucport), sizeof(UCPORT_CONFIG));
    BYTE ucPortIndex;   ///< Contains the index of the UCPORT_CONFIG structure (in cfgStruct.ucPort), for this "IO Port". Or, 0xff if not available

    /// Fixed String name. Each IO Port has a fixed name, maximum 3 char long. An additional label can be assigned for a IO Port via
    /// cfgStruct.ioPort. Does NOT have a NULL terminator if 3 characters long! If 1 or 2 characters long, it DOES have a NULL terminator
    BYTE name[3];
} IOPORT_PROP;


/**
 * Microcontroller (UC) Port Properties for board.
 * Ensure all elements, and order of elements is EXACTLY the same as in the CFG_BLOCK_UC66_PORT structure!
 *
 * To read this structure from configuration data, modify following sample code:
 * @code
 * //Get UCPORT_PROP for Port 1 (old port name X1), using UCPORT_xx defines (creates smaller code!)
 * UCPORT_PROP ucportProp;
 * cfgGetDfltArray(GET_UCPORT_PROP_OFFSET(UCPORT_01), (BYTE *)&ucportProp, sizeof(ucportProp));
 *
 * //Get UCPORT_CONFIG for a Port 1 (old port name X1), using the "Port ID"
 * UCPORT_PROP ucportProp;
 * index = ucportGetIndex(1);
 * if (index != UCPORT_INDEX_NA) {
 *     cfgGetArray(offsetof(CFG_STRUCT, ucPort) + (index * sizeof(UCPORT_CONFIG)), (BYTE *)&ucportConfig, sizeof(ucportConfig));
 * }
 * @endcode
 * EXACTLY THE SAME members (in same location) as CFG_BLOCK_UC66_PORT
 */
typedef struct __attribute__((__packed__))
{
    UCPORT_PROP     B0;
    UCPORT_PROP     B1;
    UCPORT_PROP     B2;
    UCPORT_PROP     B3;
    UCPORT_PROP     B4;
    UCPORT_PROP     B5;
    UCPORT_PROP     B6;
    UCPORT_PROP     B7;
    UCPORT_PROP     B8;
    UCPORT_PROP     B9;
    UCPORT_PROP     B10;
    UCPORT_PROP     B11;
    UCPORT_PROP     B12;
    UCPORT_PROP     B13;
    UCPORT_PROP     B14;
    UCPORT_PROP     B15;

    UCPORT_PROP     D0;
    UCPORT_PROP     D1;
    UCPORT_PROP     D2;
    UCPORT_PROP     D3;
    UCPORT_PROP     D6;
    UCPORT_PROP     D7;
    UCPORT_PROP     D8;
    UCPORT_PROP     D9;
    UCPORT_PROP     D10;

    UCPORT_PROP     E2;
    UCPORT_PROP     E3;
    UCPORT_PROP     E4;
    UCPORT_PROP     E5;
    UCPORT_PROP     E6;
    UCPORT_PROP     E7;

    UCPORT_PROP     F0;
    UCPORT_PROP     F1;
    UCPORT_PROP     F3;
    UCPORT_PROP     F4;
    UCPORT_PROP     F5;

    UCPORT_PROP     G6;
    UCPORT_PROP     G7;
    UCPORT_PROP     G8;
    UCPORT_PROP     G9;

    BYTE            reserve[13];    ///< Reserve, and get block size on 16 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (6 bytes long). Use cfgInfo debug message to confirm!
} TABLE_BLOCK_UC66_PORT;


/**
 * IO Port Properties (ports of SBC).
 */
typedef struct __attribute__((__packed__))
{
    IOPORT_PROP    P00;
    IOPORT_PROP    P01;
    IOPORT_PROP    P02;
    IOPORT_PROP    P03;
    IOPORT_PROP    P04;
    IOPORT_PROP    P05;
    IOPORT_PROP    P06;
    IOPORT_PROP    P07;
    IOPORT_PROP    P08;
    IOPORT_PROP    P09;
    IOPORT_PROP    P10;
    IOPORT_PROP    P11;
    IOPORT_PROP    P12;
    IOPORT_PROP    P13;
    IOPORT_PROP    P14;
    IOPORT_PROP    P15;
    IOPORT_PROP    P16;
    IOPORT_PROP    P17;
    IOPORT_PROP    P18;
    IOPORT_PROP    P19;
    IOPORT_PROP    P20;
    IOPORT_PROP    P21;
    IOPORT_PROP    P22;
    IOPORT_PROP    P23;
    IOPORT_PROP    P24;
    IOPORT_PROP    P25;
    IOPORT_PROP    P26;
    IOPORT_PROP    P27;
    IOPORT_PROP    P28;
    IOPORT_PROP    P29;
    IOPORT_PROP    P30;
    IOPORT_PROP    P31;
    IOPORT_PROP    P32;
    IOPORT_PROP    P33;
    IOPORT_PROP    P34;
    IOPORT_PROP    P35;
    IOPORT_PROP    P36;
    IOPORT_PROP    P37;
    IOPORT_PROP    P38;
    IOPORT_PROP    P39;
    IOPORT_PROP    P40;
    IOPORT_PROP    P41;

    BYTE            reserve[20];    ///< Reserve, and get block size on 16 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (6 bytes long). Use cfgInfo debug message to confirm!
} TABLE_BLOCK_SBC66_PORT;

#endif

