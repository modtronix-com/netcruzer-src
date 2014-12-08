 /**
 * @brief           This file defines a Circular Buffer interface
 * @file            nz_circularBuffer.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 <!-- ========== Description ========== -->
 @section nz_circularBuffer_desc Description
 Circular buffers form a very important part of the Netcruzer Library. They are used for many of the
 modules contained in the library, and provide a consistent way for exchanging data between modules.
 In it's simplest form it is a "first in, first out" BYTE buffer. It provides the basic functions for
 reading and writing data from and to the buffer. But also contains many additional, more advanced functions.

 This file defines a Circular Buffer interface. The implementation has to be done
 in a separate c file by creating instances of these functions.
  The Netcruzer library contains two implementations:
 - nz_circularBufferStd.c, this is a default implementation for creating circular buffers of any
   size.
 - nz_circularBufferPwr2.c, this is a more efficient implementation, with the condition that the
   size has to be a power of 2 value (4,8,16,32,64,128...).
 Most examples and projects use the nz_circularBufferPwr2.c implementation.


 <!-- ========== Configuration ========== -->
 @section nz_circularBuffer_conf Configuration
 The following defines are used to configure this module, and should be placed in projdefs.h. Note
 that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code

// *********************************************************************
// ------- Circular Buffer Configuration (nz_circularBuffer.h) ---------
// *********************************************************************
//Specifies what circular buffer is used. Ensure only one of the following is uncommented. If none selected, nz_circularBufferStd is used
//#define    CIRBUF_USE_CIRCULAR_BUFFER_STD                 //Use nz_circularBufferStd
#define     CIRBUF_USE_CIRCULAR_BUFFER_PWR2                 //Use nz_circularBufferPwrs

//Optimize for size, not speed!
#define     CIRBUF_OPTIMIZE_SIZE

//Configure buffer NOT to supports packets - define to create smaller code when packets not used.
//#define   CIRBUF_DISABLE_PACKETS

//Configure buffer NOT to supports large packets (65,279 bytes, standard is 254) - define to create smaller code when large packets not used.
//#define   CIRBUF_DISABLE_LARGE_PACKET

//Defines minimum size allowed for a circular buffer. Default is 8
//#define   CIRBUF_MIN_SIZE                     ( 8 )

//Disable all MARCO_xxx functions. Uncomment this line for smaller code. Comment this line for faster, larger code.
//#define   CIRBUF_DISABLE_ALL_MACRO_FUNCTIONS

//Standard Escape Character
#define     CIRBUF_ESC_CHAR                     ( '^' )     //[-DEFAULT-]

//Small code saving by using cbufPutByte() for cbufPutByteNoCheck(). Not enabled by default!
//#define   CIRBUF_USE_PUTBYTE_FOR_PUTBYTENOCHECK

@endcode


 <!-- ========== Usage ========== -->
 @section nz_circularBuffer_usage Usage
 To use a circular buffer, the following must be done:
 - Copy the @ref nz_circularBuffer_conf "Configuration" section above (from nz_circularBuffer.h)
   to the projdefs.h file.
 - In this "Configuration" section, change any default values if required (if default
   values should be used, define is not required in projdefs.h).
 - Add a default implementation c file to the MPLAB project, like nz_circularBufferPwr2.c for
   example.
 - Add nz_helpers.c to the MPLAB project.
 - <b>All DONE!</b> The functions defined in this file can now be used in the project.

 
 <!-- ========== Buffer Type ========== -->
  @section nz_circularBuffer_type Buffer Type
 When creating a circular buffer, the type and format is specified. When working with Circular Buffers,
 the buffer's type can be obtained with the cbufGetType() function. There are two main buffer
 types, streaming and packet. For a standard data buffer, with no formatting, a streaming
 buffer with no formatting should be created. This is done by the following code:
 @code
    CIRBUF cbufTest;    //CIRCUF structure
    BYTE   bufTest[32]; //Byte array for buffer data
    cbufInit(&cbufTest, bufTest, sizeof(bufTest), CIRBUF_FORMAT_NONE | CIRBUF_TYPE_STREAMING);
 @endcode

 <!-- - -  - - -->
 @subsection nz_circularBuffer_type_streaming Streaming Type
 This type of buffer is created by passing the CIRBUF_TYPE_STREAMING define to the cbufInit() function.
 It creates a simple "first in, first out" type buffer, for reading and writing single bytes.
 There is no formatting! Bytes are added with cbufPutByte(), and removed with cbufGetByte() functions.

 <!-- - -  - - -->
 @subsection nz_circularBuffer_type_packet Packet Type
 This type of buffer is created by passing the CIRBUF_TYPE_PACKET define to the cbufInit() function.
 A packet type buffer groups data together in packets, with a maximum data size of 254 bytes.
 It has the following format:<br>
 [Size 8-bit][Data]
 - <b>size</b>: Is the 8-bit size of data to follow
 - <b>data</b>: The packet data.

 Various packet functions are provided for reading and writing data from and to the buffer in packet
 format. Some of them are:<br>
 cbufIsPacket(), cbufHasWholePacket(), cbufPeekPacketByte(), cbufPeekPacketDataSize(), <br>
 cbufPutPacket(), cbufGetContiguousPacket()....

 <!-- - -  - - -->
 @subsection nz_circularBuffer_type_large_packet Large Packet Type
 This type of buffer is created by passing the CIRBUF_TYPE_LARGE_PACKET define to the cbufInit() function.
 Same as CIRBUF_TYPE_PACKET, but the packet has a much larger possible data size.
 Data is grouped data together in packets, with a maximum data size of 65,279 bytes.
 It has the following format:<br>
 [Size 16-bit][Data] = [size MSB][size LSB][data]
 - <b>size</b>: Is the 16-bit size of data to follow. Fist byte (cbufPeekByte(pBuf))
   is MSB part of size. If first byte is 0xFF, it is a dummy packet till end of contiguous
   buffer space. This is used to fill buffer if last part of buffer is too small to hold
   a packet. Buffer is filled with dummy packet, and packet is written to contiguous data
   space at beginning of buffer.
 - <b>data</b>: The packet data.


 <!-- ========== Buffer Format ========== -->
 @section nz_circularBuffer_format Buffer Format
 When creating a circular buffer, four formatting types can be specified. The buffer formatting will
 influence the data provided to, and obtained from certain functions. When working with Circular Buffers,
 the buffer's format can be obtained with the cbufGetFormat() function.

 <!-- - - No Format - - -->
 @subsection nz_circularBuffer_format_none No Format
 Is created by passing the CIRBUF_FORMAT_NONE constant to the cbufInit() function.<br>
 Buffer contains <b>binary data</b>. No formatting is provided by any functions.

 <!-- - - Binary Format - - -->
 @subsection nz_circularBuffer_format_bin Binary Format
 Is created by passing the CIRBUF_FORMAT_BIN constant to the cbufInit() function.<br>
 Buffer contains <b>binary data</b>. No conversion is done to data read and written from and to
 the buffer.

 <!-- - - Binary Format, with Escape Sequence - - -->
 @subsection nz_circularBuffer_format_bin_esc Binary Format, with Escape Sequence
 Is created by passing the CIRBUF_FORMAT_BIN_ESC constant to the cbufInit() function.<br>
 Buffer contains <b>binary data</b>. This is the same as the <b>Binary Format</b> buffer, but provides additional
 functionality for embedding "control characters" in the data. The following additional functions are provided:
 @code
 - cbufGetEscapedSizeRequired()
 - cbufGetEscapeCharacter()
 - cbufGetEscapedByte()
 - cbufPutEscapedByte()
 - cbufPutControlChar()
  @endcode
 The '^' character is the default "escape character". It is used to add "control characters" to
 the buffer. A "control character" is preceded by the '^' character. Two '^' characters following each
 other represents a single '^' character. The default "escape character" can be changed by adding
 the CIRBUF_ESC_CHAR define to the projdefs.h file (see @ref nz_circularBuffer_conf).
 - It is recommended to use <b>lower case</b> characters 'a' to 'z' for "control characters".
 - It is recommended to use a "not too often" used ASCII character for the "escape character".
 
 The following functions provide additional functionality for writing "ASCII formatted strings". 
 @code
 - cbufPutAsciiEscString()
 @endcode
 The given string will be encoded, and stored in binary format in the buffer.
 
 The "escape sequences" are:<br>
 | Escape<br> Sequence | Description                                                                                                    |
 |---------------------|----------------------------------------------------------------------------------------------------------------|
 | @b ^^  | Two "escape characters" represents a single '^' character.                                                                  |
 | @b ^x  | Use this format to represent a "control character". For example '^s' could be used to represent an I2C bus Start condition. |

 An example of this type of buffer would be for sending I2C data. The I2C bus can put START and STOP conditions
 on the bus in addition to normal data. The '^s' and '^p' "escape sequences" can be use to represent Start and
 Stop commands. For example, to put a START condition on the bus, followed by "Hello", followed by the STOP condition,
 the following bytes should be added to the buffer:<br>
  '<b>^</b>', '<b>s</b>', '<b>H</b>', '<b>e</b>', '<b>l</b>', '<b>l</b>', '<b>o</b>', '<b>^</b>', '<b>p</b>'
  
 <!-- - - ASCII Format - - -->
 @subsection nz_circularBuffer_format_ascii ASCII Format
 Is created by passing the CIRBUF_FORMAT_ASCII constant to the cbufInit() function.<br>
 The buffer contain readable <b>ASCII characters</b> (human readable ASCII format):
 - Bytes are represented as two-digit, upper case hex characters.
 - Strings can be inserted by enclosing them with single quotation marks(').
 
 No special formatting is provided by any functions.
 
 The buffer has the following format:<br>
 | Format | Description                                                                                                                 |
 |---------------------|----------------------------------------------------------------------------------------------------------------|
 | @b XX  | Two upper case characters representing a single byte (hex value).                                                           |
 | @b '  | Enclose string with single quotes. A double ' character will not end the string, but represents a single ' character.        |
 
 The following example shows how to represent 3 bytes (0x50, 0xC0 and 0xFA) with an "ASCII formatted string":
 @code
 50C0FA
 @endcode
 It is stored in the buffer as <b>6</b> characters, and represents the following <b>3</b> bytes = 0x50, 0xC0, 0xFA.
 
 The following example shows how to represent the same 3 bytes as above (0x50, 0xC0 and 0xFA), with a string (Hello)
 added to it:
 @code
 50C0FA'Hello'
 @endcode
 It is stored in the buffer as <b>13</b> characters, and represents the following <b>8</b> bytes:<br>
 0x50, 0xC0, 0xFA, 'H', 'e', 'l', 'l', 'o'

 
 <!-- - - ASCII Format, with Escape Sequence - - -->
 @subsection nz_circularBuffer_format_ascii_esc ASCII Format, with Escape Sequence
 Is created by passing the CIRBUF_FORMAT_ASCII_ESC constant to the cbufInit() function.<br>
 This is the same as the <b>ASCII Format</b> buffer, but provides additional functionality
 for embedding "control characters" in the data.
 - Bytes are represented as two-digit, upper case hex characters.
 - Strings can be inserted by enclosing them with single quotation marks(').
 - "Control characters" are represented by a lower case character 'a' to 'z'
 - <b>Additionally</b> "Control characters" can be inserting by using the "escape character", followed by any ASCII character
 - 's' an 'p' control characters represent the start and stop of a message.
 
 For lower case control characters in the range 'a' to 'z', no "escape character" is required! For "control characters" outside
 this range, an "escape sequence" is required! It is recommended always using lower case "control characters"!

 The buffer has the following format:<br>
 | Format | Description                                                                                                                 |
 |---------------------|----------------------------------------------------------------------------------------------------------------|
 | @b XX  | Two upper case characters representing a single byte (hex value).                                                           |
 | @b c   | Lower case character 'a' to 'z' represents a "control character".                                                          |
 | @b s, @b p | 's' an 'p' control characters represent the start and stop of a message.                                                    |
 | @b ^^  | Two "escape characters" represents a single '^' character.                                                                  |
 | @b ^x  | Use this format to represent a "control character". For example '^s' could be used to represent an I2C bus Start condition. |
 | @b '   | Enclose string with single quotes. A double ' character will not end the string, but represents a single ' character.       |
 
 The following example shows how to represent 3 bytes (0x50, 0xC0 and 0xFA) with an "ASCII formatted string":
 @code
 50C0FA
 @endcode
 It is stored in the buffer as <b>6</b> characters, and represents the following <b>3</b> bytes = 0x50, 0xC0, 0xFA.
 
 The following example shows how to represent the same 3 bytes as above (0x50, 0xC0 and 0xFA) with an
 "ASCII formatted string". In addition it adds a 's' and 'p' control character to the front and back of the 3 bytes:
 @code
 s50C0FAp
 @endcode
 It is stored in the buffer as <b>8</b> characters, and represents the following <b>5</b> bytes<br>:
 <i>'s'(Control Character)</i>, 0x50, 0xC0, 0xFA, <i>'p'(Control Character)</i>

 The following example shows how to represent the same 3 bytes as above (0x50, 0xC0 and 0xFA) with an
 "ASCII formatted string". In addition it adds a 'Z' and 'p' control character to the front and back of the 3 bytes.
 <b>NOTE</b> that because 'Z' is not lower case, it need to be written as an "escape sequence"!:
 @code
 ^Z50C0FAp
 @endcode
 It is stored in the buffer as <b>9</b> characters, and represents the following <b>5</b> bytes<br>:
 <i>'Z'(Control Character)</i>, 0x50, 0xC0, 0xFA, <i>'p'(Control Character)</i>

 The following example shows how to represent the same 3 bytes as above (0x50, 0xC0 and 0xFA), with a string (Hello)
 added to it:
 @code
 50C0FA'Hello'
 @endcode
 It is stored in the buffer as <b>13</b> characters, and represents the following <b>8</b> bytes:<br>
 0x50, 0xC0, 0xFA, 'H', 'e', 'l', 'l', 'o'

 The following example shows how to represent the same 3 bytes as above (0x50, 0xC0 and 0xFA), with a string (Hi)
 added to it. It also adds a 's' and 'p' control character to the front and back of the data:
 @code
 s50C0FA'Hi'p
 @endcode
 It is stored in the buffer as <b>12</b> characters, and represents the following <b>7</b> bytes<br>:
 <i>'s'(Control Character)</i>, 0x50, 0xC0, 0xFA, 'H', 'i', <i>'p'(Control Character)</i>
 

 **********************************************************************
 * @section nz_circularBuffer_lic Software License Agreement
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
 * 2010-10-11, David H. (DH):
 *    - Initial version
 *********************************************************************/

/////////////////////////////////////////////////
//  Circular Buffer - documentation module
/** @defgroup info_cirbuf Circular Buffers
 Circular buffers form a very important part of the Netcruzer Library. They are used for many of the
 modules contained in the library, and provide a consistent way for exchanging data between modules.
 In it's simplest form it is a "first in, first out" BYTE buffer. It provides the basic functions for
 reading and writing data from and to the buffer. But also contains many additional, more advanced functions.

 For details on Circular Buffers, see the @ref nz_circularBuffer_desc "nz_CircularBuffer.h Documentation".
 */

#ifndef NZ_CIRCULARBUFFER_H
#define    NZ_CIRCULARBUFFER_H


/////////////////////////////////////////////////
// Default values if none are defined
/////////////////////////////////////////////////

//If no circular buffers are defined, define CIRBUF_USE_CIRCULAR_BUFFER_PWR2
#if !defined(CIRBUF_USE_CIRCULAR_BUFFER_STD) && !defined(CIRBUF_USE_CIRCULAR_BUFFER_PWR2)
#define    CIRBUF_USE_CIRCULAR_BUFFER_PWR2      //Use nz_circularBufferPwr2
#endif


//Defines minimum size allowed for a circular buffer. Default is 8
#if !defined(CIRBUF_MIN_SIZE)
#define    CIRBUF_MIN_SIZE     8
#endif

/**
 * For packet buffers, this causes packet to ONLY be placed into contiguous data space in the buffer.
 * If a packet is added to the buffer, a check is done to see if there is enough contiguous space at
 * the end of the buffer to add the packet. If not, a dummy packet (MSB byte of size = 0xff) is written
 * to the end of the buffer (fills the end of buffer, and the packet is written at the start of the buffer.
 * This will cause space to be lost in a buffer, but greatly simplifies code, seeing that packets are
 * always located in contiguous data space.
 */
//#define CIRBUF_DISABLE_CONTIGUOUS_PACKETS


//Default Escape Character
#if !defined(CIRBUF_ESC_CHAR)
#define     CIRBUF_ESC_CHAR     '^'
#endif



/////////////////////////////////////////////////
// Buffer Type Defines
/////////////////////////////////////////////////

/**
 * Default buffer type. Streaming buffer, there is no formatting! Bytes are added
 * with cbufPutByte(), and removed with cbufGetByte() functions.
 */
#define CIRBUF_TYPE_STREAMING   0x0000

/**
 * Packet buffer type, maximum 254 bytes long. Each packet added has the format:<br>
 * [Size 8-bit][Data]
 * - <b>size</b>: Is the 8-bit size of data to follow
 * - <b>data</b>: The data
 *
 * New packet is only added if there is enough space, ensuring buffer does not
 * get corrupted. If no space, low level code responsible for adding packet might
 * send failure message to sender, requesting it is sent again at a later stage.
 */
#define CIRBUF_TYPE_PACKET      0x0001

/**
 * Large Packet buffer type, maximum 65,279 bytes long. Each packet added has the
 * format:<br>
 * [Size 16-bit][Data] = [size MSB][size LSB][data]
 * - <b>size</b>: Is the 16-bit size of data to follow. Fist byte (cbufPeekByte(pBuf))
 * is MSB part of size. If first byte is 0xFF, it is a dummy packet till end of contiguous
 * buffer space. This is used to fill buffer if last part of buffer is too small to hold
 * a packet. Buffer is filled with dummy packet, and packet is written to contiguous data
 * space at beginning of buffer.
 * - <b>data</b>: The data
 *
 * Same as CIRBUF_TYPE_PACKET, but larger possible data.
 * Get size of packet (data part, whole packet is one more) with
 * bufGetLargePacketSize(CIRBUF_RX_xxx) function
 */
#define CIRBUF_TYPE_LARGE_PACKET    0x0003

#define CIRBUF_FLAGS_POWER2     0x0020



/////////////////////////////////////////////////
// Byffer Format Defines
/////////////////////////////////////////////////

/** Buffer has no formatting. */
#define CIRBUF_FORMAT_NONE      0x0000
#define BUFFORMAT_NONE          0

/** Buffer is in ASCII (plain text) format */
#define CIRBUF_FORMAT_ASCII     0x0004
#define BUFFORMAT_ASCII         1

/**
 * Buffer is in ASCII format (plain text), with Escape sequence (Control Character)
 * processing. The '^' is the default escape character. Standard Two '^' characters
 * following each other represent a single '^' character.
 */
#define CIRBUF_FORMAT_ASCII_ESC     0x0008
#define BUFFORMAT_ASCII_ESC         2

/** Buffer is in plain Binary format. No processing of the data is done. */
#define CIRBUF_FORMAT_BIN           0x000C
#define BUFFORMAT_BIN               3


/**
 * Buffer is in plain Binary format, with Escape sequence processing. The '^' is the
 * default escape character. Two '^' characters following each other represent a single '^'
 * character. Exact processing of Escape sequence depends on buffer type, and how
 * it processes them. For example, an I2C buffer could interpret '^S' as
 * a "Bus Start condition", and '^P' as a "Bus Stop condition".
 */
#define CIRBUF_FORMAT_BIN_ESC       0x0010
#define BUFFORMAT_BIN_ESC           4


//CIRBUF is defined in nz_netcruzer.h
/**
 * Circular Buffer Structure, for buffers with maximum size of 65279. Exactly the same as
 * CIRBUF structure, but flagBits.bSizePower2 flag must be clear.
 * Use bufXxx functions with this structure. bufXxx functions always work, and
 * buffXxx are optimzed functions for when buffer size is a power of 2.
 */
//typedef struct __attribute__((aligned(2), packed)) _CIRBUF
typedef struct __attribute__((__packed__)) _CIRBUF
{
    WORD  put;          //Put Pointer(offset) for Buffer, a value from 0-(bufSize)
    WORD  get;          //Get Pointer(offset) for Buffer, a value from 0-(bufSize)
    BYTE* buf;          //Pointer to start of buffer.
    WORD  maxOffset;    //Max value offset can have = usable buffer size. Actual buffer size is maxOffset+1 (because put=get is used to indicate empty buffer)
                        //When buffer size is power of 2, this is also the mask used for updating put and get offsets.
    union {
        struct
        {
            unsigned int bPacket : 1;      ///< Buffer Type: If 0 is streaming(CIRBUF_TYPE_STREAMING), else packet(CIRBUF_TYPE_PACKET or CIRBUF_TYPE_LARGE_PACKET)
            unsigned int bPacketLarge : 1; ///< Buffer Type: Only valid for Packet Buffer Type, if 0 is CIRBUF_TYPE_PACKET, else CIRBUF_TYPE_LARGE_PACKET
            unsigned int format : 3;       ///< Buffer Format: Formatting, is a BUFFORMAT_XXX Define
            unsigned int bSizePower2 : 1;  ///< Buffer size must be a power of 2. This makes buffer more efficient
            unsigned int fill6_7 : 2;      ///< Fill
            unsigned int fill8_15 : 8;
        } flagBits;
        struct
        {
            unsigned int type : 2;         ///< Buffer Type, is a CIRBUF_TYPE_XXX define. Can only be streaming or packet types
            unsigned int format : 3;       ///< Formatting, is a BUFFORMAT_XXX Define
            unsigned int fill : 11;
        } flagTF;  //Type and Format
        struct {
            unsigned int val : 5;           ///< Buffer Type and Format, OR'ed combination of CIRBUF_TYPE_XXX and CIRBUF_FORMAT_XXX
            unsigned int fill5_7 : 3;       ///< Reserved, might add to format
            unsigned int fill : 8;
        } typeFormat;  //Type and Format
        struct
        {
            BYTE    LB;                 ///< Lower Byte, contains Buffer Type and Format
            BYTE    HB;                 ///< Upper Byte
        } flagBytes;
        WORD flagVal;
    };
} CIRBUF;



/////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////

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
void cbufInitDefault(CIRBUF* pBuf, BYTE* bufArray, WORD size);


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
void cbufInit(CIRBUF* pBuf, BYTE* bufArray, WORD size, BYTE typeFormat);


/**
 * Must be called every couple of ms
 *
 */
void cbufTask(void);


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
void cbufSetTypeAndFormat(CIRBUF* pBuf, BYTE typeFormat);


/**
 * Empty the given buffer. Any data contained in the buffer is lost.
 *
 * For "Packet" buffer, do not use this function. When calling cbufPutPacket(), and there
 * is no space AND 2 contiguous blocks, write dummy packet. This will cause the next
 * bufGetXxx function to remove dummy packet, and reset buffer to PUT=GET=0
 *
 * @param pBuf Pointer to CIRBUF structure
 */
void cbufEmpty(CIRBUF* pBuf);


/**
 * Checks if the given buffer is empty.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns true if the given buffer is empty. Else, returns false.
 */
BOOL cbufIsEmpty(CIRBUF* pBuf);


/**
 * Checks if the given transmit buffer is full - no more space for adding data.
 * This is the case when put+1 = get
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns true if the given buffer is full. Else, returns false.
 */
BOOL cbufIsFull(CIRBUF* pBuf);


/**
 * Checks if the given buffer contains data.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns true if the given buffer has data.
 */
BOOL cbufHasData(CIRBUF* pBuf);


/**
 * Checks if the given buffer is not full. Has space for adding data.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns true if the given buffer has data.
 */
#define cbufHasSpace(pBuf)  (cbufIsFull(pBuf)==0)


/**
 * Gets number of bytes available in buffer for reading.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns number of bytes available in buffer.
 */
WORD cbufGetCount(CIRBUF* pBuf);


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
WORD cbufGetFree(CIRBUF* pBuf);


/**
 * Returns this buffer's format.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns this buffer's format, is a CIRBUF_FORMAT_XXX constant
 */
BYTE cbufGetFormat(CIRBUF* pBuf);


/**
 * Returns this buffer's type.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns this buffer's type, is a CIRBUF_TYPE_XXX constant
 */
BYTE cbufGetType(CIRBUF* pBuf);


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
WORD cbufGetEscapedSizeRequired(CIRBUF* pBuf, BYTE* buf, WORD size);


/**
 * Get the "escape characters". The default escape character is '^'.
 * This can be changed by defining CIRBUF_ESC_CHAR in the projdefs.h file.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns the escape character, is '^' by default.
 */
#define cbufGetEscapeCharacter(pBuf)    (CIRBUF_ESC_CHAR)


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
BYTE cbufPeekByte(CIRBUF* pBuf);


/**
 * Gets a byte at the given offset from the Buffer, without removing it.
 * The byte is NOT removed from the buffer, and the buffer pointers are NOT updated!
 * The byte at the given offset it returned. The offset is how deep the byte is in
 * the buffer. For example, 0 will return first byte in buffer, 5 will return the 6th
 * byte in the buffer.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @preCondition Ensure offset parameter is not larger than current number of byte
 * contained in buffer. Call cbufGetCount() to get current number of bytes in buffer.
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param offset Offset of byte to return. Is a value from 0-n, where n = (cbufGetCount() - 1)
 *
 * @return Returns the byte at the given offset in the given buffer.
 */
BYTE cbufPeekByteAt(CIRBUF* pBuf, WORD offset);


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
BYTE cbufPeekEscapedByte(CIRBUF* pBuf, BYTE* b);


/**
 * Find first occurance of given byte. The offset in the buffer is returned. For example, if the
 * string is "Name=Mark", and we call cbufFindByte(pBuf, 0, '='), it will return 4 (index of =).
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param offset Offset to start the search from, 0 is start of buffer.
 * NOT SUPPORTED YET, but will be in future!
 *
 * @param value Byte to search forl
 *
 * @return Offset of byte, or -1 of not found
 */
WORD cbufFindByte(CIRBUF* pBuf, WORD offset, BYTE value);


/**
 * Removes a byte from the Buffer. Removes bytes from the back of the circular
 * buffer (from GET pointer). The same as cbufGetByte() will remove the byte read!
 *
 * @preCondition Before calling this function, it must be confirmed that the buffer has data.
 * The cbufHasData() is an example function that can be used to do this check.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 */
void cbufRemoveByte(CIRBUF* pBuf);


/**
 * Removes given number of bytes from the Buffer.
 *
 * @preCondition Before calling this function, it must be confirmed that the buffer has the
 * requested number of bytes to remove.
 *
 * !!! IMPORTANT !!! If more bytes are requested to be removed then available, bad things will happen!!!!!!!!
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param n Number of bytes to remove
 */
void cbufRemoveBytes(CIRBUF* pBuf, WORD n);


/**
 * Removes last byte written to the Buffer. Removes bytes from the front of the circular
 * buffer (from PUT pointer). This function is NOT the same as the cbufRemoveByte() function!
 * It should be used to undo a byte added via cbufPutByte().
 *
 * @preCondition Before calling this function, it must be confirmed that the buffer has data.
 * The cbufHasData() is an example function that can be used to do this check.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 */
void cbufRemovePutByte(CIRBUF* pBuf);


/**
 * Gets a byte from the Buffer, and removes it from buffer.
 *
 * @preCondition cbufIsEmpty() must have been called to confirm the buffer has data!
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns the next byte in the given buffer.
 */
BYTE cbufGetByte(CIRBUF* pBuf);


/**
 * Gets a byte from the Buffer, and removes it from buffer. MACRO version, ONLY use as expression, like "c=cbufGetByte_MACRO(pBuf);" !
 * !!!! IMPORTANT !!!!
 * Never use in statment, but only on single line. DON'T use like this:
 * if ((i = cbufGetByte(1)) > 1)
 * Use like that:
 * i = cbufGetByte(1);
 *
 * @preCondition cbufIsEmpty() must have been called to confirm the buffer has data!
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns the next byte in the given buffer.
 */
BYTE cbufGetByte_MACRO(CIRBUF* pBuf);



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
BYTE cbufGetEscapedByte(CIRBUF* pBuf, BYTE* b);


/**
 * Gets a string from the "Circular Buffer", and copies it to given destination array. The string
 * is remove them from source "Circular Buffer". The actual number of bytes copied is returned.
 * Is only done if a string is found, or end of source buffer reached. Returns number of bytes
 * copied.
 *
 * If size is exceeded, -1 is returned, and no bytes are removed from source buffer! A side affect
 * is that bytes will still be copied to given destination array (pDstArr), but nothing is removed
 * from source buffer!
 *
 * !!!! IMPORTANT !!!! Note that if given byte not found, or end of source buffer not reached,
 * nothing is removed from source buffer and -1 is returned. This is the case if the size of the
 * given destination array (size parameter) is reached. Bytes (size) are still copied to destination
 * array (pDstArr).
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to source CIRBUF structure, that data is copied from
 * @param pDstArr Pointer to destination BYTE array
 * @param size Maximum number of bytes to copy. Could be less if pBuf contains less
 *
 * @return Returns number of bytes added to array.
 */
WORD cbufGetString(CIRBUF* pBuf, BYTE* pDstArr, WORD size);


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
 * @return Returns number of bytes added to destination array.
 */
WORD cbufGetArray(CIRBUF* pBuf, BYTE* pDstArr, WORD size);


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
WORD cbufGetArrayTillByte(CIRBUF* pBuf, BYTE* pDstArr, BYTE value, WORD size);


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
WORD cbufGetRdArrSize(CIRBUF* pBuf);


/**
 * Get byte pointer to current GET location. Use this function with cbufGetRdArrSize() function.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @return BYTE pointer to current GET location
 */
BYTE* cbufGetRdArr(CIRBUF* pBuf);


/**
 * Adds a BYTE to the buffer, and updates the buffer pointers.
 * This function checks given buffer has enough space(1 byte) to add BYTE.
 * If the destination buffer has enough space, 1 byte will be added.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param b Byte to add to the buffer
 *
 * @return Returns number of bytes added to buffer, will be 0 or 1.
 *         Returns 0 if buffer did not have enough space to add given byte.
 *         In this case, nothing is added to the buffer.
 */
BYTE cbufPutByte(CIRBUF* pBuf, BYTE b);


/**
 * Adds a BYTE to the buffer, and updates the buffer pointers. No check is done
 * to see if there is space available in the buffer!
 *
 * @preCondition cbufIsFull() must have been called to confirm the buffer is not full!
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param b Byte to add to the buffer
 */
void cbufPutByteNoCheck(CIRBUF* pBuf, BYTE b);


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
BYTE cbufPutWord(CIRBUF* pBuf, WORD w);


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
BYTE cbufPutByteAscii(CIRBUF* pBuf, BYTE b);


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
BYTE cbufPutByteAsciiHex(CIRBUF* pBuf, BYTE b);


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
BYTE cbufPutWordAscii(CIRBUF* pBuf, WORD w);


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
BYTE cbufPutWordAsciiHex(CIRBUF* pBuf, WORD w);


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
BYTE cbufShaddowPutByte(CIRBUF* pBuf, BYTE b, WORD putShaddow);


/**
 * Commit shaddow PUT pointer.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @param putShaddow Shaddow PUT pointer to commit
 */
void cbufShaddowPutCommit(CIRBUF* pBuf, WORD putShaddow);


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
BYTE cbufPutControlChar(CIRBUF* pBuf, BYTE b);


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
BYTE cbufPutEscapedByte(CIRBUF* pBuf, BYTE b);


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
WORD cbufPutString(CIRBUF* pBuf, const char * s);


/**
 * Flag instructing cbufPutAsciiEscString() function to adds a 's' and 'p' control character to
 * front and back of destination.
 */
#define ASCIIESC_ADD_S_AND_P 0x0001


/**
 * @function WORD cbufPutAsciiEscString(CIRBUF* cbufDst, CIRBUF* cbufSrc, const char* pStrSrc, WORD flags)
 * 
 * Adds the given string (in ASCII format, with escape sequences) to the given destination.
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
WORD cbufPutAsciiEscString(CIRBUF* cbufDst, CIRBUF* cbufSrc, const char* pStrSrc, WORD flags);


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
WORD cbufPutStringWait(CIRBUF* pBuf, const char * s);


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
WORD cbufPutArray(CIRBUF* pBuf, const BYTE* pSrcArr, WORD size);


/**
 * Adds given number of bytes to buffer. If buffer does not have enough space, this function
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
 * @return Returns number of bytes added to buffer. If a timeout occurred, it will be less then
 *         the requested size.
 */
WORD cbufPutArrayWait(CIRBUF* pBuf, const BYTE* pSrcArr, WORD size);


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
WORD cbufMove(CIRBUF* pBufSrc, CIRBUF* pBufDst);


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
WORD cbufGetWrArrSize(CIRBUF* pBuf);


/**
 * Function for getting byte pointer to current PUT location. Use this function with
 * cbufGetWrArrSize() function.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Byte pointer to put buffer.
 */
BYTE* cbufGetWrArr(CIRBUF* pBuf);


/**
 * Increments PUT pointer by given size, taking care of any wrapping required. This function
 * is meant to be used after writing bytes to a CIRBUF buffer's array obtained with cbufGetWrArrSize() function.
 *
 * For <b>Multi Threaded Applications</b>, call from "PUT context".
 *
 * @param pBuf Pointer to CIRBUF structure
 * @param n Size to increment put pointer by
 */
void cbufUpdatePut(CIRBUF* pBuf, WORD n);


/**
 * Get the bErrorFull flag. Use the cbufClearError() function to clear the error flag.
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return The bErrorFull flag
 */
BOOL cbufGetError(CIRBUF* pBuf);


/**
 * Clears all error flags.
 *
 * @param pBuf Pointer to CIRBUF structure
 */
void cbufClearError(CIRBUF* pBuf);


/**
 * Indicates if given buffer is a "Packet Type" buffer.
 * If true, the cbufIsPacketLarge() function can be used to determine if it is a normal or
 * large "Packet Circular Buffer".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns 1 if this is a Packet buffer.
 */
BOOL cbufIsPacket(CIRBUF* pBuf);


/**
 * Indicates if the given buffer is a "Large Packet Type" buffer. This is the case if the bPacketLarge flag is set.
 * <br>!!! IMPORTANT !!!<br>
 * This function does not check it is is a packet, but only the bPacketLarge flag. This flag is only
 * valid if this buffer is a packet type. First use cbufIsPacket() function to check if this buffer is a
 * packet. To check if this buffer is a packet AND if it is a large packet, use:
 * if (cbufIsPacket() && cbufIsPacketLarge()) { .... }
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns 1 if this is Large Packet buffer (has a 16-bit size field).
 */
BOOL cbufIsPacketLarge(CIRBUF* pBuf);


/**
 * Indicates if given buffer contains a whole packet.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 *
 * @return Returns 0 if false, else size of Packet (size of whole packet = [Size] and [Data] part of packet).
 */
WORD cbufHasWholePacket(CIRBUF* pBuf);


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
WORD cbufPacketStrcmp(CIRBUF* pBuf, const char* str);


/**
 * Gets number of free bytes available in buffer for a packet (packet data). This is how
 * much space is available for the data part of a packet. This function can be called
 * before calling cbufPutPacket() to see if there is enough space to add a packet.
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
WORD cbufGetFreeForPacket(CIRBUF* pBuf);


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
WORD cbufGetContiguousPacket(CIRBUF* pBuf, BYTE** ppData);


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
WORD cbufGetPacketDataSize(CIRBUF* pBuf);


/**
 * Return the first byte of the Packet data, without removing it.
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @preCondition This must be a "Packet Buffer", and there MUST be a packet available!
 *  Use cbufHasWholePacket() function to check if there is a packet available!
 *
 * @param pBuf Pointer to CIRBUF circular buffer
 *
 * @return Returns first byte of Packet's data.
 */
BYTE cbufPeekPacketByte(CIRBUF* pBuf);


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
WORD cbufPeekPacketDataSize(CIRBUF* pBuf);


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
WORD cbufPutPacket(CIRBUF* pBuf, const BYTE* pSrcArr, WORD size);


/**
 * Checks if there is a packet in the buffer, and if so, removes it.
 *
 * @preCondition This must be a "Packet Buffer"!
 *
 * For <b>Multi Threaded Applications</b>, call from "GET context".
 *
 * @param pBuf Pointer to CIRBUF structure
 */
void cbufRemovePacket(CIRBUF* pBuf);


/////////////////////////////////////////////////
// Include cirbuf implementation include files

#if !defined(CIRBUF_USE_CIRCULAR_BUFFER_STD) && !defined(CIRBUF_USE_CIRCULAR_BUFFER_PWR2)
//Use nz_circularBufferStd by default if projdefs.h does not define a circular buffer
#define     CIRBUF_USE_CIRCULAR_BUFFER_STD
#endif

#if defined(CIRBUF_USE_CIRCULAR_BUFFER_STD)
    #if defined(CIRBUF_USE_CIRCULAR_BUFFER_PWR2)
    #error "Configured to use multiple Circular Buffers! Only one can be used! Ensure only one CIRBUF_USE_XXX macro is defined (projdefs.h), and it matches circular buffer *.c file included with project!"
    #endif
    #include "nz_circularBufferStd.h"
#endif

#if defined(CIRBUF_USE_CIRCULAR_BUFFER_PWR2)
    #if defined(CIRBUF_USE_CIRCULAR_BUFFER_STD)
    #error "Configured to use multiple Circular Buffers! Only one can be used! Ensure only one CIRBUF_USE_XXX macro is defined (projdefs.h), and it matches circular buffer *.c file included with project!"
    #endif
    #include "nz_circularBufferPwr2.h"
#endif

#endif    /* NZ_CIRCULARBUFFER_H */

