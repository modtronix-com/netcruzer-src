/**
 * @brief           Netcruzer RTOS Fibers
 * @file            nzos_fiber.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 & XC32 Compilers
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
 * 2014-02-02, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZOS_FIBER_C

#include "HardwareProfile.h"
#include "nzos_defsInternal.h"
#include "nzos_fiber.h"

#if (nzosFIBER_ENABLE==1)

// TEMP TEMP for testing
#define DEBUG_CONF_NZOS_FIBER DEBUG_LEVEL_INFO

//Add debugging to this file. The DEBUG_CONF_NZOS_FIBER macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_NZOS_FIBER)
    #define DEBUG_CONF_NZOS_FIBER       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_NZOS_FIBER
#include "nz_debug.h"
#include "nz_helpersCx.h"


////////// Defines //////////////////////////////


////////// Function Prototypes //////////////////


////////// Variables ////////////////////////////


////////// Internal Global Variables ////////////
#if (nzosFIBER_LEVELS==2) || (nzosFIBER_LEVELS==4) || (nzosFIBER_LEVELS==6) || (nzosFIBER_LEVELS==8)
NZOS_FIBER_INFO zvFbrInfo;
#else
#error "nzosFIBER_LEVELS not supported"
#endif


void nzFbrInit(void) {
    //Initialize all fibers with 0
    memset(&zvFbrInfo, 0, sizeof(zvFbrInfo));
}


WORD nzFbrCreate(BYTE level, BOOL highPriority, void (*ptrFiber)(void), FIBER_TCB* fbrTcb) {
    BYTE bitsUsed = 0;
    BYTE i, mask, bitFree;
    WORD retMask;

    //Convert level from 1-nzosFIBER_LEVELS, to 0 to (nzosFIBER_LEVELS-1)
    level--;

    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
    if (level>=nzosFIBER_LEVELS) {
        debugPutString("\nnzFbrCreate Invalid level!");
        return NZ_FBR_ERR_INVALID_LEVEL;
    }
    #endif

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nFbrCreate Success");

    //Set bit in bitsUsed for each pFiber pointer that has already been allocated an address
    mask = 0x01;
    for (i=0; i<8; i++) {
        if (zvFbrInfo.pFibers[(level*8)+i] != 0) {
            bitsUsed |= mask;   //Set bit indicating what fibers are already allocated for this level
        }
        mask = mask << 1;
    }

    //Get position of first free pFiber pointer
    if (highPriority)
        bitFree = nzBytePosOfFirstClearedMsbBit(bitsUsed);
    else
        bitFree = nzBytePosOfFirstClearedLsbBit(bitsUsed);

    //Check if all available fibers in requested level are taken
    if (bitFree == 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nFbrCreate Level Full!");
        return NZ_FBR_ERR_LEVEL_FULL;
    }
    bitFree--;  //Convert from 1-8, to 0-7

    zvFbrInfo.pFibers[(level*8)+bitFree] = ptrFiber;

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nFbrCreated, lvl=");
    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, level+1);
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " pos=");
    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, bitFree);

    fbrTcb->pSchedule = &zvFbrInfo.runBits[level/2];
    retMask = (level&0x01) ? 0x0100 : 0x0001;
    fbrTcb->mask = retMask << bitFree;

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " mask=0x");
    DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, fbrTcb->mask);

    return 0;   //Success
}


/**
 * Run the given fiber
 * @param priority
 */
//void nzFbrSchedule(WORD hFbr) {
//    ((BYTE*)zvFbrInfo.runBits)[nzWordGetHighByte_MACRO(hFbr)]=nzWordGetLowByte_MACRO(hFbr);
//
//    //zvArrFibers[priority]();
//    //Set flag for kernel to indicate fiber must be ran
//    //....
//
//    //Schedule the "Netcruzer RTOS Kernel" to run!
//    NZOS_INT_KERNEL_SET_IF();
//    return 0;
//}



#endif  //if (nzosFIBER_ENABLE==1)