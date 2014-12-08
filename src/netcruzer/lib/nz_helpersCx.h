/**
 * @brief           Netcruzer Advanced Helper Functions, means to internal use.
 * @file            nz_helpersCx.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_helpersCx_desc Description
 *****************************************
 * This module contains code for implementing a variable size circular buffers. The size can
 * be configured at run time. It is not limited to a power of 2 value.
 *
 * @subsection nz_helpersCx_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // ----------- Netcruzer Helper Configuration (nz_helpersCx.h) -----------
 // *********************************************************************

 @endcode
 **********************************************************************
 * @section nz_helpersCx_lic Software License Agreement
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
#ifndef NZ_HELPERS_CX_H
#define NZ_HELPERS_CX_H

#if defined(HAS_WEBSERVER)
//Function prototypes for functions defined in Helpers.h
BYTE	btohexa_high(BYTE b);
BYTE	btohexa_low(BYTE b);
#endif


#if defined(__C30__)
    #define nzWordSwapBytes_ASM_R(W)    \
    { asm( "SWAP.w %0"                  \
        : "+r" (W) /*outputs*/); }
#elif defined(__PIC32MX__)
    //TODO PIC32MX - Fix this function! Current code is just to get XC32 to compiler for testing!
#else
	#error "Unknown processor or compiler."
#endif

/**
 * @function void nzWordBitToggle(WORD *w, WORD bitPosition)
 * 
 * Toggle bit in word.
 * @param w A pointer to the data item for which a bit should be toggled.
 * @param bitPosition A literal value in the range of 0 to 15.
 */
#if defined(__C30__)
    #define nzWordBitToggle(w, bitPosition) __builtin_btg(w,bitPosition)
#elif defined(__PIC32MX__)
    //TODO PIC32MX - Fix this function! Current code is just to get XC32 to compiler for testing!
#else
	#error "Unknown processor or compiler."
#endif


#if defined(__C30__)
    #define nzByteSwapNibble_ASM_R(W)   \
    { asm("SWAP.b %0"                   \
        : "+r"(W) /*outputs*/); }
#elif defined(__PIC32MX__)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * Finds the first occurrence of a 1 starting from the Most Significant bit of
 * given WORD, and working towards the Least Significant bit.
 * The bit number result is zero-extended to 16 bits.
 * Bit numbering begins with the Most Significant bit (allocated number 1)
 * and advances to the Least Significant bit (allocated number 16). A result
 * of zero indicates a 1 was not found.
 *
 * * !!!!! This instruction operates in Word mode only !!!!!
 */
#if defined(__C30__)
    #define nzWordPosOfFirstMsbBit_ASM(W, WPos)     \
    { asm( "FF1L %1,%0"                             \
        : "=r"(WPos) /*outputs*/                    \
        : "r"(W) /*inputs*/); }
#elif defined(__PIC32MX__)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * Finds the first occurrence of a 1 starting from the Least Significant bit of
 * given WORD, and working towards the Most Significant bit.
 * The bit number result is zero-extended to 16 bits.
 * Bit numbering begins with the Least Significant bit (allocated number 1)
 * and advances to the Most Significant bit (allocated number 16). A result of
 * zero indicates a 1 was not found.
 *
 * !!!!! This instruction operates in Word mode only !!!!!
*/
#if defined(__C30__)
    #define nzWordPosOfFirstLsbBit_ASM(W, WPos)     \
    { asm( "FF1R %1,%0"                             \
        : "=r"(WPos) /*outputs*/                    \
        : "r"(W) /*inputs*/ ); }
#elif defined(__PIC32MX__)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * @function WORD nzWordPosOfFirstChangeMsb(WORD value)
 *
 * Finds the first bit change from left in value. This is useful for dynamic scaling of
 * fixed-point data. Find the first occurrence of a one (for a positive value), or zero
 * (for a negative value), starting from the Most Significant bit after the sign bit of
 * Ws and working towards the Least Significant bit of the word operand. The bit number
 * result is sign-extended to 16 bits.
 * The next Most Significant bit after the sign bit is allocated bit number 0 and
 * the Least Significant bit is allocated bit number -14. This bit ordering
 * allows for the immediate use of Wd with the SFTAC instruction for scaling
 * values up. If a bit change is not found, a result of -15 is returned and the C
 * flag is set. When a bit change is found, the C flag is cleared.
 *
 * !!!!! This instruction operates in Word mode only !!!!!
 * 
 * @param value Integer number of first bit change.
 * @return Returns the shifted addition result to an accumulator.
 */
#if defined(__C30__)
    //#define nzWordPosOfFirstChangeMsb( W, WPos) asm( "FBCL %1,%0" : "=r"(WPos) /*outputs*/ : "r"(W) /*inputs*/)
    #define nzWordPosOfFirstChangeMsb(value) __builtin_fbcl(value);
#elif defined(__PIC32MX__)
#else
	#error "Unknown processor or compiler."
#endif


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
BYTE nzBytePosOfFirstClearedMsbBit(BYTE b);


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
BYTE nzBytePosOfFirstClearedLsbBit(BYTE b);


#if defined(__C30__)
    #define nzWordShiftLefs_ASM(W, Number)      \
    { asm( "SL %0, #%1, %0"                     \
        : "+r" (W) /*outputs*/                  \
        : "i" (Number) /*inputs*/ ); }
#elif defined(__PIC32MX__)
#else
	#error "Unknown processor or compiler."
#endif



#if defined(__C30__)
    #define nzWordShiftRight_ASM( W, Number)    \
    { asm( "SR %0, #%1, %0"                     \
        : "+r" (W) /*outputs*/                  \
        : "i" (Number) /*inputs*/ ); }
#elif defined(__PIC32MX__)
#else
	#error "Unknown processor or compiler."
#endif



/**
 * Sets the given memory with the given byte.
 *
 * @param adr Destination address of array
 * @param c Byte to set array to
 * @param count Number of bytes to write
 */
#if defined(__C30__)
    #define nzMemSet_ASM_RRR(adr, c, count)                     \
    { asm( "MOV %0,w0 \n REPEAT #(%2-1) \n MOV.B %1,[w0++]"     \
           : /*no outputs*/                                     \
           : "r" (adr), "r" (c), "i" (count)                    \
           : "w0"); }
#elif defined(__PIC32MX__)
    #define nzMemSet_ASM_RRR(adr, c, count)     memset(adr,c,count)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * Copy src array to destination array, countReg times, using ASM code.
 * Parameters have to be (RRR):
 * REGISTER (pDst), REGISTER (pSrc), REGISTER (countReg)
 *
 * <br>!!! IMPORTANT !!!
 * - Ensure countReg IS NOT 0
 * - Ensure countReg is a WORD (or cast to a WORD)
 *
 * When returning, pDst and pScr will both BE INCREMENTED countReg times !!!
 * If this is not required, use the nzMemCpy() function. The XC16 compiler seems
 * to ALWAYS increment pDst and pSrc, even if temporary variables are created and
 * passed to pDst and pSrc. If this is done, the compiler optimization ignores these
 * temporary variables, and still increments pSrc and pDst.
 *
 * So, remember:
 * - Only use this macro if pSrc and pDst should be incremented!
 * - Ensure countReg is a WORD, or cast to word. If countReg is a BYTE, this function
 *   can go very badly wrong!
 * - Use nzMemCpy() function if 0 checking of countReg is required.
 * - When returning, pDst and pScr will both BE INCREMENTED countReg times!
 *
 * ! NOTE on compiler optimization !
 * If after this macro the pDst and pSrc variables are restored to their original
 * values, it seems like the XC16 compiler IGNORES it (optimization on). It assumes
 * these bytes still have their original values, and ignores this code!
 *
 * Example<br>
 * @code
 *    WORD size = 7;
 *    BYTE arr[128];
 *    BYTE arrSrc[128];
 *    BYTE* pArr = arr;
 *    BYTE* pArrSrc = arrSrc;
 *
 *    //Ensure all first 2 parameters are of type BYTE*, and size is a WORD variable!
 *    nzMemCpy_ASM_RRR(pArr, pArrSrc, size);
 *
 *    //This is WRONG, ERROR!!! "arr" and "arrScr" are constants (address value)
 *    nzMemCpy_ASM_RRC(arr, arrSrc, 7);
 *
 *    //This is WRONG, ERROR!!! "size" must be a register, not a constant!
 *    nzMemCpy_ASM_RRR(arr, arrSrc, 7);
 * @endcode
 *
 * @param pDst Destination array, must be a BYTE* variable!
 * @param pSrc Source array, must be a BYTE* variable!
 * @param countReg Number of bytes to write, ENSURE IT IS NOT 0! Must be a WORD variable!
 */
#if defined(__C30__)
    #define nzMemCpy_ASM_RRR(pDst, pSrc, countReg)                                  \
    { asm volatile ( "DEC %2,%2 \n REPEAT %2 \n MOV.B [%1++],[%0++] \n INC %2,%2"   \
            : "+d" (pDst), "+d" (pSrc)  /* outputs, + = input/outputs */            \
            : "d" (countReg) /* inputs (must be "d", DEC required a W reg) */       \
            : "cc" ); }
#elif defined(__PIC32MX__)
    #define nzMemCpy_ASM_RRR(pDst, pSrc, countReg)      memcpy(pDst, pSrc, countReg)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * Alternative version for nzMemCpy_ASM_RRR. The "countReg" input does not get modified in this version!
 */
#if defined(__C30__)
    #define nzMemCpy2_ASM_RRR(pDst, pSrc, countReg)                                     \
    { asm volatile ( "MOV %2,w0 \n DEC w0,w0 \n REPEAT w0 \n MOV.B [%1++],[%0++]"       \
            : "+d" (pDst), "+d" (pSrc)  /* outputs, + = input/outputs */                \
            : "r" (countReg) /* inputs (can be "r", is copied to a W reg by MOV)  */    \
            : "w0", "cc" ); }
#elif defined(__PIC32MX__)
    #define nzMemCpy2_ASM_RRR(pDst, pSrc, countReg)     memcpy(pDst, pSrc, countReg)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * Save as nzMemCpy_ASM_RRR(), but increments pointers backwards.
 *
 * <br>!!! IMPORTANT !!!
 * - Ensure countReg IS NOT 0
 * - Ensure countReg is a WORD (or cast to a WORD)
 *
 * When returning, pDst and pScr will both BE DECREMENTED countReg times !!!
 */
#if defined(__C30__)
    #define nzMemCpyDec_ASM_RRR(pDst, pSrc, countReg)                            \
    { asm  ( "DEC %2,%2 \n REPEAT %2 \n MOV.B [%1--],[%0--] \n INC %2,%2"        \
            : "+d" (pDst), "+d" (pSrc)  /* outputs, + = input/outputs */         \
            : "d" (countReg) /* inputs */                                        \
            : "cc" ); }

#elif defined(__PIC32MX__)
    //TODO PIC32MX - Fix this function! Currently just put memcpy in here so it compiles for testin
    #define nzMemCpyDec_ASM_RRR(pDst, pSrc, countReg)       memcpy(pDst, pSrc, countReg)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * Alternative version for nzMemCpy_ASM_RRR. The "countReg" input does not get modified in this version!
 */
#if defined(__C30__)
    #define nzMemCpyDec2_ASM_RRR(pDst, pSrc, countReg)                              \
    { asm volatile ( "MOV %2,w0 \n DEC w0,w0 \n REPEAT w0 \n MOV.B [%1--],[%0--]"   \
            : "+d" (pDst), "+d" (pSrc)  /* outputs, + = input/outputs */            \
            : "r" (countReg) /* inputs */                                           \
            : "w0", "cc" ); }
#elif defined(__PIC32MX__)
    //TODO PIC32MX - Fix this function! Currently just put memcpy in here so it compiles for testin
    #define nzMemCpyDec2_ASM_RRR(pDst, pSrc, countReg)      memcpy(pDst, pSrc, countReg)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * Similar to nzMemCpyDec_ASM_RRR, but NOT garaunteed that pDst and pSrc will be incremented
 * on return (no output). They could be, but should not be relied on!
 *
 * <br>!!! IMPORTANT !!!
 * - Ensure countReg IS NOT 0
 * - Ensure countReg is a WORD (or cast to a WORD)
 *
 * When returning, pDst and pScr will both BE DECREMENTED countReg times !!!
 */
#if defined(__C30__)
    #define nzMemCpyDecNoOut_ASM_RRR(pDst, pSrc, countReg)                  \
    { asm  ( "DEC %2,%2 \n REPEAT %2 \n MOV.B [%1--],[%0--] \n INC %2,%2"   \
            : /* outputs, + = input/outputs */                              \
            : "d" (pDst), "d" (pSrc), "d" (countReg) /* inputs */           \
            : "cc" ); }
#elif defined(__PIC32MX__)
    //TODO PIC32MX - Fix this function! Currently just put memcpy in here so it compiles for testin
    #define nzMemCpyDecNoOut_ASM_RRR(pDst, pSrc, countReg)      memcpy(pDst, pSrc, countReg)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * Copy src array to destination array, countReg times, using ASM code.
 * Parameters have to be (RRC):
 * REGISTER (pDst), REGISTER (pSrc), CONSTANT (countReg)
 *
 * <br>!!! IMPORTANT !!!
 * - Ensure countReg IS NOT 0
 * - Ensure countReg is a WORD (or cast to a WORD)
 *
 * When returning, pDst and pScr will both BE INCREMENTED countReg times !!!
 * If this is not required, use the nzMemCpy() function. The XC16 compiler seems
 * to ALWAYS increment pDst and pSrc, even if temporary variables are created and
 * passed to pDst and pSrc. If this is done, the compiler optimization ignores these
 * temporary variables, and still increments pSrc and pDst.
 *
 * So, remember:
 * - Only use this macro if pSrc and pDst should be incremented!
 * - Ensure countReg is a WORD, or cast to word. If countReg is a BYTE, this function
 *   can go very badly wrong!
 * - Use nzMemCpy() function if 0 checking of countReg is required.
 * - When returning, pDst and pScr will both BE INCREMENTED countReg times!
 *
 * ! NOTE on compiler optimization !
 * If after this macro the pDst and pSrc variables are restored to their original
 * values, it seems like the XC16 compiler IGNORES it (optimization on). It assumes
 * these bytes still have their original values, and ignores this code!
 *
 * Example<br>
 * @code
 *    WORD size = 7;
 *    BYTE arr[128];
 *    BYTE arrSrc[128];
 *    BYTE* pArr = arr;
 *    BYTE* pArrSrc = arrSrc;
 *
 *    //Ensure all first 2 parameters are of type BYTE*, and size is a constant!
 *    nzMemCpy_ASM_RRC(pArr, pArrSrc, 7);
 *
 *    //This is WRONG, ERROR!!! "arr" and "arrScr" are constants (address value)
 *    nzMemCpy_ASM_RRC(arr, arrSrc, 7);
 *
 *    //This is WRONG, ERROR!!! "size" must be a constant, NOT a register!
 *    nzMemCpy_ASM_RRC(arr, arrSrc, size);
 * @endcode
 *
 * @param pDst Destination array, must be a BYTE* variable!
 * @param pSrc Source array, must be a BYTE* variable!
 * @param count Number of bytes to write, ENSURE IT IS NOT 0! Must be a WORD literal constant!
 */
#if defined(__C30__)
    #define nzMemCpy_ASM_RRC(pDst, pSrc, count)         \
    { asm( "REPEAT #(%2-1) \n MOV.B [%1++],[%0++]"      \
            :                                           \
            : "r" (pDst), "r" (pSrc), "i" (count) ); }
#elif defined(__PIC32MX__)
    #define nzMemCpy_ASM_RRC(pDst, pSrc, countReg)      memcpy(pDst, pSrc, countReg)
#else
	#error "Unknown processor or compiler."
#endif


/**
 * Same as nzMemCpy_ASM_RRC(), but copies backwards.
 */
#if defined(__C30__)
    #define nzMemCpyDec_ASM_RRC(pDst, pSrc, count)                          \
    { asm volatile ( "REPEAT #(%2-1) \n MOV.B [%1--],[%0--]"                \
            : "+r" (pDst), "+r" (pSrc) /* outputs, + = input/outputs */     \
            :  "i" (count) /* inputs */ ); }
#elif defined(__PIC32MX__)
    //TODO PIC32MX - Fix this function! Current code is just to get XC32 to compiler for testing!
    #define nzMemCpyDec_ASM_RRC(pDst, pSrc, countReg)   memcpy(pDst, pSrc, countReg)
#else
	#error "Unknown processor or compiler."
#endif

#endif
