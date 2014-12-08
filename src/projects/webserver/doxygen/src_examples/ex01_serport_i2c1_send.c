/**
@example ex01_serport_i2c1_send.c
This example shows how to send data on the I2C 1 port.
*/

#include "netcruzer.h"

/**
 * Debug service routine
 */
void ex01_SerportI2C1Send(char doThis) {

    if (doThis=='0') {
        //This writes a 2 byte message to I2C port 1.
        //Each packet added to the CIRBUF_TX_I2C1 buffer is written as an I2C message, with an I2C Start
        //and Stop Condition added to the front and back of packet data.
        //This example message (0x50, 0x28) is the command for turning off the backlight of a LCD2S display.
        cbufPutPacket(CIRBUF_TX_I2C1, (BYTE*)"\x50\x28", 2);  //LCD backlight off
        //cbufPutPacket(CIRBUF_TX_I2C1, (const BYTE[]){0x50, 0x28}, 2);  //LCD backlight off
    }
    else if (doThis=='1') {
        //This writes a 2 byte message to I2C port 1.
        //Each packet added to the CIRBUF_TX_I2C1 buffer is written as an I2C message, with an I2C Start
        //and Stop Condition added to the front and back of packet data.
        //This example message (0x50, 0x20) is the command for turning on the backlight of a LCD2S display.
        cbufPutPacket(CIRBUF_TX_I2C1, (BYTE*)"\x50\x20", 2);  //LCD backlight on
    }
    else if (doThis=='2') {
        BYTE buf[] = {0x50, 0x80, '\f', 'H', 'e', 'l', 'l', 'o'};

        //This writes the contents of the buf[] array as an I2C message to I2C port 1.
        //Each packet added to the CIRBUF_TX_I2C1 buffer is written as an I2C message, with an I2C Start
        //and Stop Condition added to the front and back of packet data.
        cbufPutPacket(CIRBUF_TX_I2C1, buf, sizeof(buf));
    }
    else if (doThis=='3') {
        //This writes the contents of the buf[] array as an I2C message to I2C port 1.
        //Each packet added to the CIRBUF_TX_I2C1 buffer is written as an I2C message, with an I2C Start
        //and Stop Condition added to the front and back of packet data.
        cbufPutPacket(CIRBUF_TX_I2C1, (BYTE*)"\x50\x80\fHello\nWorld", 14);
    }
    else if (doThis=='4') {
        char buf[] = "\x50\x80\fHello\nWorld";  //Initialize with NULL terminated string. Use 'sizeof(buf)-1' to not include terminating NULL
        //This writes the contents of the buf[] array as an I2C message to I2C port 1.
        //Each packet added to the CIRBUF_TX_I2C1 buffer is written as an I2C message, with an I2C Start
        //and Stop Condition added to the front and back of packet data.
        cbufPutPacket(CIRBUF_TX_I2C1, (BYTE*)buf, sizeof(buf)-1);
    }
}
