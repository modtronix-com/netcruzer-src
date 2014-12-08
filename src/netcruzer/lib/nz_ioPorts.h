/**
 * @brief           Code for using and configuring I/O ports
 * @file            nz_ioPorts.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_ioPorts_desc Description
 *****************************************
 * Code for using and configuring I/O ports
 *
 * @subsection nz_ioPorts_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// ---------------------- ioPorts Configuration ------------------------
// *********************************************************************
//Optimize for size, not speed!
#define    IOPORTS_OPTIMIZE_SIZE

 @endcode
 *
 **********************************************************************
 * @section nz_ioPorts_lic Software License Agreement
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
#ifndef IO_PORTS_H
#define IO_PORTS_H

#include "nz_ioPortDefs.h"



/**
 * Initialization
 */
void portInit(void);

/**
 * Configure I/O Port (ports of SBC) with the given configuration data.
 * The first parameter is the "Port ID", and the second is an ORed combination of
 * PCFG_XX and PTYPE_XX defines. The PTYPE_XX define gives the type of port, and the
 * PCFG_XX define give optional configuration. Common combinations are:
 * - Digital input pin = (PTYPE_DIN)
 * - Digital input pin, with pull-up resistor = (PTYPE_DIN | PCFG_DIN_PULLUP)
 * - Digital input pin, with pull-down resistor = (PTYPE_DIN | PCFG_DIN_PULLDOWN)
 * - Digital output pin = (PTYPE_DOUT)
 * - Analog input pin = (PTYPE_ADC)
 * - PWM output pin = (PTYPE_PWM)
 *
 * ----- Details -----
 * The upper byte (bit 8-15) of the given typeConf parameter is a PCFG_XXX_TYPE
 * define, like PCFG_DIN_TYPE for example.
 * The lower byte (bit 0-7) are PCFG_XXX flags, like PCFG_IN_FL_PULLUP for example.
 * 
 * The following example configures port 5 as a digital output:
 * @code
 * portConfig(10, PTYPE_DOUT)
 * @endcode
 *
 * The following example configures port 10 as a digital input, with pull-up resistor enabled:
 * @code
 * portConfig(10, PTYPE_DIN | PCFG_DIN_PULLUP)
 * @endcode
 *
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 * @param typeConf Type and configuration
 *
 * @return Returns 0 if successful, else error occurred.
 */
BYTE portConfig(BYTE portID, WORD typeConf);

/**
 * Gets an index for the given "Port ID". The index is for:
 * - The UCPORT_PROP element in the TABLE_BLOCK_UC66_PORT structure
 * - The UCPORT_CONFIG element in the CFG_BLOCK_UC66_PORT structure
 * The index can be used to get the UCPORT_PROP or UCPORT_CONFIG structure.
 * @code
 * UCPORT_PROP ucportProp;
 * BYTE index = ucportGetIndex(1);
 * if (index != UCPORT_INDEX_NA) {
 *     //Read UCPORT_PROP into ucportProp
 *     cfgGetDfltArray(offsetof(CFG_STRUCT, ucPortProp) + (index * sizeof(UCPORT_PROP)), (BYTE *)&ucportProp, sizeof(ucportProp));
 * }
 * @endcode
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns the index of UCPORT_PROP element in TABLE_BLOCK_UC66_PORT, and UCPORT_CONFIG
 *         element in CFG_BLOCK_UC66_PORT.
 *         Return UCPORT_INDEX_NA if not available, or error.
 */
BYTE ucportGetIndex(BYTE portID);


/**
 * Gets the "Microcontroller port ID"(a UCPORT_ID_xx define) that is mapped to the given
 * "Port ID" (a number from 0 to 41, also known as the "Netcruzer Port Name").
 * For example, on the SBC66EC, port 1 is connected to Microcontroller port RB1. The following
 * function will return UCPORT_ID_B1:
 * @code
 * BYTE ucport;
 * ucport = portGetUcportID(1);
 * @endcode
 *
 * The returned "Microcontroller port ID" is used by many port functions.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns the "Microcontroller Port ID" (a UCPORT_ID_xx define) for the given "Port ID".
 *      For example UCPORT_ID_B1. Will return UCPORT_ID_NA if the given IO Port(port of SBC) is not connected
 *      to a Microcontroller port.
 */
BYTE portGetUcportID(BYTE portID);


/**
 * Gets the "Port ID" (a number from 0 to 41, also known as the "Netcruzer Port Name")
 * for the given string. String must contain 1 or 2 digits. For numbers less than 10, can
 * have an optional leading 0.
 *
 * @param str A string representing the "Port ID" of the requested port. Is a number from 0
 *      to 41, also known as the "Netcruzer Port Name". Can be "00" to "41", or "0" to "41".
 *
 * @return Returns the "Port ID" of the requested port. Is a number from 0
 *      to 41, also known as the "Netcruzer Port Name". Will return IOPORT_ID_NA
 *      if the given string was could not be associated with an IO Port(port of SBC).
 */
BYTE getPortIdForStr(const char* str);


/**
 * Reads a Port bit with given "port bitAdr". A "Port bitAdr" can be used to identify any of a port's control bits:<br>
 * LAT, ODC (Open Drain), PORT, TRIS, Pullup and Pulldown registers<br>
 * This function is useful if a variable (16-bit WORD) is required to represent a Port bit address.
 * It is done by ORing the SRF's address with it's bit position (for PIC32 add 0x0088 for upper Word).
 * The bit position is stored in the upper nibble (bits 12 to 15).
 * For example, to represent bit 2 of a register with address 0x0C18, use 0x2C18.
 *
 * If the "Port bitAdr" is available as a literal constant, the portReadBitadr_MACRO() macro will generate
 * a single line of code!
 *
 * Many "Port bitAdr" constants have been defined in the "brd_xxx.h" include file for selected board. For example
 * PIN_xx_BITADR and LAT_xx_BITADR defines are available for accessing bits in the PORT and LAT registers.
 * Various portGetXxx() and getUcportXxx() functions are also available for getting the "Port bitAdr" for port
 * TRIS, LAT, PORT, ODC, PULLUP and PULLDOWN registers.
 *
 * @param bitAdr The "Port bitAdr" of the bit to set or clear.
 *
 * @return Returns 0 if requested bit is clear, or 1 if set
 */
BOOL portReadBitadr(WORD bitAdr);


/**
 * @function BOOL portReadBitadr_MACRO(WORD bitAdr)
 *
 * Reads a Port bit with given "port bitAdr". A "Port bitAdr" can be used to identify any of a port's control bits:<br>
 * LAT, ODC (Open Drain), PORT, TRIS, Pullup and Pulldown registers<br>
 * This function is useful if a variable (16-bit WORD) is required to represent a Port bit address.
 * It is done by ORing the SRF's address with it's bit position (for PIC32 add 0x0088 for upper Word).
 * The bit position is stored in the upper nibble (bits 12 to 15).
 * For example, to represent bit 2 of a register with address 0x0C18, use 0x2C18.
 *
 * If the "Port bitAdr" is available as a literal constant, the portReadBitadr_MACRO() macro will generate
 * a single line of code!
 *
 * Many "Port bitAdr" constants have been defined in the "brd_xxx.h" include file for selected board. For example
 * PIN_xx_BITADR and LAT_xx_BITADR defines are available for accessing bits in the PORT and LAT registers.
 * Various portGetXxx() and getUcportXxx() functions are also available for getting the "Port bitAdr" for port
 * TRIS, LAT, PORT, ODC, PULLUP and PULLDOWN registers.
 *
 * @param bitAdr The "Port bitAdr" of the bit to set or clear.
 *
 * @return Returns 0 if requested bit is clear, or 1 if set
 */
#define portReadBitadr_MACRO(bitAdr) (*((volatile WORD*)((bitAdr)&0xFFF)) & (0x01 << ((bitAdr)>>12)))


/**
 * @function void portSetBitadr(WORD bitAdr)
 *
 * Set a Port bit with given "Port bitAdr". A "Port bitAdr" can be used to identify any of a port's control bits:<br>
 * LAT, ODC (Open Drain), PORT, TRIS, Pullup and Pulldown registers<br>
 * This function is useful if a variable (16-bit WORD) is required to represent a Port bit address.
 * It is done by ORing the SRF's address with it's bit position (for PIC32 add 0x0088 for upper Word).
 * The bit position is stored in the upper nibble (bits 12 to 15).
 * For example, to represent bit 2 of a register with address 0x0C18, use 0x2C18.
 *
 * If the "Port bitAdr" is available as a literal constant, the portSetBitadr_MACRO() macro will generate
 * a single line of code!
 *
 * Many "Port bitAdr" constants have been defined in the "brd_xxx.h" include file for selected board. For example
 * PIN_xx_BITADR and LAT_xx_BITADR defines are available for accessing bits in the PORT and LAT registers.
 * Various portGetXxx() and getUcportXxx() functions are also available for getting the "Port bitAdr" for port
 * TRIS, LAT, PORT, ODC, PULLUP and PULLDOWN registers.
 *
 * @param bitAdr The "Port bitAdr" of the bit to set.
 */
#if !defined(DOXY_FUNC)
#if defined(IOPORTS_OPTIMIZE_SIZE)
#define portSetBitadr(bitAdr) portWriteBitadr(bitAdr, 1)
#else
void portSetBitadr(WORD bitAdr);
#endif
#else
void portSetBitadr(WORD bitAdr);
#endif

/**
 * @function void portSetBitadr_MACRO(WORD bitAdr)
 *
 * Set a Port bit with given "Port bitAdr". A "Port bitAdr" can be used to identify any of a port's control bits:<br>
 * LAT, ODC (Open Drain), PORT, TRIS, Pullup and Pulldown registers<br>
 * This function is useful if a variable (16-bit WORD) is required to represent a Port bit address.
 * It is done by ORing the SRF's address with it's bit position (for PIC32 add 0x0088 for upper Word).
 * The bit position is stored in the upper nibble (bits 12 to 15).
 * For example, to represent bit 2 of a register with address 0x0C18, use 0x2C18.
 *
 * If the "Port bitAdr" is available as a literal constant, the portSetBitadr_MACRO() macro will generate
 * a single line of code!
 *
 * Many "Port bitAdr" constants have been defined in the "brd_xxx.h" include file for selected board. For example
 * PIN_xx_BITADR and LAT_xx_BITADR defines are available for accessing bits in the PORT and LAT registers.
 * Various portGetXxx() and getUcportXxx() functions are also available for getting the "Port bitAdr" for port
 * TRIS, LAT, PORT, ODC, PULLUP and PULLDOWN registers.
 *
 * @param bitAdr The "Port bitAdr" of the bit to set.
 */
#define portSetBitadr_MACRO(bitAdr) (*((volatile WORD*)((bitAdr)&0xFFF)) |= (0x01 << ((bitAdr)>>12)))


/**
 * @function void portClearBitadr(WORD bitAdr)
 *
 * Set a Port bit with given "Port bitAdr". A "Port bitAdr" can be used to identify any of a port's control bits:<br>
 * LAT, ODC (Open Drain), PORT, TRIS, Pullup and Pulldown registers<br>
 * This function is useful if a variable (16-bit WORD) is required to represent a Port bit address.
 * It is done by ORing the SRF's address with it's bit position (for PIC32 add 0x0088 for upper Word).
 * The bit position is stored in the upper nibble (bits 12 to 15).
 * For example, to represent bit 2 of a register with address 0x0C18, use 0x2C18.
 *
 * If the "Port bitAdr" is available as a literal constant, the portClearBitadr_MACRO() macro will generate
 * a single line of code!
 *
 * Many "Port bitAdr" constants have been defined in the "brd_xxx.h" include file for selected board. For example
 * PIN_xx_BITADR and LAT_xx_BITADR defines are available for accessing bits in the PORT and LAT registers.
 * Various portGetXxx() and getUcportXxx() functions are also available for getting the "Port bitAdr" for port
 * TRIS, LAT, PORT, ODC, PULLUP and PULLDOWN registers.
 *
 * @param bitAdr The "Port bitAdr" of the bit to clear.
 */
#if !defined(DOXY_FUNC)
#if defined(IOPORTS_OPTIMIZE_SIZE)
#define portClearBitadr(bitAdr) portWriteBitadr(bitAdr, 0)
#else
void portClearBitadr(WORD bitAdr);
#endif
#else
void portClearBitadr(WORD bitAdr);
#endif

/**
* @function void portClearBitadr_MACRO(WORD bitAdr)
 *
 * Set a Port bit with given "Port bitAdr". A "Port bitAdr" can be used to identify any of a port's control bits:<br>
 * LAT, ODC (Open Drain), PORT, TRIS, Pullup and Pulldown registers<br>
 * This function is useful if a variable (16-bit WORD) is required to represent a Port bit address.
 * It is done by ORing the SRF's address with it's bit position (for PIC32 add 0x0088 for upper Word).
 * The bit position is stored in the upper nibble (bits 12 to 15).
 * For example, to represent bit 2 of a register with address 0x0C18, use 0x2C18.
 *
 * If the "Port bitAdr" is available as a literal constant, the portClearBitadr_MACRO() macro will generate
 * a single line of code!
 *
 * Many "Port bitAdr" constants have been defined in the "brd_xxx.h" include file for selected board. For example
 * PIN_xx_BITADR and LAT_xx_BITADR defines are available for accessing bits in the PORT and LAT registers.
 * Various portGetXxx() and getUcportXxx() functions are also available for getting the "Port bitAdr" for port
 * TRIS, LAT, PORT, ODC, PULLUP and PULLDOWN registers.
 *
 * @param bitAdr The "Port bitAdr" of the bit to clear.
 */
#define portClearBitadr_MACRO(bitAdr) (*((volatile WORD*)((bitAdr)&0xFFF)) &= (~(0x01 << ((bitAdr)>>12))))


/**
 * Set a Port bit with given "port bitAdr". A "Port bitAdr" can be used to identify any of a port's control bits:<br>
 * LAT, ODC (Open Drain), PORT, TRIS, Pullup and Pulldown registers<br>
 * This function is useful if a variable (16-bit WORD) is required to represent a Port bit address.
 * It is done by ORing the SRF's address with it's bit position (for PIC32 add 0x0088 for upper Word).
 * The bit position is stored in the upper nibble (bits 12 to 15).
 * For example, to represent bit 2 of a register with address 0x0C18, use 0x2C18.
 *
 * If the "Port bitAdr" is available as a literal constant, the portWriteBitadr_MACRO() macro will generate
 * a single line of code!
 *
 * Many "Port bitAdr" constants have been defined in the "brd_xxx.h" include file for selected board. For example
 * PIN_xx_BITADR and LAT_xx_BITADR defines are available for accessing bits in the PORT and LAT registers.
 * Various portGetXxx() and getUcportXxx() functions are also available for getting the "Port bitAdr" for port
 * TRIS, LAT, PORT, ODC, PULLUP and PULLDOWN registers.
 *
 * @param bitAdr The "Port bitAdr" of the bit to set or clear.
 *
 * @param value 0 will clear given port bit, 1 will set it
 */
void portWriteBitadr(WORD bitAdr, BOOL value);


/**
 * @function void portWriteBitadr_MACRO(WORD bitAdr, BOOL value);
 *
 * Set a Port bit with given "port bitAdr". A "Port bitAdr" can be used to identify any of a port's control bits:<br>
 * LAT, ODC (Open Drain), PORT, TRIS, Pullup and Pulldown registers<br>
 * This function is useful if a variable (16-bit WORD) is required to represent a Port bit address.
 * It is done by ORing the SRF's address with it's bit position (for PIC32 add 0x0088 for upper Word).
 * The bit position is stored in the upper nibble (bits 12 to 15).
 * For example, to represent bit 2 of a register with address 0x0C18, use 0x2C18.
 *
 * If the "Port bitAdr" is available as a literal constant, the portWriteBitadr_MACRO() macro will generate
 * a single line of code!
 *
 * Many "Port bitAdr" constants have been defined in the "brd_xxx.h" include file for selected board. For example
 * PIN_xx_BITADR and LAT_xx_BITADR defines are available for accessing bits in the PORT and LAT registers.
 * Various portGetXxx() and getUcportXxx() functions are also available for getting the "Port bitAdr" for port
 * TRIS, LAT, PORT, ODC, PULLUP and PULLDOWN registers.
 *
 * @param bitAdr The "Port bitAdr" of the bit to set or clear.
 *
 * @param value 0 will clear given port bit, 1 will set it
 */
#define portWriteBitadr_MACRO(bitAdr, value) ((value==0) ? (*((volatile WORD*)((bitAdr)&0xFFF)) &= (~(0x01 << ((bitAdr)>>12)))) : (*((volatile WORD*)((bitAdr)&0xFFF)) |= (0x01 << ((bitAdr)>>12))))


/**
 * @function WORD portGetLAT(BYTE portID)
 *
 * Gets a "Port bitAdr" for the requested port's LAT register's.
 * The port is identified by it's "Port ID".
 * The returned "Port bitAdr" can be used to set or clear the port using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * The "Port bitAdr" for all ports are also defined by the LAT_xx_BITADR defines! The following two lines
 * will both return the requested port's "Port bitAdr" value:
 * @code
 * WORD portX0;
 * WORD portX1;
 *
 * portX0 = LAT_00_BITADR;
 * portX1 = portGetLAT(1);
 * @endcode
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns a "Port bitAdr" for the requested port's LAT register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the LAT register is 4 up from TRIS register
    #define portGetLAT(portID) (portGetTRISwOffset(portID, 4))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the LAT register is 0x30 up from TRIS register
    #define portGetLAT(portID) (portGetTRISwOffset(portID, 0x30))
#else
    #error "portGetLAT() not defined for this compiler!"
#endif
#else
WORD portGetLAT(BYTE portID);
#endif


/**
 * WORD ucportGetLAT(BYTE ucportID)
 *
 * Gets a "Port bitAdr" for the requested port's LAT register's. The port is identified by it's
 * "Microcontroller Port ID", is a UCPORT_ID_xx define (UCPORT_ID_01 for example).
 * The returned "Port bitAdr" can be used to set or clear the port using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @code
 * WORD portB1;
 *
 * portB1 = ucportGetLAT(UCPORT_ID_B1);
 * @endcode
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 *
 * @return Returns a "Port bitAdr" for the requested port's LAT register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the LAT register is 4 up from TRIS register
    #define ucportGetLAT(ucportID) (ucportGetTRISwOffset(ucportID, 4))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the LAT register is 0x30 up from TRIS register
    #define ucportGetLAT(ucportID) (ucportGetTRISwOffset(ucportID, 0x30))
#else
    #error "ucportGetLAT() not defined for this compiler!"
#endif
#else
WORD ucportGetLAT(BYTE ucportID);
#endif


/**
 * WORD portGetPIN(BYTE portID)
 * 
 * Gets a "Port bitAdr" for the requested port's PORT register's. The port is identified by it's "Port ID".
 * The returned "Port bitAdr" can be used to read or write the digital pin value of the port.
 * The portWriteBitadr(), portSetBitadr() and portClearBitadr() functions can be used.
 *
 * IMPORTANT! When writing a port pin, the LAT registers (port output latch) should be used in stead!
 * Using the PORT register could fail if the port is driving low impedance loads (read-write effect)!
 *
 * The "Port bitAdr" for all ports are also defined by the PIN_xx_BITADR defines! The following two lines
 * will both return the requested port's "Port bitAdr" value:
 * @code
 * WORD pinX0;
 * WORD pinX1;
 *
 * pinX0 = PIN_00_BITADR;
 * pinX1 = portGetPIN(1);
 * @endcode
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns a "Port bitAdr" for the requested port's PORT register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the PORT register is 2 up from TRIS register
    #define portGetPIN(portID) (portGetTRISwOffset(portID, 2))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the PORT register is 0x20 up from TRIS register
    #define portGetPIN(portID) (portGetTRISwOffset(portID, 0x20))
#else
    #error "portGetPIN() not defined for this compiler!"
#endif
#else
WORD portGetPIN(BYTE portID);
#endif


/**
 * WORD ucportGetPIN(BYTE ucportID)
 *
 * Gets a "Port bitAdr" for the requested port's PORT register's. The port is identified by it's
 * "Microcontroller Port ID", is a UCPORT_ID_xx define (UCPORT_ID_01 for example).
 * The returned "Port bitAdr" can be used to read or write the digital pin value of the port.
 * The portWriteBitadr(), portSetBitadr() and portClearBitadr() functions can be used.
 *
 * IMPORTANT! When writing a port pin, the LAT registers (port output latch) should be used in stead!
 * Using the PORT register could fail if the port is driving low impedance loads (read-write effect)!
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 *
 * @return Returns a "Port bitAdr" for the requested port's PORT register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the PORT register is 2 up from TRIS register
    #define ucportGetPIN(ucportID) (ucportGetTRISwOffset(ucportID, 2))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the PORT register is 0x20 up from TRIS register
    #define ucportGetPIN(ucportID) (ucportGetTRISwOffset(ucportID, 0x20))
#else
    #error "ucportGetPIN() not defined for this compiler!"
#endif
#else
WORD ucportGetPIN(BYTE ucportID);
#endif



/**
 * @function WORD portGetODC(BYTE portID)
 *
 * Gets a "Port bitAdr" for the requested port's ODC (Open Drain Control) register's.
 * The port is identified by it's "Port ID".
 * The returned "Port bitAdr" can be used to enable or disable the Open Collector feature
 * of the port using the portWriteBitadr(), portSetBitadr() and portClearBitadr() functions.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns a "Port bitAdr" for the requested port's ODC (Open Drain Control) register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the ODC register is 6 up from TRIS register
    #define portGetODC(portID) (portGetTRISwOffset(portID, 6))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the ODC register is 0x40 up from TRIS register
    #define portGetODC(portID) (portGetTRISwOffset(portID, 0x40))
#else
    #error "portGetODC() not defined for this compiler!"
#endif
#else
WORD portGetODC(BYTE portID);
#endif


/**
 * @function WORD ucportGetODC(BYTE ucportID)
 *
 * Gets a "Port bitAdr" for the requested port's ODC (Open Drain Control) register's.
 * The port is identified by it's "Microcontroller Port ID", is a UCPORT_ID_xx define
 * (UCPORT_ID_01 for example).
 * The returned "Port bitAdr" can be used to enable or disable the Open Collector feature
 * of the port using the portWriteBitadr(), portSetBitadr() and portClearBitadr() functions.
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 *
 * @return Returns a "Port bitAdr" for the requested port's ODC (Open Drain Control) register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the ODC (Open Drain Control) register is 6 up from TRIS register
    #define ucportGetODC(ucportID) (ucportGetTRISwOffset(ucportID, 6))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the ODC (Open Drain Control) register is 0x40 up from
    //the TRIS register
    #define ucportGetODC(ucportID) (ucportGetTRISwOffset(ucportID, 0x40))
#else
    #error "ucportGetODC() not defined for this compiler!"
#endif
#else
WORD ucportGetODC(BYTE ucportID);
#endif


/**
 * @function WORD portGetTRIS(BYTE portID)
 *
 * Gets a "Port bitAdr" for the requested port's TRIS register's. The port is identified by it's "Port ID".
 * The returned "Port bitAdr" can be used to set or clear the port using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns a "Port bitAdr" for the requested port's TRIS register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(IOPORTS_OPTIMIZE_SIZE)
#define portGetTRIS(portID) portGetTRISwOffset(portID, 0)
#else
WORD portGetTRIS(BYTE portID);
#endif
#else
WORD portGetTRIS(BYTE portID);
#endif


/**
 * @function WORD portGetTRISwOffset(BYTE portID)
 *
 * !!!!!!! IMPORTANT !!!!!!!!!
 * This functions if used by the system, is not meant for general purpose use!
 *
 * Gets a "Port bitAdr" for the requested port's TRIS register's, adding the given offset
 * to it.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 * @param offset An offset to add to returned value if it is NOT 0.
 *
 * @return Returns a "Port bitAdr" for the requested port's TRIS register.
 *         Returns BITADR_NA if not available, or error.
 */
WORD portGetTRISwOffset(BYTE portID, BYTE offset);


/**
 * @function WORD portGetTRIS(BYTE portID)
 *
 * Gets a "Port bitAdr" for the requested port's TRIS register's. The port is identified by it's "Port ID".
 * The returned "Port bitAdr" can be used to set or clear the port using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns a "Port bitAdr" for the requested port's TRIS register.
 *         Returns BITADR_NA if not available, or error.
 */
#define portGetTRIS_MACRO(portID) ((portID>PORT_ID_MAX) ? (BITADR_NA/*Returns BITADR_NA if not available or error*/) : TRIS_PORTID_BITADR_MAP[portID])


/**
 * Gets a "Port bitAdr" for the requested port's TRIS register's. The port is identified by it's
 * "Microcontroller Port ID", is a UCPORT_ID_xx define (UCPORT_ID_01 for example).
 * The returned "Port bitAdr" can be used to set or clear the port using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 *
 * @return Returns a "Port bitAdr" for the requested port's TRIS register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(IOPORTS_OPTIMIZE_SIZE)
#define ucportGetTRIS(ucportID) ucportGetTRISwOffset(ucportID, 0)
#else
WORD ucportGetTRIS(BYTE ucportID);
#endif
#else
WORD ucportGetTRIS(BYTE ucportID);
#endif



/**
 * !!!!!!! IMPORTANT !!!!!!!!!
 * This functions if used by the system, is not meant for general purpose use!

 * Gets a "Port bitAdr" for the requested port's TRIS register's, adding the given
 * offset to it.
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 * @param offset An offset to add to returned value if it is NOT 0.
 *
 * @return Returns a "Port bitAdr" for the requested port's TRIS register.
 *         Returns BITADR_NA if not available, or error.
 */
WORD ucportGetTRISwOffset(BYTE ucportID, BYTE offset);


/**
 * @function WORD portGetCNPD(BYTE portID)
 *
 * Gets a "Port bitAdr" for the requested port's CNPD (Pull-Down) register. The port
 * is identified by it's "Port ID". The returned "Port bitAdr" can be used to
 * enable or disable the port's Pull-Down resistor using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns a "Port bitAdr" for the requested port's CNPD (Pull-Down) register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(IOPORTS_OPTIMIZE_SIZE)
#define portGetCNPD(portID) portGetCNPDwOffset(portID, 0)
#else
WORD portGetCNPD(BYTE portID);
#endif
#else
WORD portGetCNPD(BYTE portID);
#endif


/**
 * @function WORD portGetCNPDwOffset(BYTE portID, BYTE offset)
 *
 * !!!!!!! IMPORTANT !!!!!!!!!
 * This functions if used by the system, is not meant for general purpose use!
 *
 * Gets a "Port bitAdr" for the requested port's CNPD (Pull-Down) register, adding
 * the given offset to it.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 * @param offset An offset to add to returned value if it is NOT 0.
 *
 * @return Returns a "Port bitAdr" for the requested port's CNPD (Pull-Down) register.
 *         Returns BITADR_NA if not available, or error.
 */
WORD portGetCNPDwOffset(BYTE portID, BYTE offset);


/**
 * Gets a "Port bitAdr" for the requested port's CNPD (Pull-Down) register. The port
 * is identified by it's "Microcontroller Port ID", is a UCPORT_ID_xx define
 * (UCPORT_ID_01 for example). The returned "Port bitAdr" can be used to
 * enable or disable the port's Pull-Down resistor using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 *
 * @return Returns a "Port bitAdr" for the requested port's CNPD (Pull-Down) register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(IOPORTS_OPTIMIZE_SIZE)
#define ucportGetCNPD(ucportID) ucportGetCNPDwOffset(ucportID, 0)
#else
WORD ucportGetCNPD(BYTE ucportID);
#endif
#else
WORD ucportGetCNPD(BYTE ucportID);
#endif


/**
 * !!!!!!! IMPORTANT !!!!!!!!!
 * This functions if used by the system, is not meant for general purpose use!
 *
 * Gets a "Port bitAdr" for the requested port's CNPD (Pull-Down) register, adding
 * the given offset to it.
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 * @param offset An offset to add to returned value if it is NOT 0.
 *
 * @return Returns a "Port bitAdr" for the requested port's CNPD (Pull-Down) register.
 *         Returns BITADR_NA if not available, or error.
 */
WORD ucportGetCNPDwOffset(BYTE ucportID, BYTE offset);


/**
 * @function WORD ucportGetCNPU(BYTE ucportID)
 *
 * Gets a "Port bitAdr" for the requested port's CNPU (Pull-Up) register. The port
 * is identified by it's "Microcontroller Port ID", is a UCPORT_ID_xx define
 * (UCPORT_ID_01 for example). The returned "Port bitAdr" can be used to
 * enable or disable the port's Pull-Up resistor using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 *
 * @return Returns a "Port bitAdr" for the requested port's CNPU (Pull-Up) register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the CNPU (Pull-Up) register is 0x18 up from CNPD register
    #define ucportGetCNPU(ucportID) (ucportGetCNPDwOffset(ucportID, 0x18))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the CNPU (Pull-Up Control) register is 0x10 down from
    //the CNPD register
    #define ucportGetCNPU(ucportID) (ucportGetCNPDwOffset(ucportID, -0x10))
#else
    #error "ucportGetCNPU() not defined for this compiler!"
#endif
#else
WORD ucportGetCNPU(BYTE ucportID);
#endif


/**
 * @function WORD portGetCNIE(BYTE portID)
 *
 * Gets a "Port bitAdr" for the requested port's CNIE (CN Interrupt Enable) register.
 * The port is identified by it's "Port ID". The returned "Port bitAdr" can be used to
 * enable or disable the port's CN interrupts using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns a "Port bitAdr" for the requested port's CNIE (CN Interrupt Enable) register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the CNPU (Pull-Up) register is 0x18 up from CNPD register
    #define portGetCNPU(portID) (portGetCNPDwOffset(portID, 0x18))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the CNPU (Pull-Up Control) register is 0x10 down from
    //the CNPD register
    #define portGetCNPU(portID) (portGetCNPDwOffset(portID, -0x10))
#else
    #error "portGetCNPU() not defined for this compiler!"
#endif
#else
WORD portGetCNIE(BYTE portID);
#endif


/**
 * @function WORD ucportGetCNIE(BYTE ucportID)
 *
 * Gets a "Port bitAdr" for the requested port's CNIE (CN Interrupt Enable) register.
 * The port is identified by it's "Microcontroller Port ID", is a UCPORT_ID_xx define
 * (UCPORT_ID_01 for example). The returned "Port bitAdr" can be used to
 * enable or disable the port's CN interrupts using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 *
 * @return Returns a "Port bitAdr" for the requested port's CNIE (CN Interrupt Enable) register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the CNIE (CN Interrupt Enable) register is 0x0c up from CNPD register
    #define ucportGetCNIE(ucportID) (ucportGetCNPDwOffset(ucportID, 0x0c))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the CNIE (CN Interrupt Enable) register is 0x20 up from
    //the CNPD register
    #define ucportGetCNIE(ucportID) (ucportGetCNPDwOffset(ucportID, 0x20))
#else
    #error "ucportGetCNIE() not defined for this compiler!"
#endif
#else
WORD ucportGetCNIE(BYTE ucportID);
#endif


/**
 * @function WORD portGetCNIE(BYTE portID)
 *
 * Gets a "Port bitAdr" for the requested port's CNIE (CN Interrupt Enable) register.
 * The port is identified by it's "Port ID". The returned "Port bitAdr" can be used to
 * enable or disable the port's CN interrupts using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 *         known as the "Netcruzer Port Name".
 *
 * @return Returns a "Port bitAdr" for the requested port's CNIE (CN Interrupt Enable) register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(DOXY_FUNC)
#if defined(__C30__)
    //For PIC24F processors, the CNIE (CN Interrupt Enable) register is 0x0c up from CNPD register
    #define portGetCNIE(portID) (portGetCNPDwOffset(portID, 0x0c))
#elif defined(__C32__)
    //For PIC32MX and PIC32MZ processors, the CNIE (CN Interrupt Enable) register is 0x20 up from
    //the CNPD register
    #define portGetCNIE(portID) (portGetCNPDwOffset(portID, 0x20))
#else
    #error "portGetCNIE() not defined for this compiler!"
#endif
#else
WORD portGetCNIE(BYTE portID);
#endif


/**
 * Enables of disabled the pull-up resistor for the given port
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 *
 * @param val If 0, the pull-up resistor is disabled, else it is enabled.
 */
void portWritePullup(BYTE portID, BOOL val);


/**
 * Enables or disabled the pull-down resistor for the given port
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 *
 * @param val If 0, the pull-down resistor is disabled, else it is enabled.
 */
void portWritePulldown(BYTE portID, BOOL val);


/**
 * Set direction of Digital port.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 * @param dir Direction of port, 0 sets port as an output, 1 as an input.
 */
void portConfigDir(BYTE portID, BOOL dir);


/**
 * Write a Digital port.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 *
 * @param value HIGH will set the port(3.3V), and LOW will clear(0V) the port pin.
 */
void portWrite(BYTE portID, BYTE value);


/**
 * Read a Digital port.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 *
 * @return Returns the value of the requested digitial port, will be 0 (LOW = 0V) if not set,
 *      or 1 (HIGH = 3.3V) if set.
 */
BOOL portRead(BYTE portID);



/////////////////////////////////////////////////
//Arduino compatible functions
//All the following functions use the "Microcontroller port ID"(a UCPORT_ID_xx define)
//to identify a port. It is not as fast as using functions that take a "port bitAdr".
//But, all functions can use the same "Microcontroller port ID", where the "port bitAdr" is
//different for each function (LAT, TRIS, PORT...).
//To get the "Microcontroller port ID":
// - If the "Port ID" is know, the portGetUcportID() function will return the "port ID"
// - The "port ID" for each IO port (port of SBC) is defined in the "brd_xxx.h" include file
//   for selected board, like UCPORT_ID_28 for example
#if !defined(HIGH)
#define HIGH 1
#endif
#if !defined(LOW)
#define LOW 0
#endif


/**
 * @function void digitalWrite(BYTE portID, BYTE value)
 *
 * Write a Digital port.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 *
 * @param value HIGH will set the port(3.3V), and LOW will clear(0V) the port pin.
 */
#if !defined(DOXY_FUNC)
#define digitalWrite(portID, value) portWrite(portID, value)
#else
void digitalWrite(BYTE portID, BYTE value);
#endif


/**
 * @function BOOL digitalRead(BYTE portID)
 *
 * Read a Digital port.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 *
 * @return Returns the value of the requested digitial port, will be 0 (LOW = 0V) if not set,
 *      or 1 (HIGH = 3.3V) if set.
 */
#if !defined(DOXY_FUNC)
#define digitalRead(portID) portRead(portID)
#else
BOOL digitalRead(BYTE portID);
#endif



#endif

