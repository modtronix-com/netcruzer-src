/**
 * @brief           Standard C Libraries - 
 * @file            time.h
 * @compiler        MPLAB XC16 compiler
 *
 * @section time_desc Description
 **********************************************************************
 The header file, time.h, consists of types, macros and functions that manipulate time.
 
 **********************************************************************
 * @section time_lic Software License Agreement
 *
 * The documentation in this header file has been copied from the documentation
 * provided with the Microchip MPLAB XC16 compiler. The original lincense
 * agreement included with the XC16 compiler applies!
 */

/**
Description: Stores processor time values.

<b>Include:</b> <time.h>
 */
typedef long clock_t


/**
Description: The type of the result of the sizeof operator.

<b>Include:</b> <time.h>
 */
#define size_t



/**
Description: Structure used to hold the time and date (calendar time).

<b>Include:</b> <time.h>

<b>Remarks:</b><br> If tm_isdst is a positive value, Daylight Savings is in effect. If it is
zero, Daylight Saving Time is not in effect. If it is a negative value, the
status of Daylight Saving Time is not known.
 */
struct tm {
    int tm_sec; //seconds after the minute ( 0 to 61 )
    //allows for up to two leap seconds
    int tm_min; //minutes after the hour ( 0 to 59 )
    int tm_hour; //hours since midnight ( 0 to 23 )
    int tm_mday; //day of month ( 1 to 31 )
    int tm_mon; //month ( 0 to 11 where January = 0 )
    int tm_year; //years since 1900
    int tm_wday; //day of week ( 0 to 6 where Sunday = 0)
    int tm_yday; //day of year ( 0 to 365 where January 1 = 0 )
    int tm_isdst; //Daylight Savings Time flag
}




/**
Description: Represents calendar time values.

<b>Include:</b> <time.h>
 */
typedef long time_t



/**
Description: Number of processor clocks per second.

<b>Include:</b> <time.h>

<b>Value:</b>  1

<b>Remarks:</b><br> The compiler returns clock ticks (instruction cycles) not actual time.
 */
#define CLOCKS_PER_SEC



/**
Description: The value of a null pointer constant.

<b>Include:</b> <time.h>
 */
NULL



/**
Description: Converts the time structure to a character string.

<b>Include:</b> <time.h>

@param tptr time/date structure

@return Returns a pointer to a character string of the following format:
DDD MMM dd hh:mm:ss YYYY
DDD is day of the week
MMM is month of the year
dd is day of the month
hh is hour
mm is minute
ss is second
YYYY is year

<b>Example:</b><br>
@code
#include <time.h> // for asctime, tm 
#include <stdio.h> // for printf 
volatile int i;

int main(void) {
    struct tm when;
    time_t whattime;
    when.tm_sec = 30;
    when.tm_min = 30;
    when.tm_hour = 2;
    when.tm_mday = 1;
    when.tm_mon = 1;
    when.tm_year = 103;
    whattime = mktime(&when);
    printf("Day and time is %s\n", asctime(&when));
}
@endcode
<b>Output:</b><br>
Day and time is Sat Feb 1 02:30:30 2003
 */
char *asctime(const struct tm *tptr);


/**
Description: Calculates the processor time.

<b>Include:</b> <time.h>

@return Returns the number of clock ticks of elapsed processor time.

<b>Remarks:</b><br> If the target environment cannot measure elapsed processor time, the
function returns -1, cast as a clock_t. (i.e. (clock_t) -1).

By default, the 16-bit compiler returns the time as instruction cycles.

<b>Example:</b><br>
@code
#include <time.h> // for clock 
#include <stdio.h> // for printf 
volatile int i;

int main(void) {
    clock_t start, stop;
    int ct;
    start = clock();
    for (i = 0; i < 10; i++)
        stop = clock();
    printf("start = %ld\n", start);
    printf("stop = %ld\n", stop);
}
@endcode
<b>Output:</b><br>
start = 0 <br>
stop = 317
 */
clock_t clock(void);



/**
Description: Converts calendar time to a string representation of local time.

<b>Include:</b> <time.h>

@param tod pointer to stored time

@return Returns the address of a string that represents the local time of the
parameter passed.

<b>Remarks:</b><br> This function is equivalent to asctime(localtime(tod)).

<b>Example:</b><br>
@code
#include <time.h> // for mktime, tm, ctime 
#include <stdio.h> // for printf 

int main(void) {
    time_t whattime;
    struct tm nowtime;
    nowtime.tm_sec = 30;
    nowtime.tm_min = 30;
    nowtime.tm_hour = 2;
    nowtime.tm_mday = 1;
    nowtime.tm_mon = 1;
    nowtime.tm_year = 103;
    whattime = mktime(&nowtime);
    printf("Day and time %s\n", ctime(&whattime));
}
@endcode
<b>Output:</b><br>
Day and time Sat Feb 1 02:30:30 2003
 */
char *ctime(const time_t *tod);



/**
Description: Find the difference between two times.

<b>Include:</b> <time.h>

@param t1 ending time

@param t0 beginning time

@return Returns the number of seconds between t1 and t0.

<b>Remarks:</b><br> By default, the 16-bit compiler returns the time as instruction cycles so
difftime returns the number of ticks between t1 and t0.

<b>Example:</b><br>
@code
#include <time.h> // for clock, difftime 
#include <stdio.h> // for printf 
volatile int i;

int main(void) {
    clock_t start, stop;
    double elapsed;
    start = clock();
    for (i = 0; i < 10; i++)
        stop = clock();
    printf("start = %ld\n", start);
    printf("stop = %ld\n", stop);
    elapsed = difftime(stop, start);
    printf("Elapsed time = %.0f\n", elapsed);
}
@endcode
<b>Output:</b><br>
start = 0 <br>
stop = 317 <br>
Elapsed time = 317
 */
double difftime(time_t t1, time_t t0);



/**
Description: Converts calendar time to time structure expressed as Universal Time
Coordinated (UTC) also known as Greenwich Mean Time (GMT).

<b>Include:</b> <time.h>

@param tod pointer to stored time

@return Returns the address of the time structure.

<b>Remarks:</b><br> This function breaks down the tod value into the time structure of type
tm. By default, the 16-bit compiler returns the time as instruction
cycles. With this default, gmtime and localtime will be equivalent,
except gmtime will return tm_isdst (Daylight Savings Time flag) as
zero to indicate that Daylight Savings Time is not in effect.

<b>Example:</b><br>
@code
#include <time.h> // for gmtime, asctime, 
// time_t, tm 
#include <stdio.h> // for printf 

int main(void) {
    time_t timer;
    struct tm *newtime;
    timer = 1066668182; // Mon Oct 20 16:43:02 2003 
    newtime = gmtime(&timer);
    printf("UTC time = %s\n", asctime(newtime));
}
@endcode
<b>Output:</b><br>
UTC time = Mon Oct 20 16:43:02 2003
 */
struct tm *gmtime(const time_t *tod);



/**
Description: Converts a value to the local time.

<b>Include:</b> <time.h>

@param tod pointer to stored time

@return Returns the address of the time structure.

<b>Remarks:</b><br> By default, the 16-bit compiler returns the time as instruction cycles.
With this default, localtime and gmtime will be equivalent, except
localtime will return tm_isdst (Daylight Savings Time flag) as -1 to
indicate that the status of Daylight Savings Time is not known.

<b>Example:</b><br>
@code
#include <time.h> // for localtime, 
// asctime, time_t, tm 
#include <stdio.h> // for printf 

int main(void) {
    time_t timer;
    struct tm *newtime;
    timer = 1066668182; // Mon Oct 20 16:43:02 2003 
    newtime = localtime(&timer);
    printf("Local time = %s\n", asctime(newtime));
}
@endcode
<b>Output:</b><br>
Local time = Mon Oct 20 16:43:02 2003
 */
struct tm *localtime(const time_t *tod);



/**
Description: Converts local time to a calendar value.

<b>Include:</b> <time.h>

@param tptr a pointer to the time structure

@return Returns the calendar time encoded as a value of time_t.

<b>Remarks:</b><br> If the calendar time cannot be represented, the function returns -1, cast
as a time_t (i.e. (time_t) -1).

<b>Example:</b><br>
@code
#include <time.h> // for localtime, 
// asctime, mktime, 
// time_t, tm 
#include <stdio.h> // for printf 

int main(void) {
    time_t timer, whattime;
    struct tm *newtime;
    timer = 1066668182; // Mon Oct 20 16:43:02 2003 
    // localtime allocates space for struct tm 
    newtime = localtime(&timer);
    printf("Local time = %s", asctime(newtime));
    whattime = mktime(newtime);
    printf("Calendar time as time_t = %ld\n",
            whattime);
}
@endcode
<b>Output:</b><br>
Local time = Mon Oct 20 16:43:02 2003 <br>
Calendar time as time_t = 1066668182
 */
time_t mktime(struct tm *tptr);



/**
Description: Formats the time structure to a string based on the format parameter.

<b>Include:</b> <time.h>
const char *format, const struct tm *tptr);

@param s output string

@param n maximum length of string

@param format format-control string

@param tptr pointer to tm data structure

@return Returns the number of characters placed in the array, s, if the total,
including the terminating null, is not greater than n. Otherwise, the
function returns 0 and the contents of array, s, are indeterminate.

<b>Remarks:</b><br> The format parameters follow:
%a abbreviated weekday name
%A full weekday name
%b abbreviated month name
%B full month name
%c appropriate date and time representation
%d day of the month (01-31)
%H hour of the day (00-23)
%I hour of the day (01-12)
%j day of the year (001-366)
%m month of the year (01-12)
%M minute of the hour (00-59)
%p AM/PM designator
%S second of the minute (00-61)
allowing for up to two leap seconds
%U week number of the year where Sunday is the first day of week 1
(00-53)
%w weekday where Sunday is day 0 (0-6)
%W week number of the year where Monday is the first day of week 1
(00-53)
%x appropriate date representation
%X appropriate time representation
%y year without century (00-99)
%Y year with century
%Z time zone (possibly abbreviated) or no characters if time zone is
unavailable
%% percent character %

<b>Example:</b><br>
@code
#include <time.h> // for strftime, 
// localtime, 
// time_t, tm 
#include <stdio.h> // for printf 

int main(void) {
    time_t timer, whattime;
    struct tm *newtime;
    char buf[128];
    timer = 1066668182; // Mon Oct 20 16:43:02 2003 
    // localtime allocates space for structure 
    newtime = localtime(&timer);
    strftime(buf, 128, "It was a %A, %d days into the "
            "month of %B in the year %Y.\n", newtime);
    printf(buf);
    strftime(buf, 128, "It was %W weeks into the year "
            "or %j days into the year.\n", newtime);
    printf(buf);
}

@endcode
<b>Output:</b><br>
It was a Monday, 20 days into the month of October in the year 2003. <br>
It was 42 weeks into the year or 293 days into the year.
 */
size_t strftime(char *s, size_t n,



        /**
        Description: Calculates the current calendar time.
        
<b>Include:</b> <time.h>
        
@param tod pointer to storage location for time
        
@return Returns the calendar time encoded as a value of time_t.
        
<b>Remarks:</b><br> If the target environment cannot determine the time, the function
        returns -1, cast as a time_t. By default, the 16-bit compiler returns the
        time as instruction cycles. This function is customizable. See
        pic30-libs.

        <b>Example:</b><br>
        @code
#include <time.h> // for time 
#include <stdio.h> // for printf 
        volatile int i;
        int main(void) {
    time_t ticks;
            time(0); // start time 
    for (i = 0; i < 10; i++) // waste time 
            time(&ticks); // get time 
            printf("Time = %ld\n", ticks);
    }
@endcode
<b>Output:</b><br>
Time = 256
 */
time_t time(time_t *tod);