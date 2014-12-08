/**
 * @brief           Standard C Libraries - 
 * @file            stdio.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section stdio_desc Description
 **********************************************************************
The header file, stdio.h, consists of types, macros and functions that provide support
to perform input and output operations on files and streams. When a file is opened it is
associated with a stream. A stream is a pipeline for the flow of data into and out of files.
Because different systems use different properties, the stream provides more uniform
properties to allow reading and writing of the files.

Streams can be text streams or binary streams. Text streams consist of a sequence of
characters divided into lines. Each line is terminated with a newline ('\\n') character. The
characters may be altered in their internal representation, particularly in regards to line
endings. Binary streams consist of sequences of bytes of information. The bytes transmitted
to the binary stream are not altered. There is no concept of lines - the file is just
a series of bytes.

At start-up three streams are automatically opened: stdin, stdout, and stderr. stdin
provides a stream for standard input, stdout is standard output and stderr is the standard
error. Additional streams may be created with the fopen function. See fopen for
the different types of file access that are permitted. These access types are used by
fopen and freopen.

The type FILE is used to store information about each opened file stream. It includes
such things as error indicators, end-of-file indicators, file position indicators, and other
internal status information needed to control a stream. Many functions in the stdio use
FILE as an argument.

There are three types of buffering: unbuffered, line buffered and fully buffered. Unbuffered
means a character or byte is transferred one at a time. Line buffered collects and
transfers an entire line at a time (i.e., the newline character indicates the end of a line).
Fully buffered allows blocks of an arbitrary size to be transmitted. The functions,
setbuf and setvbuf, control file buffering.

The stdio.h file also contains functions that use input and output formats. The input
formats, or scan formats, are used for reading data. Their descriptions can be found
under scanf, but they are also used by fscanf and sscanf. The output formats, or print
formats, are used for writing data. Their descriptions can be found under printf.
These print formats are also used by fprintf, sprintf, vfprintf, vprintf and
vsprintf.

@section stdio_compiler_options Compiler Options
Certain compiler options may affect how standard I/O performs. In an effort to provide
a more tailored version of the formatted I/O routines, the tool chain may convert a call
to a printf or scanf style function to a different call. The options are summarized
below:
- The -msmart-io option, when enabled, will attempt to convert printf, scanf
and other functions that use the input output formats to an integer only variant.
The functionality is the same as that of the C standard forms, minus the support
for floating-point output. -msmart-io=0 disables this feature and no conversion
will take place. -msmart-io=1 or -msmart-io (the default) will convert a function
call if it can be proven that an I/O function will never be presented with a
floating-point conversion. -msmart-io=2 is more optimistic than the default and
will assume that non-constant format strings or otherwise unknown format
strings will not contain a floating-point format. In the event that -msmart-io=2 is
used with a floating-point format, the format letter will appear as literal text and
its corresponding argument will not be consumed.
- The -fno-short-double option will cause the compiler to generate calls to formatted I/O
routines that support double as if it were a long double type.

Mixing modules compiled with these options may result in a larger executable size, or
incorrect execution if large and small double-sized data is shared across modules.

@section stdio_customizing Customizing STDIO
The standard I/O relies on helper functions described in Chapter 4. "Standard C
Libraries - Support Functions". These functions include read(), write(), open(),
and close() which are called to read, write, open or close handles that are associated
with standard I/O FILE pointers. The sources for these libraries are provided for you to
customize as you wish.

The simplest way to redirect standard I/O to the peripheral of your choice is to select
one of the default handles already in use. Also, you could open files with a specific
name, via fopen(), by rewriting open() to return a new handle to be recognized by
read() or write(), as appropriate.

If only a specific peripheral is required, then you could associate handle 1 == stdout,
or 2 == stderr, to another peripheral by writing the correct code to talk to the
interested peripheral.
A complete generic solution might be:
@code
//should be in a header file

enum my_handles {
    handle_stdin,
    handle_stdout,
    handle_stderr,
    handle_can1,
    handle_can2,
    handle_spi1,
    handle_spi2,
};

int __attribute__((__weak__, __section__(".libc"))) open(const char
        *name, int access, int mode) {
    switch (name[0]) {
        case 'i': return handle_stdin;
        case 'o': return handle_stdout;
        case 'e': return handle_stderr;
        case 'c': return handle_can1;
        case 'C': return handle_can2;
        case 's': return handle_spi1;
        case 'S': return handle_spi2;
        default: return handle_stderr;
    }
}

@endcode

Single letters were used in this example because they are faster to check and use less
memory. However, if memory is not an issue, you could use strcmp to compare full
names.
In write(), you would write:
@code
write(int handle, void *buffer, unsigned int len) {
    int i;
    volatile UxMODEBITS *umode = &U1MODEbits;
    volatile UxSTABITS *ustatus = &U1STAbits;
    volatile unsigned int *txreg = &U1TXREG;
    volatile unsigned int *brg = &U1BRG;

    switch (handle)
        default:
        case 0:
        case 1:
        case 2:
        if ((__C30_UART != 1) && (&U2BRG)) {
            umode = &U2MODEbits;
            ustatus = &U2STAbits;
            txreg = &U2TXREG;
            brg = &U2BRG;
        }
    if ((umode->UARTEN) == 0) {
        *brg = 0;
        umode->UARTEN = 1;
    }
    if ((ustatus->UTXEN) == 0) {
        ustatus->UTXEN = 1;
    }
    for (i = len; i; --i) {
        while ((ustatus->TRMT) == 0);
        *txreg = *(char*) buffer++;
    }
    break;
    case handle_can1: //code to support can1 
    break;
    case handle_can2: // code to support can2 
    break;
    case handle_spi1: // code to support spi1 
    break;
    case handle_spi2: // code to support spi2 
    break;
}
return (len);
}
@endcode

where you would fill in the appropriate code as specified in the comments.
Now you can use the generic C STDIO features to write to another port:

@code
FILE *can1 = fopen("c", "w");
fprintf(can1, "This will be output through the can\n");
@endcode


 
 **********************************************************************
 * @section stdio_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original license
 * agreement included with the XC16 compiler applies!
 */






/**
Description: Stores information for a file stream.

<b>Include:</b> <stdio.h>
 */
#define FILE FILE



/**
Description: Type of a variable used to store a file position.

<b>Include:</b> <stdio.h>
 */
#define fpos_t fpos_t



/**
Description: The result type of the sizeof operator.

<b>Include:</b> <stdio.h>
 */
#define size_t size_t



/**
Description: Indicates full buffering.

<b>Include:</b> <stdio.h>

<b>Remarks:</b><br> Used by the function, setvbuf.
 */
#define _IOFBF _IOFBF



/**
Description: Indicates line buffering.

<b>Include:</b> <stdio.h>

<b>Remarks:</b><br> Used by the function, setvbuf.
 */
#define _IOLBF _IOLBF



/**
Description: Indicates no buffering.

<b>Include:</b> <stdio.h>

<b>Remarks:</b><br> Used by the function, setvbuf.
 */
#define _IONBF



/**
Description: Defines the size of the buffer used by the function, setbuf.

<b>Include:</b> <stdio.h>

<b>Value:</b>  512
 */
#define BUFSIZ



/**
Description: A negative number indicating the end-of-file has been reached or to
report an error condition.

<b>Include:</b> <stdio.h>

<b>Remarks:</b><br> If an end-of-file is encountered, the end-of-file indicator is set. If an
error condition is encountered, the error indicator is set. Error
conditions include write errors and input or read errors.
 */
#define EOF



/**
Description: Maximum number of characters in a filename including the null
terminator.

<b>Include:</b> <stdio.h>

<b>Value:</b>  260
 */
#define FILENAME_MAX



/**
Description: Defines the maximum number of files that can be simultaneously open.

<b>Include:</b> <stdio.h>

<b>Value:</b>  8

<b>Remarks:</b><br> stderr, stdin and stdout are included in the FOPEN_MAX count.
 */
#define FOPEN_MAX



/**
Description: Defines the number of characters for the longest temporary filename
created by the function, tmpnam.

<b>Include:</b> <stdio.h>

<b>Value:</b>  16

<b>Remarks:</b><br> L_tmpnam is used to define the size of the array used by tmpnam.
 */
#define L_tmpnam



/**
Description: The value of a null pointer constant.

<b>Include:</b> <stdio.h>
 */
#define	NULL	(0)



/**
Description: Indicates that fseek should seek from the current position of the file
pointer.

<b>Include:</b> <stdio.h>

<b>Example:</b><br>
See example for fseek.
 */
#define SEEK_CUR



/**
Description: Indicates that fseek should seek from the end of the file.

<b>Include:</b> <stdio.h>

<b>Example:</b><br>
See example for fseek.
 */
#define SEEK_END



/**
Description: Indicates that fseek should seek from the beginning of the file.

<b>Include:</b> <stdio.h>

<b>Example:</b><br>
See example for fseek.
 */
#define SEEK_SET



/**
Description: File pointer to the standard error stream.

<b>Include:</b> <stdio.h>
 */
#define stderr stderr



/**
Description: File pointer to the standard input stream.

<b>Include:</b> <stdio.h>
 */
#define stdin stdin



/**
Description: File pointer to the standard output stream.

<b>Include:</b> <stdio.h>
 */
#define stdout stdout



/**
Description: The maximum number of unique filenames the function tmpnam can
generate.

<b>Include:</b> <stdio.h>

<b>Value:</b>  32
 */
#define TMP_MAX



/**
Description: Resets the error indictor for the stream.

<b>Include:</b> <stdio.h>

@param stream stream to reset error indicators


<b>Remarks:</b><br> The function clears the end-of-file and error indicators for the given
stream (i.e., feof and ferror will return false after the function
clearerr is called).

<b>Example:</b><br>
@code
// This program tries to write to a file that is 
// readonly. This causes the error indicator to 
// be set. The function ferror is used to check 
// the error indicator. The function clearerr is 
// used to reset the error indicator so the next 
// time ferror is called it will not report an 
// error. 
#include <stdio.h> // for ferror, clearerr, 
// printf, fprintf, fopen,
// fclose, FILE, NULL 

int main(void) {
    FILE *myfile;
    if ((myfile = fopen("sampclearerr.c", "r")) ==
            NULL)
        printf("Cannot open file\n");
    else {
        fprintf(myfile, "Write this line to the "
                "file.\n");
        if (ferror(myfile))
            printf("Error\n");
        else
            printf("No error\n");
        clearerr(myfile);
        if (ferror(myfile))
            printf("Still has Error\n");
        else
            printf("Error indicator reset\n");
        fclose(myfile);
    }
}
@endcode
<b>Output:</b><br>
Error <br>
Error indicator reset
 */
void clearerr(FILE *stream);



/**
Description: Close a stream.

<b>Include:</b> <stdio.h>

@param stream pointer to the stream to close

@return Returns 0 if successful; otherwise, returns EOF if any errors were
detected.

<b>Remarks:</b><br> fclose writes any buffered output to the file.

<b>Example:</b><br>
@code
#include <stdio.h> // for fopen, fclose, 
// printf,FILE, NULL, EOF 

int main(void) {
    FILE *myfile1, *myfile2;
    int y;
    if ((myfile1 = fopen("afile1", "w+")) == NULL)
        printf("Cannot open afile1\n");
    else {
        printf("afile1 was opened\n");
        y = fclose(myfile1);
        if (y == EOF)
            printf("afile1 was not closed\n");
        else
            printf("afile1 was closed\n");
    }
}
@endcode
<b>Output:</b><br>
afile1 was opened <br>
afile1 was closed
 */
int fclose(FILE *stream);



/**
Description: Tests for end-of-file.

<b>Include:</b> <stdio.h>

@param stream stream to check for end-of-file

@return Returns non-zero if stream is at the end-of-file; otherwise, returns zero.

<b>Example:</b><br>
@code
#include <stdio.h> // for feof, fgetc, fputc, 
// fopen, fclose, FILE, 
// NULL 

int main(void) {
    FILE *myfile;
    int y = 0;
    if ((myfile = fopen("afile.txt", "rb")) == NULL)
        printf("Cannot open file\n");
    else {
        for (;;) {
            y = fgetc(myfile);
            if (feof(myfile))
                break;
            fputc(y, stdout);
        }
        fclose(myfile);
    }
}
@endcode
<b>Input:</b><br>
Contents of afile.txt (used as input):
This is a sentence.
<b>Output:</b><br>
This is a sentence.
 */
int feof(FILE *stream);



/**
Description: Tests if error indicator is set.

<b>Include:</b> <stdio.h>

@param stream pointer to FILE structure

@return Returns a non-zero value if error indicator is set; otherwise, returns a
zero.

<b>Example:</b><br>
@code
// This program tries to write to a file that is 
// readonly. This causes the error indicator to 
// be set. The function ferror is used to check 
// the error indicator and find the error. The 
// function clearerr is used to reset the error 
// indicator so the next time ferror is called 
// it will not report an error. 
#include <stdio.h> // for ferror, clearerr, 
// printf, fprintf, 
// fopen, fclose, 
// FILE, NULL 

int main(void) {
    FILE *myfile;
    if ((myfile = fopen("sampclearerr.c", "r")) ==
            NULL)
        printf("Cannot open file\n");
    else {
        fprintf(myfile, "Write this line to the "
                "file.\n");
        if (ferror(myfile))
            printf("Error\n");
        else
            printf("No error\n");
        clearerr(myfile);
        if (ferror(myfile))
            printf("Still has Error\n");
        else
            printf("Error indicator reset\n");
        fclose(myfile);
    }
}
@endcode
<b>Output:</b><br>
Error <br>
Error indicator reset
 */
int ferror(FILE *stream);



/**
Description: Flushes the buffer in the specified stream.

<b>Include:</b> <stdio.h>

@param stream pointer to the stream to flush.

@return Returns EOF if a write error occurs; otherwise, returns zero for success.

<b>Remarks:</b><br> If stream is a null pointer, all output buffers are written to files. fflush
has no effect on an unbuffered stream.
 */
int fflush(FILE *stream);



/**
Description: Get a character from a stream

<b>Include:</b> <stdio.h>

@param stream pointer to the open stream

@return Returns the character read or EOF if a read error occurs or end-of-file
is reached.

<b>Remarks:</b><br> The function reads the next character from the input stream, advances
the file-position indicator and returns the character as an unsigned
char converted to an int.

<b>Example:</b><br>
@code
#include <stdio.h> // for fgetc, printf, 
// fclose, FILE, 
// NULL, EOF 

int main(void) {
    FILE *buf;
    char y;
    if ((buf = fopen("afile.txt", "r")) == NULL)
        printf("Cannot open afile.txt\n");
    else {
        y = fgetc(buf);
        while (y != EOF) {
            printf("%c|", y);
            y = fgetc(buf);
        }
        fclose(buf);
    }
}
@endcode
<b>Input:</b><br>
Contents of afile.txt (used as input):
Short
Longer string
<b>Output:</b><br>
S|h|o|r|t| <br>
|L|o|n|g|e|r| |s|t|r|i|n|g|
 */
int fgetc(FILE *stream);



/**
Description: Gets the stream's file position.

<b>Include:</b> <stdio.h>

@param stream target stream

@param pos position-indicator storage

@return Returns 0 if successful; otherwise, returns a non-zero value.

<b>Remarks:</b><br> The function stores the file-position indicator for the given stream in
 *pos if successful, otherwise, fgetpos sets errno.

<b>Example:</b><br>
@code
// This program opens a file and reads bytes at 
// several different locations. The fgetpos 
// function notes the 8th byte. 21 bytes are 
// read then 18 bytes are read. Next the 
// fsetpos function is set based on the 
// fgetpos position and the previous 21 bytes 
// are reread. 
#include <stdio.h> // for fgetpos, fread, 
// printf, fopen, fclose, 
// FILE, NULL, perror, 
// fpos_t, sizeof 

int main(void) {
    FILE *myfile;
    fpos_t pos;
    char buf[25];
    if ((myfile = fopen("sampfgetpos.c", "rb")) ==
            NULL)
        printf("Cannot open file\n");
    else {
        fread(buf, sizeof (char), 8, myfile);
        if (fgetpos(myfile, &pos) != 0)
            perror("fgetpos error");
        else {
            fread(buf, sizeof (char), 21, myfile);
            printf("Bytes read: %.21s\n", buf);
            fread(buf, sizeof (char), 18, myfile);
            printf("Bytes read: %.18s\n", buf);
        }
        if (fsetpos(myfile, &pos) != 0)
            perror("fsetpos error");
        fread(buf, sizeof (char), 21, myfile);
        printf("Bytes read: %.21s\n", buf);
        fclose(myfile);
    }
}
@endcode
<b>Output:</b><br>
Bytes read: program opens a file <br>
Bytes read: and reads bytes at <br>
Bytes read: program opens a file
 */
int fgetpos(FILE *stream, fpos_t *pos);



/**
Description: Get a string from a stream.

<b>Include:</b> <stdio.h>

@param s pointer to the storage string

@param n maximum number of characters to read

@param stream pointer to the open stream.

@return Returns a pointer to the string s if successful; otherwise, returns a null
pointer.

<b>Remarks:</b><br> The function reads characters from the input stream and stores them
into the string pointed to by s until it has read n-1 characters, stores a
newline character or sets the end-of-file or error indicators. If any characters
were stored, a null character is stored immediately after the last
read character in the next element of the array. If fgets sets the error
indicator, the array contents are indeterminate.

<b>Example:</b><br>
@code
#include <stdio.h> // for fgets, printf, 
// fopen, fclose, 
// FILE, NULL 
#define MAX 50

int main(void) {
    FILE *buf;
    char s[MAX];
    if ((buf = fopen("afile.txt", "r")) == NULL)
        printf("Cannot open afile.txt\n");
    else {
        while (fgets(s, MAX, buf) != NULL) {
            printf("%s|", s);
        }
        fclose(buf);
    }
}
@endcode
<b>Input:</b><br>
Contents of afile.txt (used as input):
Short
Longer string
<b>Output:</b><br>
Short <br>
|Longer string
 */
char *fgets(char *s, int n, FILE *stream);




/**
Description: Opens a file.

<b>Include:</b> <stdio.h>

@param filename name of the file

@param mode type of access permitted

@return Returns a pointer to the open stream. If the function fails a null pointer
is returned.

<b>Remarks:</b><br> Following are the types of file access:
- <i>r</i> - opens an existing text file for reading
- <i>w</i> - opens an empty text file for writing. (An existing file will be overwritten.)
- <i>a</i> - opens a text file for appending. (A file is created if it doesn't exist.)
- <i>rb</i> - opens an existing binary file for reading.
- <i>wb</i> - opens an empty binary file for writing. (An existing file will be overwritten.)
- <i>ab</i> - opens a binary file for appending. (A file is created if it doesn't exist.)
- <i>r+</i> - opens an existing text file for reading and writing.
- <i>w+</i> - opens an empty text file for reading and writing. (An existing file will be overwritten.)
- <i>a+</i> - opens a text file for reading and appending. (A file is created if it doesn't exist.)
- <i>r+b or rb+</i> - opens an existing binary file for reading and writing.
- <i>w+b or wb+</i> - opens an empty binary file for reading and writing. (An existing file will be overwritten.)
- <i>a+b</i> or ab+ - opens a binary file for reading and appending. (A file is created if it doesn't exist.)

<b>Example:</b><br>
@code
#include <stdio.h> // for fopen, fclose, 
// printf, FILE, 
// NULL, EOF 

int main(void) {
    FILE *myfile1, *myfile2;
    int y;
    if ((myfile1 = fopen("afile1", "r")) == NULL)
        printf("Cannot open afile1\n");
    else {
        printf("afile1 was opened\n");
        y = fclose(myfile1);
        if (y == EOF)
            printf("afile1 was not closed\n");
        else
            printf("afile1 was closed\n");
    }
    if ((myfile1 = fopen("afile1", "w+")) == NULL)
        printf("Second try, cannot open afile1\n");
    else {
        printf("Second try, afile1 was opened\n");
        y = fclose(myfile1);
        if (y == EOF)
            printf("afile1 was not closed\n");
        else
            printf("afile1 was closed\n");
    }
    if ((myfile2 = fopen("afile2", "w+")) == NULL)
        printf("Cannot open afile2\n");
    else {
        printf("afile2 was opened\n");
        y = fclose(myfile2);
        if (y == EOF)
            printf("afile2 was not closed\n");
        else
            printf("afile2 was closed\n");
    }
}
@endcode
<b>Output:</b><br>
Cannot open afile1 <br>
Second try, afile1 was opened <br>
afile1 was closed <br>
afile2 was opened <br>
afile2 was closed <br>

Explanation: <br>
afile1 must exist before it can be opened for reading (r) or the
fopen function will fail. If the fopen function opens a file for writing
(w+) it does not have to already exist. If it doesn't exist, it will be created
and then opened.
 */
FILE *fopen(const char *filename, const char *mode);



/**
Description: Prints formatted data to a stream.

<b>Include:</b> <stdio.h>

@param stream pointer to the stream in which to output data

@param format format control string

@param ... optional arguments

@return Returns number of characters generated or a negative number if an
error occurs.

<b>Remarks:</b><br> The format argument has the same syntax and use that it has in
print.

<b>Example:</b><br>
@code
#include <stdio.h> // for fopen, fclose, 
// fprintf, printf, 
// FILE, NULL 

int main(void) {
    FILE *myfile;
    int y;
    char s[] = "Print this string";
    int x = 1;
    char a = '\n';
    if ((myfile = fopen("afile", "w")) == NULL)
        printf("Cannot open afile\n");
    else {
        y = fprintf(myfile, "%s %d time%c", s, x, a);
        printf("Number of characters printed "
                "to file = %d", y);
        fclose(myfile);
    }
}
@endcode
<b>Output:</b><br>
Number of characters printed to file = 25 <br>
Contents of afile: <br>
Print this string 1 time
 */
int fprintf(FILE *stream, const char *format, ...);



/**
Description: Puts a character to the stream.

<b>Include:</b> <stdio.h>

@param c character to be written

@param stream pointer to the open stream

@return Returns the character written or EOF if a write error occurs.

<b>Remarks:</b><br> The function writes the character to the output stream, advances the
file-position indicator and returns the character as an unsigned char
converted to an int.

<b>Example:</b><br>
@code
#include <stdio.h> // for fputc, EOF, stdout 

int main(void) {
    char *y;
    char buf[] = "This is text\n";
    int x;
    x = 0;
    for (y = buf; (x != EOF) && (*y != '\0'); y++) {
        x = fputc(*y, stdout);
        fputc('|', stdout);
    }
}
@endcode
<b>Output:</b><br>
T|h|i|s| |i|s| |t|e|x|t|
 */
int fputc(int c, FILE *stream);



/**
Description: Puts a string to the stream.

<b>Include:</b> <stdio.h>

@param s string to be written

@param stream pointer to the open stream

@return Returns a non-negative value if successful; otherwise, returns EOF.

<b>Remarks:</b><br> The function writes characters to the output stream up to but not
including the null character.

<b>Example:</b><br>
@code
#include <stdio.h> // for fputs, stdout 

int main(void) {
    char buf[] = "This is text\n";
    fputs(buf, stdout);
    fputs("|", stdout);
}
@endcode
<b>Output:</b><br>
This is text
 */
int fputs(const char *s, FILE *stream);



/**
Description: Reads data from the stream.

<b>Include:</b> <stdio.h>

@param ptr pointer to the storage buffer

@param size size of item

@param nelem maximum number of items to be read

@param stream pointer to the stream

@return Returns the number of complete elements read up to nelem whose
size is specified by size.

<b>Remarks:</b><br> The function reads characters from a given stream into the buffer
pointed to by ptr until the function stores size * nelem characters
or sets the end-of-file or error indicator. fread returns n/size where n is
the number of characters it read. If n is not a multiple of size, the value
of the last element is indeterminate. If the function sets the error
indicator, the file-position indicator is indeterminate.

<b>Example:</b><br>
@code
#include <stdio.h> // for fread, fwrite, 
// printf, fopen, fclose, 
// sizeof, FILE, NULL 

int main(void) {
    FILE *buf;
    int x, numwrote, numread;
    double nums[10], readnums[10];
    if ((buf = fopen("afile.out", "w+")) != NULL) {
        for (x = 0; x < 10; x++) {
            nums[x] = 10.0 / (x + 1);
            printf("10.0/%d = %f\n", x + 1, nums[x]);
        }
        numwrote = fwrite(nums, sizeof (double),
                10, buf);
        printf("Wrote %d numbers\n\n", numwrote);
        fclose(buf);
    } else
        printf("Cannot open afile.out\n");
    if ((buf = fopen("afile.out", "r+")) != NULL) {
        numread = fread(readnums, sizeof (double),
                10, buf);
        printf("Read %d numbers\n", numread);
        for (x = 0; x < 10; x++) {
            printf("%d * %f = %f\n", x + 1, readnums[x],
                    (x + 1) * readnums[x]);
        }
        fclose(buf);
    } else
        printf("Cannot open afile.out\n");
}
@endcode
<b>Output:</b><br>
10.0/1 = 10.000000 <br>
10.0/2 = 5.000000 <br>
10.0/3 = 3.333333 <br>
10.0/4 = 2.500000 <br>
10.0/5 = 2.000000 <br>
10.0/6 = 1.666667 <br>
10.0/7 = 1.428571 <br>
10.0/8 = 1.250000 <br>
10.0/9 = 1.111111 <br>
10.0/10 = 1.000000 <br>
Wrote 10 numbers <br>
Read 10 numbers <br>
1 * 10.000000 = 10.000000 <br>
2 * 5.000000 = 10.000000 <br>
3 * 3.333333 = 10.000000 <br>
4 * 2.500000 = 10.000000 <br>
5 * 2.000000 = 10.000000 <br>
6 * 1.666667 = 10.000000 <br>
7 * 1.428571 = 10.000000 <br>
8 * 1.250000 = 10.000000 <br>
9 * 1.111111 = 10.000000 <br>
10 * 1.000000 = 10.000000 <br>

Explanation: <br>
This program uses fwrite to save 10 numbers to a file in binary form.
This allows the numbers to be saved in the same pattern of bits as the
program is using which provides more accuracy and consistency. Using
fprintf would save the numbers as text strings which could cause
the numbers to be truncated. Each number is divided into 10 to produce
a variety of numbers. Retrieving the numbers with fread to a
new array and multiplying them by the original number shows the
numbers were not truncated in the save process.
 */
size_t fread(void *ptr, size_t size, size_t nelem, FILE *stream);



/**
Description: Reassigns an existing stream to a new file.

<b>Include:</b> <stdio.h>

@param filename name of the new file

@param mode type of access permitted

@param stream pointer to the currently open stream

@return Returns a pointer to the new open file. If the function fails a null pointer
is returned.

<b>Remarks:</b><br> The function closes the file associated with the stream as though
fclose was called. Then it opens the new file as though fopen was
called. freopen will fail if the specified stream is not open. See fopen
for the possible types of file access.

<b>Example:</b><br>
@code
#include <stdio.h> // for fopen, freopen, 
// printf, fclose, 
// FILE, NULL 

int main(void) {
    FILE *myfile1, *myfile2;
    int y;
    if ((myfile1 = fopen("afile1", "w+")) == NULL)
        printf("Cannot open afile1\n");
    else {
        printf("afile1 was opened\n");
        if ((myfile2 = freopen("afile2", "w+",
                myfile1)) == NULL) {
            printf("Cannot open afile2\n");
            fclose(myfile1);
        } else {
            printf("afile2 was opened\n");
            fclose(myfile2);
        }
    }
}
@endcode
<b>Output:</b><br>
afile1 was opened <br>
afile2 was opened <br>

Explanation: <br>
This program uses myfile2 to point to the stream when freopen is
called so if an error occurs, myfile1 will still point to the stream and
can be closed properly. If the freopen call is successful, myfile2 can
be used to close the stream properly.
 */
FILE *freopen(const char *filename, const char *mode, FILE *stream);



/**
Description: Scans formatted text from a stream.

<b>Include:</b> <stdio.h>

@param stream pointer to the open stream from which to read data

@param format format control string

@param ... optional arguments

@return Returns the number of items successfully converted and assigned. If
no items are assigned, a 0 is returned. EOF is returned if end-of-file is
encountered before the first conversion or if an error occurs.

<b>Remarks:</b><br> The format argument has the same syntax and use that it has in
scanf.

<b>Example:</b><br>
@code
#include <stdio.h> // for fopen, fscanf, 
// fclose, fprintf, 
// fseek, printf, FILE, 
// NULL, SEEK_SET 

int main(void) {
    FILE *myfile;
    char s[30];
    int x;
    char a;
    if ((myfile = fopen("afile", "w+")) == NULL)
        printf("Cannot open afile\n");
    else {
        fprintf(myfile, "%s %d times%c",
                "Print this string", 100, '\n');
        fseek(myfile, 0L, SEEK_SET);
        fscanf(myfile, "%s", s);
        printf("%s\n", s);
        fscanf(myfile, "%s", s);
        printf("%s\n", s);
        fscanf(myfile, "%s", s);
        printf("%s\n", s);
        fscanf(myfile, "%d", &x);
        printf("%d\n", x);
        fscanf(myfile, "%s", s);
        printf("%s\n", s);
        fscanf(myfile, "%c", a);
        printf("%c\n", a);
        fclose(myfile);
    }
}
@endcode
<b>Input:</b><br>
Contents of afile:
Print this string 100 times
<b>Output:</b><br>
Print <br>
this <br>
string <br>
100 <br>
times
 */
int fscanf(FILE *stream, const char *format, ...);



/**
Description: Moves file pointer to a specific location.

<b>Include:</b> <stdio.h>

@param stream stream in which to move the file pointer.

@param offset value to add to the current position

@param mode type of seek to perform

@return Returns 0 if successful; otherwise, returns a non-zero value and set
errno.

<b>Remarks:</b><br> mode can be one of the following: <br>
SEEK_SET - seeks from the beginning of the file <br>
SEEK_CUR - seeks from the current position of the file pointer <br>
SEEK_END - seeks from the end of the file

<b>Example:</b><br>
@code
#include <stdio.h> // for fseek, fgets, 
// printf, fopen, fclose, 
// FILE, NULL, perror, 
// SEEK_SET, SEEK_CUR, 
// SEEK_END 

int main(void) {
    FILE *myfile;
    char s[70];
    int y;
    myfile = fopen("afile.out", "w+");
    if (myfile == NULL)
        printf("Cannot open afile.out\n");
    else {
        fprintf(myfile, "This is the beginning, "
                "this is the middle and "
                "this is the end.");
        y = fseek(myfile, 0L, SEEK_SET);
        if (y)
            perror("Fseek failed");
        else {
            fgets(s, 22, myfile);
            printf("\"%s\"\n\n", s);
        }
        y = fseek(myfile, 2L, SEEK_CUR);
        if (y)
            perror("Fseek failed");
        else {
            fgets(s, 70, myfile);
            printf("\"%s\"\n\n", s);
        }
        y = fseek(myfile, -16L, SEEK_END);
        if (y)
            perror("Fseek failed");
        else {
            fgets(s, 70, myfile);
            printf("\"%s\"\n", s);
        }
        fclose(myfile);
    }
}
@endcode
<b>Output:</b><br>
"This is the beginning" <br>
"this is the middle and this is the end." <br>
"this is the end." <br>

Explanation: <br>
The file, afile.out, is created with the text, "This is the beginning,
this is the middle and this is the end".
The function, fseek, uses an offset of zero and SEEK_SET to set the
file pointer to the beginning of the file. fgets then reads 22 characters
which are "This is the beginning", and adds a null character to the
string.

Next, fseek uses an offset of two and SEEK_CURRENT to set the file
pointer to the current position plus two (skipping the comma and
space). fgets then reads up to the next 70 characters. The first 39
characters are "this is the middle and this is the end". It stops when it
reads EOF and adds a null character to the string.

Finally, fseek uses an offset of negative 16 characters and SEEK_END
to set the file pointer to 16 characters from the end of the file. fgets
then reads up to 70 characters. It stops at the EOF after reading
16 characters "this is the end". and adds a null character to the string.
 */
int fseek(FILE *stream, long offset, int mode);



/**
Description: Sets the stream's file position.

<b>Include:</b> <stdio.h>

@param stream target stream

@param pos position-indicator storage as returned by an earlier call to fgetpos

@return Returns 0 if successful; otherwise, returns a non-zero value.

<b>Remarks:</b><br> The function sets the file-position indicator for the given stream in *pos
if successful; otherwise, fsetpos sets errno.

<b>Example:</b><br>
@code
// This program opens a file and reads bytes at 
// several different locations. The fgetpos 
// function notes the 8th byte. 21 bytes are 
// read then 18 bytes are read. Next the 
// fsetpos function is set based on the 
// fgetpos position and the previous 21 bytes 
// are reread. 
#include <stdio.h> // for fgetpos, fread, 
// printf, fopen, fclose, 
// FILE, NULL, perror, 
// fpos_t, sizeof 

int main(void) {
    FILE *myfile;
    fpos_t pos;
    char buf[25];
    if ((myfile = fopen("sampfgetpos.c", "rb")) ==
            NULL)
        printf("Cannot open file\n");
    else {
        fread(buf, sizeof (char), 8, myfile);
        if (fgetpos(myfile, &pos) != 0)
            perror("fgetpos error");
        else {
            fread(buf, sizeof (char), 21, myfile);
            printf("Bytes read: %.21s\n", buf);
            fread(buf, sizeof (char), 18, myfile);
            printf("Bytes read: %.18s\n", buf);
        }
        if (fsetpos(myfile, &pos) != 0)
            perror("fsetpos error");
        fread(buf, sizeof (char), 21, myfile);
        printf("Bytes read: %.21s\n", buf);
        fclose(myfile);
    }
}
@endcode
<b>Output:</b><br>
Bytes read: program opens a file <br>
Bytes read: and reads bytes at <br>
Bytes read: program opens a file
 */
int fsetpos(FILE *stream, const fpos_t *pos);



/**
Description: Gets the current position of a file pointer.

<b>Include:</b> <stdio.h>

@param stream stream in which to get the current file position

@return Returns the position of the file pointer if successful; otherwise, returns
-1.

<b>Example:</b><br>
@code
#include <stdio.h> // for ftell, fread, 
// fprintf, printf, 
// fopen, fclose, sizeof, 
// FILE, NULL 

int main(void) {
    FILE *myfile;
    char s[75];
    long y;
    myfile = fopen("afile.out", "w+");
    if (myfile == NULL)
        printf("Cannot open afile.out\n");
    else {
        fprintf(myfile, "This is a very long sentence "
                "for input into the file named "
                "afile.out for testing.");
        fclose(myfile);
        if ((myfile = fopen("afile.out", "rb")) != NULL) {
            printf("Read some characters:\n");
            fread(s, sizeof (char), 29, myfile);
            printf("\t\"%s\"\n", s);
            y = ftell(myfile);
            printf("The current position of the "
                    "file pointer is %ld\n", y);
            fclose(myfile);
        }
    }
}
@endcode
<b>Output:</b><br>
Read some characters: <br>
"This is a very long sentence " <br>
The current position of the file pointer is 29
 */
long ftell(FILE *stream);



/**
Description: Writes data to the stream.

<b>Include:</b> <stdio.h>

@param ptr pointer to the storage buffer

@param size size of item

@param nelem maximum number of items to be read

@param stream pointer to the open stream

@return Returns the number of complete elements successfully written, which
will be less than nelem only if a write error is encountered.

<b>Remarks:</b><br> The function writes characters to a given stream from a buffer pointed
to by ptr up to nelem elements whose size is specified by size. The
file position indicator is advanced by the number of characters successfully
written. If the function sets the error indicator, the file-position
indicator is indeterminate.

<b>Example:</b><br>
@code
#include <stdio.h> // for fread, fwrite, 
// printf, fopen, fclose, 
// sizeof, FILE, NULL 

int main(void) {
    FILE *buf;
    int x, numwrote, numread;
    double nums[10], readnums[10];
    if ((buf = fopen("afile.out", "w+")) != NULL) {
        for (x = 0; x < 10; x++) {
            nums[x] = 10.0 / (x + 1);
            printf("10.0/%d = %f\n", x + 1, nums[x]);
        }
        numwrote = fwrite(nums, sizeof (double),
                10, buf);
        printf("Wrote %d numbers\n\n", numwrote);
        fclose(buf);
    } else
        printf("Cannot open afile.out\n");
    if ((buf = fopen("afile.out", "r+")) != NULL) {
        numread = fread(readnums, sizeof (double),
                10, buf);
        printf("Read %d numbers\n", numread);
        for (x = 0; x < 10; x++) {
            printf("%d * %f = %f\n", x + 1, readnums[x],
                    (x + 1) * readnums[x]);
        }
        fclose(buf);
    } else
        printf("Cannot open afile.out\n");
}
@endcode
<b>Output:</b><br>
10.0/1 = 10.000000 <br>
10.0/2 = 5.000000 <br>
10.0/3 = 3.333333 <br>
10.0/4 = 2.500000 <br>
10.0/5 = 2.000000 <br>
10.0/6 = 1.666667 <br>
10.0/7 = 1.428571 <br>
10.0/8 = 1.250000 <br>
10.0/9 = 1.111111 <br>
10.0/10 = 1.000000 <br>
Wrote 10 numbers <br>
Read 10 numbers <br>
1 * 10.000000 = 10.000000 <br>
2 * 5.000000 = 10.000000 <br>
3 * 3.333333 = 10.000000 <br>
4 * 2.500000 = 10.000000 <br>
5 * 2.000000 = 10.000000 <br>
6 * 1.666667 = 10.000000 <br>
7 * 1.428571 = 10.000000 <br>
8 * 1.250000 = 10.000000 <br>
9 * 1.111111 = 10.000000 <br>
10 * 1.000000 = 10.000000 <br>

Explanation: <br>
This program uses fwrite to save 10 numbers to a file in binary form.
This allows the numbers to be saved in the same pattern of bits as the
program is using which provides more accuracy and consistency. Using
fprintf would save the numbers as text strings, which could cause
the numbers to be truncated. Each number is divided into 10 to produce
a variety of numbers. Retrieving the numbers with fread to a
new array and multiplying them by the original number shows the
numbers were not truncated in the save process.
 */
size_t fwrite(const void *ptr, size_t size, size_t nelem, FILE *stream);



/**
Description: Get a character from the stream.

<b>Include:</b> <stdio.h>

@param stream pointer to the open stream

@return Returns the character read or EOF if a read error occurs or end-of-file
is reached.


<b>Remarks:</b><br> getc is the same as the function fgetc.

<b>Example:</b><br>
@code
#include <stdio.h> // for getc, printf, 
// fopen, fclose, 
// FILE, NULL, EOF 

int main(void) {
    FILE *buf;
    char y;
    if ((buf = fopen("afile.txt", "r")) == NULL)
        printf("Cannot open afile.txt\n");
    else {
        y = getc(buf);
        while (y != EOF) {
            printf("%c|", y);
            y = getc(buf);
        }
        fclose(buf);
    }
}
@endcode
<b>Input:</b><br>
Contents of afile.txt (used as input):
Short
Longer string
<b>Output:</b><br>
S|h|o|r|t| <br>
|L|o|n|g|e|r| |s|t|r|i|n|g|
 */
int getc(FILE *stream);



/**
Description: Get a character from stdin.

<b>Include:</b> <stdio.h>

@return Returns the character read or EOF if a read error occurs or end-of-file
is reached.

<b>Remarks:</b><br> Same effect as fgetc with the argument stdin.

<b>Example:</b><br>
@code
#include <stdio.h> // for getchar, printf 

int main(void) {
    char y;
    y = getchar();
    printf("%c|", y);
    y = getchar();
    printf("%c|", y);
    y = getchar();
    printf("%c|", y);
    y = getchar();
    printf("%c|", y);
    y = getchar();
    printf("%c|", y);
}
@endcode
<b>Input:</b><br>
Contents of UartIn.txt (used as stdin input for simulator):
Short
Longer string
<b>Output:</b><br>
S|h|o|r|t|
 */
int getchar(void);



/**
Description: Get a string from stdin.

<b>Include:</b> <stdio.h>

@param s pointer to the storage string

@return Returns a pointer to the string s if successful; otherwise, returns a null
pointer.

<b>Remarks:</b><br> The function reads characters from the stream stdin and stores them
into the string pointed to by s until it reads a newline character (which is
not stored) or sets the end-of-file or error indicators. If any characters
were read, a null character is stored immediately after the last read
character in the next element of the array. If gets sets the error
indicator, the array contents are indeterminate.

<b>Example:</b><br>
@code
#include <stdio.h> // for gets, printf 

int main(void) {
    char y[50];
    gets(y);
    printf("Text: %s\n", y);
}
@endcode
<b>Input:</b><br>
Contents of UartIn.txt (used as stdin input for simulator):
Short
Longer string
<b>Output:</b><br>
Text: Short
 */
char *gets(char *s);



/**
Description: Prints an error message to stderr.

<b>Include:</b> <stdio.h>

@param s string to print

@return None.

<b>Remarks:</b><br> The string s is printed followed by a colon and a space. Then, an error
message based on errno is printed followed by an newline.

<b>Example:</b><br>
@code
#include <stdio.h> // for perror, fopen, 
// fclose, printf, 
// FILE, NULL 

int main(void) {
    FILE *myfile;
    if ((myfile = fopen("samp.fil", "r+")) == NULL)
        perror("Cannot open samp.fil");
    else
        printf("Success opening samp.fil\n");
    fclose(myfile);
}
@endcode
<b>Output:</b><br>
Cannot open samp.fil: file open error
 */
void perror(const char *s);



/**
Description: Prints formatted text to stdout.

<b>Include:</b> <stdio.h>

@param format format control string

@param ... optional arguments

@return Returns number of characters generated or a negative number if an
error occurs.

<b>Remarks:</b><br> There must be exactly the same number of arguments as there are format
specifiers. If the are less arguments than match the format specifiers,
the output is undefined. If there are more arguments than match
the format specifiers, the remaining arguments are discarded. Each format
specifier begins with a percent sign followed by optional fields and
a required type as shown here:

@code
%[flags][width][.precision][size]type
@endcode

<i>flags</i><br>
<table>
<tr>
<td>-</td><td>left justify the value within a given field width</td>
</tr>
<tr>
<td>0</td><td>Use 0 for the pad character instead of space (which is the default)</td>
</tr>
<tr>
<td>+</td><td>generate a plus sign for positive signed values space generate a space or signed values that have neither a plus nor a minus sign</td>
</tr>
<tr>
<td>@#</td><td>to prefix 0 on an octal conversion, to prefix 0x or 0X on a hexadecimal conversion, or to generate a decimal point and
fraction digits that are otherwise suppressed on a floating-point conversion</td>
</tr>
</table>

<i>width</i><br>
Specify the number of characters to generate
for the conversion. If the asterisk (*) is used instead of a decimal number, the next argument
(which must be of type int) will be used for the field width. If the result
is less than the field width, pad characters will be used on the left to fill
the field. If the result is greater than the field width, the field is
expanded to accommodate the value without padding.

<i>precision</i><br>
The field width can be followed with dot (.) and a decimal integer representing
the precision that specifies one of the following:
- minimum number of digits to generate on an integer conversion
- number of fraction digits to generate on an e, E, or f conversion
- maximum number of significant digits to generate on a g or G conversion
- maximum number of characters to generate from a C string on an s conversion

If the period appears without the integer the integer is assumed to be
zero. If the asterisk (*) is used instead of a decimal number, the next
argument (which must be of type int) will be used for the precision.

<i>size</i><br>
- <i>h modifier:</i> used with type d, i, o, u, x, X; converts the value to a short int or unsigned short int
- <i>h modifier:</i> used with n; specifies that the pointer points to a short int
- <i>l modifier:</i> used with type d, i, o, u, x, X; converts the value to a long int or unsigned long int
- <i>l modifier:</i> used with n; specifies that the pointer points to a long int
- <i>l modifier:</i> used with c; specifies a wide character
- <i>l modifier:</i> used with type e, E, f, F, g, G; converts the value to a double
- <i>ll modifier:</i> used with type d, i, o, u, x, X; converts the value to a long long int or unsigned long long int
- <i>ll modifier:</i> used with n; specifies that the pointer points to a long long int
- <i>L modifier:</i> used with e, E, f, g, G; converts the value to a long double

<i>type</i><br>
- <i>d, i</i> signed int
- <i>o</i> unsigned int in octal
- <i>u</i> unsigned int in decimal
- <i>x</i> unsigned int in lowercase hexadecimal
- <i>X</i> unsigned int in uppercase hexadecimal
- <i>e, E</i> double in scientific notation
- <i>f</i> double decimal notation
- <i>g, G</i> double (takes the form of e, E or f as appropriate)
- <i>c</i> char - a single character
- <i>s</i> string
- <i>p</i> value of a pointer
- <i>n</i> the associated argument shall be an integer pointer into which is placed the number of characters written so far. No characters are printed.
- <i>@%</i> A @% character is printed

<b>Example:</b><br>
@code
#include <stdio.h> // for printf 

int main(void) {
    // print a character right justified in a 3 
    // character space. 
    printf("%3c\n", 'a');
    // print an integer, left justified (as  specified by the minus sign in the
    // format string) in a 4 character space. Print a second integer that is
    // right justified in a 4 character space using the pipe (|) as a separator
    //between the integers.
    printf("%-4d|%4d\n", -4, 4);
    // print a number converted to octal in 4 digits. 
    printf("%.4o\n", 10);
    // print a number converted to hexadecimal format with a 0x prefix. 
    printf("%#x\n", 28);
    // print a float in scientific notation 
    printf("%E\n", 1.1e20);
    // print a float with 2 fraction digits 
    printf("%.2f\n", -3.346);
    // print a long float with %E, %e, or %f whichever is the shortest version 
    printf("%Lg\n", .02L);
}
@endcode
<b>Output:</b><br>
a <br>
-4 | 4 <br>
0012 <br>
0x1c <br>
1.100000E+20 <br>
-3.35 <br>
0.02
 */
int printf(const char *format, ...);



/**
Description: Puts a character to the stream.

<b>Include:</b> <stdio.h>

@param c character to be written

@param stream pointer to FILE structure

@return Returns the character or EOF if an error occurs or end-of-file is
reached.

<b>Remarks:</b><br> putc is the same as the function fputc.

<b>Example:</b><br>
@code
#include <stdio.h> // for putc, EOF, stdout 

int main(void) {
    char *y;
    char buf[] = "This is text\n";
    int x;
    x = 0;
    for (y = buf; (x != EOF) && (*y != '\0'); y++) {
        x = putc(*y, stdout);
        putc('|', stdout);
    }
}
@endcode
<b>Output:</b><br>
T|h|i|s| |i|s| |t|e|x|t|
 */
int putc(int c, FILE *stream);



/**
Description: Put a character to stdout.

<b>Include:</b> <stdio.h>

@param c character to be written

@return Returns the character or EOF if an error occurs or end-of-file is
reached.

<b>Remarks:</b><br> Same effect as fputc with stdout as an argument.

<b>Example:</b><br>
@code
#include <stdio.h> // for putchar, printf, 
// EOF, stdout 

int main(void) {
    char *y;
    char buf[] = "This is text\n";
    int x;
    x = 0;
    for (y = buf; (x != EOF) && (*y != '\0'); y++)
        x = putchar(*y);
}
@endcode
<b>Output:</b><br>
This is text
 */
int putchar(int c);



/**
Description: Put a string to stdout.

<b>Include:</b> <stdio.h>

@param s string to be written

@return Returns a non-negative value if successful; otherwise, returns EOF.

<b>Remarks:</b><br> The function writes characters to the stream stdout. A newline character
is appended. The terminating null character is not written to the
stream.

<b>Example:</b><br>
@code
#include <stdio.h> // for puts 

int main(void) {
    char buf[] = "This is text\n";
    puts(buf);
    puts("|");
}
@endcode
<b>Output:</b><br>
This is text
*/
int puts(const char *s);



/**
Description: Deletes the specified file.

<b>Include:</b> <stdio.h>

@param filename name of file to be deleted

@return Returns 0 if successful, -1 if not.

<b>Remarks:</b><br> If filename does not exist or is open, remove will fail.

<b>Example:</b><br>
@code
#include <stdio.h> // for remove, printf 

int main(void) {
    if (remove("myfile.txt") != 0)
        printf("Cannot remove file");
    else
        printf("File removed");
}
@endcode
<b>Output:</b><br>
File removed
 */
int remove(const char *filename);



/**
Description: Renames the specified file.

<b>Include:</b> <stdio.h>

@param old pointer to the old name

@param new pointer to the new name

@return Return 0 if successful, non-zero if not.

<b>Remarks:</b><br> The new name must not already exist in the current working directory,
the old name must exist in the current working directory.

<b>Example:</b><br>
@code
#include <stdio.h> // for rename, printf 

int main(void) {
    if (rename("myfile.txt", "newfile.txt") != 0)
        printf("Cannot rename file");
    else
        printf("File renamed");
}
@endcode
<b>Output:</b><br>
File renamed
 */
int rename(const char *old, const char *new);



/**
Description: Resets the file pointer to the beginning of the file.

<b>Include:</b> <stdio.h>

@param stream stream to reset the file pointer

<b>Remarks:</b><br> The function calls fseek(stream, 0L, SEEK_SET) and then clears
the error indicator for the given stream.

<b>Example:</b><br>
@code
#include <stdio.h> // for rewind, fopen, 
// fscanf, fclose, 
// fprintf, printf, 
// FILE, NULL 

int main(void) {
    FILE *myfile;
    char s[] = "cookies";
    int x = 10;
    if ((myfile = fopen("afile", "w+")) == NULL)
        printf("Cannot open afile\n");
    else {
        fprintf(myfile, "%d %s", x, s);
        printf("I have %d %s.\n", x, s);
        // set pointer to beginning of file 
        rewind(myfile);
        fscanf(myfile, "%d %s", &x, &s);
        printf("I ate %d %s.\n", x, s);
        fclose(myfile);
    }
}
@endcode
<b>Output:</b><br>
I have 10 cookies. <br>
I ate 10 cookies.
 */
void rewind(FILE *stream);

/**
Description: Scans formatted text from stdin.

<b>Include:</b> <stdio.h>

@param format format control string


@param ... optional arguments

@return Returns the number of items successfully converted and assigned. If
no items are assigned, a 0 is returned. EOF is returned if an input
failure is encountered before the first.

<b>Remarks:</b><br>
Each format specifier begins with a percent sign followed by optional
fields and a required type as shown here:
@code
%[*][width][modifier]type
@endcode

<i>*</i><br> indicates assignment suppression. This will cause the input field to be
skipped and no assignment made.

<i>width</i><br>
Specify the maximum number of input characters to match for the conversion
not including white space that can be skipped.

<i>modifier</i><br>
- <i>h modifier</i> used with type d, i, o, u, x, X; converts the value to a short int or unsigned short int.
- <i>h modifier</i> used with n; specifies that the pointer points to a short int
- <i>l modifier</i> used with type d, i, o, u, x, X; converts the value to a long int or unsigned long int
- <i>l modifier</i> used with n; specifies that the pointer points to a long int
- <i>l modifier</i> used with c; specifies a wide character
- <i>l modifier</i> used with type e, E, f, F, g, G; converts the value to a double
- <i>ll modifier</i> used with type d, i, o, u, x, X; converts the value to a long long int or unsigned long long int
- <i>ll modifier</i> used with n; specifies that the pointer points to a long long int
- <i>L modifier</i> used with e, E, f, g, G; converts the value to a long double

<i>type</i><br>
- <i>d,i</i> signed int <br>
- <i>o</i> unsigned int in octal <br>
- <i>u</i> unsigned int in decimal <br>
- <i>x</i> unsigned int in lowercase hexadecimal <br>
- <i>X</i> unsigned int in uppercase hexadecimal <br>
- <i>e,E</i> double in scientific notation <br>
- <i>f</i> double decimal notation <br>
- <i>g,G</i> double (takes the form of e, E or f as appropriate) <br>
- <i>c</i> char - a single character <br>
- <i>s</i> string <br>
- <i>p</i> value of a pointer <br>
- <i>n</i> the associated argument shall be an integer pointer into, which is placed the number of characters read so far.
No characters are scanned. <br>
<i>[...]</i> character array. Allows a search of a set of characters.
A caret (^) immediately after the left bracket ( [ ) inverts
the scanset and allows any ASCII character except
those specified between the brackets. A dash character
(-) may be used to specify a range beginning with the
character before the dash and ending the character
after the dash. A null character can not be part of the
scanset. <br>
- <i>\%</i> A \% character is scanned

<b>Example:</b><br>
@code
For MPLAB SIM simulator.
#include <stdio.h> // for scanf, printf 
#include <libpic30.h>
int main(void) {
    int number, items;
    char letter;
    char color[30], string[30];
    float salary;
    __attach_input_file("UartIn.txt");
    printf("Enter your favorite number, "
            "favorite letter, ");
    printf("favorite color desired salary "
            "and SSN:\n");
    items = scanf("%d %c %[A-Za-z] %f %s", &number,
            &letter, &color, &salary, &string);
    printf("Number of items scanned = %d\n", items);
    printf("Favorite number = %d, ", number);
    printf("Favorite letter = %c\n", letter);
    printf("Favorite color = %s, ", color);
    printf("Desired salary = $%.2f\n", salary);
    printf("Social Security Number = %s, ", string);
}
If not using the simulator, remove these lines :
#include <libpic30.h>
__attach_input_file("uart_in.txt");
@endcode
<b>Input:</b><br>
Contents of UartIn.txt (used as stdin input for simulator):
5 T Green 300000 123-45-6789

<b>Output:</b><br>
Enter your favorite number, favorite letter, <br>
favorite color, desired salary and SSN: <br>
Number of items scanned = 5 <br>
Favorite number = 5, Favorite letter = T <br>
Favorite color = Green, Desired salary = $300000.00 <br>
Social Security Number = 123-45-6789
 */
int scanf(const char *format, ...);



/**
Description: Defines how a stream is buffered.

<b>Include:</b> <stdio.h>

@param stream pointer to the open stream

@param buf user allocated buffer

<b>Remarks:</b><br> setbuf must be called after fopen but before any other function calls
that operate on the stream. If buf is a null pointer, setbuf calls the
function setvbuf(stream, 0, _IONBF, BUFSIZ) for no buffering;
otherwise setbuf calls setvbuf(stream, buf, _IOFBF,
BUFSIZ) for full buffering with a buffer of size BUFSIZ. See setvbuf.

<b>Example:</b><br>
@code
#include <stdio.h> // for setbuf, printf, 
// fopen, fclose, 
// FILE, NULL, BUFSIZ 

int main(void) {
    FILE *myfile1, *myfile2;
    char buf[BUFSIZ];
    if ((myfile1 = fopen("afile1", "w+")) != NULL) {
        setbuf(myfile1, NULL);
        printf("myfile1 has no buffering\n");
        fclose(myfile1);
    }
    if ((myfile2 = fopen("afile2", "w+")) != NULL) {
        setbuf(myfile2, buf);
        printf("myfile2 has full buffering");
        fclose(myfile2);
    }
}
@endcode
<b>Output:</b><br>
myfile1 has no buffering <br>
myfile2 has full buffering
 */
void setbuf(FILE *stream, char *buf);



/**
Description: Defines the stream to be buffered and the buffer size.

<b>Include:</b> <stdio.h>

@param stream pointer to the open stream

@param buf user allocated buffer

@param mode type of buffering

@param size size of buffer

@return Returns 0 if successful

<b>Remarks:</b><br> setvbuf must be called after fopen but before any other function
calls that operate on the stream. For mode use one of the following: <br>
_IOFBF - for full buffering <br>
_IOLBF - for line buffering <br>
_IONBF - for no buffering

<b>Example:</b><br>
@code
#include <stdio.h> // for setvbuf, fopen, 
// printf, FILE, NULL, 
// _IONBF, _IOFBF 

int main(void) {
    FILE *myfile1, *myfile2;
    char buf[256];
    if ((myfile1 = fopen("afile1", "w+")) != NULL) {
        if (setvbuf(myfile1, NULL, _IONBF, 0) == 0)
            printf("myfile1 has no buffering\n");
        else
            printf("Unable to define buffer stream "
                "and/or size\n");
    }
    fclose(myfile1);
    if ((myfile2 = fopen("afile2", "w+")) != NULL) {
        if (setvbuf(myfile2, buf, _IOFBF, sizeof (buf)) ==
                0)
            printf("myfile2 has a buffer of %d "
                "characters\n", sizeof (buf));
        else
            printf("Unable to define buffer stream "
                "and/or size\n");
    }
    fclose(myfile2);
}
@endcode
<b>Output:</b><br>
myfile1 has no buffering <br>
myfile2 has a buffer of 256 characters
 */
int setvbuf(FILE *stream, char *buf, int mode, size_t size);



/**
Description: Prints formatted text to a string.

<b>Include:</b> <stdio.h>

@param s storage string for output

@param format format control string

@param ... optional arguments

@return Returns the number of characters stored in s excluding the terminating
null character.

<b>Remarks:</b><br> The format argument has the same syntax and use that it has in
printf.

<b>Example:</b><br>
@code
#include <stdio.h> // for sprintf, printf 

int main(void) {
    char sbuf[100], s[] = "Print this string";
    int x = 1, y;
    char a = '\n';
    y = sprintf(sbuf, "%s %d time%c", s, x, a);
    printf("Number of characters printed to "
            "string buffer = %d\n", y);
    printf("String = %s\n", sbuf);
}
@endcode
<b>Output:</b><br>
Number of characters printed to string buffer = 25 <br>
String = Print this string 1 time
 */
int sprintf(char *s, const char *format, ...);



/**
Description: Scans formatted text from a string.

<b>Include:</b> <stdio.h>

@param s storage string for input

@param format format control string

@param ... optional arguments

@return Returns the number of items successfully converted and assigned. If
no items are assigned, a 0 is returned. EOF is returned if an input error
is encountered before the first conversion.

<b>Remarks:</b><br> The format argument has the same syntax and use that it has in
scanf.

<b>Example:</b><br>
@code
#include <stdio.h> // for sscanf, printf 

int main(void) {
    char s[] = "5 T green 3000000.00";
    int number, items;
    char letter;
    char color[10];
    float salary;
    items = sscanf(s, "%d %c %s %f", &number, &letter,
            &color, &salary);
    printf("Number of items scanned = %d\n", items);
    printf("Favorite number = %d\n", number);
    printf("Favorite letter = %c\n", letter);
    printf("Favorite color = %s\n", color);
    printf("Desired salary = $%.2f\n", salary);
}
@endcode
<b>Output:</b><br>
Number of items scanned = 4 <br>
Favorite number = 5 <br>
Favorite letter = T <br>
Favorite color = green <br>
Desired salary = $3000000.00
 */
int sscanf(const char *s, const char *format, ...);



/**
Description: Creates a temporary file.

<b>Include:</b> <stdio.h>

@return Returns a stream pointer if successful; otherwise, returns a NULL
pointer.

<b>Remarks:</b><br> tmpfile creates a file with a unique filename. The temporary file is
opened in w+b (binary read/write) mode. It will automatically be
removed when exit is called; otherwise the file will remain in the
directory.

<b>Example:</b><br>
@code
#include <stdio.h> // for tmpfile, printf, 
// FILE, NULL 

int main(void) {
    FILE *mytempfile;
    if ((mytempfile = tmpfile()) == NULL)
        printf("Cannot create temporary file");
    else
        printf("Temporary file was created");
}
@endcode
<b>Output:</b><br>
Temporary file was created
 */
FILE *tmpfile(void);



/**
Description: Creates a unique temporary filename.

<b>Include:</b> <stdio.h>

@param s pointer to the temporary name

@return Returns a pointer to the filename generated and stores the filename in
s. If it can not generate a filename, the NULL pointer is returned.

<b>Remarks:</b><br> The created filename will not conflict with an existing file name. Use
L_tmpnam to define the size of array the argument of tmpnam points
to.

<b>Example:</b><br>
@code
#include <stdio.h> // for tmpnam, L_tmpnam, 
// printf, NULL 

int main(void) {
    char *myfilename;
    char mybuf[L_tmpnam];
    char *myptr = (char *) &mybuf;
    if ((myfilename = tmpnam(myptr)) == NULL)
        printf("Cannot create temporary file name");
    else
        printf("Temporary file %s was created",
            myfilename);
}
@endcode
<b>Output:</b><br>
Temporary file ctm00001.tmp was created
 */
char *tmpnam(char *s);




/**
Description: Pushes character back onto stream.

<b>Include:</b> <stdio.h>

@param c character to be pushed back

@param stream pointer to the open stream

@return Returns the pushed character if successful; otherwise, returns EOF.

<b>Remarks:</b><br> The pushed back character will be returned by a subsequent read on
the stream. If more than one character is pushed back, they will be
returned in the reverse order of their pushing. A successful call to a file
positioning function (fseek, fsetpos or rewind) cancels any pushed
back characters. Only one character of push back is guaranteed.
Multiple calls to ungetc without an intervening read or file positioning
operation may cause a failure.

<b>Example:</b><br>
@code
#include <stdio.h> // for ungetc, fgetc, 
// printf, fopen, fclose, 
// FILE, NULL, EOF 

int main(void) {
    FILE *buf;
    char y, c;
    if ((buf = fopen("afile.txt", "r")) == NULL)
        printf("Cannot open afile.txt\n");
    else {
        y = fgetc(buf);
        while (y != EOF) {
            if (y == 'r') {
                c = ungetc(y, buf);
                if (c != EOF) {
                    printf("2");
                    y = fgetc(buf);
                }
            }
            printf("%c", y);
            y = fgetc(buf);
        }
        fclose(buf);
    }
}
@endcode
<b>Input:</b><br>
Contents of afile.txt (used as input):
Short
Longer string
<b>Output:</b><br>
Sho2rt <br>
Longe2r st2ring
 */
int ungetc(int c, FILE *stream);



/**
Description: Prints formatted data to a stream using a variable length argument list.

<b>Include:</b> <stdio.h>
<stdarg.h>

@param stream pointer to the open stream

@param format format control string

@param ap pointer to a list of arguments

@return Returns number of characters generated or a negative number if an
error occurs.

<b>Remarks:</b><br> The format argument has the same syntax and use that it has in
printf.
To access the variable length argument list, the ap variable must be initialized
by the macro va_start and may be reinitialized by additional
calls to va_arg. This must be done before the vfprintf function is
called. Invoke va_end after the function returns. For more details, see
stdarg.h.

<b>Example:</b><br>
@code
#include <stdio.h> // for vfprintf, fopen, 
// fclose, printf, 
// FILE, NULL 
#include <stdarg.h> // for va_start, 
// va_list, va_end 
FILE *myfile;

void errmsg(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(myfile, fmt, ap);
    va_end(ap);
}

int main(void) {
    int num = 3;
    if ((myfile = fopen("afile.txt", "w")) == NULL)
        printf("Cannot open afile.txt\n");
    else {
        errmsg("Error: The letter '%c' is not %s\n", 'a',
                "an integer value.");
        errmsg("Error: Requires %d%s%c", num,
                " or more characters.", '\n');
    }
    fclose(myfile);
}
@endcode
<b>Output:</b><br>
Contents of afile.txt <br>
Error: The letter 'a' is not an integer value. <br>
Error: Requires 3 or more characters.
 */
int vfprintf(FILE *stream, const char *format, va_list ap);



/**
Description: Prints formatted text to stdout using a variable length argument list.

<b>Include:</b> <stdio.h>
<stdarg.h>

@param format format control string

@param ap pointer to a list of arguments

@return Returns number of characters generated or a negative number if an
error occurs.

<b>Remarks:</b><br> The format argument has the same syntax and use that it has in
printf.
To access the variable length argument list, the ap variable must be initialized
by the macro va_start and may be reinitialized by additional
calls to va_arg. This must be done before the vprintf function is
called. Invoke va_end after the function returns. For more details, see
stdarg.h

<b>Example:</b><br>
@code
#include <stdio.h> // for vprintf, printf 
#include <stdarg.h> // for va_start, 
// va_list, va_end 

void errmsg(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf("Error: ");
    vprintf(fmt, ap);
    va_end(ap);
}

int main(void) {
    int num = 3;
    errmsg("The letter '%c' is not %s\n", 'a',
            "an integer value.");
    errmsg("Requires %d%s\n", num,
            " or more characters.\n");
}
@endcode
<b>Output:</b><br>
Error: The letter 'a' is not an integer value. <br>
Error: Requires 3 or more characters.
 */
int vprintf(const char *format, va_list ap);



/**
Description: Prints formatted text to a string using a variable length argument list.

<b>Include:</b> <stdio.h>
<stdarg.h>

@param s storage string for output

@param format format control string

@param ap pointer to a list of arguments

@return Returns number of characters stored in s excluding the terminating null
character.

<b>Remarks:</b><br> The format argument has the same syntax and use that it has in
printf.
To access the variable length argument list, the ap variable must be initialized
by the macro va_start and may be reinitialized by additional
calls to va_arg. This must be done before the vsprintf function is
called. Invoke va_end after the function returns. For more details, see
stdarg.h

<b>Example:</b><br>
@code
#include <stdio.h> // for vsprintf, printf 
#include <stdarg.h> // for va_start, 
// va_list, va_end 

void errmsg(const char *fmt, ...) {
    va_list ap;
    char buf[100];
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    printf("Error: %s", buf);
}

int main(void) {
    int num = 3;
    errmsg("The letter '%c' is not %s\n", 'a',
            "an integer value.");
    errmsg("Requires %d%s\n", num,
            " or more characters.\n");
}
@endcode
<b>Output:</b><br>
Error: The letter 'a' is not an integer value. <br>
Error: Requires 3 or more characters.
 */
int vsprintf(char *s, const char *format, va_list ap);