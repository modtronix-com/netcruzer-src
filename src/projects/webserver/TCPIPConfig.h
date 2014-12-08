/*********************************************************************
 *
 *    Microchip TCP/IP Stack Demo Application Configuration Header Stub
 *
 *********************************************************************
 * FileName:        TCPIPConfig.h
 * Dependencies:    Microchip TCP/IP Stack
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.10 or higher
 *                    Microchip C30 v3.12 or higher
 *                    Microchip C18 v3.34 or higher
 *                    HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *        ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *        used in conjunction with a Microchip ethernet controller for
 *        the sole purpose of interfacing with the ethernet controller.
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
 * Howard Schlunder        09/21/2010    Converted to a stub
 ********************************************************************/
#ifndef TCPIPCONFIG_STUB_H
#define TCPIPCONFIG_STUB_H

#if defined(CUSTOM_PROJECT)
    #include "CustomFiles/TCPIP_custom.h"
#endif


// =======================================================================
//   Defines not present in TCPIP_xxx.h file
// =======================================================================
#if !defined(MY_DEFAULT_DHCP_ENABLED)
#define MY_DEFAULT_DHCP_ENABLED         (1)
#endif



// =======================================================================
//   Select TCPIP_xxx.h file (created by config tool)
// =======================================================================
#if !defined(DONT_INCLUDE_BOARD_TCPIP_FILE)
// These definitions are set in the MPLAB X Project settings.
#if     defined(BRD_SBC66EC_R1)
        #if defined(CFG_INCLUDE_TCP_200_PERF)
            #include "Configs/TCPIP_SBC66EC-R1_TCP_200_PERF.h"
        #elif defined(CFG_INCLUDE_TCP_2000_PERF)
            #include "Configs/TCPIP_SBC66EC-R1_TCP_2000_PERF.h"
        #elif defined(CFG_INCLUDE_UDP_PERF)
            #include "Configs/TCPIP_SBC66EC-R1_UDP_PERF.h"
        #else
            #include "Configs/TCPIP_SBC66EC-R1.h"
        #endif
#elif     defined(BRD_SBC66EC_R2)
        #if defined(CFG_INCLUDE_TCP_200_PERF)
            #include "Configs/TCPIP_SBC66EC-R2_TCP_200_PERF.h"
        #elif defined(CFG_INCLUDE_TCP_2000_PERF)
            #include "Configs/TCPIP_SBC66EC-R2_TCP_2000_PERF.h"
        #elif defined(CFG_INCLUDE_UDP_PERF)
            #include "Configs/TCPIP_SBC66EC-R2_UDP_PERF.h"
        #else
            #include "Configs/TCPIP_SBC66EC-R2.h"
        #endif
#elif   defined(BRD_SBC66ECL_R2)
        #if defined(CFG_INCLUDE_TCP_200_PERF)
            #include "Configs/TCPIP_SBC66ECL-R2_TCP_200_PERF.h"
        #elif defined(CFG_INCLUDE_TCP_2000_PERF)
            #include "Configs/TCPIP_SBC66ECL-R2_TCP_2000_PERF.h"
        #elif defined(CFG_INCLUDE_UDP_PERF)
            #include "Configs/TCPIP_SBC66ECL-R2_UDP_PERF.h"
        #else
            #include "Configs/TCPIP_SBC66ECL-R2.h"
        #endif
#else
        #error Missing project macro definition to select proper TCPIPConfig.h
#endif
#endif  //#if !defined(DONT_INCLUDE_BOARD_TCPIP_FILE)


#endif
