/**
 * @brief           Standard C Libraries
 * @file            string.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section strings_desc Description
 **********************************************************************
The header file, string.h, consists of types, macros and functions that provide tools
to manipulate strings.
 
 **********************************************************************
 * @section strings_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original lincense
 * agreement included with the XC16 compiler applies!
 */


/**
Description: The type of the result of the sizeof operator.

<b>Include:</b> <string.h>
 */
#define size_t size_t



/**
Description: The value of a null pointer constant.

<b>Include:</b> <string.h>
 */
#define	NULL	(0)



/**
Description: Locates a character in a buffer.

<b>Include:</b> <string.h>

@param s pointer to the buffer

@param c character to search for

@param n number of characters to check

@return Returns a pointer to the location of the match if successful; otherwise,
returns null.

<b>Remarks:</b><br> memchr stops when it finds the first occurrence of c or after searching
n number of characters.

<b>Example:</b><br>
@code
#include <string.h> // for memchr, NULL 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "What time is it?";
    char ch1 = 'i', ch2 = 'y';
    char *ptr;
    int res;
    printf("buf1 : %s\n\n", buf1);
    ptr = memchr(buf1, ch1, 50);
    if (ptr != NULL) {
        res = ptr - buf1 + 1;
        printf("%c found at position %d\n", ch1, res);
    } else
        printf("%c not found\n", ch1);
    printf("\n");
    ptr = memchr(buf1, ch2, 50);
    if (ptr != NULL) {
        res = ptr - buf1 + 1;
        printf("%c found at position %d\n", ch2, res);
    } else
        printf("%c not found\n", ch2);
}
@endcode
<b>Output:</b><br>
buf1 : What time is it? <br>
i found at position 7 <br>
y not found
 */
void *memchr(const void *s, int c, size_t n);



/**
Description: Compare the contents of two buffers.

<b>Include:</b> <string.h>

@param s1 first buffer

@param s2 second buffer

@param n number of characters to compare

@return Returns a positive number if s1 is greater than s2, zero if s1 is equal to
s2 or a negative number if s1 is less than s2.

<b>Remarks:</b><br> This function compares the first n characters in s1 to the first n characters
in s2 and returns a value indicating whether the buffers are less
than, equal to or greater than each other.

<b>Example:</b><br>
@code
#include <string.h> // memcmp 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "Where is the time?";
    char buf2[50] = "Where did they go?";
    char buf3[50] = "Why?";
    int res;
    printf("buf1 : %s\n", buf1);
    printf("buf2 : %s\n", buf2);
    printf("buf3 : %s\n\n", buf3);
    res = memcmp(buf1, buf2, 6);
    if (res < 0)
        printf("buf1 comes before buf2\n");
    else if (res == 0)
        printf("6 characters of buf1 and buf2 "
            "are equal\n");
    else
        printf("buf2 comes before buf1\n");
    printf("\n");
    res = memcmp(buf1, buf2, 20);
    if (res < 0)
        printf("buf1 comes before buf2\n");
    else if (res == 0)
        printf("20 characters of buf1 and buf2 "
            "are equal\n");
    else
        printf("buf2 comes before buf1\n");
    printf("\n");
    res = memcmp(buf1, buf3, 20);
    if (res < 0)
        printf("buf1 comes before buf3\n");
    else if (res == 0)
        printf("20 characters of buf1 and buf3 "
            "are equal\n");
    else
        printf("buf3 comes before buf1\n");
}
@endcode
<b>Output:</b><br>
buf1 : Where is the time? <br>
buf2 : Where did they go? <br>
buf3 : Why? <br>
6 characters of buf1 and buf2 are equal <br>
buf2 comes before buf1 <br>
buf1 comes before buf3
 */
int memcmp(const void *s1, const void *s2, size_t n);



/**
Description: Copies characters from one buffer to another.

<b>Include:</b> <string.h>

@param dst buffer to copy characters to

@param src buffer to copy characters from

@param n number of characters to copy

@return Returns dst.

<b>Remarks:</b><br> memcpy copies n characters from the source buffer src to the
destination buffer dst. If the buffers overlap, the behavior is undefined.

<b>Example:</b><br>
@code
#include <string.h> // memcpy 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "";
    char buf2[50] = "Where is the time?";
    char buf3[50] = "Why?";
    printf("buf1 : %s\n", buf1);
    printf("buf2 : %s\n", buf2);
    printf("buf3 : %s\n\n", buf3);
    memcpy(buf1, buf2, 6);
    printf("buf1 after memcpy of 6 chars of "
            "buf2: \n\t%s\n", buf1);
    printf("\n");
    memcpy(buf1, buf3, 5);
    printf("buf1 after memcpy of 5 chars of "
            "buf3: \n\t%s\n", buf1);
}
@endcode
<b>Output:</b><br>
buf1 : <br>
buf2 : Where is the time? <br>
buf3 : Why? <br>
buf1 after memcpy of 6 chars of buf2: <br>
Where <br>
buf1 after memcpy of 5 chars of buf3: <br>
Why?
 */
void *memcpy(void *dst, const void *src, size_t n);



/**
Description: Copies n characters of the source buffer into the destination buffer,
even if the regions overlap.

<b>Include:</b> <string.h>

@param s1 buffer to copy characters to (destination)

@param s2 buffer to copy characters from (source)

@param n number of characters to copy from s2 to s1

@return Returns a pointer to the destination buffer.

<b>Remarks:</b><br> If the buffers overlap, the effect is as if the characters are read first from
s2, then written to s1, so the buffer is not corrupted.

<b>Example:</b><br>
@code
#include <string.h> // for memmove 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "When time marches on";
    char buf2[50] = "Where is the time?";
    char buf3[50] = "Why?";
    printf("buf1 : %s\n", buf1);
    printf("buf2 : %s\n", buf2);
    printf("buf3 : %s\n\n", buf3);
    memmove(buf1, buf2, 6);
    printf("buf1 after memmove of 6 chars of "
            "buf2: \n\t%s\n", buf1);
    printf("\n");
    memmove(buf1, buf3, 5);
    printf("buf1 after memmove of 5 chars of "
            "buf3: \n\t%s\n", buf1);
}
@endcode
<b>Output:</b><br>
buf1 : When time marches on <br>
buf2 : Where is the time? <br>
buf3 : Why? <br>
buf1 after memmove of 6 chars of buf2: <br>
Where ime marches on <br>
buf1 after memmove of 5 chars of buf3: <br>
Why?
 */
void *memmove(void *s1, const void *s2, size_t n);



/**
@function void *memset(void *s, int c, size_t n)

Description: Copies the specified character into the destination buffer.

<b>Include:</b> <string.h>

@param s buffer

@param c character to put in buffer

@param n number of times

@return Returns the buffer with characters written to it.

<b>Remarks:</b><br> The character, c, is written to the buffer n times.

<b>Example:</b><br>
@code
#include <string.h> // for memset 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[20] = "What time is it?";
    char buf2[20] = "";
    char ch1 = '?', ch2 = 'y';
    char *ptr;
    int res;
    printf("memset(\"%s\", \'%c\',4);\n", buf1, ch1);
    memset(buf1, ch1, 4);
    printf("buf1 after memset: %s\n", buf1);
    printf("\n");
    printf("memset(\"%s\", \'%c\',10);\n", buf2, ch2);
    memset(buf2, ch2, 10);
    printf("buf2 after memset: %s\n", buf2);
}
@endcode
<b>Output:</b><br>
memset("What time is it?", '?',4); <br>
buf1 after memset: ???? time is it? <br>
memset("", 'y',10); <br>
buf2 after memset: yyyyyyyyyy
 */
void *memset(void *s, int c, size_t n);



/**
Description: Appends a copy of the source string to the end of the destination string.

<b>Include:</b> <string.h>

@param s1 null terminated destination string to copy to

@param s2 null terminated source string to be copied

@return Returns a pointer to the destination string.

<b>Remarks:</b><br> This function appends the source string (including the terminating null
character) to the end of the destination string. The initial character of
the source string overwrites the null character at the end of the
destination string. If the buffers overlap, the behavior is undefined.

<b>Example:</b><br>
@code
#include <string.h> // for strcat, strlen 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "We're here";
    char buf2[50] = "Where is the time?";
    printf("buf1 : %s\n", buf1);
    printf("\t(%d characters)\n\n", strlen(buf1));
    printf("buf2 : %s\n", buf2);
    printf("\t(%d characters)\n\n", strlen(buf2));
    strcat(buf1, buf2);
    printf("buf1 after strcat of buf2: \n\t%s\n",
            buf1);
    printf("\t(%d characters)\n", strlen(buf1));
    printf("\n");
    strcat(buf1, "Why?");
    printf("buf1 after strcat of \"Why?\": \n\t%s\n",
            buf1);
    printf("\t(%d characters)\n", strlen(buf1));
}
@endcode
<b>Output:</b><br>
buf1 : We're here <br>
(10 characters) <br>
buf2 : Where is the time? <br>
(18 characters) <br>
buf1 after strcat of buf2: <br>
We're hereWhere is the time? <br>
(28 characters) <br>
buf1 after strcat of "Why?": <br>
We're hereWhere is the time?Why? <br>
(32 characters)
 */
char *strcat(char *s1, const char *s2);



/**
Description: Locates the first occurrence of a specified character in a string.

<b>Include:</b> <string.h>

@param s pointer to the string

@param c character to search for

@return Returns a pointer to the location of the match if successful; otherwise,
returns a null pointer.

<b>Remarks:</b><br> This function searches the string s to find the first occurrence of the
character, c.

<b>Example:</b><br>
@code
#include <string.h> // for strchr, NULL 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "What time is it?";
    char ch1 = 'm', ch2 = 'y';
    char *ptr;
    int res;
    printf("buf1 : %s\n\n", buf1);
    ptr = strchr(buf1, ch1);
    if (ptr != NULL) {
        res = ptr - buf1 + 1;
        printf("%c found at position %d\n", ch1, res);
    } else
        printf("%c not found\n", ch1);
    printf("\n");
    ptr = strchr(buf1, ch2);
    if (ptr != NULL) {
        res = ptr - buf1 + 1;
        printf("%c found at position %d\n", ch2, res);
    } else
        printf("%c not found\n", ch2);
}
@endcode
<b>Output:</b><br>
buf1 : What time is it? <br>
m found at position 8 <br>
y not found
 */
char *strchr(const char *s, int c);



/**
Description: Compares two strings.

<b>Include:</b> <string.h>

@param s1 first string

@param s2 second string

@return Returns a positive number if s1 is greater than s2, zero if s1 is equal to
s2 or a negative number if s1 is less than s2.

<b>Remarks:</b><br> This function compares successive characters from s1 and s2 until
they are not equal or the null terminator is reached.

<b>Example:</b><br>
@code
#include <string.h> // for strcmp 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "Where is the time?";
    char buf2[50] = "Where did they go?";
    char buf3[50] = "Why?";
    int res;
    printf("buf1 : %s\n", buf1);
    printf("buf2 : %s\n", buf2);
    printf("buf3 : %s\n\n", buf3);
    res = strcmp(buf1, buf2);
    if (res < 0)
        printf("buf1 comes before buf2\n");
    else if (res == 0)
        printf("buf1 and buf2 are equal\n");
    else
        printf("buf2 comes before buf1\n");
    printf("\n");
    res = strcmp(buf1, buf3);
    if (res < 0)
        printf("buf1 comes before buf3\n");
    else if (res == 0)
        printf("buf1 and buf3 are equal\n");
    else
        printf("buf3 comes before buf1\n");
    printf("\n");
    res = strcmp("Why?", buf3);
    if (res < 0)
        printf("\"Why?\" comes before buf3\n");
    else if (res == 0)
        printf("\"Why?\" and buf3 are equal\n");
    else
        printf("buf3 comes before \"Why?\"\n");
}
@endcode
<b>Output:</b><br>
buf1 : Where is the time? <br>
buf2 : Where did they go? <br>
buf3 : Why? <br>
buf2 comes before buf1 <br>
buf1 comes before buf3 <br>
"Why?" and buf3 are equal
 */
int strcmp(const char *s1, const char *s2);



/**
Description: Compares one string to another. (See Remarks.)

<b>Include:</b> <string.h>

@param s1 first string

@param s2 second string

@return Using the locale-dependent rules, it returns a positive number if s1 is
greater than s2, zero if s1 is equal to s2 or a negative number if s1 is
less than s2.

<b>Remarks:</b><br> Since the 16-bit compiler does not support alternate locales, this
function is equivalent to strcmp.
 */
int strcoll(const char *s1, const char *s2);



/**
Description: Copy the source string into the destination string.

<b>Include:</b> <string.h>

@param s1 destination string to copy to

@param s2 source string to copy from

@return Returns a pointer to the destination string.

<b>Remarks:</b><br> All characters of s2 are copied, including the null terminating character.
If the strings overlap, the behavior is undefined.

<b>Example:</b><br>
@code
#include <string.h> // for strcpy, strlen 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "We're here";
    char buf2[50] = "Where is the time?";
    char buf3[50] = "Why?";
    printf("buf1 : %s\n", buf1);
    printf("buf2 : %s\n", buf2);
    printf("buf3 : %s\n\n", buf3);
    strcpy(buf1, buf2);
    printf("buf1 after strcpy of buf2: \n\t%s\n\n",
            buf1);
    strcpy(buf1, buf3);
    printf("buf1 after strcpy of buf3: \n\t%s\n",
            buf1);
}
@endcode
<b>Output:</b><br>
buf1 : We're here <br>
buf2 : Where is the time? <br>
buf3 : Why? <br>
buf1 after strcpy of buf2: <br>
Where is the time? <br>
buf1 after strcpy of buf3: <br>
Why?
 */
char *strcpy(char *s1, const char *s2);



/**
Description: Calculate the number of consecutive characters at the beginning of a
string that are not contained in a set of characters.

<b>Include:</b> <string.h>

@param s1 pointer to the string to be searched

@param s2 pointer to characters to search for

@return Returns the length of the segment in s1 not containing characters
found in s2.

<b>Remarks:</b><br> This function will determine the number of consecutive characters from
the beginning of s1 that are not contained in s2.

<b>Example:</b><br>
@code
#include <string.h> // for strcspn 
#include <stdio.h> // for printf 

int main(void) {
    char str1[20] = "hello";
    char str2[20] = "aeiou";
    char str3[20] = "animal";
    char str4[20] = "xyz";
    int res;
    res = strcspn(str1, str2);
    printf("strcspn(\"%s\", \"%s\") = %d\n",
            str1, str2, res);
    res = strcspn(str3, str2);
    printf("strcspn(\"%s\", \"%s\") = %d\n",
            str3, str2, res);
    res = strcspn(str3, str4);
    printf("strcspn(\"%s\", \"%s\") = %d\n",
            str3, str4, res);
}
@endcode
<b>Output:</b><br>
strcspn("hello", "aeiou") = 1 <br>
strcspn("animal", "aeiou") = 0 <br>
strcspn("animal", "xyz") = 6 <br>

Explanation: <br>
In the first result, e is in s2 so it stops counting after h. <br>
In the second result, a is in s2. <br>
In the third result, none of the characters of s1 are in s2 so all
characters are counted.
 */
size_t strcspn(const char *s1, const char *s2);



/**
Description: Gets an internal error message.

<b>Include:</b> <string.h>

@param errcode number of the error code

@return Returns a pointer to an internal error message string corresponding to
the specified error code errcode.

<b>Remarks:</b><br> The array pointed to by strerror may be overwritten by a
subsequent call to this function.

<b>Example:</b><br>
@code
#include <stdio.h> // for fopen, fclose, 
// printf, FILE, NULL 
#include <string.h> // for strerror 
#include <errno.h> // for errno 

int main(void) {
    FILE *myfile;
    if ((myfile = fopen("samp.fil", "r+")) == NULL)
        printf("Cannot open samp.fil: %s\n",
            strerror(errno));
    else
        printf("Success opening samp.fil\n");
    fclose(myfile);
}
@endcode
<b>Output:</b><br>
Cannot open samp.fil: file open error
 */
char *strerror(int errcode);



/**
Description: Finds the length of a string.

<b>Include:</b> <string.h>

@param s the string

@return Returns the length of a string.

<b>Remarks:</b><br> This function determines the length of the string, not including the
terminating null character.

<b>Example:</b><br>
@code
#include <string.h> // for strlen 
#include <stdio.h> // for printf 

int main(void) {
    char str1[20] = "We are here";
    char str2[20] = "";
    char str3[20] = "Why me?";
    printf("str1 : %s\n", str1);
    printf("\t(string length = %d characters)\n\n",
            strlen(str1));
    printf("str2 : %s\n", str2);
    printf("\t(string length = %d characters)\n\n",
            strlen(str2));
    printf("str3 : %s\n", str3);
    printf("\t(string length = %d characters)\n\n\n",
            strlen(str3));
}
@endcode
<b>Output:</b><br>
str1 : We are here <br>
(string length = 11 characters) <br>
str2 : <br>
(string length = 0 characters) <br>
str3 : Why me? <br>
(string length = 7 characters)
 */
size_t strlen(const char *s);



/**
Description: Append a specified number of characters from the source string to the
destination string.

<b>Include:</b> <string.h>

@param s1 destination string to copy to

@param s2 source string to copy from

@param n number of characters to append

@return Returns a pointer to the destination string.

<b>Remarks:</b><br> This function appends up to n characters (a null character and characters
that follow it are not appended) from the source string to the end of
the destination string. If a null character is not encountered, then a terminating
null character is appended to the result. If the strings overlap,
the behavior is undefined.

<b>Example:</b><br>
@code
#include <string.h> // for strncat, strlen 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "We're here";
    char buf2[50] = "Where is the time?";
    char buf3[50] = "Why?";
    printf("buf1 : %s\n", buf1);
    printf("\t(%d characters)\n\n", strlen(buf1));
    printf("buf2 : %s\n", buf2);
    printf("\t(%d characters)\n\n", strlen(buf2));
    printf("buf3 : %s\n", buf3);
    printf("\t(%d characters)\n\n\n", strlen(buf3));
    strncat(buf1, buf2, 6);
    printf("buf1 after strncat of 6 characters "
            "of buf2: \n\t%s\n", buf1);
    printf("\t(%d characters)\n", strlen(buf1));
    printf("\n");
    strncat(buf1, buf2, 25);
    printf("buf1 after strncat of 25 characters "
            "of buf2: \n\t%s\n", buf1);
    printf("\t(%d characters)\n", strlen(buf1));
    printf("\n");
    strncat(buf1, buf3, 4);
    printf("buf1 after strncat of 4 characters "
            "of buf3: \n\t%s\n", buf1);
    printf("\t(%d characters)\n", strlen(buf1));
}
@endcode
<b>Output:</b><br>
buf1 : We're here <br>
(10 characters) <br>
buf2 : Where is the time? <br>
(18 characters) <br>
buf3 : Why? <br>
(4 characters) <br>
buf1 after strncat of 6 characters of buf2: <br>
We're hereWhere <br>
(16 characters) <br>
buf1 after strncat of 25 characters of buf2: <br>
We're hereWhere Where is the time? <br>
(34 characters) <br>
buf1 after strncat of 4 characters of buf3: <br>
We're hereWhere Where is the time?Why? <br>
(38 characters)
 */
char *strncat(char *s1, const char *s2, size_t n);



/**
Description: Compare two strings, up to a specified number of characters.

<b>Include:</b> <string.h>

@param s1 first string

@param s2 second string

@param n number of characters to compare

@return Returns a positive number if s1 is greater than s2, zero if s1 is equal to
s2 or a negative number if s1 is less than s2.

<b>Remarks:</b><br> strncmp returns a value based on the first character that differs
between s1 and s2. Characters that follow a null character are not
compared.

<b>Example:</b><br>
@code
#include <string.h> // for strncmp 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "Where is the time?";
    char buf2[50] = "Where did they go?";
    char buf3[50] = "Why?";
    int res;
    printf("buf1 : %s\n", buf1);
    printf("buf2 : %s\n", buf2);
    printf("buf3 : %s\n\n", buf3);
    res = strncmp(buf1, buf2, 6);
    if (res < 0)
        printf("buf1 comes before buf2\n");
    else if (res == 0)
        printf("6 characters of buf1 and buf2 "
            "are equal\n");
    else
        printf("buf2 comes before buf1\n");
    printf("\n");
    res = strncmp(buf1, buf2, 20);
    if (res < 0)
        printf("buf1 comes before buf2\n");
    else if (res == 0)
        printf("20 characters of buf1 and buf2 "
            "are equal\n");
    else
        printf("buf2 comes before buf1\n");
    printf("\n");
    res = strncmp(buf1, buf3, 20);
    if (res < 0)
        printf("buf1 comes before buf3\n");
    else if (res == 0)
        printf("20 characters of buf1 and buf3 "
            "are equal\n");
    else
        printf("buf3 comes before buf1\n");
}
@endcode
<b>Output:</b><br>
buf1 : Where is the time? <br>
buf2 : Where did they go? <br>
buf3 : Why? <br>
6 characters of buf1 and buf2 are equal <br>
buf2 comes before buf1 <br>
buf1 comes before buf3
 */
int strncmp(const char *s1, const char *s2, size_t n);



/**
Description: Copy characters from the source string into the destination string, up to
the specified number of characters.

<b>Include:</b> <string.h>

@param s1 destination string to copy to

@param s2 source string to copy from

@param n number of characters to copy

@return Returns a pointer to the destination string.

<b>Remarks:</b><br> Copies n characters from the source string to the destination string. If
the source string is less than n characters, the destination is filled with
null characters to total n characters. If n characters were copied and no
null character was found, then the destination string will not be
null-terminated. If the strings overlap, the behavior is undefined.

<b>Example:</b><br>
@code
#include <string.h> // for strncpy, strlen 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "We're here";
    char buf2[50] = "Where is the time?";
    char buf3[50] = "Why?";
    char buf4[7] = "Where?";
    printf("buf1 : %s\n", buf1);
    printf("buf2 : %s\n", buf2);
    printf("buf3 : %s\n", buf3);
    printf("buf4 : %s\n", buf4);
    strncpy(buf1, buf2, 6);
    printf("buf1 after strncpy of 6 characters "
            "of buf2: \n\t%s\n", buf1);
    printf("\t( %d characters)\n", strlen(buf1));
    printf("\n");
    strncpy(buf1, buf2, 18);
    printf("buf1 after strncpy of 18 characters "
            "of buf2: \n\t%s\n", buf1);
    printf("\t( %d characters)\n", strlen(buf1));
    printf("\n");
    strncpy(buf1, buf3, 5);
    printf("buf1 after strncpy of 5 characters "
            "of buf3: \n\t%s\n", buf1);
    printf("\t( %d characters)\n", strlen(buf1));
    printf("\n");
    strncpy(buf1, buf4, 9);
    printf("buf1 after strncpy of 9 characters "
            "of buf4: \n\t%s\n", buf1);
    printf("\t( %d characters)\n", strlen(buf1));
}
@endcode
<b>Output:</b><br>
buf1 : We're here <br>
buf2 : Where is the time? <br>
buf3 : Why? <br>
buf4 : Where? <br>
buf1 after strncpy of 6 characters of buf2: <br>
Where here <br>
( 10 characters) <br>
buf1 after strncpy of 18 characters of buf2: <br>
Where is the time? <br>
( 18 characters) <br>
buf1 after strncpy of 5 characters of buf3: <br>
Why? <br>
( 4 characters) <br>
buf1 after strncpy of 9 characters of buf4: <br>
Where? <br>
( 6 characters) <br>

Explanation: <br>
Each buffer contains the string shown, followed by null characters for a
length of 50. Using strlen will find the length of the string up to, but
not including, the first null character.

In the first example, 6 characters of buf2 ("Where ") replace the first
6 characters of buf1 ("We're ") and the rest of buf1 remains the same
("here" plus null characters).

In the second example, 18 characters replace the first 18 characters of
buf1 and the rest remain null characters.

In the third example, 5 characters of buf3 ("Why?" plus a null terminating
character) replace the first 5 characters of buf1. buf1 now actually
contains ("Why?", 1 null character, " is the time?", 32 null characters).
strlen shows 4 characters because it stops when it reaches the first
null character.

In the fourth example, since buf4 is only 7 characters strncpy uses
2 additional null characters to replace the first 9 characters of buf1. The
result of buf1 is 6 characters ("Where?") followed by 3 null characters,
followed by 9 characters ("the time?"), followed by 32 null characters.
 */
char *strncpy(char *s1, const char *s2, size_t n);



/**
Description: Search a string for the first occurrence of a character from a specified
set of characters.

<b>Include:</b> <string.h>

@param s1 pointer to the string to be searched

@param s2 pointer to characters to search for

@return Returns a pointer to the matched character in s1 if found; otherwise,
returns a null pointer.

<b>Remarks:</b><br> This function will search s1 for the first occurrence of a character
contained in s2.

<b>Example:</b><br>
@code
#include <string.h> // for strpbrk, NULL 
#include <stdio.h> // for printf 

int main(void) {
    char str1[20] = "What time is it?";
    char str2[20] = "xyz";
    char str3[20] = "eou?";
    char *ptr;
    int res;
    printf("strpbrk(\"%s\", \"%s\")\n", str1, str2);
    ptr = strpbrk(str1, str2);
    if (ptr != NULL) {
        res = ptr - str1 + 1;
        printf("match found at position %d\n", res);
    } else
        printf("match not found\n");
    printf("\n");
    printf("strpbrk(\"%s\", \"%s\")\n", str1, str3);
    ptr = strpbrk(str1, str3);
    if (ptr != NULL) {
        res = ptr - str1 + 1;
        printf("match found at position %d\n", res);
    } else
        printf("match not found\n");
}
@endcode
<b>Output:</b><br> <br>
strpbrk("What time is it?", "xyz") <br>
match not found <br>
strpbrk("What time is it?", "eou?") <br>
match found at position 9
 */
char *strpbrk(const char *s1, const char *s2);



/**
Description: Search for the last occurrence of a specified character in a string.

<b>Include:</b> <string.h>

@param s pointer to the string to be searched

@param c character to search for

@return Returns a pointer to the character if found; otherwise, returns a null
pointer.

<b>Remarks:</b><br> The function searches the string s, including the terminating null
character, to find the last occurrence of character c.

<b>Example:</b><br>
@code
#include <string.h> // for strrchr, NULL 
#include <stdio.h> // for printf 

int main(void) {
    char buf1[50] = "What time is it?";
    char ch1 = 'm', ch2 = 'y';
    char *ptr;
    int res;
    printf("buf1 : %s\n\n", buf1);
    ptr = strrchr(buf1, ch1);
    if (ptr != NULL) {
        res = ptr - buf1 + 1;
        printf("%c found at position %d\n", ch1, res);
    } else
        printf("%c not found\n", ch1);
    printf("\n");
    ptr = strrchr(buf1, ch2);
    if (ptr != NULL) {
        res = ptr - buf1 + 1;
        printf("%c found at position %d\n", ch2, res);
    } else
        printf("%c not found\n", ch2);
}
@endcode
<b>Output:</b><br>
buf1 : What time is it? <br>
m found at position 8 <br>
y not found
 */
char *strrchr(const char *s, int c);



/**
Description: Calculate the number of consecutive characters at the beginning of a
string that are contained in a set of characters.

<b>Include:</b> <string.h>

@param s1 pointer to the string to be searched

@param s2 pointer to characters to search for

@return Returns the number of consecutive characters from the beginning of s1
that are contained in s2.

<b>Remarks:</b><br> This function stops searching when a character from s1 is not in s2.

<b>Example:</b><br>
@code
#include <string.h> // for strspn 
#include <stdio.h> // for printf 

int main(void) {
    char str1[20] = "animal";
    char str2[20] = "aeiounm";
    char str3[20] = "aimnl";
    char str4[20] = "xyz";
    int res;
    res = strspn(str1, str2);
    printf("strspn(\"%s\", \"%s\") = %d\n",
            str1, str2, res);
    res = strspn(str1, str3);
    printf("strspn(\"%s\", \"%s\") = %d\n",
            str1, str3, res);
    res = strspn(str1, str4);
    printf("strspn(\"%s\", \"%s\") = %d\n",
            str1, str4, res);
}
@endcode
<b>Output:</b><br>
strspn("animal", "aeiounm") = 5 <br>
strspn("animal", "aimnl") = 6 <br>
strspn("animal", "xyz") = 0 <br>

Explanation: <br>
In the first result, l is not in s2. <br>
In the second result, the terminating null is not in s2. <br>
In the third result, a is not in s2 , so the comparison stops.
 */
size_t strspn(const char *s1, const char *s2);



/**
Description: Search for the first occurrence of a string inside another string.

<b>Include:</b> <string.h>

@param s1 pointer to the string to be searched

@param s2 pointer to substring to be searched for

@return Returns the address of the first element that matches the substring if
found; otherwise, returns a null pointer.

<b>Remarks:</b><br> This function will find the first occurrence of the string, s2 (excluding
the null terminator) within the string, s1. If s2 points to a zero length
string, s1 is returned.

<b>Example:</b><br>
@code
#include <string.h> // for strstr, NULL 
#include <stdio.h> // for printf 

int main(void) {
    char str1[20] = "What time is it?";
    char str2[20] = "is";
    char str3[20] = "xyz";
    char *ptr;
    int res;
    printf("str1 : %s\n", str1);
    printf("str2 : %s\n", str2);
    printf("str3 : %s\n\n", str3);
    ptr = strstr(str1, str2);
    if (ptr != NULL) {
        res = ptr - str1 + 1;
        printf("\"%s\" found at position %d\n",
                str2, res);
    } else
        printf("\"%s\" not found\n", str2);
    printf("\n");
    ptr = strstr(str1, str3);
    if (ptr != NULL) {
        res = ptr - str1 + 1;
        printf("\"%s\" found at position %d\n",
                str3, res);
    } else
        printf("\"%s\" not found\n", str3);
}
@endcode
<b>Output:</b><br>
str1 : What time is it? <br>
str2 : is <br>
str3 : xyz <br>
"is" found at position 11 <br>
"xyz" not found
 */
char *strstr(const char *s1, const char *s2);



/**
Description: Break a string into substrings, or tokens, by inserting null characters in
place of specified delimiters.

<b>Include:</b> <string.h>

@param s1 pointer to the null terminated string to be searched

@param s2 pointer to characters to be searched for (used as delimiters)

@return Returns a pointer to the first character of a token (the first character in
s1 that does not appear in the set of characters of s2). If no token is
found, the null pointer is returned.

<b>Remarks:</b><br> A sequence of calls to this function can be used to split up a string into
substrings (or tokens) by replacing specified characters with null characters.

The first time this function is invoked on a particular string, that
string should be passed in s1. After the first time, this function can continue
parsing the string from the last delimiter by invoking it with a null
value passed in s1.

It skips all leading characters that appear in the string, s2 (delimiters),
then skips all characters not appearing in s2 (this segment of characters
is the token), and then overwrites the next character with a null
character, terminating the current token. The function, strtok, then
saves a pointer to the character that follows, from which the next
search will start.

If strtok finds the end of the string before it finds a
delimiter, the current token extends to the end of the string pointed to
by s1.

If this is the first call to strtok, it does not modify the string (no
null characters are written to s1). The set of characters that is passed
in s2 need not be the same for each call to strtok.

If strtok is called with a non-null parameter for s1 after the initial call,
the string becomes the new string to search. The old string previously
searched will be lost.

<b>Example:</b><br>
@code
#include <string.h> // for strtok, NULL 
#include <stdio.h> / * for printf 

int main(void) {
    char str1[30] = "Here, on top of the world!";
    char delim[5] = ", .";
    char *word;
    int x;
    printf("str1 : %s\n", str1);
    x = 1;
    word = strtok(str1, delim);
    while (word != NULL) {
        printf("word %d: %s\n", x++, word);
        word = strtok(NULL, delim);
    }
}
@endcode
<b>Output:</b><br>
str1 : Here, on top of the world! <br>
word 1: Here <br>
word 2: on <br>
word 3: top <br>
word 4: of <br>
word 5: the <br>
word 6: world!
 */
char *strtok(char *s1, const char *s2);



/**
Description: Transforms a string using the locale-dependent rules. (See Remarks.)

<b>Include:</b> <string.h>

@param s1 destination string

@param s2 source string to be transformed

@param n number of characters to transform

@return Returns the length of the transformed string not including the terminating
null character. If n is zero, the string is not transformed (s1 may be
a point null in this case) and the length of s2 is returned.

<b>Remarks:</b><br> If the return value is greater than or equal to n, the content of s1 is
indeterminate. Since the 16-bit compiler does not support alternate
locales, the transformation is equivalent to strcpy, except that the
length of the destination string is bounded by n-1.
 */
size_t strxfrm(char *s1, const char *s2, size_t n);