 /**
 * @brief           Variable size Circular Buffer.
 * @file            nz_circularBufferStd.c
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
 * 2010-11-10, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_CIRCULAR_BUFFER_STD_C

#include "HardwareProfile.h"

#if defined(CIRBUF_USE_CIRCULAR_BUFFER_STD)

#include "nz_circularBufferStd.h"
#include "nz_helpers.h"
#include "nz_helpersCx.h"
#include "nz_interrupt.h"

#include <stdlib.h>     //for malloc, free
#include <string.h>     //for memset

//Add debugging to this file. The DEBUG_CONF_CIRCULAR_BUFFER macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_CIRCULAR_BUFFER)
    #define DEBUG_CONF_CIRCULAR_BUFFER        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_CIRCULAR_BUFFER
#include "nz_debug.h"



// Function Prototypes //////////////////////////
void checkIfDummyAndRemove(CIRBUF* pBuf);


/**
 * Default Buffer initialization. Buffer type is set to streaming (no packet formatting),
 * and with no formatting. Use this function creating a basic buffer with no
 * formatting. Bytes are added and removed in serial fashion.
 * Use cbufInit() function to initialize buffer with different type and formatting.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param bufArray Pointer to buffer array that is used for the this CIRBUF
 *        objects internal storage. Must have at least 'size' bytes available.
 *
 * @param size Size of this buffer.
 */
void cbufInitDefault(CIRBUF* pBuf, BYTE* bufArray, WORD size)
{
    //Check if size is power of 2 -AND- equal to or larger than 8
    //Report error if not 8,16,32,64,128,512,1024,2048,4096,8192,16384....
    #if defined(HAS_NZ_DEBUGGING)
    if ( size > 16384) {
        debugErrorFlags.bits.bCirbufSizeError = 1;
        size = 256;     //Set size to default value
    }
    #endif

    //No formatting or type, is a streaming buffer
    pBuf->flagVal = CIRBUF_TYPE_STREAMING | CIRBUF_FORMAT_NONE;

    pBuf->maxOffset = size - 1;
    pBuf->put = pBuf->get = 0;      //Initialize buffer empty
    pBuf->buf = bufArray;
}


/**
 * Buffer initialization. Buffer type and format is configured via typeFormat
 * parameter.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param bufArray Pointer to buffer array that is used for the this CIRBUF
 *        objects internal storage. Must have at least 'size' bytes available.
 *
 * @param size Size of this buffer.
 *
 * @param typeFormat Gives the 'type' and 'format' of the buffer. Is an ORed combination
 *      of CIRBUF_FORMAT_XX and CIRBUF_TYPE_XX defines. For example:
 *      (CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING)
 */
void cbufInit(CIRBUF* pBuf, BYTE* bufArray, WORD size, BYTE typeFormat)
{
    //Default initialization
    cbufInitDefault(pBuf, bufArray, size);
    
    #if defined(HAS_NZ_DEBUGGING)
        //Packets NOT supported
        #if defined(CIRBUF_DISABLE_PACKETS)
        if ( (typeFormat & 0x03) != 0) {
            debugErrorFlags.bits.bCirbufPacketFormatError = 1;
        }
        //Packets ARE supported
        #else
            //Large packets NOT supported
            #if defined(CIRBUF_DISABLE_LARGE_PACKET)
            if ( (typeFormat & 0x03) == CIRBUF_TYPE_LARGE_PACKET) {
                debugErrorFlags.bits.bCirbufPacketFormatError = 1;
            }
            #endif
        #endif
    #endif

    pBuf->typeFormat.val = typeFormat;
}


/**
 * Sets the Type and Format of the buffer. This is normally done via the cbufInit()
 * or cbufInitDefault() functions!
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param typeFormat Gives the 'type' and 'format' of the buffer. Is an ORed combination
 *      of CIRBUF_FORMAT_XX and CIRBUF_TYPE_XX defines. For example:
 *      (CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING)
 */
void cbufSetTypeAndFormat(CIRBUF* pBuf, BYTE typeFormat) {
    pBuf->typeFormat.val = typeFormat;
}


#if defined(CIRBUF_OPTIMIZE_SIZE)
/**
 * Empty the given buffer. Any data contained in the buffer is lost.
 *
 * For "Packet" buffer, do not use this function. When calling cbufPutPacket(), and there
 * is no space AND 2 contiguous blocks, write dummy packet. This will cause the next
 * bufGetXxx function to remove dummy packet, and reset buffer to PUT=GET=0
 *
 * @param pBuf Pointer to CIRBUF structure
 */
void cbufEmpty(CIRBUF* pBuf) {
    //Don't update put, only GET
    (pBuf)->get = (pBuf)->put;
    return;
}
#endif


#if defined(CIRBUF_OPTIMIZE_SIZE)
/**
 * Checks if the given buffer is empty.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns true if the given buffer is empty. Else, returns false.
 */
BOOL cbufIsEmpty(CIRBUF* pBuf) {
    return ((pBuf)->get == (pBuf)->put);
}
#endif


/**
 * Checks if the given transmit buffer is full - no more space for adding data.
 * This is the case when put+1 = get
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns true if the given buffer is full. Else, returns false.
 */
BOOL cbufIsFull(CIRBUF* pBuf)
{
    WORD putCpy, getCpy;

    //Thread save, get copies of GET and PUT, and only use them in this function.
    NZ_BUILTIN_DISI(0x3FFF); // Disable interrupts, excluding level 7
    putCpy = (pBuf)->put;
    getCpy = (pBuf)->get;
    NZ_BUILTIN_DISI(0x0000); // Enable interrupts


    if (putCpy==(pBuf)->maxOffset)
        return (getCpy==0);
    else
        return ((putCpy+1)==getCpy);
}


///**
// * Checks if the given buffer is not full. Has space for adding data.
// *
// * For <b>Multi Threaded Applications</b>, call from "PUT context".
// *
// * @param pBuf Pointer to CIRBUF structure
// *
// * @return Returns true if the given buffer has data.
// */
//BOOL cbufHasSpace(CIRBUF* pBuf)
//{
//    WORD putCpy, getCpy;
//
//    //Thread save, get copies of GET and PUT, and only use them in this function.
//    NZ_BUILTIN_DISI(0x3FFF); // Disable interrupts, excluding level 7
//    putCpy = (pBuf)->put;
//    getCpy = (pBuf)->get;
//    NZ_BUILTIN_DISI(0x0000); // Enable interrupts
//
//    return (putCpy==(pBuf)->maxOffset) ? (getCpy!=0) : ((putCpy+1)!=getCpy);
//}


/**
 * Gets number of bytes available in buffer.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns number of bytes available in buffer.
 */
WORD cbufGetCount(CIRBUF* pBuf)
{
    WORD putCpy, getCpy;

    //Thread save, get copies of GET and PUT, and only use them in this function.
    NZ_BUILTIN_DISI(0x3FFF); // Disable interrupts, excluding level 7
    putCpy = (pBuf)->put;
    getCpy = (pBuf)->get;
    NZ_BUILTIN_DISI(0x0000); // Enable interrupts

    return (putCpy>=getCpy) ? (putCpy-getCpy) : ((putCpy+(pBuf)->maxOffset+1)-getCpy);
}


/**
 * Gets number of free bytes available in buffer. This is how much space is available
 * for writing.
 *
 * Do NOT use for "Packet Buffers" (if this buffer has been configured as
 * a "Packet Buffer"). Use cbufGetFreeForPacket() in stead!
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns number of free bytes available in buffer. This is maximum bytes we can write to buffer
 */
WORD cbufGetFree(CIRBUF* pBuf)
{
    WORD putCpy, getCpy;

    //Thread save, get copies of GET and PUT, and only use them in this function.
    NZ_BUILTIN_DISI(0x3FFF); // Disable interrupts, excluding level 7
    putCpy = (pBuf)->put;
    getCpy = (pBuf)->get;
    NZ_BUILTIN_DISI(0x0000); // Enable interrupts

    return (putCpy < getCpy) ? (getCpy-putCpy-1) : ((getCpy + (pBuf)->maxOffset) - putCpy);
}


/**
 * Returns this buffer's format.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns this buffer's format, is a CIRBUF_FORMAT_XXX constant
 */
BYTE cbufGetFormat(CIRBUF* pBuf) {
    return (pBuf)->flagTF.format;
}


/**
 * Returns this buffer's type.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns this buffer's type, is a CIRBUF_TYPE_XXX constant
 */
BYTE cbufGetType(CIRBUF* pBuf) {
    return (pBuf)->flagTF.type;
}


/**
 * Gets the buffer size required for the given array, taking "escape characters"
 * into account. The default escape character is '^'. For each of these characters
 * contained in the array, it has to be escaped with a second '^' character.
 * For example, the string "Esc=^" will return 6, even though the string is only
 * 5 characters long. This is how much space is required to add this string to the
 * buffer. It is stored as "Esc=^^" in the buffer.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param buf The buffer containing the array to process
 *
 * @param size Size of the given buffer
 *
 * @return Returns number of free bytes available in buffer. This is maximum bytes we can write to buffer
 */
WORD cbufGetEscapedSizeRequired(CIRBUF* pBuf, BYTE* buf, WORD size) {
    WORD i;
    WORD sizeRequired = size;  //Size required for msg. Each bytes that = escape character requires 2 bytes! Must be escaped!

    //Count escape characters
    for (i=size; i!=0; i--) {
        if (buf[i] == CIRBUF_ESC_CHAR) {
            sizeRequired++;
        }
    }
    return sizeRequired;
}


#if defined(CIRBUF_OPTIMIZE_SIZE)
/**
 * Gets a byte from the Buffer, without removing it. The byte is NOT removed from the
 * buffer, and the buffer pointers are NOT updated! To remove the byte, and update the
 * pointers, call the cbufRemoveByte() function.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @preCondition cbufIsEmpty() must have been called to confirm the Buffer has data!
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns the next byte in the given buffer.
 */
BYTE cbufPeekByte(CIRBUF* pBuf) {
    return ( (pBuf)->buf[(pBuf)->get] );
}
#endif


/**
 * Gets a byte at the given offset from the Buffer, without removing it.
 * The byte is NOT removed from the buffer, and the buffer pointers are NOT updated! 
 * The byte at the given offset it returned. The offset is how deep the byte is in
 * the buffer. For example, 0 will return first byte in buffer, 5 will return the 6th
 * byte in the buffer.
 *
 * @preCondition Ensure offset parameter is not larger than current number of bytes
 * contained in buffer. Call cbufGetCount() to get current number of bytes in buffer.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param offset Offset of byte to return. Is a value from 0-n, where n = (cbufGetCount() - 1)
 *
 * @return Returns the byte at the given offset in the given buffer.
 */
BYTE cbufPeekByteAt(CIRBUF* pBuf, WORD offset) {
    WORD ofst;

    ofst = pBuf->get + offset;

    //Check if offset wrap around
    if (ofst > pBuf->maxOffset) {
        ofst = ofst - (pBuf->maxOffset + 1);
    }

    return pBuf->buf[ofst];
}


/**
 * Gets a byte from the Buffer, taking "escape characters" into account. The returned
 * byte is NOT removed from the buffer, and the buffer pointers are NOT updated!
 * To remove the byte, and update the pointers, call the cbufRemoveByte() function.
 *
 * If the first byte in the buffer is an "escape character", the second byte is returned.
 * - If this byte is another "escape character", it represents a single character with
 *   the same value as the "escape character". This function returns 0, and the read
 *   character is returned via the "b" parameter
 * - If this byte is NOT the "escape character", it is a "control character".  This
 *   function returns 1, and the control character is returned via the "b" parameter
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @preCondition cbufGetCount() must have been called to confirm the buffer has AT
 * LEAST 2 bytes available to read!
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param b Pointer to BYTE for returned character. This function will place the returned
 * character in the byte pointed to by this parameter.
 *
 * @return Returns 0 if a normal byte is returned, else 1 if a "control character"
 * is returned.
 */
BYTE cbufPeekEscapedByte(CIRBUF* pBuf, BYTE* b) {
    BYTE c;

    c = cbufPeekByte_MACRO(pBuf);
    if (c == CIRBUF_ESC_CHAR) {
        //Get byte following "Escape Character"
        *b = cbufPeekByteAt(pBuf, 1);

        //If next byte is another "Escape Character", it is NOT a "control character"!
        if (*b == CIRBUF_ESC_CHAR)
            return 0;   //Returned byte is a normal byte
        return 1;       //Returned byte is a "control character"
    }

    *b = c;
    return 0;
}


#if defined(CIRBUF_OPTIMIZE_SIZE)
void cbufRemoveByte(CIRBUF* pBuf) {
    (pBuf)->get = (((pBuf)->get==(pBuf)->maxOffset) ? 0 : ((pBuf)->get+1));
}
#endif


void cbufRemoveBytes(CIRBUF* pBuf, WORD n) {
    WORD getNew;

    //Use this line in Macro if required
    //pBuf->get = ((pBuf->get + n) > pBuf->maxOffset) ? ((pBuf->get + n) - pBuf->maxOffset - 1) : (pBuf->get + n);
        
    getNew = pBuf->get + n;

    if (getNew > pBuf->maxOffset) {
        getNew = getNew - (pBuf->maxOffset + 1);
    }

    pBuf->get = getNew;
}


void cbufRemovePutByte(CIRBUF* pBuf) {
    if (pBuf->put == 0) {
        pBuf->put = pBuf->maxOffset;
    }
    else {
        pBuf->put = (pBuf->put - 1);
    }
}


/**
 * Gets a byte from the Buffer, and removes it from buffer.
 *
 * @preCondition cbufIsEmpty() must have been called to confirm the buffer has data!
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns the next byte in the given buffer.
 */
BYTE cbufGetByte(CIRBUF* pBuf) {
    //Get byte at current GET pointer
    BYTE ret = (pBuf->buf[ pBuf->get ]);

    //Remove last byte in buffer
    pBuf->get = ((pBuf->get==pBuf->maxOffset) ? 0 : (pBuf->get+1));

    return ret;
}


/**
 * Gets and removes a byte from the Buffer, taking "escape characters" into account.
 * If the read character is an "escape character", a second byte is also read.
 * - If this byte is another "escape character", it represents a single character with
 *   the same value as the "escape character". This function returns 0, and the read
 *   character is returned via the "b" parameter
 * - If this byte is NOT the "escape character", it is a "control character".  This
 *   function returns 1, and the control character is returned via the "b" parameter
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @preCondition cbufGetCount() must have been called to confirm the buffer has AT
 * LEAST 2 bytes available to read!
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param b Pointer to BYTE for returned character. This function will place the returned
 * character in the byte pointed to by this parameter.
 *
 * @return Returns 0 if a normal byte is returned, else 1 if a "control character"
 * is returned.
 */
BYTE cbufGetEscapedByte(CIRBUF* pBuf, BYTE* b) {
    BYTE c;

    c = cbufGetByte(pBuf);
    if (c == CIRBUF_ESC_CHAR) {
        *b = cbufGetByte(pBuf);

        //If next byte is another "Escape Character", it is NOT a "control character"!
        if (*b == CIRBUF_ESC_CHAR)
            return 0;   //Returned byte is a normal byte
        return 1;       //Returned byte is a "Control Character"
    }

    *b = c;
    return 0;   //Returned byte is a normal byte
}


/**
 * Copies given number of bytes to BYTE array, remove them from source "Circular Buffer", and
 * returns actual number of bytes copied.
 * If returned value is less than given size, all bytes were copies from source, and
 * it didn't have enough bytes.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to source CIRBUF structure, that data is copied from
 * @param pDstArr Pointer to destination BYTE array
 * @param size Maximum number of bytes to copy. Could be less if pBuf contains less
 *
 * @return Returns number of bytes added to array.
 */
WORD cbufGetArray(CIRBUF* pBuf, BYTE* pDstArr, WORD size) {
    WORD sizeCtgs;          //Size of block of contiguous data we can write to
    BYTE* pGetCtgs;         //Pointer to block of contiguous data we can write to
    WORD sizeRemaining;

    sizeRemaining = size;

    //Get pointer and size of first continuous block of data
    pGetCtgs = cbufGetRdArr(pBuf);
    sizeCtgs = cbufGetRdArrSize(pBuf);

    //Do not write more than requested
    if (sizeCtgs > sizeRemaining) {
        sizeCtgs = sizeRemaining;
    }
    sizeRemaining -= sizeCtgs;  //Update size to indicate how many bytes still have to be written

    //Copy from source to destination array
    //IMPORTANT!!! pDstArr and pGetCtgs are both incremented by given size (sizeCtgs) !!! Because nzMemCpy_ASM_RRR is a macro.
    if (sizeCtgs > 0) {
        //Macro version. !!!! IMPORTANT !!!
        // - Ensure size IS NOT 0
        // - Note dst and src will be incremented by this macro!
        nzMemCpy_ASM_RRR(pDstArr, pGetCtgs, sizeCtgs);

        //Alternative using nzMemCpyNoCheck() or memcpy. Will require "pDstArr += sizeCtgs;" and "pGetCtgs += sizeCtgs;"
        //nzMemCpyNoCheck(pDstArr, pGetCtgs, sizeCtgs);
        //memcpy(pDstArr, pGetCtgs, sizeCtgs);
        //pDstArr += sizeCtgs;      //Not required, is a side affect of nzMemCpy_ASM_RRR
        //pGetCtgs += sizeCtgs;     //Not required, is a side affect of nzMemCpy_ASM_RRR

        //Remove bytes from source buffer
        cbufRemoveBytes(pBuf, sizeCtgs);
    }

    // - Ensure buffer not empty (requested size could have been more than buffer had)
    // - Check if there is a second contiguous block of data. If so, copy bytes from it
    // - If no second contigous block, return bytes copied so far
    if ( (sizeRemaining > 0) && cbufHasData(pBuf) ) {

        //If there are any bytes remaining, they must be located in the second contiguous block. This
        //is from beginning of buffer till PUT
        //sizeCtgs = cbufGetCount(pBuf) - sizeCtgs;
        sizeCtgs = (pBuf)->put; //Don't subtract 1, GET can go up to PUT = buffer empty
        pGetCtgs = pBuf->buf;   //Second contiguous block is from beginning of buffer

        //Do not write more than requested
        if (sizeCtgs > sizeRemaining) {
            sizeCtgs = sizeRemaining;
        }
        sizeRemaining -= sizeCtgs;  //Update size to indicate how many bytes still have to be written

        //Copy from source to destination array
        //IMPORTANT!!! pDstArr and pGetCtgs are both incremented by given size (sizeCtgs) !!! Because nzMemCpy_ASM_RRR is a macro.
        if (sizeCtgs > 0) {
            //Macro version. !!!! IMPORTANT !!!
            // - Ensure size IS NOT 0
            // - Note dst and src will be incremented by this macro!
            nzMemCpy_ASM_RRR(pDstArr, pGetCtgs, sizeCtgs);

            //Alternative using nzMemCpyNoCheck() or memcpy.
            //nzMemCpyNoCheck(pDstArr, pGetCtgs, sizeCtgs);
            //memcpy(pDstArr, pGetCtgs, sizeCtgs);

            //Remove bytes from source buffer
            cbufRemoveBytes(pBuf, sizeCtgs);
        }
    }

    //Return number of bytes written
    return size - sizeRemaining;
}



/**
 * Copies bytes from given source "Circular Buffer" (and removed them) to destination
 * "Byte Array" until:
 * - A byte with value given in "value" parameter is found (matching byte ALSO COPIED)
 * - End of source "Circular Buffer" reached
 * The actual number of bytes copied is returned.
 *
 * These are the ONLY conditions under which this functions removes bytes from the
 * source "Circular Buffer" (as given by return value)!
 *
 * The following will return -1, and not remove anything from source "Circular Buffer":
 * - Given "size" is exceeded
 * - Given byte ("value") was not found
 * - End of source "Circular Buffer" not reached
 * <br>!!!! IMPORTANT !!!!<br>
 * A side affect is that bytes will still be copied to given destination "Byte Array" (pDstArr),
 * but nothing is removed from source "Circular Buffer"!
 *
 * Some examples:
 * <br>If the string is "Name=Value", and we call cbufGetArrayTillByte(pBuf, pDstArr, '=', 20):
 * <br>- The string "Name=" (5 bytes) will be removed from pBuf, and copied to pDstArr. Returns 5
 *
 * <br>If the string is "Name=Value", and we call cbufGetArrayTillByte(pBuf, pDstArr, '=', 3):
 * <br>- NOTHING is removed from pBuf! The string "Nam" (3 bytes) is however still copied to pDstArr. Returns -1
 *
 * <br>If the string is "Value", and we call cbufGetArrayTillByte(pBuf, pDstArr, '&', 20):
 * <br>- The string "Value" (5 bytes) will be removed from pBuf, and copied to pDstArr. Returns 5
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to source CIRBUF structure, that data is copied from
 * @param pDstArr Pointer to destination BYTE array
 * @param value Byte to search for
 * @param size Maximum number of bytes to copy to pDstArr.
 *
 * @return Returns number of bytes copied to destination array and removed from source buffer, or -1 if
 * nothing removed. NOTE that if -1 returned, bytes are still copied to destination array, but nothing is
 * removed from source buffer
 */
WORD cbufGetArrayTillByte(CIRBUF* pBuf, BYTE* pDstArr, BYTE value, WORD size) {
    WORD sizeCtgs;              //Size of block of contiguous data we can write to
    BYTE* pGetCtgs;             //Pointer to block of contiguous data we can write to
    BYTE* ptr;
    WORD sizeRemaining;         //Maximum number of bytes we are allowed to copy to destination array
    WORD i;
    BYTE loopCount=0;
    WORD sizeToCopy;

    #define BYTE_FOUND 2

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ncbufGetArrayTillByte()");

    sizeRemaining = size;

    //Get pointer and size of first continuous block of data
    pGetCtgs = cbufGetRdArr(pBuf);
    sizeCtgs = cbufGetRdArrSize(pBuf);

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nBuf Free=");  //Loop 1
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cbufGetFree(pBuf));  //Loop 1
    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Avail=");  //Loop 1
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cbufGetCount(pBuf));  //Loop 1
    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " MO=");  //Loop 1
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, pBuf->maxOffset);  //Loop 1
    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " P=");  //Loop 1
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, pBuf->put);  //Loop 1
    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " G=");  //Loop 1
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, pBuf->get);  //Loop 1
    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " SzParm=");  //Loop 1
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, size);  //Loop 1

    while(1) {
        //#if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
        //if (loopCount==0) {
        //    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nL1 SCtgs=");  //Loop 1
        //    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, sizeCtgs);  //Loop 1
        //    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " SRmng=");  //Loop 1
        //    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, sizeRemaining);  //Loop 1
        //}
        //else {
        //    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nL2 SCtgs=");  //Loop 1
        //    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, sizeCtgs);  //Loop 1");  //Loop 2
        //    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " SRmng=");  //Loop 1
        //    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, sizeRemaining);  //Loop 1
        //}
        //#endif

        sizeToCopy = min(sizeCtgs, sizeRemaining);

        //Search current block of contiguous data for given byte. If found, update sizeCtgs to point up till this byte
        ptr = pGetCtgs;
        for (i=0; i<sizeToCopy; i++) {
            //Is current byte = value. NOTE!! Use *ptr++ and not pGetCtgs[i], seeing that *pGetCtgs++ generates smaller, faster code!
            if (*ptr++ == value) {
                sizeToCopy = i+1;       //Update size of block we must copy. Include current found byte.
                loopCount = BYTE_FOUND; //Indicase byte found!
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nFound");  //Loop 2
                break;
            }
        }

        //Copy from source to destination array
        //IMPORTANT!!! pDstArr and pGetCtgs are both incremented by given size (sizeCtgs) !!! Because nzMemCpy_ASM_RRR is a macro.
        if (sizeToCopy > 0) {
            //Macro version. !!!! IMPORTANT !!!
            // - Ensure size IS NOT 0
            // - Note dst and src will be incremented by this macro!
            nzMemCpy_ASM_RRR(pDstArr, pGetCtgs, sizeToCopy);
            //pDstArr += sizeCtgs;       //Not required, is a side affect of nzMemCpy_ASM_RRR
            //pGetCtgs += sizeCtgs;   //Not required, is a side affect of nzMemCpy_ASM_RRR

            //Alternative using nzMemCpy() or memcpy. Will require "pDstArr += sizeCtgs;" and "pGetCtgs += sizeCtgs;"
            //nzMemCpy(pDstArr, pGetCtgs, sizeCtgs);
            //memcpy(pDstArr, pGetCtgs, sizeCtgs);
            //pDstArr += sizeCtgs;
            //pGetCtgs += sizeCtgs;
        }

        sizeRemaining -= sizeToCopy;    //Update size to indicate how many bytes still have to be written

        //Check if:
        // - Byte not found yet (loopCount will be 0)
        // - There is a second contiguous block of data. If so, copy bytes from it
        // - If no second contigous block, return bytes copied so far
        if ((loopCount==0) && (sizeRemaining > 0)) {
            loopCount++;
            //If there are any bytes remaining, they must be located in the second contiguous block.
            sizeCtgs = cbufGetCount(pBuf) - sizeCtgs;
            pGetCtgs = pBuf->buf;   //Second contiguous block is from beginning of buffer
        }
        else {
            break;
        }
    }

    //If Given size was reached -AND- byte not found -AND- data remaining in buffer we didn't search yet
    if ((sizeRemaining==0) && (loopCount!=BYTE_FOUND) && (sizeCtgs!=sizeToCopy)) {
       //If we searched till end of
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nRet -1");
        return -1;
    }

    //Remove bytes from source buffer
    cbufRemoveBytes(pBuf, size - sizeRemaining);

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nRmvd ");
    //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, size - sizeRemaining);

    //Return number of bytes written
    return size - sizeRemaining;
}


/**
 * Gets number of contiguous bytes that can be read from buffer. Bytes are NOT removed! Call cbufRemoveBytes(n) to remove
 * bytes if required. Bytes can be read from cirbuf.buf[ cirbuf.get ]. Call cbufHasData() to see if more bytes available
 * when done. Note that if available bytes cross buffer end boundry, there will be 2 congiguous blocks of data.
 *
 * !!!! IMPORTANT !!!!
 * Remember to call cbufRemoveBytes(n) to remove bytes if required.
 *
 * For Eexample:
 *  WORD size, byteWritten;
 *  //Get number of bytes available for reading
 *  size = cbufGetRdArrSize(&cirbuf);
 *  //Call some function that requires an array and size as input parameters
 *  byteWritten = usbPutArr( cbufGetRdArr(&cirbuf), size);
 *  //Now remove bytes from buffer
 *  cbufRemoveBytes(&cirbuf, byteWritten);
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Number of bytes available in returned buffer
 */
WORD cbufGetRdArrSize(CIRBUF* pBuf) {
    WORD putCpy, getCpy;

    //Thread save, get copies of GET and PUT, and only use them in this function.
    NZ_BUILTIN_DISI(0x3FFF); // Disable interrupts, excluding level 7
    putCpy = (pBuf)->put;
    getCpy = (pBuf)->get;
    NZ_BUILTIN_DISI(0x0000); // Enable interrupts

    return ((putCpy < getCpy)
            ? (((pBuf)->maxOffset + 1) - getCpy)
            : (putCpy - getCpy));
}


BYTE cbufPutByte(CIRBUF* pBuf, BYTE b) {
    if (cbufIsFull_MACRO(pBuf)) {
        return 0;
    }

    // Write byte to buf
    pBuf->buf[(pBuf)->put] = b;

    (pBuf)->put = (((pBuf)->put==(pBuf)->maxOffset) ? 0 : ((pBuf)->put+1));

    return 1;
}

#if !defined(CIRBUF_USE_PUTBYTE_FOR_PUTBYTENOCHECK)
#if defined(CIRBUF_OPTIMIZE_SIZE)
void cbufPutByteNoCheck(CIRBUF* pBuf, BYTE b) {

    // Write byte to buf
    pBuf->buf[(pBuf)->put] = b;

    (pBuf)->put = (((pBuf)->put==(pBuf)->maxOffset) ? 0 : ((pBuf)->put+1));
}
#endif
#endif


/**
 * Adds a WORD to the buffer, and updates the buffer pointers.
 * This function checks given buffer has enough space(2 bytes) to add WORD.
 * If the destination buffer has enough space, 2 bytes will always be added
 * to the buffer, first the LSB followed by the MSB.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param w Word to add to the buffer
 *
 * @return Returns number of bytes added to buffer, will be 0 or 2.
 *         Returns 0 if buffer did not have enough space to add given word.
 *         In this case, nothing is added to the buffer.
 */
BYTE cbufPutWord(CIRBUF* pBuf, WORD w) {
    if (cbufGetFree_MACRO(pBuf)>=2) {
        cbufPutByteNoCheck(pBuf, nzWordGetLowByte_MACRO(w));
        cbufPutByteNoCheck(pBuf, nzWordGetHighByte_MACRO(w));
        return 2;
    }
    return 0;

//    #if defined(CIRBUF_OPTIMIZE_SIZE)
//    return cbufPutArray(pBuf, ((const BYTE*)(&w)), 2);
//    #else
//    if (cbufGetFree_MACRO(pBuf)>=2) {
//        cbufPutByteNoCheck(pBuf, nzWordGetLowByte_MACRO(w));
//        cbufPutByteNoCheck(pBuf, nzWordGetHighByte_MACRO(w));
//    }
//    return 0;
//    #endif
}


#if !defined(CBUF_DISABLE_ASCII_BYTE_FUNCTIONS)
/**
 * Adds a BYTE in decimal format to the buffer, and updates the buffer pointers.
 * For example, cbufPutByteAscii(100) will add the string "100" to the buffer.
 * The NULL terminator is NOT included.
 * This function checks given buffer has enough space to add string.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param b Byte to add to the buffer
 *
 * @return Returns number of bytes added to buffer.
 *         Zero is returned if buffer did not have enough space to add string. In this case,
 *         nothing is added to the buffer.
 */
BYTE cbufPutByteAscii(CIRBUF* pBuf, BYTE b)  {
    char buf[6];
    nzUintToAsciiStr(b, (BYTE*)buf);
    return cbufPutString(pBuf, buf);
}


/**
 * Adds given BYTE as a "2-Character Uppercase ASCII Hex Formatted String" to the
 * buffer, and updates the buffer pointers.
 * For example, cbufPutByteAscii(0x5A) will add the string "5A" to the buffer. The
 * NULL terminator is NOT included.
 * This function checks given buffer has enough space to add string.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param b Byte to add to the buffer
 *
 * @return Returns number of bytes added to buffer, will be 0 or 2.
 *         Zero is returned if buffer did not have enough space to add string. In this case,
 *         nothing is added to the buffer.
 */
BYTE cbufPutByteAsciiHex(CIRBUF* pBuf, BYTE b)  {
    if (cbufGetFree_MACRO(pBuf)>=2) {
        cbufPutByteNoCheck(pBuf, nzHighNibbleToAsciiHex(b));
        cbufPutByteNoCheck(pBuf, nzLowNibbleToAsciiHex(b));
        return 2;
    }
    return 0;   //ERROR, nothing written

//    #if defined(CIRBUF_OPTIMIZE_SIZE)
//    WORD_VAL w;
//    w = nzByteToAsciiHex(b);
//    return cbufPutArray(pBuf, ((const BYTE*)(&w.byte.LB)), 2);
//    #else
//    if (cbufGetFree_MACRO(pBuf)>=2) {
//        cbufPutByteNoCheck(pBuf, nzHighNibbleToAsciiHex(b));
//        cbufPutByteNoCheck(pBuf, nzLowNibbleToAsciiHex(b));
//        return 2;
//    }
//    return 0;
//    #endif
}
#endif  //#if !defined(CBUF_DISABLE_ASCII_BYTE_FUNCTIONS)


#if !defined(CBUF_DISABLE_ASCII_WORD_FUNCTIONS)
/**
 * Adds a WORD in decimal format to the buffer, and updates the buffer pointers.
 * For example, cbufPutByteAscii(100) will add the string "100" to the buffer. The
 * NULL terminator is NOT included.
 * This function checks given buffer has enough space to add string.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param w Word to add to the buffer
 *
 * @return Returns number of bytes added to buffer.
 *         Zero is returned if buffer did not have enough space to add string. In this case,
 *         nothing is added to the buffer.
 */
BYTE cbufPutWordAscii(CIRBUF* pBuf, WORD w)  {
    char buf[6];
    nzUintToAsciiStr(w, (BYTE*)buf);
    return cbufPutString(pBuf, buf);
}


/**
 * Adds given WORD as a "4-Character Uppercase ASCII Hex Formatted String" to the
 * buffer, and updates the buffer pointers.
 * For example, cbufPutByteAscii(0x065A) will add the string "065A" to the buffer. The
 * NULL terminator is NOT included.
 * This function checks given buffer has enough space to add string.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param w Byte to add to the buffer
 *
 * @return Returns number of bytes added to buffer, will be 0 or 4.
 *         Zero is returned if buffer did not have enough space to add string. In this case,
 *         nothing is added to the buffer.
 */
BYTE cbufPutWordAsciiHex(CIRBUF* pBuf, WORD w) {
    #if defined(CIRBUF_OPTIMIZE_SIZE)
    char cBuf[4];
    cBuf[0] = nzHighNibbleToAsciiHex(nzWordGetHighByte_MACRO(w));
    cBuf[1] = nzLowNibbleToAsciiHex(nzWordGetHighByte_MACRO(w));
    cBuf[2] = nzHighNibbleToAsciiHex(nzWordGetLowByte_MACRO(w));
    cBuf[3] = nzLowNibbleToAsciiHex(nzWordGetLowByte_MACRO(w));
    return cbufPutArray(pBuf, (BYTE*)cBuf, 4);
    #else
    if (cbufGetFree_MACRO(pBuf)>=4) {
        cbufPutByteNoCheck(pBuf, nzHighNibbleToAsciiHex(nzWordGetHighByte_MACRO(w)));
        cbufPutByteNoCheck(pBuf, nzLowNibbleToAsciiHex(nzWordGetHighByte_MACRO(w)));
        cbufPutByteNoCheck(pBuf, nzHighNibbleToAsciiHex(nzWordGetLowByte_MACRO(w)));
        cbufPutByteNoCheck(pBuf, nzLowNibbleToAsciiHex(nzWordGetLowByte_MACRO(w)));
        return 4;
    }
    return 0;   //ERROR, nothing written
    #endif
}
#endif  //#if !defined(CBUF_DISABLE_ASCII_WORD_FUNCTIONS)


#if !defined(CBUF_DISABLE_SHADDOW_FUNCTIONS)
/**
 * Adds a byte to the buffer, using supplied PUT pointer. Returns updated PUT pointer.
 * The actual PUT pointer of buffer is NOT modified, meaning data is added to buffer,
 * but it will NOT be available for reading by any other threads until commited. Use
 * this function to create thread safe code. Is required when writing data to buffer, that
 * might get changed before done.
 *
 * For example, when writing a packet, we migth update the packet size (first by of packet)
 * only when done, seeing that we do not know the size until finished. In this case, we do
 * not want any other process reading the packet until we are done!
 *
 * Call cbufShaddowPutCommit() function to commit all changes made via this function.
 *
 * @preCondition cbufIsFull() must have been called to confirm the buffer is not full!
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param b Byte to add to the buffer
 *
 * @param putShaddow Is the shaddow PUT pointer
 *
 * @return Returns the updated shaddow PUT pointer.
 */
BYTE cbufShaddowPutByte(CIRBUF* pBuf, BYTE b, WORD putShaddow) {
    // Write byte to buf
    pBuf->buf[putShaddow] = b;

    return ((putShaddow==(pBuf)->maxOffset) ? 0 : (putShaddow+1));
}


/**
 * Commit shaddow PUT pointer.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param putShaddow Shaddow PUT pointer to commit
 */
void cbufShaddowPutCommit(CIRBUF* pBuf, WORD putShaddow) {
    pBuf->put = putShaddow;
}
#endif  //#if !defined(CBUF_DISABLE_SHADDOW_FUNCTIONS)


/**
 * Add an "Escape Character" and given byte to buffer. The default escape character
 * is '^'. This function is used to add special control characters to the buffer.
 *
 * @preCondition buffer must have space, this function does NOT check if there is space
 * in the buffer! Use cbufGetFree() to get free space for writing.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param b Byte to add to the buffer
 * 
 * @return Returns 0 if ERROR, else number of bytes added to buffer. Will
 *         be 2 if successfull (escape character and given byte).
 */
BYTE cbufPutControlChar(CIRBUF* pBuf, BYTE b) {
    #if defined(CIRBUF_OPTIMIZE_SIZE)
    WORD_VAL w;
    w.byte.LB = CIRBUF_ESC_CHAR;
    w.byte.HB = b;
    return cbufPutWord(pBuf, w.Val);
    #else
    if (cbufGetFree_MACRO(pBuf)>=2) {
        cbufPutByteNoCheck(pBuf, CIRBUF_ESC_CHAR);
        cbufPutByteNoCheck(pBuf, b);
        return 2;
    }
    return 0;
    #endif
}


/**
 * Add the given byte to the buffer. If it is equal to the "Escape Character", it has to be
 * preceded by an additional escape characters(escaped). The default escape character
 * is '^'. When reading back the buyte with cbufGetEscapedByte, the original bytes will be
 * read (excluding any escape characters that might have been added).
 *
 * @preCondition buffer must have space, this function does NOT check if there is space
 * in the buffer! Use cbufGetFree() to get free space for writing.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param b Byte to add to the buffer
 *
 * @return Returns 0 if ERROR, else number of bytes added to buffer.<br>
 *         Returns 2 if given byte was the "Escape Character" (had to be escaped with
 *         additional character)<br>
 *         Returns 1 if single byte added to buffer.
 */
BYTE cbufPutEscapedByte(CIRBUF* pBuf, BYTE b) {
    BYTE ret = 1;

    //Is given byte the "Escape Character"?
    if (b == CIRBUF_ESC_CHAR) {
        ret++;
    }

    //Is the enough space in buffer
    if (cbufGetFree_MACRO(pBuf) < ret) {
        return 0;   //ERROR
    }

    //If given byte is the "Escape Character", need to put an "Escape Charcter" in front of it
    if (ret == 2) {
        cbufPutByteNoCheck(pBuf, CIRBUF_ESC_CHAR);
    }

    //Put given character
    cbufPutByteNoCheck(pBuf, b);

    return ret;
}


/**
 * Adds given NULL terminated string to buffer, and updates the buffer pointers.
 * The NULL terminator is NOT included (not written to buffer!)
 * This function checks given buffer has enough space to add string.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param s Null terminated string to add
 *
 * @return Returns number of bytes added to buffer.
 *         Zero is returned if buffer did not have enough space to add string. In this case,
 *         nothing is added to the buffer.
 */
WORD cbufPutString(CIRBUF* pBuf, const char *s) {
    WORD strLen;
    char* p;

    //Get string length
    p = (char*)s;
    strLen = 0;
    while (*p++ != 0) {
        strLen++;
    }

    //If string too long for buffer, return 0
    if (strLen > cbufGetFree(pBuf)) {
        return 0;
    }

    return cbufPutArray(pBuf, (BYTE*)s, strLen);
}


/**
 * Adds given source buffer to given destination buffer.
 *
 * The source must be an ASCII String, and can contain Escape Sequences.<br>
 * The destination is a "Circular Buffer", and must be in "Binary Format, with Escape Sequence".
 * It can be a normal (Streaming) or "Packet" type "Circular Buffer".<br>
 * This function will decoded the source, and add it to the destination as binary data.
 *
 * For details on formats, see @ref nz_circularBuffer_format_ascii_esc "ASCII Format, with Escape Sequences",
 * and @ref nz_circularBuffer_format_bin_esc "Binary Format, with Escape Sequence".
 * For details on "Circular Buffer" types, see @ref nz_circularBuffer_type_streaming "Circular Buffers"
 * and @ref nz_circularBuffer_type_packet "Packet Type Circular Buffers".
 *
 * The source can be a String, "Circular Buffer" or "Packet Type Circular Buffer"
 * - <b>String:</b> Give string in the pStrSrc parameter, and NULL in cbufSrc parameter. The string
 *   must be NULL terminated!
 * - <b>Circular Buffer:</b> Give "Circular Buffer" in the cbufSrc parameter, and NULL in pStrSrc parameter.
 *   The "Circular Buffer" must contain a NULL terminated string.
 * - <b>Packet Type Circular Buffer:</b> Give "Circular Buffer" in the cbufSrc parameter, and NULL in pStrSrc
 *   parameter. If the packet contains a NULL terminated string, it is added. Else, the first packet is added to
 *   the destination.
 *
 * If destination "Circular Buffer" did not have enough space to add data:
 *  - NOTHING is added to the destination "Circular Buffer".
 *  - NOTHING is removed from the source "Circular Buffer" (if source is a "Circular Buffer").
 *
 * Else:
 *  - All processed bytes are removed from the source "Circular Buffer" (if source is a "Circular Buffer").
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @preCondition If source is a Packet type "Circular Buffer", it MUST contain a whole packet.
 *
 * @param cbufDst Is the destination "Circular Buffer"
 *
 * @param cbufSrc If not NULL, is pointer to source "Circular Buffer".
 * - If Packet type, all bytes from next packet are processed and added to destination.
 * - Else all bytes till NULL character (end of string) are processed and added to destination.
 *
 * @param pStrSrc If cbufSrc=NULL, this must contain the source as a NULL terminated string.
 *
 * @param flags A combination of ASCIIESC_XXX flags that add aditional functionality:
 *  - ASCIIESC_ADD_S_AND_P: Adds a 's' and 'p' control character to front and back of destination.
 *
 * @return Returns number of bytes added to buffer.
 *         Zero is returned if destination "Circular Buffer" did not have enough space to add
 *         string. In this case, nothing is added to the buffer.
 */
WORD cbufPutAsciiEscString(CIRBUF* cbufDst, CIRBUF* cbufSrc, const char* pStrSrc, WORD flags) {
    BYTE c;
    WORD sizeRdArr;
    BYTE msb=0;
    SHORT putAvailable;         //Space in destination buffer
    SHORT putAvailableOriginal; //Space in destination buffer
    WORD sizeSecondRdArr = 0;
    WORD bytesToRemove;         //Bytes to remove from source "Circular Buffer" if successfull
    WORD putShaddow;
    #if !defined(CIRBUF_DISABLE_PACKETS)    //Packets ARE supported
    BYTE skipPacketSizeBytes = 0;
    #endif

    union {
        struct {
            unsigned char inSq : 1;         //We are inside a "Quoted String"
            unsigned char sqFoundInSq : 1;  //Single quote found inside "Quoted String"
            unsigned char firstHexChar : 1; //First hex character found
            unsigned char putEsc : 1;       //Put a backslash character
        } bits;
        BYTE val;
    } flags2;
    flags2.val = 0;

    sizeRdArr = 0xffff;

    //Get current PUT pointer
    putShaddow = cbufDst->put;

    //Check destination is "Circular Buffer" format is "Binary, with Escape Sequence Processing"
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
    if (cbufGetFormat(cbufDst) != BUFFORMAT_BIN_ESC) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\ncbufPAECStr NOT BinEsc!");
        return 0;
    }
    #endif

    //Check if the destination is a Packets, and this is NOT supported
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
        #if !defined(CIRBUF_DISABLE_PACKETS) && !defined(CIRBUF_PUTASCIIESC_SUPPORT_DST_PACKETS)
        if (cbufIsPacket(cbufDst)) {
            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nbufPAECStr Dst Pct NOT supported!");
            return 0;
        }
        #endif
    #endif

    //Source is a "Circular Buffer"
    if (cbufSrc != 0) {
        WORD totalSize; //Total size. packet size (size byte(s) AND data)

        #if !defined(CIRBUF_DISABLE_PACKETS)    //Packets ARE supported
        //Source is a Packet Type "Circular Buffer"
        if(cbufIsPacket(cbufSrc)) {
            checkIfDummyAndRemove(cbufSrc);
            skipPacketSizeBytes = cbufIsPacketLarge(cbufSrc) ? 2 : 1;   //Get number of size bytes, is 1 or 2
            totalSize = cbufPeekPacketDataSize(cbufSrc);   //Get packet Data size
            totalSize += skipPacketSizeBytes;              //Add "packet size byte(s)"
        }
        else
        #endif  //#if !defined(CIRBUF_DISABLE_PACKETS)
        {
            totalSize = cbufGetCount(cbufSrc);
        }

        //Get pointer and size of first "contiguous block" of data
        pStrSrc = (const char*)cbufGetRdArr(cbufSrc);
        sizeRdArr = cbufGetRdArrSize(cbufSrc);

        //Source is a Packet Type "Circular Buffer".
        //Limit "RdArr" size to size of first packet
        #if !defined(CIRBUF_DISABLE_PACKETS)    //Packets ARE supported
        if(cbufIsPacket(cbufSrc)) {
            if (sizeRdArr > totalSize) {
                sizeRdArr = totalSize;
            }
        }
        #endif  //#if !defined(CIRBUF_DISABLE_PACKETS)

        //Check if there is a second contiguous block of "read array" data
        if (totalSize > sizeRdArr) {
            sizeSecondRdArr = totalSize - sizeRdArr;
        }

        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nsize = ");
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, sizeRdArr);
        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ' ');
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, sizeSecondRdArr);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, ", GET = ");
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cbufSrc->get);
    }
    //Both cbufSrc AND pStrSrc are 0 - ERROR!
    else if (pStrSrc==0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufPAECStr NULL");
        return 0;   //Nothing added to destination, or removed from source buffers
    }

    //Get available space in Tx buffer
    putAvailable = putAvailableOriginal = cbufGetFree(cbufDst);

    //The destination is a Packet Type "Circular Buffer". Add given string as a packet
    #if !defined(CIRBUF_DISABLE_PACKETS) && defined(CIRBUF_PUTASCIIESC_SUPPORT_DST_PACKETS) //Packets ARE supported
    if (cbufIsPacket(cbufDst)) {
        #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)    //Contiguous packets can have dummy packet to fill end of buffer
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\ncbufPAECStr Does NOT impl Contiguous Pct");

        //Currently this functions DOES not implement writing all packets as contiguous packets! This is
        //done by adding a dummy packet to end of buffer if not enough space for whole packet.
        //To implement this, some possibilities are:
        // - Add code that calculates size required for packet before adding it. Check if enough space at end
        //   of buffer. If not, add dummy packet, and add packet with data to beginning of buffer
        // - Use source string size as estimate size for required packet.
        #endif  //#if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)

        //Put size of Packet. Can not know for sure at this stage, will change when all done
        if (putAvailable-- > 0)
            putShaddow = cbufShaddowPutByte(cbufDst, 0, putShaddow);     //Put byte, without updating buffer pointers! Data added is not available for reading!
        if (cbufIsPacketLarge(cbufDst)) {
            if (putAvailable-- > 0)
                putShaddow = cbufShaddowPutByte(cbufDst, 0, putShaddow);    //Put byte, without updating buffer pointers! Data added is not available for reading!
        }
    }
    #endif  //#if !defined(CIRBUF_DISABLE_PACKETS)

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\ncbufPAECStr = ");

    //Reset before starting to write anything to destination
    bytesToRemove = 0;

    //Check if 's' and 'p' flag must be added to front and back of destination
    if ((flags&ASCIIESC_ADD_S_AND_P) != 0) {
        if (putAvailable >= 2) {
            putShaddow = cbufShaddowPutByte(cbufDst, CIRBUF_ESC_CHAR, putShaddow);  //Put byte, without updating buffer pointers! Data added is not available for reading!
            putShaddow = cbufShaddowPutByte(cbufDst, 's', putShaddow);  //Put byte, without updating buffer pointers! Data added is not available for reading!
        }
        putAvailable -= 2;  //ALWAYS decrement putAvailable! Do NOT put it in if{}, we want it to be negative to indicate error!
    }

    //Read bytes from source
    while(putAvailable > 0) {
        //No more data in current "contiguous block"
        if (sizeRdArr-- == 0) {
            //Is there a second "contiguous block" of data
            if (sizeSecondRdArr) {
                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "(SRA=");
                DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, sizeSecondRdArr);
                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, ')');
                sizeRdArr = sizeSecondRdArr - 1;
                pStrSrc = (const char*)(cbufSrc)->buf;
                sizeSecondRdArr = 0;
            }
            else {
                //There is no second read array
                break;
            }
        }

        //Get next character.
        c = *pStrSrc;
        pStrSrc++;

        //Source is a "Circular Buffer"
        if (cbufSrc != 0) {
            bytesToRemove++;    //Bytes to remove from source "Circular Buffer" if successfull

            //If source is a packet, first remove size bytes (1 for normal, 2 for large packet)
            #if !defined(CIRBUF_DISABLE_PACKETS)    //Packets ARE supported
            if(skipPacketSizeBytes != 0) {
                skipPacketSizeBytes--;
                DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '.');
                continue;
            }
            #endif  //#if !defined(CIRBUF_DISABLE_PACKETS)
        }

        //Source is a NULL terminated string
        if (c==0) {
            break;
        }

        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, c);

        //Process read byte
        //Previous char was a ' inside a '. Can be escape sequence to put a single quote, or end of "Quoted String"
        if (flags2.bits.sqFoundInSq == TRUE) {
            //flags2.bits.dqFoundInDq = FALSE;   //Don't clear, is needed below!

            //It was the end of the "Quoted String". Clear flag. c contains next char, will be processed below
            if (c != '\'') {
                flags2.bits.inSq = FALSE;
                flags2.bits.sqFoundInSq = FALSE;
            }
            //else, it was an escape sequence to put a single quote. Will be processed below in if (flags2.bits.dqFoundInDq == TRUE) {}
        }

        //Second byte of hex character expected
        if (flags2.bits.firstHexChar == TRUE) {
            flags2.bits.firstHexChar = FALSE;

            //Second hex char must be now
            c = nzAsciiHexNibbleToByte(c);

            //If both bytes were not hex encoded characters, ERROR!
            if ((c == 0xff) || (msb == 0xff)) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\ncbufPAECStr Hex Err!");
                return 0;   //Nothing added to destination, or removed from source buffers
            }

            //Get hex number represented by last two hex characters received
            c = ((msb << 4) | c);

            //c already contains correct character (hex code), will be added to buffer below
        }
        //Inside "Quoted String"
        else if (flags2.bits.inSq == TRUE) {
            //Single quote found inside current "Quoted String". Can be escape sequence to put a single single quote,
            //or end of "Quoted String"
            if (c == '\'') {
                //Previous char was a ' inside a '. This is another ' = an escaped single quote (two '' = single '), leave c as is, will be added to buffer below.
                if (flags2.bits.sqFoundInSq == TRUE) {
                    flags2.bits.sqFoundInSq = FALSE;
                    //c already contains correct character ('), will be added to buffer below
                }
                else {
                    flags2.bits.sqFoundInSq=TRUE;
                    continue;               //Don't put c, continue!
                }
            }
            //c already contains correct character, will be added to buffer below
        }
        //First character of two byte hex code
        else if ((c<='F')&&(c>='0')) {
            //This is the MSB of a hex code (in 2 character ASCII HEX format)
            flags2.bits.firstHexChar = TRUE;
            msb = nzAsciiHexNibbleToByte(c);
            continue;               //Don't put first byte of hex code, continue!
        }
        //Found a single quote
        else if (c == '\'') {
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n\' Found");
            flags2.bits.inSq=TRUE;
            continue;               //Don't put ', continue!
        }
        //Found a control character (lower case character)
        else if ((c<='z')&&(c>='a')) {
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nc Found");
            flags2.bits.putEsc=TRUE;  //Cause an "Escape" character to precede the control character

            //We have to write a "escape character" followed by the current "control character" to the TX buffer.
            //c already contains the "control character", and flags2.bits.putEsc will cause a "escape character" to be put before it.
        }
        else {
            //Invalid char, continue
            continue;
        }

        //If it the Escape Character, it must be escaped
        if (c == CIRBUF_ESC_CHAR) {
            flags2.bits.putEsc=TRUE;  //Cause an "Escape" character to precede the control character
        }

        //Must escape character be written to TX buf
        if (flags2.bits.putEsc==TRUE) {
            flags2.bits.putEsc=FALSE;
            if (putAvailable-- > 0) {
                putShaddow = cbufShaddowPutByte(cbufDst, '^', putShaddow);  //Put byte, without updating buffer pointers! Data added is not available for reading!
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nTXint ^");
            }
        }

        //Write current char to tx buffer
        if (putAvailable-- > 0) {
            putShaddow = cbufShaddowPutByte(cbufDst, c, putShaddow);  //Put byte, without updating buffer pointers! Data added is not available for reading!
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nTXint ");
            //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, c);
        }
    }

    //Check if 's' and 'p' flag must be added to front and back of destination
    if ( (flags&ASCIIESC_ADD_S_AND_P) != 0) {
        if (putAvailable >= 2) {
            putShaddow = cbufShaddowPutByte(cbufDst, CIRBUF_ESC_CHAR, putShaddow);  //Put byte, without updating buffer pointers! Data added is not available for reading!
            putShaddow = cbufShaddowPutByte(cbufDst, 'p', putShaddow);  //Put byte, without updating buffer pointers! Data added is not available for reading!
        }
        putAvailable -= 2;  //ALWAYS decrement putAvailable! Do NOT put it in if{}, we want it to be negative to indicate error!
    }

    //If destination did not have enough place
    if (putAvailable < 0) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufPAECStr No Space!");
        return 0;
    }

    //Use sizeRdArr to store bytesWritten, it is not used any more.
    #define bytesWritten sizeRdArr  //Bytes written to destination. If it is a Packet "Circular Buffer", this includes the size byte(s)
    bytesWritten = putAvailableOriginal - putAvailable;

    //Remove bytes read from source. If source is a "Circular Buffer", remove processed bytes
    if (bytesToRemove) {
        //if (cbufIsPacket(cbufSrc)) {
        //    cbufRemovePacket(cbufSrc);
        //}
        cbufRemoveBytes(cbufSrc, bytesToRemove);

        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nRemove ");
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, bytesToRemove);
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, ", GET = ");
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, cbufSrc->get);
    }

    //If destination is a packet buffer, update packet size. At this we have only used Shaddow put functions,
    //so writing to buffer will write use original PUT pointer.
    #if !defined(CIRBUF_DISABLE_PACKETS) && defined(CIRBUF_PUTASCIIESC_SUPPORT_DST_PACKETS) //Packets ARE supported
    if (cbufIsPacket(cbufDst)) {
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUpdating PctSize=");
        DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, bytesWritten);

        bytesWritten--; //Remove size byte written to beginning of packet. We want to know "data bytes written"

        //Put size of Packet. Can not know for sure at this stage, will change when all done
        if (cbufIsPacketLarge(cbufDst)) {
            bytesWritten--; //Remove second size byte written to beginning of packet. We want to know "data bytes written"
            //[Size] part of packet is a 16-bit value = [MSB][LSB]
            cbufPutByte(cbufDst, (BYTE)(bytesWritten>>8));
        }
        cbufPutByte(cbufDst, (BYTE)bytesWritten);
    }
    #endif  //#if !defined(CIRBUF_DISABLE_PACKETS)

    //Commit data added to destination
    cbufShaddowPutCommit(cbufDst, putShaddow);

    return bytesWritten;
}


/**
 * Adds given NULL terminated string to buffer, and updates the buffer pointers. If the
 * buffer is full, this function will wait until space is available in buffer to add string.
 * !!!!! IMPORTANT !!!!!
 * Only call this function for buffers that use ISR to empty buffer. Else, buffer will never
 * get empty if we wait for it!
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param s String to add to the buffer
 *
 * @return Returns number of bytes added to buffer.
 */
WORD cbufPutStringWait(CIRBUF* pBuf, const char *s) {
    WORD i;
    BYTE* p;
    WORD addCount;  //Counts how many bytes were added to buffer

    addCount = 0;

    p = &pBuf->buf[pBuf->put];    //Get put pointer in buffer (where next byte has to be put)

    //Do until end of string reached
    while (*s != 0) {
        //Wait if buffer full
        i = 0;
        while ( cbufIsFull(pBuf) ) {
            if (i++ == 0xffff) {
                return addCount;
            }
        }

        //Add next character to buffer
        *p++ = *s++;
        addCount++;

        //Increment put pointer, or wrap around.
        if (pBuf->put++ == pBuf->maxOffset) {
            pBuf->put = 0;
            p = &pBuf->buf[0];    //Get put pointer in buffer (where next byte has to be put)
        }
    }

    return addCount;    //Return bytes written
}


/**
 * Adds given number of bytes to buffer. This function checks given buffer has enough space.
 * The actual number of bytes written is returned.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param pSrcArr Pointer to source BYTE array
 * @param size Number of bytes to copy
 *
 * @return Returns number of bytes added to buffer.
 */
WORD cbufPutArray(CIRBUF* pBuf, const BYTE* pSrcArr, WORD size) {
    WORD sizeCtgs;
    WORD sizeRemain;
    BYTE* pDstArr;
    WORD getCpy;

    //Nothing to do!
    if (size==0)
        return 0;

    //Thread save, get copy of GET. No need to get copy of PUT, seeing that this function is called from PUT context
    NZ_BUILTIN_DISI(0x3FFF); // Disable interrupts, excluding level 7
    getCpy = pBuf->get;
    NZ_BUILTIN_DISI(0x0000); // Enable interrupts

#define BUF_PUT_ARRAY_USE_MEMCPY
#if defined (BUF_PUT_ARRAY_USE_MEMCPY)
    //Get number of contiguous bytes that can be put in buffer.
    // - GET > PUT, it is till PUT pointer = GET - (PUT + 1)
    if (getCpy > pBuf->put) {
        //There are following number of contiguous bytes: GET - (PUT + 1)
        sizeCtgs = (getCpy - (pBuf->put + 1));
    }
    // - GET <= PUT, then it is till buff array = (maxOffset + 1) - PUT
    else {
        sizeCtgs = (pBuf->maxOffset - pBuf->put);

        //If GET = 0, we can NOT write last location in array. If we do, PUT would wrap arround and also
        //be = 0, and buffer would be empty! So, if GET is not 0, we can put one more byte = sizeCtgs++
        if (getCpy != 0) {
            sizeCtgs++;
        }

        //Buffer is full, no space
        if (sizeCtgs == 0) {
            return 0;
        }
    }

    pDstArr = &pBuf->buf[pBuf->put];    //Get put pointer in buffer (where next byte has to be put)


    /////////////////////////////////////////////////
    //More (or equal) contiguous bytes available then requested size, all bytes can be written at once
    if(sizeCtgs >= size) {
        sizeCtgs = size;          //Adjust to length of requested size

        //Copy given bytes to buffer
        //Macro version. !!!! IMPORTANT - Ensure size IS NOT 0 !!!! This macro does NOT check it!
        nzMemCpy_ASM_RRR(pDstArr, pSrcArr, sizeCtgs);
        //!!!!! IMPORTANT - pDstArr and pSrcArr are incremented by sizeCtgs !!!!!

        //Alternative using nzMemCpyNoCheck() or memcpy.
        //nzMemCpyNoCheck(pDstArr, pSrcArr, sizeCtgs);
        //memcpy((void*)pDstArr, (void*)pSrcArr, sizeCtgs);

        //Update PUT pointer
        pBuf->put += sizeCtgs;
        //If PUT pointer reached end of array, reset to 0
        if (pBuf->put > pBuf->maxOffset) {
            pBuf->put = pBuf->put - (pBuf->maxOffset + 1);
        }

        return size;    //All bytes were written
    }


    /////////////////////////////////////////////////
    //Less contiguous bytes available then requested. In this case:
    // - First write from current PUT pointer to end of pBuf->buf array (if GET=0, till one before end)
    //   ~ If GET=0, this will write up to second last byte in pBuf->buf array. PUT pointer will be = maxOffset,
    //     and we can return. Buffer will be full. Can not write to last byte, because then PUT would wrap
    //     around to also be 0, and buffer would be empty.
    //   ~ If GET!=0, this will write up to last byte in pBuf->buf array, and PUT will increment to 0
    // - Second write from beginning of pBuf->buf array, till one location before GET.

    //Copy given bytes to buffer
    if (sizeCtgs != 0) {
        //Macro version. !!!! IMPORTANT - Ensure size IS NOT 0 !!!! This macro does NOT check it!
        nzMemCpy_ASM_RRR(pDstArr, pSrcArr, sizeCtgs);
        //!!!!! IMPORTANT - pDstArr and pSrcArr are incremented by sizeCtgs !!!!!
        //pSrcArr += sizeCtgs;     //Not required, is a side affect of nzMemCpy_ASM_RRR

        //Alternative using nzMemCpyNoCheck() or memcpy.
        //nzMemCpyNoCheck(pDstArr, pSrcArr, sizeCtgs);
        //memcpy((void*)pDstArr, (void*)pSrcArr, sizeCtgs);
        //pSrcArr += sizeCtgs;      //Update pointer to given data
    }

    //Buffer is full! Write up to second last byte in pBuf->buf array, set PUT to last byte (maxOffset), and exit.
    if (getCpy == 0) {
        pBuf->put = pBuf->maxOffset;
        return sizeCtgs;
    }

    //At this stage we know that PUT should be = 0. Update PUT pointer, will wrap around to beginning of buffer
    //pBuf->put = 0;    //Not required, is updated later

    sizeRemain = size - sizeCtgs; //Get remaining number of bytes that have to be written

    //Get second number of contiguous bytes that can be put in buffer. We now know that PUT = 0, and GET != 0
    sizeCtgs = getCpy - 1;

    //If more bytes available then needed, adjust sizeCtgs to sizeRemain
    if (sizeCtgs > sizeRemain) {
        sizeCtgs = sizeRemain;
    }

    //Now write remaining bytes = size - sizeCtgs. We now have to start at biginning of array
    if (sizeCtgs != 0) {
        pDstArr = &pBuf->buf[0];
        //Macro version. !!!! IMPORTANT - Ensure size IS NOT 0 !!!! This macro does NOT check it!
        nzMemCpy_ASM_RRR(pDstArr, pSrcArr, sizeCtgs);
        //!!!!! IMPORTANT - pDstArr and pSrcArr are incremented by sizeCtgs !!!!!

        //Alternative using nzMemCpyNoCheck() or memcpy.
        //nzMemCpyNoCheck(pDstArr, pSrcArr, sizeCtgs);
        //memcpy((void*)pDstArr, (void*)pSrcArr, sizeCtgs);
    }

    //Update PUT pointer, would have wrapped around to beginning of buffer, and incremented to (size - sizeCtgs)
    pBuf->put = sizeCtgs;

    return ( (size-sizeRemain) + sizeCtgs);    //Return bytes written
#else
    WORD i;

    //Get number of contiguous bytes that can be put in buffer.
    // - GET > PUT, it is till PUT pointer = GET - (PUT + 1)
    if (getCpy > pBuf->put) {
        //There are following number of contiguous bytes: GET - (PUT + 1)
        sizeCtgs = (getCpy - (pBuf->put + 1));
    }
    // - GET <= PUT, then it is till buff array = (maxOffset + 1) - PUT
    else {
        sizeCtgs = (pBuf->maxOffset - pBuf->put);

        //If GET = 0, we can NOT write last location in array. If we do, PUT would wrap arround and also
        //be = 0, and buffer would be empty! So, if GET is not 0, we can put one more byte = sizeCtgs++
        if (getCpy != 0) {
            sizeCtgs++;   //0 bytes added to buffer
        }
    }

    //Buffer is full, no space
    if (sizeCtgs == 0) {
        return 0;
    }

    pDstArr = &pBuf->buf[pBuf->put];    //Get put pointer in buffer (where next byte has to be put)


    /////////////////////////////////////////////////
    //More (or equal) contiguous bytes available then requested size, all bytes can be written at once
    if(sizeCtgs >= size) {
        sizeCtgs = size;          //Adjust to length of requested size

        //Write next byte to buffer
        for (i=sizeCtgs; i!=0; i--) {
            *pDstArr++ = *pSrcArr++;
        }

        //Update PUT pointer
        pBuf->put += sizeCtgs;
        //If PUT pointer reached end of array, reset to 0
        if (pBuf->put > pBuf->maxOffset) {
            pBuf->put = pBuf->put - (pBuf->maxOffset + 1);
        }

        return size;    //All bytes were written
    }


    /////////////////////////////////////////////////
    //Less contiguous bytes available then requested. In this case:
    // - First write from current PUT pointer to end of pBuf->buf array (if GET=0, till one before end)
    //   ~ If GET=0, this will write up to second last byte in pBuf->buf array. PUT pointer will be = maxOffset,
    //     and we can return. Buffer will be full. Can not write to last byte, because then PUT would wrap
    //     around to also be 0, and buffer would be empty.
    //   ~ If GET!=0, this will write up to last byte in pBuf->buf array, and PUT will increment to 0
    // - Second write from beginning of pBuf->buf array, till one location before GET.

    //Write next bytes to buffer.
    for (i=sizeCtgs; i!=0; i--) {
        *pDstArr++ = *pSrcArr++;
    }

    //Buffer is full! Write up to second last byte in pBuf->buf array, set PUT to last byte (maxOffset), and exit.
    if (getCpy == 0) {
        pBuf->put = pBuf->maxOffset;
        return sizeCtgs;
    }

    //At this stage we know that should be PUT = 0. Update PUT pointer, will wrap around to beginning of buffer
    //pBuf->put = 0;    //Not required, is updated later

    sizeRemain = size - sizeCtgs; //Get remaining number of bytes that have to be written

    //Get second number of contiguous bytes that can be put in buffer. We now know that PUT = 0, and GET != 0
    sizeCtgs = getCpy - 1;

    //If more bytes available then needed, adjust sizeCtgs to sizeRemain
    if (sizeCtgs > sizeRemain) {
        sizeCtgs = sizeRemain;
    }

    //Now write remaining bytes = size - sizeCtgs. We now have to start at biginning of array
    pDstArr = &pBuf->buf[0];
    for ( i=sizeCtgs; i!=0; i--) {
        *pDstArr++ = *pSrcArr++;
    }

    //Update PUT pointer, would have wrapped around to beginning of buffer, and incremented to (size - sizeCtgs)
    pBuf->put = sizeCtgs;

    return ( (size-sizeRemain) + sizeCtgs);    //Return bytes written
#endif
}


/**
 * Adds given number of bytes to buffer. If the buffer is full, this function
 * will wait until space is available in buffer to add requested bytes.
 * !!!!! IMPORTANT !!!!!
 * Only call this function for buffers that use ISR to empty buffer. Else, buffer will never
 * get empty if we wait for it!
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param pSrcArr Pointer to source BYTE array
 * @param size Number of bytes to copy
 *
 * @return Returns number of bytes added to buffer.
 *         Zero is returned if buffer did not have enough space. In this case,
 *         nothing is added to the buffer.
 */
WORD cbufPutArrayWait(CIRBUF* pBuf, const BYTE* pSrcArr, WORD size) {
    WORD i;
    BYTE* pDstArr;
    WORD addCount;  //Counts how many bytes were added to buffer

    // TODO - Make this function more efficient! Use contiguous blocks of data like in cbufPutArray()

    addCount = 0;

    pDstArr = &pBuf->buf[pBuf->put];    //Get put pointer to buffer (where next byte has to be put)

    //Do until all bytes have been written
    while (size != addCount) {
        //Wait if buffer full
        i = 0;
        while ( cbufIsFull(pBuf) ) {
            if (i++ == 0xffff) {
                return addCount;
            }
        }

        //Add next BYTE to buffer
        *pDstArr++ = *pSrcArr++;
        addCount++;

        //Increment put pointer, or wrap around.
        if (pBuf->put++ == pBuf->maxOffset) {
            pBuf->put = 0;
            pDstArr = &pBuf->buf[0];    //Get put pointer in buffer (where next byte has to be put)
        }
    }

    return addCount;    //Return bytes written
}


/**
 * Moves as many bytes as possible from given source buffer (BIFCIR), to given destination buffer (CIRBUF).
 * This function checks how many Source bytes are available, and maximum number of bytes
 * that can be added to Destination.
 *
 * !! IMPORTANT !! When using in multi threaded application, ensure function is not interrupted by other tasks
 * that can modify the source or destination buffers!
 *
 * @param pBufSrc Pointer to Source CIRBUF structure
 * @param pBufDst Pointer to Source CIRBUF structure
 *
 * @return Returns number of bytes moved from source to destination buffers.
 */
WORD cbufMove(CIRBUF* pBufSrc, CIRBUF* pBufDst) {
    WORD lenSrc;
    WORD bytesWritten;
    WORD loops;

    loops = 0;
    bytesWritten = 0;

    //Check if any bytes available in Source. Use while loop because there could be 2 contiguous blocks of data.
    while ( cbufHasData(pBufSrc) && (loops++ < 2) ) {
        //Get number of contiguous bytes available for reading from Source
        //Will alway be > 0, seeing that cbufHasData() test was done
        lenSrc = cbufGetRdArrSize(pBufSrc);

        //No need for this test, because cbufHasData() test was done. lenSrc will always be > 0
        //if (lenSrc != 0) {
            //Write array of bytes from Source to Destination. Save actual number of bytes written in lenSrc
            lenSrc = cbufPutArray(pBufDst, cbufGetRdArr(pBufSrc), lenSrc);
        //}

        //Now remove bytes from Source buffer
        if (pBufSrc->flagBits.bSizePower2) {
            cbufRemoveBytes( ((CIRBUF*)pBufSrc), lenSrc);  //This is a CIRBUF structure, cast it
        }
        else {
            cbufRemoveBytes(pBufSrc, lenSrc);
        }

        bytesWritten += lenSrc;
    }


    return bytesWritten;
}

/**
 * Gets number of contiguous bytes that can be written to buffer. Buffer PUT pointer is NOT updated! Call cbufUpdatePut() to update
 * PUT pointer. Bytes can be written to cirbuf.buf[ cirbuf.put ]. Call cbufIsFull() to see if more bytes can be written
 * when done (and PUT pointer updated). Note that if available bytes cross buffer end boundry, there will be 2 congiguous blocks of data.
 *
 * !!!! IMPORTANT !!!!
 * Remember to call cbufUpdatePut() to update PUT pointer after adding bytes to array.
 *
 * For Example:
 *  WORD size, bytesRead;
 *  //Get number of bytes available that can be written to buffer array
 *  size = cbufGetWrArrSize(&cirbuf);
 *  //Call some function that requires an array and size as input parameters
 *  bytesRead = usbGetArr( cbufGetWrArr(&cirbuf), size);
 *  //Now update PUT pointer with bytes written
 *  cbufUpdatePut(&cirbuf, bytesRead);
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Number of bytes available in returned buffer
 */
WORD cbufGetWrArrSize(CIRBUF* pBuf) {
    WORD putCpy, getCpy;

    //Thread save, get copies of GET and PUT, and only use them in this function.
    NZ_BUILTIN_DISI(0x3FFF); // Disable interrupts, excluding level 7
    putCpy = (pBuf)->put;
    getCpy = (pBuf)->get;
    NZ_BUILTIN_DISI(0x0000); // Enable interrupts

    //If PUT < GET, available contiguous array is GET - PUT - 1. We can always put data till PUT is one behind GET (not = GET, thus -1).
    if (putCpy < getCpy) {
        return (getCpy - putCpy - 1);
    }

    //At this stage we know PUT >= GET. This means PUT can put data till highest location in array,
    //except when GET=0! When GET=0, PUT has to stop one before last array position!
    return ((getCpy == 0)
            ? ( (pBuf->maxOffset) - putCpy)
            : ( (pBuf->maxOffset) - putCpy + 1));
}

/**
 * Increments PUT pointer by given size, taking care of any wrapping required. This function
 * is meant to be used after writing bytes to a CIRBUF buffer's array obtained with cbufGetWrArrSize() function.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param n Size to increment put pointer by
 */
void cbufUpdatePut(CIRBUF* pBuf, WORD n) {
    pBuf->put = (((pBuf->put + n) > pBuf->maxOffset)
        ? ((pBuf->put + n) - pBuf->maxOffset - 1)
        : (pBuf->put + n) );
}


#if !defined(CIRBUF_DISABLE_PACKETS)    //Packets ARE supported

#if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)    //Contiguous packets can have dummy packet to fill end of buffer
/**
 * Check if the next packet to read is a dummy packet, and if so, remove it.
 * A dummy packet will have the first by (MSB byte of size) = 0xff. It will alway
 * reside in the last section of the buffer.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET" context".
 */
void checkIfDummyAndRemove(CIRBUF* pBuf) {
    //Check if next packet to read is a dummy packet
    if (pBuf->buf[pBuf->get] == 0xff) {
        //If dummy, remove it. This is done by simply adjusting GET to first byte of buffer
        pBuf->get = 0;
    }
}
#endif


/**
 * Indicates if given buffer contains a whole packet.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns 0 if false, else size of Packet (size of whole packet = [Size] and [Data] part of packet).
 */
WORD cbufHasWholePacket(CIRBUF* pBuf) {
    WORD sizeAvailable;
    WORD_VAL sizePacket;

    //Check this is a packet
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    if (pBuf->flagBits.bPacket == FALSE) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufPeekPacketDataSize() not packet!");
        return 0;
    }
    #endif

    //Return false if empty
    if (cbufIsEmpty(pBuf))
        return 0;

    //Check if next packet to get is dummy, and if so, remove it!
    #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)    //Contiguous packets can have dummy packet to fill end of buffer
        #if defined(CIRBUF_OPTIMIZE_SIZE)
        checkIfDummyAndRemove(pBuf);
        #else
        if (pBuf->buf[pBuf->get] == 0xff) {
            //If dummy, remove it. This is done by simply adjusting GET to first byte of buffer
            pBuf->get = 0;
        }
        #endif
    #endif

    //Get number of bytes available in buffer for reading
    sizeAvailable = cbufGetCount(pBuf);

    #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
        //This is a large buffer (16-bit size field)
        if (pBuf->flagBits.bPacketLarge == TRUE) {
            //Check it is large enought to contain full packet (data can be 0). For a large buffer (16-bit size field) it must at least by 2 bytes
            if (sizeAvailable<2)
                return 0;

            #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)
                //For contiguous packet, don't have to call cbufPeekByteAt(), seeing that LSB size byte will always be after MSB part
                sizePacket.byte.HB = (pBuf)->buf[(pBuf)->get];
                sizePacket.byte.LB = (pBuf)->buf[(pBuf)->get+1];
            #else
                sizePacket.byte.HB = cbufPeekByte(pBuf);
                sizePacket.byte.LB = cbufPeekByteAt(pBuf, 1);
            #endif

            //Currently sizePacket.Val contains size of [Data] part of packet. Get size of whole
            //packet = [Size] + [Data] part of packet. The "+2" is to include [Size] part for large packet (16-bits).
            sizePacket.Val = sizePacket.Val + 2;
        }
        else {
            //Check it is large enought to contain full packet (data can be 0). For small buffer, must just be 1 or larger
            if (sizeAvailable==0)
                return 0;

            //Get size of whole packet = [Size] + [Data] part of packet. The "+1" is to include [Size] part for normal packet (8-bits).
            sizePacket.Val = (WORD)cbufPeekByte(pBuf) + 1;
        }
    #else
        //Check it is large enought to contain full packet (data can be 0). For small buffer, must just be 1 or larger
        if (sizeAvailable==0)
            return 0;

        //Get size of whole packet = [Size] + [Data] part of packet. The "+1" is to include [Size] part for normal packet (8-bits).
        sizePacket.Val = (WORD)cbufPeekByte(pBuf) + 1;
    #endif  //#if defined(CIRBUF_DISABLE_LARGE_PACKET)

    if (sizeAvailable >= sizePacket.Val)
        return sizePacket.Val;

    //Has not got whole packet
    return 0;
}


/**
 * Compares given string to contents of Packet.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @preCondition The buffer must contain a Packet. Use cbufHasWholePacket() function
 *      prior to calling this function to check if the buffer contains a whole packet!
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param str String to compare contents of packet to.
 *
 * @return Returns 0 if string matches contents of buffer. The lenght is also checked, and
 *      buffer contents must be same as strings length.
 */
WORD cbufPacketStrcmp(CIRBUF* pBuf, const char* str) {
    WORD sizePacket;
    char c;
    char* p;

    //Get size of packet data
    sizePacket = cbufPeekPacketDataSize(pBuf);

    //Packet are written to contiguous space in buffer
    #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)

        //Get pointer to start of packet's [Data]. It is after the packet [Size], which can be 8 or 16 bits
        #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
            //This is a large buffer (16-bit size field)
            if (pBuf->flagBits.bPacketLarge == TRUE) {
                p = (char*)&pBuf->buf[pBuf->get+2];
            }
            else {
                p = (char*)&pBuf->buf[pBuf->get+1];
            }
        #else
            p = (char*)&pBuf->buf[pBuf->get+1];
        #endif

        //Compare string. If equal, sizePacket should be 0 when finished
        while( (c = *str++) != 0) {
            if(c != *p++) {
                //Not equal, return NON-0 value
                return 1;
            }
            sizePacket--;
        }
    //Packet are NOT written to contiguous space in buffer
    // !!!!!!!!! NOT IMPLEMENTED YET !!!!!!!!
    #else
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufPacketStrcmp() not implemented!");
        return 1;
    #endif

    //String matches
    return ( (sizePacket==0) ? 0 : 1);
}


/**
 * Gets number of free bytes available in buffer for a packet (packet data). This is how
 * much space is available for the data part of a packet. This function can be called
 * before calling cbufPutPacket() to see if there is enough space to add a packet.
 * Do not confuse this function with cbufGetFree()! If this is a "Packet Buffer", cbufGetFree()
 * can NOT be used! 
 *
 * @preCondition This must be a "Packet Buffer"!
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns number of free bytes available in buffer for the data part of a
 * packet.
 */
WORD cbufGetFreeForPacket(CIRBUF* pBuf) {
    WORD blockEnd;              //How much space for data at end of buffer
    WORD blockStart = 0;        //How much space for data at start of buffer
    BYTE bytesForSize = 1;      //bytes required for size, can be 1 (normal packet), or 2 (large packet)
    WORD putCpy, getCpy;

    //Thread save, get copies of GET and PUT, and only use them in this function.
    NZ_BUILTIN_DISI(0x3FFF); // Disable interrupts, excluding level 7
    putCpy = (pBuf)->put;
    getCpy = (pBuf)->get;
    NZ_BUILTIN_DISI(0x0000); // Enable interrupts

    #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
    //One extra byte required if large size
    if (pBuf->flagBits.bPacketLarge == TRUE) {
        bytesForSize++;
    }
    #endif

    //Packet must be written to contiguous space in buffer
    #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)
        //If GET > PUT, there is JUST ONE contiguous space available = (GET - PUT - 1). We can always
        //put data till PUT is one behind GET (not = GET, thus -1).
        //If GET = (PUT+1) buffer is full. That is (getCpy - putCpy - 1) = 0
        if (getCpy > putCpy) {
            blockEnd = (getCpy - putCpy - 1);  //Size of available block at end of buffer
        }
        //If PUT >= GET, there could be TWO contiguous blocks! This means PUT can put data till highest
        //location in array, except when GET=0! When GET=0, there is just ONE contiguous block (PUT
        //has to stop one before last array position)
        else {
            //Get space available at end of buffer. This is the space if GET = 0. Is incremented below if GET != 0
            blockEnd = pBuf->maxOffset - putCpy;

            //There are TWO contiguous blocks!
            if (getCpy != 0) {
                //End block is one larger, seeing that GET is NOT 0 (tested above)!
                blockEnd++;

                //Second contiguous block if from start to 1 position behind GET
                blockStart = getCpy - 1;
            }
            //ELSE, if there is only ONE contiguous block at end of buffer, do nothing
        }

        //At this stage blockEnd and blockStart contains space available at end and start of buffer. Space required
        //for [Size] HAS NOT been subtracted yet!

        //Test whick contiguous block is larger, and put largest block in blockEnd
        if (blockStart > blockEnd) {
            blockEnd = blockStart;
        }

        if (blockEnd <= bytesForSize)
            return 0;

        return blockEnd - bytesForSize;
    #else   //Packet does NOT have to be written to contiguous space in buffer
        //Get available space in blockEnd
        blockEnd = cbufGetFree(pBuf);

        if (blockEnd <= bytesForSize)
            return 0;

        return blockEnd - bytesForSize;
    #endif
}


/**
 * Checks if there is a whole packet available to read, and if so, returns it's size and a pointer
 * to it's data. The data is contained in a contiguous block, pointed to by the returned ppData pointer.
 * The returned size is a value from 1 to 254 for a normal "Packet Circular Buffer", and 1-65279
 * for a large "Packet Circular Buffer".
 * This function does NOT remove any data from the buffer. If the returned data is processed, the packet
 * can be removed by calling the cbufRemovePacket() function.
 *
 * @preCondition This must be a "Packet Buffer"!
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF circular buffer
 *
 * @param ppData This paramater is updated (output parameter!) with a pointer to the packet's data buffer.
 * The returned size indicates how much data is available to read..
 *
 * @return Returns size of packet data. A value from 1 to 254 for a normal "Packet Circular Buffer" and 1-65279
 * for a large "Packet Circular Buffer". Returns 0 if buffer empty, OR does not contain a packet, OR size
 * part of packet not available.
 */
WORD cbufGetContiguousPacket(CIRBUF* pBuf, BYTE** ppData)
{
    WORD sizePacket;

    //This function is not implemented yet for when (CIRBUF_DISABLE_CONTIGUOUS_PACKETS == 0)
    //To do this, we will have to check if packet if contained in two contiguous blocks,
    //and if so, copy it to temporary memory to a single block.
    #if defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufGetContiguousPacket() not implemented!");
        return 0;
    #endif

    //Check this is a packet
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    if (pBuf->flagBits.bPacket == FALSE) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufGetContiguousPacket() not packet!");
        return 0;
    }
    #endif

    //Done in cbufHasWholePacket() below
//    //return 0 if empty
//    if (cbufIsEmpty(pBuf))
//        return 0;
//
//    #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)  //Contiguous packets can have dummy packet to fill end of buffer
//    //Check if next packet to get is dummy, and if so, remove it!
//    #if defined(CIRBUF_OPTIMIZE_SIZE)
//    checkIfDummyAndRemove(pBuf);
//    #else
//    if (pBuf->buf[pBuf->get] == 0xff) {
//        //If dummy, remove it. This is done by simply adjusting GET to first byte of buffer
//        pBuf->get = 0;
//    }
//    #endif
//    #endif

    //Get size of whole packet = Packet's [Size] + [Data] part
    if ((sizePacket = cbufHasWholePacket(pBuf)) == 0) {
        return 0;
    }

    //Adjust sizePacket to size of [Data] part of packet only. Remove size of [Size] part of packet
    sizePacket--;
    #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
        //This is a large buffer (16-bit size field)
        if (pBuf->flagBits.bPacketLarge == TRUE) {
            sizePacket--;

            //Get pointer to packet's contiguous data
            *ppData = &pBuf->buf[pBuf->get+2];
        }
        else {
            //Get pointer to packet's contiguous data
            *ppData = &pBuf->buf[pBuf->get+1];
        }
    #else
        //Get pointer to packet's contiguous data
        *ppData = &pBuf->buf[pBuf->get+1];
    #endif

    //At this stage we know this is a small buffer (8-bit size field), and it's size is first value in buffer
    return sizePacket;
}


/**
 * Check if size part of packet(size of packet data) is in buffer, and if so, get and remove it. No
 * check is done to see if data part of packet is already in buffer.
 * The packet size is a value from 1 to 254 for a normal "Packet Circular Buffer", and 1-65279
 * for a large "Packet Circular Buffer".
 * It gives the size of the data part of the packet to follow. After calling this function, the
 * data part remains in the buffer and MUST be processed and removed! Use cbufPeekPacketDataSize() to
 * get size without removing it.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @preCondition This must be a "Packet Buffer"!
 *
 * @param pBuf Pointer to CIRBUF circular buffer
 *
 * @return Returns size of packet data. A value from 1 to 254 for a normal "Packet Circular
 * Buffer", and 1-65279 for a large "Packet Circular Buffer". Returns 0 if buffer empty, OR
 * does not contain a packet, OR size part of packet not available.
 */
WORD cbufGetPacketDataSize(CIRBUF* pBuf) {

    //return 0 if empty
    if (cbufIsEmpty(pBuf))
        return 0;

    //Check this is a packet
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    if (pBuf->flagBits.bPacket == FALSE) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nbufGetPacketDataSize() not packet!");
        return 0;
    }
    #endif

    //Check if next packet to get is dummy, and if so, remove it!
    #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)    //Contiguous packets can have dummy packet to fill end of buffer
        #if defined(CIRBUF_OPTIMIZE_SIZE)
        checkIfDummyAndRemove(pBuf);
        #else
        if (pBuf->buf[pBuf->get] == 0xff) {
            //If dummy, remove it. This is done by simply adjusting GET to first byte of buffer
            pBuf->get = 0;
        }
        #endif
    #endif

    #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
    //This is a large buffer (16-bit size field)
    if (pBuf->flagBits.bPacketLarge == TRUE) {
        WORD_VAL ret;

        //Check it is large enought to contain full packet (data can be 0). For a large buffer (16-bit size field) it must at least by 2 bytes
        if (cbufGetCount(pBuf)<2)
            return 0;

        ret.byte.HB = cbufGetByte(pBuf);
        ret.byte.LB = cbufGetByte(pBuf);
        return ret.Val;
        //return ( (((WORD)cbufGetByte(pBuf))<<8) | cbufGetByte(pBuf));
    }
    #endif  //#if defined(CIRBUF_DISABLE_LARGE_PACKET)

    //At this stage we know this is a small buffer (8-bit size field), and it's size is first value in buffer
    return cbufGetByte(pBuf);
}


/**
 * Return the first byte of the Packet data, without removing it.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @preCondition This must be a "Packet Buffer", and there MUST be a packet available!
 *      Use cbufHasWholePacket() function to check if there is a packet available!
 *
 * @param pBuf Pointer to CIRBUF circular buffer
 *
 * @return Returns first byte of Packet's data.
 */
BYTE cbufPeekPacketByte(CIRBUF* pBuf) {
    //Check this is a packet
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    if (pBuf->flagBits.bPacket == FALSE) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufPeekPacketDataByte() not packet!");
        return 0;
    }
    #endif

    //Not required - is done in cbufHasWholePacket(). Calling cbufHasWholePacket() pre-conditions!
    //Check if next packet to get is dummy, and if so, remove it!
//    #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)  //Contiguous packets can have dummy packet to fill end of buffer
//    #if defined(CIRBUF_OPTIMIZE_SIZE)
//    checkIfDummyAndRemove(pBuf);
//    #else
//    if (pBuf->buf[pBuf->get] == 0xff) {
//        //If dummy, remove it. This is done by simply adjusting GET to first byte of buffer
//        pBuf->get = 0;
//    }
//    #endif
//    #endif

    #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
    //This is a large Packet Circular Buffer (16-bit size field) - packet data starts at offset 2 (first 2 bytes is size)
    if (pBuf->flagBits.bPacketLarge == TRUE) {
        #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)
            //For contiguous packet, don't have to call cbufPeekByteAt(), seeing that all packet bytes are in configuous array!
            return ( (pBuf)->buf[(pBuf)->get+2] );
        #else
            return cbufPeekByteAt(pBuf, 2);
        #endif
    }
    #endif  //#if defined(CIRBUF_DISABLE_LARGE_PACKET)

    //This is a small Packet Circular Buffer (8-bit size field) - packet data starts at offset 1 (first byte is size)
    #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)
        //For contiguous packet, don't have to call cbufPeekByteAt(), seeing that all packet bytes are in configuous array!
        return ( (pBuf)->buf[(pBuf)->get+1] );
    #endif

    return cbufPeekByteAt(pBuf, 1);
}


/**
 * Returns size of packet data. Nothing is removed from the buffer!
 * The packet size is a value from 1 to 254 for a normal "Packet Circular Buffer", and 1-65279
 * for a large "Packet Circular Buffer". It gives the size of the data part of the packet to
 * follow.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @preCondition The buffer must contain a Packet. Use cbufHasWholePacket() function
 *      prior to calling this function to check if the buffer contains a whole packet!
 *
 * @param pBuf Pointer to CIRBUF circular buffer
 *
 * @return Returns size of packet data. A a value from 1 to 254 for a normal "Packet
 * Circular Buffer", and 1-65279 for a large "Packet Circular Buffer".
 */
WORD cbufPeekPacketDataSize(CIRBUF* pBuf) {
    //Check this is a packet
    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    if (pBuf->flagBits.bPacket == FALSE) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufPeekPacketDataSize() not packet!");
        return 0;
    }
    #endif

    //Not required - is done in cbufHasWholePacket(). Calling cbufHasWholePacket() pre-conditions!
    //Check if next packet to get is dummy, and if so, remove it!
//    #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)  //Contiguous packets can have dummy packet to fill end of buffer
//    #if defined(CIRBUF_OPTIMIZE_SIZE)
//    checkIfDummyAndRemove(pBuf);
//    #else
//    if (pBuf->buf[pBuf->get] == 0xff) {
//        //If dummy, remove it. This is done by simply adjusting GET to first byte of buffer
//        pBuf->get = 0;
//    }
//    #endif
//    #endif

    #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
    //This is a large buffer (16-bit size field)
    if (pBuf->flagBits.bPacketLarge == TRUE) {
        //Not required - is done in cbufHasWholePacket(). Calling cbufHasWholePacket() pre-conditions!
        //Check it is large enought to contain full packet (data can be 0). For a large buffer (16-bit size field) it must at least by 2 bytes
        //if (cbufGetCount(pBuf)<2)
        //    return 0;

        #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)
            //For contiguous packet, don't have to call cbufPeekByteAt(), seeing that LSB size byte will always be after MSB part
            return ( (((WORD)cbufPeekByte(pBuf))<<8) | ((pBuf)->buf[(pBuf)->get+1]) );
        #else
            return ( (((WORD)cbufPeekByte(pBuf))<<8) | cbufPeekByteAt(pBuf, 1));
        #endif
    }
    #endif  //#if defined(CIRBUF_DISABLE_LARGE_PACKET)

    //Not required - is done in cbufHasWholePacket(). Calling cbufHasWholePacket() pre-conditions!
    //Check it is large enought to contain full packet (data can be 0). For small buffer, must just be 1 or larger
    //if (cbufIsEmpty(pBuf))
    //    return 0;

    //At this stage we know this is a small buffer (8-bit size field), and it's size is first value in buffer
    return cbufPeekByte(pBuf);
}


/**
 * Adds given number of bytes to buffer as a packet. This function checks given buffer has enough
 * space, and either adds all given bytes, or nothing (not enough space). A Packet buffer has the format:
 * [Size][Data]
 * The size parameter must give the size of the source BYTE array (pSrcArr parameter). It will be copied to the
 * [Data] part of the Packet.
 * The [Size] part can be a 8 or 16-bit value, depending if it is a normal or large packet buffer. Use the
 * cbufIsPacketLarge() to determine if normal or large. When normal, function will fail if the size parameter
 * is larger than 254.
 *
 * The cbufGetFreeForPacket() function can be called prior to calling this function to
 * check if there is enough space. But if not called, and there is not enough space,
 * this function will return with 0 without any error occuring!
 *
 * @preCondition This must be a "Packet Buffer"!
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to destination CIRBUF Circular Buffer. Packet is added to this buffer.
 * @param pSrcArr Pointer to source BYTE array. This is copied to the [Data] part of packet.
 * @param size Size of source BYTE array(pSrcArr).
 *
 * @return Returns number of bytes added to buffer.
 */
WORD cbufPutPacket(CIRBUF* pBuf, const BYTE* pSrcArr, WORD size) {
    WORD getCpy;

    //Thread save, get copy of GET. No need to get copy of PUT, because this function is called from PUT context
    //NZ_BUILTIN_DISI(0x3FFF); // Disable interrupts, excluding level 7
    getCpy = (pBuf)->get;
    //NZ_BUILTIN_DISI(0x0000); // Enable interrupts

    #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
    BOOL largeSize = FALSE;
    #endif

    //All cases (except where we return) adds at least 1 to size (for large packet, have to add 2 - increment below for this case)
    WORD sizeRequired = size+1;   //Size including [Size] that must be added to buffer

    #if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    //Check this buffer is a packet type
    if (pBuf->flagBits.bPacket == FALSE) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufPutPacket() not packet buffer!");
        return 0;
    }
    #endif

    #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
    //One extra byte required if large size
    if (pBuf->flagBits.bPacketLarge == TRUE) {
        largeSize=TRUE;
        sizeRequired++;
    }
    #endif

    //Packet must be written to contiguous space in buffer
    #if !defined(CIRBUF_DISABLE_CONTIGUOUS_PACKETS)
    //!!!!! IMPORTANT !!!!!
    //Normally put functions should NEVER update GET pointer! This is because they can run in different thread context.
    //But, if buffer is empty, it is OK
    //If buffer is empty, reset PUT and GET pointer to 0. This prevents problem where empty buffer has
    //2 contiguous blocks. Buy doing this, we reset to 1 contiguous buffer
    if (pBuf->get == pBuf->put) {
        pBuf->get = pBuf->put = 0;
        getCpy = 0;
    }

    //If GET > PUT, there is JUST ONE contiguous space available = (GET - PUT - 1). We can always
    //put data till PUT is one behind GET (not = GET, thus -1).
    //If GET = (PUT+1) buffer is full. That is (getCpy - pBuf->put - 1) = 0
    if (getCpy > pBuf->put) {
        if (sizeRequired >= (getCpy - pBuf->put)) {  //No need to '-1', because we use '>=' in stead of '>'
            DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\ncbufPutPacket() no space!");
            //Not enought space left!
            return 0;
        }
    }
    //There are possibly two contiguous blocks! First till end of buffer, then from start till GET pointer
    else {
        WORD blockEnd;

        //First check if there is enough contiguous space at end of buffer?
        //If PUT >= GET, there could be TWO contiguous blocks! This means PUT can put data till highest
        //location in array, except when GET=0! When GET=0, there is just ONE contiguous block (PUT
        //has to stop one before last array position)
        blockEnd = pBuf->maxOffset - pBuf->put;

        if (getCpy != 0)
            blockEnd++;
        if (sizeRequired > blockEnd) {
            //If not enough space at end of buffer, check second contiguous block (beginning of buffer)
            //There is not enough space if:
            // - There is no second contiguous block (GET = 0)
            // - There is no space in second contiguous block (sizeRequired > getCpy)
            if ((getCpy == 0) || (sizeRequired >= getCpy)) {
                DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\ncbufPutPacket() no space!");

                //Not enought space left!
                return 0;
            }

            //If code gets here:
            // - There is space in SECOND contiguous block of buffer
            // - We have to append a 'Dummy Packet' to end of buffer (first contiguous space),
            //   and put packet at beginning (second contiguous space).

            //Put 'Dummy Packet' in first contiguous space. This is done by simply setting size
            //byte of packet (or first by of size for large packet) to 0xff.
            cbufPutByteNoCheck(pBuf, 0xff);

            //Now update PUT to second contiguous block = first byte of buffer = 0
            pBuf->put = 0;
        }
    }
    #else   //Packet does NOT have to be written to contiguous space in buffer
    //Enough space?
    if (sizeRequired > cbufGetFree(pBuf)) {
        DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "\ncbufPutPacket() no space!");
        //Not enought space left!
        return 0;
    }
    #endif


    #if !defined(CIRBUF_DISABLE_LARGE_PACKET)
    //Size is larger than 254, but packet type has a max size of 254!
    if ((largeSize == FALSE) && (size > 254)) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufPutPacket() size too big!");
        return 0;
    }

    //[Size] part of packet is a 16-bit value = [MSB][LSB]
    if (largeSize) {
        cbufPutByteNoCheck(pBuf, (BYTE)(size>>8));
    }
    #else
    //Size is larger than 254, but packet type has a max size of 254!
    if (size > 254) {
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\ncbufPutPacket() size too big!");
        return 0;
    }
    #endif

    //Next the LSB part of size. For packet with 8-bit size, this is whole size
    cbufPutByteNoCheck(pBuf, (BYTE)(size));

    //Next put data
    cbufPutArray(pBuf, pSrcArr, size);

    return sizeRequired;
}


/**
 * Checks if there is a packet in the buffer, and if so, removes it.
 *
 * @preCondition This must be a "Packet Buffer"!
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 */
void cbufRemovePacket(CIRBUF* pBuf) {
    WORD size;

    //Remove any Dummy packet, AND get packet size (if there is a packet
    if ((size = cbufGetPacketDataSize(pBuf)) > 0) {
        cbufRemoveBytes(pBuf, size);
    }
}

#endif  //#if !defined(CIRBUF_DISABLE_PACKETS)

#endif  //#if defined(CIRBUF_USE_CIRCULAR_BUFFER_STD)