/**
 * @brief           Functions for an Incremental Rotary Encoder
 * @file            nz_encIncRot.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16
 *
 * @section nz_encIncRot_desc Description
 *****************************************
 * Functions for an Incremental Rotary Encoder
 * 
 * @subsection nz_encIncRot_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
// *********************************************************************
// --- Incremental Rotary Encoder Configuration (from nz_encIncRot.h) --
// *********************************************************************
//Circular Buffer size
#define ENC_INC_ROT_BUFSIZE 128     //[-DEFAULT-]

//Resolution in us, must be power of 2 value, ex. 8,16,32,64,128... Is used as timebase for storing time between events
#define ENC_INC_ROT_RESOLUTION 16   //[-DEFAULT-]

//Debounce time in us
#define ENC_INC_ROT_DEBOUNCE 8      //[-DEFAULT-]

 @endcode
 *
 * 
 * @subsection nz_encIncRot_usage Usage
 *****************************************
 * To use this module, the following must be done:
 * - Include nz_encIncRot.h in the c file it is used in.
 * - Add nz_encIncRot.c to the MPLAB project, this is the main driver file.
 * - The following additional files are required by nz_encIncRot.c, and must be added to the project:
 *   nz_netcruzer.c
 * - In projdefs.h, do any nz_encIncRot configuration required. This is done by copying the
 *   configuration section from the nz_encIncRot.h files to projdefs.h.
 * - In code, call rotinc_init() to initiliaze, and incrot_task() repetitively in main loop.
 * - <b>All DONE!</b> Can now use this driver's functions!
 * 
 **********************************************************************
 * @section nz_encIncRot_lic Software License Agreement
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
 * 2014-01-02, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef NZ_ENC_INC_ROT_H
#define NZ_ENC_INC_ROT_H

////////// Defines //////////////////////////////


////////// Default Defines //////////////////////
//Circular Buffer size
#if !defined(ENC_INC_ROT_BUFSIZE)
#define ENC_INC_ROT_BUFSIZE 128
#endif



//typedef struct ENCROT_INFO_
typedef struct __attribute__((__packed__)) ENCROT_INFO_
//typedef struct __attribute__((aligned(2), packed)) ENCROT_INFO_
{
    WORD    bitadrChA;  //Channel A
    WORD    bitadrChB;  //Channel B
    DWORD   oldUticks;  //Old Utick value
    #if (ENC_INC_ROT_BUFSIZE<256)
    BYTE    put;
    #else
    WORD    put;
    #endif
    WORD   buf[ENC_INC_ROT_BUFSIZE];   //Signed 16-bit value
    union {
        struct
        {
            unsigned int oldValA : 1;   ///< Old value of channel A
        } flags;
        BYTE flagVal;
    };
} ENCROT_INFO;


////////// Functions ////////////////////////////

/**
 * Initialization.
 *
 * @param pObj Pointer to ENCROT_INFO structure
 * @param portIdChA The "Port ID" connected to channel A of the encoder. Is a
 * 			number from 0 to 41, also known as the "Netcruzer Port Name".
 * @param portIdChB The "Port ID" connected to channel B of the encoder. Is a
 * 			number from 0 to 41, also known as the "Netcruzer Port Name".
 *
 * @return The status of the Initialization, 0=Success, else error code
 */
BYTE rotinc_init(ENCROT_INFO* pObj, BYTE portIdChA, BYTE portIdChB);


/**
 * Main Task
 *
 * @param pObj Pointer to ENCROT_INFO structure
 */
void rotinc_task(ENCROT_INFO* pObj);

#endif  //#ifndef NZ_ENC_INC_ROT_H
