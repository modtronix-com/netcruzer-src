/**
 * @brief           Standard C Libraries - 
 * @file            assert.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section assert_desc Description
 **********************************************************************
 The header file, assert.h, consists of a single macro that is useful for debugging logic
errors in programs. By using the assert statement in critical locations where certain
conditions should be true, the logic of the program may be tested.
Assertion testing may be turned off without removing the code by defining NDEBUG
before including <assert.h>. If the macro NDEBUG is defined, assert() is ignored and
no code is generated.
 
 **********************************************************************
 * @section assert_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */



/**
Description: If the expression is false, an assertion message is printed to stderr and
the program is aborted.

<b>Include:</b> <assert.h>

@param expression The expression to test.

<b>Remarks:</b><br> The expression evaluates to zero or non-zero. If zero, the assertion
fails, and a message is printed to stderr. The message includes the
source file name (__FILE__), the source line number (__LINE__),
the expression being evaluated and the message. The macro then calls
the function abort(). If the macro _VERBOSE_DEBUGGING is defined,
a message will be printed to stderr each time assert() is called.

<b>Example:</b><br>
@code
#include <assert.h> // for assert 

int main(void) {
    int a;
    a = 2 * 2;
    assert(a == 4); // if true-nothing prints 
    assert(a == 6); // if false-print message 
    // and abort 
}
@endcode
<b>Output:</b><br>
sampassert.c:9 a == 6 -- assertion failed <br>
ABRT <br>
with _VERBOSE_DEBUGGING defined: <br>
sampassert.c:8 a == 4 -- OK <br>
sampassert.c:9 a == 6 -- assertion failed <br>
ABRT
 */
void assert(int expression);
