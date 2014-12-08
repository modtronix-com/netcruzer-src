/**
 * Application Configuration functions
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
 * 2011-08-16, David Hosken (DH):
 *  - Initial version, modified version of Microchip's xEeprom.h file from their TCP/IP stack
 *********************************************************************/

#ifndef xeePROM_H
#define xeePROM_H

#define XEE_SUCCESS FALSE

//Define size (in bytes) of EEPROM used
#define EEPROM_SIZE                        (4096)

typedef BOOL XEE_RESULT;

#if (EEPROM_SIZE > 65535)
    typedef DWORD XEE_ADDRESS;
#else
    typedef WORD XEE_ADDRESS;
#endif


//void xeeInit(void);

XEE_RESULT xeeBeginWrite(XEE_ADDRESS address);

XEE_RESULT xeeWrite(BYTE val);

void xeeWriteArray(BYTE *val, WORD wLen);

XEE_RESULT xeeEndWrite(void);

XEE_RESULT xeeBeginRead(XEE_ADDRESS address);

BYTE xeeRead(void);

XEE_RESULT xeeReadArray(XEE_ADDRESS address, BYTE *buffer, WORD length);

XEE_RESULT xeeEndRead(void);

BOOL xeeIsBusy(void);

#endif
