/**
 * @brief           Conversion helper functions
 * @file            convert.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 *
 * @section nz_convert_desc Description
 *****************************************
 * Conversion helper functions
 *
 *
 *********************************************************************/

 /*********************************************************************
 * @section nz_convert_lic Software License Agreement
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
 * 2010-06-08, David Hosken (DH):
 *    - Initial version
 *********************************************************************/


#ifndef _CONVERT_H_
#define _CONVERT_H_

/**
 * Converts a unsigned 8 bit value to a string
 * @param b Unsigned 8 bit value to convert
 * @param str Buffer to write converted value to
 * @param formatedLength Gives the string length of the returned string. If 0, the sting
 *                       lengh is automatically calculated.
 */
void uitoa(uint16 b, char * str, uint8 formatedLength);

#endif
