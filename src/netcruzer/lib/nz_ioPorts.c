/**
 * @brief           Configures all I/O ports
 * @file            nz_ioPorts.h
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
#define THIS_IS_NZ_IO_PORTS_C

#include "HardwareProfile.h"

#include <ctype.h>  // for tolower
#include <stdlib.h> // for atoi

#include "nz_ioPorts.h"

//Add debugging to this file. The DEBUG_CONF_IOPORTS macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_IOPORTS)
    #define DEBUG_CONF_IOPORTS      DEBUG_LEVEL_ERROR  //Default Debugging level if not defined = ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_IOPORTS
#include "nz_debug.h"

#define WORD_BITS(D,C,B,A) ((D<<12) | (C<<8) | (B<<4) | A)


/**
 * Map the TRIS "Port bitAdr" to "Port ID" (number from 0 to 41, also known as
 * the "Netcruzer Port Name").
 *
 * This array make code like the following possible:
 * portSetBitadr(TRIS_PORTID_BITADR_MAP[8]);
 * The compiler generates very optimized code for the instruction above!
 */
#if defined(__C30__)
const WORD TRIS_PORTID_BITADR_MAP[] = {
    TRIS_00_BITADR,
    TRIS_01_BITADR,
    TRIS_02_BITADR,
    TRIS_03_BITADR,
    TRIS_04_BITADR,
    TRIS_05_BITADR,
    TRIS_06_BITADR,
    TRIS_07_BITADR,
    TRIS_08_BITADR,
    TRIS_09_BITADR,
    TRIS_10_BITADR,
    TRIS_11_BITADR,
    TRIS_12_BITADR,
    TRIS_13_BITADR,
    TRIS_14_BITADR,
    TRIS_15_BITADR,
    TRIS_16_BITADR,
    TRIS_17_BITADR,
    TRIS_18_BITADR,
    TRIS_19_BITADR,
    TRIS_20_BITADR,
    TRIS_21_BITADR,
    TRIS_22_BITADR,
    TRIS_23_BITADR,
    TRIS_24_BITADR,
    TRIS_25_BITADR,
    TRIS_26_BITADR,
    TRIS_27_BITADR,
    TRIS_28_BITADR,
    TRIS_29_BITADR,
    TRIS_30_BITADR,
    TRIS_31_BITADR,
    TRIS_32_BITADR,
    TRIS_33_BITADR,
    TRIS_34_BITADR,
    TRIS_35_BITADR,
    TRIS_36_BITADR,
    TRIS_37_BITADR,
    TRIS_38_BITADR,
    TRIS_39_BITADR,
    TRIS_40_BITADR,
    TRIS_41_BITADR
};
#elif defined(__C32__)
const WORD TRIS_PORTID_BITADR_MAP[] = {
    TRIS_00_BITADR,
    TRIS_01_BITADR,
    TRIS_02_BITADR,
    TRIS_03_BITADR,
    TRIS_04_BITADR,
    TRIS_05_BITADR,
    TRIS_06_BITADR,
    TRIS_07_BITADR,
    TRIS_08_BITADR,
    TRIS_09_BITADR,
    TRIS_10_BITADR,
    TRIS_11_BITADR,
    TRIS_12_BITADR,
    TRIS_13_BITADR,
    TRIS_14_BITADR,
    TRIS_15_BITADR,
    TRIS_16_BITADR,
    TRIS_17_BITADR,
    TRIS_18_BITADR,
    TRIS_19_BITADR,
    TRIS_20_BITADR,
    TRIS_21_BITADR,
    TRIS_22_BITADR,
    TRIS_23_BITADR,
    TRIS_24_BITADR,
    TRIS_25_BITADR,
    TRIS_26_BITADR,
    TRIS_27_BITADR,
    TRIS_28_BITADR,
    TRIS_29_BITADR,
    TRIS_30_BITADR,
    TRIS_31_BITADR,
    TRIS_32_BITADR,
    TRIS_33_BITADR,
    TRIS_34_BITADR,
    TRIS_35_BITADR,
    TRIS_36_BITADR,
    TRIS_37_BITADR,
    TRIS_38_BITADR,
    TRIS_39_BITADR,
    TRIS_40_BITADR,
    TRIS_41_BITADR
};
#endif


/**
 * Map PORT_ID_xxx to UCPORT_ID_xxx
 */
const BYTE PORTID_UCPORT_MAP[] = {
    UCPORT_ID_00,
    UCPORT_ID_01,
    UCPORT_ID_02,
    UCPORT_ID_03,
    UCPORT_ID_04,
    UCPORT_ID_05,
    UCPORT_ID_06,
    UCPORT_ID_07,
    UCPORT_ID_08,
    UCPORT_ID_09,
    UCPORT_ID_10,
    UCPORT_ID_11,
    UCPORT_ID_12,
    UCPORT_ID_13,
    UCPORT_ID_14,
    UCPORT_ID_15,
    UCPORT_ID_16,
    UCPORT_ID_17,
    UCPORT_ID_18,
    UCPORT_ID_19,
    UCPORT_ID_20,
    UCPORT_ID_21,
    UCPORT_ID_22,
    UCPORT_ID_23,
    UCPORT_ID_24,
    UCPORT_ID_25,
    UCPORT_ID_26,
    UCPORT_ID_27,
    UCPORT_ID_28,
    UCPORT_ID_29,
    UCPORT_ID_30,
    UCPORT_ID_31,
    UCPORT_ID_32,
    UCPORT_ID_33,
    UCPORT_ID_34,
    UCPORT_ID_35,
    UCPORT_ID_36,
    UCPORT_ID_37,
    UCPORT_ID_38,
    UCPORT_ID_39,
    UCPORT_ID_40,
    UCPORT_ID_41
};

#define HAS_PORTID_UC66PORT_OFFSET_MAP

#if defined(HAS_PORTID_UC66PORT_OFFSET_MAP)
/**
 * Map giving index of elements(structures) in CFG_BLOCK_UC66_PORT and TABLE_BLOCK_UC66_PORT
 * for "Port ID" (PORT_ID_xxx defines).
 * Offset of elements must be exactly the same as PORT_ID_xx values! For example,
 * 28 = 10, so the 10th element in this structure must be for P20!
 *
 * This map is used to get the entry in the CFG_BLOCK_UC66_PORT and TABLE_BLOCK_UC66_PORT
 * structures for a given "Port ID".
 */
const BYTE PORTID_UC66PORT_OFFSET_MAP[] = {
#ifdef UCPORT_00
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_00) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_01
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_01) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_02
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_02) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_03
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_03) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_04
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_04) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_05
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_05) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_06
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_06) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_07
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_07) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_08
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_08) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_09
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_09) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_10
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_10) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_11
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_11) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_12
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_12) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_13
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_13) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_14
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_14) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_15
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_15) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_16
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_16) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_17
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_17) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_18
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_18) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_19
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_19) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_20
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_20) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_21
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_21) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_22
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_22) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_23
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_23) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_24
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_24) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_25
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_25) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_26
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_26) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_27
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_27) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_28
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_28) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_29
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_29) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_30
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_30) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_31
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_31) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_32
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_32) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_33
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_33) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_34
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_34) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_35
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_35) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_36
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_36) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_37
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_37) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_38
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_38) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_39
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_39) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_40
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_40) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
#ifdef UCPORT_41
    offsetof(CFG_BLOCK_UC66_PORT, UCPORT_41) / sizeof(UCPORT_CONFIG),
#else
    0xff,
#endif
};
#endif  //#if defined(HAS_PORTID_UC66PORT_OFFSET_MAP)


/**
 * Map giving ICN number in PIC internal registers, mapped to UCPORT_ID_xxx.
 * Start UCPORT_ID at UCPORT_ID_B0, seeing that Port A is not used
 */
const BYTE ICN_UCPORT_PIC24F_MAP[] = {
    2,      //B0
    3,      //B1
    4,      //B2
    5,      //B3
    6,      //B4
    7,      //B5
    24,     //B6
    25,     //B7
    26,     //B8
    27,     //B9
    28,     //B10
    29,     //B11
    30,     //B12
    31,     //B13
    32,     //B14
    12,     //B15
    0xff,   //C0 - Not available
    0xff,   //C1 - Not available
    0xff,   //C2 - Not available
    0xff,   //C3 - Not available
    0xff,   //C4 - Not available
    0xff,   //C5 - Not available
    0xff,   //C6 - Not available
    0xff,   //C7 - Not available
    0xff,   //C8 - Not available
    0xff,   //C9 - Not available
    0xff,   //C10 - Not available
    0xff,   //C11 - Not available
    23,     //C12
    1,      //C13
    0,      //C14
    22,     //C15
    49,     //D0
    50,     //D1
    51,     //D2
    52,     //D3
    13,     //D4
    14,     //D5
    15,     //D6
    16,     //D7
    0xff,   //D8 - Not available
    0xff,   //D9 - Not available
    0xff,   //D10 - Not available
    0xff,   //D11 - Not available
    0xff,   //D12 - Not available
    0xff,   //D13 - Not available
    0xff,   //D14 - Not available
    0xff,   //D15 - Not available
    58,     //E0
    59,     //E1
    60,     //E2
    61,     //E3
    62,     //E4
    63,     //E5
    64,     //E6
    65,     //E7
    0xff,   //E8 - Not available
    0xff,   //E9 - Not available
    0xff,   //E10 - Not available
    0xff,   //E11 - Not available
    0xff,   //E12 - Not available
    0xff,   //E13 - Not available
    0xff,   //E14 - Not available
    0xff,   //E15 - Not available
    68,     //F0
    69,     //F1
    0xff,   //F2 - Not available
    71,     //F3
    17,     //F4
    18,     //F5
    0xff,   //F6 - Not available
    0xff,   //F7 - Not available
    0xff,   //F8 - Not available
    0xff,   //F9 - Not available
    0xff,   //F10 - Not available
    0xff,   //F11 - Not available
    0xff,   //F12 - Not available
    0xff,   //F13 - Not available
    0xff,   //F14 - Not available
    0xff,   //F15 - Not available
    0xff,   //G0 - Not available
    0xff,   //G1 - Not available
    83,     //G2 - Not available
    84,     //G3 - Not available
    0xff,   //G4 - Not available
    0xff,   //G5 - Not available
    8,      //G6 - Not available
    9,      //G7 - Not available
    10,     //G8 - Not available
    11,     //G9 - Not available
};

#define ICN_UCPORT_PIC24F_MAP_STARTOFFSET UCPORT_ID_B0
#define ICN_UCPORT_PIC24F_MAP_LASTENTRY UCPORT_ID_G9


/**
 * Initialization
 */
void portInit(void) {

}


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
BYTE portConfig(BYTE portID, WORD typeConf) {
    WORD bitadrTris;

    #define type ((BYTE)((typeConf>>8)&0xff))
    #define conf ((BYTE)(typeConf&0xff))

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nportConfig ");
    DEBUG_PUT_BYTE(DEBUG_LEVEL_INFO, portID);
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, ": ");

    if ((bitadrTris=portGetTRIS(portID)) == BITADR_NA) {
        //ERROR
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nportConfig() Inv portID!");
        return 0xff;
    }

    if (type == PCFG_DIN_TYPE) {
    //if (type == WORD_HIGH_BYTE(PTYPE_DIN)) {  //Alternative - generates same code
    //if ((typeConf&0xff00) == PTYPE_DIN) {     //Alternative - generates same code

        //Set TRIS register = input
        portSetBitadr(bitadrTris);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-in");

        if ((conf&PCFG_IN_FL_PULLUP) != 0) {
            //Enable Pull-Up resistor, disable Pull-down resistor
            portSetBitadr(portGetCNPU(portID));
            portClearBitadr(portGetCNPD(portID));
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-Pullup");
        }
        else if ((conf&PCFG_IN_FL_PULLDOWN) != 0) {
            //Enable Pull-Down resistor, disable Pull-up resistor
            portSetBitadr(portGetCNPD(portID));
            portClearBitadr(portGetCNPU(portID));
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-Pulldwn");
        }
    }
    else if (type == PCFG_DOUT_TYPE) {
        //Clear TRIS register = output
        portClearBitadr(bitadrTris);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-out");

        if ((conf&PCFG_OUT_FL_OPEN_COLL) != 0) {
            //Enable Open Collector
            portSetBitadr(portGetODC(portID));
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-Opencol");
        }
        else {
            //Disable Open Collector
            portClearBitadr(portGetODC(portID));
        }
    }
    else if (type == PCFG_PWM_TYPE) {
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-pwm");

    }
    else if (type == PCFG_ADC_TYPE) {
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-adc");
    }
    else {
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "-UNDEF");
    }

    return 0;   //OK
}


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
BYTE ucportGetIndex(BYTE portID) {
    //Can use the following methods to get the index:
    // - Use the PORTID_UC66PORT_OFFSET_MAP[] map
    // - Get the IOPORT_PROP structure (in TABLE_BLOCK_SBC66_PORT = cfgStruct.ioPortProp).
    //   The ucPortIndex element of this structure contains the index.

    //Use PORTID_UCPORT_MAP to calculate requested "Port bitAdr"
    if (portID > PORT_ID_MAX) {
        //Don't print error if input was IOPORT_ID_NA
        if (portID != IOPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: ucportGetIndex() invalid ID!");
        }
        return UCPORT_INDEX_NA;
    }

#if defined(HAS_PORTID_UC66PORT_OFFSET_MAP)
    if (sizeof(PORTID_UC66PORT_OFFSET_MAP) > portID) {
        //If PORTID_UC66PORT_OFFSET_MAP has been defined, use it!
        return PORTID_UC66PORT_OFFSET_MAP[portID];
    }
#else
    //To save space, PORTID_UC66PORT_OFFSET_MAP has not been defined. Have to use
    //slower method. Get the IOPORT_PROP structure (in TABLE_BLOCK_SBC66_PORT = cfgStruct.ioPortProp).
    //The ucPortIndex element of this structure contains the index that PORTID_UC66PORT_OFFSET_MAP
    //would have contained.

#endif

    return UCPORT_INDEX_NA;
}


/**
 * Gets the "Port ID" (a number from 0 to 41, also known as the "Netcruzer Port Name")
 * for the given string. String must contain 1 or 2 digits. For numbers less then 10, can
 * have a leading 0, or not.
 *
 * @param str A string representing the "Port ID" of the requested port. Is a number from 0
 *      to 41, also known as the "Netcruzer Port Name". Can be "00" to "41", or "0" to "41".
 *
 * @return Returns the "Port ID" of the requested port. Is a number from 0
 *      to 41, also known as the "Netcruzer Port Name". Will return IOPORT_ID_NA
 *      if the given string could not be associated with a IO Port (port of SBC).
 */
BYTE getPortIdForStr(const char* str) {
    BYTE ret = IOPORT_ID_NA;

    if (isdigit(str[1])) {
        if (isdigit((str[2]))) {
            ret = ((str[1]-'0')*10) + (str[2]-'0');
        }
        else {
            ret = str[1]-'0';
        }

        if (ret > PORT_ID_MAX) {
            ret = IOPORT_ID_NA;
        }
    }

    return ret;
}


/**
 * @function BOOL portReadBitadr(WORD bitAdr)
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
BOOL portReadBitadr(WORD bitAdr) {
    //Do nothing if given "Port bitAdr" is BITADR_NA
    #if defined(IOPORTS_CHECK_BITADR)
    if (bitAdr == BITADR_NA)
        return 0;
    #endif

    return (((*((volatile WORD*)((bitAdr)&0xFFF)) & (0x01 << ((bitAdr)>>12)))==0) ? 0 : 1);
}


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
#if !defined(IOPORTS_OPTIMIZE_SIZE)
void portSetBitadr(WORD bitAdr) {

    //Do nothing if given "Port bitAdr" is BITADR_NA
    #if defined(IOPORTS_CHECK_BITADR)
    if (bitAdr == BITADR_NA)
        return;
    #endif

#if defined(__C30__)
    volatile WORD* p;
    WORD mask = 0x01 << ((bitAdr)>>12);

    p = (volatile WORD*)((bitAdr) & 0x07FF);    //Ensure not larger than 0x7ff = highest SFR!

    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_WARNING)
    if ( (((((WORD)p)>=CNPD1_ADR)&&(((WORD)p)<=CNPU6_ADR)) || ((((WORD)p)>=0x02C0)&&(((WORD)p)<=0x02F6))) == 0 ) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: portSetBitadr() invalid Bitadr!");
        return;
    }
    #endif

    *p = *p | mask;
#elif defined(__C32__)
    //For PIC32 all PORT bytes are located somewhere in 0x00880000 to 0x00886FFF, so upper 20 bits are always 0x88006

    //Port addresses are 32-bit - upper 20-bits of address is 0xBF886
    volatile WORD* p;
    WORD mask = 0x01 << ((bitAdr)>>12);
    p = (volatile WORD*)(0xBF886000 | ((DWORD)((bitAdr) & 0x0FFF)) );

    *p = *p | mask;
#else
    #error "portGetTRIS() not defined for this compiler!"
#endif

}
#endif


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
#if !defined(IOPORTS_OPTIMIZE_SIZE)
void portClearBitadr(WORD bitAdr) {

    //Do nothing if given "Port bitAdr" is BITADR_NA
    //This will NOT cause any error for PIC24F chips. It will cause 0x07FF to be written. Nothing there!
    #if defined(IOPORTS_CHECK_BITADR)
    if (bitAdr == BITADR_NA)
        return;
    #endif

#if defined(__C30__)
    volatile WORD* p;
    WORD mask = 0x01 << ((bitAdr)>>12);
    p = (volatile WORD*)((bitAdr) & 0x07FF);    //Ensure not larger than 0x7ff = highest SFR!

    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_WARNING)
    if (   (((((WORD)p)>=CNPD1_ADR)&&(((WORD)p)<=CNPU6_ADR)) || ((((WORD)p)>=0x02C0)&&(((WORD)p)<=0x02F6))) == 0 ) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: portClearBitadr() invalid Bitadr!");
        DEBUG_PUT_HEXWORD(DEBUG_LEVEL_WARNING, ((WORD)p));
        return;
    }
    #endif

    *p = *p & (~mask);
#elif defined(__C32__)
    //Port addresses are 32-bit - upper 20-bits of address is 0xBF886
    volatile WORD* p;
    WORD mask = 0x01 << ((bitAdr)>>12);
    p = (volatile WORD*)(0xBF886000 | ((DWORD)((bitAdr) & 0x0FFF)) );

    *p = *p & (~mask);
#else
    #error "portGetTRIS() not defined for this compiler!"
#endif

}
#endif


/**
 * @function void portWriteBitadr(WORD bitAdr, BOOL value)
 *
 * Set a Port bit with given "Port bitAdr". A "Port bitAdr" can be used to identify any of a port's control bits:<br>
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
void portWriteBitadr(WORD bitAdr, BOOL value) {

    //Do nothing if given "Port bitAdr" is BITADR_NA = 0xffff
    //This will NOT cause any error for PIC24F chips. It will cause 0x07FF to be written. Nothing there!
    #if defined(IOPORTS_CHECK_BITADR)
    if (bitAdr == BITADR_NA)
        return;
    #endif

#if defined(__C30__)
    volatile WORD* p;
    WORD mask = 0x01 << ((bitAdr)>>12);
    p = (volatile WORD*)((bitAdr) & 0x07FF);    //Ensure not larger than 0x7ff = highest SFR!

    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_WARNING)
    if ( (((((WORD)p)>=CNPD1_ADR)&&(((WORD)p)<=CNPU6_ADR)) || ((((WORD)p)>=0x02C0)&&(((WORD)p)<=0x02F6))) == 0 ) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: portWriteBitadr() invalid Bitadr!");
        return;
    }
    #endif

    if (value==0) {
        *p = *p & (~mask);
    }
    else {
        *p = *p | mask;
    }
#elif defined(__C32__)
    //PIC32MZ - Update this code to also work for PIC32MZ processors!
    //Port addresses are 32-bit - upper 20-bits of address is 0xBF886
    volatile WORD* p;
    WORD mask = 0x01 << ((bitAdr)>>12);
    p = (volatile WORD*)(0xBF886000 | ((DWORD)((bitAdr) & 0x0FFF)) );

    if (value==0) {
        *p = *p & (~mask);
    }
    else {
        *p = *p | mask;
    }
#else
    #error "portWriteBitadr() not defined for this compiler!"
#endif
}


/**
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
#if !defined(IOPORTS_OPTIMIZE_SIZE)
WORD portGetTRIS(BYTE portID) {
    //Use PORTID_UCPORT_MAP to calculate requested "Port bitAdr"
    if (portID > PORT_ID_MAX) {
        //Don't print error if input was IOPORT_ID_NA
        if (portID != IOPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: portGetTRIS() invalid ID!");
        }
        return BITADR_NA;
    }

    return TRIS_PORTID_BITADR_MAP[portID];
}
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
WORD portGetTRISwOffset(BYTE portID, BYTE offset) {
    WORD bitadr;

    //Use PORTID_UCPORT_MAP to calculate requested "Port bitAdr"
    if (portID > PORT_ID_MAX) {
        //Don't print error if input was IOPORT_ID_NA
        if (portID != IOPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: portGetTRISwOffset() invalid ID!");
        }
        return BITADR_NA;
    }

    bitadr = TRIS_PORTID_BITADR_MAP[portID];

    if (bitadr==BITADR_NA)
        return BITADR_NA;

    return (bitadr + offset);
}


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
#if !defined(IOPORTS_OPTIMIZE_SIZE)
WORD ucportGetTRIS(BYTE ucportID) {
    WORD adrLAT;    //Address of LAT register

    if (ucportID > UCPORT_ID_MAX) {
        //Don't print error if ucportID is UCPORT_ID_NA
        if (ucportID != UCPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: ucportGetTRIS() invalid ID!");
        }
        return BITADR_NA;
    }

    //Check above takes care of this
    //if (ucportID==UCPORT_ID_NA) {
    //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nucportGetTRIS() NA port");
    //    return BITADR_NA;
    //}

#if defined(__C30__)
    // - TRISA is at address 0x02C0 for PIC24F chips
    // - Each following port's address is 8 bytes higher, for example TRISB is 0x02C8
    adrLAT = 0x02C0 + (8 * (ucportID/16));
#elif defined(__C32__)
    // - TRISA is at address 0xBF88 6010 for PIC32MX chips, and 0xBF86 0010 for PIC32MZ chips
    // - Each following port's address is 0x100 bytes higher, for example TRISB is 0xBF88 6110 for PIC32MX
    adrLAT = 0x6010 + (0x100 * (ucportID/16));
#else
    #error "portGetTRIS() not defined for this compiler!"
#endif

    //To get "Port bitAdr", have to add bit address to bits 12-15 of "Port bitAdr"
    adrLAT = adrLAT | (((WORD)(ucportID%16))<<12);

    return adrLAT;
}
#endif


/**
 * !!!!!!! IMPORTANT !!!!!!!!!
 * This functions if used by the system, is not meant for general purpose use!

 * Gets a "Port bitAdr" for the requested port's TRIS register's, adding the given
 * offset to it.
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 * @param offet An offset to add to returned value if it is NOT 0.
 *
 * @return Returns a "Port bitAdr" for the requested port's TRIS register.
 *         Returns BITADR_NA if not available, or error.
 */
WORD ucportGetTRISwOffset(BYTE ucportID, BYTE offset) {
    WORD bitadr;    //Address of LAT register

    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    if (ucportID > UCPORT_ID_MAX) {
        //Don't print error if ucportID is UCPORT_ID_NA
        if (ucportID != UCPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: ucportGetTRISwOffset() invalid ID!");
        }
        return BITADR_NA;
    }
    #endif

    //Check above takes care of this
    //if (ucportID==UCPORT_ID_NA) {
    //    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nucportGetTRIS() NA port");
    //    return BITADR_NA;
    //}

#if defined(__C30__)
    // - TRISA is at address 0x02C0 for PIC24F chips
    // - Each following port's address is 8 bytes higher, for example TRISB is 0x02C8
    bitadr = 0x02C0 + (8 * (ucportID/16));
#elif defined(__C32__)
    // - TRISA is at address 0xBF88 6010 for PIC32MX chips, and 0xBF86 0010 for PIC32MZ chips
    // - Each following port's address is 0x100 bytes higher, for example TRISB is 0xBF88 6110 for PIC32MX
    bitadr = 0x6010 + (0x100 * (ucportID/16));
#else
    #error "portGetTRIS() not defined for this compiler!"
#endif

    //To get "Port bitAdr", have to add bit address to bits 12-15 of "Port bitAdr"
    bitadr = bitadr | (((WORD)(ucportID%16))<<12);

    if (bitadr==BITADR_NA)
        return BITADR_NA;

    return (bitadr + offset);
}


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
#if !defined(IOPORTS_OPTIMIZE_SIZE)
WORD portGetCNPD(BYTE portID) {
    //Use PORTID_UCPORT_MAP to calculate requested "Port bitAdr"
    if (portID > PORT_ID_MAX) {
        //Don't print error if input was IOPORT_ID_NA
        if (portID != IOPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: portGetCNPD() invalid ID!");
        }
        return BITADR_NA;
    }

    return ucportGetCNPD(PORTID_UCPORT_MAP[portID]);
}
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
 * @param offet An offset to add to returned value if it is NOT 0.
 *
 * @return Returns a "Port bitAdr" for the requested port's CNPD (Pull-Down) register.
 *         Returns BITADR_NA if not available, or error.
 */
WORD portGetCNPDwOffset(BYTE portID, BYTE offset) {
    WORD bitadr;    //Address of LAT register

    //Use PORTID_UCPORT_MAP to calculate requested "Port bitAdr"
    if (portID > PORT_ID_MAX) {
        //Don't print error if input was IOPORT_ID_NA
        if (portID != IOPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: portGetCNPDwOffset() invalid ID!");
        }
        return BITADR_NA;
    }

    bitadr = ucportGetCNPD(PORTID_UCPORT_MAP[portID]);

    if (bitadr==BITADR_NA)
        return BITADR_NA;

    return (bitadr + offset);
}


/**
 * Gets a "Port bitAdr" for the requested port's CNPD register's. The port is identified by it's
 * "Microcontroller Port ID", is a UCPORT_ID_xx define (UCPORT_ID_01 for example).
 * The returned "Port bitAdr" can be used to set or clear the port using the portWriteBitadr(),
 * portSetBitadr() and portClearBitadr() functions.
 *
 * @param ucportID The port's "Microcontroller Port ID", is a UCPORT_ID_xx define
 *      (UCPORT_ID_01 for example).
 *
 * @return Returns a "Port bitAdr" for the requested port's CNPD register.
 *         Returns BITADR_NA if not available, or error.
 */
#if !defined(IOPORTS_OPTIMIZE_SIZE)
WORD ucportGetCNPD(BYTE ucportID) {
    WORD adrCNPD;    //Address of LAT register
    BYTE icn;       //ICN number

#if defined(__C30__)
    //Ensure given ucportID is within upper and lower limits of ICN_UCPORT_PIC24F_MAP
    if ((ucportID < ICN_UCPORT_PIC24F_MAP_STARTOFFSET) || (ucportID > ICN_UCPORT_PIC24F_MAP_LASTENTRY)) {
        //Don't print error if ucportID is UCPORT_ID_NA
        if (ucportID != UCPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: ucportGetCNPD() invalid ID!");
        }
        return BITADR_NA;
    }

    icn = ICN_UCPORT_PIC24F_MAP[ucportID - ICN_UCPORT_PIC24F_MAP_STARTOFFSET];

    //There is no CNPD bit for given "Microcontroller Port ID"
    if (icn == 0xff)
        return BITADR_NA;

    // - CNPD1 register is at address CNPD1_ADR for PIC24F chips
    // - Each CNPDx register contains 16 CNxPDE bits (icn number)
    // - Each following CNPDx port's address is 2 bytes higher, for example CNPD2 is 0x0058
    adrCNPD = CNPD1_ADR + (2 * (icn/16));

    //To get "Port bitAdr", have to add bit address to bits 12-15 of "Port bitAdr"
    adrCNPD = adrCNPD | (((WORD)(icn%16))<<12);
#elif defined(__C32__)
    if (ucportID > UCPORT_ID_MAX) {
        //Don't print error if ucportID is UCPORT_ID_NA
        if (ucportID != UCPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: ucportGetCNPD() invalid ID!");
        }
        return 0;
    }

    // - CNPDA is at address 0xBF88 6060 for PIC32MX chips, and 0xBF86 ???? for PIC32MZ chips
    // - Each following port's address is 0x100 bytes higher, for example CNPDB is 0xBF88 6160 for PIC32MX
    adrCNPD = 0x6060 + (0x100 * (ucportID/16));

    //To get "Port bitAdr", have to add bit address to bits 12-15 of "Port bitAdr"
    adrCNPD = adrCNPD | (((WORD)(ucportID%16))<<12);
#else
    #error "portGetCNPD() not defined for this compiler!"
#endif

    return adrCNPD;
}
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
 * @param offet An offset to add to returned value if it is NOT 0.
 *
 * @return Returns a "Port bitAdr" for the requested port's CNPD (Pull-Down) register.
 *         Returns BITADR_NA if not available, or error.
 */
WORD ucportGetCNPDwOffset(BYTE ucportID, BYTE offset) {
    WORD bitadr;    //Address of LAT register
    BYTE icn;       //ICN number

#if defined(__C30__)
    //Ensure given ucportID is within upper and lower limits of ICN_UCPORT_PIC24F_MAP
    if ((ucportID < ICN_UCPORT_PIC24F_MAP_STARTOFFSET) || (ucportID > ICN_UCPORT_PIC24F_MAP_LASTENTRY)) {
        //Don't print error if ucportID is UCPORT_ID_NA
        if (ucportID != UCPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: ucportGetCNPDwOffset() invalid ID!");
        }
        return BITADR_NA;
    }

    icn = ICN_UCPORT_PIC24F_MAP[ucportID - ICN_UCPORT_PIC24F_MAP_STARTOFFSET];

    //There is no CNPD bit for given "Microcontroller Port ID"
    if (icn == 0xff)
        return BITADR_NA;

    // - CNPD1 register is at address CNPD1_ADR for PIC24F chips
    // - Each CNPDx register contains 16 CNxPDE bits (icn number)
    // - Each following CNPDx port's address is 2 bytes higher, for example CNPD2 is 0x0058
    bitadr = CNPD1_ADR + (2 * (icn/16));

    //To get "Port bitAdr", have to add bit address to bits 12-15 of "Port bitAdr"
    bitadr = bitadr | (((WORD)(icn%16))<<12);
#elif defined(__C32__)
    if (ucportID > UCPORT_ID_MAX) {
        //Don't print error if ucportID is UCPORT_ID_NA
        if (ucportID != UCPORT_ID_NA) {
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\nERR: ucportGetCNPDwOffset() invalid ID!");
        }
        return BITADR_NA;
    }

    // - CNPDA is at address 0xBF88 6060 for PIC32MX chips, and 0xBF86 ???? for PIC32MZ chips
    // - Each following port's address is 0x100 bytes higher, for example CNPDB is 0xBF88 6160 for PIC32MX
    bitadr = 0x6060 + (0x100 * (ucportID/16));

    //To get "Port bitAdr", have to add bit address to bits 12-15 of "Port bitAdr"
    bitadr = bitadr | (((WORD)(ucportID%16))<<12);
#else
    #error "portGetCNPD() not defined for this compiler!"
#endif

    return (bitadr + offset);
}


/**
 * Set direction of Digital port.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 * @param dir Direction of port, 0 sets port as an output, 1 as an input.
 *
 * @param value HIGH will set the port(3.3V), and LOW will clear(0V) the port pin.
 */
void portConfigDir(BYTE portID, BOOL dir) {
    WORD bitadr;
    bitadr = portGetTRIS(portID);

    //Do nothing if given "Port bitAdr" is BITADR_NA
    if (bitadr == BITADR_NA)
        return;

    #if defined(IOPORTS_OPTIMIZE_SIZE)
    portWriteBitadr(bitadr, dir);
    #else
    portWriteBitadr_MACRO(bitadr, dir);
    #endif
}


/**
 * Write a Digital port.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 *
 * @param value HIGH will set the port(3.3V), and LOW will clear(0V) the port pin.
 */
void portWrite(BYTE portID, BYTE value) {
    WORD bitadr;
    bitadr = portGetLAT(portID);

    //Do nothing if given "Port bitAdr" is BITADR_NA
    if (bitadr == BITADR_NA)
        return;

    #if defined(IOPORTS_OPTIMIZE_SIZE)
    portWriteBitadr(bitadr, value);
    #else
    portWriteBitadr_MACRO(bitadr, value);
    #endif
}


/**
 * Read a Digital port.
 *
 * @param portID The "Port ID" of the requested port. Is a number from 0 to 41, also
 * 		known as the "Netcruzer Port Name".
 *
 * @return Returns the value of the requested digitial port, will be 0 (LOW = 0V) if not set,
 *      or 1 (HIGH = 3.3V) if set.
 */
BOOL portRead(BYTE portID) {
    WORD bitadr;
    bitadr = portGetPIN(portID);

    //Do nothing if given "Port bitAdr" is BITADR_NA
    if (bitadr == BITADR_NA)
        return 0;

    #if defined(IOPORTS_OPTIMIZE_SIZE)
    return portReadBitadr(bitadr);
    #else
    return portReadBitadr_MACRO(bitadr);
    #endif
}




