/**
 * @brief           Netcruzer RTOS Internal Defines
 * @file            nzos_defsInternal.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 & XC32 Compilers
 *
 **********************************************************************
 * @section nzos_defsInternal_lic Software License Agreement
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
#ifndef NZOS_DEFS_INTERNAL_H
#define NZOS_DEFS_INTERNAL_H

#if (nzosENABLE==1)

////////// Default Defines //////////////////////

#if !defined(nzTckGetUtick)
#define nzTckGetUtick()                          utick32Get()
#endif


////////// Internal Global Variables ////////////


////////// Function Prototypes //////////////////
//Netcruzer RTOS includes
#if (nzosENABLE==1)

#endif
#if (nzosFSM_ENABLE==1)

#endif
#if (nzosFIBER_ENABLE==1)
void nzFbrInit(void);
#endif
#if (nzosTASK_ENABLE==1)

#endif
#if (nzosEVENT_ENABLE==1)

#endif
#if (nzosMUTEX_ENABLE==1)

#endif
#if (nzosQUEUE_ENABLE==1)

#endif
#if (nzosSEMAPHORE_ENABLE==1)

#endif


#endif  //#if (nzosENABLE==1)

#endif  //#ifndef NZOS_DEFS_INTERNAL_H
