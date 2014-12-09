/**
 * @brief           iMod Driver Functions
 * @file            nz_imod.c
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
 * 2013-09-03, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_IMOD_C

#include "HardwareProfile.h"

#if defined(HAS_IMOD)

#include "nz_imod.h"

/////////////////////////////////////////////////
// Defines


/////////////////////////////////////////////////
// Local variables. Lower nibble is current state, upper nibble is latched values


/////////////////////////////////////////////////
// Global variables.


//Add debugging to this file. The DEBUG_LEVEL_ALLOFF macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if defined (DEBUG_LEVEL_ALLOFF)
    #define MY_DEBUG_LEVEL      0
#else
    #define MY_DEBUG_LEVEL      DEBUG_LEVEL_WARNING
#endif
#include "nz_debug.h"



/////////////////////////////////////////////////
//Include iMod c files

// im2BL
#if (IM2BL_COUNT >= 1)
#include "nz_im2bl.c"
#endif  //#if (IM2BL_COUNT >= 1)

// im4MOS
#if (IM4MOS_COUNT >= 1)
#include "nz_im4mos.c"
#endif  //#if (IM4MOS_COUNT >= 1)

// im6BT
#if (IN6BT_COUNT >= 1)
#include "nz_in6bt.c"
#endif  //(IN6BT_COUNT >= 1)




#if defined(IMOD_ENABLE_MULTI_PORT_FUNCTIONS)

/**
 * Get the value of Pin 3 for the given iMod port
 *
 * @param port The iMod port, a value from 0 to n
 *
 * @return The value of the requested iMod Port's pin 3
 */
BOOL imod_GetPin3(BYTE port) {
    switch(port) {
#if defined(IMOD0_PIN3_IS_DIG) || defined(IMOD0_PIN3_IS_XDIG)
        case 0:
        return imod0_GetPin3();
#endif
#if defined(IMOD1_PIN3_IS_DIG) || defined(IMOD1_PIN3_IS_XDIG)
        case 1:
        return imod1_ReadPin3();
#endif
#if defined(IMOD2_PIN3_IS_DIG) || defined(IMOD2_PIN3_IS_XDIG)
        case 2:
        return imod2_ReadPin3();
#endif
#if defined(IMOD3_PIN3_IS_DIG) || defined(IMOD3_PIN3_IS_XDIG)
        case 3:
        return imod3_GetPin3();
#endif
#if defined(IMOD4_PIN3_IS_DIG) || defined(IMOD4_PIN3_IS_XDIG)
        case 4:
        return imod4_GetPin3();
#endif
#if defined(IMOD5_PIN3_IS_DIG) || defined(IMOD5_PIN3_IS_XDIG)
        case 5:
        return imod5_GetPin3();
#endif
#if defined(IMOD6_PIN3_IS_DIG) || defined(IMOD6_PIN3_IS_XDIG)
        case 6:
        return imod6_GetPin3();
#endif
#if defined(IMOD7_PIN3_IS_DIG) || defined(IMOD7_PIN3_IS_XDIG)
        case 7:
        return imod7_GetPin3();
#endif
#if defined(IMOD8_PIN3_IS_DIG) || defined(IMOD8_PIN3_IS_XDIG)
        case 8:
        return imod8_GetPin3();
#endif
#if defined(IMOD9_PIN3_IS_DIG) || defined(IMOD9_PIN3_IS_XDIG)
        case 9:
        return imod9_GetPin3();
#endif
#if defined(IMOD10_PIN3_IS_DIG) || defined(IMOD10_PIN3_IS_XDIG)
        case 10:
        return imod10_GetPin3();
#endif
    }   //switch(port)

    return 0;
}


/**
 * Get the value of Pin 5 of the given iMod port
 *
 * @param port The iMod port, a value from 0 to n
 *
 * @return The value of the requested iMod Port's pin 5
 */
BOOL imod_GetPin5(BYTE port) {
    switch(port) {
#if defined(IMOD0_PIN5_IS_DIG) || defined(IMOD0_PIN5_IS_XDIG)
        case 0:
        return imod0_GetPin5();
#endif
#if defined(IMOD1_PIN5_IS_DIG) || defined(IMOD1_PIN5_IS_XDIG)
        case 1:
        return imod1_ReadPin5();
#endif
#if defined(IMOD2_PIN5_IS_DIG) || defined(IMOD2_PIN5_IS_XDIG)
        case 2:
        return imod2_ReadPin5();
#endif
#if defined(IMOD3_PIN5_IS_DIG) || defined(IMOD3_PIN5_IS_XDIG)
        case 3:
        return imod3_GetPin5();
#endif
#if defined(IMOD4_PIN5_IS_DIG) || defined(IMOD4_PIN5_IS_XDIG)
        case 4:
        return imod4_GetPin5();
#endif
#if defined(IMOD5_PIN5_IS_DIG) || defined(IMOD5_PIN5_IS_XDIG)
        case 5:
        return imod5_GetPin5();
#endif
#if defined(IMOD6_PIN5_IS_DIG) || defined(IMOD6_PIN5_IS_XDIG)
        case 6:
        return imod6_GetPin5();
#endif
#if defined(IMOD7_PIN5_IS_DIG) || defined(IMOD7_PIN5_IS_XDIG)
        case 7:
        return imod7_GetPin5();
#endif
#if defined(IMOD8_PIN5_IS_DIG) || defined(IMOD8_PIN5_IS_XDIG)
        case 8:
        return imod8_GetPin5();
#endif
#if defined(IMOD9_PIN5_IS_DIG) || defined(IMOD9_PIN5_IS_XDIG)
        case 9:
        return imod9_GetPin5();
#endif
#if defined(IMOD10_PIN5_IS_DIG) || defined(IMOD10_PIN5_IS_XDIG)
        case 10:
        return imod10_GetPin5();
#endif
    }   //switch(port)

    return 0;
}


#endif  //#if defined(HAS_IMOD)

#endif  //#if defined(IM2BL_ENABLE_MULTI_PORT_FUNCTIONS)
