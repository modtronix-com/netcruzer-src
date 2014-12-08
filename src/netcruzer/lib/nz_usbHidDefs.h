/**
 * @brief           Contains default defines for the Netcruzer USB HID interface.
 * @file            nz_usbHidDefs.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section nz_usbHidDefs_desc Description
 *****************************************
 * Contains hardware specific defines for items NOT on boards.
 *
 **********************************************************************
 * @section nz_usbHidDefs_lic Software License Agreement
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
 * 2012-08-06, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_USBHIDDEFS_H
#define NZ_USBHIDDEFS_H


// Commands /////////////////////////////////////
#define CMDUSB_DEBUG_MESSAGE       0x01     //Debug message. Parameter 1 is message type. Rest is NULL terminated string. When received from
                                            //device, is normally displays in "Netcruzer Terminal" Send a "Command" to the device. Message content is a NULL terminated string.
#define CMDUSB_COMMAND             0x02     //Send a "Command" to the device. Message content is a NULL terminated string.
#define CMDUSB_REQUEST_COMMAND     0x03     //Send a "Request Command" to the device. Message content is a NULL terminated string.
#define CMDUSB_DEVICE_INFO         0x10
#define CMDUSB_RESET_DEVICE        0x11
#define CMDUSB_SYNC                0xA5     //Sync character


//Defines for USB HID Class /////////////////////
//usbState Variable States
#define IDLE_STATE                          0x00
#define NOT_IDLE_STATE                      0x01

#define TOTAL_PACKET_SIZE                   64

//Board IDs
typedef enum _BOARD_ID {
    BOARD_ID_UNKNOWN = 0,
    BOARD_ID_DEFAULT_DEBUG = 1,

    BOARD_ID_SBC66EC_CONFIG = 10,
    BOARD_ID_SBC66EC_MAIN_APP = 11,

    BOARD_ID_SBC66ECL_CONFIG = 20,
    BOARD_ID_SBC66ECL_MAIN_APP = 21,

    BOARD_ID_SBC66ZL_CONFIG = 30,
    BOARD_ID_SBC66ZL_MAIN_APP = 31,

    BOARD_ID_SBC66ZB_CONFIG = 40,
    BOARD_ID_SBC66ZB_MAIN_APP = 41,

    BOARD_ID_SBC66WL_CONFIG = 50,
    BOARD_ID_SBC66WL_MAIN_APP = 51,

    BOARD_ID_SBC66WB_CONFIG = 60,
    BOARD_ID_SBC66WB_MAIN_APP = 61,

    BOARD_ID_SBC32UL_CONFIG = 1000,
    BOARD_ID_SBC32UL_MAIN_APP = 1001,
} BOARD_ID;

#if !defined(DONT_DEFINE_PACKET_TO_FROM_PC)
typedef union __attribute__((packed)) _USB_HID_BOOTLOADER_COMMAND
{
    unsigned char Contents[64];

    //Reply to CMDUSB_DEVICE_INFO command, returns board ID and Revision
    struct __attribute__((packed)) {
        unsigned char Command;
        unsigned char reserved; //Get parameters on WORD boundaries
        WORD BoardID;           //A BOARD_ID value (Is in PacketToFromPC.Data[0] and PacketToFromPC.Data[1] )
        WORD BoardRev;          //Board revision   (Is in PacketToFromPC.Data[2] and PacketToFromPC.Data[3] )
    } DeviceInfo;

    //Structure used Sending and Receiving Data Packets via USB
    struct __attribute__((packed)) {
        unsigned char Command;
        unsigned char Size;                         //Size of Data
        unsigned char Data[TOTAL_PACKET_SIZE - 2];  //Byte buffer, 64-2 = 62 bytes
    };

    //Structure for storing USB command in "Circular Buffer Packet".
    //UsbCommandSize = Size of "Circular Buffer Packet"
    //UsbCommandData = Data of "Circular Buffer Packet"
    struct __attribute__((packed)) {
        unsigned char UsbCommandSize;                           //Not used for most USB commands
        unsigned char UsbCommandData[TOTAL_PACKET_SIZE - 1];    //Byte buffer, 64-1 = 63 bytes
    };
} PacketToFromPC;
#endif  //#if !defined(DONT_DEFINE_PACKET_TO_FROM_PC)


//BoardRev is 1 more than actual revision. This is due to the way it was (accidentally) done
//in first version of bootloader. Keep consistent!
#if defined(BRD_SBC66EC_R1)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66EC_MAIN_APP
    #define USBHID_BOARD_REV 2
#elif defined(BRD_SBC66EC_R2)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66EC_MAIN_APP
    #define USBHID_BOARD_REV 3
#elif defined(BRD_SBC66EC_R3)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66EC_MAIN_APP
    #define USBHID_BOARD_REV 4
#elif defined(BRD_SBC66ECL_R2)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66ECL_MAIN_APP
    #define USBHID_BOARD_REV 3
#elif defined(BRD_SBC66ECL_R3)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66ECL_MAIN_APP
    #define USBHID_BOARD_REV 4
#elif defined(BRD_SBC66ZL_R1)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66ZL_MAIN_APP
    #define USBHID_BOARD_REV 2
#elif defined(BRD_SBC66ZL_R2)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66ZL_MAIN_APP
    #define USBHID_BOARD_REV 3
#elif defined(BRD_SBC66ZB_R1)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66ZB_MAIN_APP
    #define USBHID_BOARD_REV 2
#elif defined(BRD_SBC66ZB_R2)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66ZB_MAIN_APP
    #define USBHID_BOARD_REV 3
#elif defined(BRD_SBC66WL_R1)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66WL_MAIN_APP
    #define USBHID_BOARD_REV 2
#elif defined(BRD_SBC66WL_R2)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66WL_MAIN_APP
    #define USBHID_BOARD_REV 3
#elif defined(BRD_SBC66WB_R1)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66WB_MAIN_APP
    #define USBHID_BOARD_REV 2
#elif defined(BRD_SBC66WB_R2)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC66WB_MAIN_APP
    #define USBHID_BOARD_REV 3
#elif defined(BRD_SBC32UL_R1)
    #define USBHID_BOARD_ID_MAIN BOARD_ID_SBC32UL_MAIN_APP
    #define USBHID_BOARD_REV 3
#else
    #error "No board defined, required for BOARD_ID_Xxxx"
#endif


#endif
