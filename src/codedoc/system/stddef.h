/**
 * @brief           Standard C Libraries - 
 * @file            stddef.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section stddef_desc Description
 **********************************************************************
The header file, stddef.h, consists of several types and macros that are of general use
in programs.
 
 **********************************************************************
 * @section stddef_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */


/**
Description: The type of the result of subtracting two pointers.

<b>Include:</b> <stddef.h>
 */
typedef	__PTRDIFF_TYPE__ ptrdiff_t;



/**
Description: The type of the result of the sizeof operator.

<b>Include:</b> <stddef.h>
 */
typedef	__SIZE_TYPE__ size_t;



/**
Description: A type that holds a wide character value.

<b>Include:</b> <stddef.h>
 */
typedef __WCHAR_TYPE__ wchar_t;



/**
Description: The value of a null pointer constant.

<b>Include:</b> <stddef.h>
 */
#define	NULL	(0)



/**
Description: Gives the offset of a structure member from the beginning of the
structure.

<b>Include:</b> <stddef.h>

@param T name of structure

@param mbr name of member in structure T

@return Returns the offset in bytes of the specified member (mbr) from the
beginning of the structure.

<b>Remarks:</b><br> The macro offsetof is undefined for bitfields. An error message will
occur if bitfields are used.

<b>Example:</b><br>
@code
#include <stddef.h> // for offsetof 
#include <stdio.h> // for printf 

struct info {
    char item1[5];
    int item2;
    char item3;
    float item4;
};

int main(void) {
    printf("Offset of item1 = %d\n",
            offsetof(struct info, item1));
    printf("Offset of item2 = %d\n",
            offsetof(struct info, item2));
    printf("Offset of item3 = %d\n",
            offsetof(struct info, item3));
    printf("Offset of item4 = %d\n",
            offsetof(struct info, item4));
}
@endcode
<b>Output:</b><br>
- Offset of item1 = 0
- Offset of item2 = 6
- Offset of item3 = 8
- Offset of item4 = 10

Explanation: <br>
This program shows the offset in bytes of each structure member from
the start of the structure. Although item1 is only 5 bytes (char
item1[5]), padding is added so the address of item2 falls on an
even boundary. The same occurs with item3; it is 1 byte (char
item3) with 1 byte of padding.
 */
#define	offsetof(T, mbr)	((int)&(((T *)0)->mbr))
