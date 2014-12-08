/**
 * @brief           Standard C Libraries - 
 * @file            limits.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section limits_desc Description
 **********************************************************************
The header file, limits.h, consists of macros that define the minimum and maximum
values of integer types. Each of these macros can be used in 'if' preprocessing
directives.
 
 **********************************************************************
 * @section limits_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */



/**
Description: Number of bits to represent type char.

<b>Include:</b> <limits.h>

<b>Value:</b>  8
 */
#define CHAR_BIT   8



/**
Description: Maximum value of a char.

<b>Include:</b> <limits.h>

<b>Value:</b>  127
 */
#define CHAR_MAX   SCHAR_MAX



/**
Description: Minimum value of a char.

<b>Include:</b> <limits.h>

<b>Value:</b>  -128
 */
#define CHAR_MIN   SCHAR_MIN



/**
Description: Maximum value of an int.

<b>Include:</b> <limits.h>

<b>Value:</b>  32767
 */
#define INT_MAX    SHRT_MAX



/**
Description: Minimum value of an int.

<b>Include:</b> <limits.h>

<b>Value:</b>  -32768
 */
#define INT_MIN    SHRT_MIN



/**
Description: Maximum value of a long long int.

<b>Include:</b> <limits.h>

<b>Value:</b>  9223372036854775807
 */
#define LLONG_MAX  0x7fffffffffffffffLL



/**
Description: Minimum value of a long long int.

<b>Include:</b> <limits.h>

<b>Value:</b>  -9223372036854775808
 */
#define LLONG_MIN  (-LLONG_MAX -1)



/**
Description: Maximum value of a long int.

<b>Include:</b> <limits.h>

<b>Value:</b>  2147483647
 */
#define LONG_MAX   0x7fffffff



/**
Description: Minimum value of a long int.

<b>Include:</b> <limits.h>

<b>Value:</b>  -2147483648
 */
#define LONG_MIN   (-LONG_MAX -1)



/**
Description: Maximum number of bytes in a multibyte character.

<b>Include:</b> <limits.h>

<b>Value:</b>  1
 */
#define MB_LEN_MAX 1



/**
Description: Maximum value of a signed char.

<b>Include:</b> <limits.h>

<b>Value:</b>  127
 */
#define SCHAR_MAX  0x7f



/**
Description: Minimum value of a signed char.

<b>Include:</b> <limits.h>

<b>Value:</b>  -128
 */
#define SCHAR_MIN  (-SCHAR_MAX -1)



/**
Description: Maximum value of a short int.

<b>Include:</b> <limits.h>

<b>Value:</b>  32767
 */
#define SHRT_MAX   0x7fff



/**
Description: Minimum value of a short int.

<b>Include:</b> <limits.h>

<b>Value:</b>  -32768
 */
#define SHRT_MIN   (-SHRT_MAX -1)



/**
Description: Maximum value of an unsigned char.

<b>Include:</b> <limits.h>

<b>Value:</b>  255
 */
#define UCHAR_MAX  0xff



/**
Description: Maximum value of an unsigned int.

<b>Include:</b> <limits.h>

<b>Value:</b>  65535
 */
#define UINT_MAX   USHRT_MAX


/**
Description: Maximum value of a long long unsigned int.

<b>Include:</b> <limits.h>

<b>Value:</b>  18446744073709551615
 */
#define ULLONG_MAX 0xffffffffffffffffULL



/**
Description: Maximum value of a long unsigned int.

<b>Include:</b> <limits.h>

<b>Value:</b>  4294967295
 */
#define ULONG_MAX  0xffffffffU



/**
Description: Maximum value of an unsigned short int.

<b>Include:</b> <limits.h>

<b>Value:</b>  65535
 */
#define USHRT_MAX  0xffff
