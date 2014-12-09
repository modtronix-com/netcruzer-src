 /**
 * @brief           Makes USB available as a "Serial Data Port".
 * @file            serportUSB.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 *********************************************************************
 * Software License Agreement
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
 * 2012-08-08, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_SERPORT_USB_MODULE

#include "HardwareProfile.h"

#include "nz_serUSB.h"
#include "nz_usbHidDefs.h"
#include "nz_serDataPorts.h"
#include "nz_xflashDefsSbc66.h"
#include "cmd.h"

//Includes required for USB
#include "USB\usb.h"
#if defined(HAS_SERPORT_USB_CDC)
    #include "USB\usb_function_cdc.h"
#elif defined(HAS_SERPORT_USB_HID)
    #include "USB\usb_function_hid.h"
#endif

//Includes required for Webserver
#if defined(HAS_WEBSERVER)
#include "TCPIP Stack/HTTP2.h"
#endif


//Add debugging to this file. The DEBUG_CONF_SERPORTUSB macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_SERPORTUSB)
    #define DEBUG_CONF_SERPORTUSB       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_SERPORTUSB
#include "nz_debug.h"

#if defined(HAS_SERPORT_USB_CDC)
    char USB_Out_Buffer[64];
#elif defined(HAS_SERPORT_USB_HID)
    PacketToFromPC PacketFromPC;            //64 byte buffer for receiving packets on EP1 OUT from the PC
    PacketToFromPC PacketToPC;              //64 byte buffer for sending packets on EP1 IN to the PC
    PacketToFromPC PacketFromPCBuffer;
    USB_HANDLE USBOutHandle = 0;
    USB_HANDLE USBInHandle = 0;
    unsigned char usbState;
#endif


/**
 * Structure containing USB Serport Flags
 */
typedef struct __attribute__((aligned(2), packed))
{
    union {
        struct
        {
            unsigned int bDeviceInfoCmdSent : 1;        //Indicates if host has already requested "Device Info"
            unsigned int bFill : 15;                    //Fill to ensure flags take up a 16-bit WORD
        } flags;
        WORD flagsVal;
    };
} SERPORT_USB_FLAGS;

SERPORT_USB_FLAGS serportUsbFlags;

//Function Prototypes
extern void blinkUSBStatus(void);


#if defined(HAS_SERPORT_USB)

#define COMMANDS_USE_USBHID 1

/**
 * USB Task
 */
void serUsbProcessCmd(void) {
    BYTE command;
    BOOL done = TRUE;

    typedef enum {
        SM_SERUSB_IDLE = 0,
        SM_SERUSB_COMMAND,
        SM_SERUSB_COMMAND_WAIT_REPLY
    } SM_SER_USB;
    static SM_SER_USB smSerUSB = 0;

    static BYTE sizeData;

    do {
    switch (smSerUSB) {
        case SM_SERUSB_IDLE:
            //Check if anything in CIRBUF_RX_USB
            if (cbufGetCount(CIRBUF_RX_USB)>2) {
                //At this smSerUSB the CIRBUF_RX_USB buffer should contain a USB message. It is stored
                //as Packet (buffer type is CIRBUF_TYPE_PACKET). The buffer contains [Size][Command][Data].

                //Check for error. Can remove this code later maybe.
                //If error, empty buffer, and return
                //#if (MY_DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
                //if (cbufGetCount(CIRBUF_RX_USB)<3) {
                //    DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nserUSBTask buf error!");
                //    cbufEmpty(CIRBUF_RX_USB);
                //    return;
                //}
                //#endif

                //Buffer contains [Size][Command][Data]
                //Buffer contains a CIRBUF_TYPE_PACKET Packet = [Size 8-Bit][Data]. The [Data] part contains the original USB message [Command] and [Data]
                sizeData = cbufGetByte_MACRO(CIRBUF_RX_USB);    //cbufGetByte(CIRBUF_RX_USB) = Size of [Command] and [Data].
                sizeData--;                                     //Adjust to size of Data only
                command = cbufGetByte_MACRO(CIRBUF_RX_USB);     //MACRO version, ONLY use as expression, like "c=cbufGetByte_MACRO(pBuf);" !

                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nserUSBTask HD, sz=");
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, sizeData);
                //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, " cmd=");
                //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, command);

                if ((command == CMDUSB_COMMAND) && COMMANDS_USE_USBHID) {
                    //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nserUSBTask CMD Fnd");
                    done=FALSE;
                    smSerUSB = SM_SERUSB_COMMAND;
                }
                //If unknown command, or we are not configured to handle it, remove from buffer
                else {
                    cbufRemoveBytes(CIRBUF_RX_USB, sizeData);
                }
            }
            break;
        case SM_SERUSB_COMMAND:
            {
            WORD_VAL ret;
            //USB Receive buffer now contains only [Data] part of original message received via USB. sizeData gives it's size.

            //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nserUSBTask CMD, sz=");
            //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, sizeData);

            done=TRUE;

            //Check if cmd Module is ready to execute commands. If not, return and check again later.
            if (cmdReadyToExec() == FALSE) {
                return;     //Return, we have to execute a command
            }

            //Parse all bytes contained in Packed in USB Buffer (CIRBUF_RX_USB)
            ret = cmdParseCirbuf(CIRBUF_RX_USB, sizeData, CIRBUF_TX_USB, CMDUSB_REQUEST_COMMAND);

            //Update how many bytes are still contained in buffer for this packet. The CIRBUF_RX_USB is a Packet (CIRBUF_TYPE_PACKET) type
            //buffer. We have to ensure we remove all bytes, so buffer will correctly point to start of next packet when done!
            sizeData -= ret.byte.LB;

            // TODO - When ret.byte.HB=0, we could still be waiting for a reply command. Find way to do this!

            //All commands processed and finished
            if (ret.byte.HB == 0) {
                smSerUSB = SM_SERUSB_IDLE;
                if (sizeData != 0) {
                    cbufRemoveBytes(CIRBUF_RX_USB, sizeData);
                }
            }
            //All commands NOT processed and finished. Go to SM_SERUSB_COMMAND_WAIT_REPLY and wait till all done.
            else {
                smSerUSB++;
                //State not implemented in code yet, send error!!!
                DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nserUSBTask E2");
            }
            }
            break;
        case SM_SERUSB_COMMAND_WAIT_REPLY:
            //We are waiting for a reply from cmd.c module for commands it is processing for us
            if (cmdReplyReady() == TRUE) {
                //Get reply, pass out buffer.
                //cmdGetReply(CIRBUF_RX_USB);

                smSerUSB = SM_SERUSB_COMMAND;
            }
            break;
    }                       // switch (smSerUSB) {
    } while (done==FALSE);  // while (done==FALSE) {
}
#endif //#if defined(HAS_SERPORT_USB)


/**
 * USB Task. Will do following:
 * - Check CIRBUF_TX_USB if there is anything to send. If so, sends it on USB port
 * - Check USB Stack if anything received. If so, will check what was received.
 *   If debug message, adds it to CIRBUF_RX_DEBUG. If command, adds it to CIRBUF_RX_USB.
 */
void serUSBTask(void) {
    BYTE i, size;
    //static WORD cntTest = 0;
    //char cbuf[16];

    #if (defined HAS_SERPORT_USB_CDC)
    BYTE numBytesRead;
    WORD len;
    BYTE buff[100];
    #endif

    nzGlobals.wdtFlags.bits.serUSB = 1;

    //Blink the LEDs according to the USB device status
    blinkUSBStatus();

    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE) || USBIsDeviceSuspended()) return;


    /////////////////////////////////////////////////
    // Process USB if configured, and not suspended
    #if (defined HAS_SERPORT_USB_CDC)
        //Check if anything received on USB CDC "Serial Data Port".
        numBytesRead = getsUSBUSART(USB_Out_Buffer, 64);
        if (numBytesRead != 0) {

            /////////////////////////////////////////////////
            //Add all bytes received via CDC to Debug "Serial Data Port" Receive buffer
            cbufPutArray(CIRBUF_RX_DEBUG, (BYTE*) USB_Out_Buffer, numBytesRead);
        }

        //Check if anything to send on USB. Use while loop because there could be 2 contiguous blocks of data.
        if (cbufHasData(CIRBUF_TX_USB)) {
            //Is CDC ready to send data
            if (USBUSARTIsTxTrfReady()) {
                //buff[0] = cbufGetByte(CIRBUF_TX_USB);
                //putUSBUSART( (char*)&buff[0], 1);

                //Get number of contiguous bytes available for reading
                len = cbufGetRdArrSize(CIRBUF_TX_USB);

                //Call putUSBUSART() with CIRBUF array. putUSBUSART can except maximum length of 255 bytes.
                if (len > 255) len = 255;

                if (len > 0) {
                    putUSBUSART((char*) cbufGetRdArr(CIRBUF_TX_USB), len);
                    //Now remove bytes from buffer
                    cbufRemoveBytes(CIRBUF_TX_USB, len);
                }
            }
        }

        //Handles device-to-host transaction(s)
        CDCTxService();

    #elif (defined HAS_SERPORT_USB_HID)

        /////////////////////////////////////////////////
        //Check if anything has been received

        //Ensure we are done sending the last response. Have to check this because we might have to send
        //a reply to host depending on what we receive
        if(!HIDTxHandleBusy(USBInHandle))
        {
            //Did we receive a command?
            if(!HIDRxHandleBusy(USBOutHandle))
            {
                //Check if it is a command we handle here in this function, normally lower level commands.
                //Higher level commands, like CMDUSB_COMMAND, is added to buffer, and handled later, they could potentially
                //take time to execute.
                switch(PacketFromPC.Command)
                {
                    //USB HID is used for debugging! Data received with CMDUSB_DEBUG_MESSAGE command is not added
                    //to CIRBUF_RX_USB, but to debug receive buffer!
                    #if defined(HAS_USBHID_DEBUGGING)
                    //Debug message. Add the received debug message to the debug buffer. Is handled by "debug.c".
                    case CMDUSB_DEBUG_MESSAGE:
                        //Debug messages are always NULL terminated strings. This means PacketFromPC.Size = string lenght + 1 (for NULL)
                        #if defined(DEBUG_USE_STREAMING)
                            //Ensure enough size for data
                            if (cbufGetFree(CIRBUF_RX_DEBUG) > (PacketFromPC.Size)) {
                                //Add all bytes received via DEBUG MESSAGE to Debug "Serial Data Port" Receive buffer. If too little space, remaining bytes are lost!
                                cbufPutArray(CIRBUF_RX_DEBUG, (BYTE*) &PacketFromPC.Data[0], PacketFromPC.Size);
                            }
                        #else
                            //Add debug string as a packet. Don't included NULL string terminator, not needed for "Packet Circular Buffer"
                            if (PacketFromPC.Size > 0) {
                                cbufPutPacket(CIRBUF_RX_DEBUG, (BYTE*) &PacketFromPC.Data[0], PacketFromPC.Size - 1);    //"-1" to remove NULL string terminator
                            }
                        #endif
                        break;
                    #endif
                    case CMDUSB_DEVICE_INFO:
                        PacketToPC.Command = PacketFromPC.Command;  //Build reply message

                        PacketToPC.DeviceInfo.BoardID = USBHID_BOARD_ID_MAIN;
                        PacketToPC.DeviceInfo.BoardRev = USBHID_BOARD_REV;

                        USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&PacketToPC.Contents[0],64);
                        serportUsbFlags.flags.bDeviceInfoCmdSent = TRUE;
                        break;
                    case CMDUSB_RESET_DEVICE:
                    {
                        cmdReboot();
                        break;
                    }
                    //Not handled here, add to USB buffer
                    default:
                        //Add to USB buffer. It is added as a Packet (see nz_serUSB.h for details on "Circular Buffer Packets").
                        //A "Circular Buffer Packet" has the format [Size][Data]. We store PacketFromPC.Command and PacketFromPC.Data
                        //in the [Data] part of the packet.
                        size = PacketFromPC.Size+1; //Packet [Size] = PacketFromPC.Command + PacketFromPC.Data (PacketFromPC.Size is just for PacketFromPC.Data)
                        //!!!! Don't use PacketFromPC.Size any more after next line, it is overwritten! It is PacketFromPC.UsbCommandData[0]
                        PacketFromPC.UsbCommandData[0] = PacketFromPC.Command; //Make Command second byte in PacketFromPC.Contents. Data starts from third byte.
                        cbufPutPacket(CIRBUF_RX_USB, (BYTE*) &PacketFromPC.UsbCommandData[0], size);

                        //DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nUSBTask CMD, size=");
                        //DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, size);
                        break;
                }

                //Re-arm the OUT endpoint for the next packet
                USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&PacketFromPC,64);
            }
        }


        /////////////////////////////////////////////////
        //Check if there is anything to send
        // - Application configured debug message to be sent via USB HID port
        // - There is "Debug Message" to send AND "Device Info" has already been sent.

        //Ensure we are done sending the last response.
        if(!HIDTxHandleBusy(USBInHandle))
        {
            WORD sizeData;

            #if defined(HAS_USBHID_DEBUGGING)
            if (cbufHasData(CIRBUF_TX_DEBUG) && serportUsbFlags.flags.bDeviceInfoCmdSent) {
                //Set size to maximum bytes that can be sent per HID USB message
                size = sizeof(PacketToPC.Data);
                //There are less bytes to send than bytes we ALWAYS send in standard USB message
                if (size > cbufGetCount(CIRBUF_TX_DEBUG)) {
                    size = cbufGetCount(CIRBUF_TX_DEBUG);
                }
                PacketToPC.Size = size;

                PacketToPC.Command = CMDUSB_DEBUG_MESSAGE;          //Command
                for (i=0; i<size; i++) {
                    PacketToPC.Data[i] = cbufGetByte_MACRO(CIRBUF_TX_DEBUG); //Read next byte from buffer, MACRO version, ONLY use as expression, like "c=cbufGetByte_MACRO(pBuf);" !
                }

                USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&PacketToPC.Contents[0],64);
            }
            else {
            #endif
            //The USB Transmit buffer (CIRBUF_TX_USB) is a Packet buffer. Send each packet in a USB message.
            if ( (sizeData=cbufGetPacketDataSize(CIRBUF_TX_USB)) != 0) {

                //The message we sent to USB device consists of [Command][Size][Data].
                //The CIRBUF_TX_USB packet contains the [Command] and [Data] part (their joint size = sizeData)
                //The size of the [Data] part is sizeData -1. sizeData must have a value of at least 1 bytes, seeing
                //that the [Data] part can have a size of 0 - not be there.
                sizeData--;    //Get size of [Data] part

                #define cbufEmpty(pBuf)  {(pBuf)->get = (pBuf)->put; /*Don't update put, only GET*/}

                if (sizeData > sizeof(PacketToPC.Data)) {
                    DEBUG_PUT_STR(DEBUG_LEVEL_WARNING, "USB Tx Pckt too big");
                    cbufEmpty(CIRBUF_TX_USB);
                    return;
                }

                //Set size of [Data] part of message HID USB message
                PacketToPC.Size = sizeData;
                //PacketToPC.Command = CMDUSB_COMMAND;              //Command
                PacketToPC.Command = cbufGetByte_MACRO(CIRBUF_TX_USB);   //First byte in packet is Command, MACRO version, ONLY use as expression, like "c=cbufGetByte_MACRO(pBuf);" !

                for (i=0; i<sizeData; i++) {
                    PacketToPC.Data[i] = cbufGetByte_MACRO(CIRBUF_TX_USB);   //Read next byte from buffer, MACRO version, ONLY use as expression, like "c=cbufGetByte_MACRO(pBuf);" !
                }

                USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&PacketToPC.Contents[0],64);
            }
            #if defined(HAS_USBHID_DEBUGGING)
            }
            #endif
        }       //if(!HIDTxHandleBusy(USBInHandle))
    #endif
}


/**
 * USB Initialization.
 */
void serUSBInit(void)
{
    serportUsbFlags.flagsVal = 0;
    
    //The USB specifications require that USB peripheral devices must never source
    //current onto the Vbus pin.  Additionally, USB peripherals should not source
    //current on D+ or D- when the host/hub is not actively powering the Vbus line.
    //When designing a self powered (as opposed to bus powered) USB peripheral
    //device, the firmware should make sure not to turn on the USB module and D+
    //or D- pull up resistor unless Vbus is actively powered.  Therefore, the
    //firmware needs some means to detect when Vbus is being powered by the host.
    //A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
    //can be used to detect when Vbus is high (host actively powering), or low
    //(host is shut down or otherwise not supplying power).  The USB firmware
    //can then periodically poll this I/O pin to know when it is okay to turn on
    //the USB module/D+/D- pull up resistor.  When designing a purely bus powered
    //peripheral device, it is not possible to source current on D+ or D- when the
    //host is not actively providing power on Vbus. Therefore, implementing this
    //bus sense feature is optional.  This firmware can be made to use this bus
    //sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
    //HardwareProfile.h file.
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
    #endif

    //If the host PC sends a GetStatus (device) request, the firmware must respond
    //and let the host know if the USB peripheral device is currently bus powered
    //or self powered.  See chapter 9 in the official USB specifications for details
    //regarding this request.  If the peripheral device is capable of being both
    //self and bus powered, it should not return a hard coded value for this request.
    //Instead, firmware should check if it is currently self or bus powered, and
    //respond accordingly.  If the hardware has been configured like demonstrated
    //on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
    //currently selected power source.  On the PICDEM FS USB Demo Board, "RA2"
    //is used for        this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
    //has been defined in HardwareProfile.h, and that an appropriate I/O pin has been mapped
    //to it in HardwareProfile.h.
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN; // See HardwareProfile.h
    #endif

    USBDeviceInit(); //usb_device.c.  Initializes USB module SFRs and firmware

    #if defined(HAS_SERPORT_USB_CDC)
        //....
    #elif defined(HAS_SERPORT_USB_HID)
        //initialize the variable holding the handle for the last transmission
        USBOutHandle = 0;
        USBInHandle = 0;

        usbState = IDLE_STATE;
    #endif
}//end


/**
 * This function indicates if the USB module is in suspend mode.  This function
 * does NOT indicate that a suspend request has been received.  It only
 * reflects the state of the USB module.
 *
 * @return Returns true if the USB modules is suspended.
 */
BOOL serUSBIsSuspended(void) {
	return ((USBDeviceState < CONFIGURED_STATE) || USBIsDeviceSuspended());
}

/**
 * Call back that is invoked when a USB suspend is detected
 */
void USBCBSuspend(void) {
    //Example power saving code.  Insert appropriate code here for the desired
    //application behavior.  If the microcontroller will be put to sleep, a
    //process similar to that shown below may be used:

    //ConfigureIOPinsForLowPower();
    //SaveStateOfAllInterruptEnableBits();
    //DisableAllInterruptEnableBits();
    //EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
    //Sleep();
    //RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
    //RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

    //IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is
    //cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause
    //things to not work as intended.

#if defined(__C30__)
	//Disable sleep on suspend!
    //USBSleepOnSuspend();
#endif
}


/**
 * The host may put USB peripheral devices in low power suspend mode (by "sending" 3+ms
 * of idle). Once in suspend mode, the host may wake the device back up by sending
 * non-idle state signalling. This call back is invoked when a wakeup from USB suspend
 * is detected.
 */
void USBCBWakeFromSuspend(void) {
    // If clock switching or other power savings measures were taken when
    // executing the USBCBSuspend() function, now would be a good time to
    // switch back to normal full power run mode conditions.  The host allows
    // a few milliseconds of wakeup time, after which the device must be
    // fully back to normal, and capable of receiving and processing USB
    // packets.  In order to do this, the USB module must receive proper
    // clocking (IE: 48MHz clock must be available to SIE for full speed USB
    // operation).
}


/**
 * The USB host sends out a SOF packet to full-speed devices every 1 ms. This interrupt
 * may be useful for isochronous pipes. End designers should implement callback routine
 * as necessary.
 */
void USBCB_SOF_Handler(void) {
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}


/**
 * The purpose of this callback is mainly for debugging during development. Check UEIR
 * to see which error causes the interrupt.
 */
void USBCBErrorHandler(void) {
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

    // Typically, user firmware does not need to do anything special
    // if a USB error occurs.  For example, if the host sends an OUT
    // packet to your device, but the packet gets corrupted (ex:
    // because of a bad connection, or the user unplugs the
    // USB cable during the transmission) this will typically set
    // one or more USB error interrupt flags.  Nothing specific
    // needs to be done however, since the SIE will automatically
    // send a "NAK" packet to the host.  In response to this, the
    // host will normally retry to send the packet again, and no
    // data loss occurs.  The system will typically recover
    // automatically, without the need for application firmware
    // intervention.

    // Nevertheless, this callback function is provided, such as
    // for debugging purposes.
}


/**
 * When SETUP packets arrive from the host, some firmware must process the request and
 * respond appropriately to fulfill the request. Some of the SETUP packets will be for
 * standard USB "chapter 9" (as in, fulfilling chapter 9 of the official USB specifications)
 * requests, while others may be specific to the USB device class that is being implemented.
 * For example, a HID class device needs to be able to respond to "GET REPORT" type of requests.
 * This is not a standard USB chapter 9 request, and therefore not handled by usb_device.c.
 * Instead this request should be handled by class specific firmware, such as that contained in
 * usb_function_hid.c.
 */
void USBCBCheckOtherReq(void) {
    #if defined(HAS_SERPORT_USB_CDC)
        USBCheckCDCRequest();
    #elif defined(HAS_SERPORT_USB_HID)
        USBCheckHIDRequest();
    #endif
}//end


/**
 * The USBCBStdSetDscHandler() callback function is called when a SETUP, bRequest: SET_DESCRIPTOR
 * request arrives. Typically SET_DESCRIPTOR requests are not used in most applications, and it is
 * optional to support this type of request.
 */
void USBCBStdSetDscHandler(void) {
    // Must claim session ownership if supporting this request
}//end


/**
 * This function is called when the device becomes initialized, which occurs after the host
 * sends a SET_CONFIGURATION (wValue not = 0) request. This callback function should
 * initialize the endpoints for the device's usage according to the current configuration.
 */
void USBCBInitEP(void) {
    #if defined(HAS_SERPORT_USB_CDC)
        CDCInitEP();
    #elif defined(HAS_SERPORT_USB_HID)
        //enable the HID endpoint
        USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

        //Arm the OUT endpoint for the first packet
        //USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&PacketFromPCBuffer,64);
        USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&PacketFromPC,64);
    #endif
}


/**
 * The USB specifications allow some types of USB peripheral devices to wake up a host PC (such
 * as if it is in a low power suspend to RAM state). This can be a very useful feature in some
 * USB applications, such as an Infrared remote control receiver.  If a user presses the "power"
 * button on a remote control, it is nice that the IR receiver can detect this signalling, and then
 * send a USB "command" to the PC to wake up.
 *
 * The usbCBSendResume() "callback" function is used to send this special USB signalling which wakes
 * up the PC.  This function may be called by application firmware to wake up the PC.  This
 * function will only be able to wake up the host if all of the below are true:
 *
 * 1.  The USB driver used on the host PC supports the remote wakeup capability.
 * 2.  The USB configuration descriptor indicates the device is remote wakeup capable in the
 *      bmAttributes field.
 * 3.  The USB host PC is currently sleeping, and has previously sent your device a SET
 *      FEATURE setup packet which "armed" the remote wakeup capability.
 *
 * If the host has not armed the device to perform remote wakeup, then this function will return without
 * actually performing a remote wakeup sequence.  This is the required behavior, as a USB device that
 * has not been armed to perform remote wakeup must not drive remote wakeup signalling onto the bus;
 * doing so will cause USB compliance testing failure.
 *
 * This callback should send a RESUME signal that has the period of 1-15ms.
 *
 * Note: This function does nothing and returns quickly, if the USB bus and host are not in a
 * suspended condition, or are otherwise not in a remote wakeup ready state. Therefore, it
 * is safe to optionally call this function regularly, ex: anytime application stimulus occurs,
 * as the function will have no effect, until the bus really is in a state ready
 * to accept remote wakeup.
 *
 * When this function executes, it may perform clock switching, depending upon the application
 * specific code in USBCBWakeFromSuspend(). This is needed, since the USB bus will no longer
 * be suspended by the time this function returns.  Therefore, the USB module will need to be ready
 * to receive traffic from the host.
 *
 * The modifiable section in this routine may be changed to meet the application needs.
 * Current implementation temporary blocks other functions from executing for a
 * period of ~3-15 ms depending on the core frequency.
 *
 * According to USB 2.0 specification section 7.1.7.7, "The remote wakeup device must
 * hold the resume signaling for at least 1 ms but for no more than 15 ms." The idea here
 * is to use a delay counter loop, using a common value that would work over a wide range of core
 * frequencies. That value selected is 1800. See table below:
 * ==========================================================
 * Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 * ==========================================================
 *      48              12          1.05
 *       4              1           12.6
 * ==========================================================
 * These timing could be incorrect when using code optimization or extended instruction mode,
 * or when having other interrupts enabled. Make sure to verify using the MPLAB SIM's Stopwatch
 * and verify the actual signal on an oscilloscope.
 */
void usbCBSendResume(void) {
    static WORD delay_count;

    //First verify that the host has armed us to perform remote wakeup.
    //It does this by sending a SET_FEATURE request to enable remote wakeup,
    //usually just before the host goes to standby mode (note: it will only
    //send this SET_FEATURE request if the configuration descriptor declares
    //the device as remote wakeup capable, AND, if the feature is enabled
    //on the host (ex: on Windows based hosts, in the device manager
    //properties page for the USB device, power management tab, the
    //"Allow this device to bring the computer out of standby." checkbox
    //should be checked).
    if (USBGetRemoteWakeupStatus() == TRUE) {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if (USBIsBusSuspended() == TRUE) {
            USBMaskInterrupts();

            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0;
            USBBusIsSuspended = FALSE; //So we don't execute this code again,
            //until a new suspend condition is detected.

            //Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            //device must continuously see 5ms+ of idle on the bus, before it sends
            //remote wakeup signalling.  One way to be certain that this parameter
            //gets met, is to add a 2ms+ blocking delay here (2ms plus at
            //least 3ms from bus idle to USBIsBusSuspended() == TRUE, yeilds
            //5ms+ total delay since start of idle).
            delay_count = 3600U;
            do {
                delay_count--;
            } while (delay_count);

            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1; // Start RESUME signaling
            delay_count = 1800U; // Set RESUME line for 1-13 ms
            do {
                delay_count--;
            } while (delay_count);
            USBResumeControl = 0; //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}


/**
 * This function is called whenever a EP0 data packet is received. This gives the
 * user (and thus the various class examples a way to get data that is received via
 * the control endpoint. This function needs to be used in conjunction with the
 * USBCBCheckOtherReq() function since the USBCBCheckOtherReq() function is the apps
 * method for getting the initial control transfer before the data arrives.
 *
 * PreCondition: ENABLE_EP0_DATA_RECEIVED_CALLBACK must be defined already (in usb_config.h)
 */
#if defined(ENABLE_EP0_DATA_RECEIVED_CALLBACK)
void USBCBEP0DataReceived(void) {
}
#endif


/**
 * This function is called from the USB stack to  notify a user application that a USB event
 * occurred.  This callback is in interrupt context when the USB_INTERRUPT option is selected.
 *
 * @param event the type of event
 * @param pdata pointer to the event data
 * @param size size of the event data
 *
 * @ret None
 */
BOOL USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size) {
    switch (event) {
        case EVENT_TRANSFER:
            //Add application specific callback task or callback function here if desired.
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED:
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER_TERMINATED:
            //Add application specific callback task or callback function here if desired.
            //The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
            //FEATURE (endpoint halt) request on an application endpoint which was
            //previously armed (UOWN was = 1).  Here would be a good place to:
            //1.  Determine which endpoint the transaction that just got terminated was
            //      on, by checking the handle value in the *pdata.
            //2.  Re-arm the endpoint if desired (typically would be the case for OUT
            //      endpoints).
            break;
        default:
            break;
    }
    return TRUE;
}
