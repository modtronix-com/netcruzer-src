/**
 * @brief           Standard C Libraries - 
 * @file            stdarg.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section stdarg_desc Description
 **********************************************************************
The header file, stdarg.h, supports functions with variable argument lists. This allows
functions to have arguments without corresponding parameter declarations. There
must be at least one named argument. The variable arguments are represented by
ellipses (...). An object of type, va_list, must be declared inside the function to hold
the arguments. va_start will initialize the variable to an argument list, va_arg will
access the argument list, and va_end will end the use of the argument.
 
 **********************************************************************
 * @section stdarg_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */



/**
Description: The type va_list declares a variable that will refer to each argument
in a variable-length argument list.

<b>Include:</b> <stdarg.h>

<b>Example:</b><br>
See va_arg.
 */
//#define va_list



/**
Description: Gets the current argument.

<b>Include:</b> <stdarg.h>

@param ap pointer to list of arguments

@param Ty type of argument to be retrieved

@return Returns the current argument

<b>Remarks:</b><br> va_start must be called before va_arg.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdarg.h> // for va_arg, va_start,

va_list, va_end
void tprint(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    while (*fmt) {
        switch (*fmt) {
            case '%':
                fmt++;
                if (*fmt == 'd') {
                    int d = va_arg(ap, int);
                    printf("<%d> is an integer\n", d);
                } else if (*fmt == 's') {
                    char *s = va_arg(ap, char*);
                    printf("<%s> is a string\n", s);
                } else {
                    printf("%%%c is an unknown format\n",
                            *fmt);
                }
                fmt++;
                break;
            default:
                printf("%c is unknown\n", *fmt);
                fmt++;
                break;
        }
    }
    va_end(ap);
}

int main(void) {
    tprint("%d%s.%c", 83, "This is text.", 'a');
}
@endcode
<b>Output:</b><br>
<83> is an integer <br>
<This is text.> is a string <br>
. is unknown <br>
\%c is an unknown format <br>
 */
#define va_arg(ap, Ty)	\
	(AP = (void *) ((char *) (AP) - __va_rounded_size (TYPE)), \
        *((TYPE *) (void *) ((char *) (AP) )))



/**
Description: Ends the use of ap.

<b>Include:</b> <stdarg.h>

@param ap pointer to list of arguments

<b>Remarks:</b><br> After a call to va_end, the argument list pointer, ap, is considered to be
invalid. Further calls to va_arg should not be made until the next
va_start. In the 16-bit compiler, va_end does nothing, so this call is
not necessary but should be used for readability and portability.

<b>Example:</b><br>
See va_arg.
 */
#define va_end(ap)		(void)0



/**
Description: Sets the argument pointer ap to first optional argument in the
variable-length argument list.

<b>Include:</b> <stdarg.h>

@param ap pointer to list of arguments

@param last_arg last named argument before the optional arguments

<b>Example:</b><br>
See va_arg.
 */
#define va_start(ap, last_arg) AP=(void *) __builtin_next_arg (LASTARG)