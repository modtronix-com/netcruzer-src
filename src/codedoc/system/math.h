/**
 * @brief           Standard C Libraries - Math Functions
 * @file            math.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section math_desc Description
 **********************************************************************
 
The header file, math.h, consists of a macro and various functions that calculate common
mathematical operations. Error conditions may be handled with a domain error or
range error (see errno.h).
A domain error occurs when the input argument is outside the domain over which the
function is defined. The error is reported by storing the value of EDOM in errno and
returning a particular value defined for each function.
A range error occurs when the result is too large or too small to be represented in the
target precision. The error is reported by storing the value of ERANGE in errno and
returning HUGE_VAL if the result overflowed (return value was too large) or a zero if the
result underflowed (return value is too small).
Responses to special values, such as NaNs, zeros and infinities, may vary depending
upon the function. Each function description includes a definition of the function’s
response to such values. 
 
 **********************************************************************
 * @section math_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */

/**
Description: Calculates the trigonometric arc cosine function of a double precision
floating-point value.

<b>Include:</b> <math.h>

@param x value between -1 and 1 for which to return the arc cosine

@return Returns the arc cosine in radians in the range of 0 to pi (inclusive).

<b>Remarks:</b><br> A domain error occurs if x is less than -1 or greater than 1.

<b>Example:</b><br>
@code
#include <math.h> // for acos
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = -2.0;
    y = acos(x);
    if (errno)
        perror("Error");
    printf("The arccosine of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.10;
    y = acos(x);
    if (errno)
        perror("Error");
    printf("The arccosine of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
Error: domain error<br>
The arccosine of -2.000000 is nan<br>
The arccosine of 0.100000 is 1.470629
 */
double acos(double x);

/**
Description: Calculates the trigonometric arc cosine function of a single precision
floating-point value.

<b>Include:</b> <math.h>

@param x value between -1 and 1

@return Returns the arc cosine in radians in the range of 0 to pi (inclusive).

<b>Remarks:</b><br> A domain error occurs if x is less than -1 or greater than 1.

<b>Example:</b><br>
@code
#include <math.h> // for acosf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    errno = 0;
    x = 2.0F;
    y = acosf(x);
    if (errno)
        perror("Error");
    printf("The arccosine of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.0F;
    y = acosf(x);
    if (errno)
        perror("Error");
    printf("The arccosine of %f is %f\n", x, y);
}
@endcode
<b>Output:</b><br>
Error: domain error<br>
The arccosine of 2.000000 is nan<br>
The arccosine of 0.000000 is 1.570796
 */
float acosf(float x);

/**
Description: Calculates the trigonometric arc sine function of a double precision
floating-point value.

<b>Include:</b> <math.h>

@param x value between -1 and 1 for which to return the arc sine

@return Returns the arc sine in radians in the range of -pi/2 to +pi/2 (inclusive).

<b>Remarks:</b><br> A domain error occurs if x is less than -1 or greater than 1.

<b>Example:</b><br>
@code
#include <math.h> // for asin
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = 2.0;
    y = asin(x);
    if (errno)
        perror("Error");
    printf("The arcsine of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.0;
    y = asin(x);
    if (errno)
        perror("Error");
    printf("The arcsine of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
Error: domain error<br>
The arcsine of 2.000000 is nan<br>
The arcsine of 0.000000 is 0.000000
 */
double asin(double x);

/**
Description: Calculates the trigonometric arc sine function of a single precision
floating-point value.

<b>Include:</b> <math.h>

@param x value between -1 and 1

@return Returns the arc sine in radians in the range of -pi/2 to +pi/2 (inclusive).

<b>Remarks:</b><br> A domain error occurs if x is less than -1 or greater than 1.

<b>Example:</b><br>
@code
#include <math.h> // for asinf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    errno = 0;
    x = 2.0F;
    y = asinf(x);
    if (errno)
        perror("Error");
    printf("The arcsine of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.0F;
    y = asinf(x);
    if (errno)
        perror("Error");
    printf("The arcsine of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
Error: domain error<br>
The arcsine of 2.000000 is nan<br>
The arcsine of 0.000000 is 0.000000
 */
float asinf(float x);

/**
Description: Calculates the trigonometric arc tangent function of a double precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the arc tangent

@return Returns the arc tangent in radians in the range of -pi/2 to +pi/2
(inclusive).

<b>Remarks:</b><br> No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for atan
#include <stdio.h> // for printf
int main(void)
{
    double x, y;
    x = 2.0;
    y = atan(x);
    printf("The arctangent of %f is %f\n\n", x, y);
    x = -1.0;
    y = atan(x);
    printf("The arctangent of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The arctangent of 2.000000 is 1.107149<br>
The arctangent of -1.000000 is -0.785398
 */
double atan(double x);

/**
Description: Calculates the trigonometric arc tangent function of a single precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the arc tangent

@return Returns the arc tangent in radians in the range of -pi/2 to +pi/2
(inclusive).

<b>Remarks:</b><br> No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for atanf
#include <stdio.h> // for printf
int main(void)
{
    float x, y;
    x = 2.0F;
    y = atanf(x);
    printf("The arctangent of %f is %f\n\n", x, y);
    x = -1.0F;
    y = atanf(x);
    printf("The arctangent of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The arctangent of 2.000000 is 1.107149<br>
The arctangent of -1.000000 is -0.785398
 */
float atanf(float x);

/**
Description: Calculates the trigonometric arc tangent function of y/x.

<b>Include:</b> <math.h>

@param y value for which to return the arc tangent

@param x value for which to return the arc tangent

@return Returns the arc tangent in radians in the range of -pi to pi (inclusive)
with the quadrant determined by the signs of both parameters.

<b>Remarks:</b><br> A domain error occurs if both x and y are zero or both x and y are
+/- infinity.

<b>Example:</b><br>
@code
#include <math.h> // for atan2
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y, z;
    errno = 0;
    x = 0.0;
    y = 2.0;
    z = atan2(y, x);
    if (errno)
        perror("Error");
    printf("The arctangent of %f/%f is %f\n\n",
            y, x, z);
    errno = 0;
    x = -1.0;
    y = 0.0;
    z = atan2(y, x);
    if (errno)
        perror("Error");
    printf("The arctangent of %f/%f is %f\n\n",
            y, x, z);
    errno = 0;
    x = 0.0;
    y = 0.0;
    z = atan2(y, x);
    if (errno)
        perror("Error");
    printf("The arctangent of %f/%f is %f\n\n",
            y, x, z);
}
@endcode
<b>Output:</b><br>
The arctangent of 2.000000/0.000000 is 1.570796<br>
The arctangent of 0.000000/-1.000000 is 3.141593<br>
Error: domain error<br>
The arctangent of 0.000000/0.000000 is nan
 */
double atan2(double y, double x);

/**
Description: Calculates the trigonometric arc tangent function of y/x.

<b>Include:</b> <math.h>

@param y value for which to return the arc tangent

@param x value for which to return the arc tangent

@return Returns the arc tangent in radians in the range of -pi to pi with the
quadrant determined by the signs of both parameters.

<b>Remarks:</b><br> A domain error occurs if both x and y are zero or both x and y are
+/- infinity.

<b>Example:</b><br>
@code
#include <math.h> // for atan2f
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y, z;
    errno = 0;
    x = 2.0F;
    y = 0.0F;
    z = atan2f(y, x);
    if (errno)
        perror("Error");
    printf("The arctangent of %f/%f is %f\n\n",
            y, x, z);
    errno = 0;
    x = 0.0F;
    y = -1.0F;
    z = atan2f(y, x);
    if (errno)
        perror("Error");
    printf("The arctangent of %f/%f is %f\n\n",
            y, x, z);
    errno = 0;
    x = 0.0F;
    y = 0.0F;
    z = atan2f(y, x);
    if (errno)
        perror("Error");
    printf("The arctangent of %f/%f is %f\n\n",
            y, x, z);
}
@endcode
<b>Output:</b><br>
The arctangent of 2.000000/0.000000 is 1.570796<br>
The arctangent of 0.000000/-1.000000 is 3.141593<br>
Error: domain error<br>
The arctangent of 0.000000/0.000000 is nan
 */
float atan2f(float y, float x);

/**
Description: Calculates the ceiling of a value.

<b>Include:</b> <math.h>

@param x a floating-point value for which to return the ceiling

@return Returns the smallest integer value greater than or equal to x.

<b>Remarks:</b><br> No domain or range error will occur. See floor.

<b>Example:</b><br>
@code
#include <math.h> // for ceil
#include <stdio.h> // for printf
int main(void)
{
    double x[8] = {2.0, 1.75, 1.5, 1.25, -2.0,
        -1.75, -1.5, -1.25};
    double y;
    int i;
    for (i = 0; i < 8; i++) {
        y = ceil(x[i]);
        printf("The ceiling for %f is %f\n", x[i], y);
    }
}
@endcode
<b>Output:</b><br>
The ceiling for 2.000000 is 2.000000<br>
The ceiling for 1.750000 is 2.000000<br>
The ceiling for 1.500000 is 2.000000<br>
The ceiling for 1.250000 is 2.000000<br>
The ceiling for -2.000000 is -2.000000<br>
The ceiling for -1.750000 is -1.000000<br>
The ceiling for -1.500000 is -1.000000<br>
The ceiling for -1.250000 is -1.000000<br>
 */
double ceil(double x);

/**
Description: Calculates the ceiling of a value.

<b>Include:</b> <math.h>

@param x floating-point value

@return Returns the smallest integer value greater than or equal to x.

<b>Remarks:</b><br> No domain or range error will occur. See floorf.

<b>Example:</b><br>
@code
#include <math.h> // for ceilf
#include <stdio.h> // for printf
int main(void)
{
    float x[8] = {2.0F, 1.75F, 1.5F, 1.25F,
        -2.0F, -1.75F, -1.5F, -1.25F};
    float y;
    int i;
    for (i = 0; i < 8; i++) {
        y = ceilf(x[i]);
        printf("The ceiling for %f is %f\n", x[i], y);
    }
}
@endcode
<b>Output:</b><br>
The ceiling for 2.000000 is 2.000000<br>
The ceiling for 1.750000 is 2.000000<br>
The ceiling for 1.500000 is 2.000000<br>
The ceiling for 1.250000 is 2.000000<br>
The ceiling for -2.000000 is -2.000000<br>
The ceiling for -1.750000 is -1.000000<br>
The ceiling for -1.500000 is -1.000000<br>
The ceiling for -1.250000 is -1.000000
 */
float ceilf(float x);

/**
Description: Calculates the trigonometric cosine function of a double precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the cosine

@return Returns the cosine of x in radians in the ranges of -1 to 1 inclusive.

<b>Remarks:</b><br> A domain error will occur if x is a NaN or infinity.

<b>Example:</b><br>
@code
#include <math.h> // for cos
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = -1.0;
    y = cos(x);
    if (errno)
        perror("Error");
    printf("The cosine of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.0;
    y = cos(x);
    if (errno)
        perror("Error");
    printf("The cosine of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The cosine of -1.000000 is 0.540302<br>
The cosine of 0.000000 is 1.000000
 */
double cos(double x);

/**
Description: Calculates the trigonometric cosine function of a single precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the cosine

@return Returns the cosine of x in radians in the ranges of -1 to 1 inclusive.

<b>Remarks:</b><br> A domain error will occur if x is a NaN or infinity.

<b>Example:</b><br>
@code
#include <math.h> // for cosf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    errno = 0;
    x = -1.0F;
    y = cosf(x);
    if (errno)
        perror("Error");
    printf("The cosine of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.0F;
    y = cosf(x);
    if (errno)
        perror("Error");
    printf("The cosine of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The cosine of -1.000000 is 0.540302<br>
The cosine of 0.000000 is 1.000000
 */
float cosf(float x);

/**
Description: Calculates the hyperbolic cosine function of a double precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the hyperbolic cosine

@return Returns the hyperbolic cosine of x.

<b>Remarks:</b><br> A range error will occur if the magnitude of x is too large.

<b>Example:</b><br>
@code
#include <math.h> // for cosh
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = -1.5;
    y = cosh(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic cosine of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 0.0;
    y = cosh(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic cosine of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 720.0;
    y = cosh(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic cosine of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The hyperbolic cosine of -1.500000 is 2.352410<br>
The hyperbolic cosine of 0.000000 is 1.000000<br>
Error: range error<br>
The hyperbolic cosine of 720.000000 is inf
 */
double cosh(double x);

/**
Description: Calculates the hyperbolic cosine function of a single precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the hyperbolic cosine

@return Returns the hyperbolic cosine of x.

<b>Remarks:</b><br> A range error will occur if the magnitude of x is too large.

<b>Example:</b><br>
@code
#include <math.h> // for coshf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    errno = 0;
    x = -1.0F;
    y = coshf(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic cosine of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 0.0F;
    y = coshf(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic cosine of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 720.0F;
    y = coshf(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic cosine of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The hyperbolic cosine of -1.000000 is 1.543081<br>
The hyperbolic cosine of 0.000000 is 1.000000<br>
Error: range error<br>
The hyperbolic cosine of 720.000000 is inf<br>
 */
float coshf(float x);

/**
Description: Calculates the exponential function of x (e raised to the power x where
x is a double precision floating-point value).

<b>Include:</b> <math.h>

@param x value for which to return the exponential

@return Returns the exponential of x. On an overflow, exp returns inf and on
an underflow exp returns 0.

<b>Remarks:</b><br> A range error occurs if the magnitude of x is too large.

<b>Example:</b><br>
@code
#include <math.h> // for exp
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = 1.0;
    y = exp(x);
    if (errno)
        perror("Error");
    printf("The exponential of %f is %f\n\n", x, y);
    errno = 0;
    x = 1E3;
    y = exp(x);
    if (errno)
        perror("Error");
    printf("The exponential of %f is %f\n\n", x, y);
    errno = 0;
    x = -1E3;
    y = exp(x);
    if (errno)
        perror("Error");
    printf("The exponential of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The exponential of 1.000000 is 2.718282<br>
Error: range error<br>
The exponential of 1000.000000 is inf<br>
Error: range error<br>
The exponential of -1000.000000 is 0.000000
 */
double exp(double x);

/**
Description: Calculates the exponential function of x (e raised to the power x where
x is a single precision floating-point value).

<b>Include:</b> <math.h>

@param x floating-point value for which to return the exponential

@return Returns the exponential of x. On an overflow, expf returns inf and on
an underflow exp returns 0.

<b>Remarks:</b><br> A range error occurs if the magnitude of x is too large.

<b>Example:</b><br>
@code
#include <math.h> // for expf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    errno = 0;
    x = 1.0F;
    y = expf(x);
    if (errno)
        perror("Error");
    printf("The exponential of %f is %f\n\n", x, y);
    errno = 0;
    x = 1.0E3F;
    y = expf(x);
    if (errno)
        perror("Error");
    printf("The exponential of %f is %f\n\n", x, y);
    errno = 0;
    x = -1.0E3F;
    y = expf(x);
    if (errno)
        perror("Error");
    printf("The exponential of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The exponential of 1.000000 is 2.718282<br>
Error: range error<br>
The exponential of 1000.000000 is inf<br>
Error: range error<br>
The exponential of -1000.000000 is 0.000000
 */
float expf(float x);

/**
Description: Calculates the absolute value of a double precision floating-point value.

<b>Include:</b> <math.h>

@param x floating-point value for which to return the absolute value

@return Returns the absolute value of x. (A negative number is returned as
positive, a positive number is unchanged.)

<b>Remarks:</b><br> No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for fabs
#include <stdio.h> // for printf
int main(void)
{
    double x, y;
    x = 1.75;
    y = fabs(x);
    printf("The absolute value of %f is %f\n", x, y);
    x = -1.5;
    y = fabs(x);
    printf("The absolute value of %f is %f\n", x, y);
}
@endcode
<b>Output:</b><br>
The absolute value of 1.750000 is 1.750000<br>
The absolute value of -1.500000 is 1.500000
 */
double fabs(double x);

/**
Description: Calculates the absolute value of a single precision floating-point value.

<b>Include:</b> <math.h>

@param x floating-point value for which to return the absolute value

@return Returns the absolute value of x. (A negative number is returned as
positive, a positive number is unchanged.)

<b>Remarks:</b><br> No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for fabsf
#include <stdio.h> // for printf
int main(void)
{
    float x, y;
    x = 1.75F;
    y = fabsf(x);
    printf("The absolute value of %f is %f\n", x, y);
    x = -1.5F;
    y = fabsf(x);
    printf("The absolute value of %f is %f\n", x, y);
}
@endcode
<b>Output:</b><br>
The absolute value of 1.750000 is 1.750000<br>
The absolute value of -1.500000 is 1.500000
 */
float fabsf(float x);

/**
Description: Calculates the floor of a double precision floating-point value.

<b>Include:</b> <math.h>

@param x floating-point value for which to return the floor

@return Returns the largest integer value less than or equal to x.

<b>Remarks:</b><br> No domain or range error will occur. See ceil.

<b>Example:</b><br>
@code
#include <math.h> // for floor
#include <stdio.h> // for printf
int main(void)
{
    double x[8] = {2.0, 1.75, 1.5, 1.25, -2.0,
        -1.75, -1.5, -1.25};
    double y;
    int i;
    for (i = 0; i < 8; i++) {
        y = floor(x[i]);
        printf("The ceiling for %f is %f\n", x[i], y);
    }
}
@endcode
<b>Output:</b><br>
The floor for 2.000000 is 2.000000<br>
The floor for 1.750000 is 1.000000<br>
The floor for 1.500000 is 1.000000<br>
The floor for 1.250000 is 1.000000<br>
The floor for -2.000000 is -2.000000<br>
The floor for -1.750000 is -2.000000<br>
The floor for -1.500000 is -2.000000<br>
The floor for -1.250000 is -2.000000
 */
double floor(double x);

/**
Description: Calculates the floor of a single precision floating-point value.

<b>Include:</b> <math.h>

@param x floating-point value

@return Returns the largest integer value less than or equal to x.

<b>Remarks:</b><br> No domain or range error will occur. See ceilf.
Standard C Libraries - Math Functions
? 2010 Microchip Technology Inc. DS51456G-page 167

<b>Example:</b><br>
@code
#include <math.h> // for floorf
#include <stdio.h> // for printf
int main(void)
{
    float x[8] = {2.0F, 1.75F, 1.5F, 1.25F,
        -2.0F, -1.75F, -1.5F, -1.25F};
    float y;
    int i;
    for (i = 0; i < 8; i++) {
        y = floorf(x[i]);
        printf("The floor for %f is %f\n", x[i], y);
    }
}
@endcode
<b>Output:</b><br>
The floor for 2.000000 is 2.000000<br>
The floor for 1.750000 is 1.000000<br>
The floor for 1.500000 is 1.000000<br>
The floor for 1.250000 is 1.000000<br>
The floor for -2.000000 is -2.000000<br>
The floor for -1.750000 is -2.000000<br>
The floor for -1.500000 is -2.000000<br>
The floor for -1.250000 is -2.000000
 */
float floorf(float x);

/**
Description: Calculates the remainder of x/y as a double precision value.

<b>Include:</b> <math.h>

@param x a double precision floating-point value

@param y a double precision floating-point value

@return Returns the remainder of x divided by y.

<b>Remarks:</b><br> If y = 0, a domain error occurs. If y is non-zero, the result will have the
same sign as x and the magnitude of the result will be less than the
magnitude of y.

<b>Example:</b><br>
@code
#include <math.h> // for fmod
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y, z;
    errno = 0;
    x = 7.0;
    y = 3.0;
    z = fmod(x, y);
    if (errno)
        perror("Error");
    printf("For fmod(%f, %f) the remainder is %f\n\n",
            x, y, z);
    errno = 0;
    x = 7.0;
    y = 7.0;
    z = fmod(x, y);
    if (errno)
        perror("Error");
    printf("For fmod(%f, %f) the remainder is %f\n\n",
            x, y, z);
    errno = 0;
    x = -5.0;
    y = 3.0;
    z = fmod(x, y);
    if (errno)
        perror("Error");
    printf("For fmod(%f, %f) the remainder is %f\n\n",
            x, y, z);
    errno = 0;
    x = 5.0;
    y = -3.0;
    z = fmod(x, y);
    if (errno)
        perror("Error");
    printf("For fmod(%f, %f) the remainder is %f\n\n",
            x, y, z);
    errno = 0;
    x = -5.0;
    y = -5.0;
    z = fmod(x, y);
    if (errno)
        perror("Error");
    printf("For fmod(%f, %f) the remainder is %f\n\n",
            x, y, z);
    errno = 0;
    x = 7.0;
    y = 0.0;
    z = fmod(x, y);
    if (errno)
        perror("Error");
    printf("For fmod(%f, %f) the remainder is %f\n\n",
            x, y, z);
}
@endcode
<b>Output:</b><br>
For fmod(7.000000, 3.000000) the remainder is
1.000000<br>
For fmod(7.000000, 7.000000) the remainder is
0.000000<br>
For fmod(-5.000000, 3.000000) the remainder is
-2.000000<br>
For fmod(5.000000, -3.000000) the remainder is
2.000000<br>
For fmod(-5.000000, -5.000000) the remainder is
-0.000000<br>
Error: domain error<br>
For fmod(7.000000, 0.000000) the remainder is nan
 */
double fmod(double x, double y);

/**
Description: Calculates the remainder of x/y as a single precision value.

<b>Include:</b> <math.h>

@param x a single precision floating-point value

@param y a single precision floating-point value

@return Returns the remainder of x divided by y.

<b>Remarks:</b><br> If y = 0, a domain error occurs. If y is non-zero, the result will have the
same sign as x and the magnitude of the result will be less than the
magnitude of y.

<b>Example:</b><br>
@code
#include <math.h> // for fmodf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y, z;
    errno = 0;
    x = 7.0F;
    y = 3.0F;
    z = fmodf(x, y);
    if (errno)
        perror("Error");
    printf("For fmodf (%f, %f) the remainder is"
            " %f\n\n", x, y, z);
    errno = 0;
    x = -5.0F;
    y = 3.0F;
    z = fmodf(x, y);
    if (errno)
        perror("Error");
    printf("For fmodf (%f, %f) the remainder is"
            " %f\n\n", x, y, z);
    errno = 0;
    x = 5.0F;
    y = -3.0F;
    z = fmodf(x, y);
    if (errno)
        perror("Error");
    printf("For fmodf (%f, %f) the remainder is"
            " %f\n\n", x, y, z);
    errno = 0;
    x = 5.0F;
    y = -5.0F;
    z = fmodf(x, y);
    if (errno)
        perror("Error");
    printf("For fmodf (%f, %f) the remainder is"
            " %f\n\n", x, y, z);
    errno = 0;
    x = 7.0F;
    y = 0.0F;
    z = fmodf(x, y);
    if (errno)
        perror("Error");
    printf("For fmodf (%f, %f) the remainder is"
            " %f\n\n", x, y, z);
    errno = 0;
    x = 7.0F;
    y = 7.0F;
    z = fmodf(x, y);
    if (errno)
        perror("Error");
    printf("For fmodf (%f, %f) the remainder is"
            " %f\n\n", x, y, z);
}
@endcode
<b>Output:</b><br>
For fmodf (7.000000, 3.000000) the remainder is
1.000000<br>
For fmodf (-5.000000, 3.000000) the remainder is
-2.000000<br>
For fmodf (5.000000, -3.000000) the remainder is
2.000000<br>
For fmodf (5.000000, -5.000000) the remainder is
0.000000<br>
Error: domain error<br>
For fmodf (7.000000, 0.000000) the remainder is nan<br>
For fmodf (7.000000, 7.000000) the remainder is
0.000000
 */
float fmodf(float x, float y);

/**
Description: Gets the fraction and the exponent of a double precision floating-point
number.

<b>Include:</b> <math.h>

@param x floating-point value for which to return the fraction and exponent

@param exp pointer to a stored integer exponent

@return Returns the fraction, exp points to the exponent. If x is 0, the function
returns 0 for both the fraction and exponent.

<b>Remarks:</b><br> The absolute value of the fraction is in the range of 1/2 (inclusive) to 1
(exclusive). No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for frexp
#include <stdio.h> // for printf
int main(void)
{
    double x, y;
    int n;
    x = 50.0;
    y = frexp(x, &n);
    printf("For frexp of %f\n the fraction is %f\n ",
            x, y);
    printf(" and the exponent is %d\n\n", n);
    x = -2.5;
    y = frexp(x, &n);
    printf("For frexp of %f\n the fraction is %f\n ",
            x, y);
    printf(" and the exponent is %d\n\n", n);
    x = 0.0;
    y = frexp(x, &n);
    printf("For frexp of %f\n the fraction is %f\n ",
            x, y);
    printf(" and the exponent is %d\n\n", n);
}
@endcode
<b>Output:</b><br>
For frexp of 50.000000<br>
the fraction is 0.781250<br>
and the exponent is 6<br>
For frexp of -2.500000<br>
the fraction is -0.625000<br>
and the exponent is 2<br>
For frexp of 0.000000<br>
the fraction is 0.000000<br>
and the exponent is 0
 */
double frexp(double x, int *exp);

/**
Description: Gets the fraction and the exponent of a single precision floating-point
number.

<b>Include:</b> <math.h>

@param x floating-point value for which to return the fraction and exponent

@param exp pointer to a stored integer exponent

@return Returns the fraction, exp points to the exponent. If x is 0, the function
returns 0 for both the fraction and exponent.

<b>Remarks:</b><br> The absolute value of the fraction is in the range of 1/2 (inclusive) to 1
(exclusive). No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for frexpf
#include <stdio.h> // for printf
int main(void)
{
    float x, y;
    int n;
    x = 0.15F;
    y = frexpf(x, &n);
    printf("For frexpf of %f\n the fraction is %f\n ",
            x, y);
    printf(" and the exponent is %d\n\n", n);
    x = -2.5F;
    y = frexpf(x, &n);
    printf("For frexpf of %f\n the fraction is %f\n ",
            x, y);
    printf(" and the exponent is %d\n\n", n);
    x = 0.0F;
    y = frexpf(x, &n);
    printf("For frexpf of %f\n the fraction is %f\n ",
            x, y);
    printf(" and the exponent is %d\n\n", n);
}
@endcode
<b>Output:</b><br>
For frexpf of 0.150000<br>
the fraction is 0.600000<br>
and the exponent is -2<br>
For frexpf of -2.500000<br>
the fraction is -0.625000<br>
and the exponent is 2<br>
For frexpf of 0.000000<br>
the fraction is 0.000000<br>
and the exponent is 0
 */
float frexpf(float x, int *exp);

/**
Description: Calculates the result of a double precision floating-point number
multiplied by an exponent of 2.

<b>Include:</b> <math.h>

@param x floating-point value

@param ex integer exponent

@return Returns x * 2^ex. On an overflow, ldexp returns inf and on an
underflow, ldexp returns 0.

<b>Remarks:</b><br> A range error will occur on overflow or underflow.

<b>Example:</b><br>
@code
#include <math.h> // for ldexp
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    int n;
    errno = 0;
    x = -0.625;
    n = 2;
    y = ldexp(x, n);
    if (errno)
        perror("Error");
    printf("For a number = %f and an exponent = %d\n",
            x, n);
    printf(" ldexp(%f, %d) = %f\n\n",
            x, n, y);
    errno = 0;
    x = 2.5;
    n = 3;
    y = ldexp(x, n);
    if (errno)
        perror("Error");
    printf("For a number = %f and an exponent = %d\n",
            x, n);
    printf(" ldexp(%f, %d) = %f\n\n",
            x, n, y);
    errno = 0;
    x = 15.0;
    n = 10000;
    y = ldexp(x, n);
    if (errno)
        perror("Error");
    printf("For a number = %f and an exponent = %d\n",
            x, n);
    printf(" ldexp(%f, %d) = %f\n\n",
            x, n, y);
}
@endcode
<b>Output:</b><br>
For a number = -0.625000 and an exponent = 2<br>
ldexp(-0.625000, 2) = -2.500000<br>
For a number = 2.500000 and an exponent = 3<br>
ldexp(2.500000, 3) = 20.000000<br>
Error: range error<br>
For a number = 15.000000 and an exponent = 10000<br>
ldexp(15.000000, 10000) = inf
 */
double ldexp(double x, int ex);

/**
Description: Calculates the result of a single precision floating-point number
multiplied by an exponent of 2.

<b>Include:</b> <math.h>

@param x floating-point value

@param ex integer exponent

@return Returns x * 2^ex. On an overflow, ldexp returns inf and on an
underflow, ldexp returns 0.

<b>Remarks:</b><br> A range error will occur on overflow or underflow.

<b>Example:</b><br>
@code
#include <math.h> // for ldexpf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    int n;
    errno = 0;
    x = -0.625F;
    n = 2;
    y = ldexpf(x, n);
    if (errno)
        perror("Error");
    printf("For a number = %f and an exponent = %d\n",
            x, n);
    printf(" ldexpf(%f, %d) = %f\n\n",
            x, n, y);
    errno = 0;
    x = 2.5F;
    n = 3;
    y = ldexpf(x, n);
    if (errno)
        perror("Error");
    printf("For a number = %f and an exponent = %d\n",
            x, n);
    printf(" ldexpf(%f, %d) = %f\n\n",
            x, n, y);
    errno = 0;
    x = 15.0F;
    n = 10000;
    y = ldexpf(x, n);
    if (errno)
        perror("Error");
    printf("For a number = %f and an exponent = %d\n",
            x, n);
    printf(" ldexpf(%f, %d) = %f\n\n",
            x, n, y);
}
@endcode
<b>Output:</b><br>
For a number = -0.625000 and an exponent = 2<br>
ldexpf(-0.625000, 2) = -2.500000<br>
For a number = 2.500000 and an exponent = 3<br>
ldexpf(2.500000, 3) = 20.000000<br>
Error: range error<br>
For a number = 15.000000 and an exponent = 10000<br>
ldexpf(15.000000, 10000) = inf
 */
float ldexpf(float x, int ex);

/**
Description: Calculates the natural logarithm of a double precision floating-point
value.

<b>Include:</b> <math.h>

@param x any positive value for which to return the log

@return Returns the natural logarithm of x. -inf is returned if x is 0 and NaN is
returned if x is a negative number.

<b>Remarks:</b><br> A domain error occurs if x ? 0.

<b>Example:</b><br>
@code
#include <math.h> // for log
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = 2.0;
    y = log(x);
    if (errno)
        perror("Error");
    printf("The natural logarithm of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 0.0;
    y = log(x);
    if (errno)
        perror("Error");
    printf("The natural logarithm of %f is %f\n\n",
            x, y);
    errno = 0;
    x = -2.0;
    y = log(x);
    if (errno)
        perror("Error");
    printf("The natural logarithm of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The natural logarithm of 2.000000 is 0.693147<br>
The natural logarithm of 0.000000 is -inf<br>
Error: domain error<br>
The natural logarithm of -2.000000 is nan
*/
double log(double x);

/**
Description: Calculates the base-10 logarithm of a double precision floating-point
value.

<b>Include:</b> <math.h>

@param x any double precision floating-point positive number

@return Returns the base-10 logarithm of x. -inf is returned if x is 0 and NaN
is returned if x is a negative number.

<b>Remarks:</b><br> A domain error occurs if x ? 0.

<b>Example:</b><br>
@code
#include <math.h> // for log10
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = 2.0;
    y = log10(x);
    if (errno)
        perror("Error");
    printf("The base-10 logarithm of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 0.0;
    y = log10(x);
    if (errno)
        perror("Error");
    printf("The base-10 logarithm of %f is %f\n\n",
            x, y);
    errno = 0;
    x = -2.0;
    y = log10(x);
    if (errno)
        perror("Error");
    printf("The base-10 logarithm of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The base-10 logarithm of 2.000000 is 0.301030<br>
The base-10 logarithm of 0.000000 is -inf<br>
Error: domain error<br>
The base-10 logarithm of -2.000000 is nan
 */
double log10(double x);

/**
Description: Calculates the base-10 logarithm of a single precision floating-point
value.

<b>Include:</b> <math.h>

@param x any single precision floating-point positive number

@return Returns the base-10 logarithm of x. -inf is returned if x is 0 and NaN
is returned if x is a negative number.

<b>Remarks:</b><br> A domain error occurs if x ? 0.

<b>Example:</b><br>
@code
#include <math.h> // for log10f
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    errno = 0;
    x = 2.0F;
    y = log10f(x);
    if (errno)
        perror("Error");
    printf("The base-10 logarithm of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 0.0F;
    y = log10f(x);
    if (errno)
        perror("Error");
    printf("The base-10 logarithm of %f is %f\n\n",
            x, y);
    errno = 0;
    x = -2.0F;
    y = log10f(x);
    if (errno)
        perror("Error");
    printf("The base-10 logarithm of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The base-10 logarithm of 2.000000 is 0.301030<br>
Error: domain error<br>
The base-10 logarithm of 0.000000 is -inf<br>
Error: domain error<br>
The base-10 logarithm of -2.000000 is nan
 */
float log10f(float x);

/**
Description: Calculates the natural logarithm of a single precision floating-point
value.

<b>Include:</b> <math.h>

@param x any positive value for which to return the log

@return Returns the natural logarithm of x. -inf is returned if x is 0 and NaN is
returned if x is a negative number.

<b>Remarks:</b><br> A domain error occurs if x ? 0.

<b>Example:</b><br>
@code
#include <math.h> // for logf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    errno = 0;
    x = 2.0F;
    y = logf(x);
    if (errno)
        perror("Error");
    printf("The natural logarithm of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 0.0F;
    y = logf(x);
    if (errno)
        perror("Error");
    printf("The natural logarithm of %f is %f\n\n",
            x, y);
    errno = 0;
    x = -2.0F;
    y = logf(x);
    if (errno)
        perror("Error");
    printf("The natural logarithm of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The natural logarithm of 2.000000 is 0.693147<br>
The natural logarithm of 0.000000 is -inf<br>
Error: domain error<br>
The natural logarithm of -2.000000 is nan
 */
float logf(float x);

/**
Description: Splits a double precision floating-point value into fractional and integer
parts.

<b>Include:</b> <math.h>

@param x double precision floating-point value

@param pint pointer to a stored the integer part

@return Returns the signed fractional part and pint points to the integer part.

<b>Remarks:</b><br> The absolute value of the fractional part is in the range of 0 (inclusive)
to 1 (exclusive). No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for modf
#include <stdio.h> // for printf
int main(void)
{
    double x, y, n;
    x = 0.707;
    y = modf(x, &n);
    printf("For %f the fraction is %f\n ", x, y);
    printf(" and the integer is %0.f\n\n", n);
    x = -15.2121;
    y = modf(x, &n);
    printf("For %f the fraction is %f\n ", x, y);
    printf(" and the integer is %0.f\n\n", n);
}
@endcode
<b>Output:</b><br>
For 0.707000 the fraction is 0.707000<br>
and the integer is 0<br>
For -15.212100 the fraction is -0.212100<br>
and the integer is -15
 */
double modf(double x, double *pint);

/**
Description: Splits a single precision floating-point value into fractional and integer
parts.

<b>Include:</b> <math.h>

@param x single precision floating-point value

@param pint pointer to stored integer part

@return Returns the signed fractional part and pint points to the integer part.

<b>Remarks:</b><br> The absolute value of the fractional part is in the range of 0 (inclusive)
to 1 (exclusive). No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for modff
#include <stdio.h> // for printf
int main(void)
{
    float x, y, n;
    x = 0.707F;
    y = modff(x, &n);
    printf("For %f the fraction is %f\n ", x, y);
    printf(" and the integer is %0.f\n\n", n);
    x = -15.2121F;
    y = modff(x, &n);
    printf("For %f the fraction is %f\n ", x, y);
    printf(" and the integer is %0.f\n\n", n);
}
@endcode
<b>Output:</b><br>
For 0.707000 the fraction is 0.707000<br>
and the integer is 0<br>
For -15.212100 the fraction is -0.212100<br>
and the integer is -15
 */
float modff(float x, float *pint);

/**
Description: Calculates x raised to the power y.

<b>Include:</b> <math.h>

@param x the base

@param y the exponent

@return Returns x raised to the power y (x^y).

<b>Remarks:</b><br> If y is 0, pow returns 1. If x is 0.0 and y is less than 0, pow returns inf
and a domain error occurs. If the result overflows or underflows, a
range error occurs.

<b>Example:</b><br>
@code
#include <math.h> // for pow
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y, z;
    errno = 0;
    x = -2.0;
    y = 3.0;
    z = pow(x, y);
    if (errno)
        perror("Error");
    printf("%f raised to %f is %f\n\n ", x, y, z);
    errno = 0;
    x = 3.0;
    y = -0.5;
    z = pow(x, y);
    if (errno)
        perror("Error");
    printf("%f raised to %f is %f\n\n ", x, y, z);
    errno = 0;
    x = 4.0;
    y = 0.0;
    z = pow(x, y);
    if (errno)
        perror("Error");
    printf("%f raised to %f is %f\n\n ", x, y, z);
    errno = 0;
    x = 0.0;
    y = -3.0;
    z = pow(x, y);
    if (errno)
        perror("Error");
    printf("%f raised to %f is %f\n\n ", x, y, z);
}
@endcode
<b>Output:</b><br>
-2.000000 raised to 3.000000 is -8.000000<br>
3.000000 raised to -0.500000 is 0.577350<br>
4.000000 raised to 0.000000 is 1.000000<br>
Error: domain error<br>
0.000000 raised to -3.000000 is inf
 */
double pow(double x, double y);

/**
Description: Calculates x raised to the power y.

<b>Include:</b> <math.h>

@param x base

@param y exponent

@return Returns x raised to the power y (x^y).

<b>Remarks:</b><br> If y is 0, powf returns 1. If x is 0.0 and y is less than 0, powf returns
inf and a domain error occurs. If the result overflows or underflows, a
range error occurs.

<b>Example:</b><br>
@code
#include <math.h> // for powf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y, z;
    errno = 0;
    x = -2.0F;
    y = 3.0F;
    z = powf(x, y);
    if (errno)
        perror("Error");
    printf("%f raised to %f is %f\n\n ", x, y, z);
    errno = 0;
    x = 3.0F;
    y = -0.5F;
    z = powf(x, y);
    if (errno)
        perror("Error");
    printf("%f raised to %f is %f\n\n ", x, y, z);
    errno = 0;
    x = 0.0F;
    y = -3.0F;
    z = powf(x, y);
    if (errno)
        perror("Error");
    printf("%f raised to %f is %f\n\n ", x, y, z);
}
@endcode
<b>Output:</b><br>
-2.000000 raised to 3.000000 is -8.000000<br>
3.000000 raised to -0.500000 is 0.577350<br>
Error: domain error<br>
0.000000 raised to -3.000000 is inf
 */
float powf(float x, float y);

/**
Description: Calculates the trigonometric sine function of a double precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the sine

@return Returns the sine of x in radians in the ranges of -1 to 1 inclusive.

<b>Remarks:</b><br> A domain error will occur if t x is a NaN or infinity.

<b>Example:</b><br>
@code
#include <math.h> // for sin
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = -1.0;
    y = sin(x);
    if (errno)
        perror("Error");
    printf("The sine of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.0;
    y = sin(x);
    if (errno)
        perror("Error");
    printf("The sine of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The sine of -1.000000 is -0.841471<br>
The sine of 0.000000 is 0.000000
 */
double sin(double x);

/**
Description: Calculates the trigonometric sine function of a single precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the sine

@return Returns the sin of x in radians in the ranges of -1 to 1 inclusive.

<b>Remarks:</b><br> A domain error will occur if x is a NaN or infinity.

<b>Example:</b><br>
@code
#include <math.h> // for sinf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    errno = 0;
    x = -1.0F;
    y = sinf(x);
    if (errno)
        perror("Error");
    printf("The sine of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.0F;
    y = sinf(x);
    if (errno)
        perror("Error");
    printf("The sine of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The sine of -1.000000 is -0.841471<br>
The sine of 0.000000 is 0.000000
 */
float sinf(float x);

/**
Description: Calculates the hyperbolic sine function of a double precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the hyperbolic sine

@return Returns the hyperbolic sine of x

<b>Remarks:</b><br> A range error will occur if the magnitude of x is too large.

<b>Example:</b><br>
@code
#include <math.h> // for sinh
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = -1.5;
    y = sinh(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic sine of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 0.0;
    y = sinh(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic sine of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 720.0;
    y = sinh(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic sine of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The hyperbolic sine of -1.500000 is -2.129279<br>
The hyperbolic sine of 0.000000 is 0.000000<br>
Error: range error<br>
The hyperbolic sine of 720.000000 is inf
 */
double sinh(double x);

/**
Description: Calculates the hyperbolic sine function of a single precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the hyperbolic sine

@return Returns the hyperbolic sine of x .

<b>Remarks:</b><br> A range error will occur if the magnitude of x is too large.

<b>Example:</b><br>
@code
#include <math.h> // for sinhf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    errno = 0;
    x = -1.0F;
    y = sinhf(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic sine of %f is %f\n\n",
            x, y);
    errno = 0;
    x = 0.0F;
    y = sinhf(x);
    if (errno)
        perror("Error");
    printf("The hyperbolic sine of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The hyperbolic sine of -1.000000 is -1.175201<br>
The hyperbolic sine of 0.000000 is 0.000000
 */
float sinhf(float x);

/**
Description: Calculates the square root of a double precision floating-point value.

<b>Include:</b> <math.h>

@param x a non-negative floating-point value

@return Returns the non-negative square root of x.

<b>Remarks:</b><br> If x is negative, a domain error occurs.

<b>Example:</b><br>
@code
#include <math.h> // for sqrt
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = 0.0;
    y = sqrt(x);
    if (errno)
        perror("Error");
    printf("The square root of %f is %f\n\n", x, y);
    errno = 0;
    x = 9.5;
    y = sqrt(x);
    if (errno)
        perror("Error");
    printf("The square root of %f is %f\n\n", x, y);
    errno = 0;
    x = -25.0;
    y = sqrt(x);
    if (errno)
        perror("Error");
    printf("The square root of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The square root of 0.000000 is 0.000000<br>
The square root of 9.500000 is 3.082207<br>
Error: domain error<br>
The square root of -25.000000 is nan
 */
double sqrt(double x);

/**
Description: Calculates the square root of a single precision floating-point value.

<b>Include:</b> <math.h>

@param x non-negative floating-point value

@return Returns the non-negative square root of x.

<b>Remarks:</b><br> If x is negative, a domain error occurs.

<b>Example:</b><br>
@code
#include <math.h> // for sqrtf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x;
    errno = 0;
    x = sqrtf(0.0F);
    if (errno)
        perror("Error");
    printf("The square root of 0.0F is %f\n\n", x);
    errno = 0;
    x = sqrtf(9.5F);
    if (errno)
        perror("Error");
    printf("The square root of 9.5F is %f\n\n", x);
    errno = 0;
    x = sqrtf(-25.0F);
    if (errno)
        perror("Error");
    printf("The square root of -25F is %f\n", x);
}
@endcode
<b>Output:</b><br>
The square root of 0.0F is 0.000000<br>
The square root of 9.5F is 3.082207<br>
Error: domain error<br>
The square root of -25F is nan
 */
float sqrtf(float x);

/**
Description: Calculates the trigonometric tangent function of a double precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the tangent

@return Returns the tangent of x in radians.

<b>Remarks:</b><br> A domain error will occur if x is a NaN or infinity.

<b>Example:</b><br>
@code
#include <math.h> // for tan
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    double x, y;
    errno = 0;
    x = -1.0;
    y = tan(x);
    if (errno)
        perror("Error");
    printf("The tangent of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.0;
    y = tan(x);
    if (errno)
        perror("Error");
    printf("The tangent of %f is %f\n\n", x, y);
}
@endcode
<b>Output:</b><br>
The tangent of -1.000000 is -1.557408<br>
The tangent of 0.000000 is 0.000000
 */
double tan(double x);

/**
Description: Calculates the trigonometric tangent function of a single precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the tangent

@return Returns the tangent of x.

<b>Remarks:</b><br> A domain error will occur if x is a NaN or infinity.

<b>Example:</b><br>
@code
#include <math.h> // for tanf
#include <stdio.h> // for printf, perror
#include <errno.h> // for errno
int main(void)
{
    float x, y;
    Standard C Libraries - Math Functions
            ? 2010 Microchip Technology Inc. DS51456G - page 191
            errno = 0;
    x = -1.0F;
    y = tanf(x);
    if (errno)
        perror("Error");
    printf("The tangent of %f is %f\n\n", x, y);
    errno = 0;
    x = 0.0F;
    y = tanf(x);
    if (errno)
        perror("Error");
    printf("The tangent of %f is %f\n", x, y);
}
@endcode
<b>Output:</b><br>
The tangent of -1.000000 is -1.557408<br>
The tangent of 0.000000 is 0.000000
 */
float tanf(float x);

/**
Description: Calculates the hyperbolic tangent function of a double precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the hyperbolic tangent

@return Returns the hyperbolic tangent of x in the ranges of -1 to 1 inclusive.

<b>Remarks:</b><br> No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for tanh
#include <stdio.h> // for printf
int main(void)
{
    double x, y;
    x = -1.0;
    y = tanh(x);
    printf("The hyperbolic tangent of %f is %f\n\n",
            x, y);
    x = 2.0;
    y = tanh(x);
    printf("The hyperbolic tangent of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The hyperbolic tangent of -1.000000 is -0.761594<br>
The hyperbolic tangent of 2.000000 is 0.964028
 */
double tanh(double x);

/**
Description: Calculates the hyperbolic tangent function of a single precision
floating-point value.

<b>Include:</b> <math.h>

@param x value for which to return the hyperbolic tangent

@return Returns the hyperbolic tangent of x in the ranges of -1 to 1 inclusive.

<b>Remarks:</b><br> No domain or range error will occur.

<b>Example:</b><br>
@code
#include <math.h> // for tanhf
#include <stdio.h> // for printf
int main(void)
{
    float x, y;
    x = -1.0F;
    y = tanhf(x);
    printf("The hyperbolic tangent of %f is %f\n\n",
            x, y);
    x = 0.0F;
    y = tanhf(x);
    printf("The hyperbolic tangent of %f is %f\n\n",
            x, y);
}
@endcode
<b>Output:</b><br>
The hyperbolic tangent of -1.000000 is -0.761594<br>
The hyperbolic tangent of 0.000000 is 0.000000
 */
float tanhf(float x);