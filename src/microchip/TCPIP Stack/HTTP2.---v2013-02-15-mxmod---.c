/*********************************************************************
 *
 *  HyperText Transfer Protocol (HTTP) Server
 *  Module for Microchip TCP/IP Stack
 *   -Serves dynamic pages to web browsers such as Microsoft Internet 
 *    Explorer, Mozilla Firefox, etc.
 *	 -Reference: RFC 2616
 *
 **********************************************************************
 * FileName:        HTTP2.c
 * Dependencies:    TCP, MPFS2, Tick, CustomHTTPApp.c callbacks
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/14/01     Original
 * Elliott Wood			6/4/07		Complete rewrite, known as HTTP2
 ********************************************************************/

#define __HTTP2_C

#include "TCPIP Stack/TCPIP.h"
#include "appConfig.h"
#include "cmd.h"
#include "nz_xFlash.h"
#include "nz_xflashDefsSbc66.h"

#if defined(STACK_USE_HTTP2_SERVER)

//MODTRONIX added next 5 lines
#if !defined(DEBUG_CONF_HTTP2)
    #define DEBUG_CONF_HTTP2        DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_HTTP2
#include "nz_debug.h"

#include "HTTPPrint.h"
#include "nz_helpers.h"

#define BYTELOW(v)      (*( (unsigned char *)(&v)))
#define BYTEHIGH(v)     (*( ((unsigned char *)(&v)) + 1))
#define BYTE0_DWORD(v)  (*( (unsigned char *)(&v) ))
#define BYTE1_DWORD(v)  (*( ((unsigned char *)(&v)) + 1))
#define BYTE2_DWORD(v)  (*( ((unsigned char *)(&v)) + 2))
#define BYTE3_DWORD(v)  (*( ((unsigned char *)(&v)) + 3))
#define WORD0_DWORD(v)  (*( (WORD *)(&v) ))
#define WORD1_DWORD(v)  (*( ((WORD *)(&v)) + 1))


/****************************************************************************
  Section:
	String Constants
  ***************************************************************************/
	static ROM BYTE HTTP_CRLF[] = "\r\n";	// New line sequence
	#define HTTP_CRLF_LEN	2				// Length of above string


/****************************************************************************
  Section:
	File and Content Type Settings
  ***************************************************************************/
	// File type extensions corresponding to HTTP_FILE_TYPE
	static ROM char * ROM httpFileExtensions[HTTP_UNKNOWN+1] =
	{
	    "txt",          // HTTP_TXT
	    "htm",          // HTTP_HTM
	    "html",         // HTTP_HTML
	    "cgi",          // HTTP_CGI
	    "xml",          // HTTP_XML
	    "css",          // HTTP_CSS
	    "gif",          // HTTP_GIF
	    "png",          // HTTP_PNG
	    "jpg",          // HTTP_JPG
	    "cla",          // HTTP_JAVA
	    "wav",          // HTTP_WAV
		"\0\0\0"		// HTTP_UNKNOWN
	};
	
	// Content-type strings corresponding to HTTP_FILE_TYPE
	static ROM char * ROM httpContentTypes[HTTP_UNKNOWN+1] =
	{
	    "text/plain",            // HTTP_TXT
	    "text/html",             // HTTP_HTM
	    "text/html",             // HTTP_HTML
	    "text/html",             // HTTP_CGI
	    "text/xml",              // HTTP_XML
	    "text/css",              // HTTP_CSS
	    "image/gif",             // HTTP_GIF
	    "image/png",             // HTTP_PNG
	    "image/jpeg",            // HTTP_JPG
	    "application/java-vm",   // HTTP_JAVA
	    "audio/x-wave",          // HTTP_WAV
		""						 // HTTP_UNKNOWN
	};
		
/****************************************************************************
  Section:
	Commands and Server Responses
  ***************************************************************************/

	// Initial response strings (Corresponding to HTTP_STATUS)
	static ROM char * ROM HTTPResponseHeaders[] =
	{
		"HTTP/1.1 200 OK\r\nConnection: close\r\n",
		"HTTP/1.1 200 OK\r\nConnection: close\r\n",
		"HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n400 Bad Request: can't handle Content-Length\r\n",
		"HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"Protected\"\r\nConnection: close\r\n\r\n401 Unauthorized: Password required\r\n",
		#if defined(HTTP_MPFS_UPLOAD)
		"HTTP/1.1 404 Not found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n404: File not found<br>Use <a href=\"/" HTTP_MPFS_UPLOAD "\">MPFS Upload</a> to program web pages\r\n",
		#else		
		"HTTP/1.1 404 Not found\r\nConnection: close\r\n\r\n404: File not found\r\n",
		#endif
		"HTTP/1.1 414 Request-URI Too Long\r\nConnection: close\r\n\r\n414 Request-URI Too Long: Buffer overflow detected\r\n",
		"HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n500 Internal Server Error: Expected data not present\r\n",
		"HTTP/1.1 501 Not Implemented\r\nConnection: close\r\n\r\n501 Not Implemented: Only GET and POST supported\r\n",
		#if defined(HTTP_MPFS_UPLOAD)
		"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body style=\"margin:100px\"><form method=post action=\"/" HTTP_MPFS_UPLOAD "\" enctype=\"multipart/form-data\"><b>MPFS Image Upload</b><p><input type=file name=i size=40> &nbsp; <input type=submit value=\"Upload\"></form></body></html>",
		"",
		"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body style=\"margin:100px\"><b>MPFS Update Successful</b><p><a href=\"/\">Site main page</a></body></html>",
		"HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body style=\"margin:100px\"><b>MPFS Image Corrupt or Wrong Version</b><p></body></html>",
		#endif
		"HTTP/1.1 302 Found\r\nConnection: close\r\nLocation: ",
		"HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n403 Forbidden: SSL Required - use HTTPS\r\n"
	};
	
/****************************************************************************
  Section:
	Header Parsing Configuration
  ***************************************************************************/
	
	// Header strings for which we'd like to parse
	static ROM char * ROM HTTPRequestHeaders[] =
	{
		"Cookie:",
		"Authorization:",
		"Content-Length:",
        "Content-Type:"
	};
	
	// Set to length of longest string above
	#define HTTP_MAX_HEADER_LEN		(15u)

	// Content-Type in header of HTTP POST message. MUST correspond to POST_CONTENT_TYPE in HTTP2.h!!
	static ROM char * HTTPPostContentTypeHeaders[] =
	{
		"multipart/form-data",                  //POST_CONTENT_TYPE_MUL_FORM_DATA
		"application/x-www-form-urlencoded"     //POST_CONTENT_TYPE_APP_X_WWW_FORM_URLENCODED
	};
    //Set to lenght of longest string above
    #define POST_CONTENT_TYPE_MAX_STRLEN    (sizeof("application/x-www-form-urlencoded"))



/****************************************************************************
  Section:
	HTTP Connection State Global Variables
  ***************************************************************************/
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata HTTP_CONNECTION_STATES
	#endif
	#if defined(HTTP_SAVE_CONTEXT_IN_PIC_RAM)
		HTTP_CONN					HTTPControlBlocks[MAX_HTTP_CONNECTIONS];
		#define HTTPLoadConn(a)		do{curHTTPID = (a);}while(0)
	#else
		HTTP_CONN curHTTP;							// Current HTTP connection state
		static void HTTPLoadConn(BYTE hHTTP);
	#endif
	HTTP_STUB httpStubs[MAX_HTTP_CONNECTIONS];	// HTTP stubs with state machine and socket
	BYTE curHTTPID;								// ID of the currently loaded HTTP_CONN
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata
	#endif

/****************************************************************************
  Section:
	Function Prototypes
  ***************************************************************************/
	static void HTTPHeaderParseLookup(BYTE i, WORD len);
	#if defined(HTTP_USE_COOKIES)
	static void HTTPHeaderParseCookie(void);
	#endif
	#if defined(HTTP_USE_AUTHENTICATION)
	static void HTTPHeaderParseAuthorization(void);
	#endif
	#if defined(HTTP_USE_POST)
	static void HTTPHeaderParseContentLength(void);
    static void HTTPHeaderParseContentType(WORD len);
	static HTTP_READ_STATUS HTTPReadTo(BYTE delim, BYTE* buf, WORD len);
	#endif
	
	static void HTTPProcess(void);
	static BOOL HTTPSendFile(void);

	#if defined(HTTP_MPFS_UPLOAD)
	static HTTP_IO_RESULT HTTPMPFSUpload(void);
	#endif

	#define mMIN(a, b)	((a<b)?a:b)
	#define smHTTP		httpStubs[curHTTPID].sm			// Access the current state machine

/*****************************************************************************
  Function:
	void HTTPInit(void)

  Summary:
	Initializes the HTTP server module.

  Description:
	Sets all HTTP sockets to the listening state, and initializes the
	state machine and file handles for each connection.  If SSL is
	enabled, opens a socket on that port as well.

  Precondition:
	TCP must already be initialized.

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
	This function is called only one during lifetime of the application.
  ***************************************************************************/
void HTTPInit(void)
{
    for(curHTTPID = 0; curHTTPID < MAX_HTTP_CONNECTIONS; curHTTPID++)
    {
		smHTTP = SM_HTTP_IDLE;
		sktHTTP = TCPOpen(0, TCP_OPEN_SERVER, HTTP_PORT, TCP_PURPOSE_HTTP_SERVER);
		#if defined(STACK_USE_SSL_SERVER)
		TCPAddSSLListener(sktHTTP, HTTPS_PORT);
		#endif
		
	    // Save the default record (just invalid file handles)
		curHTTP.file = MPFS_INVALID_HANDLE;
		curHTTP.offsets = MPFS_INVALID_HANDLE;
		#if !defined(HTTP_SAVE_CONTEXT_IN_PIC_RAM)
		{
			PTR_BASE oldPtr;

			oldPtr = MACSetWritePtr(BASE_HTTPB_ADDR + curHTTPID*sizeof(HTTP_CONN));
			MACPutArray((BYTE*)&curHTTP, sizeof(HTTP_CONN));
			MACSetWritePtr(oldPtr);
		}
		#endif
    }

	// Set curHTTPID to zero so that first call to HTTPLoadConn() doesn't write 
	// dummy data outside reserved HTTP memory.
    curHTTPID = 0;	
}


/*****************************************************************************
  Function:
	void HTTPServer(void)

  Summary:
	Performs periodic tasks for the HTTP2 module.

  Description:
	Browses through each open connection and attempts to process any
	pending operations.

  Precondition:
	HTTPInit() must already be called.

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
	This function acts as a task (similar to one in an RTOS).  It
	performs its task in a co-operative manner, and the main application
	must call this function repeatedly to ensure that all open or new
	connections are served in a timely fashion.
  ***************************************************************************/
void HTTPServer(void)
{
	BYTE conn;

	for(conn = 0; conn < MAX_HTTP_CONNECTIONS; conn++)
	{
		if(httpStubs[conn].socket == INVALID_SOCKET)
			continue;
		
		// If a socket is disconnected at any time 
		// forget about it and return to idle state.
		// Must do this here, otherwise we will wait until a new
		// connection arrives, which causes problems with Linux and with SSL
		if(TCPWasReset(httpStubs[conn].socket))
		{
            //MODTRONIX added next 3 lines
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: TCPWasReset - ");
            //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, (WORD)httpStubs[conn].socket);

			HTTPLoadConn(conn);
			smHTTP = SM_HTTP_IDLE;

			// Make sure any opened files are closed
			if(curHTTP.file != MPFS_INVALID_HANDLE)
			{
				MPFSClose(curHTTP.file);
				curHTTP.file = MPFS_INVALID_HANDLE;
			}
			if(curHTTP.offsets != MPFS_INVALID_HANDLE)
			{
				MPFSClose(curHTTP.offsets);
				curHTTP.offsets = MPFS_INVALID_HANDLE;
			}

			// Adjust FIFO sizes to half and half.  Default state must remain
			// here so that SSL handshakes, if required, can proceed
			TCPAdjustFIFOSize(sktHTTP, 1, 0, TCP_ADJUST_PRESERVE_RX);
		}
		
		// Determine if this connection is eligible for processing
		if(httpStubs[conn].sm != SM_HTTP_IDLE || TCPIsGetReady(httpStubs[conn].socket))
		{
			HTTPLoadConn(conn);
			HTTPProcess();
		}
	}
}

/*****************************************************************************
  Function:
	static void HTTPLoadConn(BYTE hHTTP)

  Summary:
	Switches the currently loaded connection for the HTTP2 module.

  Description:
	Saves the currently loaded HTTP connection back to Ethernet buffer
	RAM, then loads the selected connection into curHTTP in local RAM
	for processing.

  Precondition:
	None

  Parameters:
	hHTTP - the connection ID to load

  Returns:
  	None
  ***************************************************************************/
#if !defined(HTTP_SAVE_CONTEXT_IN_PIC_RAM)
static void HTTPLoadConn(BYTE hHTTP)
{
    WORD oldPtr;
    
    // Return if already loaded
    if(hHTTP == curHTTPID)
    	return;
    
    // Save the old one
    oldPtr = MACSetWritePtr(BASE_HTTPB_ADDR + curHTTPID*sizeof(HTTP_CONN));
	MACPutArray((BYTE*)&curHTTP, sizeof(HTTP_CONN));
	MACSetWritePtr(oldPtr);
	
	// Load the new one
    oldPtr = MACSetReadPtr(BASE_HTTPB_ADDR + hHTTP*sizeof(HTTP_CONN));
	MACGetArray((BYTE*)&curHTTP, sizeof(HTTP_CONN));
	MACSetReadPtr(oldPtr);
	
	// Remember which one is loaded
	curHTTPID = hHTTP;
			
}
#endif

/*****************************************************************************
  Function:
	static void HTTPProcess(void)

  Description:
	Performs any pending operations for the currently loaded HTTP connection.

  Precondition:
	HTTPInit() and HTTPLoadConn() have been called.

  Parameters:
	None

  Returns:
  	None
  ***************************************************************************/
static void HTTPProcess(void)
{
    WORD lenA, lenB;
	BYTE c, i;
    BOOL isDone;
	BYTE *ext;
	BYTE buffer[HTTP_MAX_HEADER_LEN+1];

    do
    {
        isDone = TRUE;

        switch(smHTTP)
        {

        case SM_HTTP_IDLE:

			// Check how much data is waiting
			lenA = TCPIsGetReady(sktHTTP);

			// If a connection has been made, then process the request
            if(lenA)
            {// Clear out state info and move to next state
				curHTTP.ptrData = curHTTP.data;
                curHTTP.ptrCookies = 0;
				smHTTP = SM_HTTP_PARSE_REQUEST;
				curHTTP.isAuthorized = 0xff;
				curHTTP.hasArgs = FALSE;
				curHTTP.callbackID = TickGet() + HTTP_TIMEOUT*TICK_SECOND;  //MODTRONIX added comment. callbackPos & callbackID seem to be used for watchdog and timeout detection
				curHTTP.callbackPos = 0xffffffff;                           //MODTRONIX added comment. callbackPos & callbackID seem to be used for watchdog and timeout detection
				curHTTP.byteCount = 0;
				#if defined(HTTP_USE_POST)
				curHTTP.smPost = 0x00;
				#endif
				
				// Adjust the TCP FIFOs for optimal reception of 
				// the next HTTP request from the browser
				TCPAdjustFIFOSize(sktHTTP, 1, 0, TCP_ADJUST_PRESERVE_RX | TCP_ADJUST_GIVE_REST_TO_RX);

                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: Conn Made");    //MODTRONIX added this line
            }
 			else
 				// Don't break for new connections.  There may be 
 				// an entire request in the buffer already.
 				break;

		case SM_HTTP_PARSE_REQUEST:

			// Verify the entire first line is in the FIFO
			if(TCPFind(sktHTTP, '\n', 0, FALSE) == 0xffff)
			{// First line isn't here yet
				if(TCPGetRxFIFOFree(sktHTTP) == 0u)
				{// If the FIFO is full, we overflowed
					curHTTP.httpStatus = HTTP_OVERFLOW;
					smHTTP = SM_HTTP_SERVE_HEADERS;
					isDone = FALSE;
                    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: FIFO Overflow1");    //MODTRONIX added this line
				}
				if((LONG)(TickGet() - curHTTP.callbackID) > (LONG)0)
				{// A timeout has occurred
					TCPDisconnect(sktHTTP);
					smHTTP = SM_HTTP_DISCONNECT;
					isDone = FALSE;
                    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: Timeout1");    //MODTRONIX added this line
				}
				break;
			}

			// Reset the watchdog timer
			curHTTP.callbackID = TickGet() + HTTP_TIMEOUT*TICK_SECOND;

			// Determine the request method
			lenA = TCPFind(sktHTTP, ' ', 0, FALSE);
			if(lenA > 5u)
				lenA = 5;
			TCPGetArray(sktHTTP, curHTTP.data, lenA+1);

		    if ( memcmppgm2ram(curHTTP.data, (ROM void*)"GET", 3) == 0)
			    curHTTP.httpStatus = HTTP_GET;
			#if defined(HTTP_USE_POST)
		    else if ( memcmppgm2ram(curHTTP.data, (ROM void*)"POST", 4) == 0)
			    curHTTP.httpStatus = HTTP_POST;
			#endif
		    else
			{// Unrecognized method, so return not implemented
		        curHTTP.httpStatus = HTTP_NOT_IMPLEMENTED;
				smHTTP = SM_HTTP_SERVE_HEADERS;
				isDone = FALSE;
                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: Unrcnzd Mthd");    //MODTRONIX added this line
				break;
			}

			// Find end of filename
			lenA = TCPFind(sktHTTP, ' ', 0, FALSE);
			lenB = TCPFindEx(sktHTTP, '?', 0, lenA, FALSE);
			lenA = mMIN(lenA, lenB);
			
			// If the file name is too long, then reject the request
			if(lenA > HTTP_MAX_DATA_LEN - HTTP_DEFAULT_LEN - 1)
			{
				curHTTP.httpStatus = HTTP_OVERFLOW;
				smHTTP = SM_HTTP_SERVE_HEADERS;
				isDone = FALSE;
                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: Filename too long");    //MODTRONIX added this line
				break;
			}

			// Read in the filename and decode
			lenB = TCPGetArray(sktHTTP, curHTTP.data, lenA);
			curHTTP.data[lenB] = '\0';
			HTTPURLDecode(curHTTP.data);

			// Decode may have changed the string length - update it here
			lenB = strlen((char*)curHTTP.data);

			// Check if this is an MPFS or Firmware Upload
			#if defined(HTTP_MPFS_UPLOAD)
			if(memcmppgm2ram(&curHTTP.data[1], HTTP_MPFS_UPLOAD, sizeof(HTTP_MPFS_UPLOAD)-1) == 0)
			{// Read remainder of line, and bypass all file opening, etc.
                //Above check checks that string starts with HTTP_MPFS_UPLOAD, NULL terminator is not checked (sizeof(HTTP_MPFS_UPLOAD)-1)
                //URL is /mpfsupload  -OR-  /mpfsuploadf
                if ( (curHTTP.data[sizeof(HTTP_MPFS_UPLOAD)] == '\0') ||
                     ((curHTTP.data[sizeof(HTTP_MPFS_UPLOAD)] == 'f') && (curHTTP.data[sizeof(HTTP_MPFS_UPLOAD)+1] == '\0')) )
                {
                    BYTE0_DWORD(curHTTP.nextCallback) = (BYTE)POST_CONTENT_TYPE_UNKNOWN;
                    #if defined(HTTP_USE_AUTHENTICATION)
                    curHTTP.isAuthorized = HTTPNeedsAuth(&curHTTP.data[1]);
                    #endif
                    if(curHTTP.httpStatus == HTTP_GET)
                        curHTTP.httpStatus = HTTP_MPFS_FORM;
                    else
                        curHTTP.httpStatus = HTTP_MPFS_UP;

                    smHTTP = SM_HTTP_PARSE_HEADERS;
                    isDone = FALSE;
                    break;
                }
			}
			#endif
			
			// If the last character is a not a directory delimiter, then try to open the file
			// String starts at 2nd character, because the first is always a '/'
			if(curHTTP.data[lenB-1] != '/')
				curHTTP.file = MPFSOpen(&curHTTP.data[1]);
				
			// If the open fails, then add our default name and try again
			if(curHTTP.file == MPFS_INVALID_HANDLE)
			{
				// Add the directory delimiter if needed
				if(curHTTP.data[lenB-1] != '/')
					curHTTP.data[lenB++] = '/';
				
				// Add our default file name			
				#if defined(STACK_USE_SSL_SERVER)
				if(TCPIsSSL(sktHTTP))
				{
					strcpypgm2ram((void*)&curHTTP.data[lenB], HTTPS_DEFAULT_FILE);
					lenB += strlenpgm(HTTPS_DEFAULT_FILE);
				}
				else
				#endif
				{
					strcpypgm2ram((void*)&curHTTP.data[lenB], HTTP_DEFAULT_FILE);
					lenB += strlenpgm(HTTP_DEFAULT_FILE);
				}
					
				// Try to open again
				curHTTP.file = MPFSOpen(&curHTTP.data[1]);
			}
			
			// Find the extension in the filename
			for(ext = curHTTP.data + lenB-1; ext != curHTTP.data; ext--)
				if(*ext == '.')
					break;
					
			// Compare to known extensions to determine Content-Type
			ext++;
			for(curHTTP.fileType = HTTP_TXT; curHTTP.fileType < HTTP_UNKNOWN; curHTTP.fileType++)
				if(!stricmppgm2ram(ext, (ROM void*)httpFileExtensions[curHTTP.fileType]))
					break;
			
			// Perform first round authentication (pass file name only)
			#if defined(HTTP_USE_AUTHENTICATION)
			curHTTP.isAuthorized = HTTPNeedsAuth(&curHTTP.data[1]);
			#endif
			
			// If the file was found, see if it has an index
			if(curHTTP.file != MPFS_INVALID_HANDLE &&
				(MPFSGetFlags(curHTTP.file) & MPFS2_FLAG_HASINDEX) )
			{
				curHTTP.offsets = MPFSOpenID(MPFSGetID(curHTTP.file) + 1);
			}

			// Read GET args, up to buffer size - 1
			lenA = TCPFind(sktHTTP, ' ', 0, FALSE);
			if(lenA != 0u)
			{
				curHTTP.hasArgs = TRUE;
				
				// Trash the '?'
				TCPGet(sktHTTP, &c);

				// Verify there's enough space
				lenA--;
				if(lenA >= HTTP_MAX_DATA_LEN - 2)
				{
			        curHTTP.httpStatus = HTTP_OVERFLOW;
					smHTTP = SM_HTTP_SERVE_HEADERS;
					isDone = FALSE;
					break;
				}

				//Read in the arguments and '&' terminate in anticipation of cookies.
                //Cookies are added to GET args in curHTTP.data buffer. This results in curHTTP.data
                //buffer having all GET name=value args, AND cookies. Cookies parsed in header in the
                //SM_HTTP_PARSE_HEADERS state below, and the HTTPHeaderParseLookup() function will call
                //the HTTPHeaderParseCookie() function that will add all found cookies.
				curHTTP.ptrData += TCPGetArray(sktHTTP, curHTTP.data, lenA);
				*(curHTTP.ptrData++) = '&';
			}

			// Clear the rest of the line
			lenA = TCPFind(sktHTTP, '\n', 0, FALSE);
			TCPGetArray(sktHTTP, NULL, lenA + 1);

			// Move to parsing the headers
			smHTTP = SM_HTTP_PARSE_HEADERS;
			
			// No break, continue to parsing headers

		case SM_HTTP_PARSE_HEADERS:
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: Parsing Hdrs");    //MODTRONIX added this line
			// Loop over all the headers
			while(1)
			{
				// Make sure entire line is in the FIFO. Each line ends with "\r\n"
				lenA = TCPFind(sktHTTP, '\n', 0, FALSE);
				if(lenA == 0xffff)
				{// If not, make sure we can receive more data
					if(TCPGetRxFIFOFree(sktHTTP) == 0u)
					{// Overflow
						curHTTP.httpStatus = HTTP_OVERFLOW;
						smHTTP = SM_HTTP_SERVE_HEADERS;
						isDone = FALSE;
                        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: FIFO Overflow2");    //MODTRONIX added this line
					}
					if((LONG)(TickGet() - curHTTP.callbackID) > (LONG)0)
					{// A timeout has occurred
						TCPDisconnect(sktHTTP);
						smHTTP = SM_HTTP_DISCONNECT;
						isDone = FALSE;
                        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: Timeout2");    //MODTRONIX added this line
					}
					break;
				}
				
				// Reset the watchdog timer
				curHTTP.callbackID = TickGet() + HTTP_TIMEOUT*TICK_SECOND;
				
				// If a CRLF is immediate, then headers are done
				if(lenA == 1u)
				{// Remove the CRLF and move to next state
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: Fnd CRLF, Hdr Done");    //MODTRONIX added this line
					TCPGetArray(sktHTTP, NULL, 2);
					smHTTP = SM_HTTP_AUTHENTICATE;
					isDone = FALSE;
					break;
				}
	
				// Find the header name, and use isDone as a flag to indicate a match. +2 so
				lenB = TCPFindEx(sktHTTP, ':', 0, lenA, FALSE) + 2;
				isDone = FALSE;
	
				// If name is too long or this line isn't a header, ignore it
				if(lenB > sizeof(buffer))
				{
                    TCPGetArray(sktHTTP, NULL, lenA+1);
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: Hdr Ignored");    //MODTRONIX added this line
					continue;
				}
				
				// Read in the header name
				TCPGetArray(sktHTTP, buffer, lenB);
				buffer[lenB-1] = '\0';
				lenA -= lenB;

                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHdr=");              //MODTRONIX added this line
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (const char*)buffer);   //MODTRONIX added this line
		
				// Compare header read to ones we're interested in: Cookie:, Authorization:, Content-Length:, Content-Type:
				for(i = 0; i < sizeof(HTTPRequestHeaders)/sizeof(HTTPRequestHeaders[0]); i++)
				{
					if(strcmppgm2ram((char*)buffer, (ROM char *)HTTPRequestHeaders[i]) == 0)
					{// Parse the header and stop the loop
						HTTPHeaderParseLookup(i, lenA);
						isDone = TRUE;
						break;
					}
				}

				// Clear the rest of the line, and call the loop again
				if(isDone)
				{// We already know how much to remove unless a header was found
					lenA = TCPFind(sktHTTP, '\n', 0, FALSE);
				}
				TCPGetArray(sktHTTP, NULL, lenA+1);
			}
			
			break;

		case SM_HTTP_AUTHENTICATE:
		
			#if defined(HTTP_USE_AUTHENTICATION)
			// Check current authorization state
			if(curHTTP.isAuthorized < 0x80)
			{// 401 error
                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: Unauthorized");    //MODTRONIX added this line
				curHTTP.httpStatus = HTTP_UNAUTHORIZED;
				smHTTP = SM_HTTP_SERVE_HEADERS;
				isDone = FALSE;
				
				#if defined(HTTP_NO_AUTH_WITHOUT_SSL)
				if(!TCPIsSSL(sktHTTP))
					curHTTP.httpStatus = HTTP_SSL_REQUIRED;
				#endif

				break;
			}
			#endif

			// Parse the args string
			*curHTTP.ptrData = '\0';
			curHTTP.ptrData = HTTPURLDecode(curHTTP.data);  //Decode curHTTP.data, and update curHTTP.ptrData with NEW first free byte

			// If this is an MPFS upload form request, bypass to headers
			#if defined(HTTP_MPFS_UPLOAD)
			if(curHTTP.httpStatus == HTTP_MPFS_FORM)
			{
				smHTTP = SM_HTTP_SERVE_HEADERS;
				isDone = FALSE;
				break;
			}
			#endif
			
			// Move on to GET args, unless there are none
			smHTTP = SM_HTTP_PROCESS_GET;
			if(!curHTTP.hasArgs)
				smHTTP = SM_HTTP_PROCESS_POST;
			isDone = FALSE;
			curHTTP.hasArgs = FALSE;
			break;

		case SM_HTTP_PROCESS_GET:
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: GET");    //MODTRONIX added this line
			// Run the application callback HTTPExecuteGet()
			if(HTTPExecuteGet() == HTTP_IO_WAITING)
			{// If waiting for asynchronous process, return to main app
				break;
			}

            //At this stage, we have processed all GET name-value pairs contained in curHTTP.data. Was
            //done in HTTPExecuteGet().
            //1) Following the GET name-value pairs are possible received Cookie name-value pairs, that have
            //   NOT been processed yet! They can possibly be requested via tags. They start at curHTTP.ptrCookies
            //2) ALSO, if curHTTP.hasArgs!=0, curHTTP.ptrData will contain Cookie name-value pairs that must
            //   be sent to client via "Set-Cookie" header.
            //The curHTTP.ptrData pointer points to first free byte in curHTTP.data. To get more free space,
            //we remove all GET name-value pairs, so only the Cookie name-value pairs remain!
            //Remove GET name-value pairs if there is cookie data AND if there is NOT "Set-Cookie" header data
            if((curHTTP.ptrCookies != 0) && (curHTTP.hasArgs==0)) {
                BYTE* ptrCkEnd = curHTTP.ptrCookies;
                //Search for end of cookies
                while(*ptrCkEnd != '\0')
                {
                    // Skip past two strings (NUL bytes)
                    ptrCkEnd += strlen((char*)ptrCkEnd) + 1;
                    ptrCkEnd += strlen((char*)ptrCkEnd) + 1;
                }
                //Save length of cookie data, including second NULL, in ptrCkEnd
                *((WORD*)&ptrCkEnd) = ((WORD)ptrCkEnd) - ((WORD)curHTTP.ptrCookies) + 1;   //Add 1 to increment past second NULL

                nzMemCpy(curHTTP.data, curHTTP.ptrCookies, (WORD)ptrCkEnd);

                //Update first available byte in curHTTP.data
                curHTTP.ptrData = curHTTP.data + ((WORD)ptrCkEnd);
            }

			// Move on to POST data
			smHTTP = SM_HTTP_PROCESS_POST;

		case SM_HTTP_PROCESS_POST:
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: POST");    //MODTRONIX added this line
			#if defined(HTTP_USE_POST)
			
			// See if we have any new data
			if(TCPIsGetReady(sktHTTP) == curHTTP.callbackPos)   //MODTRONIX added comment. callbackPos & callbackID seem to be used for watchdog and timeout detection
			{
				if((LONG)(TickGet() - curHTTP.callbackID) > (LONG)0)
				{// If a timeout has occurred, disconnect
					TCPDisconnect(sktHTTP);
					smHTTP = SM_HTTP_DISCONNECT;
					isDone = FALSE;
                    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: Timeout3");    //MODTRONIX added this line
					break;
				}
			}
			
			if(curHTTP.httpStatus == HTTP_POST 
				#if defined(HTTP_MPFS_UPLOAD)
				|| (curHTTP.httpStatus >= HTTP_MPFS_UP && curHTTP.httpStatus <= HTTP_MPFS_ERROR)
				#endif
				 )
			{
				// Run the application callback HTTPExecutePost()
				#if defined(HTTP_MPFS_UPLOAD)
				if(curHTTP.httpStatus >= HTTP_MPFS_UP && curHTTP.httpStatus <= HTTP_MPFS_ERROR)
				{
					c = HTTPMPFSUpload();
					if(c == (BYTE)HTTP_IO_DONE)
					{
						smHTTP = SM_HTTP_SERVE_HEADERS;
						isDone = FALSE;
						break;
					}
				}
				else
				#endif
                    c = HTTPExecutePost();
				
				// If waiting for asynchronous process, return to main app
				if(c == (BYTE)HTTP_IO_WAITING)
				{// return to main app and make sure we don't get stuck by the watchdog
					curHTTP.callbackPos = TCPIsGetReady(sktHTTP) - 1;   //MODTRONIX added comment. Store how many bytes are ready to read. Later check if this does NOT change = timeout
					break;
				}
				else if(c == (BYTE)HTTP_IO_NEED_DATA)
				{// If waiting for more data
					curHTTP.callbackPos = TCPIsGetReady(sktHTTP);               //MODTRONIX added comment. callbackPos & callbackID seem to be used for watchdog and timeout detection
					curHTTP.callbackID = TickGet() + HTTP_TIMEOUT*TICK_SECOND;
					
					// If more is expected and space is available, return to main app
					if(curHTTP.byteCount > curHTTP.callbackPos && TCPGetRxFIFOFree(sktHTTP) != 0u)
						break;

                    //MODTRONIX added, for this case it seems to incorrectly not break
                    if (curHTTP.httpStatus == HTTP_MPFS_ERROR)
                        break;
					
					// Handle cases where application ran out of data or buffer space
					curHTTP.httpStatus = HTTP_INTERNAL_SERVER_ERROR;
					smHTTP = SM_HTTP_SERVE_HEADERS;
					isDone = FALSE;
                    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: No Buffer Space left."); //MODTRONIX added this line
                    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, " ByteCount=");                    //MODTRONIX added this line
                    DEBUG_PUT_WORD(DEBUG_LEVEL_ERROR, curHTTP.byteCount);               //MODTRONIX added this line
                    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, " callbackPos=");                  //MODTRONIX added this line
                    DEBUG_PUT_WORD(DEBUG_LEVEL_ERROR, curHTTP.callbackPos);             //MODTRONIX added this line
					break;	
				}
			}
			#endif

			// We're done with POST
			smHTTP = SM_HTTP_PROCESS_REQUEST;
			// No break, continue to sending request

		case SM_HTTP_PROCESS_REQUEST:

			// Check for 404
            if(curHTTP.file == MPFS_INVALID_HANDLE)
            {
                curHTTP.httpStatus = HTTP_NOT_FOUND;
                smHTTP = SM_HTTP_SERVE_HEADERS;
                isDone = FALSE;
                break;
            }

			// Set up the dynamic substitutions
			curHTTP.byteCount = 0;
			if(curHTTP.offsets == MPFS_INVALID_HANDLE)
            {// If no index file, then set next offset to huge
	            curHTTP.nextCallback = 0xffffffff;
            }
            else
            {// Read in the next callback index
	            MPFSGetLong(curHTTP.offsets, &(curHTTP.nextCallback));
			}
			
			// Move to next state
			smHTTP = SM_HTTP_SERVE_HEADERS;

		case SM_HTTP_SERVE_HEADERS:

			// We're in write mode now:
			// Adjust the TCP FIFOs for optimal transmission of 
			// the HTTP response to the browser
			TCPAdjustFIFOSize(sktHTTP, 1, 0, TCP_ADJUST_GIVE_REST_TO_TX);
				
			// Send headers
			TCPPutROMString(sktHTTP, (ROM BYTE*)HTTPResponseHeaders[curHTTP.httpStatus]);
			
			// If this is a redirect, print the rest of the Location: header			   
			if(curHTTP.httpStatus == HTTP_REDIRECT)
			{
				TCPPutString(sktHTTP, curHTTP.data);
				TCPPutROMString(sktHTTP, (ROM BYTE*)"\r\n\r\n304 Redirect: ");
				TCPPutString(sktHTTP, curHTTP.data);
				TCPPutROMString(sktHTTP, (ROM BYTE*)HTTP_CRLF);
			}

			// If not GET or POST, we're done - This will be the case for HTTP_MPFS_UPLOAD!
			if(curHTTP.httpStatus != HTTP_GET && curHTTP.httpStatus != HTTP_POST)
			{// Disconnect
				smHTTP = SM_HTTP_DISCONNECT;
				break;
			}

			// Output the content type, if known
			if(curHTTP.fileType != HTTP_UNKNOWN)
			{
				TCPPutROMString(sktHTTP, (ROM BYTE*)"Content-Type: ");
				TCPPutROMString(sktHTTP, (ROM BYTE*)httpContentTypes[curHTTP.fileType]);
				TCPPutROMString(sktHTTP, HTTP_CRLF);
			}
			
			// Output the gzip encoding header if needed
			if(MPFSGetFlags(curHTTP.file) & MPFS2_FLAG_ISZIPPED)
			{
				TCPPutROMString(sktHTTP, (ROM BYTE*)"Content-Encoding: gzip\r\n");
			}
						
			// Output the cache-control
			TCPPutROMString(sktHTTP, (ROM BYTE*)"Cache-Control: ");
			if(curHTTP.httpStatus == HTTP_POST || curHTTP.nextCallback != 0xffffffff)
			{// This is a dynamic page or a POST request, so no cache
				TCPPutROMString(sktHTTP, (ROM BYTE*)"no-cache");
			}
			else
			{// This is a static page, so save it for the specified amount of time
				TCPPutROMString(sktHTTP, (ROM BYTE*)"max-age=");
				TCPPutROMString(sktHTTP, (ROM BYTE*)HTTP_CACHE_LEN);
			}
			TCPPutROMString(sktHTTP, HTTP_CRLF);
			
			// Check if we should output cookies
            //MODTRONIX added comment: In GET or POST processing, cookies could have been added as name-value pairs
            //to curHTTP.data. The number of cookes added is given by curHTTP.hasArgs. The code below will send a
            //"Set-Cookie" header to the browser for each one. From then on, the browser will include the cookies in
            //the header of each request sent to us.
            //Don't confuse this with Cookies that were received via HTTP request, and added as name-value pairs
            //along with GET name value-pairs! For them, curHTTP.hasArgs will be 0.
            //What we are seaching for here, are Cookies we have to SEND to the client, and instruct it to set this
            //Cookie.
			if(curHTTP.hasArgs)
				smHTTP = SM_HTTP_SERVE_COOKIES;
			else
			{// Terminate the headers
				TCPPutROMString(sktHTTP, HTTP_CRLF);
				smHTTP = SM_HTTP_SERVE_BODY;
			}
	
			// Move to next stage
			isDone = FALSE;
			break;

		case SM_HTTP_SERVE_COOKIES:

			#if defined(HTTP_USE_COOKIES)
			// If the TX FIFO runs out of space, the client will never get CRLFCRLF
			// Avoid writing huge cookies - keep it under a hundred bytes max

			// Write cookies one at a time as space permits
			for(curHTTP.ptrRead = curHTTP.data; curHTTP.hasArgs != 0u; curHTTP.hasArgs--)
			{
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCkySet:");    //MODTRONIX added this line
				// Write the header
				TCPPutROMString(sktHTTP, (ROM BYTE*)"Set-Cookie: ");

				// Write the name, URL encoded, one character at a time
				while((c = *(curHTTP.ptrRead++)))
				{
					if(c == ' ')
						TCPPut(sktHTTP, '+');
					else if(c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
					{
						TCPPut(sktHTTP, '%');
						TCPPut(sktHTTP, btohexa_high(c));
						TCPPut(sktHTTP, btohexa_low(c));
					}
					else {
						TCPPut(sktHTTP, c);
                        //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, c);    //MODTRONIX added this line
                    }
				}

				TCPPut(sktHTTP, '=');
                //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '=');    //MODTRONIX added this line
				
				// Write the value, URL encoded, one character at a time
				while((c = *(curHTTP.ptrRead++)))
				{
					if(c == ' ')
						TCPPut(sktHTTP, '+');
					else if(c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
					{
						TCPPut(sktHTTP, '%');
						TCPPut(sktHTTP, btohexa_high(c));
						TCPPut(sktHTTP, btohexa_low(c));
					}
					else {
						TCPPut(sktHTTP, c);
                        //DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, c);    //MODTRONIX added this line
                    }
				}
				
				// Finish the line
				TCPPutROMString(sktHTTP, HTTP_CRLF);

			}
			#endif

			// We're done, move to next state
			TCPPutROMString(sktHTTP, HTTP_CRLF);
			smHTTP = SM_HTTP_SERVE_BODY;

		case SM_HTTP_SERVE_BODY:
            //MODTRONIX added comment: At this stage, curHTTP.data will contain name-value pairs for received GET
            //data and Cookies. These are Cookies that were present in request header.
            //If curHTTP.hasArgs!=0, curHTTP.data will contain Cookies we have to sent to client via "Set-Cookie" header.
            //This is DIFFERENT from the cookies that will be in curHTTP.data if curHTTP.hasArgs=0!

			isDone = FALSE;

			// Try to send next packet
			if(HTTPSendFile())
			{// If EOF, then we're done so close and disconnect
				MPFSClose(curHTTP.file);
				curHTTP.file = MPFS_INVALID_HANDLE;
				smHTTP = SM_HTTP_DISCONNECT;
				isDone = TRUE;
                DelayMs(2);     //MODTRONIX added. Browsers seem to miss packets if they are sent too fast. Ensure at least 1ms
			}
			
			// If the TX FIFO is full, then return to main app loop
			if(TCPIsPutReady(sktHTTP) == 0u) {
                DelayMs(2);     //MODTRONIX added. Browsers seem to miss packets if they are sent too fast. Ensure at least 1ms delay between packets when large file is sent.
				isDone = TRUE;
            }
            break;

		case SM_HTTP_SEND_FROM_CALLBACK:
            //MODTRONIX added comment: At this stage:
            // - curHTTP.ptrData[0] will contain tag length
            // - curHTTP.ptrData[1] = NULL terminated tag
            //This data (curHTTP.ptrData) was placed there in HTTPSendFile() function
			isDone = TRUE;

			// Check that at least the minimum bytes are free
			if(TCPIsPutReady(sktHTTP) < HTTP_MIN_CALLBACK_FREE)
				break;

            //MODTRONIX added code below from here
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nTag=");                          //MODTRONIX added this line
            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (const char*)&curHTTP.ptrData[1]);  //MODTRONIX added this line
            if (curHTTP.ptrData[1]=='#') {
                //This is a Modtronix tag
                curHTTP.callbackPos = cmdGetTag(&curHTTP.ptrData[1], curHTTP.callbackPos, NULL, 0, 0);
            }
            else {
            //MODTRONIX end
                // Fill TX FIFO from callback
                HTTPPrint(curHTTP.callbackID);
            }   //MODTRONIX added this line
			
			if(curHTTP.callbackPos == 0u)
			{// Callback finished its output, so move on
				isDone = FALSE;
				smHTTP = SM_HTTP_SERVE_BODY;
			}// Otherwise, callback needs more buffer space, so return and wait
			
			break;

		case SM_HTTP_DISCONNECT:
			// Make sure any opened files are closed
			if(curHTTP.file != MPFS_INVALID_HANDLE)
			{
				MPFSClose(curHTTP.file);
				curHTTP.file = MPFS_INVALID_HANDLE;
			}
			if(curHTTP.offsets != MPFS_INVALID_HANDLE)
			{
				MPFSClose(curHTTP.offsets);
				curHTTP.offsets = MPFS_INVALID_HANDLE;
			}

			TCPDisconnect(sktHTTP);
            smHTTP = SM_HTTP_IDLE;
            break;
		}
	} while(!isDone);

}


/*****************************************************************************
  Function:
	static BOOL HTTPSendFile(void)

  Description:
	Serves up the next chunk of curHTTP's file, up to a) available TX FIFO
	space or b) the next callback index, whichever comes first.

  Precondition:
	curHTTP.file and curHTTP.offsets have both been opened for reading.

  Parameters:
	None

  Return Values:
	TRUE - the end of the file was reached and reading is done
	FALSE - more data remains to be read
  ***************************************************************************/
static BOOL HTTPSendFile(void)
{
	WORD numBytes, len;
	BYTE c, data[64];
    
    //Store the tag in first free space in curHTTP.data, as given by curHTTP.ptrData
    BYTE* ptrTag = curHTTP.ptrData;

	// Determine how many bytes we can read right now
	len = TCPIsPutReady(sktHTTP);
	numBytes = mMIN(len, curHTTP.nextCallback - curHTTP.byteCount);
	
	// Get/put as many bytes as possible
	curHTTP.byteCount += numBytes;
	while(numBytes > 0u)
	{
		len = MPFSGetArray(curHTTP.file, data, mMIN(numBytes, sizeof(data)));
		if(len == 0u)
			return TRUE;
		else
			TCPPutArray(sktHTTP, data, len);
		numBytes -= len;
	}

	// Check if a callback index was reached
	if(curHTTP.byteCount == curHTTP.nextCallback)
	{
		// Update the state machine
		smHTTP = SM_HTTP_SEND_FROM_CALLBACK;
		curHTTP.callbackPos = 0;

        //If NOT at least 2 bytes available for tag, ERROR!! Mark len=0 so no data overwrite occurs!
        len = 1;
        if ((WORD)ptrTag >= ((WORD)(curHTTP.data + HTTP_MAX_DATA_LEN - 1))) {
            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nNo space for Tag!");
            len = 0;    //Mark error, no space!
        }

		// Read past the variable name and close the MPFS
		MPFSGet(curHTTP.file, NULL);

        //Save tag in prtTag[]
		do
		{
			if(!MPFSGet(curHTTP.file, &c))
				break;
			curHTTP.byteCount++;

            //MODTRONIX added next 3 lines
            if (c != '~') {
                //Save next byte of tag if these is enough space (-1 for null termination)
                if (((WORD)ptrTag+len) < ((WORD)(curHTTP.data + HTTP_MAX_DATA_LEN - 1))) {
                    ptrTag[len++] = c;
                }
            }
		} while(c != '~');
		curHTTP.byteCount++;

        if (len != 0) {         //Only do if there is space, len is marked with 0 when no space!
            ptrTag[0] = len-1;  //Store tag string length at ptrTag[0]
            ptrTag[len] = 0;    //NULL terminate tag string
        }
		
		// Read in the callback address and next offset
		MPFSGetLong(curHTTP.offsets, &(curHTTP.callbackID));
		if(!MPFSGetLong(curHTTP.offsets, &(curHTTP.nextCallback)))
		{
			curHTTP.nextCallback = 0xffffffff;
			MPFSClose(curHTTP.offsets);
			curHTTP.offsets = MPFS_INVALID_HANDLE;
		}
	}

    // We are not done sending a file yet...
    return FALSE;
}

/*****************************************************************************
  Function:
	static void HTTPHeaderParseLookup(BYTE i, WORD len)

  Description:
	Calls the appropriate header parser based on the index of the header
	that was read from the request.
    Header has been removed from TCP input. Next byte is first byte after header.

  Precondition:
	None

  Parameters:
	i - the index of the string found in HTTPRequestHeaders
    len - The length of the line contained in the TCP buffer. Is the length up to (but not including) the '/n' character

  Return Values:
	TRUE - the end of the file was reached and reading is done
	FALSE - more data remains to be read
  ***************************************************************************/
static void HTTPHeaderParseLookup(BYTE i, WORD len)
{
    //===== At this stage we know that whole line of header data is contained in TCP buffer =====
    //===== Actual header has been removed, and "Header Value" is now in TCP buffer =====

	// i corresponds to an index in HTTPRequestHeaders
	
	#if defined(HTTP_USE_COOKIES)
	if(i == 0u)
	{
		HTTPHeaderParseCookie();
		return;
	}
	#endif
	
	#if defined(HTTP_USE_AUTHENTICATION)	
	if(i == 1u)
	{
		HTTPHeaderParseAuthorization();
		return;
	}
	#endif
	
	#if defined(HTTP_USE_POST)
	if(i == 2u)
	{
		HTTPHeaderParseContentLength();
		return;
	}
	if(i == 3u)
	{
		HTTPHeaderParseContentType(len);
		return;
	}
	#endif
}

/*****************************************************************************
  Function:
	static void HTTPHeaderParseAuthorization(void)

  Summary:
	Parses the "Authorization:" header for a request and verifies the
	credentials.

  Description:
	Parses the "Authorization:" header for a request.  For example, 
	"BASIC YWRtaW46cGFzc3dvcmQ=" is decoded to a user name of "admin" and
	a password of "password".  Once read, HTTPCheckAuth is called from
	CustomHTTPApp.c to determine if the credentials are acceptable.

	The return value of HTTPCheckAuth is saved in curHTTP.isAuthorized for
	later use by the application.

  Precondition:
	None

  Parameters:
	None

  Returns:
	None

  Remarks:
	This function is ony available when HTTP_USE_AUTHENTICATION is defined.
  ***************************************************************************/
#if defined(HTTP_USE_AUTHENTICATION)
static void HTTPHeaderParseAuthorization(void)
{
    WORD len;
    BYTE buf[40];
	BYTE *ptrBuf;

    //===== At this stage we know that whole line of header data is contained in TCP buffer =====

	// If auth processing is not required, return
	if(curHTTP.isAuthorized & 0x80)
		return;

	// Clear the auth type ("BASIC ")
	TCPGetArray(sktHTTP, NULL, 6);

	// Find the terminating CRLF and make sure it's a multiple of four
	len = TCPFindROMArray(sktHTTP, HTTP_CRLF, HTTP_CRLF_LEN, 0, FALSE);
	len += 3;
	len &= 0xfc;
	len = mMIN(len, sizeof(buf)-4);
	
	// Read in 4 bytes at a time and decode (slower, but saves RAM)
	for(ptrBuf = buf; len > 0u; len-=4, ptrBuf+=3)
	{
		TCPGetArray(sktHTTP, ptrBuf, 4);
		Base64Decode(ptrBuf, 4, ptrBuf, 3);
	}

	// Null terminate both, and make sure there's at least two terminators
	*ptrBuf = '\0';
	for(len = 0, ptrBuf = buf; len < sizeof(buf); len++, ptrBuf++)
		if(*ptrBuf == ':')
			break;
	*(ptrBuf++) = '\0';
	
	// Verify credentials
	curHTTP.isAuthorized = HTTPCheckAuth(buf, ptrBuf);

	return;
}
#endif

/*****************************************************************************
  Function:
	static void HTTPHeaderParseCookie(void)

  Summary:
	Parses the "Cookie:" headers for a request and stores them as GET
	variables.

  Description:
	Parses the "Cookie:" headers for a request.  For example, 
 	"Cookie: name=Wile+E.+Coyote; order=ROCKET_LAUNCHER" is decoded to 
	"name=Wile+E.+Coyote&order=ROCKET_LAUNCHER&" and stored as any other 
	GET variable in curHTTP.data.

	The user application can easily access these values later using the
	HTTPGetArg() and HTTPGetROMArg() functions.

  Precondition:
	None

  Parameters:
	None

  Returns:
	None

  Remarks:
	This function is ony available when HTTP_USE_COOKIES is defined.
  ***************************************************************************/
#if defined(HTTP_USE_COOKIES)
static void HTTPHeaderParseCookie(void)
{
	WORD lenA, lenB;

    //MODTRONIX added next 4 lines
    //At this stage we know that whole line of header data is contained in TCP buffer.
    //At this stage the curHTTP.data buffer will have all GET name=value args.
    //This function will add all cookies to the curHTTP.data as additional name=value
    //arguments. Save location of first cookie in curHTTP.ptrCookie.

	// Verify there's enough space
	lenB = TCPFindROMArray(sktHTTP, HTTP_CRLF, HTTP_CRLF_LEN, 0, FALSE);
	if(lenB >= (WORD)(curHTTP.data + HTTP_MAX_DATA_LEN - curHTTP.ptrData - 2))
	{// If not, overflow
		curHTTP.httpStatus = HTTP_OVERFLOW;
		smHTTP = SM_HTTP_SERVE_HEADERS;
        DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: FIFO Overflow3");    //MODTRONIX added this line
		return;
	}

	// While a CRLF is not immediate, grab a cookie value
	while(lenB != 0u)
	{
        //Is this the first cookie? Add marker
        if (curHTTP.ptrCookies==0) {
            curHTTP.ptrCookies = curHTTP.ptrData;
        }

		// Look for a ';' and use the shorter of that or a CRLF
		lenA = TCPFind(sktHTTP, ';', 0, FALSE);

		// Read to the terminator
		curHTTP.ptrData += TCPGetArray(sktHTTP, curHTTP.ptrData, mMIN(lenA, lenB));

		// Insert an & to anticipate another cookie
		*(curHTTP.ptrData++) = '&';
		
		// If semicolon, trash it and whitespace
		if(lenA < lenB)
		{
			TCPGet(sktHTTP, NULL);
			while(TCPFind(sktHTTP, ' ', 0, FALSE) == 0u)
				TCPGet(sktHTTP, NULL);
		}
		
		// Find the new distance to the CRLF
		lenB = TCPFindROMArray(sktHTTP, HTTP_CRLF, HTTP_CRLF_LEN, 0, FALSE);
	}

	return;

}
#endif

/*****************************************************************************
  Function:
	static void HTTPHeaderParseContentLength(void)

  Summary:
	Parses the "Content-Length:" header for a request.

  Description:
	Parses the "Content-Length:" header to determine how many bytes of
	POST data to expect after the request.  This value is stored in 
	curHTTP.byteCount.

  Precondition:
	None

  Parameters:
	None

  Returns:
	None

  Remarks:
	This function is ony available when HTTP_USE_POST is defined.
  ***************************************************************************/
#if defined(HTTP_USE_POST)
static void HTTPHeaderParseContentLength(void)
{
	WORD len;
	BYTE buf[10];

    //===== At this stage we know that whole line of header data is contained in TCP buffer =====

	// Read up to the CRLF (max 9 bytes or ~1GB)
	len = TCPFindROMArray(sktHTTP, HTTP_CRLF, HTTP_CRLF_LEN, 0, FALSE);
	if(len >= sizeof(buf))
	{
		curHTTP.httpStatus = HTTP_BAD_REQUEST;
		curHTTP.byteCount = 0;
		return;
	}	
	len = TCPGetArray(sktHTTP, buf, len);
	buf[len] = '\0';
	
	curHTTP.byteCount = atol((char*)buf);
}
#endif

/*****************************************************************************
  Function:
	static void HTTPHeaderParseContentType(WORD len)

  Summary:
	Parses the "Content-Type:" header for a request.

  Description:
	Parses the "Content-Type:" header to determine the type of data sent by client
    in the POST data (after the request). Standard browsers and Microchip MPFS2 tool
    seem to always use the "multipart/form-data".
    A problem arrises with wget for example that sends a file using the
    "application/x-www-form-urlencoded" content type. With this type, the file follows
    immediately after the header. With the "multipart/form-data" type, after the
    header, another block of data follows that is terminated by a CRLFCRLF (/n/r/n/r),
    and only after this the file follows.
    This value is stored in lower byte of curHTTP.nextCallback

    The "multipart/form-data" header has a "boundary" required parameter. This boundary is
    used to seperate parts of the message. For details, see:
    http://www.w3.org/Protocols/rfc2616/rfc2616-sec19.html#sec19.2
    We do not have space to save this whole boundary string, and do not need it. We just
    save it's length in the second (offset 1) byte of curHTTP.nextCallback


  Precondition:
	None

  Parameters:
    len - The length of the line contained in the TCP buffer. Is the length up to (but not including) the '/n' character
 
  Returns:
	None

  Remarks:
	This function is ony available when HTTP_USE_POST is defined.
***************************************************************************/
#if defined(HTTP_USE_POST)
static void HTTPHeaderParseContentType(WORD len)
{
    BYTE i;
    WORD lenRead;

    //BYTE buf[POST_CONTENT_TYPE_MAX_STRLEN];
    //This line causes XC16 compiler to crash! Make it static till compiler fixed
    //unsigned char buf[64];
    static unsigned char buf[64];

    char * ptr;

    //===== At this stage we know that whole line of header data is contained in TCP buffer =====
    //===== Header "Name" has been removed, and "Value" is now in TCP buffer =====

    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: HTTPHeaderParseContentType()");

    lenRead = min(len, sizeof(buf)-1);

    //Read next batch of bytes until buffer is full
	lenRead = TCPGetArray(sktHTTP, buf, lenRead);
	buf[lenRead] = '\0';

    //Remove any remaining bytes
    if (len > lenRead) {
        TCPGetArray(sktHTTP, NULL, len - lenRead);
    }

    //Remove any remaining bytes
    //if (len > lenRead) {
    //    TCPGetArray(sktHTTP, NULL, lenLine - lenRead);
    //}

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nContent-Type = ");
    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (const char*)buf);

    // Compare 'Content-Type' to supported types
    for(i = 0; i < sizeof(HTTPPostContentTypeHeaders)/sizeof(HTTPPostContentTypeHeaders[0]); i++)
    {
        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nStrlen = ");
        //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, strlen(HTTPPostContentTypeHeaders[i]));

        if(memcmppgm2ram((char*)buf, (ROM char *)HTTPPostContentTypeHeaders[i], strlen(HTTPPostContentTypeHeaders[i])) == 0)
        //if(strcmppgm2ram((char*)buf, (ROM char *)HTTPPostContentTypeHeaders[i]) == 0)
        {
            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nContent-Type Match!");
            //Use lower byte of nextCallback to save Content-Type found
            BYTE0_DWORD(curHTTP.nextCallback) = (BYTE)i;

            //If this is "multipart/form-Data" content type, then a boundary string is to follow. Save it's length
            if (i == POST_CONTENT_TYPE_MUL_FORM_DATA) {
                //Get location of '='. The "multipart/form-Data" string will be followed by a "boundary=" string, something like:
                //"multipart/form-Data; boundary=-----------abcdefghijkl\r\n"

                ptr = strchr(((char*)&buf[0]) + strlen(HTTPPostContentTypeHeaders[i]), '=');
                
                //If '=' was found
                if (ptr != 0) {
                    //Get lenght of boundary string. (ptr-buf) is number of characters up till '='. Use -2
                    //to compensate for \r\n at end of string
                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nBdryLen = ");
                    DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, len - ( ((BYTE*)ptr)-buf) - 2);

                    BYTE1_DWORD(curHTTP.nextCallback) = (BYTE)(len - ( ((BYTE*)ptr)-buf) - 2);
                }
                /*
                lenA = TCPFind(sktHTTP, ' ', 0, FALSE);
                lenB = TCPFindEx(sktHTTP, '?', 0, lenA, FALSE);
                lenA = mMIN(lenA, lenB);
                */
            }
            break;
        }
    }
}
#endif


/*****************************************************************************
  Function:
	BYTE* HTTPURLDecode(BYTE* cData)

  Summary:
	Parses a string from URL encoding to plain-text.

  Description:
	Parses a string from URL encoding to plain-text.  The following
	conversions are made: ?=? to ?\0?, ?&? to ?\0?, ?+? to ? ?, and
	?%xx? to a single hex byte.
 
	After completion, the data has been decoded and a null terminator
	signifies the end of a name or value.  A second null terminator (or a
	null name parameter) indicates the end of all the data.

  Precondition:
	The data parameter is null terminated and has at least one extra
	byte free.

  Parameters:
	cData - The string which is to be decoded in place.

  Returns:
	A pointer to the last null terminator in data, which is also the
	first free byte for new data.

  Remarks:
	This function is called by the stack to parse GET arguments and 
	cookie data.  User applications can use this function to decode POST
	data, but first need to verify that the string is null-terminated.
  ***************************************************************************/
BYTE* HTTPURLDecode(BYTE* cData)
{
	BYTE *pRead, *pWrite;
	WORD wLen;
	BYTE c;
	WORD hex;
	 
	// Determine length of input
	wLen = strlen((char*)cData);
	 
	// Read all characters in the string
	for(pRead = pWrite = cData; wLen != 0u; )
	{
		c = *pRead++;
		wLen--;
		
		if(c == '=' || c == '&')
			*pWrite++ = '\0';
		else if(c == '+')
			*pWrite++ = ' ';
		else if(c == '%')
		{
			if(wLen < 2u)
				wLen = 0;
			else
			{
				((BYTE*)&hex)[1] = *pRead++;
				((BYTE*)&hex)[0] = *pRead++;
				wLen--;
				wLen--;
				*pWrite++ = hexatob(*((WORD_VAL*)&hex));
			}
		}
		else
			*pWrite++ = c;
	}
	
	// Double null terminate the last value
	*pWrite++ = '\0';
	*pWrite = '\0';
	
	return pWrite;
}

/*****************************************************************************
  Function:
	BYTE* HTTPGetArg(BYTE* cData, BYTE* cArg)

  Summary:
	Locates a form field value in a given data array.

  Description:
	Searches through a data array to find the value associated with a
	given argument.  It can be used to find form field values in data
	received over GET or POST.
	
	The end of data is assumed to be reached when a null name parameter is
	encountered.  This requires the string to have an even number of 
	null-terminated strings, followed by an additional null terminator.

  Precondition:
	The data array has a valid series of null terminated name/value pairs.

  Parameters:
	data - the buffer to search
	arg - the name of the argument to find

  Returns:
	A pointer to the argument value, or NULL if not found.
  ***************************************************************************/
BYTE* HTTPGetArg(BYTE* cData, BYTE* cArg)
{
	// Search through the array while bytes remain
	while(*cData != '\0')
	{
        DEBUG_PUT_CHAR(DEBUG_LEVEL_INFO, '-');
        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, (const char*)cData);

		// Look for arg at current position
		if(!strcmp((char*)cArg, (char*)cData))
		{// Found it, so return parameter
			return cData + strlen((char*)cArg) + 1;
		}
		
		// Skip past two strings (NUL bytes)
		cData += strlen((char*)cData) + 1;
		cData += strlen((char*)cData) + 1;
	}
	 	
	// Return NULL if not found
	return NULL;
}

/*****************************************************************************
  Function:
	BYTE* HTTPGetROMArg(BYTE* cData, ROM BYTE* cArg)

  Summary:
	Locates a form field value in a given data array.

  Description:
	Searches through a data array to find the value associated with a
	given argument.  It can be used to find form field values in data
	received over GET or POST.
	
	The end of data is assumed to be reached when a null name parameter is
	encountered.  This requires the string to have an even number of 
	null-terminated strings, followed by an additional null terminator.

  Precondition:
	The data array has a valid series of null terminated name/value pairs.

  Parameters:
	data - the buffer to search
	arg - the name of the argument to find

  Returns:
	A pointer to the argument value, or NULL if not found.

  Remarks:
  	This function is aliased to HTTPGetArg on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
BYTE* HTTPGetROMArg(BYTE* cData, ROM BYTE* cArg)
{
	// Search through the array while bytes remain
	while(*cData != '\0')
	{
		// Look for arg at current position
		if(!memcmppgm2ram(cData, (ROM void*)cArg, strlenpgm((ROM char*)cArg) + 1))
		{// Found it, so skip to next string
			return cData + strlenpgm((ROM char*)cArg) + 1;
		}
		
		// Skip past two strings (NUL bytes)
		cData += strlen((char*)cData) + 1;
		cData += strlen((char*)cData) + 1;
	}
	 	
	// Return NULL if not found
	return NULL;
}
#endif

/*****************************************************************************
  Function:
	HTTP_READ_STATUS HTTPReadPostName(BYTE* cData, WORD wLen)

  Summary:
	Reads a name from a URL encoded string in the TCP buffer.

  Description:
	Reads a name from a URL encoded string in the TCP buffer.  This function
	is meant to be called from an HTTPExecutePost callback to facilitate
	easier parsing of incoming data.  This function also prevents buffer
	overflows by forcing the programmer to indicate how many bytes are
	expected.  At least 2 extra bytes are needed in cData over the maximum
	length of data expected to be read.
	
	This function will read until the next '=' character, which indicates the
	end of a name parameter.  It assumes that the front of the buffer is
	the beginning of the name paramter to be read.
	
	This function properly updates curHTTP.byteCount by decrementing it
	by the number of bytes read.  It also removes the delimiting '=' from
	the buffer.

  Precondition:
	Front of TCP buffer is the beginning of a name parameter, and the rest of
	the TCP buffer contains a URL-encoded string with a name parameter 
	terminated by a '=' character.

  Parameters:
	cData - where to store the name once it is read
	wLen - how many bytes can be written to cData

  Return Values:
	HTTP_READ_OK - name was successfully read
	HTTP_READ_TRUNCTATED - entire name could not fit in the buffer, so the
							value was truncated and data has been lost
	HTTP_READ_INCOMPLETE - entire name was not yet in the buffer, so call
							this function again later to retrieve
  ***************************************************************************/
#if defined(HTTP_USE_POST)
HTTP_READ_STATUS HTTPReadPostName(BYTE* cData, WORD wLen)
{
	HTTP_READ_STATUS status;
	
	status = HTTPReadTo('=', cData, wLen);

	// Decode the data (if not reading to null or blank) and return
	if(cData && *cData)
		HTTPURLDecode(cData);
	return status;
}	
#endif

/*****************************************************************************
  Function:
	HTTP_READ_STATUS HTTPReadPostValue(BYTE* cData, WORD wLen)

  Summary:
	Reads a value from a URL encoded string in the TCP buffer.

  Description:
	Reads a value from a URL encoded string in the TCP buffer.  This function
	is meant to be called from an HTTPExecutePost callback to facilitate
	easier parsing of incoming data.  This function also prevents buffer
	overflows by forcing the programmer to indicate how many bytes are
	expected.  At least 2 extra bytes are needed in cData above the maximum
	length of data expected to be read.
	
	This function will read until the next '&' character, which indicates the
	end of a value parameter.  It assumes that the front of the buffer is
	the beginning of the value paramter to be read.  If curHTTP.byteCount
	indicates that all expected bytes are in the buffer, it assumes that 
	all remaining data is the value and acts accordingly.
	
	This function properly updates curHTTP.byteCount by decrementing it
	by the number of bytes read.  The terminating '&' character is also 
	removed from the buffer.
	
  Precondition:
	Front of TCP buffer is the beginning of a name parameter, and the rest of
	the TCP buffer contains a URL-encoded string with a name parameter 
	terminated by a '=' character.

  Parameters:
	cData - where to store the value once it is read
	wLen - how many bytes can be written to cData

  Return Values:
	HTTP_READ_OK - value was successfully read
	HTTP_READ_TRUNCTATED - entire value could not fit in the buffer, so the
							value was truncated and data has been lost
	HTTP_READ_INCOMPLETE - entire value was not yet in the buffer, so call
							this function again later to retrieve
  ***************************************************************************/
#if defined(HTTP_USE_POST)
HTTP_READ_STATUS HTTPReadPostValue(BYTE* cData, WORD wLen)
{
	HTTP_READ_STATUS status;
	
	// Try to read the value
	status = HTTPReadTo('&', cData, wLen);
	
	// If read was incomplete, check if we're at the end
	if(status == HTTP_READ_INCOMPLETE)
	{
		// If all data has arrived, read all remaining data
		if(curHTTP.byteCount == TCPIsGetReady(sktHTTP))
			status = HTTPReadTo('\0', cData, wLen);
	}
		
	// Decode the data (if not reading to null or blank) and return
	if(cData && *cData)
		HTTPURLDecode(cData);
	return status;
}	
#endif

/*****************************************************************************
  Function:
	static HTTP_READ_STATUS HTTPReadTo(BYTE cDelim, BYTE* cData, WORD wLen)

  Summary:
	Reads to a buffer until a specified delimiter character.

  Description:
	Reads from the TCP buffer to cData until either cDelim is reached, or
	until wLen - 2 bytes have been read.  The value read is saved to cData and 
	null terminated.  (wLen - 2 is used so that the value can be passed to
	HTTPURLDecode later, which requires a null terminator plus one extra free
	byte.)
	
	The delimiter character is removed from the buffer, but not saved to 
	cData. If all data cannot fit into cData, it will still be removed from 
	the buffer but will not be saved anywhere.

	This function properly updates curHTTP.byteCount by decrementing it
	by the number of bytes read. 

  Precondition:
	None

  Parameters:
  	cDelim - the character at which to stop reading, or NULL to read to
  			 the end of the buffer
	cData - where to store the data being read
	wLen - how many bytes can be written to cData

  Return Values:
	HTTP_READ_OK - data was successfully read
	HTTP_READ_TRUNCTATED - entire data could not fit in the buffer, so the
							data was truncated and data has been lost
	HTTP_READ_INCOMPLETE - delimiter character was not found
  ***************************************************************************/
#if defined(HTTP_USE_POST)
static HTTP_READ_STATUS HTTPReadTo(BYTE cDelim, BYTE* cData, WORD wLen)
{
	HTTP_READ_STATUS status;
	WORD wPos;
	
	// Either look for delimiter, or read all available data
	if(cDelim)
		wPos = TCPFind(sktHTTP, cDelim, 0, FALSE);
	else
		wPos = TCPIsGetReady(sktHTTP);
	
	// If not found, return incomplete
	if(wPos == 0xffff)
		return HTTP_READ_INCOMPLETE;
	
	// Read the value
	if(wLen < 2u && cData != NULL)
	{// Buffer is too small, so read to NULL instead
		curHTTP.byteCount -= TCPGetArray(sktHTTP, NULL, wPos);
		status = HTTP_READ_TRUNCATED;
	}
	else if(cData == NULL)
	{// Just remove the data
		curHTTP.byteCount -= TCPGetArray(sktHTTP, NULL, wPos);
		status = HTTP_READ_OK;
	}
	else if(wPos > wLen - 2)
	{// Read data, but truncate at max length
		curHTTP.byteCount -= TCPGetArray(sktHTTP, cData, wLen - 2);
		curHTTP.byteCount -= TCPGetArray(sktHTTP, NULL, wPos - (wLen - 2));
		cData[wLen - 2] = '\0';
		status = HTTP_READ_TRUNCATED;
	}
	else
	{// Read the data normally
		curHTTP.byteCount -= TCPGetArray(sktHTTP, cData, wPos);
		cData[wPos] = '\0';
		status = HTTP_READ_OK;
	}
	
	// Remove the delimiter
	if(cDelim)
		curHTTP.byteCount -= TCPGet(sktHTTP, NULL);
	
	return status;
}	
#endif

#if defined(HTTP_MPFS_UPLOAD)

#define HTTP_MAX_DATA_LEN_FIRMWARE 80

typedef struct __attribute__((__packed__)) HTTP_FIRMWARE_UPLOAD_
{
    BYTE        sm;                 //Firmware Upload State Machine, is a SM_FIRMWARE_xx constant
    BYTE        recordLen;
    BYTE        recordType;
    BYTE        recordCheckSum;
    DWORD_VAL   adr;
    DWORD       adrTemp;
    BYTE        data[HTTP_MAX_DATA_LEN_FIRMWARE];
} HTTP_FIRMWARE_UPLOAD;

//#if (HTTP_MAX_DATA_LEN < sizeof(HTTP_FIRMWARE_UPLOAD))
#if (HTTP_MAX_DATA_LEN < (12+HTTP_MAX_DATA_LEN_FIRMWARE))
#error "For uploading firmware, increase size of HTTP_MAX_DATA_LEN!"
#endif

/*****************************************************************************
  Function:
	HTTP_IO_RESULT HTTPMPFSUpload(void)

  Summary:
	Saves a file uploaded via POST as the new MPFS image in EEPROM or
	external Flash.

  Description:
	Allows the MPFS image in EEPROM or external Flash to be updated via a
	web page by accepting a file upload and storing it to the external memory.

  Precondition:
	MPFSFormat() has been called.

  Parameters:
	None

  Return Values:
	HTTP_IO_DONE - on success
	HTTP_IO_NEED_DATA - if more data is still expected

  Remarks:
	This function is only available when MPFS uploads are enabled and
	the MPFS image is stored in EEPROM.

  Internal:
	After the headers, the first line from the form will be the MIME
	separator.  Following that is more headers about the file, which
	are discarded.  After another CRLFCRLF pair the file data begins,
	which is read 16 bytes at a time and written to external memory.
  ***************************************************************************/
static HTTP_IO_RESULT HTTPMPFSUpload(void)
{
	BYTE c[16];
	WORD lenA, lenB;
    BYTE done = TRUE;
    BYTE i, j;
    BYTE contentType;
    BOOL bFirmware;     //This is a firmware upload
    DWORD xflAdr;       //external flash address
    WORD_VAL wv;

    #define SM_MPFSUPLOAD_ENTRY                 (0u)
   	#define SM_MPFSUPLOAD_REMOVE_HEADERS        (1u)
    #define SM_MPFSUPLOAD_CHECK_VERSION         (2u)
    #define SM_MPFSUPLOAD_FIRMWARE_ENTRY        (3u)
    #define SM_MPFSUPLOAD_FIRMWARE_FLASH_ERASE  (4u)
    #define SM_MPFSUPLOAD_FIRMWARE              (5u)

    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTPMPFSUpload()");             //MODTRONIX added this line

    //Check if URL is "mpfsuploadf" - last char = 'f'.
    //IMPORTANT!!!! Note that this test will NOT BE VALID from the SM_MPFSUPLOAD_FIRMWARE state
    //onward, seeing that we use curHTTP.data[] as a buffer!
    bFirmware = (curHTTP.data[sizeof(HTTP_MPFS_UPLOAD)] == 'f');
    
    do {
        switch(curHTTP.httpStatus)
        {
            // Upload Web Pages or Firmware
            case HTTP_MPFS_UP:

                switch(curHTTP.smPost)
                {
                    //First state when entering HTTPMPFSUpload() for first time. Check the 'Content-Type' of post message received.
                    case SM_MPFSUPLOAD_ENTRY:
                        if (bFirmware) {
                            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, " - Firmware Upload");   //MODTRONIX added this line
                        }
                        else {
                            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, " - Web Page Upload");   //MODTRONIX added this line
                        }

                        done = FALSE;   //Continue after this step

                        //contentType = (BYTE)(curHTTP.nextCallback&0x00ff);  //Content-Type is stored in lower byte of nextCallback
                        contentType = BYTE0_DWORD(curHTTP.nextCallback);

                        //Content-Type =  'multipart/form-Data'. There are leading header, a CRLFCRLF, and then only the posted file.
                        if (contentType == POST_CONTENT_TYPE_MUL_FORM_DATA) {
                            curHTTP.smPost = SM_MPFSUPLOAD_REMOVE_HEADERS;

                            //Byte 1 of curHTTP.nextCallback contains length of Boundary string. The file contents
                            //will be terminated with "\r\n--BOUNDARY_STRING--\r\n". See http://www.w3.org/Protocols/rfc2616/rfc2616-sec19.html#sec19.2
                            //This means last ("Lenght of Boundary String" + 8) is NOT part of file. Decrement byteCount by this amount.
                            curHTTP.byteCount -= BYTE1_DWORD(curHTTP.nextCallback) + 8;
                            if (curHTTP.byteCount < 0) {
                                curHTTP.byteCount = 0;
                                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, " - byteCount < 0");   //MODTRONIX added this line
                            }
                        }
                        //Content-Type =  'application/x-www-form-urlencoded'. There posted file will start straight away.
                        else if (contentType == POST_CONTENT_TYPE_APP_X_WWW_FORM_URLENCODED) {
                            if (bFirmware) {
                                curHTTP.smPost = SM_MPFSUPLOAD_FIRMWARE_ENTRY;
                            }
                            else {
                                curHTTP.smPost = SM_MPFSUPLOAD_CHECK_VERSION;
                            }
                        }
                        //Not supported content type
                        else {
                            curHTTP.httpStatus = HTTP_MPFS_ERROR;
                            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nUnknow Content-Type");    //MODTRONIX added this line
                        }
                        break;
                    case SM_MPFSUPLOAD_REMOVE_HEADERS:
                        lenA = TCPFindROMArray(sktHTTP, (ROM BYTE*)"\r\n\r\n", 4, 0, FALSE);
                        if(lenA != 0xffff)
                        {// Found it, so remove all data up to and including (+4)
                            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: Fnd MPFS start");    //MODTRONIX added this line
                            lenA = TCPGetArray(sktHTTP, NULL, lenA+4);
                            curHTTP.byteCount -= (lenA);
                            done = FALSE;   //Continue as long as more data is available in TCP buffer
                            if (bFirmware) {
                                curHTTP.smPost = SM_MPFSUPLOAD_FIRMWARE_ENTRY;

                                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " Size = ");           //MODTRONIX added this line
                                DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, curHTTP.byteCount);  //MODTRONIX added this line
                            }
                            else {
                                curHTTP.smPost = SM_MPFSUPLOAD_CHECK_VERSION;
                            }

                        }
                        else
                        {// Otherwise, remove as much as possible. Leave last 4 bytes, might be part of \r\n\r\n we are looking for.
                            lenB = TCPIsGetReady(sktHTTP);  //Store number of bytes ready to get in lenB
                            if (lenB > 4) {
                                lenA = TCPGetArray(sktHTTP, NULL, lenB - 4);
                                curHTTP.byteCount -= lenA;
                                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCRLFCRLF Not yet fnd, rmvng:");             //MODTRONIX added this line
                            }
                            else {
                                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nCRLFCRLF Not yet fnd");             //MODTRONIX added this line
                            }
                            return HTTP_IO_NEED_DATA;
                        }
                        break;
                    case SM_MPFSUPLOAD_CHECK_VERSION:
                        // Make sure first 6 bytes are also in
                        if(TCPIsGetReady(sktHTTP) < 6u )
                        {
                            return HTTP_IO_NEED_DATA;
                        }

                        // Make sure it's an MPFS of the correct version
                        lenA = TCPGetArray(sktHTTP, c, 6);
                        curHTTP.byteCount -= lenA;
                        if(memcmppgm2ram(c, (ROM void*)"MPFS\x02\x01", 6) == 0)
                        {// Read as Ver 2.1
                            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nHTTP2: Fnd MPFS2");    //MODTRONIX added this line
                            curHTTP.httpStatus = HTTP_MPFS_OK;

                            //TODO!!! Check if MPFSFormat should be called here, or if we should maybe just erase all sectors required for webpages.
                            //As we do for firmware upgrade (see SM_MPFSUPLOAD_FIRMWARE_FLASH_ERASE case).
                            
                            // Format MPFS storage and put 6 byte tag
                            curHTTP.file = MPFSFormat();
                            MPFSPutArray(curHTTP.file, &c[0], 6);
                            done = FALSE;   //Continue as long as more data is available in TCP buffer
                        }
                        else
                        {// Version is wrong
                            curHTTP.httpStatus = HTTP_MPFS_ERROR;
                            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: Version ERROR");   //MODTRONIX added this line
                            c[6] = '\0';
                            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nReceived: ");             //MODTRONIX added this line
                            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, (const char*)c);             //MODTRONIX added this line
                            done = TRUE;    //Continue as long as more data is available in TCP buffer
                        }
                        break;
                    case SM_MPFSUPLOAD_FIRMWARE_ENTRY:
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n - Process Firmware Entry");    //MODTRONIX added this line

                        if (firmwareFlags.flags.bits.bCurr == CURRENT_FIRMWARE_A) {
                            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n - To B");    //MODTRONIX added this line
                        }
                        else {
                            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n - To A");    //MODTRONIX added this line
                        }

                        //First of all, invalidate firmware by erasing FIRMWARE_INFO
                        SPIFlashEraseSector((firmwareFlags.flags.bits.bCurr == CURRENT_FIRMWARE_A) ? XFLASH_FIRMWAREB_INFO_ADR : XFLASH_FIRMWAREA_INFO_ADR);

                        //Use curHTTP.data[0] to remember how many sectors still have to be erased in SM_MPFSUPLOAD_FIRMWARE_FLASH_ERASE state
                        curHTTP.data[0] = 0;

                        curHTTP.smPost = SM_MPFSUPLOAD_FIRMWARE_FLASH_ERASE;
                        return HTTP_IO_WAITING; //Done, and return from this function. Do this, seeing that each SPIFlashEraseSector() can take up to 400ms.
                        //break;
                    case SM_MPFSUPLOAD_FIRMWARE_FLASH_ERASE:
                        //We have finished erasing all sectors
                        if (curHTTP.data[0] >= (XFLASH_FIRMWARE_SIZE_IN_SECTORS + XFLASH_FIRMWARE_CONFIG_SIZE_IN_SECTORS)) {
                            curHTTP.smPost = SM_MPFSUPLOAD_FIRMWARE;

                            //Initialize curHTTP.data with 0. Is used for HTTP_FIRMWARE_UPLOAD in "case SM_MPFSUPLOAD_FIRMWARE"
                            memset(curHTTP.data, 0, HTTP_MAX_DATA_LEN);

                            //debugPrintFlash((firmwareFlags.flags.bits.bCurr == CURRENT_FIRMWARE_A) ? XFLASH_FIRMWAREB_INFO_ADR : XFLASH_FIRMWAREA_INFO_ADR, sizeof(FIRMWARE_INFO));

                            done = FALSE;
                        }
                        //Erase next sector
                        else {
                            //Next erase FIRMWAREx --AND-- FIRMWAREx_CONFIG area. FIRMWAREx_CONFIG follows FIRMWAREx in External FLASH

                            SPIFlashEraseSector( (DWORD)((firmwareFlags.flags.bits.bCurr == CURRENT_FIRMWARE_A) ? XFLASH_FIRMWAREB_ADR : XFLASH_FIRMWAREA_ADR) + ( ((DWORD)curHTTP.data[0]) * ((DWORD)XFLASH_SECTOR_SIZE) ));
                            curHTTP.data[0]++;  //Increment sector

                            return HTTP_IO_WAITING; //Done, and return from this function. Do this, seeing that each SPIFlashEraseSector() can take up to 400ms.
                        }

                        break;
                    case SM_MPFSUPLOAD_FIRMWARE:
                    {
                        HTTP_FIRMWARE_UPLOAD* pData;    //Pointer to data
                        DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n - Update Firmware");    //MODTRONIX added this line

                        // Find out how many bytes are available to be read
                        lenA = TCPIsGetReady(sktHTTP);
                        if(lenA > curHTTP.byteCount)
                            lenA = curHTTP.byteCount;

                        //Read and process available data
                        while(lenA > 0u)
                        {
                            //Wait for space to become available in buffer
                            //while (cbufGetFree(CIRBUF_TX_DEBUG) < 100) {
                            //    nzSysTaskDefault();
                            //}

                            pData = (HTTP_FIRMWARE_UPLOAD*)curHTTP.data;

                            // Verify a whole line line is in the FIFO. Unix ends with '\n', Windows ends with '\r\n'
                            if((lenB = TCPFind(sktHTTP, '\n', 0, FALSE)) == 0xffff)
                            {
                                //In case last line doesn't end with '\n'. In this case, lenA == curHTTP.byteCount. This means
                                //there is no more data for this file, and last read line does not end with '\n'.
                                if (lenA == curHTTP.byteCount) {
                                    lenB = TCPGetArray(sktHTTP, NULL, lenA);
                                    curHTTP.byteCount -= lenB;
                                    return HTTP_IO_DONE;
                                }
                                return HTTP_IO_NEED_DATA;   // Ask for more data
                            }

                            //Line can be
                            if ((lenB+1) >= HTTP_MAX_DATA_LEN_FIRMWARE) {
                                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nLine too long");    //MODTRONIX added this line
                                lenB = TCPGetArray(sktHTTP, NULL, lenB+1);
                                curHTTP.byteCount -= lenB;
                                lenA -= lenB;
                                continue;
                            }

                            lenB = TCPGetArray(sktHTTP, pData->data, lenB+1);
                            curHTTP.byteCount -= lenB;
                            lenA -= lenB;

                            //Each line of hex file must be at least 11 bytes long = 11 + '\n' = 12
                            if (lenB < 12)
                            { //Error in current line read. Continue to next line
                                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\n - Err: Length < 11");    //MODTRONIX added this line
                                continue;
                            }

                            //Replace '/n' with NULL terminator. Also adjust lenB so it indicates lenght of string (NOT including NULL terminator)
                            pData->data[--lenB] = '\0';

                            //Do some checks on data read
                            if (pData->data[0] != ':') {
                                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\n - Err: No :");    //MODTRONIX added this line
                                continue;
                            }

                            //At this stage lenB is lenght of string, NOT including NULL terminator. For ":00000001FF" it will be 11

                            //Remove trailing white spaces
                            //while ((lenB > 0) && (isspace(pData->data[lenB-1]))) {
                            while ((lenB > 0) && ( (pData->data[lenB-1]==' ') || (pData->data[lenB-1]=='\t') || (pData->data[lenB-1]=='\r') || (pData->data[lenB-1]=='\n'))) {
                                pData->data[--lenB] = '\0';
                            }

                            //Calculate checksum of all data bytes
                            pData->recordCheckSum = 0;
                            i=1;
                            while (i<lenB) {
                                wv.v[1] = pData->data[i++];
                                wv.v[0] = pData->data[i++];
                                pData->recordCheckSum += hexatob(wv);
                            }
                            if (pData->recordCheckSum != 0) {
                                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\n - Chksum Err");  //MODTRONIX added this line
                                curHTTP.httpStatus = HTTP_MPFS_ERROR;
                                done = TRUE;    //Continue as long as more data is available in TCP buffer
                                break;
                            }

                            //Get record length
                            wv.v[0] = pData->data[2];
                            wv.v[1] = pData->data[1];
                            pData->recordLen = hexatob(wv);

                            //Get supplied address. Depending on record type, might be bits 16 - 31 or address (for 4)
                            wv.v[0] = pData->data[4];
                            wv.v[1] = pData->data[3];
                            pData->adr.v[1] = hexatob(wv);
                            wv.v[0] = pData->data[6];
                            wv.v[1] = pData->data[5];
                            pData->adr.v[0] = hexatob(wv);

                            //Get record type
                            wv.v[0] = pData->data[8];
                            wv.v[1] = pData->data[7];
                            pData->recordType = hexatob(wv);

                            if (pData->recordType == 4) {
                                wv.v[0] = pData->data[10];
                                wv.v[1] = pData->data[9];
                                pData->adr.v[3] = hexatob(wv);
                                wv.v[0] = pData->data[12];
                                wv.v[1] = pData->data[11];
                                pData->adr.v[2] = hexatob(wv);

                                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n - AdrU = ");      //MODTRONIX added this line
                                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, pData->adr.w[1]);    //MODTRONIX added this line
                            }
                            else if (pData->recordType == 1) {
                                DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n - End of Hex file");   //MODTRONIX added this line

                                //Update "FirmwareA Info" or "FirmwareB Info" to indicate that new firmware is available. Next time device
                                //is reset, this new firmware will be programmed into device.
                                xflAdr = ((firmwareFlags.flags.bits.bCurr == CURRENT_FIRMWARE_A) ? XFLASH_FIRMWAREB_INFO_ADR : XFLASH_FIRMWAREA_INFO_ADR);

                                //Initialize curHTTP.data with 0. Is used for HTTP_FIRMWARE_UPLOAD in "case SM_MPFSUPLOAD_FIRMWARE"
                                memset(curHTTP.data, 0, HTTP_MAX_DATA_LEN);

                                //Use pData->data[] to build "Firmware Info" structure, and then write it to External Flash
                                memset(&pData->data[0], 0, sizeof(FIRMWARE_INFO));
                                ((FIRMWARE_INFO*)&pData->data[0])->magicNumber.Val = FIRMWARE_INFO_MAGIC_NUMBER;
                                //((FIRMWARE_INFO*)&pData->data[0])->versionMajor = ??;
                                //((FIRMWARE_INFO*)&pData->data[0])->versionMinor = ??;
                                ((FIRMWARE_INFO*)&pData->data[0])->fwState.Val = FIRMWARE_STATE_NEW;

                                //Write data to External Flash.
                                SPIFlashBeginWrite(xflAdr);
                                SPIFlashWriteArray(pData->data, sizeof(FIRMWARE_INFO), FALSE);

                                //debugPrintFlash((firmwareFlags.flags.bits.bCurr == CURRENT_FIRMWARE_A) ? XFLASH_FIRMWAREB_INFO_ADR : XFLASH_FIRMWAREA_INFO_ADR, sizeof(FIRMWARE_INFO));
                            }
                            else if (pData->recordType == 0)
                            {
                                //Adjust
                                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n - Adr = ");       //MODTRONIX added this line
                                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, pData->adr.w[0]);    //MODTRONIX added this line

                                if ( (pData->adr.Val >= MEM_PROGRAM_START) && (pData->adr.Val <= MEM_PROGRAM_LAST) ) {
                                    //pData->recordType = 0;    //Mart that this contains Program Memory - Is already 0

                                    //Get address in External Flash where this Program Memory is stored.
                                    // - Subtract offset of MEM_PROGRAM_START.
                                    // - Scale, each 4 bytes in flash is packed to 3 bytes in External Flash
                                    // - Add External Flash offset for XFLASH_FIRMWAREA_ADR or XFLASH_FIRMWAREB_ADR
                                    xflAdr = ((pData->adr.Val - MEM_PROGRAM_START)*3)/4;
                                    //xflAdr = (pData->adr.Val - MEM_PROGRAM_START);    //Following two lines are alternative method
                                    //xflAdr = (xflAdr/4) + (xflAdr/2);
                                    xflAdr += ((firmwareFlags.flags.bits.bCurr == CURRENT_FIRMWARE_A) ? XFLASH_FIRMWAREB_ADR : XFLASH_FIRMWAREA_ADR);

                                    //sprintf(debugTempBuf, "\n-PgmMem=%lx, xflAdr=%lx", pData->adr.Val, xflAdr);
                                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, debugTempBuf);
                                }
                                else if ( (pData->adr.Val >= MEM_XEEPROM_START) && (pData->adr.Val <= MEM_XEEPROM_LAST) ) {
                                    pData->recordType = 0xff;   //Mark that this contains Configuration data
                                    
                                    //Get address in External Flash where this "External EEPROM"(Firmware Config) data is stored.
                                    // - Subtract offset of MEM_XEEPROM_START.
                                    // - Scale, each 4 bytes in hex file is packed to 2 bytes in External Flash
                                    // - Add offset of XFLASH_FIRMWAREA_CONFIG or XFLASH_FIRMWAREB_CONFIG
                                    xflAdr = (pData->adr.Val - MEM_XEEPROM_START) / 2;
                                    xflAdr += ((firmwareFlags.flags.bits.bCurr == CURRENT_FIRMWARE_A) ? XFLASH_FIRMWAREB_CONFIG : XFLASH_FIRMWAREA_CONFIG);
                                    
                                    //sprintf(debugTempBuf, "\n-CfgMem=%lx, xflAdr=%lx", pData->adr.Val, xflAdr);
                                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, debugTempBuf);
                                }
                                else {
                                    DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\n-UndefMem=");
                                    DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, pData->adr.word.HW);
                                    DEBUG_PUT_HEXWORD(DEBUG_LEVEL_INFO, pData->adr.word.LW);
                                    continue;
                                }

                                //Convert data in pData->data from Ascii to Hex, and pack it. Each Ascii byte required 2 chars. Each hex byte only 1.
                                // - For Program Memory, each 2 Word Instructions is packed into 3 bytes. Last byte of odd word is not used.
                                i=9;
                                j=0;
                                while (i<(lenB-2)) {
                                    wv.v[1] = pData->data[i++];
                                    wv.v[0] = pData->data[i++];

                                    //- For Program Memory, do NOT use upper byte of odd word
                                    //- For Configuration Data, do not use odd word.
                                    if (   ((pData->recordType == 0) && ((pData->adr.v[0]&0x03) != 0x03))
                                        || ((pData->recordType == 0xff) && ((pData->adr.v[0]&0x02) == 0)) )
                                    {
                                        pData->data[j++] = hexatob(wv);
                                    }
                                    pData->adr.v[0]++;  //Increment last byte of address. Only last byte is required above to see if this is upper byte of odd word of program memory.
                                }

                                //Write data to External Flash.
                                SPIFlashBeginWrite(xflAdr);
                                SPIFlashWriteArray(pData->data, j, FALSE);

                                //debugPutHexLine(xflAdr, pData->data, j);

                                //Program Memory
                                //if ( pData->recordType == 0 ) {
                                    //debugPutHexLine(xflAdr, pData->data, j);
                                //}
                                //Configuration Data
                                //else if ( pData->recordType == 0xff ) {
                                    //debugPutHexLine(xflAdr, pData->data, j);
                                //}

                            }   // else if (pData->recordType == 0) {
                        }   // while(lenA > 0u)


                        if (curHTTP.httpStatus == HTTP_MPFS_ERROR)
                            break;

                        // If we've read all the data, Redirect to the page
                        if(curHTTP.byteCount == 0u) {
                            strcpypgm2ram((char*)curHTTP.data, "uploadfirm.htm");
                            curHTTP.httpStatus = HTTP_REDIRECT;
                            return HTTP_IO_DONE;
                        }

                        return HTTP_IO_NEED_DATA;   // Ask for more data
                    }
                    //break;

                    // Other states are not valid here
                    default:
                        break;
                }   // switch(curHTTP.smPost)

                break;
            // Received file is invalid
            case HTTP_MPFS_ERROR:
                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nHTTP2: Invld File");    //MODTRONIX added this line
                curHTTP.byteCount -= TCPIsGetReady(sktHTTP);
                TCPDiscard(sktHTTP);
                if(curHTTP.byteCount < 100u || curHTTP.byteCount > 0x80000000u)
                {// If almost all data was read, or if we overflowed, then return
                    return HTTP_IO_DONE;
                }
                break;
            /////////////////////////////////////////////////
            // File is verified, so write the data to external FLASH
            case HTTP_MPFS_OK:
                done = TRUE;
                // Determine how much to read
                lenA = TCPIsGetReady(sktHTTP);
                if(lenA > curHTTP.byteCount)
                    lenA = curHTTP.byteCount;

                while(lenA > 0u)
                {
                    lenB = TCPGetArray(sktHTTP, c, mMIN(lenA,16u));
                    curHTTP.byteCount -= lenB;
                    lenA -= lenB;
                    MPFSPutArray(curHTTP.file, c, lenB);
                    // Print file that get's written to FLASH. Use for debuging, only when sending very small web pages images
                    //debugPutHexLine(curHTTP.byteCount, c, lenB);
                }

                // If we've read all the data
                if(curHTTP.byteCount == 0u)
                {
                    MPFSPutEnd(TRUE);
                    return HTTP_IO_DONE;
                }

            // Other states are not valid here
            default:
                break;
        }   // switch(curHTTP.httpStatus)
    } while (done != TRUE);
		
	// Ask for more data
	return HTTP_IO_NEED_DATA;
	
}
#endif

/*****************************************************************************
  Function:
	void HTTPIncFile(ROM BYTE* cFile)

  Summary:
	Writes a file byte-for-byte to the currently loaded TCP socket.

  Description:
	Allows an entire file to be included as a dynamic variable, providing
	a basic templating system for HTML web pages.  This reduces unneeded
	duplication of visual elements such as headers, menus, etc.

	When curHTTP.callbackPos is 0, the file is opened and as many bytes
	as possible are written.  The current position is then saved to 
	curHTTP.callbackPos and the file is closed.  On subsequent calls, 
	reading begins at the saved location and continues.  Once the end of
	the input file is reached, curHTTP.callbackPos is set back to 0 to 
	indicate completion.

  Precondition:
	None

  Parameters:
	cFile - the name of the file to be sent

  Returns:
  	None
  	
  Remarks:
	Users should not call this function directly, but should instead add
	dynamic variables in the form of ~inc:filename.ext~ in their HTML code
	to include (for example) the file "filename.ext" at that specified
	location.  The MPFS2 Generator utility will handle the rest.
  ***************************************************************************/
void HTTPIncFile(ROM BYTE* cFile)
{
	WORD wCount, wLen;
	BYTE data[64];
	MPFS_HANDLE fp;
	
	// Check if this is a first round call
	if(curHTTP.callbackPos == 0x00u)
	{// On initial call, open the file and save its ID
		fp = MPFSOpenROM(cFile);
		if(fp == MPFS_INVALID_HANDLE)
		{// File not found, so abort
			return;
		}
		((DWORD_VAL*)&curHTTP.callbackPos)->w[0] = MPFSGetID(fp);
	}
	else
	{// The file was already opened, so load up its ID and seek
		fp = MPFSOpenID(((DWORD_VAL*)&curHTTP.callbackPos)->w[0]);
		if(fp == MPFS_INVALID_HANDLE)
		{// No file handles available, so wait for now
			return;
		}
		MPFSSeek(fp, ((DWORD_VAL*)&curHTTP.callbackPos)->w[1], MPFS_SEEK_FORWARD);
	}
	
	// Get/put as many bytes as possible
	wCount = TCPIsPutReady(sktHTTP);
	while(wCount > 0u)
	{
		wLen = MPFSGetArray(fp, data, mMIN(wCount, sizeof(data)));
		if(wLen == 0u)
		{// If no bytes were read, an EOF was reached
			MPFSClose(fp);
			curHTTP.callbackPos = 0x00;
			return;
		}
		else
		{// Write the bytes to the socket
			TCPPutArray(sktHTTP, data, wLen);
			wCount -= wLen;
		}
	}
	
	// Save the new address and close the file
	((DWORD_VAL*)&curHTTP.callbackPos)->w[1] = MPFSTell(fp);
	MPFSClose(fp);
	
	return;
}

#endif
