/**
 * @brief           Standard C Libraries - 
 * @file            errno.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section errno_desc Description
 **********************************************************************
The header file, errno.h, consists of macros that provide error codes that are reported
by certain library functions (see individual functions). The variable, errno, may return
any value greater than zero. To test if a library function encounters an error, the
program should store the zero value in errno immediately before calling the library
function. The value should be checked before another function call could change the
value. At program start-up, errno is zero. Library functions will never set errno to zero.
 
 **********************************************************************
 * @section errno_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */


/**
Description: Represents a domain error.

<b>Include:</b> <errno.h>

<b>Remarks:</b><br> EDOM represents a domain error, which occurs when an input argument
is outside the domain in which the function is defined.
 */
#define    ERANGE    34    //Range error


/**
Description: Represents an overflow or underflow error.

<b>Include:</b> <errno.h>

<b>Remarks:</b><br> ERANGE represents an overflow or underflow error, which occurs when
a result is too large or too small to be stored.
 */
extern int errno;


/**
Description: Contains the value of an error when an error occurs in a function.

<b>Include:</b> <errno.h>

<b>Remarks:</b><br> The variable, errno, is set to a non-zero integer value by a library function
when an error occurs. At program start-up, errno is set to zero.
Errno should be reset to zero prior to calling a function that sets it.
 */
#define    EDOM    33    //Domain error
