/// @file ModbusRequestResponseParser.h
/// @brief This header files contains necessary tools to extract features from Request and Response messages.

#ifndef MODBUS_REQUEST_RESPONSE_PARSER_H
#define MODBUS_REQUEST_RESPONSE_PARSER_H

#include "Arduino.h"
#include "WordFunctions.h"

/// @brief It gets a message and extracts datas and features needed from it.
class ModbusRequestResponseParser {
private:
    byte* message;
    WordFunctions funcs {};

    boolean* digitalValues; // Digital value refers to both coil and Discrete input values
    word* registerValues;

public:
    /// @brief Creates an instance of the class and gets a pointer to the message.
    /// @param message  Pointer to an aray of bytes containing the message.
    ModbusRequestResponseParser(byte* message);

    /// @brief Sets the the message.
    /// @param message pointer to array of bytes containing the message.
    void setMessage(byte* message) { this->message = message; };

    /// @brief Extracts the Slave ID from the message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A byte containing the Slave Id.
    uint8_t getSlaveID();

    /// @brief Extracts the Function Code from the message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A byte containing the Function Code.
    uint8_t getFunctionCode();

    /// @brief Extracts the Starting address or requested address from the message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A word containing the address.
    uint16_t getAddress();

    /// @brief Extracts the quantity of registers or digitals from the message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A word containing the quantity.
    uint16_t getNumberOfRegsOrDigitals();

    /// @brief Extracts the byte count field from the Response message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A byte containing the byte count.
    uint8_t getByteCountInReponse();

    /// @brief Extracts the byte count field from the Request message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A byte containing the byte count.
    uint8_t getByteCountInRequest();

    /// @brief Extracts the value of the Register in the Write Single Register message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A word containing the register value.
    uint16_t getWriteSingleRegisterValue();

    /// @brief Extracts the value of the Coil in the Write Single Coil message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A byte containing the Coil value.(COIL_VALUE_SET for true, COIL_VALUE_RESET for false, COIL_VALUE_ERROR if the field of data contains an unacceptable value).
    uint8_t getWriteSingleCoilValue();

    /// @brief Extracts the value of a Holding Register or Input Register in Response message by the index you have given.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A word containing the register value.
    uint16_t getRegisterValueByIndexInResponse(uint32_t index);

    /// @brief Extracts the value of a Holding Register or Input Register in Request message by the index you have given.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A word containing the register value.
    uint16_t getRegisterValueByIndexInRequest(uint32_t index);

    /// @brief Extracts the value of a Coil or Discrete input in Response message by the index you have given.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A boolean containing the digital value.
    boolean getDigitalValueByIndexInResponse(uint32_t index);

    /// @brief Extracts the value of a Coil or Discrete input in Request message by the index you have given.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A boolean containing the digital value.
    boolean getDigitalValueByIndexInRequest(uint32_t index);

    /// @brief Extracts the values of Coils or Discrete Inputs in message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// Do not use this function for Write Single Coil Function Code.
    /// @return A boolean pointer to the array of digital values.
    boolean* getDigitalValuesArray();

    /// @brief Extracts the values of Holding Register or Input Register in message.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// Do not use this function for Write Single Register Function Code.
    /// @return A word pointer to the array of Register values.
    word* getRegisterValuesArray();


    /// @brief Weather the message is an exception message or not.
    /// @return Returns true if the message is an exception message.
    boolean isItException();
    
    /// @brief Extracts the value of the excception code.
    ///
    /// Note that if the data doesn't exist it will return wrong data or a null pointer runtime error will happen.
    /// @return A byte containing the exception code value.    
    uint8_t getExceptionCode();

    ~ModbusRequestResponseParser();
};

#endif