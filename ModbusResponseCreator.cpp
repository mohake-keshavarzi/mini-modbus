#include "ModbusResponseCreator.h"
#include "WordFunctions.h"
#include "AceCRC.h"

uint16_t ModbusResponseCreator::addCRC(uint16_t message_size)
{
    word crc = ace_crc::crc16modbus_nibble::crc_calculate(message,message_size);
    message[message_size++]= funcs.getLSByte(crc);
    message[message_size++]= funcs.getMSByte(crc);
    return message_size;
}

ModbusResponseCreator::ModbusResponseCreator()
{
    message = new byte[8];
    message_size = 8;
}

void ModbusResponseCreator::setCoilsRefrence(boolean* coils_refrence, uint16_t size)
{
    this->coils = coils_refrence;
    this->coilsSize = size;
}
void ModbusResponseCreator::setDiscreteInputsRefrence(boolean* inputDiscretes_refrence, uint16_t size)
{
    this->discreteInputs = inputDiscretes_refrence;
    this->discreteInputsSize = size;
}
void ModbusResponseCreator::setHoldingRegistersRefrence(word* holdingRegisters_refrence, uint16_t size)
{
    this->holdingRegisters = holdingRegisters_refrence;
    this->holdingRegsSize = size;
}
void ModbusResponseCreator::setInputRegistersRefrence(word* inputRegisters_refrence, uint16_t size)
{
    this->inputRegisters = inputRegisters_refrence;
    this->inputRegsSize = size;
}

/**
 * Returns 0 for INVALID_DATA_ADDRESS_EXCEPTION_CODE or invalid input type;
 */
uint16_t ModbusResponseCreator::createReadCoilsOrDiscreteInputsResponse(uint16_t startAddress, uint16_t quantity, uint8_t type)
{
    boolean* refrence;
    byte functionCode;
    switch(type){
        case COILS:
            refrence=coils;
            functionCode = READ_COILS_FUNCTIONCODE;
            break;
        case DISCRETE_INPUTS:
            refrence=discreteInputs;
            functionCode= READ_DISCRETE_INPUTS_FUNCTIONCODE;
            break;
        default: //Invalid Type
            return 0;
    }
    
    // INVALID_DATA_ADDRESS_EXCEPTION_CODE;
    if (isDataAddressWrong(startAddress, quantity, type)) {
        return 0;
    }
    delete[] message;
    message_size = 5; // Master id +function code + byte count + CRC

    uint8_t byteCount { quantity / 8 };
    if (quantity % 8 != 0)
        byteCount++;

    message_size += byteCount;
    message = new byte[message_size];

    message[0] = MASTER_ID;
    message[1] = functionCode;
    message[2] = byteCount;
    int currentPlaceInMessage = 3;
    byte myBytes[byteCount]{};
    for (uint16_t i = 0; i < quantity; i++)
    {
        // Calculate the index of the coil status byte that contains the current coil
        uint16_t byteIndex = (i) / 8;
    
        // Calculate the bit position of the current coil within the current coil status byte
        uint8_t bitPosition = (i) % 8;
    
        // Set the bit in the coil status byte if the current coil is true
        if (refrence[startAddress+i])
        {
            myBytes[byteIndex] |= (1 << bitPosition);
        }
    }
    for(int j{};j<byteCount;j++){
        message[currentPlaceInMessage+j]=myBytes[j];
    }
    return addCRC(message_size-2);
}

/**
 * Returns 0 for INVALID_DATA_ADDRESS_EXCEPTION_CODE;
 */
uint16_t ModbusResponseCreator::createWriteSingleRegisterResponse(uint16_t address, word value)
{
    // INVALID_DATA_ADDRESS_EXCEPTION_CODE;
    if (isDataAddressWrong(address, 0, HOLDING_REGISTERS)) {
        return 0;
    }
    holdingRegisters[address] = value;
    delete[] message;
    message_size = 8; // master ID+function code + address + value + CRC = 1+1+2+2+2
    message = new byte[message_size];
    message[0] = MASTER_ID;
    message[1] = WRITE_SINGLE_REGISTER_FUNCTIONCODE;
    message[2] = funcs.getMSByte(address);
    message[3] = funcs.getLSByte(address);
    message[4] = funcs.getMSByte(value);
    message[5] = funcs.getLSByte(value);

    return addCRC(message_size-2);
}

/**
 * Returns 0 for INVALID_DATA_ADDRESS_EXCEPTION_CODE or invalid type;
 */
uint16_t ModbusResponseCreator::createReadHoldingOrInputRegistersResponse(uint16_t startAddress,uint16_t numOfRegisters,uint8_t type)
{

    word* refrence;
    byte functionCode;
    switch(type){
        case INPUT_REGISTERS:
            refrence=inputRegisters;
            functionCode = READ_INPUT_REGISTERS_FUNCTIONCODE;
            break;
        case HOLDING_REGISTERS:
            refrence=holdingRegisters;
            functionCode= READ_HOLDING_REGISTERS_FUNCTIONCODE;
            break;
        default: //Invalid Type
            return 0;
    }
    // INVALID_DATA_ADDRESS_EXCEPTION_CODE;
    if (isDataAddressWrong(startAddress, numOfRegisters, type)) {
        return 0;
    }
    delete[] message;
    message_size = 5; // master id +function code + byte count + CRC

    uint8_t byteCount { numOfRegisters * 2 };

    message_size += byteCount;
    message = new byte[message_size];
    message[0] = MASTER_ID;
    message[1] = functionCode;
    message[2] = byteCount;

    int currentPlaceInMessage = 3;
    for (int i { startAddress }; i < numOfRegisters + startAddress; i++) {
        message[currentPlaceInMessage] = funcs.getMSByte(refrence[i]);
        currentPlaceInMessage++;
        message[currentPlaceInMessage] = funcs.getLSByte(refrence[i]);
        currentPlaceInMessage++;
    }

    return addCRC(message_size-2);
}

/**
 * Returns 0 for INVALID_DATA_ADDRESS_EXCEPTION_CODE;
 */
uint16_t ModbusResponseCreator::createWriteSingleCoilResponse(uint16_t address, boolean value)
{
    // INVALID_DATA_ADDRESS_EXCEPTION_CODE;
    if (isDataAddressWrong(address, 0, COILS)) {
        return 0;
    }
    delete[] message;
    // size is master id + function code + address + value + CRC = 1+1+2+2+2=8
    message_size = 8;
    message = new byte[message_size];

    message[0] = MASTER_ID;
    message[1] = WRITE_SINGLE_COIL_FUNCTIONCODE;
    message[2] = funcs.getMSByte(address);
    message[3] = funcs.getLSByte(address);
    if (value) {
        coils[address] = 1;
        message[4] = 0xFF;
        message[5] = 0x00;
    } else {
        coils[address] = 0;
        message[4] = 0x00;
        message[5] = 0x00;
    }

    return addCRC(message_size-2);
}

uint16_t ModbusResponseCreator::createWriteCoilsResponse(uint16_t startAddress, boolean* values, uint16_t quantity)
{
    if (isDataAddressWrong(startAddress, quantity, COILS)) {
        return 0;
    }
    delete[] message;
    // size is master id + function code + start address + quantitiy of written coils + CRC = 1+1+2+2+2=8
    message_size = 8;
    message = new byte[message_size];

    message[0] = MASTER_ID;
    message[1] = WRITE_MULTIPLE_COILS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(quantity);
    message[5] = funcs.getLSByte(quantity);

    for(int i{0};i<quantity;i++){
        coils[startAddress + i]=values[i];
    }

    return addCRC(message_size-2);

}

uint16_t ModbusResponseCreator::createWriteRegistersResponse(uint16_t startAddress, word* values, uint16_t quantity)
{
    if (isDataAddressWrong(startAddress, quantity, HOLDING_REGISTERS)) {
        return 0;
    }
    delete[] message;
    // size is master id + function code + start address + quantitiy of written coils + CRC = 1+1+2+2+2=8
    message_size = 8;
    message = new byte[message_size];

    message[0] = MASTER_ID;
    message[1] = WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(quantity);
    message[5] = funcs.getLSByte(quantity);

    for(int i{0};i<quantity;i++){
        holdingRegisters[startAddress + i]=values[i];
    }

    return addCRC(message_size-2);

}

uint16_t ModbusResponseCreator::createExceptionResponse(uint8_t functionCode, uint8_t exceptionCode)
{
    delete[] message;
    // size is master id + function code for error + ExceptionCode + CRC = 1+1+1+2
    message_size = 5;
    message = new byte[message_size];

    message[0] = MASTER_ID;
    message[1] = functionCode + 0x80;
    message[2] = exceptionCode;
    return addCRC(message_size-2);
}

boolean ModbusResponseCreator::isDataAddressWrong(uint16_t startAddress, uint16_t numberOfCoilsOrRegisters, uint8_t typeOfMemory)
{
    switch (typeOfMemory) {
    case COILS:
        // Serial.print("RESPONSE CREATOR: ");
        // Serial.print(startAddress);
        // Serial.print(" : ");
        // Serial.print(coilsSize);
        // Serial.print(" : ");
        // Serial.println(numberOfCoilsOrRegisters);

        return startAddress > coilsSize - 1 || startAddress < 0 || startAddress + numberOfCoilsOrRegisters > coilsSize;
        break;
    case DISCRETE_INPUTS:
        return startAddress > discreteInputsSize - 1 || startAddress < 0 || startAddress + numberOfCoilsOrRegisters > discreteInputsSize;
        break;
    case HOLDING_REGISTERS:
        return startAddress > holdingRegsSize - 1 || startAddress < 0 || startAddress + numberOfCoilsOrRegisters > holdingRegsSize;
        break;
    case INPUT_REGISTERS:
        return startAddress > inputRegsSize - 1 || startAddress < 0 || startAddress + numberOfCoilsOrRegisters > inputRegsSize;
        break;
    default:
        return true;
        break;
    }
}

ModbusResponseCreator::~ModbusResponseCreator()
{
    delete[] message;
}
