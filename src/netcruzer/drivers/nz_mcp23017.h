/**
 * @brief           MCP23017 I/O Expander functions
 * @file            nz_mcp23017.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_mcp23017_desc Description
 *****************************************
 *
 * Functions for using the MCP23017 I/O Expander
 *
 * @subsection  nz_mcp23017_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // -------------- MCP23017 Configuration --------------------
 // *********************************************************************
 //Uncomment one of the following, depending on what I2C bus should be used
 #define MCP23017_USE_I2C1      //[-DEFAULT-] Use I2C Bus 1
 //#define MCP23017_USE_I2C2    //Use I2C Bus 2
 //#define MCP23017_USE_I2C3    //Use I2C Bus 3
 //#define MCP23017_USE_I2C4    //Use I2C Bus 4
 
 @endcode
 *********************************************************************/

 /*********************************************************************
 * @section nz_mcp23017_lic Software License Agreement
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
 * 2011-06-03, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#ifndef _MCP23017_H_
#define _MCP23017_H_

#define MCP23017_REG_IODIRA      0x00
#define MCP23017_REG_IODIRB      0x01
#define MCP23017_REG_IPOLA       0x02   
#define MCP23017_REG_IPOLB       0x03
#define MCP23017_REG_GPINTENA    0x04
#define MCP23017_REG_GPINTENB    0x05
#define MCP23017_REG_DEFVALA     0x06 
#define MCP23017_REG_DEFVALB     0x07 
#define MCP23017_REG_INTCONA     0x08 
#define MCP23017_REG_INTCONB     0x09 
#define MCP23017_REG_IOCON       0x0A
#define MCP23017_REG_GPPUA       0x0C   
#define MCP23017_REG_GPPUB       0x0D   
#define MCP23017_REG_INTFA       0x0E   
#define MCP23017_REG_INTFB       0x0F   
#define MCP23017_REG_INTCAPA     0x10 
#define MCP23017_REG_INTCAPB     0x11 
#define MCP23017_REG_GPIOA       0x12   
#define MCP23017_REG_GPIOB       0x13   
#define MCP23017_REG_OLATA       0x14   
#define MCP23017_REG_OLATB       0x15   

/* Not used
#define MCP23017_REG_IODIRA_B1      0x00
#define MCP23017_REG_IODIRB_B1      0x10
#define MCP23017_REG_IPOLA_B1       0x01
#define MCP23017_REG_IPOLB_B1       0x11
#define MCP23017_REG_GPINTENA_B1    0x02
#define MCP23017_REG_GPINTENB_B1    0x12
#define MCP23017_REG_DEFVALA_B1     0x03
#define MCP23017_REG_DEFVALB_B1     0x13
#define MCP23017_REG_INTCONA_B1     0x04
#define MCP23017_REG_INTCONB_B1     0x14
#define MCP23017_REG_IOCON_B1       0x05
#define MCP23017_REG_GPPUA_B1       0x06
#define MCP23017_REG_GPPUB_B1       0x16
#define MCP23017_REG_INTFA_B1       0x07
#define MCP23017_REG_INTFB_B1       0x17
#define MCP23017_REG_INTCAPA_B1     0x08
#define MCP23017_REG_INTCAPB_B1     0x18
#define MCP23017_REG_GPIOA_B1       0x09
#define MCP23017_REG_GPIOB_B1       0x19
#define MCP23017_REG_OLATA_B1       0x0A
#define MCP23017_REG_OLATB_B1       0x1A
*/

#define MCP23017_ADDRESS            0x40    /*Base address if A0 to A3 are all 0 */

//Bit address of port pins. A value from 0 - 15 are allocated for port pins GPA0-GPA7 and GPB0-GPB7
#define MCP23017_GPA0               0
#define MCP23017_GPA1               1
#define MCP23017_GPA2               2
#define MCP23017_GPA3               3
#define MCP23017_GPA4               4
#define MCP23017_GPA5               5
#define MCP23017_GPA6               6
#define MCP23017_GPA7               7
#define MCP23017_GPB0               8
#define MCP23017_GPB1               9
#define MCP23017_GPB2               10
#define MCP23017_GPB3               11
#define MCP23017_GPB4               12
#define MCP23017_GPB5               13
#define MCP23017_GPB6               14
#define MCP23017_GPB7               15

#define MCP23017_SETPIN             0x80
#define MCP23017_CLEARPIN           0


/**
 * Structure containing information for a MCP23017 chip. Can be multiple MCP23017 on a bus.
 */
typedef struct __attribute__((aligned(2), packed)) _MCP23017
{
    BYTE        adr;        //Slave I2C address.  Depending on state of A0 to A2 lines (bits 1,2 and 3), can be 0x40 to 0x4E
    BYTE        fill;       //Filler byte
    WORD_VAL    dir;        //Direction of 16 ports, 0=output, 1=input
    WORD_VAL    pullUp;     //Indicates if pull-ups are enabled for 16 ports
    WORD_VAL    latch;      //Current setting of output latch for 16 ports
    WORD_VAL    polarity;   //Input polarity for 16 ports
    WORD_VAL    pin;        //Current pin value for 16 ports
} MCP23017;


/**
 * Initializes the MCP23017 I/O Expander
 *
 * @preCondition The I2C port must have been initialized for Master Mode
 * operation before calling this function!
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_Init(MCP23017* obj);

/**
 * Configure direction of pins.
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param dir Direction bit for 16 port pins
 *
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_ConfigDir(MCP23017* obj, UINT16 dir);


/**
 * Configure direction of pins. A mask and value is supplied.
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param dir Direction bit for 16 port pins
 *
 * @param mask 16-bit mask
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_ConfigDirMask(MCP23017* obj, UINT16 dir, UINT16 mask);


/**
 * Configure pull-up resistors.
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param pullUps Bits indicate if pull-up's are active for 16 port pins
 *
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_ConfigPullups(MCP23017* obj, UINT16 pullUps);


/**
 * Configures what pins to use for interrupt-on-change function.
 * If the value of any of these pins change, a interrupt will be
 * generated.
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param intPins Bits indicate what pins to use for interrupt-on-change function
 *
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_ConfigIntPins(MCP23017* obj, UINT16 intPins);


/**
 * Write value to pin.
 * For example, to set pin GPA5, use command:
 * mcp23017_WritePin(0x40, MCP23017_GPA5 | MCP23017_SETPIN)
 * Or, to clear pin GPB7, use command:
 * mcp23017_WritePin(0x40, MCP23017_GPB7 | MCP23017_CLEARPIN)
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param pinVal Indicates what pin to set/clear, and value. Bits 0-3 give pin's
 *        address, and bit 7 it's value
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_WritePin(MCP23017* obj, UINT8 pinVal);


/**
 * Set/clear all pins by providing a 16 bit value and mask.
 * For example, to set pin 0 and 7, and clear pins 4 and 5, use command:
 * mcp23017_WriteMask(0x40, 0b0000000010000001, 0b0000000010110001);
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param val 16-bit pin values
 *
 * @param mask 16-bit mask
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_WriteMask(MCP23017* obj, UINT16 val, UINT16 mask);


/**
 * Set a single pin.
 * For example, to set pin GPA5, use command:
 * mcp23017_WritePin(0x40, MCP23017_GPA5)
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param pin Address of pin to set. Is a value from 0 to 15. 0=GPA0, 7=GPA7
 *        8=GPB0 and 15=GPB7
 */
#define mcp23017_WritePin(obj, pin) mcp23017_WritePin(obj, pin | 0x80)


/**
 * Clear a single pin.
 * For example, to set pin GPA5, use command:
 * mcp23017_ClearPin(0x40, MCP23017_GPA5)
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param pin Address of pin to clear. Is a value from 0 to 15. 0=GPA0, 7=GPA7
 *        8=GPB0 and 15=GPB7
 */
#define mcp23017_ClearPin(obj, pin) mcp23017_WritePin(obj, pin & 0x7f)


/**
 * Reads value of all pins, and updates obj->pin. This function does a read of the
 * MCP23017 chip, and updates the pin value in the given MCP23017 object.
 *
 * @param obj Pointer to structure with data for MCP23017 chip
 * 
 * @return 0 if all pins same as for last read,
 *         1 if pins had different value
 *         -1 if error
 */
INT8 mcp23017_ReadPins(MCP23017* obj);


/**
 * Returns the current pin values saved in the MCP23017 object. The current pin values are NOT
 * read from the MCP23017 chip. The mcp23017_ReadPins() will cause MCP23017 chip to be read.
 * Use DINPTST1_IOX_MASK_XXX constants to test if pin is 0 or 1.
 *
 * @param obj Pointer to structure with data for MCP23017 chip
 * 
 * @return Word with pin values in bits 0 to 15
 */
#define mcp23017_ReadPins(obj) ((obj)->pin.Val)


/**
 *
 * @param obj Structure with data for MCP23017 chip
 */
//void mcp23017(UINT16 );

#endif
