/**
 * @brief           Contains hardware specific defines for items NOT on boards.
 * @file            nz_board_defaults.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section nz_board_defaults_desc Description
 *****************************************
 * Contains hardware specific defines for items NOT on boards.
 *
 **********************************************************************
 * @section nz_board_defaults_lic Software License Agreement
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
#ifndef NZ_BOARD_DEFAULTS_H
#define NZ_BOARD_DEFAULTS_H



// *********************************************************************
// ---- LED & Button Defines Used by Microchip Application Library -----
// *********************************************************************
#ifndef LED0_TRIS
#define LED0_TRIS           (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED1_TRIS
#define LED1_TRIS           (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED2_TRIS
#define LED2_TRIS           (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED3_TRIS
#define LED3_TRIS           (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED4_TRIS
#define LED4_TRIS           (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED5_TRIS
#define LED5_TRIS           (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED6_TRIS
#define LED6_TRIS           (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED7_TRIS
#define LED7_TRIS           (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif

#ifndef LED0_IO
#define LED0_IO             (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED1_IO
#define LED1_IO             (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED2_IO
#define LED2_IO             (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED3_IO
#define LED3_IO             (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED4_IO
#define LED4_IO             (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED5_IO
#define LED5_IO             (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED6_IO
#define LED6_IO             (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif
#ifndef LED7_IO
#define LED7_IO             (((BYTE*)&NVMKEY)[1])   // No such LED, map to dummy register
#endif

#ifndef BUTTON0_TRIS
#define BUTTON0_TRIS        (((BYTE*)&NVMKEY)[1])   // No such button, map to dummy register
#endif
#ifndef BUTTON1_TRIS
#define BUTTON1_TRIS        (((BYTE*)&NVMKEY)[1])   // No such button, map to dummy register
#endif
#ifndef BUTTON2_TRIS
#define BUTTON2_TRIS        (((BYTE*)&NVMKEY)[1])   // No such button, map to dummy register
#endif
#ifndef BUTTON3_TRIS
#define BUTTON3_TRIS        (((BYTE*)&NVMKEY)[1])   // No such button, map to dummy register
#endif
#ifndef BUTTON4_TRIS
#define BUTTON4_TRIS        (((BYTE*)&NVMKEY)[1])   // No such button, map to dummy register
#endif
#ifndef BUTTON5_TRIS
#define BUTTON5_TRIS        (((BYTE*)&NVMKEY)[1])   // No such button, map to dummy register
#endif
#ifndef BUTTON6_TRIS
#define BUTTON6_TRIS        (((BYTE*)&NVMKEY)[1])   // No such button, map to dummy register
#endif
#ifndef BUTTON7_TRIS
#define BUTTON7_TRIS        (((BYTE*)&NVMKEY)[1])   // No such button, map to dummy register
#endif

#ifndef BUTTON0_IO
#define BUTTON0_IO          (1)
#endif
#ifndef BUTTON1_IO
#define BUTTON1_IO          (1)
#endif
#ifndef BUTTON2_IO
#define BUTTON2_IO          (1)
#endif
#ifndef BUTTON3_IO
#define BUTTON3_IO          (1)
#endif
#ifndef BUTTON4_IO
#define BUTTON4_IO          (1)
#endif
#ifndef BUTTON5_IO
#define BUTTON5_IO          (1)
#endif
#ifndef BUTTON6_IO
#define BUTTON6_IO          (1)
#endif
#ifndef BUTTON7_IO
#define BUTTON7_IO          (1)
#endif

#ifndef mInitAllLEDs
#define mInitAllLEDs()
#endif

#ifndef mLED_1
#define mLED_1
#endif
#ifndef mLED_2
#define mLED_2
#endif
#ifndef mLED_3
#define mLED_3
#endif
#ifndef mLED_4
#define mLED_4
#endif

#ifndef mGetLED_1
#define mGetLED_1()         1
#endif
#ifndef mGetLED_2
#define mGetLED_2()         1
#endif
#ifndef mGetLED_3
#define mGetLED_3()         1
#endif
#ifndef mGetLED_4
#define mGetLED_4()         1
#endif

#ifndef mLED_1_On
#define mLED_1_On()
#endif
#ifndef mLED_2_On
#define mLED_2_On()
#endif
#ifndef mLED_3_On
#define mLED_3_On()
#endif
#ifndef mLED_4_On
#define mLED_4_On()
#endif

#ifndef mLED_1_Off
#define mLED_1_Off()
#endif
#ifndef mLED_2_Off
#define mLED_2_Off()
#endif
#ifndef mLED_3_Off
#define mLED_3_Off()
#endif
#ifndef mLED_4_Off
#define mLED_4_Off()
#endif

#ifndef mLED_1_Toggle
#define mLED_1_Toggle()
#endif
#ifndef mLED_2_Toggle
#define mLED_2_Toggle()
#endif
#ifndef mLED_3_Toggle
#define mLED_3_Toggle()
#endif
#ifndef mLED_4_Toggle
#define mLED_4_Toggle()
#endif



// *********************************************************************
// ----------- LED & Button Defines Used by Netcruzer Library ----------
// *********************************************************************
#ifndef DIR_LED0
#define DIR_LED0    LED0_TRIS
#endif
#ifndef DIR_LED1
#define DIR_LED1    LED1_TRIS
#endif
#ifndef DIR_LED2
#define DIR_LED2    LED2_TRIS
#endif
#ifndef DIR_LED3
#define DIR_LED3    LED3_TRIS
#endif
#ifndef DIR_LED4
#define DIR_LED4    LED4_TRIS
#endif
#ifndef DIR_LED5
#define DIR_LED5    LED5_TRIS
#endif
#ifndef DIR_LED6
#define DIR_LED6    LED6_TRIS
#endif
#ifndef DIR_LED7
#define DIR_LED7    LED7_TRIS
#endif
#ifndef DIR_LED8
#define DIR_LED8    LED8_TRIS
#endif

#ifndef LAT_LED0
#define LAT_LED0    (((BYTE*)&NVMKEY)[1])   //No such LED, map to dummy register
#endif
#ifndef LAT_LED1
#define LAT_LED1    (((BYTE*)&NVMKEY)[1])   //No such LED, map to dummy register
#endif
#ifndef LAT_LED2
#define LAT_LED2    (((BYTE*)&NVMKEY)[1])   //No such LED, map to dummy register
#endif
#ifndef LAT_LED3
#define LAT_LED3    (((BYTE*)&NVMKEY)[1])   //No such LED, map to dummy register
#endif
#ifndef LAT_LED4
#define LAT_LED4    (((BYTE*)&NVMKEY)[1])   //No such LED, map to dummy register
#endif
#ifndef LAT_LED5
#define LAT_LED5    (((BYTE*)&NVMKEY)[1])   //No such LED, map to dummy register
#endif
#ifndef LAT_LED6
#define LAT_LED6    (((BYTE*)&NVMKEY)[1])   //No such LED, map to dummy register
#endif
#ifndef LAT_LED7
#define LAT_LED7    (((BYTE*)&NVMKEY)[1])   //No such LED, map to dummy register
#endif
#ifndef LAT_LED8
#define LAT_LED8    (((BYTE*)&NVMKEY)[1])   //No such LED, map to dummy register
#endif

#ifndef DIR_BUTTON0
#define DIR_BUTTON0    BUTTON0_TRIS
#endif
#ifndef DIR_BUTTON1
#define DIR_BUTTON1    BUTTON1_TRIS
#endif
#ifndef DIR_BUTTON2
#define DIR_BUTTON2    BUTTON2_TRIS
#endif
#ifndef DIR_BUTTON3
#define DIR_BUTTON3    BUTTON3_TRIS
#endif
#ifndef DIR_BUTTON4
#define DIR_BUTTON4    BUTTON4_TRIS
#endif
#ifndef DIR_BUTTON5
#define DIR_BUTTON5    BUTTON5_TRIS
#endif
#ifndef DIR_BUTTON6
#define DIR_BUTTON6    BUTTON6_TRIS
#endif
#ifndef DIR_BUTTON7
#define DIR_BUTTON7    BUTTON7_TRIS
#endif
#ifndef DIR_BUTTON8
#define DIR_BUTTON8    BUTTON8_TRIS
#endif

#ifndef PIN_BUTTON0
#define PIN_BUTTON0 BUTTON0_IO
#endif
#ifndef PIN_BUTTON1
#define PIN_BUTTON1 BUTTON1_IO
#endif
#ifndef PIN_BUTTON2
#define PIN_BUTTON2 BUTTON2_IO
#endif
#ifndef PIN_BUTTON3
#define PIN_BUTTON3 BUTTON3_IO
#endif
#ifndef PIN_BUTTON4
#define PIN_BUTTON4 BUTTON4_IO
#endif
#ifndef PIN_BUTTON5
#define PIN_BUTTON5 BUTTON5_IO
#endif
#ifndef PIN_BUTTON6
#define PIN_BUTTON6 BUTTON6_IO
#endif
#ifndef PIN_BUTTON7
#define PIN_BUTTON7 BUTTON7_IO
#endif
#ifndef PIN_BUTTON8
#define PIN_BUTTON8 BUTTON8_IO
#endif


#endif
