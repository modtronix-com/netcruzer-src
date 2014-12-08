/**
 * @brief           Standard C Libraries - 
 * @file            signal.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section signal_desc Description
 **********************************************************************
The header file, signal.h, consists of a type, several macros and two functions that
specify how the program handles signals while it is executing. A signal is a condition
that may be reported during the program execution. Signals are synchronous,
occurring under software control via the raise function.
A signal may be handled by:
- Default handling (SIG_DFL); the signal is treated as a fatal error and execution stops
- Ignoring the signal (SIG_IGN); the signal is ignored and control is returned to the user application
- Handling the signal with a function designated via signal.

By default, all signals are handled by the default handler, which is identified by
SIG_DFL.
The type sig_atomic_t is an integer type that the program access atomically. When
this type is used with the keyword volatile, the signal handler can share the data
objects with the rest of the program.
 
 **********************************************************************
 * @section signal_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */



/**
Description: A type used by a signal handler.

<b>Include:</b> <signal.h>
 */
typedef int sig_atomic_t;



/**
Description: Used as the second argument and/or the return value for signal to
specify that the default handler should be used for a specific signal.

<b>Include:</b> <signal.h>
 */
#define SIG_DFL	((void (*)(int))0)



/**
Description: Used as the return value for signal when it cannot complete a
request due to an error.

<b>Include:</b> <signal.h>
 */
#define SIG_ERR ((void (*)(int))-1)



/**
Description: Used as the second argument and/or the return value for signal to
specify that the signal should be ignored.

<b>Include:</b> <signal.h>
 */
#define SIG_IGN	((void (*)(int))1)



/**
Description: Name for the abnormal termination signal.

<b>Include:</b> <signal.h>

<b>Remarks:</b><br> SIGABRT represents an abnormal termination signal and is used in
conjunction with raise or signal. The default raise behavior
(action identified by SIG_DFL) is to output to the standard error stream:
abort - terminating
See the example accompanying signal to see general usage of
signal names and signal handling.

<b>Example:</b><br>
@code
#include <signal.h> // for raise, SIGABRT 
#include <stdio.h> // for printf 

int main(void) {
    raise(SIGABRT);
    printf("Program never reaches here.");
}
@endcode
<b>Output:</b><br>
ABRT

Explanation: <br>
ABRT stands for "abort".
 */
#define SIGABRT 1



/**
Description: Signals floating-point, error such as for division by zero or result out of
range.

<b>Include:</b> <signal.h>

<b>Remarks:</b><br> SIGFPE is used as an argument for raise and/or signal. When
used, the default behavior is to print an arithmetic error message and
terminate the calling program. This may be overridden by a user
function that defines the signal handler actions. See signal for an
example of a user-defined function.

<b>Example:</b><br>
@code
#include <signal.h> // for raise, SIGFPE 
#include <stdio.h> // for printf 

int main(void) {
    raise(SIGFPE);
    printf("Program never reaches here");
}
@endcode
<b>Output:</b><br>
FPE

Explanation: <br>
FPE stands for "floating-point error".
 */
#define SIGFPE 4



/**
Description: Signals illegal instruction.

<b>Include:</b> <signal.h>

<b>Remarks:</b><br> SIGILL is used as an argument for raise and/or signal. When
used, the default behavior is to print an invalid executable code message
and terminate the calling program. This may be overridden by a
user function that defines the signal handler actions. See signal for
an example of a user-defined function.

<b>Example:</b><br>
@code
#include <signal.h> // for raise, SIGILL 
#include <stdio.h> // for printf 

int main(void) {
    raise(SIGILL);
    printf("Program never reaches here");
}
@endcode
<b>Output:</b><br>
ILL

Explanation: <br>
ILL stands for "illegal instruction".
 */
#define SIGILL 3



/**
Description: Interrupt signal.

<b>Include:</b> <signal.h>

<b>Remarks:</b><br> SIGINT is used as an argument for raise and/or signal. When
used, the default behavior is to print an interruption message and terminate
the calling program. This may be overridden by a user function
that defines the signal handler actions. See signal for an example of
a user-defined function.

<b>Example:</b><br>
@code
#include <signal.h> // for raise, SIGINT 
#include <stdio.h> // for printf 

int main(void) {
    raise(SIGINT);
    printf("Program never reaches here.");
}
@endcode
<b>Output:</b><br>
INT

Explanation: <br>
INT stands for "interruption".
 */
#define SIGINT 2



/**
Description: Signals invalid access to storage.

<b>Include:</b> <signal.h>

<b>Remarks:</b><br> SIGSEGV is used as an argument for raise and/or signal. When
used, the default behavior is to print an invalid storage request message
and terminate the calling program. This may be overridden by a
user function that defines the signal handler actions. See signal for
an example of a user-defined function.

<b>Example:</b><br>
@code
#include <signal.h> // for raise, SIGSEGV 
#include <stdio.h> // for printf 

int main(void) {
    raise(SIGSEGV);
    printf("Program never reaches here.");
}
@endcode
<b>Output:</b><br>
SEGV

Explanation: <br>
SEGV stands for "invalid storage access".
 */
#define SIGSEGV 5



/**
Description: Signals a termination request.

<b>Include:</b> <signal.h>

<b>Remarks:</b><br> SIGTERM is used as an argument for raise and/or signal. When
used, the default behavior is to print a termination request message
and terminate the calling program. This may be overridden by a user
function that defines the signal handler actions. See signal for an
example of a user-defined function.

<b>Example:</b><br>
@code
#include <signal.h> // for raise, SIGTERM 
#include <stdio.h> // for printf 

int main(void) {
    raise(SIGTERM);
    printf("Program never reaches here.");
}
@endcode
<b>Output:</b><br>
TERM

Explanation: <br>
TERM stands for "termination request".
 */
#define SIGTERM 6



/**
Description: Reports a synchronous signal.

<b>Include:</b> <signal.h>

@param sig signal name

@return Returns a 0 if successful; otherwise, returns a non-zero value.

<b>Remarks:</b><br> raise sends the signal identified by sig to the executing program.

<b>Example:</b><br>
@code
#include <signal.h> // for raise, signal, 
// SIGILL, SIG_DFL 
#include <stdlib.h> // for div, div_t 
#include <stdio.h> // for printf 
#include <p30f6014.h> // for INTCON1bits 

void __attribute__((__interrupt__))
_MathError(void) {
    raise(SIGILL);
    INTCON1bits.MATHERR = 0;
}

void illegalinsn(int idsig) {
    printf("Illegal instruction executed\n");
    exit(1);
}

int main(void) {
    int x, y;
    div_t z;
    signal(SIGILL, illegalinsn);
    x = 7;
    y = 0;
    z = div(x, y);
    printf("Program never reaches here");
}
@endcode
<b>Output:</b><br>
Illegal instruction executed

Explanation: <br>
This example requires the linker script, p30f6014.gld. There are
three parts to this example.
- First, an interrupt handler is written for the interrupt vector,
_MathError, to handle a math error by sending an illegal instruction,
signal (SIGILL), to the executing program. The last statement in 
the interrupt handler clears, the exception flag.
- Second, the function, illegalinsn, will print an error message and call exit.
- Third, in main, signal (SIGILL, illegalinsn) sets the handler
for SIGILL to the function, illegalinsn.

When a math error occurs, due to a divide by zero, the _MathError
interrupt vector is called, which in turn, will raise a signal that will call
the handler function for SIGILL, which is the function, illegalinsn.
Thus, error messages are printed and the program is terminated.
 */
int raise(int sig);



/**
Description: Controls interrupt signal handling.

<b>Include:</b> <signal.h>

@param sig signal name

@param func function to be executed

@return Returns the previous value of func.

<b>Example:</b><br>
@code
#include <signal.h> // for signal, raise, 
// SIGINT, SIGILL, 
// SIG_IGN, and SIGFPE 
#include <stdio.h> // for printf 
// Signal handler function 

void mysigint(int id) {
    printf("SIGINT received\n");
}

int main(void) {
    // Override default with user defined function 
    signal(SIGINT, mysigint);
    raise(SIGINT);
    // Ignore signal handler 
    signal(SIGILL, SIG_IGN);
    raise(SIGILL);
    printf("SIGILL was ignored\n");
    // Use default signal handler 
    raise(SIGFPE);
    printf("Program never reaches here.");
}
@endcode
<b>Output:</b><br>
SIGINT received <br>
SIGILL was ignored <br>
FPE

Explanation:<br>
The function, mysigint, is the user-defined signal handler for
SIGINT. Inside the main program, the function, signal, is called to set
up the signal handler (mysigint) for the signal SIGINT that will
override the default actions. The function, raise, is called to report the
signal SIGINT. This causes the signal handler for SIGINT to use the
user-defined function (mysigint) as the signal handler so it prints the
"SIGINT received" message.

Next, the function, signal, is called to set up the signal handler
SIG_IGN for the signal SIGILL. The constant SIG_IGN is used to indicate
the signal should be ignored. The function, raise, is called to
report the signal, SIGILL that is ignored.

The function, raise, is called again to report the signal, SIGFPE.
Since there is no user-defined function for SIGFPE, the default signal
handler is used so the message "FPE" is printed (which stands for
"arithmetic error - terminating"). Then, the calling program
is terminated. The printf statement is never reached.
 */
void (*signal(int sig, void(*func)(int)))(int);