/**
 * @brief           Standard C Libraries - 
 * @file            setjmp.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section setjmp_desc Description
 **********************************************************************
The header file, setjmp.h, consists of a type, a macro and a function that allow control
transfers to occur that bypass the normal function call and return process.
 
 **********************************************************************
 * @section setjmp_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */
 

 
/**
Description: A type that is an array used by setjmp and longjmp to save and
restore the program environment.

<b>Include:</b> <setjmp.h>

<b>Remarks:</b><br> _NSETJMP is defined as 16 + 2 that represents 16 registers and a
32-bit return address.
*/
typedef unsigned int jmp_buf[18];



/**
Description: A macro that saves the current state of the program for later use by
longjmp.

<b>Include:</b> <setjmp.h>

@param env variable where environment is stored

@return If the return is from a direct call, setjmp returns zero. If the return is
from a call to longjmp, setjmp returns a non-zero value.
Note: If the argument val from longjmp is 0, setjmp returns 1.

<b>Example:</b><br>
See longjmp.
*/
extern	int	setjmp(jmp_buf env);



/**
Description: A function that restores the environment saved by setjmp.

<b>Include:</b> <setjmp.h>

@param env variable where environment is stored

@param val value to be returned to setjmp call.

<b>Remarks:</b><br> The value parameter, val, should be non-zero. If longjmp is invoked
from a nested signal handler (that is, invoked as a result of a signal
raised during the handling of another signal), the behavior is undefined.
*/
extern void	longjmp(jmp_buf env, int val);