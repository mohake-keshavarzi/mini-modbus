#include "Slave.h"

unsigned short Slave::readSerial(byte* buffer)
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
        if(interCharDelay>0)
            delay(interCharDelay);
    }
    
    return position;
}

Slave::Slave(uint16_t id, Stream& s, uint16_t digitalValuesBufferSize, uint16_t registerValuesBufferSize)
    : id(id)
    , serial(s)
    , parser(nullptr, digitalValuesBufferSize, registerValuesBufferSize)
{
}

void Slave::setup(int baudRate)
{
    this->baudRate=baudRate;
    interCharDelay= 1000000 / baudRate / 10; // 10 bits per character
}

void Slave::setInterCharDelay(unsigned int delay)
{
    interCharDelay = delay;
}

void Slave::setCoilsRefrence(boolean* coilsArray, uint16_t size)
{
    responseCreator.setCoilsRefrence(coilsArray, size);
}

void Slave::setDiscreteInputsRefrence(boolean* inputDiscretesArray, uint16_t size)
{
    responseCreator.setDiscreteInputsRefrence(inputDiscretesArray, size);
}

void Slave::setHoldingRegistersRefrence(word* holdingRegistersArray, uint16_t size)
{
    responseCreator.setHoldingRegistersRefrence(holdingRegistersArray, size);
}

void Slave::setInputRegistersRefrence(word* inputRegistersArray, uint16_t size)
{
    responseCreator.setInputRegistersRefrence(inputRegistersArray, size);
}

void Slave::execute()
{
    int messageSize = readSerial(message);
    int responseSize = 0;

    if (messageSize > 0) {
        parser.setMessage(message);
        
        if (parser.getSlaveID() != id){
            return;
        }
        if (!parser.isValidCRC(messageSize)){
            return; //RECOMENDED BAUDRATE IS 115200
        }
        
        switch (parser.getFunctionCode()) {

        case WRITE_SINGLE_COIL_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), 0, COILS)) {
                responseSize = responseCreator.createExceptionResponse(WRITE_SINGLE_COIL_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                uint8_t coil_value = parser.getWriteSingleCoilValue();
                if (coil_value != COIL_VALUE_ERROR) {
                    boolean value = (coil_value == COIL_VALUE_SET);
                    responseSize = responseCreator.createWriteSingleCoilResponse(parser.getAddress(), value);
                    if (onWriteDigitalfunc != nullptr)
                        onWriteDigitalfunc(COILS, parser.getAddress(), &(value), 0);
                } else
                    responseSize = responseCreator.createExceptionResponse(WRITE_SINGLE_COIL_FUNCTIONCODE, INVALID_DATA_VALUE_EXCEPTION_CODE);
            }

            break;

        case READ_INPUT_REGISTERS_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), parser.getNumberOfRegsOrDigitals(), INPUT_REGISTERS)) {
                responseSize = responseCreator.createExceptionResponse(READ_INPUT_REGISTERS_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                responseSize = responseCreator.createReadInputRegistersResponse(parser.getAddress(), parser.getNumberOfRegsOrDigitals());
                if (onReadRegisterfunc != nullptr)
                    onReadRegisterfunc(INPUT_REGISTERS, parser.getAddress(), parser.getNumberOfRegsOrDigitals());
            }
            break;

        case WRITE_SINGLE_REGISTER_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), 0, HOLDING_REGISTERS)) {
                responseSize = responseCreator.createExceptionResponse(WRITE_SINGLE_REGISTER_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                word value = parser.getWriteSingleRegisterValue();
                responseSize = responseCreator.createWriteSingleRegisterResponse(parser.getAddress(), value);
                if (onWriteRegisterfunc != nullptr)
                    onWriteRegisterfunc(HOLDING_REGISTERS, parser.getAddress(), &value, 0);
            }
            break;

        case READ_COILS_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), parser.getNumberOfRegsOrDigitals(), COILS)) {
                responseSize = responseCreator.createExceptionResponse(READ_COILS_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                responseSize = responseCreator.createReadCoilsResponse(parser.getAddress(), parser.getNumberOfRegsOrDigitals());
                if (onReadDigitalfunc != nullptr)
                    onReadDigitalfunc(COILS, parser.getAddress(), parser.getNumberOfRegsOrDigitals());
            }
            break;

        /////////////
        case WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), parser.getNumberOfRegsOrDigitals(), HOLDING_REGISTERS)) {
                responseSize = responseCreator.createExceptionResponse(WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                word* values { parser.getRegisterValuesArray() };
                responseSize = responseCreator.createWriteRegistersResponse(parser.getAddress(), values, parser.getNumberOfRegsOrDigitals());
                if (onWriteRegisterfunc != nullptr)
                    onWriteRegisterfunc(HOLDING_REGISTERS, parser.getAddress(), values, parser.getNumberOfRegsOrDigitals());
            }
            break;

        case READ_DISCRETE_INPUTS_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), parser.getNumberOfRegsOrDigitals(), DISCRETE_INPUTS)) {
                responseSize = responseCreator.createExceptionResponse(READ_DISCRETE_INPUTS_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                responseSize = responseCreator.createReadDiscreteInputsResponse(parser.getAddress(), parser.getNumberOfRegsOrDigitals());
                if (onReadDigitalfunc != nullptr)
                    onReadDigitalfunc(DISCRETE_INPUTS, parser.getAddress(), parser.getNumberOfRegsOrDigitals());
            }
            break;

        case WRITE_MULTIPLE_COILS_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), parser.getNumberOfRegsOrDigitals(), COILS)) {
                responseSize = responseCreator.createExceptionResponse(WRITE_MULTIPLE_COILS_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                boolean* values { parser.getDigitalValuesArray() };
                responseSize = responseCreator.createWriteCoilsResponse(parser.getAddress(), values, parser.getNumberOfRegsOrDigitals());
                if (onWriteDigitalfunc != nullptr)
                    onWriteDigitalfunc(COILS, parser.getAddress(), values, parser.getNumberOfRegsOrDigitals());
            }
            break;

        case READ_HOLDING_REGISTERS_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), parser.getNumberOfRegsOrDigitals(), HOLDING_REGISTERS)) {
                responseSize = responseCreator.createExceptionResponse(READ_HOLDING_REGISTERS_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                responseSize = responseCreator.createReadHoldingRegistersResponse(parser.getAddress(), parser.getNumberOfRegsOrDigitals());
                if (onReadRegisterfunc != nullptr)
                    onReadRegisterfunc(HOLDING_REGISTERS, parser.getAddress(), parser.getNumberOfRegsOrDigitals());
            }
            break;

        default:
            responseSize = responseCreator.createExceptionResponse(parser.getFunctionCode(), INVALID_FUNCTION_CODE_EXCEPTION_CODE);
            break;
        }
        newCommandRecieved = true;
        serial.write(responseCreator.getMessage(), responseSize);
    }
    serial.flush();
    messageSize = 0;
    
    delay(delayTime);

}

void Slave::setOnReadRegisterRequestFunction(void (*func)(int type, word startingAddress, int quantity))
{
    onReadRegisterfunc = func;
}

void Slave::setOnWriteRegisterRequestFunction(void (*func)(int type, word startingAddress, word* values, int quantity))
{
    onWriteRegisterfunc = func;
}

void Slave::setOnReadDigitalRequestFunction(void (*func)(int type, word startingAddress, int quantity))
{
    onReadDigitalfunc = func;
}

void Slave::setOnWriteDigitalRequestFunction(void (*func)(int type, word startingAddress, boolean* values, int quantity))
{
    onWriteDigitalfunc = func;
}

Slave::~Slave()
{
}
