#include "Master.h"

Master::Master(Stream& s,uint16_t digitalValuesBufferSize,uint16_t registerValuesBufferSize)
    : serial(s),parser(nullptr,digitalValuesBufferSize,registerValuesBufferSize)
{

}


boolean Master::writeSingleCoil(byte slaveID, word address, boolean value)
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
        int responseSize = readSerial(message);
        if (responseSize <= 0)
            continue;
        parser.setMessage(message);
        if(!parser.isValidCRC(responseSize))
            continue;
  
        if (messageIsInvalid(parser, MASTER_ID, WRITE_SINGLE_COIL_FUNCTIONCODE))
            continue;
        if (parser.isItException()) {
            exceptionCode = parser.getExceptionCode();
            return false;
        }
        if (parser.getAddress() != address)
            continue;
        if (parser.getWriteSingleCoilValue() == COIL_VALUE_ERROR)
            continue;
        if (value != (parser.getWriteSingleCoilValue() == COIL_VALUE_SET))
            continue;
        failed = false;
    }
    return !failed;
}

boolean Master::writeSingleRegister(byte slaveID, word address, word value)
{
    ModbusRequestCreator slave { slaveID };
    exceptionCode = 0x00;
    int tries = numberOfTries;
    int currentTry = 0;
    boolean failed = true;
    while (tries > 0 && failed) {
        currentTry++;
        tries--;
        int requestSize = slave.createWriteSingleRegisterRequest(address, value);
        serial.setTimeout(timeout);
        serial.write(slave.getMessage(), requestSize);
        delay(delayTime);
        int responseSize = readSerial(message);
        if (responseSize <= 0)
            continue;
        parser.setMessage(message);
        if(!parser.isValidCRC(responseSize))
            continue;
  
        if (messageIsInvalid(parser, MASTER_ID, WRITE_SINGLE_REGISTER_FUNCTIONCODE))
            continue;
        if (parser.isItException()) {
            exceptionCode = parser.getExceptionCode();
            return false;
        }
        if (parser.getAddress() != address)
            continue;
        if (value != parser.getWriteSingleRegisterValue())
            continue;
        failed = false;
    }
    return !failed;
}

boolean Master::readInputRegisters(byte slaveID, word startAddress, word quantity)
{
    ModbusRequestCreator slave { slaveID };
    exceptionCode = 0x00;
    int tries = numberOfTries;
    int currentTry = 0;
    boolean failed = true;
    while (tries > 0 && failed) {
        currentTry++;
        tries--;
        int requestSize = slave.createReadInputRegistersRequest(startAddress, quantity);
        serial.setTimeout(timeout);
        serial.write(slave.getMessage(), requestSize);
        delay(delayTime);
        int responseSize = readSerial(message);
        if (responseSize <= 0)
            continue;
        parser.setMessage(message);
        if(!parser.isValidCRC(responseSize))
            continue;
  
        if (messageIsInvalid(parser, MASTER_ID, READ_INPUT_REGISTERS_FUNCTIONCODE))
            continue;
        if (parser.isItException()) {
            exceptionCode = parser.getExceptionCode();
            return false;
        }
        if (parser.getByteCountInReponse() != 2 * quantity)
            continue;
        registersBuffer = parser.getRegisterValuesArray();
        failed = false;
    }
    return !failed;
}

boolean Master::readHoldingRegisters(byte slaveID, word startAddress, word quantity)
{
    ModbusRequestCreator slave { slaveID };
    exceptionCode = 0x00;
    int tries = numberOfTries;
    int currentTry = 0;
    boolean failed = true;
    while (tries > 0 && failed) {
        currentTry++;
        tries--;
        int requestSize = slave.createReadHoldingRegistersRequest(startAddress, quantity);
        serial.setTimeout(timeout);
        serial.write(slave.getMessage(), requestSize);

        delay(delayTime);
        int responseSize = readSerial(message);
        if (responseSize <= 0)
            continue;
        parser.setMessage(message);
        if(!parser.isValidCRC(responseSize)){
            continue;
        }
  
        if (messageIsInvalid(parser, MASTER_ID, READ_HOLDING_REGISTERS_FUNCTIONCODE))
            continue;
        if (parser.isItException()) {
            exceptionCode = parser.getExceptionCode();
            return false;
        }
        if (parser.getByteCountInReponse() != 2 * quantity)
            continue;
        registersBuffer = parser.getRegisterValuesArray();
        failed = false;
    }
    return !failed;
}

boolean Master::readCoils(byte slaveID, word startAddress, word quantity)
{
    ModbusRequestCreator slave { slaveID };
    exceptionCode = 0x00;
    int tries = numberOfTries;
    int currentTry = 0;
    boolean failed = true;
    while (tries > 0 && failed) {
        currentTry++;
        tries--;
        int requestSize = slave.createReadCoilsRequest(startAddress, quantity);
        serial.setTimeout(timeout);
        serial.write(slave.getMessage(), requestSize);
        delay(delayTime);
        int responseSize = readSerial(message);
        if (responseSize <= 0)
            continue;
        parser.setMessage(message);
        if(!parser.isValidCRC(responseSize))
            continue;
  
        if (messageIsInvalid(parser, MASTER_ID, READ_COILS_FUNCTIONCODE))
            continue;
        if (parser.isItException()) {
            exceptionCode = parser.getExceptionCode();
            return false;
        }
        if (parser.getByteCountInReponse() != (quantity + 7) / 8)
            continue;
        digitalsBuffer = parser.getDigitalValuesArray();
        failed = false;
    }
    return !failed;
}

boolean Master::readDiscreteInputs(byte slaveID, word startAddress, word quantity)
{
    ModbusRequestCreator slave { slaveID };
    exceptionCode = 0x00;
    int tries = numberOfTries;
    int currentTry = 0;
    boolean failed = true;
    while (tries > 0 && failed) {
        currentTry++;
        tries--;
        int requestSize = slave.createReadDiscreteInputsRequest(startAddress, quantity);
        serial.setTimeout(timeout);
        serial.write(slave.getMessage(), requestSize);
        delay(delayTime);
        int responseSize = readSerial(message);
        if (responseSize <= 0)
            continue;
        parser.setMessage(message);
        if(!parser.isValidCRC(responseSize))
            continue;
  
        if (messageIsInvalid(parser, MASTER_ID, READ_DISCRETE_INPUTS_FUNCTIONCODE))
            continue;
        if (parser.isItException()) {
            exceptionCode = parser.getExceptionCode();
            return false;
        }
        if (parser.getByteCountInReponse() != (quantity + 7) / 8)
            continue;
        digitalsBuffer = parser.getDigitalValuesArray();
        failed = false;
    }
    return !failed;
}

boolean Master::writeCoils(byte slaveID, word startAddress, boolean* values, word quantity)
{
    ModbusRequestCreator slave { slaveID };
    exceptionCode = 0x00;
    int tries = numberOfTries;
    int currentTry = 0;
    boolean failed = true;
    while (tries > 0 && failed) {
        currentTry++;
        tries--;
        int requestSize = slave.createWriteCoilsRequest(startAddress, values, quantity);
        serial.setTimeout(timeout);
        serial.write(slave.getMessage(), requestSize);
        delay(delayTime);
       int responseSize = readSerial(message);
        if (responseSize <= 0)
            continue;
        parser.setMessage(message);
        if(!parser.isValidCRC(responseSize))
            continue;
  
        if(messageIsInvalid(parser,MASTER_ID,WRITE_MULTIPLE_COILS_FUNCTIONCODE))
            continue;
        if(parser.isItException()){
            exceptionCode = parser.getExceptionCode();
            return false;
        }
        if(parser.getAddress() != startAddress)
            continue;
        if(parser.getNumberOfRegsOrDigitals() != quantity)
            continue;
        failed = false;
    }
    return !failed;
}

boolean Master::writeHoldingRegisters(byte slaveID, word startAddress, word* values, word quantity)
{
    ModbusRequestCreator slave { slaveID };
    exceptionCode = 0x00;
    int tries = numberOfTries;
    int currentTry = 0;
    boolean failed = true;
    while (tries > 0 && failed) {
        currentTry++;
        tries--;
        int requestSize = slave.createWriteRegistersRequest(startAddress, values, quantity);
        serial.setTimeout(timeout);
        serial.write(slave.getMessage(), requestSize);
        /////////////////////// to be deleted
        // Serial.println("REQUEST MESSAGE:");
        // for (int i = 0; i < requestSize; i++) {
        //     Serial.print(slave.getMessage()[i], HEX);
        //     Serial.print(" ");
        // }
        // Serial.println();
    /////////////////////////////////
        delay(delayTime);
        int responseSize = readSerial(message);
        if (responseSize <= 0)
            continue;
        parser.setMessage(message);
        if(!parser.isValidCRC(responseSize))
            continue;
  
        if(messageIsInvalid(parser,MASTER_ID,WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE))
            continue;
        if(parser.isItException()){
            exceptionCode = parser.getExceptionCode();
            return false;
        }
        if(parser.getAddress() != startAddress)
            continue;
        if(parser.getNumberOfRegsOrDigitals() != quantity)
            continue;
        failed = false;
    }
    return !failed;
}

boolean Master::messageIsInvalid(ModbusRequestResponseParser &parser, byte myID, byte expectedFunctionCode)
{   
    if (myID != parser.getSlaveID())
        return true;
    if (parser.isItException())
        return ((byte) parser.getFunctionCode() != (byte) (expectedFunctionCode + EXCEPTION_OFFSET));
    return parser.getFunctionCode() != expectedFunctionCode;
}

unsigned short Master::readSerial(byte* buffer)
{
    unsigned short position{};
    boolean overflow=false;
    while (serial.available())
    {
        // The maximum number of bytes is limited to the serial buffer size of 128 bytes
        // If more bytes is received than the BUFFER_SIZE the overflow flag will be set and the
        // serial buffer will be red untill all the data is cleared from the receive buffer.
        // Adopted from: https://github.com/angeloc/simplemodbusng/blob/master/SimpleModbusSlave/SimpleModbusSlave.cpp
        if (overflow)
            serial.read();
        else {
            if (position == SERIAL_MESSAGE_BUFFER_SIZE)
                overflow = true;
            buffer[position] = serial.read();
            position++;
        }
        // delay(READING_SERIAL_BYTE_INTERVAL_DELAY);
    }
    
    return position;
}

Master::~Master()
{
}
