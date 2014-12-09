/**
 * @brief           I2C Port 1 functions
 * @file            nz_serI2C.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_seri2c_desc Description
 *****************************************
 * Functions for communicating with I2C Ports. It is interrupt based, and all processing
 * is handled in the background by interrupt service routines. Internally @ref nz_circularBuffer_desc "Circular Buffers"
 * are used, allowing multiple messages to be written to the queue.
 * - @ref nz_seri2c_details "Dedicated I2C functions" can be used for communicating with I2C ports.
 * - Alternatively messages can be directly written to, and read from the underlying @ref nz_seri2c_cirbuf "Circular Buffer".
 * - @ref nz_seri2c_ascii "ASCII Commands" are also supported if human readable, ASCIII commands are required.
 * 
 * @subsection nz_seri2c_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --------------- I2C Configuration (from nz_serI2C.h) ----------------
// *********************************************************************
//To save space, this will remove all code to do with Listeners
//#define I2C_DONT_SUPPORT_LISTENERS

//Number of listeners that can be registered for all I2C ports, a value from 1 to 7.
#define    I2C_LISTENERS 4          //[-DEFAULT-]

//---- Enable I2C1 as a "Serial Data Port" ----
#define NZ_I2C1_ENABLE

//I2C1 Interrupt priority, a value from 0 to 7. 0 will disable interrupt, 7 is highest priority.
#define    nzINT_PRIORITY_I2C1 3      //[-DEFAULT-]

//Transmit and Receive buffer sizes, for compatibility, ensure it is a power of 2 value! This can be bypassed
//by using a "non power of 2" CircularBuffer implementation for this project. See nz_CircularBuffer.h for details.
#define    I2C1_RXBUF_SIZE  64      //[-DEFAULT-]
#define    I2C1_TXBUF_SIZE  256     //[-DEFAULT-]

//I2C1 speed, can be 10K, 20K, 50K, 100K, 400K or 1M. Can also use value, for example 78 will give 200kHz.
//IMPORTANT to limit it to speed of lowest device on bus! For example, is 400kHz for RTC on SBC66EC!
#define    I2C1_BAUD I2C_BAUD_100K  //[-DEFAULT-]

//Uncomment this to define your own I2C 1 TX and RX "Circular Buffers"
//#define I2C1_DONT_CREATE_CIRBUF

//I2C1 ISR timeout, default is 200ms. This is maximum time allowed from START till STOP
#define I2C1_TIMEOUT 200            //[-DEFAULT-]

//---- Enable I2C2 as a "Serial Data Port" ----
//#define NZ_I2C2_ENABLE

//I2C2 Interrupt priority, a value from 0 to 7. 0 will disable interrupt, 7 is highest priority.
#define    nzINT_PRIORITY_I2C2 4      //[-DEFAULT-]

//Transmit and Receive buffer sizes, for compatibility, ensure it is a power of 2 value! This can be bypassed
//by using a "non power of 2" CircularBuffer implementation for this project. See nz_CircularBuffer.h for details.
#define    I2C2_RXBUF_SIZE  32      //[-DEFAULT-]
#define    I2C2_TXBUF_SIZE  32      //[-DEFAULT-]
 @endcode
 *
 * 
 * @subsection nz_seri2c_usage Usage
 *****************************************
 * To use this module, the following must be done:
 * - Ensure a "Circular Buffer" has been added to the project, for details @ref nz_circularBuffer_usage "see Circular Buffer Usage".
 * - Copy the @ref nz_seri2c_conf "Configuration" section above (from nz_serI2C.h)
 *   to the projdefs.h file.
 * - In this "Configuration" section, enable required I2C ports. For example to enable I2C 1, ensure following is uncommented:
 @code
 #define NZ_I2C1_ENABLE
 @endcode
 * - In this "Configuration" section, change any default values if required (if default
 *   values should be used, define is not required in projdefs.h).
 * - Add nz_serI2C.c and nz_serDataPorts.c to the MPLAB project.
 * - Include nz_serI2C.h in the c file it is used in.
 * - <b>All DONE!</b> The functions defined in this file can now be used in the project.
 * 
 **********************************************************************
 * @section nz_seri2c1_lic Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2011-06-03, David H. (DH):
 *    - Initial version
 *********************************************************************/


/////////////////////////////////////////////////
//  I2C - documentation module
/////////////////////////////////////////////////
/** @defgroup nz_seri2c_intro I2C Serial Data Ports
 @ingroup info_serport
 Netcruzer boards have 2 to 3 I2C ports. They are all interrupt based, meaning once
 a message is written to the queue, it is processed in the background by an interrupt
 service routine.
 Functions are provided for querying the status of a transmission, and reading data read
 from the I2C port.

 For details on "Serial Data Port", see the @ref nz_seri2c_desc "nz_serI2C.h Documentation".

 @section nz_seri2c_details Details
 This module implements an I2C Master, and provides functions for communicating with slave devices.
 All I2C communication is done in the background via I2C interrupts. Transmit and receive Queues
 (Circular Buffers) are used for writing and reading data to and from I2C devices.

 <!-- - - - Writing I2C Messages - - - -->
 @subsection nz_seri2c_write Writing to the I2C Port
 To sent data to an I2C device, the message is written to the Transmit Queue (Circular Buffer)
 by using the provided i2c1Xxx to i2c3Xxx functions, like i2c1Write() for example.
 To get status of transmission, see @ref nz_seri2c_status "Status of I2C Transmission"
 section below. The Transmit Queue is monitored in the background via the I2C
 interrupts, and if it contains data, is written out via the I2C bus.
 There are a couple of different ways of doing this (I2C 1 functions shown,
 replace 1 with 2 or 3 for other I2C ports):
 - Using the i2c1BeginTransmission(), i2c1Write() and i2c1EndTransmission() functions.
 - Preparing the message in a BYTE array, and sending it via the i2c1WriteMessage() function.
 - Sending a message in "ASCII Format, with Escape Sequences" using the i2c1WriteAsciiEscString()
   function.
 - ADVANCED! Using Circular Buffer functions, and adding the message direct to the Transmit
   Circular Buffer for the required I2C bus. See "Advanced" section below for details.

 For example, to write two bytes (0x80 and 0xA7) to a device with address 0x60, the following code
 can be used.
 @code
    i2c1BeginTransmission(0x60);
    i2c1Write(0x80);
    i2c1Write(0xA7);
    i2c1EndTransmission();
 @endcode
 Alternatively the i2c1WriteMessage() function can be used to send an I2C message. The example above can be
 replaced by the following code.
 @code
 BYTE msg[] = {0x80, 0xA7};
 i2c1WriteMessage(0x60, msg, sizeof(msg));
 @endcode
 or
 @code
 i2c1WriteMessage(0x60, (BYTE*)"\x80\xA7", sizeof(msg)); //Write Byte 1=0x80, Byte2=0xA7 to slave at 0x60
 @endcode
 Using the i2c1WriteAsciiEscString() function, the following code can be used:
 @code
 i2c1WriteAsciiEscString("6080A7");
 @endcode
 The i2c1WriteAsciiEscString() function uses "ASCII Format, with Escape Sequences" (Control
 Characters). This format uses 2 upper case characters to represent a single hex character.
 Strings must be enclosed within single quotation marks('). Lower case characters 'a' to 'z'
 are used to represent "Control characters".
 See @ref nz_circularBuffer_format_ascii_esc "ASCII Format, with Escape Sequence" for details.
 The following <b>Control Characters</b> are defined:
 - <b>s and p</b>: Put Start(s) and Stop(p) Condition on bus. A second 's' character (anywhere in string)
   before 'p' character will cause a repeated Start condition.
 - <b>rNN</b>: Read from bus, where NN is two upper case hex characters stating how many bytes to read.

 This method of writing I2C messages is particularly well suited for writing messages that contain text
 strings. For example, to write a string to a "LCD2S Serial I2C Display", the following code can be used:
 @code
 //Write to LCD2S at address 0x50, 0x80=Write Text command, 0x0C=Clear Screen command, 0x0A=Go to next line command
 i2c1WriteAsciiEscString("50800C'Hello'0A'World'");
 @endcode


 For additional examples, see <a href=http://www.netcruzer.com/examples-i2c>examples on netcruzer.com</a>.

 <!-- - - - Reading I2C Messages - - - -->
 @subsection nz_seri2c_read Reading from the I2C Port
 When reading data from an I2C device, the data will be read in the background via the I2C
 interrupts. The destination of the read data is determined by the read functions used:
 - Using the i2c1Read() function, the read data will be placed in the default Circular
   Receive Buffer. It is retrieved by using the i2c1GetByte() or i2c1GetArray() functions.
 - Using the i2c1ReadToArray() function, the read data will be placed in the buffer passed to
   this function.

 The i2c1IsBusy() or i2c1GetStatusWait() functions can be used to check when the background
 transmission has finished (and if successful), and the read data is available.

 The following example show how to use i2c1Read() to read data:
 @code
 BYTE c;
 i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 i2c1Write(0xD0);                //Write 0xD0 = address of register to read
 i2c1Read(10);                   //Read 10 byte.
 i2c1EndTransmission();          //End transmission, and start sending message in background
 //Wait till transmission finished (background via interrupt), and check if successful
 if (i2c1GetStatusWait(0)==0) {
     //Get all available data from default Receive Circular Buffer (might be less than requested!).
     while (i2c1HasData()) {
         c = i2c1GetByte();      //Get next available byte
     }
 }
 @endcode

 The following example show how to use i2c1ReadToArray() to read data to a BYTE array:
 @code
 BYTE buf[2];
 i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 i2c1Write(0xD0);                //Write 0xD0 = address of register to read
 i2c1ReadToArray(buf, 2);        //Read 2 bytes to buf
 i2c1EndTransmission();          //End transmission, and start sending message in background
 //Wait till transmission finished (background via interrupt), and get status
 if (i2c1GetStatusWait(0)==0) {
     //Message Successful. Read data available in buf[0] and buf[1].
 }
 @endcode

 The i2c1ReadSlaveReg() function replaces the code shown above with a single function.
 @code
 BYTE buf[2];
 //Read 2 bytes from register 0x0D of slave at address 0x50, and save result in "buf" array
 i2c1ReadSlaveReg(0x50, 0x0D, buf, 2);
 //Wait till transmission finished (background via interrupt), and get status
 if (i2c1GetStatusWait(0)==0) {
     //Message Successful. Read data available in buf[0] and buf[1].
 }
 @endcode


 Each I2C device actually has two addresses. The even address is used to write to it, and the odd address to read from it.
 Bit 0 (R/W bit) of the address byte is used to determine if a read or write command has to be executed. If 1, a I2C read is executed.
 To read, set bit 0 of the address. For example, accessing address 0x20 will write to the node, and address 0x21 will read from it.

 <!-- - - - Status of I2C Message - - - -->
 @subsection nz_seri2c_status Status of I2C Transmission
 The status of an I2C transmission can be obtained by a couple of different methods.
 
 <b>Method 1 (pre transmission status):</b><br>
 The functions for adding data to the I2C Transmit Queue (Circular Buffer) all return 0 if successful.
 If any of them return a non-zero value, the Transmit Queue if probably full.
 At this stage the message has NOT been sent on the bus yet, so no bus error messages will be
 returned (slave not responding with ACK for example). These functions include:<br>
 i2c1BeginTransmission(), i2c1Write(), i2c1WriteString(), i2c1EndTransmission()....<br>
 To save checking the returned value of each function, only the value returned by
 i2c1EndTransmission() can be checked, for example:
 @code
 i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 i2c1Write(0xD0);                //Write 0xD0
 //End transmission, and start sending message in background. Returns 0 if ERROR!
 if (i2c1EndTransmission()!=0) {
     //Message Successfully added to transmit Queue.
 }
 @endcode
 Using this method of error checking does NOT detect if there is no slave on the bus with the
 given address. It however has <b>the advantage</b> of not having to wait for the transmission to
 finish!

 <b>Method 2 (post transmission status):</b><br>
 If additional status information is required, the code has to wait till the transmission is
 finished(is done in the background via I2C interrupts). Once finished, the i2c1GetStatus()
 function can be used to get the status of the transmission. The i2c1GetStatusWait() function
 will wait till finished, and then return the status. For example:
 @code
 i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 i2c1Write(0xD0);                //Write 0xD0
 i2c1EndTransmission();          //End transmission, and start sending message in background
 //Wait till transmission finished (background via interrupt), and check if successful
 if (i2c1GetStatusWait(0)==0) {
     //Message Successfully added to transmit Queue, and sent to slave!
 }
 @endcode

 The status of the last 4 transmissions are stored, and can be requested by passing
 the address of the slave in the "adr" parameter of the i2c1GetStatus() function. Passing 0 to
 this function will return the status of the last transmission. This function is useful
 when sending an I2C message, and checking the status at a later stage. If other tasks also
 send I2C messages to other slaves, this function can be used to be sure to get the status
 of the desired message.

 <!-- - - Port Independent I2C Functions - - -->
 @subsection nz_seri2c_port_independent Port Independent I2C Functions
 In stead of using the normal port specific i2c1Xxx(), i2c2Xxx() and i2c3Xxx() functions,
 the port independent i2cXxx() versions can be used. The same functions are available, but
 each has an additional parameter specifying the port to use. The first parameter of
 each function is a pointer to the I2C_INFO structure for the desired I2C port. The
 I2C1_INFO, I2C2_INFO and I2C3_INFO defines can be used. These functions can be used
 to <b>optimize performance</b>, seeing that they are a bit more efficient than the
 port specific versions. Internally, the port specific function call the port
 indepenent functions.

 For example, the i2cWrite() function is the port independent version of the
 i2c1Write() function. Using the normal function to write 100 to I2C bus 1 looks like this:
 @code
 i2c1Write(100);
 @endcode

 Using port independent functions look like this:
 @code
 i2cWrite(I2C1_INFO, 100);
 @endcode
 or
 @code
 I2C_INFO* objI2C = I2C1_INFO;
 i2cWrite(objI2C, 100);
 @endcode




 <!-- ========== Advanced - Using Circular Buffers ========== -->
 @section nz_seri2c_cirbuf Advanced - Using Circular Buffers
 Internally @ref nz_circularBuffer_desc "Circular Buffers" are used for queueing transmit data,
 and storing received data. In stead of using the normal i2cXxx functions (like i2cBeginTransmisstion(),
 i2cWrite()...), data can be directly written to, and read from the Circular Buffer.
 The buffers are configured as @ref nz_circularBuffer_format_bin_esc "Binary Format, with Escape Sequence".
 This means special "escape sequences"(control characters) are contained in the buffer, along with the normal data.
 See @ref nz_circularBuffer_format_bin_esc "Binary Format, with Escape Sequence" for details.
 
 The CIRBUF_TX_I2Cn and CIRBUF_RX_I2Cn constants (where n is port number) can be used with the Circular Buffer
 functions to address the required transmit or receive I2C buffers. For example, to write a byte to I2C port 1's 
 transmit buffer:
 @code
     cbufPutByte(CIRBUF_TX_I2C1, 0x6A);
 @endcode
 
 To send a message via the I2C port, it is written to the I2C transmit buffer. This buffer
 is processed in the background, and all data added to it is send via the I2S port. Special
 "escape sequences" (control characters) are used for putting Start, Stop and Repeated Start
 conditions on the bus. The '^' is the default "escape character". The "escape sequences" are:

 <!--
 <table class="doxtable">
 <tr><th>
   Escape <br/> Sequence
 </th><th>
   Description
 </th></tr>
 <tr><td>
   @b ^^
 </td><td>
   Will cause a single ^ character to be sent on the I2C port
 </td></tr> 
 <tr><td>
   @b ^s
 </td><td>
   Put START on bus. A second 's' character (anywhere in string) before a 'p' character will cause a REPEATED START condition.
 </td></tr> 
 <tr><td>
   @b ^p
 </td><td>
   Put STOP on bus
 </td></tr> 
 <tr><td>
   @b ^rn
 </td><td>
   Read @b n number of bytes(n=0-254). A value of 255 means read till slave does not ACK. Read bytes are written to receive circular buffer.
 </td></tr>
 <tr><td>
   @b ^rn^ua
 </td><td>
   Same as <b>^rn</b>, but writes read bytes to given address in "User Memory", where <i>@b a</i> is a value from 0 to 255.
 </td></tr>
 </table>
 -->

 | Escape<br> Sequence | Description                                                                                                                |
 |---------------------|----------------------------------------------------------------------------------------------------------------------------|
 | @b ^^  | Will cause a single @b '^' character to be sent on the I2C port                                                                           |
 | @b ^s  | Put START on bus. A second 's' control character (anywhere in string) before a 'p' control character will cause a REPEATED START condition.             |
 | @b ^p  | Put STOP on bus                                                                                                                         |
 | @b ^rn | Read @b 'n' number of bytes(n=0-254). A value of 255 means read till slave does not ACK. Read bytes are written to receive circular buffer.    |
 | @b ^rn^ua | Same as @b '^rn', but writes read bytes to given address in "User Memory", where @b 'a' is a value from 0 to 255.    |
 | @b ^rn^ts | Same as @b '^rn', but writes read bytes to given @ref info_serport_num, where @b 's' is the "Serial Data Port Buffer ID".    |
 | @b ^Rnbb | Read @b 'n' number of bytes(n=0-254), and writes read bytes to given address, 'bb' is address pointer.    |
 | @b ^Lna | Read @b 'n' number of bytes(n=0-254), and writes read bytes to given registered Circular Buffers, 'a' is OR'ed bits representing registered Circular Buffers |

 For the '<b>^s</b>' Escape Sequence (Start), the next byte in the buffer MUST be the slave address! Even addresses are used to write
 to the slave, and odd addresses to read from them.
 - The address must be an <b>even</b> number when writing data to the slave. For example:<br>
   ^,s,50,10,5A,^,p = START, 0x50 address, write 0x10, write 0x5A, STOP.
 - The address must be an <b>odd</b> number when reading data from a slave. A START with odd number address MUST be followed by the
   '<b>^r</b>'(Read) Escape Sequence - see description below!

 For the '<b>^r</b>' Escape Sequence (Read), the next byte in the buffer MUST be the number of bytes to read! It can only be used inside a
 message with START and STOP conditions.
 - When '<b>^r</b>' is positioned <b>after</b> written bytes, it will cause a <b>REPEATED START</b> condition. In this case, the address used for the
   START is reused, with the "R/W" bit set to 1. For example:<br>
   ^,s,50,10,^,r,02,^,p = START, 0x50 address, write 0x10, REPEATED START, 0x51 address, read 2 bytes, STOP.
 - When '<b>^r</b>' is positioned at the <b>beginning</b> of the message, directly after the START and slave address, no REPEATED START will be sent.
   For this case, the address following the START MUST be odd, meaning the "R/W" bit is set, indicating a read to follow! For example:<br>
   ^,s,51,^,r,02,^,p = START, 0x51 address, read 2 bytes, STOP.<br>
   As a shorthand, the  '<b>^r</b>' Escape Sequence (Read) can be omitted, and the number of bytes to read directly given. For example:<br>
   ^,s,51,02,^,p = START, 0x51 address, read 2 bytes, STOP.


 <!-- - - Examples - - -->
@subsection nz_seri2c_cirbuf_examples Examples
Following are some examples for using Circular Buffers for I2C Communication.

@code
    cbufPutControlChar(CIRBUF_TX_I2C1, 's');    //I2C Start - use 's' control character
    cbufPutByte(CIRBUF_TX_I2C1, 0x6A);          //I2C address - MUST follow I2C Start
    cbufPutByte(CIRBUF_TX_I2C1, 0x52);          //Data
    cbufPutControlChar(CIRBUF_TX_I2C1, 'p');    //I2C Stop - use 'p' control character
@endcode
Writes 0x52 to an I2C device with address 6A, via I2C port 1. The following characters will be added to the buffer<br>
'<b>^</b>', '<b>s</b>', <b>0x5A</b>, <b>0x52</b>, '<b>^</b>', '<b>p</b>'

@code
    cbufPutControlChar(CIRBUF_TX_I2C1, 's');    //I2C Start - use 's' control character
    cbufPutByte(CIRBUF_TX_I2C1, 0x50);          //I2C address - MUST follow I2C Start
    cbufPutString(CIRBUF_TX_I2C1, "Hello");     //Data
    cbufPutControlChar(CIRBUF_TX_I2C1, 'p');    //I2C Stop - use 'p' control character
@endcode
Writes "Hello" to an I2C device with address 50, via I2C port 2. The following characters will be added to the buffer<br>
'<b>^</b>', '<b>s</b>', <b>0x50</b>, '<b>H</b>', '<b>e</b>', '<b>l</b>', '<b>l</b>', '<b>o</b>', '<b>^</b>', '<b>p</b>'

<!-- 
Reading two bytes from an I2C device with address 51<br>
<br><code><b>s5102p</b></code>
<br>[I2C Start][Address:0x51, R/W = Read][Number of bytes to read: 02][I2C Stop]
<br>This commands sends and I2C message, beginning with a I2C Start condition, and ending with an
I2C stop condition. Two bytes are read from the I2C device with address 0x51 (or 0x50 for reading).

Repeated Start, write 1 byte (0x15), reading 3 bytes from an I2C device with address 2A (or 2B for writing)<br>
<br><code><b>s2A15 s2B03p</b></code>
<br>[I2C Start][Address:0x2A, R/W = Write][Data: 0x15] [I2C Repeated Start][Address:0x2B, R/W = Read][Number of bytes to read: 03][I2C Stop]
<br>This commands uses the I2C repeated start feature. If a second start command is sent, without sending a stop command, it will cause a repeated
start condition to be put on the I2C 1 "Serial Data Port".
A single byte is written to the I2C device with address 0x2A, and 3 bytes are read from the I2C device with address 0x2B.
-->



<!-- ========== Advanced - Using ASCII Commands ========== -->
@section nz_seri2c_ascii Advanced - Using ASCII Commands
As described in @ref nz_seri2c_cirbuf "the previous section", Circular Buffers (configured as Binary Format, with Escape
Sequence) are used for writing and reading data.
It describes how to write and read binary data to the Circular Buffers. As an alternative, this same
data can be written and read in ASCII format using the cbufPutAsciiEscString() function. This function will decode
the given ASCII format to the "Binary Format, with Escape Sequence" format of the buffer.
See @ref nz_circularBuffer_format_ascii_esc "ASCII Format, with Escape Sequence" for details on the format of the ASCII string.

In short, the ASCII string uses 2 upper case characters to represent a single hex character. Strings must be enclosed within single quotation marks(').
Lower case characters 'a' to 'z' are used to represent "Control characters".

<!-- - - Examples - - -->
@subsection nz_seri2c_ascii_examples Examples
Following are some examples for using ASCII commands for communicating I2C devices.

Writing 0x52 to an I2C device with address 6A, via I2C port 1<br>
@code
    cbufPutAsciiEscString(CIRBUF_TX_I2C1, 0, "s6A52p");    //Start, Address 0x6A, Data 0x52, Stop
@endcode
The following characters will be added to the buffer<br>
'<b>^</b>', '<b>s</b>', <b>0x6A</b>, <b>0x52</b>, '<b>^</b>', '<b>p</b>'

Writing "Hello" to an I2C device with address 50, via I2C port 2<br>
@code
    cbufPutAsciiEscString(CIRBUF_TX_I2C1, 0, "s50'Hello'p");   //Start, Address 0x50, String "Hello", Stop
@endcode
The following characters will be added to the buffer<br>
'<b>^</b>', '<b>s</b>', <b>0x50</b>, '<b>H</b>', '<b>e</b>', '<b>l</b>', '<b>l</b>', '<b>o</b>', '<b>^</b>', '<b>p</b>'
*/


#ifndef NZ_SERI2C_H
#define NZ_SERI2C_H

#if defined(HAS_SERPORT_I2C)

#include "nz_circularBuffer.h"


////////// Default Defines //////////////////////

/** 62.1 kbits/sec Baud rate, this is the SLOWEST possible I2C baud rate! */
#ifndef I2C_BAUD_62K
#define I2C_BAUD_62K   255  /* This is the slowest possible BAUD rate! */
#endif

/** 100 kbits/sec Baud rate */
#ifndef I2C_BAUD_100K
#define I2C_BAUD_100K   157
#endif

/** 201 kbits/sec Baud rate */
#ifndef I2C_BAUD_200K
#define I2C_BAUD_200K   77
#endif

/** 398.5 kbits/sec Baud rate */
#ifndef I2C_BAUD_300K
#define I2C_BAUD_300K   51
#endif

/** 400 kbits/sec Baud rate */
#ifndef I2C_BAUD_400K
#define I2C_BAUD_400K   37
#endif

/** 506.3 kbits/sec Baud rate */
#ifndef I2C_BAUD_506K
#define I2C_BAUD_506K   24
#endif

/** 601 kbits/sec Baud rate */
#ifndef I2C_BAUD_600K
#define I2C_BAUD_600K   24
#endif

/** 708 kbits/sec Baud rate */
#ifndef I2C_BAUD_708K
#define I2C_BAUD_708K   20
#endif

/** 909 kbits/sec Baud rate */
#ifndef I2C_BAUD_909K
#define I2C_BAUD_909K   15
#endif

/** 970 kbits/sec Baud rate */
#ifndef I2C_BAUD_970K
#define I2C_BAUD_970K   12
#endif

//1.03 mbits/sec Baud rate */
#ifndef I2C_BAUD_1M
#define I2C_BAUD_1M   13
#endif

#ifndef I2C_ALWAYS_ADD_WHOLE_MESSAGE
#define I2C_ALWAYS_ADD_WHOLE_MESSAGE 0
#endif
#ifndef I2C2_ALWAYS_ADD_WHOLE_MESSAGE
#define I2C2_ALWAYS_ADD_WHOLE_MESSAGE 0
#endif
#ifndef I2C3_ALWAYS_ADD_WHOLE_MESSAGE
#define I2C3_ALWAYS_ADD_WHOLE_MESSAGE 0
#endif

//Transmit and Receive buffer sizes
#if !defined(I2C1_RXBUF_SIZE)
#define    I2C1_RXBUF_SIZE  64
#endif
#if !defined(I2C1_TXBUF_SIZE)
#define    I2C1_TXBUF_SIZE  256
#endif
#if !defined(I2C2_RXBUF_SIZE)
#define    I2C2_RXBUF_SIZE  32
#endif
#if !defined(I2C2_TXBUF_SIZE)
#define    I2C2_TXBUF_SIZE  128
#endif
#if !defined(I2C3_RXBUF_SIZE)
#define    I2C3_RXBUF_SIZE  32
#endif
#if !defined(I2C3_TXBUF_SIZE)
#define    I2C3_TXBUF_SIZE  128
#endif


////////// Defines //////////////////////////////

//I2C_TXION_STATUS_xx defines, 0=OK, 1=Added to Buffer, 0x80-0x8F=Error, 0xFF=Status Not Available
#define I2C_TXION_STATUS_OK            0    //Currently OK, have not finished adding whole message to buffer
#define I2C_TXION_STATUS_ADDED_TO_BUF  1    //Successfully added whole message to the buffer (from start to stop)
#define I2C_TXION_STATUS_ERROR         4    //No space in buffer to add message
#define I2C_TXION_STATUS_ERR_NOSPACE   5    //No space in buffer to add message
#define I2C_TXION_STATUS_ERR_ADR_NOACK 6    //Slave did not ACK address we sent
#define I2C_TXION_STATUS_ERR_DAT_NOACK 7    //Slave did not ACK data we sent
#define I2C_TXION_STATUS_ERR_BUF_FRMT  8    //Buffer format error
#define I2C_TXION_STATUS_ERR_TIMEOUT   9    //Timeout
#define I2C_TXION_STATUS_NOT_AVAILABLE 0xFF //Status not available

//I2C "Serial Data Port"
#if (defined(HAS_SERPORT_I2C1) || defined(HAS_SERPORT_I2C2) || defined(HAS_SERPORT_I2C3))
    #if !defined(HAS_A_SERPORT)
    #define HAS_A_SERPORT
    #endif
#endif


////////// Configuration Checks /////////////////

//If "Power of 2" buffer used, size must by a power of 2
#if defined(CIRBUF_USE_CIRCULAR_BUFFER_PWR2)
#if defined(HAS_SERPORT_I2C1)
    //Ensure size is power of 2
    #if ((I2C1_TXBUF_SIZE & ~(I2C1_TXBUF_SIZE-1))!=I2C1_TXBUF_SIZE)
    #error "I2C1 I2C1_TXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif

    //Ensure size is power of 2
    #if ((I2C1_RXBUF_SIZE & ~(I2C1_RXBUF_SIZE-1))!=I2C1_RXBUF_SIZE)
    #error "I2C1 I2C1_RXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif
#endif
#if defined(HAS_SERPORT_I2C2)
    //Ensure size is power of 2
    #if ((I2C2_TXBUF_SIZE & ~(I2C2_TXBUF_SIZE-1))!=I2C2_TXBUF_SIZE)
    #error "I2C2 I2C2_TXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif

    //Ensure size is power of 2
    #if ((I2C2_RXBUF_SIZE & ~(I2C2_RXBUF_SIZE-1))!=I2C2_RXBUF_SIZE)
    #error "I2C2 I2C2_RXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif
#endif
#if defined(HAS_SERPORT_I2C3)
    //Ensure size is power of 2
    #if ((I2C3_TXBUF_SIZE & ~(I2C3_TXBUF_SIZE-1))!=I2C3_TXBUF_SIZE)
    #error "I2C3 I2C3_TXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif

    //Ensure size is power of 2
    #if ((I2C3_RXBUF_SIZE & ~(I2C3_RXBUF_SIZE-1))!=I2C3_RXBUF_SIZE)
    #error "I2C3 I2C3_RXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif
#endif
#endif



/**
 * Structure containing I2C Serport Flags
 */
typedef struct __attribute__((aligned(2), packed))
{
    union {
        struct
        {
            unsigned int bFill : 16;        //Fill to ensure flags take up a 16-bit WORD
        } flags;
        WORD flagsVal;
    };

    /// Configures I2C1. This is the value for I2C1CON register of PIC, see PIC datasheet for details.
    // Not all values for I2C1CON are possible, only following can be set:
    /// I2C Stop in Idle Mode bit:
    ///   - I2C_IDLE_STOP
    ///   - I2C_IDLE_CON  (Default)
    ///
    /// Intelligent Peripheral Management Interface Enable. Enable receiving all addresses:
    ///   - I2C_IPMI_EN
    ///   - I2C_IPMI_DIS  (Default)
    ///
    /// Slave Address bits, standard is 7 bits:
    ///   - I2C_10BIT_ADD
    ///   - I2C_7BIT_ADD  (Default)
    ///
    /// Disable Slew Rate Control bit. For speeds of 400kHz and above, Slew Rate should be enabled:
    ///   - I2C_SLW_DIS
    ///   - I2C_SLW_EN  (Default)
    ///
    /// SMBus Input Level bits. Enable I/O pin thresholds compliant with SMBus specification:
    ///   - I2C_SM_EN
    ///   - I2C_SM_DIS  (Default)
    WORD    config1;

    ///Baud rate, for default case where RTC is connected to I2C bus 1, do not make higher than 400kHz = maximum speed
    //for RTC. This is the value for I2C1BRG register of PIC, see PIC datasheet for details. Some values are:
    //157=100kHz, 75=200kHz, 37=400kHz, 12=970kHz, 11=1.03Mhz, 8=1.28Mhz
    WORD    baud;

} SERPORT_I2C_CONFIG;

//DON'T change! Code that checks wrDstType.bit0 and wrDstType.bit1 assumes there values!
#define RD_DEST_TYPE_LISTENERS 0
#define RD_DEST_TYPE_POINTER   2
#define RD_DEST_TYPE_CIRBUF    3

typedef struct __attribute__((__packed__)) I2C_ISR_SMFLAGS_
{
    union {
        struct
        {
            unsigned int sm : 4;            ///< ISR State
            unsigned int sAddress : 1;      ///< Address following Start condition was just sent
            unsigned int b : 1;             ///< Temp bit
            unsigned int wrDstType : 2;     ///< Write to dest contained in isrRdDest, 0(and 1)=Listener(bits), 2=Pointer, 3=Circular Buffer
            unsigned int isrTxionStatus: 4; ///< Status is ISR Transmission/Reception
            unsigned int fill12 : 4;
        } bits;
        struct
        {
            unsigned int fill0to5 : 6;      ///< Fill for sm, sAddress and b
            unsigned int bit0 : 1;          ///< wrDstType bit 0
            unsigned int bit1 : 1;          ///< wrDstType bit 1, when 0, isrRdDest = Listener(bits)
            unsigned int fill8to15 : 8;     ///< Fill for sm, sAddress and b
        } wrDstType;
        WORD val;                           ///< Bits 0-3 = State, bits 4-7 = Flags
    };
} I2C_ISR_SMFLAGS;

//Must be a power of two value: 4,8,16,32,64
#define I2C_STATUS_BUF_SIZE 8

typedef struct I2C_INFO_
//typedef struct __attribute__((__packed__)) I2C_INFO_
//typedef struct __attribute__((aligned(2), packed)) I2C_INFO_
{
    //Buffer containing status. Two bytes are used for each entry, the LSB=Address (0=empty), and MSB=Status.
    BYTE            bufStat[I2C_STATUS_BUF_SIZE];
    //BYTE            putStat;    //Put pointer to bufStat. Points to last entry added.
    BYTE            adr;        //Used by non-ISR functions to remember current slave address
    BYTE            taskSm;     //ISR State machine. 0 is idle.
    BYTE            isrAdr;     //Used in ISR - address of slave
    BYTE            isrCnt;     //Incremented in ISR each time a STOP condition is put on the bus
    BYTE            isrRead;    //Used in ISR - number of bytes to to read
    BYTE            fillByte;
    
    WORD            isrRdDest;  //Destination for read, meaning depends on value of i2c1.isr.bits.wrDstType
                                // - When 0, bits 0-15 indicates which "Circular Buffer" listeners are destinations (0=default)
                                // - When 1, contains pointer to destination BYTE array
    WORD            timeout;    //Used in task to check for a timeout
    CIRBUF*         pCbufTx;
    CIRBUF*         pCbufRx;
    I2C_ISR_SMFLAGS isr;        //ISR State machine AND flags.
    union {
        struct
        {
            BYTE ch;
            BYTE currTxionStatus;
//            unsigned int ch                 : 3;    //Channel Offset = number - 1: 0=I2C1, 1=I2C2, 2=I2C3
//            unsigned int bFill3to7          : 5;    //Fill bytes
//            unsigned int currTxionStatus    : 4;    //Status of current Transmission
//            unsigned int bFill12to15        : 4;    //Fill bytes
        } flags;
        WORD flagsVal;
    };
} I2C_INFO;



/////////////////////////////////////////////////
//I2C Functions for all ports

/**
 * Initialization code.
 */
void serI2CInit(void);

/**
 * Service given I2C "Serial Data Port"
 * 
 * @param objI2C Pointer to I2C_INFO structure for I2C Bus to service
 */
void serI2CTask(I2C_INFO* objI2C);


/**
 * Register given "Circular Buffer" as a listener. After doing this, the returned
 * "Listener ID" can be used as a parameter with certian I2C functions that return data.
 * Multiple "Listernet IDs" can be OR'ed together.
 *
 * If the given "Circular Buffer" is already registered, it will not register it
 * again. It will simply return it's "Listener ID".
 *
 * @param pBuf Pointer to "Circular Buffer"
 *
 * @return Returns "Listener ID", or 0 if error. The most likely cause of an error is
 *         that there is no more space available for registering listeners.
 */
BYTE serI2CRegisterListener(CIRBUF* pBuf);


/**
 * Gets a pointer to the I2C_INFO structure for the given bus number.
 *
 * @param i2cBus I2C Bus number. Must be an I2C bus that is available, for example
 *        if the board has an I2C 1 and I2C 2 bus, it must be 1 or 2.
 *
 * @return Returns a pointer to the I2C_INFO structure for the given bus number.
 *         Return 0 if error, or given bus number is not available.
 */
I2C_INFO* i2cGetInfoForBus(BYTE i2cBus);

/**
 * Get the status of the last transmission to the given slave (address is given via adr
 * parameter). A cache of transmission statuses is kept. This function will REMOVE the
 * status from this cache! It is VERY IMPORTANT to only call this function once to get
 * the status for a transmission, seeing that consecuture calls will return 0xff (not
 * availabe). This is because the status is removed from cache after the first call!
 *
 * If 0 is given as the address, the status of the last transmission is returned. Nothing
 * is removed from the cache! This function can consecutively be called with 0 as the
 * requested address.
 *
 * !!! IMPORTANT !!! This function does NOT check if a transmission is currently in
 * progress! If the status of a current transmission should be obtained, use the
 * i2cIsBusy() or i2cGetStatusWait() functions!
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 *
 * @param adr The address of the slave node to return the status for (and REMOVE from
 *            cache), or 0 to return status for last transmission.
 *
 * @return Returns a BYTE with one of the following values:
 * - 0: Last message was successfully sent (a I2C device acknowledged)
 * - 0xff: Status information not available. Could be that bus is still busy transmitting or receiving.
 * - I2C_TXION_STATUS_ERR_NOSPACE: Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size"
 *      to "projdefs.h" file, and increase size of buffer.
 * - I2C_TXION_STATUS_ERR_ADR_NOACK: Did not receive a ACK (Acknowledge) when sending
 *      I2C address. This is the case when there is no device on the I2C bus with the given address.
 * - I2C_TXION_STATUS_ERR_DAT_NOACK: Did not receive a ACK (Acknowledge) when sending
 *      I2C data. This is the case when there is no device on the I2C bus with the given address.
 * - I2C_TXION_STATUS_ERR_BUF_FRMT: Buffer format error
 */
BYTE i2cGetStatus(I2C_INFO* objI2C, BYTE adr);


/**
 * Same as i2cGetStatus() function, but waits for current I2C transmission to finish,
 * and when done, returns (AND REMOVES from cache) it's status.
 * A cache of transmission statuses is kept. This function will REMOVE the
 * status from this cache! It is VERY IMPORTANT to only call this function once to get
 * the status for a transmission, seeing that consecuture calls will return 0xff (not
 * availabe). This is because the status is removed from cache after the first call!
 *
 * If 0 is given as the address, the status of the last transmission is returned. Nothing
 * is removed from the cache! This function can consecutively be called with 0 as the
 * requested address.
 *
 * For am example using this function, see the port dependant version
 * i2c1GetStatusWait().
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param adr The address of the slave node to return the status for, or
 * 0 to return status for last transmission.
 *
 * @return Returns a BYTE with one of the following values:
 * - 0: Last message was successfully sent (a I2C device acknowledged)
 * - 0xff: Status information not available. Could be that bus is still busy transmitting or receiving.
 * - I2C_TXION_STATUS_ERR_NOSPACE: Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size"
 *      to "projdefs.h" file, and increase size of buffer.
 * - I2C_TXION_STATUS_ERR_ADR_NOACK: Did not receive a ACK (Acknowledge) when sending
 *      I2C address. This is the case when there is no device on the I2C bus with the given address.
 * - I2C_TXION_STATUS_ERR_DAT_NOACK: Did not receive a ACK (Acknowledge) when sending
 *      I2C data. This is the case when there is no device on the I2C bus with the given address.
 * - I2C_TXION_STATUS_ERR_BUF_FRMT: Buffer format error
 */
BYTE i2cGetStatusWait(I2C_INFO* objI2C, BYTE adr);

/**
 * Check if the I2C port is currently Transmitting or Receiving. This is done in the background
 * via the I2C interrupt. Messages can still be added if true. This function is mainly
 * required to wait for the current Transmission to finish, so the status can be read.
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 *
 * @return Returns TRUE if the I2C port is currently Transmitting or Receiving, else FALSE.
 */
BYTE i2cIsBusy(I2C_INFO* objI2C);


/**
 * Writes a message to I2C 1 port. The message is written asynchronously, meaning it
 * is added to the queue, and written in the background (I2C1 interrupt service routine).
 * Start and Stop characters are automatically added to front and back of message.
 *
 * This function does the same as using i2c1BeginTransmission(),
 * i2c1Write() and i2c1EndTransmission().
 *
 * @preCondition The I2C port must be available! Check with i2c1IsAvailable() function.
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param adr Slave address
 * @param msg Byte array containing message to write
 * @param size Size of message to write
 *
 * @return Returns 0 if successfully added message to Queue (Transmit Circular Buffer)
 *      for transmission, else return the following error message:<br>
 *      I2C_TXION_STATUS_ERR_NOSPACE = Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size" to
 *      "projdefs.h" file, and increase size of buffer. Default is 128 bytes.
 */
BYTE i2cWriteMessage(I2C_INFO* objI2C, BYTE adr, BYTE* msg, WORD size);


/**
 * Begin a transmission on I2C 1 port.
 *
 * The following example shows how to send an I2C message, and check if it has
 * successfully been added to the Transmit Queue (Circular Buffer). It does NOT
 * check the status of the transmission.
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param adr The destination I2C address.
 *
 * @return A byte is returned, with the following meaning:
 * - 0: Success
 * - 1: Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size" to "projdefs.h" file,
 *      and increase size of buffer. Default is 128 bytes.
 * - 2: Other error.
 */
BYTE i2cBeginTransmission(I2C_INFO* objI2C, BYTE adr);


/**
 * End a transmission on I2C 1 port. This function writes "End Transmisstion" to
 * the I2C Transmit Queue (Circular Buffer). The message contained in the queue will
 * be transmitted asynchronously. Use the i2c1GetStatus() function to get the status
 * of the last transmitted message.
 *
 * The following example shows how to send an I2C message, and check if it has
 * successfully been added to the Transmit Queue (Circular Buffer). It does NOT
 * check the status of the transmission.
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 *
 * @return Returns 0 if successfully added message to Queue (Transmit Circular Buffer)
 *      for transmission, else return the following error message:<br>
 *      I2C_TXION_STATUS_ERR_NOSPACE = Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size" to
 *      "projdefs.h" file, and increase size of buffer. Default is 128 bytes.
 */
BYTE i2cEndTransmission(I2C_INFO* objI2C);


/**
 * Add a byte to the I2C Transmit Queue (Circular Buffer)
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines, for example I2C1_INFO = I2C 1.
 * @param b Byte to add to I2C Transmit Queue
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2cWrite(I2C_INFO* objI2C, BYTE b);

/**
 * Add given BYTE array to the I2C Transmit Queue (Circular Buffer), Escaping any
 * characters requiring it.
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param arr The byte array to add to I2C Transmit Queue
 * @param size Number of bytes to add
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2cWriteArray(I2C_INFO* objI2C, BYTE* arr, BYTE size);


/**
 * Adds given NULL terminated string to the I2C Transmit Queue (Circular Buffer).
 * The NULL terminator is NOT included (not written to buffer!)
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param str Null terminated string to add
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2cWriteString(I2C_INFO* objI2C, const char* str);


/**
 * Adds given NULL terminated string to the I2C Transmit Queue (Circular Buffer).
 * The given string must be an ASCII String, and can contain Escape Sequences.
 *
 * For details on the format of an ASCII String with Escape Sequences,
 * see @ref nz_circularBuffer_format_ascii_esc "ASCII Format, with Escape Sequences".
 *
 * If the I2C Transmit Queue does not have enough space, a NON-Zero error code is
 * returned, and nothing is done.
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param str Null terminated string to add, in "ASCII Format, with Escape Sequences".
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2cWriteAsciiEscString(I2C_INFO* objI2C, const char* str);


/**
 * Reads given number of bytes to the default Circular Receive Buffer.
 *
 * Once bytes have been read, use the i2c1GetXxx functions to get them.
 * See i2c1Read() for an example of doing this.
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param size Number of bytes to read.
 *
 * @return Returns 0 if request successfully added to Transmission Queue, else a
 * non-zero value is returned.
 */
BYTE i2cRead(I2C_INFO* objI2C, BYTE size);


/**
 * Reads given number of bytes to the given buffer.
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param buf Pointer to buffer to write read bytes to.
 * @param size Number of bytes to read.
 *
 * @return Returns 0 if request successfully added to Transmission Queue, else a
 * non-zero value is returned.
 */
BYTE i2cReadToArray(I2C_INFO* objI2C, BYTE* buf, BYTE size);


/**
 * Sends a message to slave at given address, requesting the given number of bytes.
 * This function sends a whole message, including I2C START and STOP conditions.
 * It first writes the given register to the slave, and then reads requested number
 * of bytes to given buffer.
 *
 * The following example:<br>
 * @code
 * BYTE buf[2];
 * i2c1ReadSlaveReg(0x50, 0xD0, buf, 2);
 * //Wait till transmission finished (background via interrupt), and check if successful
 * if (i2c1GetStatusWait(0)==0) {
 *     //Message Successful. Read data available in buf[0] and buf[1].
 * }
 * @endcode
 *
 * Does the same as the following code:<br>
 * @code
 * BYTE buf[2];
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0xD0);                //Write 0xD0 = address of register to read
 * i2c1ReadToArray(buf, 2);        //Read 2 bytes to buf
 * i2c1EndTransmission();          //End transmission, and start sending message in background
 * //Wait till transmission finished (background via interrupt), and check if successful
 * if (i2c1GetStatusWait(0)==0) {
 *     //Message Successful. Read data available in buf[0] and buf[1].
 * }
 * @endcode
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param adr Slave address
 * @param reg Register to write to I2C slave
 * @param buf Pointer to buffer to write read bytes to.
 * @param size Number of bytes to read.
 *
 * @return Returns 0 if request successfully added to Transmission Queue, else a
 * non-zero value is returned.
 */
BYTE i2cReadSlaveReg(I2C_INFO* objI2C, BYTE adr, BYTE reg, BYTE* buf, BYTE size);

/**
 * Reads a single byte from the Receive Queue(Circular Buffer). They can be read
 * with the i2c1Read... functions.
 * @preCondition i2c1GetRxBufCount() or i2c1IsRxBufEmpty() must have been called to
 * confirm there is data!
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 *
 * @return Returns number of bytes available in the Receive Queue(Circular Buffer)
 * for reading.
 */
#define i2cGetByte(objI2C) cbufGetByte(objI2C->pCbufRx)


/**
 * Reads requested number of bytes from Receive Queue(Circular Buffer), and removes
 * them from the queue. The actual number of bytes read is returned.
 * If returned value is less than requested value, all bytes were read.
 *
 * @preCondition i2c1GetRxBufCount() or i2c1IsRxBufEmpty() must have been called to
 * confirm there is data!
 *
 * @param objI2C The I2C bus to request status info from. Use I2Cx_INFO defines,
 *        for example I2C1_INFO = I2C 1.
 * @param pDstArr Pointer to destination buffer to write read bytes to.
 * @param size Number of bytes to read.
 *
 * @return Returns number of bytes added to destination array.
 */
#define i2cGetArray(objI2C, pDstArr, size) cbufGetArray(objI2C->pCbufRx, pDstArr, size)


/////////////////////////////////////////////////
//I2C 1
#if defined(HAS_SERPORT_I2C1)

#if !defined(SERPORT_I2C1_CREATE_OWN_CIRBUFS)
#include "nz_serDataPorts.h"
#endif

//Modes
#define SERPORT_I2C1_MODE_ASCII 0

//This is used by others
#ifndef THIS_IS_NZ_SERI2C1_C
extern I2C_INFO i2c1Info;
#endif
#define I2C1_INFO &i2c1Info

//TX and RX "Circular Buffers". Use SERPORT_I2C1_CREATE_OWN_CIRBUFS define to use this
//module without SERPORTs. If NOT defined, this modules requires nz_serDataPorts.c
#if defined(SERPORT_I2C1_CREATE_OWN_CIRBUFS)
    //This is used by others
    #ifndef THIS_IS_NZ_SERI2C1_C
    extern CIRBUF cbufTxI2C1;
    extern CIRBUF cbufRxI2C1;
    #endif

    #define CIRBUF_TX_I2C1 &cbufTxI2C1
    #define CIRBUF_RX_I2C1 &cbufRxI2C1
#endif


/**
 * Initialization code.
 */
void serI2C1Init(void);

/**
 * "Serial Data Port" Task
 */
//void serI2C1Task(void);

/**
 * Get the status of the last transmission to the given slave (address is given via adr
 * parameter). A cache of transmission statuses is kept. This function will REMOVE the
 * status from this cache! It is VERY IMPORTANT to only call this function once to get
 * the status for a transmission, seeing that consecutive calls will return 0xff (not
 * available). This is because the status is removed from cache after the first call!
 *
 * If 0 is given as the address, the status of the last transmission is returned. Nothing
 * is removed from the cache! This function can consecutively be called with 0 as the
 * requested address.
 *
 * !!! IMPORTANT !!! This function does NOT check if a transmission is currently in
 * progress! If the status of a current transmission should be obtained, use the
 * i2cIsBusy() or i2cGetStatusWait() functions!
 *
 * @param adr The address of the slave node to return the status for (and REMOVE from
 *            cache), or 0 to return status for last transmission.
 *
 * @return Returns a BYTE with one of the following values:
 * - 0: Last message was successfully sent (a I2C device acknowledged)
 * - 0xff: Status information not available. Could be that bus is still busy transmitting or receiving.
 * - I2C_TXION_STATUS_ERR_NOSPACE: Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size"
 *      to "projdefs.h" file, and increase size of buffer.
 * - I2C_TXION_STATUS_ERR_ADR_NOACK: Did not receive a ACK (Acknowledge) when sending
 *      I2C address. This is the case when there is no device on the I2C bus with the given address.
 * - I2C_TXION_STATUS_ERR_DAT_NOACK: Did not receive a ACK (Acknowledge) when sending
 *      I2C data. This is the case when there is no device on the I2C bus with the given address.
 * - I2C_TXION_STATUS_ERR_BUF_FRMT: Buffer format error
 */
BYTE i2c1GetStatus(BYTE adr);


/**
 * Same as i2c1GetStatus() function, but waits for current I2C transmission to finish,
 * and when done, returns (AND REMOVES from cache) it's status.
 * A cache of transmission statuses is kept. This function will REMOVE the
 * status from this cache! It is VERY IMPORTANT to only call this function once to get
 * the status for a transmission, seeing that consecutive calls will return 0xff (not
 * available). This is because the status is removed from cache after the first call!
 *
 * If 0 is given as the address, the status of the last transmission is returned. Nothing
 * is removed from the cache! This function can consecutively be called with 0 as the
 * requested address.
 *
 * An example of how to use this function is:
 * @code
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0xD0);                //Write 0xD0 = address of register to read
 * i2c1Read(1);                    //Read 1 byte
 * i2c1EndTransmission();          //End transmission, and start sending message in background
 * //Wait till transmission finished (background via interrupt), and get status
 * if (i2c1GetStatusWait(0)==0) {
 *     //Message Successfully sent!!!
 * }
 * @endcode

 * This function is the same as:
 * @code
 * BYTE status;
 * while(i2c1IsBusy()){serI2CTask();};  //Wait till background transmission finished
 * status = i2c1GetStatus(0);           //Get status of last transmission
 * @endcode
 *
 * @param adr The address of the slave node to return the status for, or
 *            0 to return status for last transmission.

 *  * @return Returns a BYTE with one of the following values:
 * - 0: Last message was successfully sent (a I2C device acknowledged)
 * - 0xff: Status information not available. Could be that bus is still busy transmitting or receiving.
 * - I2C_TXION_STATUS_ERR_NOSPACE: Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size"
 *      to "projdefs.h" file, and increase size of buffer.
 * - I2C_TXION_STATUS_ERR_ADR_NOACK: Did not receive a ACK (Acknowledge) when sending
 *      I2C address. This is the case when there is no device on the I2C bus with the given address.
 * - I2C_TXION_STATUS_ERR_DAT_NOACK: Did not receive a ACK (Acknowledge) when sending
 *      I2C data. This is the case when there is no device on the I2C bus with the given address.
 * - I2C_TXION_STATUS_ERR_BUF_FRMT: Buffer format error
 */
BYTE i2c1GetStatusWait(BYTE adr);

/**
 * Check if the I2C port is currently Transmitting or Receiving. This is done in the background
 * via the I2C interrupt. Messages can still be added if true. This function is mainly
 * required to wait for the current Transmission to finish, so the status can be read.
 *
 * An example of how to use this function is:
 * @code
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0xD0);                //Write 0xD0 = address of register to read
 * i2c1Read(1);                    //Read 1 byte
 * i2c1EndTransmission();          //End transmission, and start sending message in background
 * while(i2c1IsBusy()){serI2CTask();};  //Wait till background transmission finished
 * status = i2c1GetStatus(0);           //Get status of last transmission
 * @endcode
 *
 * @return Returns TRUE if the I2C port is currently Transmitting or Receiving, else FALSE.
 */
BYTE i2c1IsBusy(void);

/**
 * Get current Transmission Count. Is incremented for each message sent on I2C bus.
 */
#define i2c1GetTxionCnt()  (i2c1Info.isrCnt)

/**
 * Get last address used for Transmission.
 */
#define i2c1GetTxionAddress()  (i2c1Info.adr)


/**
 * Writes a message to I2C 1 port. The message is written asynchronously, meaning it
 * is added to the queue, and written in the background (I2C1 interrupt service routine).
 * Start and Stop characters are automatically added to front and back of message.
 *
 * This function does the same as using i2c1BeginTransmission(),
 * i2c1Write() and i2c1EndTransmission().
 *
 * @preCondition The I2C port must be available! Check with i2c1IsAvailable() function.
 *
 * The following example shows how to write 5 bytes to I2C 1 bus via this function:
 * @code
 * BYTE status;     //Status information
 * BYTE bufMsg[] = {0x80, 0x0C, 'H', 'i'};
 * i2c1WriteMessage(0x50, bufMsg, strlen((const char*)bufMsg));
 * while(i2c1IsBusy()){serI2CTask();};  //Wait till background transmission finished
 * status = i2c1GetStatus(0);           //Get status of last transmission
 * @endcode
 *
 * @param adr Address of slave to write message to
 * @param msg Byte array containing message to write
 * @param size Size of message to write
 * 
 * @return Returns 0 if successfully added message to Queue (Transmit Circular Buffer)
 *      for transmission, else return the following error message:<br>
 *      I2C_TXION_STATUS_ERR_NOSPACE = Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size" to
 *      "projdefs.h" file, and increase size of buffer. Default is 128 bytes.
 */
BYTE i2c1WriteMessage(BYTE adr, BYTE* msg, WORD size);


/**
 * Begin a transmission on I2C 1 port.
 *
 * The following example shows how to send an I2C message, and check if it has
 * successfully been added to the Transmit Queue (Circular Buffer). It does NOT
 * check the status of the transmission.
 * @code
 * BYTE status;
 * //Write a message to I2C 1 at address 0x50.
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0x80);                //Write 0x80
 * i2c1Write(0x0C);                //Write 0x0C
 * i2c1WriteString("Gday");        //Write "Gday" string
 * if (i2c1EndTransmission() != 0) //End transmission, and start sending message in background
 * {
 *     //ERROR!!!
 *     //..... Do something .....
 * }
 * @endcode
 *
 * The following example shows how to send an I2C message, and check the status
 * after it has been transmitted.
 * @code
 * BYTE status;
 * //Write a message to I2C 1 at address 0x50.
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0x80);                //Write 0x80
 * i2c1Write(0x0C);                //Write 0x0C
 * i2c1WriteString("Gday");        //Write "Gday" string
 * i2c1EndTransmission();          //End transmission, and start sending message in background
 * //Wait till transmission finished (background via interrupt), and get status
 * status = i2c1GetStatusWait(0);
 * @endcode
 *
 * @param adr The destination I2C address.
 *
 * @return A byte is returned, with the following meaning:
 * - 0: Success
 * - 1: Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size" to "projdefs.h" file,
 *      and increase size of buffer. Default is 128 bytes.
 * - 2: Other error.
 */
BYTE i2c1BeginTransmission(BYTE adr);


/**
 * End a transmission on I2C 1 port. This function writes "End Transmission" to
 * the I2C Transmit Queue (Circular Buffer). The message contained in the queue will
 * be transmitted asynchronously. Use the i2c1GetStatus() function to get the status
 * of the last transmitted message.
 *
 * The following example shows how to send an I2C message, and check if it has
 * successfully been added to the Transmit Queue (Circular Buffer). It does NOT
 * check the status of the transmission.
 * @code
 * BYTE status;
 * //Write a message to I2C 1 at address 0x50.
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0x80);                //Write 0x80
 * i2c1Write(0x0C);                //Write 0x0C
 * i2c1WriteString("Gday");        //Write "Gday" string
 * if (i2c1EndTransmission() != 0) //End transmission, and start sending message in background
 * {
 *     //ERROR!!!
 *     //..... Do something .....
 * }
 * @endcode
 *
 * The following example shows how to send an I2C message, and check the status
 * after it has been transmitted.
 * @code
 * BYTE status;
 * //Write a message to I2C 1 at address 0x50.
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0x80);                //Write 0x80
 * i2c1Write(0x0C);                //Write 0x0C
 * i2c1WriteString("Gday");        //Write "Gday" string
 * i2c1EndTransmission();          //End transmission, and start sending message in background
 * //Wait till transmission finished (background via interrupt), and get status
 * status = i2c1GetStatusWait(0);
 * @endcode
 * 
 * @return Returns 0 if successfully added message to Queue (Transmit Circular Buffer)
 *      for transmission, else return the following error message:<br>
 *      I2C_TXION_STATUS_ERR_NOSPACE = Message too long for buffer! Add "#define I2C1_TXBUF_SIZE size" to
 *      "projdefs.h" file, and increase size of buffer. Default is 128 bytes.
 */
BYTE i2c1EndTransmission(void);


/**
 * Add a byte to the I2C Transmit Queue (Circular Buffer)
 *
 * @param b Byte to add to I2C Transmit Queue
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2c1Write(BYTE b);


/**
 * Add given BYTE array to the I2C Transmit Queue (Circular Buffer)
 *
 * @param arr The byte array to add to I2C Transmit Queue
 * @param size Number of bytes to add
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2c1WriteArray(BYTE* arr, BYTE size);


/**
 * Adds given NULL terminated string to the I2C Transmit Queue (Circular Buffer).
 * The NULL terminator is NOT included (not written to buffer!)
 *
 * @param str Null terminated string to add
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2c1WriteString(const char* str);


/**
 * Adds given NULL terminated string to the I2C Transmit Queue (Circular Buffer).
 * The given string must be an ASCII String, and can contain Escape Sequences.
 *
 * For details on the format of an ASCII String with Escape Sequences,
 * see @ref nz_circularBuffer_format_ascii_esc "ASCII Format, with Escape Sequences".
 *
 * If the I2C Transmit Queue does not have enough space, a NON-Zero error code is
 * returned, and nothing is done.
 *
 * @param str Null terminated string to add, in "ASCII Format, with Escape Sequences".
 *
 * @return Returns 0 if successfully added, else a non-zero value is returned.
 */
BYTE i2c1WriteAsciiEscString(const char* str);


/**
 * Reads given number of bytes to the default Receive Queue (Circular Buffer).
 * Once bytes have been read, use the i2c1GetXxx functions to get them.
 *
 * An example of how to use this function is:
 * @code
 * BYTE c;
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0xD0);                //Write 0xD0 = address of register to read
 * i2c1Read(1);                    //Read 1 byte
 * i2c1EndTransmission();          //End transmission, and start sending message in background
 * //Wait till transmission finished (background via interrupt), and check if successful
 * if (i2c1GetStatusWait(0)==0) {
 *     //Get all available data from default Receive Circular Buffer.
 *     while (i2c1HasData()) {
 *         c = i2c1GetByte();      //Get next available byte
 *     }
 * }
 * @endcode
 *
 * @param size Number of bytes to read.
 *
 * @return Returns 0 if request successfully added to Transmission Queue, else a
 * non-zero value is returned.
 */
BYTE i2c1Read(BYTE size);


/**
 * Reads given number of bytes to the given buffer.
 *
 * An example of how to use this function is:
 * @code
 * BYTE buf[8];
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0xD0);                //Write 0xD0 = address of register to read
 * i2c1ReadToArray(buf, 2);        //Read 2 bytes to buf
 * i2c1EndTransmission();          //End transmission, and start sending message in background
 * //Wait till transmission finished (background via interrupt), and check if successful
 * if (i2c1GetStatusWait(0)==0) {
 *     //Message Successful. Read data available in buf[0] and buf[1].
 * }
 * @endcode

 *
 * @param buf Pointer to buffer to write read bytes to.
 * @param size Number of bytes to read.
 *
 * @return Returns 0 if request successfully added to Transmission Queue, else a
 * non-zero value is returned.
 */
BYTE i2c1ReadToArray(BYTE* buf, BYTE size);


/**
 * Sends a message to slave at given address, requesting the given number of bytes.
 * This function sends a whole message, including I2C START and STOP conditions.
 * It first writes the given register to the slave, and then reads requested number
 * of bytes to given buffer.
 *
 * The following example:<br>
 * @code
 * BYTE buf[2];
 * i2c1ReadSlaveReg(0x50, 0xD0, buf, 2);
 * //Wait till transmission finished (background via interrupt), and check if successful
 * if (i2c1GetStatusWait(0)==0) {
 *     //Message Successful. Read data available in buf[0] and buf[1].
 * }
 * @endcode
 *
 * Does the same as the following code:<br>
 * @code
 * BYTE buf[2];
 * i2c1BeginTransmission(0x50);    //Start transmission to slave at address 0x50
 * i2c1Write(0xD0);                //Write 0xD0 = address of register to read
 * i2c1ReadToArray(buf, 2);        //Read 2 bytes to buf
 * i2c1EndTransmission();          //End transmission, and start sending message in background
 * //Wait till transmission finished (background via interrupt), and check if successful
 * if (i2c1GetStatusWait(0)==0) {
 *     //Message Successful. Read data available in buf[0] and buf[1].
 * }
 * @endcode
 *
 * @param adr Address of slave
 * @param reg Register to write to I2C slave
 * @param buf Pointer to buffer to write read bytes to.
 * @param size Number of bytes to read.
 *
 * @return Returns 0 if request successfully added to Transmission Queue, else a
 * non-zero value is returned.
 */
BYTE i2c1ReadSlaveReg(BYTE adr, BYTE reg, BYTE* buf, BYTE size);


/*
#define i2c1EmptyTxBuf()              cbufEmpty(CIRBUF_TX_I2C1)
#define i2c1IsTxBufEmpty()            cbufIsEmpty(CIRBUF_TX_I2C1)
#define i2c1IsTxBufFull()             cbufIsFull(CIRBUF_TX_I2C1)
#define i2c1GetTxBufCount()           cbufGetCount(CIRBUF_TX_I2C1)
#define i2c1PeekTxBuf()               cbufPeekByte(CIRBUF_TX_I2C1)
#define i2c1PeekTxBufAt(offset)       cbufPeekByteAt(CIRBUF_TX_I2C1, offset)
#define i2c1TxBufRemoveByte()         cbufRemoveByte(CIRBUF_TX_I2C1)
#define i2c1TxBufRemoveBytes(n)       cbufRemoveBytes(CIRBUF_TX_I2C1, n)
*/

/**
 * Get the available space in the Transmit Queue(Circular Buffer) for adding data.
 */
#define i2c1GetTxBufFree()  cbufGetFree(CIRBUF_TX_I2C1)


/**
 * Empty the Transmit Queue(Circular Buffer). Any data contained in the buffer is lost.
 */
#define i2c1EmptyTxBuf() cbufEmpty(CIRBUF_TX_I2C1)


/**
 * Checks if the Transmit Queue(Circular Buffer)is empty.
 *
 * @return Returns true if the given buffer is empty. Else, returns false.
 */
#define i2c1IsTxBufEmpty()  cbufIsEmpty(CIRBUF_TX_I2C1)


/**
 * Checks if the given transmit buffer is full - no more space for adding data.
 *
 * @return Returns true if the given buffer is full. Else, returns false.
 */
#define i2c1IsTxBufFull()   cbufIsFull(CIRBUF_TX_I2C1)


/**
 * Returns number of bytes available in the Receive Queue(Circular Buffer) for
 * reading. They can be read with the i2c1Read... functions.
 *
 * @return Returns number of bytes available in the Receive Queue(Circular Buffer)
 * for reading.
 */
#define i2c1GetRxBufCount() cbufGetCount(CIRBUF_RX_I2C1)


/**
 * Returns TRUE if there are bytes available in the Receive Queue(Circular Buffer) for
 * reading. They can be read with the i2c1Read... functions.
 *
 * @return TRUE if there are bytes in the Receive Queue(Circular Buffer) for reading.
 */
#define i2c1HasData() cbufHasData(CIRBUF_RX_I2C1)


/**
 * Checks if the Receive Queue(Circular Buffer)is empty. If not, the contents can
 * be read with the i2c1Read... functions.
 *
 * @return Returns true if buffer is empty. Else, returns false.
 */
#define i2c1IsRxBufEmpty()  cbufIsEmpty(CIRBUF_RX_I2C1)


/**
 * Reads a single byte from the Receive Queue(Circular Buffer). They can be read
 * with the i2c1Read... functions.
 * @preCondition i2c1GetRxBufCount() or i2c1IsRxBufEmpty() must have been called to
 * confirm there is data!
 *
 * @return Returns number of bytes available in the Receive Queue(Circular Buffer)
 * for reading.
 */
#define i2c1GetByte() cbufGetByte(CIRBUF_RX_I2C1)


/**
 * Reads requested number of bytes from Receive Queue(Circular Buffer), and removes
 * them from the queue. The actual number of bytes read is returned.
 * If returned value is less than requested value, all bytes were read.
 *
 * @preCondition i2c1GetRxBufCount() or i2c1IsRxBufEmpty() must have been called to
 * confirm there is data!
 *
 * @return Returns number of bytes added to destination array.
 */
#define i2c1GetArray(pDstArr, size) cbufGetArray(CIRBUF_RX_I2C1, pDstArr, size)

#endif  //#if defined(HAS_SERPORT_I2C1)

#endif  //#if defined(HAS_SERPORT_I2C)
#endif  //#ifndef NZ_SERI2C_H
