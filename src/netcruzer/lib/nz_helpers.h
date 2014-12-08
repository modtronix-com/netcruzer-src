/**
 * @brief           Netcruzer Helper Functions
 * @file            nz_helpers.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_helpers_desc Description
 *****************************************
 * This module contains code for implementing a variable size circular buffers. The size can
 * be configured at run time. It is not limited to a power of 2 value.
 *
 * @subsection nz_helpers_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // ----------- Netcruzer Helper Configuration (nz_helpers.h) -----------
 // *********************************************************************

 @endcode
 **********************************************************************
 * @section nz_helpers_lic Software License Agreement
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
 * 2010-11-24, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_HELPERS_H
#define NZ_HELPERS_H

#if defined(HAS_WEBSERVER)
//Function prototypes for functions defined in Helpers.h
BYTE	btohexa_high(BYTE b);
BYTE	btohexa_low(BYTE b);
#endif


#define delay_us(us)	nzDelayUs(us)
#define delay_ms(ms)	nzDelayMs(ms)


/**
 * Delays for the given number of Milli Seconds. Value must be in the range from
 * 1 to 65,536.
 *
 * @param msDelay Milli Seconds to delay
 */
void nzDelayMs(WORD msDelay);


/**
 * Delays for the given number of Micro Seconds. Value must be in the range from
 * 2 to 65,536.
 *
 * @param usDelay Micro Seconds to delay
 */
void nzDelayUs(WORD usDelay);


#define DWORD_BYTE0(dw)                 ((unsigned char)((dw)&0xFF))
#define DWORD_BYTE1(dw)                 ((unsigned char)(((dw)>>8)&0xFF))
#define DWORD_BYTE2(dw)                 ((unsigned char)(((dw)>>16)&0xFF))
#define DWORD_BYTE3(dw)                 ((unsigned char)(((dw)>>24)&0xFF))

/**
 * @function BYTE nzDwordGetLowByte_MACRO(DWORD dw)
 *
 * Get the low BYTE of the given DWORD.
 * @param dw The DWORD
 * @return Returns the low BYTE (bits 0-7)
 */
#define nzDwordGetLowByte_MACRO(dw)     ((unsigned char)((dw)&0xFF))
#define DWORD_LOW_BYTE(dw)              ((unsigned char)((dw)&0xFF))

/**
 * @function BYTE nzDwordGetHighByte_MACRO(DWORD dw)
 *
 * Get the high BYTE of the given DWORD.
 * @param dw The DWORD
 * @return Returns the high BYTE (bits 24-31)
 */
#define nzDwordGetHighByte_MACRO(dw)    ((unsigned char)(((dw)>>24)&0xFF))
#define DWORD_HIGH_BYTE(dw)             ((unsigned char)(((dw)>>24)&0xFF))


/**
 * @function WORD nzDwordGetLowWord_MACRO(DWORD dw)
 *
 * Get the low WORD of the given DWORD.
 * @param dw The DWORD
 * @return Returns the low WORD (bits 0-15)
 */
#define nzDwordGetLowWord_MACRO(dw)     ((unsigned char)((dw)&0xFFFF))
#define DWORD_LOW_WORD(dw)              ((unsigned char)((dw)&0xFFFF))


/**
 * @function WORD nzDwordGetHighWord_MACRO(DWORD dw)
 *
 * Get the high WORD of the given DWORD.
 * @param dw The DWORD
 * @return Returns the high WORD (bits 16-31)
 */
#define nzDwordGetHighWord_MACRO(dw)    ((unsigned char)(((dw)>>16)&0xFFFF))
#define DWORD_HIGH_WORD(dw)             ((unsigned char)(((dw)>>16)&0xFFFF))

#define WORD_BYTE0(w)                   ((unsigned char)((w)&0xFF))
#define WORD_BYTE1(w)                   ((unsigned char)(((w)>>8)&0xFF))

/**
 * @function BYTE nzWordGetLowByte_MACRO(WORD w)
 *
 * Get the low BYTE of the given WORD.
 * @param w The WORD
 * @return Returns the low BYTE (bits 0-7)
 */
#define nzWordGetLowByte_MACRO(w)       ((unsigned char)((w)&0xFF))
#define WORD_LOW_BYTE(w)                ((unsigned char)((w)&0xFF))


/**
 * Get the high BYTE of the given WORD.
 * @param w The WORD
 * @return Returns the high BYTE (bits 8-15)
 */
#define nzWordGetHighByte_MACRO(w)    ((unsigned char)(((w)>>8)&0xFF))
#define WORD_HIGH_BYTE(w)             ((unsigned char)(((w)>>8)&0xFF))


/**
 * @function BYTE nzByteGetLowNibble_MACRO(BYTE b)
 *
 * Gets the low nibble (4 bits) of given BYTE.
 * @param b The BYTE
 * @return The low nibble (bits 0-3)
 */
#define nzByteGetLowNibble_MACRO(b)     ((unsigned char)((b)&0x0F))
#define BYTE_LOW_NIBBLE(b)              ((unsigned char)((b)&0x0F))


/**
 * @function BYTE nzByteGetHighNibble_MACRO(BYTE b)
 *
 * Gets the high nibble (4 bits) of given BYTE.
 * @param b The BYTE
 * @return The high nibble (bits 4-7)
 */
#define nzByteGetHighNibble_MACRO(b)    ((unsigned char)(((b)>>4)&0x0F))
#define BYTE_HIGH_NIBBLE(b)             ((unsigned char)(((b)>>4)&0x0F))


/**
 * @function void nzWordSetHighByte_MACRO(WORD w, BYTE b)
 *
 * Sets the high BYTE of given WORD.
 * @param w The WORD to set the high byte of
 * @param b The BYTE to set in the given WORD
 */
#define nzWordSetHighByte_MACRO(w, b) (((BYTE*)&(w))[1] = (b))


/**
 * @function void nzWordSetLowByte_MACRO(WORD w, BYTE b)
 *
 * Sets the low BYTE of given WORD.
 * @param w The WORD to set the low byte of
 * @param b The BYTE to set in the given WORD
 */
#define nzWordSetLowByte_MACRO(w, b) (((BYTE*)&(w))[0] = (b))


/**
 * @function WORD nzWordFromBytes_MACRO(BYTE msb, BYTE lsb)
 * 
 * Convert the given 2 BYTEs to a single WORD
 * @param lsb The BYTE to use for the LSB (bits 0-7) of the WORD
 * @param msb The BYTE to use for the MSB (bits 8-15) of the WORD
 * @return The WORD consisting of the given BYTEs
 */
#define nzWordFromBytes_MACRO(msb,lsb)  ( ((WORD)((msb)<<8)) | ((WORD)(lsb)) )
#define WORD_FROM_BYTES(msb,lsb)        ( ((WORD)((msb)<<8)) | ((WORD)(lsb)) )


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
BYTE nzAsciiHexToByte(WORD_VAL asciiChars);
#if defined(HAS_WEBSERVER)
#define nzAsciiHexToByte(asciiChars) hexatob(asciiChars)
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
BYTE nzAsciiHexNibbleToByte(BYTE c);


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
WORD_VAL nzByteToAsciiHex(BYTE b);


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
void nzByteToAsciiHexStr(BYTE b, char* dst);


/**
 * Converts the upper nibble of a binary value to a hexadecimal ASCII byte.
 * For example, nzHighNibbleToAsciiHex(0xAE) will return 'A'.
 *
 * @param b The byte containing the nibble to convert
 *
 * @return The converted ASCII hex character for the given nibble
 */
BYTE nzLowNibbleToAsciiHex(BYTE b);
#if defined(HAS_WEBSERVER)
#define nzLowNibbleToAsciiHex(b) btohexa_low(b)
#endif

/**
 * Converts the lower nibble of a binary value to a hexadecimal ASCII byte.
 * For example, nzLowNibbleToAsciiHex(0xAE) will return 'E'.
 *
 * @param b The byte containing the nibble to convert
 *
 * @return The converted ASCII hex character for the given nibble
 */
BYTE nzHighNibbleToAsciiHex(BYTE b);
#if defined(HAS_WEBSERVER)
#define nzHighNibbleToAsciiHex(b) btohexa_high(b)
#endif

/**
 * Counts number of bits in x
 *
 * param x: Given parameter to count bit's that are 1
 */
BYTE nzBitCount(WORD x);

/**
 * Counts number of bits in given array that are set to 1
 *
 * @param p: Pointer to array whos bits we must count
 * @param size: Number of bytes in array we must count bits of
 */
BYTE nzBitCountArr(BYTE* p, BYTE size);


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
WORD nzParseDecHexWord(const char* str, BYTE* retFlags);


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
WORD nzParseHexDecWord(const char* str, BYTE* retFlags);


/**
 * Converts a 16-bit unsigned integer to a null-terminated decimal string.
 *
 * @param val - The number to be converted
 *
 * @param buf - Pointer in which to store the converted string
 */
void nzUintToAsciiStr(WORD val, BYTE* buf);


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
 *- "const BYTE" variable with even address is stored in LSB of lower word of instruction.<br>
 *  To update this byte, place it in LSB of __builtin_tblwtl(). Supply varialbes address to __builtin_tblwtl().<br>
 *  MSB of __builtin_tblwtl() will update variable one address higher, at address: ((varialbe's address)+1).<br>
 *  __builtin_tblwth() is not used for "const BYTE" variable.<br>
 *- "const BYTE" with odd address is stored in MSB of lower word of instruction.<br>
 *  To update this byte, place it in MSB of __builtin_tblwtl(). Supply ((varialbe's address)&0xfffe) to __builtin_tblwtl().<br>
 *  LSB of __builtin_tblwtl() will update variable one address lower, at address: ((varialbe's address)&0xfffe).<br>
 *  __builtin_tblwth() is not used for "const BYTE" variable.<br>
 *
 * @param adr: Address of "const BYTE" variable to update. Use "__builtin_tbladdress(&var)" to get address
 * @param newVal: New value that should be assigned to given "const BYTE" variable. Only 0 bits will be written.
 */
void nzFlashWriteConst(DWORD adr, BYTE newVal);


/**
 * Copy src array to destination array, countReg times, using fast REPEAT instructions.
 * This function will check that count is NOT 0. If it is, it will return with no error.
 *
 * @param pDst Destination array
 * @param pSrc Source array
 * @param count Number of bytes to write, ENSURE IT IS NOT 0 !
 */
void nzMemCpy(BYTE* pDst, BYTE* pSrc, WORD count);

/**
 * Copy src array to destination array, countReg times, using fast REPEAT instructions.
 *
 * <br>!!! IMPORTANT !!!
 * - Ensure countReg IS NOT 0
 *
 * This function does NOT check that count is NOT 0. If it is, it will cause an error!
 *
 * @param pDst Destination array
 * @param pSrc Source array
 * @param count Number of bytes to write, ENSURE IT IS NOT 0 !
 */
void nzMemCpyNoCheck(BYTE* pDst, BYTE* pSrc, WORD count);

/**
 * Save as nzMemCpyNoCheck(), but increments pointers backwards.
 * <br>!!! IMPORTANT !!!
 * - Ensure countReg IS NOT 0
 * - Ensure countReg is a WORD (or cast to a WORD)
 *
 * When returning, pDst and pScr will both BE DECREMENTED countReg times !!!
 */
void nzMemCpyDecNoCheck(BYTE* pDst, BYTE* pSrc, WORD count);


#endif
