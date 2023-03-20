#include "Master.h"

master::master(Stream& s): serial ( s )
{
}

boolean master::writeSingleCoil(byte slaveID, word address, boolean value)
{
    ModbusRequestCreator slave { slaveID };
    exceptionCode = 0x00;
    int tries = numberOfTries;
    int currentTry = 0;
    boolean failed = true;
    while (tries > 0 && failed) {
        currentTry++;
        tries--;
        int requestSize = slave.createWriteSingleCoilRequest(address, value);
        serial.setTimeout(timeout);
        serial.write(slave.getMessage(), requestSize);
        delay(delayTime);
        int responseSize = serial.available();
        if (responseSize <= 0)
            continue;
        serial.readBytes(message, responseSize);
        parser.setMessage(message);
        if(messageIsInvalid(parser,MASTER_ID,WRITE_SINGLE_COIL_FUNCTIONCODE))
            continue;
        if(parser.isItException()){
            exceptionCode = parser.getExceptionCode();
            return false;
        }
        if(parser.getAddress()!= address)
            continue;
        if(parser.getWriteSingleCoilValue() == COIL_VALUE_ERROR)
            continue;
        if(value != (parser.getWriteSingleCoilValue() == COIL_VALUE_SET) )
            continue;
        failed = false;
    }
    return !failed;
}

boolean master::messageIsInvalid(ModbusRequestResponseParser parser, byte myID, byte expectedFunctionCode)
{
    if (myID != parser.getSlaveID())
        return true;
    if (parser.isItException())
        if (parser.getFunctionCode() != expectedFunctionCode + EXCEPTION_OFFSET)
            return true;
    return parser.getFunctionCode() != expectedFunctionCode;
}

master::~master()
{
}