/**
 * @brief           Functions for an Incremental Rotary Encoder
 * @file            nz_encIncRot.c
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
#define THIS_IS_NZ_ENC_INC_ROT_C

#include "HardwareProfile.h"
#include "nz_encIncRot.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"


//Add debugging to this file. The DEBUG_CONF_DS2482 macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_INCROTENC)
    #define DEBUG_CONF_INCROTENC       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_INCROTENC
#include "nz_debug.h"


////////// Defines //////////////////////////////

//Resolution in us, must be power of 2 value, ex. 8,16,32,64,128... Is used as timebase for storing time between events
#if !defined(ENC_INC_ROT_RESOLUTION)
#define ENC_INC_ROT_RESOLUTION 16
#endif

//Debounce time in us
#if !defined(ENC_INC_ROT_DEBOUNCE)
#define ENC_INC_ROT_DEBOUNCE 8
#endif


////////// Function Prototypes //////////////////


////////// Variables ////////////////////////////


/**
 * Initialization.
 *
 * @param pObj Pointer to ENCROT_INFO structure
 *
 * @param portIdChA The "Port ID" connected to channel A of the encoder. Is a
 * 			number from 0 to 41, also known as the "Netcruzer Port Name".
 * 
 * @param portIdChB The "Port ID" connected to channel B of the encoder. Is a
 * 			number from 0 to 41, also known as the "Netcruzer Port Name".
 *
 * @return The status of the Initialization, 0=Success, else error code
 */
BYTE rotinc_init(ENCROT_INFO* pObj, BYTE portIdChA, BYTE portIdChB) {
    WORD_VAL typeConf;

    pObj->put = 0;
    pObj->flagVal = 0;
    pObj->oldUticks = 0;

    typeConf.byte.HB = PCFG_DIN_TYPE;   //Digital Input type
    typeConf.byte.LB = 0;               //No configuration
    if (portConfig(portIdChA, typeConf.Val) != 0) {
        return 1;   //ERROR
    }
    if (portConfig(portIdChB, typeConf.Val) != 0) {
        return 1;   //ERROR
    }

    //Save port A & B
    pObj->bitadrChA = portGetPIN(portIdChA);
    pObj->bitadrChB = portGetPIN(portIdChB);

    return 0;   //Success
}

/**
 * DS2482 Task, is called when Channel A causes an interrupt
 */
void rotinc_task(ENCROT_INFO* pObj) {
    BOOL newValA, newValB;
    DWORD newUticks;
    DWORD_VAL newUtickDiff;

    newUticks = utick32Get();
    newUtickDiff.Val = newUticks - pObj->oldUticks;

    //Less then debounce time, ignore and return
    if (newUtickDiff.Val < utick32ConvertFromUS(ENC_INC_ROT_DEBOUNCE) ) {
        return;
    }
    
    //Get new values of A and B channel
    newValA = portReadBitadr(pObj->bitadrChA);
    newValB = portReadBitadr(pObj->bitadrChB);

    //Check for false reading. New and old value for A should never be the same.
    //Could be due to channel bounce.
    if (newValA == pObj->flags.oldValA) {
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nIncRotEnc Old=New");
        return;
    }

    //Get new diffirence, convert "Micro Tick" to resolution as given by ENC_INC_ROT_RESOLUTION
    newUtickDiff.Val = newUtickDiff.Val / utick32ConvertFromUS(ENC_INC_ROT_RESOLUTION);

    //If difference is larger than we can store in our buffer (overflow), set to maximum value
    //We only have the lower 15-bit available. The 16th bit is the sign
    if (newUtickDiff.Val & 0xffff8000) {
        newUtickDiff.word.LW = 0x7fff;   //Set to maximum value. Only lower 15 bits of lower WORD is used
    }

    //If encoder is being turned CCW (counterclock wise), set MSB (bit 15) of value.
    if (newValA != newValB) {
        newUtickDiff.word.LW |= 0x8000;
    }

    //Store time elapsed since last event
    pObj->buf[pObj->put] = newUtickDiff.word.LW;

    pObj->put++;
    if (pObj->put >= ENC_INC_ROT_BUFSIZE)
        pObj->put = 0;


    //Update old values
    pObj->oldUticks = newUticks;
    pObj->flags.oldValA = newValA;
}


