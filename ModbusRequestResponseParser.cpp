#include "ModbusRequestResponseParser.h"
#include "WordFunctions.h"
#include "FunctionCodes.h"


//**************TODO: add data exists checking***********************//

ModbusRequestResponseParser::ModbusRequestResponseParser(byte *message)
:message{message}
{
}

uint8_t ModbusRequestResponseParser::getFunctionCode()
{
    return message[1];
}

boolean ModbusRequestResponseParser::isItException()
{
    return getFunctionCode()>0x80;
}

uint8_t ModbusRequestResponseParser::getExceptionCode()
{
    if(isItException()){
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
    return  funcs.MSBLSBJoin(message[2],message[3]);
}

uint16_t ModbusRequestResponseParser::getNumberOfRegsOrCoils()
{
    return  funcs.MSBLSBJoin(message[4],message[5]);
}

uint8_t ModbusRequestResponseParser::getByteCount()
{
    return message[2];

}
 
uint16_t ModbusRequestResponseParser::getWriteSingleRegisterValue()
{
    return  funcs.MSBLSBJoin(message[4],message[5]);
}

/**
 * For coils to set you should send 0xFF00 and for reset you should send 0x0000
 * If none of those values aren't recived it will return 0x80
 * If true recived it will return 0xFF
 * If false is  received it will return 0x00
*/
uint8_t ModbusRequestResponseParser::getWriteSingleCoilValue()
{
    if(message[4]== 0xFF && message[5]==0x00){
        return COIL_VALUE_SET;
    }else if(message[4]== 0x00 && message[5]==0x00){
        return COIL_VALUE_RESET;
    }else{
        return COIL_VALUE_ERROR;
    }
}

uint16_t ModbusRequestResponseParser::getInputRegistersValues(uint32_t index)
{   
        return funcs.MSBLSBJoin(message[3+2*index],message[4+2*index]);
}

ModbusRequestResponseParser::~ModbusRequestResponseParser()
{}