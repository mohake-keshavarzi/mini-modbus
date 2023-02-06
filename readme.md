# MiniModbus

It is a simplified version of the Modbus protocol, designed for Arduino. The main goal of this library is to _create request and response messages_ and parse those messages. __It won't handle sending and receiving the frames.__

## Supported Fuction Codes
For now, the following Function Codes are supported:
* WRITE SINGLE COIL (0x05)
* WRITE SINGLE REGISTER (0x06)
* READ COILS (0x01)
* READ INPUT REGISTERS (0x04)

## Frame Structure

The whole frame structure is so close to the original Modbus RTU but with more simplifications. 
* For now, there is no CRC code at the end of frame
* At the start of each frame, there is one byte for the destination's ID _(Master's ID is set to 0 by default)._

Structure of the frame:  
` ID of the reciever(1 byte) + Function code(1 bute)+ Data `
