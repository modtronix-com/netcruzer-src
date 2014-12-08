/*********************************************************************
 * convert.c  -   Conversion helper functions
 * Dependencies:  
 * Complier:      Microchip C30 and C32 compilers
 * Company:       Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description - See convert.h
 *
 *********************************************************************
 * File History
 *
 * 2006-06-08, David Hosken (DH):
 *    - Initial version
 *********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is intended and supplied to you,
 * the Company customer, for use solely and exclusively on products
 * manufactured by Modtronix Engineering. The code may be modified
 * and can be used free of charge for non commercial and commercial
 * applications. All rights are reserved. Any use in violation of the
 * foregoing restrictions may subject the user to criminal sanctions
 * under applicable laws, as well as to civil liability for the
 * breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */
#include "projdefs.h"
#include "sys\convert.h"

/////////////////////////////////////////////////
// Defines

/////////////////////////////////////////////////
// Global variables. Lower nibble is current state, upper nibble is latched values


/**
 * Converts a unsigned 8 bit value to a string
 * @param b Unsigned 8 bit value to convert
 * @param str Buffer to write converted value to
 * @param formatedLength Gives the string length of the returned string. If 0, the sting
 *                       lengh is automatically calculated.
 * @return Nothing
 */
void uitoa(uint16 b, char * str, uint8 formatedLength)
{
    int i;

    if (formatedLength == 0) {
        if (b >= 10000)
            i = 5;
        else if (b >= 1000)
            i = 4;
        else if (b >= 100)
            i = 3;
        else if (b >= 10)
            i = 2;
        else
            i = 1;
    }
    else
        i = formatedLength;

    str[i] = '\0';  //Terminate with 0
    do {
        str[i-1] = ((uint8)(b % 10)) + '0';
        b = b / 10;
        i--;
    } while (i != 0);
}
