/**
 * @brief           Standard C Libraries - 
 * @file            ctype.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section ctype_desc Description
 **********************************************************************
The header file, ctype.h, consists of functions that are useful for classifying and
mapping characters. Characters are interpreted according to the Standard C locale.
 
 **********************************************************************
 * @section ctype_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */


/**
Description: Test for an alphanumeric character.

<b>Include:</b> <ctype.h>

@param c The character to test.

@return Returns a non-zero integer value if the character is alphanumeric;
otherwise, returns a zero.

<b>Remarks:</b><br> Alphanumeric characters are included within the ranges A-Z, a-z or 0-9.

<b>Example:</b><br>
@code
#include <ctype.h> // for isalnum 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = '3';
    if (isalnum(ch))
        printf("3 is an alphanumeric\n");
    else
        printf("3 is NOT an alphanumeric\n");
    ch = '#';
    if (isalnum(ch))
        printf("# is an alphanumeric\n");
    else
        printf("# is NOT an alphanumeric\n");
}
@endcode
<b>Output:</b><br>
3 is an alphanumeric <br>
\# is NOT an alphanumeric
 */
int isalnum(int c);



/**
Description: Test for an alphabetic character.

<b>Include:</b> <ctype.h>

@param c The character to test.

@return Returns a non-zero integer value if the character is alphabetic;
otherwise, returns zero.

<b>Remarks:</b><br> Alphabetic characters are included within the ranges A-Z or a-z.

<b>Example:</b><br>
@code
#include <ctype.h> // for isalpha 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = 'B';
    if (isalpha(ch))
        printf("B is alphabetic\n");
    else
        printf("B is NOT alphabetic\n");
    ch = '#';
    if (isalpha(ch))
        printf("# is alphabetic\n");
    else
        printf("# is NOT alphabetic\n");
}
@endcode
<b>Output:</b><br>
B is alphabetic <br>
\# is NOT alphabetic
 */
int isalpha(int c);



/**
Description: Test for a control character.

<b>Include:</b> <ctype.h>

@param c character to test.

@return Returns a non-zero integer value if the character is a control character;
otherwise, returns zero.

<b>Remarks:</b><br> A character is considered to be a control character if its ASCII value is
in the range, 0x00 to 0x1F inclusive, or 0x7F.

<b>Example:</b><br>
@code
#include <ctype.h> // for iscntrl 
#include <stdio.h> // for printf 

int main(void) {
    char ch;
    ch = 'B';
    if (iscntrl(ch))
        printf("B is a control character\n");
    else
        printf("B is NOT a control character\n");
    ch = '\t';
    if (iscntrl(ch))
        printf("A tab is a control character\n");
    else
        printf("A tab is NOT a control character\n");
}
@endcode
<b>Output:</b><br>
B is NOT a control character <br>
A tab is a control character
 */
int iscntrl(int c);



/**
Description: Test for a decimal digit.

<b>Include:</b> <ctype.h>

@param c character to test.

@return Returns a non-zero integer value if the character is a digit; otherwise,
returns zero.

<b>Remarks:</b><br> A character is considered to be a digit character if it is in the range of
‘0'- ‘9'.

<b>Example:</b><br>
@code
#include <ctype.h> // for isdigit 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = '3';
    if (isdigit(ch))
        printf("3 is a digit\n");
    else
        printf("3 is NOT a digit\n");
    ch = '#';
    if (isdigit(ch))
        printf("# is a digit\n");
    else
        printf("# is NOT a digit\n");
}
@endcode
<b>Output:</b><br>
3 is a digit <br>
\# is NOT a digit
 */
int isdigit(int c);



/**
Description: Test for a graphical character.

<b>Include:</b> <ctype.h>

@param c character to test

@return Returns a non-zero integer value if the character is a graphical
character; otherwise, returns zero.

<b>Remarks:</b><br> A character is considered to be a graphical character if it is any
printable character except a space.

<b>Example:</b><br>
@code
#include <ctype.h> // for isgraph 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = '3';
    if (isgraph(ch))
        printf("3 is a graphical character\n");
    else
        printf("3 is NOT a graphical character\n");
    ch = '#';
    if (isgraph(ch))
        printf("# is a graphical character\n");
    else
        printf("# is NOT a graphical character\n");
    ch = ' ';
    if (isgraph(ch))
        printf("a space is a graphical character\n");
    else
        printf("a space is NOT a graphical character\n");
}
@endcode
<b>Output:</b><br>
3 is a graphical character <br>
\# is a graphical character <br>
a space is NOT a graphical character
 */
int isgraph(int c);



/**
Description: Test for a lower case alphabetic character.

<b>Include:</b> <ctype.h>

@param c character to test

@return Returns a non-zero integer value if the character is a lower case
alphabetic character; otherwise, returns zero.

<b>Remarks:</b><br> A character is considered to be a lower case alphabetic character if it is
in the range of ‘a'-‘z'.

<b>Example:</b><br>
@code
#include <ctype.h> // for islower 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = 'B';
    if (islower(ch))
        printf("B is lower case\n");
    else
        printf("B is NOT lower case\n");
    ch = 'b';
    if (islower(ch))
        printf("b is lower case\n");
    else
        printf("b is NOT lower case\n");
}
@endcode
<b>Output:</b><br>
B is NOT lower case <br>
b is lower case
 */
int islower(int c);



/**
Description: Test for a printable character (includes a space).

<b>Include:</b> <ctype.h>

@param c character to test

@return Returns a non-zero integer value if the character is printable;
otherwise, returns zero.

<b>Remarks:</b><br> A character is considered to be a printable character if it is in the range,
0x20 to 0x7e inclusive.

<b>Example:</b><br>
@code
#include <ctype.h> // for isprint 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = '&';
    if (isprint(ch))
        printf("& is a printable character\n");
    else
        printf("& is NOT a printable character\n");
    ch = '\t';
    if (isprint(ch))
        printf("a tab is a printable character\n");
    else
        printf("a tab is NOT a printable character\n");
}
@endcode
<b>Output:</b><br>
& is a printable character <br>
a tab is NOT a printable character
 */
int isprint(int c);




/**
Description: Test for a punctuation character.

<b>Include:</b> <ctype.h>

@param c character to test

@return Returns a non-zero integer value if the character is a punctuation
character; otherwise, returns zero.

<b>Remarks:</b><br> A character is considered to be a punctuation character if it is a printable
character which is neither a space nor an alphanumeric character.
Punctuation characters consist of the following:
@code
! " # $ % & ' ( ) ; < = > ? @ [ \ ] * + , - . / : ^ _ { | } ~
@endcode

<b>Example:</b><br>
@code
#include <ctype.h> // for ispunct 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = '&';
    if (ispunct(ch))
        printf("& is a punctuation character\n");
    else
        printf("& is NOT a punctuation character\n");
    ch = '\t';
    if (ispunct(ch))
        printf("a tab is a punctuation character\n");
    else
        printf("a tab is NOT a punctuation character\n");
}
@endcode
<b>Output:</b><br>
& is a punctuation character <br>
a tab is NOT a punctuation character
 */
int ispunct(int c);



/**
Description: Test for a white-space character.

<b>Include:</b> <ctype.h>

@param c character to test

@return Returns a non-zero integer value if the character is a white-space
character; otherwise, returns zero.

<b>Remarks:</b><br> A character is considered to be a white-space character if it is one of
the following:
@code
space (' '), form feed ('\f'), newline ('\n'), carriage return('\r'), horizontal tab ('\t'), or vertical tab ('\v')
@endcode

<b>Example:</b><br>
@code
#include <ctype.h> // for isspace 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = '&';
    if (isspace(ch))
        printf("& is a white-space character\n");
    else
        printf("& is NOT a white-space character\n");
    ch = '\t';
    if (isspace(ch))
        printf("a tab is a white-space character\n");
    else
        printf("a tab is NOT a white-space character\n");
}
@endcode
<b>Output:</b><br>
& is NOT a white-space character <br>
a tab is a white-space character
 */
int isspace(int c);



/**
Description: Test for an upper case letter.

<b>Include:</b> <ctype.h>

@param c character to test

@return Returns a non-zero integer value if the character is an upper case
alphabetic character; otherwise, returns zero.

<b>Remarks:</b><br> A character is considered to be an upper case alphabetic character if it
is in the range of ‘A'-‘Z'.

<b>Example:</b><br>
@code
#include <ctype.h> // for isupper 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = 'B';
    if (isupper(ch))
        printf("B is upper case\n");
    else
        printf("B is NOT upper case\n");
    ch = 'b';
    if (isupper(ch))
        printf("b is upper case\n");
    else
        printf("b is NOT upper case\n");
}
@endcode
<b>Output:</b><br>
B is upper case <br>
b is NOT upper case
 */
int isupper(int c);




/**
Description: Test for a hexadecimal digit.

<b>Include:</b> <ctype.h>

@param c character to test

@return Returns a non-zero integer value if the character is a hexadecimal digit;
otherwise, returns zero.

<b>Remarks:</b><br> A character is considered to be a hexadecimal digit character if it is in
the range of ‘0'-‘9', ‘A'-‘F', or ‘a'-‘f'. Note: The list does not include the
leading 0x because 0x is the prefix for a hexadecimal number but is not
an actual hexadecimal digit.

<b>Example:</b><br>
@code
#include <ctype.h> // for isxdigit 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = 'B';
    if (isxdigit(ch))
        printf("B is a hexadecimal digit\n");
    else
        printf("B is NOT a hexadecimal digit\n");
    ch = 't';
    if (isxdigit(ch))
        printf("t is a hexadecimal digit\n");
    else
        printf("t is NOT a hexadecimal digit\n");
}
@endcode
<b>Output:</b><br>
B is a hexadecimal digit <br>
t is NOT a hexadecimal digit
 */
int isxdigit(int c);



/**
Description: Convert a character to a lower case alphabetical character.

<b>Include:</b> <ctype.h>

@param c The character to convert to lower case.

@return Returns the corresponding lower case alphabetical character if the
argument was originally upper case; otherwise, returns the original
character.

<b>Remarks:</b><br> Only upper case alphabetical characters may be converted to lower
case.

<b>Example:</b><br>
@code
#include <ctype.h> // for tolower 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = 'B';
    printf("B changes to lower case %c\n",
            tolower(ch));
    ch = 'b';
    printf("b remains lower case %c\n",
            tolower(ch));
    ch = '@';
    printf("@ has no lower case, ");
    printf("so %c is returned\n", tolower(ch));
}
@endcode
<b>Output:</b><br>
B changes to lower case b <br>
b remains lower case b <br>
\@ has no lower case, so \@ is returned
 */
int tolower(int c);




/**
Description: Convert a character to an upper case alphabetical character.

<b>Include:</b> <ctype.h>

@param c The character to convert to upper case.

@return Returns the corresponding upper case alphabetical character if the
argument was originally lower case; otherwise, returns the original
character.

<b>Remarks:</b><br> Only lower case alphabetical characters may be converted to upper
case.

<b>Example:</b><br>
@code
#include <ctype.h> // for toupper 
#include <stdio.h> // for printf 

int main(void) {
    int ch;
    ch = 'b';
    printf("b changes to upper case %c\n",
            toupper(ch));
    ch = 'B';
    printf("B remains upper case %c\n",
            toupper(ch));
    ch = '@';
    printf("@ has no upper case, ");
    printf("so %c is returned\n", toupper(ch));
}
@endcode
<b>Output:</b><br>
b changes to upper case B <br>
B remains upper case B <br>
\@ has no upper case, so \@ is returned
 */
int toupper(int c);
