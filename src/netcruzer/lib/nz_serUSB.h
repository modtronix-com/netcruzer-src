 /**
 * @brief           This file defines a buffered USB Interface
 * @file            nz_serUSB.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 *
 * @section nz_usbBuffered_desc Description
 *****************************************
 * This file defines a Buffered USB interface. The implementation has to be done
 * in a separate c file by creating instances of these functions. The Netcruzer
 * library contains a default implementations, nz_serUSB.c.
 * To use the buffered USB module, the following must be done:
 * - Copy @ref nz_usbBuffered_conf "Configuration" section below to projdefs.h file.
 * - In this "Configuration" section, uncomment disired lines, and change any default
 *   values as needed.
 * - Add the nz_serUSB.c default implementation to the MPLAB project, or create
 *   a new implementation if the functionality should be changed.
 * - The functions defined in this file can now be used in the project.
 *
 * @subsection nz_usbBuffered_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// -------------- USB Configuration (nz_serUSB.h) -----------------
// *********************************************************************
//System will initialize and manage a USB port. We have to add an implementation to project (like nz_serUSB.c)
#define NZ_USBHID_ENABLE
//#define NZ_USBCDC_ENABLE

//If set to 1, the system LED is controlled by the nz_serUSB module
#define USB_CONTROLS_SYSLED						( 1 )

//Indicates if this application handles commands via USB. When defined, the application must implement the "nz_cmdXxx" callback functions!
#define APP_HANDLES_USB_COMMANDS

 @endcode
 **********************************************************************
 * @section nz_usbBuffered_lic Software License Agreement
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
 * 2010-10-11, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_USB_BUFFERED_H
#define    NZ_USB_BUFFERED_H

//"Serial Data Port" contain circular buffers, this include is required
#include "nz_circularBuffer.h"

#if defined(HAS_SERPORT_USB)

#if !defined(SERPORT_USB_CREATE_OWN_CIRBUFS)
#include "nz_serDataPorts.h"
#endif

////////// Default Defines //////////////////////

#if !defined(USB_CONTROLS_SYSLED)
#define USB_CONTROLS_SYSLED     0
#endif

/**
 * Size of USB TX buffer. Depending on circular buffer used for implementation, might
 * HAVE TO BE a power of 2 value (if nz_circularBufferPwr2.c is used)! Increase size
 * of this buffer if the application gets errors when trying to send messages via
 * this module.
 */
#if !defined(USB_TXBUF_SIZE)
#define USB_TXBUF_SIZE 256
#endif

/**
 * Size of USB RX buffer. Depending on circular buffer used for implementation, might HAVE TO
 * BE a power of 2 value (if nz_circularBufferPwr2.c is used)! Ensure it is large enought to
 * hold largest message expected. For example, when using the USB port for debugging, text message
 * are sent to the debug port from the @ref devtools_nzUsbTerminal, and this buffer should be large
 * enought to hold largest debug message.
 */
#if !defined(USB_RXBUF_SIZE)
#define USB_RXBUF_SIZE 32
#endif

//TX and RX "Circular Buffers". Use SERPORT_USB_CREATE_OWN_CIRBUFS define to use this
//module without SERPORTs. If NOT defined, this modules requires nz_serDataPorts.c
#if defined(SERPORT_USB_CREATE_OWN_CIRBUFS)
    //This is used by others
    #ifndef THIS_IS_NZ_SER_USB_C
    #if !defined(CIRBUF_TX_USB)
        extern CIRBUF cbufTxUsb;
        #define CIRBUF_TX_USB  (&cbufTxUsb)
    #endif

    #if !defined(CIRBUF_RX_USB)
        extern CIRBUF cbufRxUsb;
        #define CIRBUF_RX_USB  (&cbufRxUsb)
    #endif
    #endif  //#ifndef THIS_IS_NZ_SER_USB_C
#endif


/////////////////////////////////////////////////
//Defines

//USB "Serial Data Port"
#if defined(NZ_USBHID_ENABLE) && !defined(HAS_SERPORT_USB_HID)
    #define HAS_SERPORT_USB_HID
#endif
#if defined(NZ_USBCDC_ENABLE) && !defined(HAS_SERPORT_USB_CDC)
    #define HAS_SERPORT_USB_CDC
#endif
#if defined(NZ_USBMIDI_ENABLE) && !defined(HAS_SERPORT_USB_MIDI)
    #define HAS_SERPORT_USB_MIDI
#endif
#if defined(HAS_SERPORT_USB_HID) || defined(HAS_SERPORT_USB_CDC) || defined(HAS_SERPORT_USB_MIDI)
    #if !defined(HAS_A_SERPORT)
        #define HAS_A_SERPORT
    #endif
#endif


////////// Configuration Checks /////////////////

//If "Power of 2" buffer used, size must by a power of 2
#if defined(CIRBUF_USE_CIRCULAR_BUFFER_PWR2)
#if defined(HAS_SERPORT_USB)
    //Ensure size is power of 2
    #if ((USB_TXBUF_SIZE & ~(USB_TXBUF_SIZE-1))!=USB_TXBUF_SIZE)
    #error "USB USB_TXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif

    //Ensure size is power of 2
    #if ((USB_RXBUF_SIZE & ~(USB_RXBUF_SIZE-1))!=USB_RXBUF_SIZE)
    #error "USB USB_RXBUF_SIZE is not power of 2! Either change size defined in projdefs.h --OR-- use nz_circularBufferStd.c in stead of nz_circularBufferPwr2.c for project!"
    #endif
#endif
#endif


/**
 * USB Task. Will do following:
 * - Check CIRBUF_TX_USB if there is anything to send. If so, sends it on USB port
 * - Check USB Stack if anything received. If so, will check what was received.
 *   If debug message, adds it to CIRBUF_RX_DEBUG. If command, adds it to CIRBUF_RX_USB.
 */
void serUSBTask(void);


/**
 * USB Initialization.
 */
void serUSBInit(void);


void usbCBSendResume(void);


/*
#define serUSBEmptyTxBuf()              cbufEmpty(&cbufTxUsb)
#define serUSBIsTxBufEmpty()            cbufIsEmpty(&cbufTxUsb)
#define serUSBIsTxBufFull()             cbufIsFull(&cbufTxUsb)
#define serUSBGetTxBufCount()           cbufGetCount(&cbufTxUsb)
#define serUSBGetTxBufFree()            cbufGetFree(&cbufTxUsb)
#define serUSBPeekTxBuf()               cbufPeekByte(&cbufTxUsb)
#define serUSBPeekTxBufAt(offset)       cbufPeekByteAt(&cbufTxUsb, offset)
#define serUSBTxBufRemoveByte()         cbufRemoveByte(&cbufTxUsb)
#define serUSBTxBufRemoveBytes(n)       cbufRemoveBytes(&cbufTxUsb, n)
*/


/**
 * This function indicates if the USB module is in suspend mode.  This function
 * does NOT indicate that a suspend request has been received.  It only
 * reflects the state of the USB module.
 *
 * @return Returns true if the USB modules is suspended.
 */
BOOL serUSBIsSuspended(void);


/**
 * Empty the USB transmit buffer. Any data contained in the buffer is lost.
 */
#define serUSBEmptyTxBuf() cbufEmpty(&serports.usb.cbufTx)


/**
 * Checks if the given buffer is empty.
 *
 * @return Returns true if the given buffer is empty. Else, returns false.
 */
#define serUSBIsTxBufEmpty()  cbufIsEmpty(&serports.usb.cbufTx)


/**
 * Checks if the given transmit buffer is full - no more space for adding data.
 * This is the case when put+1 = get
 *
 * @return Returns true if the given buffer is full. Else, returns false.
 */
#define serUSBIsTxBufFull() cbufIsFull(&serports.usb.cbufTx)

#endif  //#if defined(HAS_SERPORT_USB)

#endif  //#ifndef NZ_USB_BUFFERED_H
