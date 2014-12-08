/**
 * @brief           Standard C Libraries - 
 * @file            float.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section float_desc Description
 **********************************************************************
The header file, float.h, consists of macros that specify various properties of
floating-point types. These properties include number of significant figures, size limits
and what rounding mode is used.
 
 **********************************************************************
 * @section float_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */


/**
Description: Number of decimal digits of precision in a double precision
floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  6 by default, 15 if the switch -fno-short-double is used

<b>Remarks:</b><br> By default, a double type is the same size as a float type (32-bit representation).
The -fno-short-double switch allows the IEEE 64-bit
representation to be used for a double precision floating-point value.
 */
#define DBL_DIG         __DBL_DIG__



/**
Description: The difference between 1.0 and the next larger representable double
precision floating-point value

<b>Include:</b> <float.h>

<b>Value:</b>  1.192093e-07 by default, 2.220446e-16 if the switch
-fno-short-double is used

<b>Remarks:</b><br> By default, a double type is the same size as a float type (32-bit representation).
The -fno-short-double switch allows the IEEE 64-bit
representation to be used for a double precision floating-point value.
 */
#define DBL_EPSILON     __DBL_EPSILON__



/**
Description: Number of base-FLT_RADIX digits in a double precision floating-point
significand.

<b>Include:</b> <float.h>

<b>Value:</b>  24 by default, 53 if the switch -fno-short-double is used

<b>Remarks:</b><br> By default, a double type is the same size as a float type (32-bit representation).
The -fno-short-double switch allows the IEEE 64-bit
representation to be used for a double precision floating-point value.
 */
#define DBL_MANT_DIG    __DBL_MANT_DIG__



/**
Description: Maximum finite double precision floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  3.402823e+38 by default, 1.797693e+308 if the switch
-fno-short-double is used

<b>Remarks:</b><br> By default, a double type is the same size as a float type (32-bit representation).
The -fno-short-double switch allows the IEEE 64-bit
representation to be used for a double precision floating-point value.
 */
#define DBL_MAX         __DBL_MAX__



/**
Description: Maximum integer value for a double precision floating-point exponent in
base 10.

<b>Include:</b> <float.h>

<b>Value:</b>  38 by default, 308 if the switch -fno-short-double is used

<b>Remarks:</b><br> By default, a double type is the same size as a float type (32-bit representation).
The -fno-short-double switch allows the IEEE 64-bit
representation to be used for a double precision floating-point value.
 */
#define DBL_MAX_10_EXP  __DBL_MAX_10_EXP__



/**
Description: Maximum integer value for a double precision floating-point exponent in
base FLT_RADIX.

<b>Include:</b> <float.h>

<b>Value:</b>  128 by default, 1024 if the switch -fno-short-double is used

<b>Remarks:</b><br> By default, a double type is the same size as a float type (32-bit representation).
The -fno-short-double switch allows the IEEE 64-bit
representation to be used for a double precision floating-point value.
 */
#define DBL_MAX_EXP     __DBL_MAX_EXP__



/**
Description: Minimum double precision floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  1.175494e-38 by default, 2.225074e-308 if the switch
-fno-short-double is used

<b>Remarks:</b><br> By default, a double type is the same size as a float type (32-bit representation).
The -fno-short-double switch allows the IEEE 64-bit
representation to be used for a double precision floating-point value.
 */
#define DBL_MIN         __DBL_MIN__




/**
Description: Minimum negative integer value for a double precision floating-point
exponent in base 10.

<b>Include:</b> <float.h>

<b>Value:</b>  -37 by default, -307 if the switch -fno-short-double is used

<b>Remarks:</b><br> By default, a double type is the same size as a float type (32-bit representation).
The -fno-short-double switch allows the IEEE 64-bit
representation to be used for a double precision floating-point value.
 */
#define DBL_MIN_10_EXP  __DBL_MIN_10_EXP__




/**
Description: Minimum negative integer value for a double precision floating-point
exponent in base FLT_RADIX.

<b>Include:</b> <float.h>

<b>Value:</b>  -125 by default, -1021 if the switch -fno-short-double is used

<b>Remarks:</b><br>By default, a double type is the same size as a float type (32-bit representation).
The -fno-short-double switch allows the IEEE 64-bit
representation to be used for a double precision floating-point value.
 */
#define DBL_MIN_EXP     __DBL_MIN_EXP__



/**
Description: Number of decimal digits of precision in a single precision floating-point
value.

<b>Include:</b> <float.h>

<b>Value:</b>  6
 */
#define FLT_DIG         __FLT_DIG__



/**
Description: The difference between 1.0 and the next larger representable single
precision floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  1.192093e-07
 */
#define FLT_EPSILON     __FLT_EPSILON__



/**
Description: Number of base-FLT_RADIX digits in a single precision floating-point
significand.

<b>Include:</b> <float.h>

<b>Value:</b>  24
 */
#define FLT_MANT_DIG    __FLT_MANT_DIG__



/**
Description: Maximum finite single precision floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  3.402823e+38
 */
#define FLT_MAX         __FLT_MAX__



/**
Description: Maximum integer value for a single precision floating-point exponent in
base 10.

<b>Include:</b> <float.h>

<b>Value:</b>  38
 */
#define FLT_MAX_10_EXP  __FLT_MAX_10_EXP__



/**
Description: Maximum integer value for a single precision floating-point exponent in
base FLT_RADIX.

<b>Include:</b> <float.h>

<b>Value:</b>  128
 */
#define FLT_MAX_EXP     __FLT_MAX_EXP__



/**
Description: Minimum single precision floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  1.175494e-38
 */
#define FLT_MIN         __FLT_MIN__



/**
Description: Minimum negative integer value for a single precision floating-point
exponent in base 10.

<b>Include:</b> <float.h>

<b>Value:</b>  -37
 */
#define FLT_MIN_10_EXP  __FLT_MIN_10_EXP__



/**
Description: Minimum negative integer value for a single precision floating-point
exponent in base FLT_RADIX.

<b>Include:</b> <float.h>

<b>Value:</b>  -125
 */
#define FLT_MIN_EXP     __FLT_MIN_EXP__



/**
Description: Radix of exponent representation.

<b>Include:</b> <float.h>

<b>Value:</b>  2

<b>Remarks:</b><br> The base representation of the exponent is base-2 or binary.
 */
#define FLT_RADIX       2



/**
Description: Represents the rounding mode for floating-point operations.

<b>Include:</b> <float.h>

<b>Value:</b>  1

<b>Remarks:</b><br> Rounds to the nearest representable value.
 */
#define FLT_ROUNDS      1



/**
Description: Number of decimal digits of precision in a long double precision
floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  15
 */
#define LDBL_DIG        __LDBL_DIG__



/**
Description: The difference between 1.0 and the next larger representable long
double precision floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  2.220446e-16
 */
#define LDBL_EPSILON    __LDBL_EPSILON__



/**
Description: Number of base-FLT_RADIX digits in a long double precision
floating-point significand.

<b>Include:</b> <float.h>

<b>Value:</b>  53
 */
#define LDBL_MANT_DIG   __LDBL_MANT_DIG__



/**
Description: Maximum finite long double precision floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  1.797693e+308
 */
#define LDBL_MAX        __LDBL_MAX__



/**
Description: Maximum integer value for a long double precision floating-point
exponent in base 10.

<b>Include:</b> <float.h>

<b>Value:</b>  308
 */
#define LDBL_MAX_10_EXP __LDBL_MAX_10_EXP__



/**
Description: Maximum integer value for a long double precision floating-point
exponent in base FLT_RADIX.

<b>Include:</b> <float.h>

<b>Value:</b>  1024
 */
#define LDBL_MAX_EXP    __LDBL_MAX_EXP__



/**
Description: Minimum long double precision floating-point value.

<b>Include:</b> <float.h>

<b>Value:</b>  2.225074e-308
 */
#define LDBL_MIN        __LDBL_MIN__



/**
Description: Minimum negative integer value for a long double precision
floating-point exponent in base 10.

<b>Include:</b> <float.h>

<b>Value:</b>  -307
 */
#define LDBL_MIN_10_EXP __LDBL_MIN_10_EXP__



/**
Description: Minimum negative integer value for a long double precision
floating-point exponent in base FLT_RADIX.

<b>Include:</b> <float.h>

<b>Value:</b>  -1021
 */
#define LDBL_MIN_EXP    __LDBL_MIN_EXP__
