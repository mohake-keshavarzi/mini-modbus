#include "ModbusRequestCreator.h"
#include "FunctionCodes.h"
#include "WordFunctions.h"
#include "AceCRC.h"

/**
 * The overall structure each request is so similar to modbus theory
 * except that here we don't have start bit at start and CRC at end and end bits at end
 * Also here the first byte is for slave ID or master ID which originally in modbus is up to phisycal layer
 */

/**
 * Each function returns the overall length of request message
 */

uint16_t ModbusRequestCreator::addCRC(uint16_t message_size)
{
    word crc = ace_crc::crc16modbus_nibble::crc_calculate(message,message_size);
    message[message_size++]= funcs.getLSByte(crc);
    message[message_size++]= funcs.getMSByte(crc);
    return message_size;
}

ModbusRequestCreator::ModbusRequestCreator(uint8_t slaveID)
{
    this->slaveID = slaveID;

    message = new byte[6];
}

uint16_t ModbusRequestCreator::createReadCoilsRequest(uint16_t startAddress, uint16_t numberOfCoils)
{
    // size is slaveID + function code + startAddress + numOfCoils + CRC = 1+1+2+2+2=8
    delete[] message;
    message = new byte[8];
    message[0] = slaveID;
    message[1] = READ_COILS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(numberOfCoils);
    message[5] = funcs.getLSByte(numberOfCoils);

    return addCRC(6);
}

uint16_t ModbusRequestCreator::createReadDiscreteInputsRequest(uint16_t startAddress, uint16_t numberOfCoils)
{
    // size is slaveID + function code + startAddress + numOfCoils + CRC = 1+1+2+2+2=8
    delete[] message;
    message = new byte[8];
    message[0] = slaveID;
    message[1] = READ_DISCRETE_INPUTS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(numberOfCoils);
    message[5] = funcs.getLSByte(numberOfCoils);

    return addCRC(6);
}

uint16_t ModbusRequestCreator::createWriteSingleRegisterRequest(uint16_t address, word value)
{
    // size is slaveID + function code + address + value + CRC = 1+1+2+2+2=8
    delete[] message;
    message = new byte[8];
    message[0] = slaveID;
    message[1] = WRITE_SINGLE_REGISTER_FUNCTIONCODE;
    message[2] = funcs.getMSByte(address);
    message[3] = funcs.getLSByte(address);
    message[4] = funcs.getMSByte(value);
    message[5] = funcs.getLSByte(value);

    return addCRC(6);
}

uint16_t ModbusRequestCreator::createReadInputRegistersRequest(uint16_t startAddress, uint16_t numOfRegisters)
{
    // size is slaveID + function code + startAddress + numOfRegisters = 1+1+2+2+2=8
    delete[] message;
    message = new byte[8];
    message[0] = slaveID;
    message[1] = READ_INPUT_REGISTERS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(numOfRegisters);
    message[5] = funcs.getLSByte(numOfRegisters);

    return addCRC(6);
}

uint16_t ModbusRequestCreator::createReadHoldingRegistersRequest(uint16_t startAddress, uint16_t numOfRegisters)
{
    // size is slaveID + function code + startAddress + numOfRegisters + CRC = 1+1+2+2+2=8
    delete[] message;
    message = new byte[8];
    message[0] = slaveID;
    message[1] = READ_HOLDING_REGISTERS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(numOfRegisters);
    message[5] = funcs.getLSByte(numOfRegisters);

    return addCRC(6);
}

/**
 * In coils to send command for set we should send 0xFF00 as a value and for reset we should send 0x0000
 */
uint16_t ModbusRequestCreator::createWriteSingleCoilRequest(uint16_t address, boolean value)
{

    // size is slaveID + function code + address + value + CRC = 1+1+2+2+2=8
    delete[] message;
    message = new byte[8];
    message[0] = slaveID;
    message[1] = WRITE_SINGLE_COIL_FUNCTIONCODE;
    message[2] = funcs.getMSByte(address);
    message[3] = funcs.getLSByte(address);
    if (value) {
        message[4] = 0xFF;
        message[5] = 0x00;
    } else {
        message[4] = 0x00;
        message[5] = 0x00;
    }

    return addCRC(6);
}

uint16_t ModbusRequestCreator::createWriteCoilsRequest(uint16_t startAddress, boolean* values, uint16_t quantity)
{
    delete[] message;
    // size is slaveID + function code + start address + quantity + byte count + values in bytes + CRC= 1+1+2+2+1+more +2 =7+more +2
    int message_size = 9;
    uint8_t byteCount { quantity / 8 };
    if (quantity % 8 != 0)
        byteCount++;
    message_size += byteCount;
    message = new byte[message_size];
    message[0] = slaveID;
    message[1] = WRITE_MULTIPLE_COILS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(quantity);
    message[5] = funcs.getLSByte(quantity);
    message[6] = byteCount;
    int currentPlaceInMessage = 7;
    byte myBytes[byteCount]{};
    for (uint16_t i = 0; i < quantity; i++)
    {
        // Calculate the index of the coil status byte that contains the current coil
        uint16_t byteIndex = (i) / 8;
    
        // Calculate the bit position of the current coil within the current coil status byte
        uint8_t bitPosition = (i) % 8;
    
        // Set the bit in the coil status byte if the current coil is true
        if (values[i])
        {
            myBytes[byteIndex] |= (1 << bitPosition);
        }
    }
    for(int j{};j<byteCount;j++){
        message[currentPlaceInMessage+j]=myBytes[j];
    }
    return addCRC(message_size-2);
}

uint16_t ModbusRequestCreator::createWriteRegistersRequest(uint16_t startAddress, word* values, uint16_t quantity)
{
    delete[] message;
    // size is slaveID + function code + start address + quantity + byte count + values in bytes + CRC= 1+1+2+2+1+more+2=7+more +2
    int message_size = 9;
    uint8_t byteCount { quantity * 2 };
    message_size += byteCount;
    message = new byte[message_size];
    message[0] = slaveID;
    message[1] = WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(quantity);
    message[5] = funcs.getLSByte(quantity);
    message[6] = byteCount;

    int currentPlaceInMessage = 7;
    for (int i { 0 }; i < quantity ; i++) {
        message[currentPlaceInMessage] = funcs.getMSByte(values[i]);
        currentPlaceInMessage++;
        message[currentPlaceInMessage] = funcs.getLSByte(values[i]);
        currentPlaceInMessage++;
    }
    return addCRC(message_size-2);
}

ModbusRequestCreator::~ModbusRequestCreator()
{
    delete[] message;
}
