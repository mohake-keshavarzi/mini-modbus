#include "ModbusRequestResponseParser.h"
#include "FunctionCodes.h"
#include "WordFunctions.h"

//**************TODO: add data exists checking***********************//

ModbusRequestResponseParser::ModbusRequestResponseParser(byte* message,uint16_t digitalValuesBufferSize,uint16_t registerValuesBufferSize)
    : message { message }
{
    digitalValues= new boolean[digitalValuesBufferSize];
    registerValues = new word[registerValuesBufferSize];
}

uint8_t ModbusRequestResponseParser::getFunctionCode()
{
    return message[1];
}

boolean ModbusRequestResponseParser::isItException()
{
    return getFunctionCode() > 0x80;
}

uint8_t ModbusRequestResponseParser::getExceptionCode()
{
    if (isItException()) {
        return message[2];
    }
    return 0;
}

uint8_t ModbusRequestResponseParser::getSlaveID()
{
    return message[0];
}

uint16_t ModbusRequestResponseParser::getAddress()
{
    return funcs.MSBLSBJoin(message[2], message[3]);
}

uint16_t ModbusRequestResponseParser::getNumberOfRegsOrDigitals()
{
    return funcs.MSBLSBJoin(message[4], message[5]);
}

uint8_t ModbusRequestResponseParser::getByteCountInReponse()
{
    return message[2];
}

uint8_t ModbusRequestResponseParser::getByteCountInRequest()
{
    return message[6];
}

uint16_t ModbusRequestResponseParser::getWriteSingleRegisterValue()
{
    return funcs.MSBLSBJoin(message[4], message[5]);
}

/**
 * For coils to set you should send 0xFF00 and for reset you should send 0x0000
 * If none of those values aren't recived it will return 0x80
 * If true recived it will return 0xFF
 * If false is  received it will return 0x00
 */
uint8_t ModbusRequestResponseParser::getWriteSingleCoilValue()
{
    if (message[4] == 0xFF && message[5] == 0x00) {
        return COIL_VALUE_SET;
    } else if (message[4] == 0x00 && message[5] == 0x00) {
        return COIL_VALUE_RESET;
    } else {
        return COIL_VALUE_ERROR;
    }
}

uint16_t ModbusRequestResponseParser::getRegisterValueByIndexInResponse(uint32_t index)
{
    return funcs.MSBLSBJoin(message[3 + 2 * index], message[4 + 2 * index]);
}

uint16_t ModbusRequestResponseParser::getRegisterValueByIndexInRequest(uint32_t index)
{
    return funcs.MSBLSBJoin(message[7 + 2 * index], message[8 + 2 * index]);
}

boolean ModbusRequestResponseParser::getDigitalValueByIndexInResponse(uint32_t index)
{
    byte b = message[3 + index / 8];
    return (b >> (index % 8)) & 0x01;
}

boolean ModbusRequestResponseParser::getDigitalValueByIndexInRequest(uint32_t index)
{
    byte b = message[7 + index / 8];
    return (b >> (index % 8)) & 0x01;
}

boolean* ModbusRequestResponseParser::getDigitalValuesArray()
{
    // delete[] digitalValues;
    int count = 0;
    switch (getFunctionCode()) {
    case READ_COILS_FUNCTIONCODE:
    case READ_DISCRETE_INPUTS_FUNCTIONCODE:
        count = getByteCountInReponse() * 8;
        // digitalValues = new boolean[count];
        for (int i { 0 }; i < count; i++)
            digitalValues[i] = getDigitalValueByIndexInResponse(i);
        break;
    case WRITE_MULTIPLE_COILS_FUNCTIONCODE:
        count = getByteCountInRequest() * 8;
        // digitalValues = new boolean[count];
        for (int i { 0 }; i < count; i++)
            digitalValues[i] = getDigitalValueByIndexInRequest(i);
        break;

    default:
        return nullptr;
        break;
    }
    return digitalValues;
}

word* ModbusRequestResponseParser::getRegisterValuesArray()
{
    // delete[] registerValues;
    int count = 0;
    switch (getFunctionCode()) {
    case READ_HOLDING_REGISTERS_FUNCTIONCODE:
    case READ_INPUT_REGISTERS_FUNCTIONCODE:
        count = getByteCountInReponse() / 2;
        // registerValues = new word[count];
        for (int i { 0 }; i < count; i++)
            registerValues[i] = getRegisterValueByIndexInResponse(i);
        break;
    case WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE:
        count = getByteCountInRequest() / 2;
        // registerValues = new word[count];
        for (int i { 0 }; i < count; i++)
            registerValues[i] = getRegisterValueByIndexInRequest(i);
        break;

    default:
        return nullptr;
        break;
    }
    return registerValues;
}

ModbusRequestResponseParser::~ModbusRequestResponseParser()
{
    delete[] digitalValues;
    delete[] registerValues;
}