#include "ModbusResponseCreator.h"
#include "WordFunctions.h"

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
void ModbusResponseCreator::setInputDiscretesRefrence(boolean* inputDiscretes_refrence, uint16_t size)
{
    this->inputDiscretes = inputDiscretes_refrence;
    this->inputDiscretesSize = size;
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
            refrence=inputDiscretes;
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
    message_size = 3; // Master id +function code + byte count

    uint8_t byteCount { quantity / 8 };
    if (quantity % 8 != 0)
        byteCount++;

    message_size += byteCount;
    message = new byte[message_size];

    message[0] = MASTER_ID;
    message[1] = functionCode;
    message[2] = byteCount;
    int currentPlaceInMessage = 3;
    byte myByte = 0x00;
    int i = startAddress;
    int j = 0;
    while (i < quantity + startAddress || j % 8 != 7) {
        myByte >> 1;
        if (i < quantity + startAddress) {
            
            
            if (refrence[i]) {
                myByte | 0x80;
            } else {
                myByte & 0x7F;
            }
        }
        if (j % 8 == 7) {
            message[currentPlaceInMessage] = myByte;
            currentPlaceInMessage++;
            myByte = 0x00;
        }
        i++;
        j++;
    }
    return message_size;
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
    message_size = 6; // master ID+function code + address + value = 1+2+2
    message = new byte[message_size];
    message[0] = MASTER_ID;
    message[1] = WRITE_SINGLE_REGISTER_FUNCTIONCODE;
    message[2] = funcs.getMSByte(address);
    message[3] = funcs.getLSByte(address);
    message[4] = funcs.getMSByte(value);
    message[5] = funcs.getLSByte(value);

    return message_size;
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
    message_size = 3; // master id +function code + byte count

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

    return message_size;
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
    // size is master id + function code + address + value = 1+2+2=5
    message_size = 6;
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

    return message_size;
}

uint16_t ModbusResponseCreator::createWriteCoilsResponse(uint16_t startAddress, boolean* values, uint16_t quantity)
{
    if (isDataAddressWrong(startAddress, quantity, COILS)) {
        return 0;
    }
    delete[] message;
    // size is master id + function code + start address + quantitiy of written coils = 1+1+2+2=6
    message_size = 6;
    message = new byte[message_size];

    message[0] = MASTER_ID;
    message[1] = WRITE_MULTIPLE_COILS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(quantity);
    message[5] = funcs.getLSByte(quantity);

    for(int i{0};i<quantity;i++){
        coils[i]=values[i];
    }

    return message_size;

}

uint16_t ModbusResponseCreator::createWriteRegistersResponse(uint16_t startAddress, word* values, uint16_t quantity)
{
    if (isDataAddressWrong(startAddress, quantity, HOLDING_REGISTERS)) {
        return 0;
    }
    delete[] message;
    // size is master id + function code + start address + quantitiy of written coils = 1+1+2+2=6
    message_size = 6;
    message = new byte[message_size];

    message[0] = MASTER_ID;
    message[1] = WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE;
    message[2] = funcs.getMSByte(startAddress);
    message[3] = funcs.getLSByte(startAddress);
    message[4] = funcs.getMSByte(quantity);
    message[5] = funcs.getLSByte(quantity);

    for(int i{0};i<quantity;i++){
        holdingRegisters[i]=values[i];
    }

    return message_size;

}

uint16_t ModbusResponseCreator::createExceptionResponse(uint8_t functionCode, uint8_t exceptionCode)
{
    delete[] message;
    // size is master id + function code for error + ExceptionCode = 1+1+1
    message_size = 3;
    message = new byte[message_size];

    message[0] = MASTER_ID;
    message[1] = functionCode + 0x80;
    message[2] = exceptionCode;
    return message_size;
}

boolean ModbusResponseCreator::isDataAddressWrong(uint16_t startAddress, uint16_t numberOfCoilsOrRegisters, uint8_t typeOfMemmory)
{
    switch (typeOfMemmory) {
    case COILS:
        // Serial.print("RESPONSE CREATOR: ");
        // Serial.print(startAddress);
        // Serial.print(" : ");
        // Serial.print(coilsSize);
        // Serial.print(" : ");
        // Serial.println(numberOfCoilsOrRegisters);

        return startAddress > coilsSize - 1 || startAddress < 0 || startAddress + numberOfCoilsOrRegisters > coilsSize;
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
