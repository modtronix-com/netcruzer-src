/*********************************************************************
 * HWP_SBC66ECL.h   - Hardware Profile for SBC66ECL board when it is used for this project
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
 * 2012-06-08, David H. (DH):
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
#ifndef __HWP_SBC66ECL_H
#define __HWP_SBC66ECL_H

#if defined(BRD_SBC66ECL_R2)

// *********************************************************************
// ------------------------------- UART --------------------------------
// *********************************************************************
#define UARTTX_TRIS         (TRISFbits.TRISF3)
#define UARTTX_IO           (PORTFbits.RF3)
#define UARTRX_TRIS         (TRISDbits.TRISD0)
#define UARTRX_IO           (PORTDbits.RD0)

// Select which UART the STACK_USE_UART and STACK_USE_UART2TCP_BRIDGE
// options will use.  You can change these to U1BRG, U1MODE, etc. if you
// want to use the UART1 module instead of UART2.
#define UBRG                    U2BRG
#define UMODE                   U2MODE
#define USTA                    U2STA
#define BusyUART()              BusyUART2()
#define CloseUART()             CloseUART2()
#define ConfigIntUART(a)        ConfigIntUART2(a)
#define DataRdyUART()           DataRdyUART2()
#define OpenUART(a,b,c)         OpenUART2(a,b,c)
#define ReadUART()              ReadUART2()
#define WriteUART(a)            WriteUART2(a)
#define getsUART(a,b,c)         getsUART2(a,b,c)
#define putsUART(a)             putsUART2((unsigned int*)a)
#define getcUART()              getcUART2()
#define putcUART(a)             do{while(BusyUART()); WriteUART(a); while(BusyUART()); }while(0)
#define putrsUART(a)            putrsUART2(a)

#endif  //#if defined(BRD_SBC66ECL_R2)
#endif
