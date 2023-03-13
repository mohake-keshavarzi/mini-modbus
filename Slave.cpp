#include "Slave.h"

Slave::Slave(uint16_t id, Stream& s)
    : id(id)
    , serial(s)
{
}

void Slave::setCoilsRefrence(boolean* coilsArray, uint16_t size)
{
    responseCreator.setCoilsRefrence(coilsArray, size);
}

void Slave::setInputDiscretesRefrence(boolean* inputDiscretesArray, uint16_t size)
{
    responseCreator.setInputDiscretesRefrence(inputDiscretesArray, size);
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

    int messageSize = serial.available();
    int responseSize = 0;
    if (messageSize > 0) {
        serial.readBytes(message, messageSize);
        parser.setMessage(message);
        if (parser.getSlaveID() != id)
            return;
        switch (parser.getFunctionCode()) {

        case WRITE_SINGLE_COIL_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), 0, COILS)) {
                responseSize = responseCreator.createExceptionResponse(WRITE_SINGLE_COIL_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                uint8_t coil_value = parser.getWriteSingleCoilValue();
                if (coil_value != COIL_VALUE_ERROR)
                    responseSize = responseCreator.createWriteSingleCoilResponse(parser.getAddress(), coil_value == COIL_VALUE_SET);
                else
                    responseSize = responseCreator.createExceptionResponse(WRITE_SINGLE_COIL_FUNCTIONCODE, INVALID_DATA_VALUE_EXCEPTION_CODE);
            }

            break;

        case READ_INPUT_REGISTERS_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), parser.getNumberOfRegsOrCoils(), INPUT_REGISTERS)) {
                responseSize = responseCreator.createExceptionResponse(READ_INPUT_REGISTERS_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                responseSize = responseCreator.createReadInputRegistersResponse(parser.getAddress(), parser.getNumberOfRegsOrCoils());
            }
            break;

        case WRITE_SINGLE_REGISTER_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), 0, HOLDING_REGISTERS)) {
                responseSize = responseCreator.createExceptionResponse(WRITE_SINGLE_REGISTER_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                responseSize = responseCreator.createWriteSingleRegisterResponse(parser.getAddress(), parser.getWriteSingleRegisterValue());
            }
            break;

        case READ_COILS_FUNCTIONCODE:
            if (responseCreator.isDataAddressWrong(parser.getAddress(), parser.getNumberOfRegsOrCoils(), COILS)) {
                responseSize = responseCreator.createExceptionResponse(READ_COILS_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
            } else {
                responseSize = responseCreator.createReadCoilsResponse(parser.getAddress(), parser.getNumberOfRegsOrCoils());
            }
            break;

        default:
            responseSize = responseCreator.createExceptionResponse(parser.getFunctionCode(), INVALID_FUNCTION_CODE_EXCEPTION_CODE);
            break;
        }
        serial.write(responseCreator.getMessage(), responseSize);
    }
    serial.flush();
    messageSize = 0;
    delay(10);
}

Slave::~Slave()
{
}
