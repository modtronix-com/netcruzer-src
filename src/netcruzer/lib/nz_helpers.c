/**
 * @brief           Netcruzer Main Include file
 * @file            nz_helpers.h
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
 * 2012-08-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_HELPERS_C

#include "HardwareProfile.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"

#if defined( __C30__ )
    #include "libpic30.h"
#elif defined( __PIC32MX__ )
#else
#error "Compiler not defined"
#endif


/**
 * Delays for the given number of Milli Seconds. Value must be in the range from
 * 1 to 65,536.
 *
 * @param msDelay Milli Seconds to delay
 */
void nzDelayMs(WORD msDelay) {
#if defined( __C30__ )
	__delay32( (DWORD)( ((DWORD)(msDelay)) * ((DWORD)((GetInstructionClock())/1000ULL)) ));
#elif defined( __PIC32MX__ )
#else
    #error "Compiler not defined"
#endif
}

/**
 * Delays for the given number of Micro Seconds. Value must be in the range from
 * 2 to 65,536.
 *
 * @param usDelay Micro Seconds to delay
 */
void nzDelayUs(WORD usDelay) {
#if defined( __C30__ )
	if (usDelay < 2)
		return;
	__delay32( (DWORD)( ((DWORD)(usDelay-1)) * ((DWORD)((GetInstructionClock())/1000000ULL)) ));
#elif defined( __PIC32MX__ )
#else
    #error "Compiler not defined"
#endif
}

#if !defined(HAS_WEBSERVER)
/**
 * Converts a "2 character ASCII hex" string to a single packed byte.
 *
 * @param asciiChars - WORD_VAL where
 *          asciiChars.v[0] is the ASCII value for the lower nibble and
 *          asciiChars.v[1] is the ASCII value for the upper nibble.
 *          Each must range from '0'-'9', 'A'-'F', or 'a'-'f'.
 *
 * @return Returns byte representation of given "2 character ASCII byte"
 */
BYTE nzAsciiHexToByte(WORD_VAL asciiChars) {
	// Convert lowercase to uppercase
	if(asciiChars.v[1] > 'F')
		asciiChars.v[1] -= 'a'-'A';
	if(asciiChars.v[0] > 'F')
		asciiChars.v[0] -= 'a'-'A';

	// Convert 0-9, A-F to 0x0-0xF
	if(asciiChars.v[1] > '9')
		asciiChars.v[1] -= 'A' - 10;
	else
		asciiChars.v[1] -= '0';

	if(asciiChars.v[0] > '9')
		asciiChars.v[0] -= 'A' - 10;
	else
		asciiChars.v[0] -= '0';

	// Concatenate
	return (asciiChars.v[1]<<4) |  asciiChars.v[0];
}
#endif

/**
 * Get the hex value for the given hex encoded character (0-9, a-f).
 * The returned value will be from 0 - 15. If the given byte is not a hex encoded
 * character, 0xff is returned!
 *
 * @param c A hex encoded character, value values are 0-9, A-F and a-f
 *
 * @return Returns a value from 0 to 15 representing the hex value of given parameter. Or, 0xff if error.
 */
BYTE nzAsciiHexNibbleToByte(BYTE c) {
    if ((c<='9') && (c>='0'))
        return (c - '0');
    else if ((c<='F')&&(c>='A'))
        return (c - 55);
    else if ((c<='f')&&(c>='a'))
        return (c - 87);

    return 0xff;    //Indicate given byte was NOT alpha numerical
}

/**
 * Converts a byte to a "2-character uppercase ASCII hex" value. The 2 bytes
 * are returned in the bytes of the returned WORD_VAL.
 * For example, nzByteToAsciiHex(0xAE) will return 'E' in the LSB, and 'A' in
 * the MSB of the returned WORD.
 *
 * @param b The byte containing the nibble to convert
 *
 * @return The LSB of the returned word contains the ASCII value for the lower nibble.
 *         The MSB of the returned word contains the ASCII value for the upper nibble.
 */
WORD_VAL nzByteToAsciiHex(BYTE b) {
    WORD_VAL w;
    w.byte.LB = nzLowNibbleToAsciiHex(b);
    w.byte.HB = nzHighNibbleToAsciiHex(b);
    return w;
}

/**
 * Converts a byte to a "2-character uppercase ASCII hex" string.
 * For example, nzByteToAsciiHex(0xAE) will return the string "AE" (LSB='E').
 * Will always return 2 characters, for example 0x5 will return "05".
 * A NULL termination is NOT added to the back of the returned 2 characters.
 *
 * @param b The byte containing the nibble to convert
 *
 * @param dst Pointer to buffer to write the result to. Will always write 2
 *      characters, without a NULL termination.
 */
void nzByteToAsciiHexStr(BYTE b, char* dst) {
    dst[0] = nzHighNibbleToAsciiHex(b);
    dst[1] = nzLowNibbleToAsciiHex(b);
}


#if !defined(HAS_WEBSERVER)
/**
 * Converts the upper nibble of a binary value to a hexadecimal ASCII byte.
 * For example, nzHighNibbleToAsciiHex(0xAE) will return 'A'.
 * Same as Microchip "btohexa_high()" function
 *
 * @param b The byte containing the nibble to convert
 *
 * @return The converted ASCII hex character for the given nibble
 */
BYTE nzHighNibbleToAsciiHex(BYTE b)
{
    b >>= 4;
    return (b>0x9u) ? b+'A'-10:b+'0';
}
#endif

#if !defined(HAS_WEBSERVER)
/**
 * Converts the lower nibble of a binary value to a hexadecimal ASCII byte.
 * For example, nzLowNibbleToAsciiHex(0xAE) will return 'E'.
 * Same as Microchip "btohexa_low()" function
 *
 * @param b The byte containing the nibble to convert
 *
 * @return The converted ASCII hex character for the given nibble
 */
BYTE nzLowNibbleToAsciiHex(BYTE b)
{
    b &= 0x0F;
    return (b>9u) ? b+'A'-10:b+'0';
}
#endif

/**
 * Counts number of bits in x
 *
 * param x: Given parameter to count bit's that are 1
 */
BYTE nzBitCount(WORD x) {
    BYTE count;

    for (count = 0; x != 0; x >>= 1) {
        if (x & 0x01)
            count++;
    }

    return count;
}


/**
 * Counts number of bits in given array that are set to 1
 *
 * @param p: Pointer to array whos bits we must count
 * @param size: Number of bytes in array we must count bits of
 */
BYTE nzBitCountArr(BYTE* p, BYTE size) {
    BYTE offset;
    BYTE x;
    BYTE count=0;

    for (offset = 0; offset < size; offset++) {
        x = *p++;
        for (count = 0; x != 0; x >>= 1) {
            if (x & 0x01)
                count++;
        }
    }
    return count;
}


/**
 * Finds the first occurrence of a 0 starting from the Most Significant bit of
 * given BYTE, and working towards the Least Significant bit.
 * Bit numbering begins with the Most Significant bit (allocated number 8)
 * and advances to the Least Significant bit (allocated number 1). A result
 * of zero indicates a 0 was not found.
 *
 * !!!!! IMPORTANT !!!!!
 * Note that this function returns 1 for LSB bit, and 8 for LSB. The equavalent WORD
 * function nzWordPosOfFirstMsbBit_ASM() return 1 for MSB (16 for LSB)!
 *
 * @param b Byte to search
 * @return A value from 1-8, or 0 if all bit were 1
 */
BYTE nzBytePosOfFirstClearedMsbBit(BYTE b) {
    BYTE i = 8;
    BYTE mask = 0x80;
    do {
        if ((b & mask)==0)
            return i;
        i--;
        mask = mask >> 1;
    } while(mask!=0);

    return 0;   //Not found
}

/**
 * Finds the first occurrence of a 0 starting from the Least Significant bit of
 * given BYTE, and working towards the Most Significant bit.
 * The bit number result is zero-extended to 8 bits.
 * Bit numbering begins with the Least Significant bit (allocated number 1)
 * and advances to the Most Significant bit (allocated number 8). A result of
 * zero indicates a 0 was not found.
 *
 * @param b Byte to search
 * @return A value from 1-8, or 0 if all bit were 1
 */
BYTE nzBytePosOfFirstClearedLsbBit(BYTE b) {
    BYTE i = 1;
    BYTE mask = 0x01;
    do {
        if ((b & mask)==0)
            return i;
        i++;
        mask = mask << 1;
    } while(mask!=0);

    return 0;   //Not found
}


/**
 * Get the value of the given decimal string. String value can have
 * leading 0s. Maximum value is 65,535. Following possible leading format specifiers are
 * checked for:
 * - x = Upper Case Hex value to follow
 *
 * Some examples:
 * "255" will return 0x00FF
 * "x010" will return 0x0010
 *
 * @param str String containing hex value
 *
 * @param retFlags A value returned from this function.
 *        - bit 0-3 is the number of bytes parsed by this function
 *        - bits 4-7 are reserved for flags, like if the parsed value is a byte
 *          or word for example.
 *
 * @return Parsed word value
 */
WORD nzParseDecHexWord(const char* str, BYTE* retFlags) {
    WORD val = 0;   //Returned value
    BYTE ret = 0;   //Return retFlags value
    BOOL isHex=0;
    const char* savedStrPtr;

    savedStrPtr = str;

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nparseHexDecWord()");

    //Does the string contain a Upper Case Hex value
    if (*str=='x') {
        str++;
        isHex = TRUE;
        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " -d");
    }

    //Parse all '0'-'9', and 'A'-'F' bytes
    while((((*str>='0')&&(*str<='9')) || ((*str>='A')&&(*str<='F')))) {
        if (isHex) {
            val = (val<<4) + ((*str<='9')?(*str-'0'):(*str-'A'+10));
        }
        else {
            val = (val*10) + ((*str)-'0');
        }
        str++;
    }

    //Get the number of bytes parsed
    ret = str - savedStrPtr;
    if (retFlags != NULL) {
        *retFlags = ret;
    }

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " l=");
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, ret);
    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " v=");
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, val);

    return val;
}


/**
 * Get the value of the given Upper Case Hex, or decimal string. String value can have
 * leading 0s. Maximum value is 0xFFFF. Following possible leading format specifiers are
 * checked for:
 * - d = Decimal value to follow
 *
 * Some examples:
 * "A100" will return 0xA100
 * "d010" will return 0xA
 *
 * @param str String containing hex value
 *
 * @param retFlags A value returned from this function.
 *        - bit 0-3 is the number of bytes parsed by this function
 *        - bits 4-7 are reserved for flags, like if the parsed value is a byte
 *          or word for example.
 *
 * @return Parsed word value
 */
WORD nzParseHexDecWord(const char* str, BYTE* retFlags) {
    WORD val = 0;   //Returned value
    BYTE ret = 0;   //Return retFlags value
    BOOL isDecimal=0;
    const char* savedStrPtr;

    savedStrPtr = str;

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nparseHexDecWord()");

    //Does the string contain a decimal value
    if (*str=='d') {
        str++;
        isDecimal = TRUE;
        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " -d");
    }

    //Parse all '0'-'9', and 'A'-'F' bytes
    while((((*str>='0')&&(*str<='9')) || ((*str>='A')&&(*str<='F')))) {
        if (isDecimal) {
            val = (val*10) + ((*str)-'0');
        }
        else {
            val = (val<<4) + ((*str<='9')?(*str-'0'):(*str-'A'+10));
        }
        str++;
    }

    //Get the number of bytes parsed
    ret = str - savedStrPtr;
    if (retFlags != NULL) {
        *retFlags = ret;
    }

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " l=");
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, ret);
    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " v=");
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, val);

    return val;
}


const WORD NZ_UITOA_DIV[] = {1,10,100,1000};

/**
 * Converts a 16-bit unsigned integer to a null-terminated decimal string.
 *
 * @param Value - The number to be converted
 *
 * @param Buffer - Pointer in which to store the converted string
 */
void nzUintToAsciiStr(WORD val, BYTE* buf)
{
	BYTE i;
	WORD digit;
	WORD divisor;
	BOOL printed = FALSE;

	if(val)
	{
        divisor = 10000;
		for(i=5; i!=0; i--)
		{
			digit = val/divisor;
			if(digit || printed)
			{
				*buf++ = '0' + digit;
				val -= digit*divisor;
				printed = TRUE;
			}

            divisor = NZ_UITOA_DIV[i-2];

            //Alternative method
            //divisor /= 10;

            //Alternative method
            //if (i==5)
            //    divisor = 1000;
            //else if (i==4)
            //    divisor = 100;
            //else if (i==3)
            //    divisor = 10;
            //else
            //    divisor = 1;
		}
	}
	else
	{
		*buf++ = '0';
	}

	*buf = '\0';
}


/**
 * Writes the given value to the "const BYTE" at the given address. The FLASH location is NOT erased, so bits
 * of variable at given address can only change from 1's to 0's. When erasing a FLASH, all bits are set to 1's.
 * For example, if variable is 0xff, it can be written with any value. If varialbe is 0, it can not be changed
 * (all bits are already 0's).
 *
 * When placing data in program memory with "const BYTE" syntax, only the lower words of instructions
 * are used. The upper word of instructions has a usable byte, and the "phantom byte", which are both
 * not used. When using the __builtin_tbladdress() function, we get the word address used in program
 * memory for given "const BYTE" variable.
 * - "const BYTE" with even address is stored in LSB of lower word of instruction.<br>
 *   To update this byte, place it in LSB of __builtin_tblwtl(). Supply varialbes address to __builtin_tblwtl().<br>
 *   MSB of __builtin_tblwtl() will update variable one address higher, at address: ((varialbe's address)+1).<br>
 *   __builtin_tblwth() is not used for "const BYTE" variable.<br>
 * - "const BYTE" with odd address is stored in MSB of lower word of instruction.<br>
 *   To update this byte, place it in MSB of __builtin_tblwtl(). Supply ((varialbe's address)&0xfffe) to __builtin_tblwtl().<br>
 *   LSB of __builtin_tblwtl() will update variable one address lower, at address: ((varialbe's address)&0xfffe).<br>
 *   __builtin_tblwth() is not used for "const BYTE" variable.<br>
 *
 * @param adr: Address of "const BYTE" variable to update. Use "__builtin_tbladdress(&var)" to get address
 *
 * @param newVal: New value that should be assigned to given "const BYTE" variable. Only 0 bits will be written.
 */
void nzFlashWriteConst(DWORD adr, BYTE newVal) {
    WORD_VAL newWVal;

#if defined(__C30__)
    newWVal.Val = 0xffff;    //Writing 0xffff to FLASH will not change anything

    //- If adr is on an odd address, it means it is located in the MSB of the lower word of the instruction (instructions
    //are 2 Words in 16-bit PICs). Set MSB to newVal. LSB will be 0xff, and not corrupt the variable that is located there.
    if (adr&0x00000001) {
        newWVal.v[1] = newVal;
    }
    //- If adr is on an even address, it means it is located in the LSB of the lower word of the instruction (instructions
    //are 2 Words in 16-bit PICs). Set LSB to newVal. MSB will be 0xff, and not corrupt the variable that is located there.
    else {
        newWVal.v[0] = newVal;
    }

    //Set up NVMCON for word programming
    NVMCON = 0x4003; // Initialize NVMCON

    //When placing data in program memory with "const BYTE" syntax, only the lower words of instructions
    //are used. The upper word of instructions has a usable byte, and the "phantom byte", which are both
    //not used. When using the __builtin_tbladdress() function, we get the word address used in program
    //memory for given "const BYTE" variable.
    //- "const BYTE" variable with even address is stored in LSB of lower word of instruction.
    //  To update this byte, place it in LSB of __builtin_tblwtl(). Supply varialbes address to __builtin_tblwtl().
    //  MSB of __builtin_tblwtl() will update variable one address higher, at address: ((varialbe's address)+1).
    //  __builtin_tblwth() is not used for "const BYTE" variable.
    //- "const BYTE" with odd address is stored in MSB of lower word of instruction.
    //  To update this byte, place it in MSB of __builtin_tblwtl(). Supply ((varialbe's address)&0xfffe) to __builtin_tblwtl().
    //  LSB of __builtin_tblwtl() will update variable one address lower, at address: ((varialbe's address)&0xfffe).
    //  __builtin_tblwth() is not used for "const BYTE" variable.
    //
    //Set up pointer to the first memory location to be written
    TBLPAG = adr>>16;       // Initialize PM Page Boundary SFR

    //Perform TBLWT instructions to write latches
    __builtin_tblwtl(adr & 0xFFFF, newWVal.Val);    // Write to address low word. This contains two "const BYTE" variables
    __builtin_tblwth(adr & 0xFFFF, 0xffff);         // Write to upper byte. Not used for "const BYTE" variable. Use 0xffff = do not change FLASH
    asm("DISI #5");                     // Block interrupts with priority < 7 for next 5 instructions
    __builtin_write_NVM();              // C30 function to perform unlock
#elif defined(__PIC32MX__)

#else
    #error "Unknown processor or compiler."
#endif

    return;
}

void nzMemCpy(BYTE* pDst, BYTE* pSrc, WORD count)
{
    //Have to decrement countReg, seeing that REPEAT instruction copies countreg+1 times!
    if (count-- == 0)
        return;

#if defined(__C30__)
    //All 3 operands are inputs. Used with the "d" constraint, seeing that they will all
    //be W0 to W7 registers (XC16 functions use W0 to W7 for parameter passing)
    asm( "REPEAT %2 \n MOV.B [%1++],[%0++]"
        : /*no outputs*/
        : "d" (pDst), "d" (pSrc), "d" (count) );

    //Alternative method if asm is not used
    //memcpy(pDst, pSrc, countReg+1);

    //This methods DOES NOT work. Works with 0 XC16 optimization, but not with optimization enabled!
    //Thought it should work, seeing that pDst and pSrc are both output and input parameters. But, DOESN'T work!!!
    //The 2nd and 3rd operand are inputs, but their values also get changed = input-output operands.
    //To specify this, place them in "output-operand" section, and use "+r" constraint
    //to indicate it is a register that is both an input AND output
//    asm( "REPEAT %2 \n MOV.B [%1++],[%0++]"
//       : "+r" (pDst), "+r" (pSrc)
//       : "r" (countReg) );
#elif defined(__PIC32MX__)
#else
	#error "Unknown processor or compiler."
#endif

}


void nzMemCpyNoCheck(BYTE* pDst, BYTE* pSrc, WORD count)
{
#if defined(__C30__)
//All 3 operands are inputs. Used with the "d" constraint, seeing that they will all
//be W0 to W7 registers (XC16 functions use W0 to W7 for parameter passing)
// - "pDst" and "pSrc" are inputs, but their values also get changed = input-output operands.
//   To specify this, place them in "output-operand" section, and use "+d" constraint to
//   indicate it is a register(d) that is both an input AND output(+)
//    asm( "DEC %2,%2 \n REPEAT %2 \n MOV.B [%1++],[%0++]"
//        : /*no outputs*/
//        : "d" (pDst), "d" (pSrc), "d" (count) );
    asm volatile ( "DEC %2,%2 \n REPEAT %2 \n MOV.B [%1++],[%0++]"
        : "+d" (pDst), "+d" (pSrc), "+d" (count) /*outputs*/
        : /*inputs*/ );
#elif defined(__PIC32MX__)
#else
	#error "Unknown processor or compiler."
#endif
}


void nzMemCpyDecNoCheck(BYTE* pDst, BYTE* pSrc, WORD count)
{

#if defined(__C30__)
//All 3 operands are inputs. Used with the "d" constraint, seeing that they will all
//be W0 to W7 registers (XC16 functions use W0 to W7 for parameter passing)
// - "pDst" and "pSrc" are inputs, but their values also get changed = input-output operands.
//   To specify this, place them in "output-operand" section, and use "+d" constraint to
//   indicate it is a register(d) that is both an input AND output(+)
//    asm( "DEC %2,%2 \n REPEAT %2 \n MOV.B [%1--],[%0--]"
//        : /*no outputs*/
//        : "d" (pDst), "d" (pSrc), "d" (count) );
    asm volatile ( "DEC %2,%2 \n REPEAT %2 \n MOV.B [%1--],[%0--]"
        : "+d" (pDst), "+d" (pSrc), "+d" (count) /*outputs*/
        : /*inputs*/ );
#elif defined(__PIC32MX__)
#else
	#error "Unknown processor or compiler."
#endif
}


