/**
 * ===== Description =====
 * This demo uses the selected hardware platform as a USB MIDI device. Connect the device to the computer.
 * Open a MIDI recording software package. Each MIDI recording software interface is different so the following
 * instructions may not apply the to software package you are using. Please refer to the user’s manual for the
 * software package you are using for more details of how to configure that tool for a USB MIDI input.
 * In this demo each time you press the button on the board, it will cycle through a series of notes.
 * The port that button is connected to is Netcruzer Port 5 (old port name X5). The button must short port 5 to ground
 * pressed. To change port, change following line in HardwareProfile.h:
 * "#define sw2 PIN_05"
 *
 * ===== Building Project =====
 * To compile for Netcruzer Board, open this project in MPLAB X, and select the "Project Configuration"
 * for desired board. For example "SBC66ECL_R2" for the SBC66ECL Revision 2 board.
 *
 * ===== Programming Board =====
 * After compiling (build), Netcruzer board can be programmed via a PIC programmer, or bootloader.
 * For details on updating firmware with USB bootloader, see http://netcruzer.com/nz/doc/update_firmware_usb/
 * For details on programming board with PIC programmer, see http://www.modtronix.com/picboards/prog/
 *
 **********************************************************************
 * File History
 *
 * 2012-08-08, David H. (DH):
 *    - Initial version, ported from Microchip MAL example
 *********************************************************************/
#define THIS_IS_MAIN_FILE   //Uniquely identifies this as the file with the main application entry function main()

/** INCLUDES *******************************************************/
#include "HardwareProfile.h"            //Required for all Netcruzer projects
#include "./USB/usb.h"
#include "./USB/usb_function_midi.h"

/** VARIABLES ******************************************************/
#define RX_BUFFER_ADDRESS_TAG
#define TX_BUFFER_ADDRESS_TAG
#define MIDI_EVENT_ADDRESS_TAG

unsigned char ReceivedDataBuffer[64] RX_BUFFER_ADDRESS_TAG;
unsigned char ToSendDataBuffer[64] TX_BUFFER_ADDRESS_TAG;
USB_AUDIO_MIDI_EVENT_PACKET midiData MIDI_EVENT_ADDRESS_TAG;


USB_HANDLE USBTxHandle = 0;
USB_HANDLE USBRxHandle = 0;


USB_VOLATILE BYTE msCounter;

/** PRIVATE PROTOTYPES *********************************************/
void blinkUSBStatus(void);
BOOL Switch2IsPressed(void);
BOOL Switch3IsPressed(void);
static void InitializeSystem(void);
void ProcessIO(void);
void UserInit(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();
void usbCBSendResume(void);
WORD_VAL ReadPOT(void);



/********************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *******************************************************************/
int main(void)
{
    InitializeSystem();

    #if defined(USB_INTERRUPT)
        USBDeviceAttach();
    #endif

    while(1)
    {
        #if defined(USB_POLLING)
		// Check bus status and service USB interrupts.
        USBDeviceTasks(); // Interrupt or polling method.  If using polling, must call
        				  // this function periodically.  This function will take care
        				  // of processing and responding to SETUP transactions 
        				  // (such as during the enumeration process when you first
        				  // plug in).  USB hosts require that USB devices should accept
        				  // and process SETUP packets in a timely fashion.  Therefore,
        				  // when using polling, this function should be called 
        				  // regularly (such as once every 1.8ms or faster** [see 
        				  // inline code comments in usb_device.c for explanation when
        				  // "or faster" applies])  In most cases, the USBDeviceTasks() 
        				  // function does not take very long to execute (ex: <100 
        				  // instruction cycles) before it returns.
        #endif


		// Application-specific tasks.
		// Application related code may be added here, or in the ProcessIO() function.
        ProcessIO();
    }//end while
}//end main


/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.
 *
 * Note:            None
 *******************************************************************/
static void InitializeSystem(void)
{
    nzSysInitDefault();     //Default initialization. All ports inputs. All analog features disabled

//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	HardwareProfile.h file.
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
    #endif

//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2"
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in HardwareProfile - (platform).h, and that an appropriate I/O pin 
//  has been mapped	to it.
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;	// See HardwareProfile.h
    #endif

    UserInit();

    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
}//end InitializeSystem



/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the demo code
 *                  initialization that is required.
 *
 * Note:
 *
 *****************************************************************************/
void UserInit(void)
{
    //Initialize all of the LED pins
    mInitAllLEDs();

    //Initialize all of the push buttons
    mInitAllSwitches();

    //initialize the variable holding the handle for the last
    // transmission
    USBTxHandle = NULL;
    USBRxHandle = NULL;
}//end UserInit

/********************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user
 *                  routines. It is a mixture of both USB and
 *                  non-USB tasks.
 *
 * Note:            None
 *******************************************************************/
void ProcessIO(void)
{
    static BYTE pitch = 0x3C;
    static BOOL sentNoteOff = TRUE;

    //Blink the LEDs according to the USB device status
    blinkUSBStatus();

    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;

    if(!USBHandleBusy(USBRxHandle))
    {
        //We have received a MIDI packet from the host, process it and then
        //  prepare to receive the next packet

        //INSERT MIDI PROCESSING CODE HERE

        //Get ready for next packet (this will overwrite the old data)
        USBRxHandle = USBRxOnePacket(MIDI_EP,(BYTE*)&ReceivedDataBuffer,64);
    }

    if(sw2==0)
    {
        if(msCounter == 0)
        {
            if(sentNoteOff == TRUE)
            {
                if(!USBHandleBusy(USBTxHandle))
                {
                    //Debounce counter for 100ms
                    msCounter = 100;

                    midiData.Val = 0;   //must set all unused values to 0 so go ahead
                                        //  and set them all to 0

                    midiData.CableNumber = 0;
                    midiData.CodeIndexNumber = MIDI_CIN_NOTE_ON;
                    midiData.DATA_0 = 0x90;         //Note on
                    midiData.DATA_1 = pitch;         //pitch
                    midiData.DATA_2 = 0x7F;  //velocity

                    USBTxHandle = USBTxOnePacket(MIDI_EP,(BYTE*)&midiData,4);
                    sentNoteOff = FALSE;
                }
            }
        }
    }
    else
    {
        if(msCounter == 0)
        {
            if(sentNoteOff == FALSE)
            {
                if(!USBHandleBusy(USBTxHandle))
                {
                    //Debounce counter for 100ms
                    msCounter = 100;

                    midiData.Val = 0;   //must set all unused values to 0 so go ahead and set them all to 0

                    midiData.CableNumber = 0;
                    midiData.CodeIndexNumber = MIDI_CIN_NOTE_ON;
                    midiData.DATA_0 = 0x90;     //Note off
                    midiData.DATA_1 = pitch++;     //pitch
                    midiData.DATA_2 = 0x00;        //velocity

                    if(pitch == 0x49)
                    {
                        pitch = 0x3C;
                    }

                    USBTxHandle = USBTxOnePacket(MIDI_EP,(BYTE*)&midiData,4);
                    sentNoteOff = TRUE;
                }
            }
        }
    }

}//end ProcessIO


/**
 * blinkUSBStatus turns on and off LEDs corresponding to the USB device state.
 * Note: USBDeviceState is declared and updated in usb_device.c.
 */
void blinkUSBStatus(void)
{
    static WORD led_count=0;

    if(led_count == 0)led_count = 64000U;
    led_count--;

    #define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
    #define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
    #define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
    #define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}

    if(USBSuspendControl == 1)
    {
        if(led_count==0)
        {
            mLED_1_Toggle();
            if(mGetLED_1())
            {
                mLED_2_On();
            }
            else
            {
                mLED_2_Off();
            }
        }//end if
    }
    else
    {
        if(USBDeviceState == DETACHED_STATE)
        {
            mLED_Both_Off();
        }
        else if(USBDeviceState == ATTACHED_STATE)
        {
            mLED_Both_On();
        }
        else if(USBDeviceState == POWERED_STATE)
        {
            mLED_Only_1_On();
        }
        else if(USBDeviceState == DEFAULT_STATE)
        {
            mLED_Only_2_On();
        }
        else if(USBDeviceState == ADDRESS_STATE)
        {
            if(led_count == 0)
            {
                mLED_1_Toggle();
                mLED_2_Off();
            }//end if
        }
        else if(USBDeviceState == CONFIGURED_STATE)
        {
            if(led_count==0)
            {
                mLED_1_Toggle();
                if(mGetLED_1())
                {
                    mLED_2_Off();
                }
                else
                {
                    mLED_2_On();
                }
            }//end if
        }//end if(...)
    }//end if(UCONbits.SUSPND...)

}//end blinkUSBStatus




// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA* each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The usbCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

// Note *: The "usb_20.pdf" specs indicate 500uA or 2.5mA, depending upon device classification. However,
// the USB-IF has officially issued an ECN (engineering change notice) changing this to 2.5mA for all 
// devices.  Make sure to re-download the latest specifications to get all of the newest ECNs.


/**
 * Call back that is invoked when a USB suspend is detected
 */
void USBCBSuspend(void)
{
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


   
}


/**
 * The host may put USB peripheral devices in low power suspend mode (by "sending" 3+ms
 * of idle). Once in suspend mode, the host may wake the device back up by sending
 * non-idle state signalling. This call back is invoked when a wakeup from USB suspend
 * is detected.
 */
void USBCBWakeFromSuspend(void)
{
	// If clock switching or other power savings measures were taken when
	// executing the USBCBSuspend() function, now would be a good time to
	// switch back to normal full power run mode conditions.  The host allows
	// 10+ milliseconds of wakeup time, after which the device must be 
	// fully back to normal, and capable of receiving and processing USB
	// packets.  In order to do this, the USB module must receive proper
	// clocking (IE: 48MHz clock must be available to SIE for full speed USB
	// operation).  
	// Make sure the selected oscillator settings are consistent with USB 
    // operation before returning from this function.
}


/**
 * The USB host sends out a SOF packet to full-speed devices every 1 ms. This interrupt
 * may be useful for isochronous pipes. End designers should implement callback routine
 * as necessary.
 */
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.

    if(msCounter != 0)
    {
        msCounter--;
    }
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
    #if defined(USB_IS_CDC)
        USBCheckCDCRequest();
    #elif defined(USB_IS_HID)
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
    #if defined(USB_IS_CDC)
        CDCInitEP();
    #elif defined(USB_IS_HID)
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
BOOL USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size)
{
    switch( event )
    {
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
