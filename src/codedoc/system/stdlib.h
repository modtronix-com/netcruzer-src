/**
 * @brief           Standard C Libraries - stdlib Utility Functions
 * @file            stdlib.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section stdlib_desc Description
 **********************************************************************
 The header file, stdlib.h, consists of types, macros and functions that provide text
conversions, memory management, searching and sorting abilities, and other general
utilities. 
 **********************************************************************
 * @section stdlib_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original lincense
 * agreement included with the XC16 compiler applies!
 */

/**
Description: A type that holds a quotient and remainder of a signed integer division
with operands of type int.

<b>Include:</b> <stdlib.h>

<b>Remarks:</b><br> This is the structure type returned by the function, div.
 */
typedef struct {
    int quot, rem;
} div_t;

/**
Description: A type that holds a quotient and remainder of a signed integer division
with operands of type long.

<b>Include:</b> <stdlib.h>

<b>Remarks:</b><br> This is the structure type returned by the function, ldiv.
 */
typedef struct {
    long quot, rem;
} ldiv_t;



/**
Description: The type of the result of the sizeof operator.

<b>Include:</b> <stdlib.h>
 */
#define size_t size_t



/**
Description: A type that holds a wide character value.

<b>Include:</b> <stdlib.h>
*/
#define wchar_t



/**
Description: Reports unsuccessful termination.

<b>Include:</b> <stdlib.h>

<b>Remarks:</b><br> EXIT_FAILURE is a value for the exit function to return an unsuccessful
termination status.

<b>Example:</b><br>
See exit for example of use.
 */
#define EXIT_FAILURE



/**
Description: Reports successful termination.

<b>Include:</b> <stdlib.h>

<b>Remarks:</b><br> EXIT_SUCCESS is a value for the exit function to return a successful
termination status.

<b>Example:</b><br>
See exit for example of use.
 */
#define EXIT_SUCCESS



/**
Description: Maximum number of characters in a multibyte character.

<b>Include:</b> <stdlib.h>

<b>Value:</b>  1
 */
#define MB_CUR_MAX



/**
Description: The value of a null pointer constant.

<b>Include:</b> <stdlib.h>
 */
#define	NULL	(0)



/**
Description: Maximum value capable of being returned by the rand function.

<b>Include:</b> <stdlib.h>

<b>Value:</b>  32767
 */
#define RAND_MAX



/**
Description: Aborts the current process.

<b>Include:</b> <stdlib.h>

<b>Remarks:</b><br> abort will cause the processor to reset.

<b>Example:</b><br>
@code
#include <stdio.h> // for fopen, fclose, 
// printf, FILE, NULL 
#include <stdlib.h> // for abort 

int main(void) {
    FILE *myfile;
    if ((myfile = fopen("samp.fil", "r")) == NULL) {
        printf("Cannot open samp.fil\n");
        abort();
    } else
        printf("Success opening samp.fil\n");
    fclose(myfile);
}
@endcode
<b>Output:</b><br>
Cannot open samp.fil <br>
ABRT
 */
void abort(void);



/**
Description: Calculates the absolute value.

<b>Include:</b> <stdlib.h>

@param i integer value

@return Returns the absolute value of i.

<b>Remarks:</b><br> A negative number is returned as positive; a positive number is
unchanged.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdlib.h> // for abs 

int main(void) {
    int i;
    i = 12;
    printf("The absolute value of %d is %d\n",
            i, abs(i));
    i = -2;
    printf("The absolute value of %d is %d\n",
            i, abs(i));
    i = 0;
    printf("The absolute value of %d is %d\n",
            i, abs(i));
}
@endcode
<b>Output:</b><br>
The absolute value of 12 is 12 <br>
The absolute value of -2 is 2 <br>
The absolute value of 0 is 0
 */
int abs(int i);



/**
Description: Registers the specified function to be called when the program
terminates normally.

<b>Include:</b> <stdlib.h>

@param func function to be called

@return Returns a zero if successful; otherwise, returns a non-zero value.

<b>Remarks:</b><br> For the registered functions to be called, the program must terminate
with the exit function call.

<b>Example:</b><br>
@code
#include <stdio.h> // for scanf, printf 
#include <stdlib.h> // for atexit, exit 
void good_msg(void);
void bad_msg(void);
void end_msg(void);

int main(void) {
    int number;
    atexit(end_msg);
    printf("Enter your favorite number:");
    scanf("%d", &number);
    printf(" %d\n", number);
    if (number == 5) {
        printf("Good Choice\n");
        atexit(good_msg);
        exit(0);
    } else {
        printf("%d!?\n", number);
        atexit(bad_msg);
        exit(0);
    }
}

void good_msg(void) {
    printf("That's an excellent number\n");
}

void bad_msg(void) {
    printf("That's an awful number\n");
}

void end_msg(void) {
    printf("Now go count something\n");
}
@endcode
<b>Input:</b><br>
With contents of UartIn.txt (used as stdin input for simulator):
5
<b>Output:</b><br>
Enter your favorite number: 5 <br>
Good Choice <br>
That's an excellent number <br>
Now go count something <br>

<b>Input:</b><br>
With contents of UartIn.txt (used as stdin input for simulator): <br>
42 <br>

<b>Output:</b><br>
Enter your favorite number: 42 <br>
42!? <br>
That's an awful number <br>
Now go count something
 */
int atexit(void(*func)(void));



/**
Description: Converts a string to a double precision floating-point value.

<b>Include:</b> <stdlib.h>

@param s pointer to the string to be converted

@return Returns the converted value if successful; otherwise, returns 0.

<b>Remarks:</b><br> The number may consist of the following: <br>
[whitespace] [sign] digits [.digits] <br>
[ { e | E }[sign]digits] <br>
optional whitespace, followed by an optional sign then a sequence
of one or more digits with an optional decimal point, followed by one
or more optional digits and an optional e or E followed by an optional
signed exponent. The conversion stops when the first unrecognized
character is reached. The conversion is the same as strtod(s,0)
except it does no error checking so errno will not be set.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdlib.h> // for atof 

int main(void) {
    char a[] = " 1.28";
    char b[] = "27.835e2";
    char c[] = "Number1";
    double x;
    x = atof(a);
    printf("String = \"%s\" float = %f\n", a, x);
    x = atof(b);
    printf("String = \"%s\" float = %f\n", b, x);
    x = atof(c);
    printf("String = \"%s\" float = %f\n", c, x);
}
@endcode
<b>Output:</b><br>
String = "1.28" float = 1.280000 <br>
String = "27.835:e2" float = 2783.500000 <br>
String = "Number1" float = 0.000000
 */
double atof(const char *s);



/**
Description: Converts a string to an integer.

<b>Include:</b> <stdlib.h>

@param s string to be converted

@return Returns the converted integer if successful; otherwise, returns 0.

<b>Remarks:</b><br> The number may consist of the following: <br>
[whitespace] [sign] digits <br>
optional whitespace, followed by an optional sign then a sequence
of one or more digits. The conversion stops when the first unrecognized
character is reached. The conversion is equivalent to (int)
strtol(s,0,10) except it does no error checking so errno will not
be set.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdlib.h> // for atoi 

int main(void) {
    char a[] = " -127";
    char b[] = "Number1";
    int x;
    x = atoi(a);
    printf("String = \"%s\"\tint = %d\n", a, x);
    x = atoi(b);
    printf("String = \"%s\"\tint = %d\n", b, x);
}
@endcode
<b>Output:</b><br>
String = " -127" int = -127 <br>
String = "Number1" int = 0
 */
int atoi(const char *s);



/**
Description: Converts a string to a long integer.

<b>Include:</b> <stdlib.h>

@param s string to be converted

@return Returns the converted long integer if successful; otherwise, returns 0.

<b>Remarks:</b><br> The number may consist of the following: <br>
[whitespace] [sign] digits <br>
optional whitespace, followed by an optional sign then a sequence
of one or more digits. The conversion stops when the first unrecognized
character is reached. The conversion is equivalent to (int)
strtol(s,0,10) except it does no error checking so errno will not
be set.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdlib.h> // for atol 

int main(void) {
    char a[] = " -123456";
    char b[] = "2Number";
    long x;
    x = atol(a);
    printf("String = \"%s\" int = %ld\n", a, x);
    x = atol(b);
    printf("String = \"%s\" int = %ld\n", b, x);
}
@endcode
<b>Output:</b><br>
String = " -123456" int = -123456 <br>
String = "2Number" int = 2
 */
long atol(const char *s);



/**
Description: Performs a binary search.

<b>Include:</b> <stdlib.h>

@param key object to search for

@param base pointer to the start of the search data

@param nelem number of elements

@param size size of elements

@param cmp pointer to the comparison function

@param ck pointer to the key for the search

@param ce pointer to the element being compared with the key.

@return Returns a pointer to the object being searched for if found; otherwise,
returns NULL.

<b>Remarks:</b><br> The value returned by the compare function is <0 if ck is less than ce,
0 if ck is equal to ce or >0 if ck is greater than ce.
In the following example, qsort is used to sort the list before bsearch
is called. bsearch requires the list to be sorted according to the
comparison function. This comp uses ascending order.

<b>Example:</b><br>
@code
#include <stdlib.h> // for bsearch, qsort 
#include <stdio.h> // for printf, sizeof 
#define NUM 7
int comp(const void *e1, const void *e2);

int main(void) {
    int list[NUM] = {35, 47, 63, 25, 93, 16, 52};
    int x, y;
    int *r;
    qsort(list, NUM, sizeof (int), comp);
    printf("Sorted List: ");
    for (x = 0; x < NUM; x++)
        printf("%d ", list[x]);
    y = 25;
    r = bsearch(&y, list, NUM, sizeof (int), comp);
    if (r)
        printf("\nThe value %d was found\n", y);
    else
        printf("\nThe value %d was not found\n", y);
    y = 75;
    r = bsearch(&y, list, NUM, sizeof (int), comp);
    if (r)
        printf("\nThe value %d was found\n", y);
    else
        printf("\nThe value %d was not found\n", y);
}

int comp(const void *e1, const void *e2) {
    const int * a1 = e1;
    const int * a2 = e2;
    if (*a1 < *a2)
        return -1;
    else if (*a1 == *a2)
        return 0;
    else
        return 1;
}
@endcode
<b>Output:</b><br>
Sorted List: 16 25 35 47 52 63 93 <br>
The value 25 was found <br>
The value 75 was not found
 */
void *bsearch(const void *key, const void *base, size_t nelem, size_t size, int *cmp, const void *ck, const void *ce);



/**
Description: Allocates an array in memory and initializes the elements to 0.

<b>Include:</b> <stdlib.h>

@param nelem number of elements

@param size length of each element

@return Returns a pointer to the allocated space if successful; otherwise,
returns a null pointer.

<b>Remarks:</b><br> Memory returned by calloc is aligned correctly for any size data
element and is initialized to zero.

<b>Example:</b><br>
@code
// This program allocates memory for the 
// array 'i' of long integers and initializes 
// them to zero. 
#include <stdio.h> // for printf, NULL 
#include <stdlib.h> // for calloc, free 

int main(void) {
    int x;
    long *i;
    i = (long *) calloc(5, sizeof (long));
    if (i != NULL) {
        for (x = 0; x < 5; x++)
            printf("i[%d] = %ld\n", x, i[x]);
        free(i);
    } else
        printf("Cannot allocate memory\n");
}
@endcode
<b>Output:</b><br>
i[0] = 0 <br>
i[1] = 0 <br>
i[2] = 0 <br>
i[3] = 0 <br>
i[4] = 0
 */
void *calloc(size_t nelem, size_t size);



/**
Description: Calculates the quotient and remainder of two numbers.

<b>Include:</b> <stdlib.h>

@param numer numerator

@param denom denominator

@return Returns the quotient and the remainder.

<b>Remarks:</b><br> The returned quotient will have the same sign as the numerator divided
by the denominator. The sign for the remainder will be such that the
quotient times the denominator plus the remainder will equal the
numerator (quot * denom + rem = numer). Division by zero will invoke
the math exception error, which by default, will cause a Reset. Write a
math error handler to do something else.

<b>Example:</b><br>
@code
#include <stdlib.h> // for div, div_t 
#include <stdio.h> // for printf 

void __attribute__((__interrupt__))
_MathError(void) {
    printf("Illegal instruction executed\n");
    abort();
}

int main(void) {
    int x, y;
    div_t z;
    x = 7;
    y = 3;
    printf("For div(%d, %d)\n", x, y);
    z = div(x, y);
    printf("The quotient is %d and the "
            "remainder is %d\n\n", z.quot, z.rem);
    x = 7;
    y = -3;
    printf("For div(%d, %d)\n", x, y);
    z = div(x, y);
    printf("The quotient is %d and the "
            "remainder is %d\n\n", z.quot, z.rem);
    x = -5;
    y = 3;
    printf("For div(%d, %d)\n", x, y);
    z = div(x, y);
    printf("The quotient is %d and the "
            "remainder is %d\n\n", z.quot, z.rem);
    x = 7;
    y = 7;
    printf("For div(%d, %d)\n", x, y);
    z = div(x, y);
    printf("The quotient is %d and the "
            "remainder is %d\n\n", z.quot, z.rem);
    x = 7;
    y = 0;
    printf("For div(%d, %d)\n", x, y);
    z = div(x, y);
    printf("The quotient is %d and the "
            "remainder is %d\n\n", z.quot, z.rem);
}
@endcode
<b>Output:</b><br>
For div(7, 3) <br>
The quotient is 2 and the remainder is 1 <br>
For div(7, -3) <br>
The quotient is -2 and the remainder is 1 <br>
For div(-5, 3) <br>
The quotient is -1 and the remainder is -2 <br>
For div(7, 7) <br>
The quotient is 1 and the remainder is 0 <br>
For div(7, 0) <br>
Illegal instruction executed <br>
ABRT
 */
div_t div(int numer, int denom);



/**
Description: Terminates program after clean up.

<b>Include:</b> <stdlib.h>

@param status exit status

<b>Remarks:</b><br> exit calls any functions registered by atexit in reverse order of registration,
flushes buffers, closes stream, closes any temporary files
created with tmpfile, and resets the processor. This function is
customizable. See pic30-libs.

<b>Example:</b><br>
@code
#include <stdio.h> // for fopen, printf, 
// FILE, NULL 
#include <stdlib.h> // for exit 

int main(void) {
    FILE *myfile;
    if ((myfile = fopen("samp.fil", "r")) == NULL) {
        printf("Cannot open samp.fil\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Success opening samp.fil\n");
        exit(EXIT_SUCCESS);
    }
    printf("This will not be printed");
}
@endcode
<b>Output:</b><br>
Cannot open samp.fil
 */
void exit(int status);



/**
Description: Frees memory.

<b>Include:</b> <stdlib.h>

@param ptr points to memory to be freed

<b>Remarks:</b><br> Frees memory previously allocated with calloc, malloc, or
realloc. If free is used on space that has already been deallocated
(by a previous call to free or by realloc) or on space not allocated
with calloc, malloc, or realloc, the behavior is undefined.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf, sizeof, 
// NULL 
#include <stdlib.h> // for malloc, free 

int main(void) {
    long *i;
    if ((i = (long *) malloc(50 * sizeof (long))) ==
            NULL)
        printf("Cannot allocate memory\n");
    else {
        printf("Memory allocated\n");
        free(i);
        printf("Memory freed\n");
    }
}
@endcode
<b>Output:</b><br>
Memory allocated <br>
Memory freed
 */
void free(void *ptr);



/**
Description: Get a value for an environment variable.

<b>Include:</b> <stdlib.h>

@param name name of environment variable

@return Returns a pointer to the value of the environment variable if successful;
otherwise, returns a null pointer.

<b>Remarks:</b><br> This function must be customized to be used as described (see
pic30-libs). By default there are no entries in the environment list
for getenv to find.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf, NULL 
#include <stdlib.h> // for getenv 

int main(void) {
    char *incvar;
    incvar = getenv("INCLUDE");
    if (incvar != NULL)
        printf("INCLUDE environment variable = %s\n",
            incvar);
    else
        printf("Cannot find environment variable "
            "INCLUDE ");
}
@endcode
<b>Output:</b><br>
Cannot find environment variable INCLUDE
 */
char *getenv(const char *name);



/**
Description: Calculates the absolute value of a long integer.

<b>Include:</b> <stdlib.h>

@param i long integer value

@return Returns the absolute value of i.

<b>Remarks:</b><br> A negative number is returned as positive; a positive number is
unchanged.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdlib.h> // for labs 

int main(void) {
    long i;
    i = 123456;
    printf("The absolute value of %7ld is %6ld\n",
            i, labs(i));
    i = -246834;
    printf("The absolute value of %7ld is %6ld\n",
            i, labs(i));
    i = 0;
    printf("The absolute value of %7ld is %6ld\n",
            i, labs(i));
}
@endcode
<b>Output:</b><br>
The absolute value of 123456 is 123456 <br>
The absolute value of -246834 is 246834 <br>
The absolute value of 0 is 0
 */
long labs(long i);



/**
Description: Calculates the quotient and remainder of two long integers.

<b>Include:</b> <stdlib.h>

@param numer numerator

@param denom denominator

@return Returns the quotient and the remainder.

<b>Remarks:</b><br> The returned quotient will have the same sign as the numerator divided
by the denominator. The sign for the remainder will be such that the
quotient times the denominator plus the remainder will equal the
numerator (quot * denom + rem = numer). If the denominator is zero,
the behavior is undefined.

<b>Example:</b><br>
@code
#include <stdlib.h> // for ldiv, ldiv_t 
#include <stdio.h> // for printf 

int main(void) {
    long x, y;
    ldiv_t z;
    x = 7;
    y = 3;
    printf("For ldiv(%ld, %ld)\n", x, y);
    z = ldiv(x, y);
    printf("The quotient is %ld and the "
            "remainder is %ld\n\n", z.quot, z.rem);
    x = 7;
    y = -3;
    printf("For ldiv(%ld, %ld)\n", x, y);
    z = ldiv(x, y);
    printf("The quotient is %ld and the "
            "remainder is %ld\n\n", z.quot, z.rem);
    x = -5;
    y = 3;
    printf("For ldiv(%ld, %ld)\n", x, y);
    z = ldiv(x, y);
    printf("The quotient is %ld and the "
            "remainder is %ld\n\n", z.quot, z.rem);
    x = 7;
    y = 7;
    printf("For ldiv(%ld, %ld)\n", x, y);
    z = ldiv(x, y);
    printf("The quotient is %ld and the "
            "remainder is %ld\n\n", z.quot, z.rem);
    x = 7;
    y = 0;
    printf("For ldiv(%ld, %ld)\n", x, y);
    z = ldiv(x, y);
    printf("The quotient is %ld and the "
            "remainder is %ld\n\n",
            z.quot, z.rem);
}
@endcode
<b>Output:</b><br>
For ldiv(7, 3) <br>
The quotient is 2 and the remainder is 1 <br>
For ldiv(7, -3) <br>
The quotient is -2 and the remainder is 1 <br>
For ldiv(-5, 3) <br>
The quotient is -1 and the remainder is -2 <br>
For ldiv(7, 7) <br>
The quotient is 1 and the remainder is 0 <br>
For ldiv(7, 0) <br>
The quotient is -1 and the remainder is 7 <br>

Explanation: <br>
In the last example (ldiv(7,0)) the denominator is zero, the behavior
is undefined.
 */
ldiv_t ldiv(long numer, long denom);



/**
Description: Allocates memory.

<b>Include:</b> <stdlib.h>

@param size number of characters to allocate

@return Returns a pointer to the allocated space if successful; otherwise,
returns a null pointer.

<b>Remarks:</b><br> malloc does not initialize memory it returns.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf, sizeof, 
// NULL 
#include <stdlib.h> // for malloc, free 

int main(void) {
    long *i;
    if ((i = (long *) malloc(50 * sizeof (long))) ==
            NULL)
        printf("Cannot allocate memory\n");
    else {
        printf("Memory allocated\n");
        free(i);
        printf("Memory freed\n");
    }
}
@endcode
<b>Output:</b><br>
Memory allocated <br>
Memory freed
 */
void *malloc(size_t size);



/**
Description: Gets the length of a multibyte character. (See Remarks.)

<b>Include:</b> <stdlib.h>

@param s points to the multibyte character

@param n number of bytes to check

@return Returns zero if s points to a null character; otherwise, returns 1.

<b>Remarks:</b><br> The 16-bit compiler does not support multibyte characters with length
greater than 1 byte.
 */
int mblen(const char *s, size_t n);



/**
Description: Converts a multibyte string to a wide character string. (See Remarks.)

<b>Include:</b> <stdlib.h>

@param wcs points to the wide character string

@param s points to the multibyte string

@param n the number of wide characters to convert.

@return Returns the number of wide characters stored excluding the null
character.

<b>Remarks:</b><br> mbstowcs converts n number of wide characters unless it encounters
a null wide character first. The 16-bit compiler does not support
multibyte characters with length greater than 1 byte.
 */
size_t mbstowcs(wchar_t *wcs, const char *s, size_t n);



/**
Description: Converts a multibyte character to a wide character. (See Remarks.)

<b>Include:</b> <stdlib.h>

@param pwc points to the wide character

@param s points to the multibyte character

@param n number of bytes to check

@return Returns zero if s points to a null character; otherwise, returns 1.

<b>Remarks:</b><br> The resulting wide character will be stored at pwc. The 16-bit compiler
does not support multibyte characters with length greater than 1 byte.
 */
int mbtowc(wchar_t *pwc, const char *s, size_t n);



/**
Description: Performs a quick sort.

<b>Include:</b> <stdlib.h>
int (*cmp)(const void *e1, const void *e2));

@param base pointer to the start of the array

@param nelem number of elements

@param size size of the elements

@param cmp pointer to the comparison function

@param e1 pointer to the key for the search

@param e2 pointer to the element being compared with the key

@param qsort overwrites the array with the sorted array. The comparison function

@param is supplied by the user. In the following example, the list is sorted

@param according to the comparison function. This comp uses ascending order.

<b>Example:</b><br>
@code
#include <stdlib.h> // for qsort 
#include <stdio.h> // for printf 
#define NUM 7
int comp(const void *e1, const void *e2);

int main(void) {
    int list[NUM] = {35, 47, 63, 25, 93, 16, 52};
    int x;
    printf("Unsorted List: ");
    for (x = 0; x < NUM; x++)
        printf("%d ", list[x]);
    qsort(list, NUM, sizeof (int), comp);
    printf("\n");
    printf("Sorted List: ");
    for (x = 0; x < NUM; x++)
        printf("%d ", list[x]);
}

int comp(const void *e1, const void *e2) {
    const int * a1 = e1;
    const int * a2 = e2;
    if (*a1 < *a2)
        return -1;
    else if (*a1 == *a2)
        return 0;
    else
        return 1;
}

@endcode
<b>Output:</b><br>
Unsorted List: 35 47 63 25 93 16 52 <br>
Sorted List: 16 25 35 47 52 63 93
 */
void qsort(void *base, size_t nelem, size_t size,



/**
Description: Generates a pseudo-random integer.

<b>Include:</b> <stdlib.h>

@return Returns an integer between 0 and RAND_MAX.

<b>Remarks:</b><br> Calls to this function return pseudo-random integer values in the range
[0,RAND_MAX]. To use this function effectively, you must seed the random
number generator using the srand function. This function will
always return the same sequence of integers when no seeds are used
(as in the example below) or when identical seed values are used. (See
srand for seed example.)

<b>Example:</b><br>
@code
include <stdio.h> // for printf 
#include <stdlib.h> // for rand 
    int main(void) {
        int x;

        for (x = 0; x < 5; x++)
            printf("Number = %d\n", rand());
    }
@endcode
<b>Output:</b><br>
Number = 21422 <br>
Number = 2061 <br>
Number = 16443 <br>
Number = 11617 <br>
Number = 9125 <br>
Notice if the program is run a second time, the numbers are the same. <br>
See the example for srand to seed the random number generator.
 */
int rand(void);



/**
Description: Reallocates memory to allow a size change.

<b>Include:</b> <stdlib.h>

@param ptr points to previously allocated memory

@param size new size to allocate to

@return Returns a pointer to the allocated space if successful; otherwise,
returns a null pointer.

<b>Remarks:</b><br> If the existing object is smaller than the new object, the entire existing
object is copied to the new object and the remainder of the new object
is indeterminate. If the existing object is larger than the new object, the
function copies as much of the existing object as will fit in the new
object. If realloc succeeds in allocating a new object, the existing
object will be deallocated; otherwise, the existing object is left
unchanged. Keep a temporary pointer to the existing object since
realloc will return a null pointer on failure.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf, sizeof, NULL 
#include <stdlib.h> // for realloc, malloc, free 

int main(void) {
    long *i, *j;
    if ((i = (long *) malloc(50 * sizeof (long)))
            == NULL)
        printf("Cannot allocate memory\n");
    else {
        printf("Memory allocated\n");
        // Temp pointer in case realloc() fails 
        j = i;
        if ((i = (long *) realloc(i, 25 * sizeof (long)))
                == NULL) {
            printf("Cannot reallocate memory\n");
            // j pointed to allocated memory 
            free(j);
        } else {

            printf("Memory reallocated\n");
            free(i);
        }
    }
}
@endcode
<b>Output:</b><br>
Memory allocated <br>
Memory reallocated
 */
void *realloc(void *ptr, size_t size);



/**
Description: Set the starting seed for the pseudo-random number sequence.

<b>Include:</b> <stdlib.h>

@param seed starting value for the pseudo-random number sequence

@return None.

<b>Remarks:</b><br> This function sets the starting seed for the pseudo-random number
sequence generated by the rand function. The rand function will
always return the same sequence of integers when identical seed values
are used. If rand is called with a seed value of 1, the sequence of
numbers generated will be the same as if rand had been called without
srand having been called first.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdlib.h> // for rand, srand 

int main(void) {
    int x;
    srand(7);

    for (x = 0; x < 5; x++)
        printf("Number = %d\n", rand());
}
@endcode
<b>Output:</b><br>
Number = 16327 <br>
Number = 5931 <br>
Number = 23117 <br>
Number = 30985 <br>
Number = 29612
 */
void srand(unsigned int seed);



/**
Description: Converts a partial string to a floating-point number of type double.

<b>Include:</b> <stdlib.h>

@param s string to be converted

@param endptr pointer to the character at which the conversion stopped

@return Returns the converted number if successful; otherwise, returns 0.

<b>Remarks:</b><br> The number may consist of the following: <br>
[whitespace] [sign] digits [.digits] <br>
[ { e | E }[sign]digits] <br>
optional whitespace, followed by an optional sign, then a sequence
of one or more digits with an optional decimal point, followed by one
or more optional digits and an optional e or E followed by an optional
signed exponent.
strtod converts the string until it reaches a character that cannot be
converted to a number. endptr will point to the remainder of the string
starting with the first unconverted character.
If a range error occurs, errno will be set.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdlib.h> // for strtod 

int main(void) {

    char *end;
    char a[] = "1.28 inches";
    char b[] = "27.835e2i";
    char c[] = "Number1";
    double x;
    x = strtod(a, &end);
    printf("String = \"%s\" float = %f\n", a, x);
    printf("Stopped at: %s\n\n", end);
    x = strtod(b, &end);
    printf("String = \"%s\" float = %f\n", b, x);
    printf("Stopped at: %s\n\n", end);
    x = strtod(c, &end);
    printf("String = \"%s\" float = %f\n", c, x);
    printf("Stopped at: %s\n\n", end);
}
@endcode
<b>Output:</b><br>
String = "1.28 inches" float = 1.280000 <br>
Stopped at: inches <br>
String = "27.835e2i" float = 2783.500000 <br>
Stopped at: i <br>
String = "Number1" float = 0.000000 <br>
Stopped at: Number1
 */
double strtod(const char *s, char **endptr);



/**
Description: Converts a partial string to a long integer.

<b>Include:</b> <stdlib.h>

@param s string to be converted

@param endptr pointer to the character at which the conversion stopped

@param base number base to use in conversion

@return Returns the converted number if successful; otherwise, returns 0.

<b>Remarks:</b><br> If base is zero, strtol attempts to determine the base automatically.
It can be octal, determined by a leading zero, hexadecimal, determined
by a leading 0x or 0X, or decimal in any other case. If base is specified
strtol converts a sequence of digits and letters a-z (case insensitive),
where a-z represents the numbers 10-36. Conversion stops when
an out of base number is encountered. endptr will point to the remainder
of the string starting with the first unconverted character. If a range
error occurs, errno will be set.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdlib.h> // for strtol 

int main(void) {

    char *end;
    char a[] = "-12BGEE";
    char b[] = "1234Number";
    long x;
    x = strtol(a, &end, 16);
    printf("String = \"%s\" long = %ld\n", a, x);
    printf("Stopped at: %s\n\n", end);
    x = strtol(b, &end, 4);
    printf("String = \"%s\" long = %ld\n", b, x);
    printf("Stopped at: %s\n\n", end);
}
@endcode
<b>Output:</b><br>
String = "-12BGEE" long = -299 <br>
Stopped at: GEE <br>
String = "1234Number" long = 27 <br>
Stopped at: 4Number
 */
long strtol(const char *s, char **endptr, int base);




/**
Description: Converts a partial string to an unsigned long integer.

<b>Include:</b> <stdlib.h>

@param s string to be converted

@param endptr pointer to the character at which the conversion stopped

@param base number base to use in conversion

@return Returns the converted number if successful; otherwise, returns 0.

<b>Remarks:</b><br> If base is zero, strtol attempts to determine the base automatically.
It can be octal, determined by a leading zero, hexadecimal, determined
by a leading 0x or 0X, or decimal in any other case. If base is specified
strtol converts a sequence of digits and letters a-z (case insensitive),
where a-z represents the numbers 10-36. Conversion stops when
an out of base number is encountered. endptr will point to the remainder
of the string starting with the first unconverted character. If a range
error occurs, errno will be set.

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 
#include <stdlib.h> // for strtoul 

int main(void) {

    char *end;
    char a[] = "12BGET3";
    char b[] = "0x1234Number";
    char c[] = "-123abc";
    unsigned long x;
    x = strtoul(a, &end, 25);
    printf("String = \"%s\" long = %lu\n", a, x);
    printf("Stopped at: %s\n\n", end);
    x = strtoul(b, &end, 0);
    printf("String = \"%s\" long = %lu\n", b, x);
    printf("Stopped at: %s\n\n", end);
    x = strtoul(c, &end, 0);
    printf("String = \"%s\" long = %lu\n", c, x);
    printf("Stopped at: %s\n\n", end);
}
@endcode
<b>Output:</b><br>
String = "12BGET3" long = 429164 <br>
Stopped at: T3 <br>
String = "0x1234Number" long = 4660 <br>
Stopped at: Number <br>
String = "-123abc" long = 4294967173 <br>
Stopped at: abc
 */
unsigned long strtoul(const char *s, char **endptr, int base);



/**
Description: Execute a command.

<b>Include:</b> <stdlib.h>

@param s command to be executed

<b>Remarks:</b><br> This function must be customized to be used as described (see
pic30-libs). By default, system will cause a Reset if called with
anything other than NULL. system(NULL) will do nothing.

<b>Example:</b><br>
@code
    // This program uses system 
    // to TYPE its source file. 
    #include <stdlib.h> // for system 
    int main(void) {
        system("type sampsystem.c");
    }
@endcode
<b>Output:</b><br>
System(type sampsystem.c) called: Aborting
 */
int system(const char *s);



/**
Description: Converts a wide character to a multibyte character. (See Remarks.)

<b>Include:</b> <stdlib.h>

@param s points to the multibyte character

@param wchar the wide character to be converted

@return Returns zero if s points to a null character; otherwise, returns 1.

<b>Remarks:</b><br> The resulting multibyte character is stored at s. The 16-bit compiler
does not support multibyte characters with length greater than
1 character.
*/
int wctomb(char *s, wchar_t wchar);



/**
Description: Converts a wide character string to a multibyte string. (See Remarks.)

<b>Include:</b> <stdlib.h>

@param s points to the multibyte string

@param wcs points to the wide character string

@param n the number of characters to convert

@return Returns the number of characters stored excluding the null character.

<b>Remarks:</b><br> wcstombs converts n number of multibyte characters unless it
encounters a null character first. The 16-bit compiler does not support
multibyte characters with length greater than 1 character.
*/
size_t wcstombs(char *s, const wchar_t *wcs, size_t n);

/**
 * Converts an integer value to a null-terminated string using the specified base
 * and stores the result in the array given by str parameter.
 *
 * If base is 10 and value is negative, the resulting string is preceded with a
 * minus sign (-). With any other base, value is always considered unsigned.
 *
 * This function is not defined in ANSI-C and is not part of C++.
 * A standard-compliant alternative for some cases may be sprintf:
 * - sprintf(str,"%d",value) converts to decimal base.
 * - sprintf(str,"%x",value) converts to hexadecimal base.
 * - sprintf(str,"%o",value) converts to octal base.
 *
 * str should be an array long enough to contain any possible value
 *
 * <b>Example:</b><br>
 * @code
 *   int i = 1750;
 *   char buffer [33];
 *   itoa (i,buffer,10);
 *   printf ("decimal: %s\n",buffer);
 *   itoa (i,buffer,16);
 *   printf ("hexadecimal: %s\n",buffer);
 *   itoa (i,buffer,2);
 *   printf ("binary: %s\n",buffer);
 * @endcode
 * <b>Output:</b><br>
 * @code
 * decimal: 1750
 * hexadecimal: 6d6
 * binary: 11011010110
 * @endcode
 *
 * @param buf Array in memory where to store the resulting null-terminated string.
 *
 * @param val Value to be converted to a string.
 *
 * @param base Numerical base used to represent the value as a string, between 2 and
 * 36, where 10 means decimal base, 16 hexadecimal, 8 octal, and 2 binary.
 *
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 */
char*	itoa(char* buf, int val, int base);

/**
 * Converts an unsigned integer value to a null-terminated string using the specified base
 * and stores the result in the array given by str parameter.
 *
 * If base is 10 and value is negative, the resulting string is preceded with a
 * minus sign (-). With any other base, value is always considered unsigned.
 *
 * @param buf
 * @param val
 * @param base
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 */
char*	utoa(char* buf, unsigned val, int base);

/**
 * Converts a long value to a null-terminated string using the specified base
 * and stores the result in the array given by str parameter.
 *
 * If base is 10 and value is negative, the resulting string is preceded with a
 * minus sign (-). With any other base, value is always considered unsigned.
 *
 *
 * @param buf
 * @param val
 * @param base
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 */
char*	ltoa(char* buf, long val, int base);

/**
 * Converts an unsigned long value to a null-terminated string using the specified base
 * and stores the result in the array given by str parameter.
 *
 * If base is 10 and value is negative, the resulting string is preceded with a
 * minus sign (-). With any other base, value is always considered unsigned.
 *
 *
 * @param buf
 * @param val
 * @param base
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 */
char*	ultoa(char* buf, unsigned long val, int base);
