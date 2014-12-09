/**
 * @brief           MCP23017 Input/Output functions
 * @file            nz_mcp23017.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 **********************************************************************
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
 * 2011-06-03, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZ_MCP23017_C

#include "projdefs.h"
#include "io\nz_mcp23017.h"

//Use I2C Bus 1 by default
#if !defined(MCP23017_USE_I2C1) && !defined(MCP23017_USE_I2C2) && !defined(MCP23017_USE_I2C3) && !defined(MCP23017_USE_I2C4)
#define MCP23017_USE_I2C1
#endif

//Redifine I2C function to use I2C bus as given by RTC_M41T82_USE_I2Cn define
#if defined(MCP23017_USE_I2C1)
    #define USE_I2C_BUS1
    #include "nz_i2c1.h"
#elif defined(MCP23017_USE_I2C2)
    #define USE_I2C_BUS2
    #include "nz_i2c2.h"
#elif defined(MCP23017_USE_I2C3)
    #define USE_I2C_BUS3
    #include "nz_i2c3.h"
#elif defined(MCP23017_USE_I2C4)
    #define USE_I2C_BUS4
    #include "nz_i2c4.h"
#endif
#include "nz_i2cDefinePort.h"


//Global variables
UINT8   MCP23017_i2cAddress;        // physical I2C address

/**
 * Initializes the MCP23017 I/O Expander
 *
 * @preCondition The I2C port must have been initialized for Master Mode
 * operation before calling this function!
 *
 * @param obj Structure with data for MCP23017 chip.
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_Init(MCP23017* obj) {

    //Write to control register.
    //xxxx xx0x - Interrupt pins active low
    //xxxx x1xx - Configures the INT pin as an open-drain output
    //xxx0 xxxx - Slew rate enabled
    //xx0x xxxx - Sequential operation enabled, address pointer increments
    //x1xx xxxx - The INT pins are internally connected
    //0xxx xxxx - The registers are in the same bank (addresses are sequential)
    if ( i2cWriteSlaveBytes(obj->adr, MCP23017_REG_IOCON, 0x44) != 0)
        return -1;

    // Set the shadow registers to power-on state
    obj->dir.Val = 0xffff;   //Power on state is for all IOs to be inputs
    obj->pullUp.Val = 0;
    obj->latch.Val = 0;
    obj->polarity.Val = 0;

    return 0;   //OK
}


/**
 * Configure direction of pins.
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param dir Bits indicate direction of I/O pins. A 0 sets pin to output, and
 *        a 1 sets pin to input.
 *
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_ConfigDir(MCP23017* obj, UINT16 dir) {

    //If new value is same as current value, nothing to do, exit
    if (obj->dir.Val == dir)
        return 0;

    //Write to direction control register
    if ( i2cWriteSlaveRegBlock(obj->adr, MCP23017_REG_IODIRA, ((BYTE*)&dir), 2) != 0)
        return -1;

    obj->dir.Val = dir;     //Update current value

    return 0;   //OK
}


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
INT8 mcp23017_ConfigDirMask(MCP23017* obj, UINT16 dir, UINT16 mask) {
    WORD_VAL    newDir;
    BYTE*       data;
    BYTE        regAdr;
    BYTE        bytesToWrite;

    newDir.Val = obj->dir.Val;
    newDir.Val &= (~mask);    //Clear all bits indicated by mask
    //Set all bits given by value
    newDir.Val |= (dir & mask);

    //If new value is same as current value, nothing to do, exit
    if (obj->dir.Val == newDir.Val)
        return 0;           //Pins the same

    //Set varariables for updating GPIOA and GPIOB
    data = &newDir.v[0];
    bytesToWrite = 2;
    regAdr = MCP23017_REG_IODIRA;

    //If bits 0-7 of value changed
    if (obj->dir.v[0] != newDir.v[0]) {
        //ONLY bits of LSB (0-7) changed. Only update GPIOA
        if (obj->dir.v[1] == newDir.v[1]) {
            //regAdr = MCP23017_REG_GPIOA;
            bytesToWrite = 1;
            //data = &newDir.v[0];
        }
    }
    //ONLY bits of MSB (8-15) changed. Only update GPIOB
    else {
        regAdr = MCP23017_REG_IODIRB;
        bytesToWrite = 1;
        data = &newDir.v[1];
    }

    //Write to direction control register
    if ( i2cWriteSlaveRegBlock(obj->adr, regAdr, data, bytesToWrite) != 0)
        return -1;

    //Write was successfull, update current value
    obj->dir.Val = newDir.Val;

    return 0;   //OK
}


/**
 * Configure pull-up resistors.
 *
 * @param obj Structure with data for MCP23017 chip
 *
 * @param pullUps Bits indicate if pull-up is active.
 *
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_ConfigPullups(MCP23017* obj, UINT16 pullUps) {

    //If new value is same as current value, nothing to do, exit
    if (obj->pullUp.Val == pullUps)
        return 0;

    //Write to direction control register
    if ( i2cWriteSlaveRegBlock(obj->adr, MCP23017_REG_GPPUA, ((BYTE*)&pullUps), 2) != 0)
        return -1;

    obj->pullUp.Val = pullUps;     //Update current value

    return 0;   //OK
}


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
INT8 mcp23017_ConfigIntPins(MCP23017* obj, UINT16 intPins) {
    UINT16 intcon;

    //Write to Interrupt control register. If a bit is set, the corresponding I/O pin is compared
    //against the associated bit in the DEFVAL register. If a bit value is clear, the corresponding
    //I/O pin is compared against the previous value.
    intcon = 0;     //Generate interrupt if previous value changed
    if ( i2cWriteSlaveRegBlock(obj->adr, MCP23017_REG_INTCONA, ((BYTE*)&intcon), 2) != 0)
        return -1;

    //Write to Interrupt-on-change control register
    if ( i2cWriteSlaveRegBlock(obj->adr, MCP23017_REG_GPINTENA, ((BYTE*)&intPins), 2) != 0)
        return -1;


    return 0;   //OK
}



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
INT8 mcp23017_WritePin(MCP23017* obj, UINT8 pinVal) {
    BYTE val;
    WORD mask;
    WORD_VAL newLatch;
    BYTE newLatchByte;
    BYTE regAdr;

    mask = 0x01 << (pinVal & 0x0f);
    newLatch.Val = obj->latch.Val;

    //Requested pin must be cleared
    if ((pinVal & 0x80) == 0) {
        val = 0;
        //Clear given pin
        newLatch.Val = newLatch.Val & (~mask);
    }
    //Requested pin must be set
    else {
        val = 1;
        //Set given pin
        newLatch.Val = newLatch.Val | mask;
    }

    //If current value is equal to requested value, return
    if (newLatch.Val == obj->latch.Val) {
        return 0;
    }

    //Given pin is in Latch A
    if ((pinVal & 0x08) == 0) {
        regAdr = MCP23017_REG_GPIOA;
        newLatchByte = newLatch.v[0];
    }
    //Given pin is in Latch B
    else {
        regAdr = MCP23017_REG_GPIOB;
        newLatchByte = newLatch.v[1];
    }

    //Write to Latch
    if ( i2cWriteSlaveBytes(obj->adr, regAdr, newLatchByte) != 0)
        return -1;

    //Write was successfull, update current value
    obj->latch.Val = newLatch.Val;

    return 0;   //OK
}


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
 *
 * @return Returns 0 if OK, else returns a non-zero value if error
 */
INT8 mcp23017_WriteMask(MCP23017* obj, UINT16 val, UINT16 mask) {
    WORD_VAL    newLatch;
    BYTE*       data;
    BYTE        regAdr;
    BYTE        bytesToWrite;

    newLatch.Val = obj->latch.Val;
    newLatch.Val &= (~mask);    //Clear all bits indicated by mask
    //Set all bits given by value
    newLatch.Val |= (val & mask);

    //If new value is same as current value, nothing to do, exit
    if (obj->latch.Val == newLatch.Val)
        return 0;           //Pins the same

    //Set varariables for updating GPIOA and GPIOB
    data = &newLatch.v[0];
    bytesToWrite = 2;
    regAdr = MCP23017_REG_GPIOA;

    //If bits 0-7 of value changed
    if (obj->latch.v[0] != newLatch.v[0]) {
        //ONLY bits of LSB (0-7) changed. Only update GPIOA
        if (obj->latch.v[1] == newLatch.v[1]) {
            //regAdr = MCP23017_REG_GPIOA;
            bytesToWrite = 1;
            //data = &newLatch.v[0];
        }
    }
    //ONLY bits of MSB (8-15) changed. Only update GPIOB
    else {
        regAdr = MCP23017_REG_GPIOB;
        bytesToWrite = 1;
        data = &newLatch.v[1];
    }

    //Write to direction control register
    if ( i2cWriteSlaveRegBlock(obj->adr, regAdr, data, bytesToWrite) != 0)
        return -1;

    //Write was successfull, update current value
    obj->latch.Val = newLatch.Val;

    return 0;   //OK
}

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
INT8 mcp23017_ReadPins(MCP23017* obj) {
    WORD pins;

    //Read to direction control register
    if ( i2cReadSlaveRegBlock(obj->adr, MCP23017_REG_GPIOA, ((BYTE*)&pins), 2) != 0)
        return -1;

    //If new value is same as current value, nothing to do, exit
    if (obj->pin.Val == pins)
        return 0;           //Pins the same

    obj->pin.Val = pins;    //Update current value

    return 1;               //Pins had different value
}

