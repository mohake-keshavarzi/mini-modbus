/// @file ModbusResponseCreator.h
///
/// @brief This header file is used for creating Response frames and manages memories based on received request in modbus.

#ifndef MODBUS_RESPONSE_CREATOR_H
#define MODBUS_RESPONSE_CREATOR_H

#include "Arduino.h"
#include "FunctionCodes.h"
#include "WordFunctions.h"

/// @brief A class with necessery tools to make response messages and manage memory blocks in Slave.
///
/// Each instance of this class is exculusively for one Slave and you should just make one instance of it per communication line in your slave.
/// This class gets some refrencess to the arrays that you have made as data blocks. Then by asking it to make any of the response frames it will also mange the memory and write or read the needed datas.
///
class ModbusResponseCreator {
private:
    boolean *coils = nullptr, *discreteInputs = nullptr;
    word *holdingRegisters = nullptr, *inputRegisters = nullptr;
    uint32_t coilsSize = 0, holdingRegsSize = 0, inputRegsSize = 0, discreteInputsSize = 0;
    ;
    byte* message;
    uint16_t message_size;

    WordFunctions funcs {};

    /// @brief Creates a response for Read Coils or Read Discrete Inputs request.
    /// @param startAddress The address of the first coil or discrete input to be read.
    /// @param quantity The number of coils or discrete inputs to be read following the start address.
    /// @param type The type of memory: COILS or DISCRETE_INPUTS.
    /// @return The size of the created message in number of bytes.
    uint16_t createReadCoilsOrDiscreteInputsResponse(uint16_t startAddress, uint16_t quantity, uint8_t type);

    /// @brief Creates a response for Read Holding Registers or Read Input Registers request.
    /// @param startAddress The address of the first Holding register or Input Register to be read.
    /// @param numOfRegisters The number of Holding registers or Input Registers to be read following the start address.
    /// @param type The type of memory: HOLDING_REGISTERS or INPUT_REGISTERS.
    /// @return The size of the message in number of bytes.
    uint16_t createReadHoldingOrInputRegistersResponse(uint16_t startAddress, uint16_t numOfRegisters, uint8_t type);

    /**
     * Function to add CRC16 code to the ned of the message
     * @param message_size Size of the input message
     * @return The new size of the message 
    */
    uint16_t addCRC(uint16_t message_size);
public:
    /// @brief Creates an instance of the class.
    ModbusResponseCreator();

    /// @brief Sets the reference to the array of coils.
    ///
    /// The code uses this refrence as the start of an array which represents the block of Coils.
    /// Be careful to not delete this refrence in your program and enter the size of the array correctly.
    ///
    /// @param coils_refrence A pointer to the first element of the array.
    /// @param size The size of the array in number of elements.
    void setCoilsRefrence(boolean* coils_refrence, uint16_t size);

    /// @brief Sets the reference to the array of discrete inputs.
    ///
    /// The code uses this refrence as the start of an array which represents the block of Discrete Inputs.
    /// Be careful to not delete this refrence in your program and enter the size of the array correctly.
    ///
    /// @param inputDiscretes_refrence A pointer to the first element of the array.
    /// @param size The size of the array in number of elements.
    void setDiscreteInputsRefrence(boolean* inputDiscretes_refrence, uint16_t size);

    /// @brief Sets the reference to the array of holding registers.
    ///
    /// The code uses this refrence as the start of an array which represents the block of Holding Registers.
    /// Be careful to not delete this refrence in your program and enter the size of the array correctly.
    ///
    /// @param holdingRegisters_refrence A pointer to the first element of the array.
    /// @param size The size of the array in number of elements.
    void setHoldingRegistersRefrence(word* holdingRegisters_refrence, uint16_t size);

    /// @brief Sets the reference to the array of input registers.
    ///
    /// The code uses this refrence as the start of an array which represents the block of Input Registers.
    /// Be careful to not delete this refrence in your program and enter the size of the array correctly.
    ///
    /// @param inputRegisters_refrence A pointer to the first element of the array.
    /// @param size The size of the array in number of elements.
    void setInputRegistersRefrence(word* inputRegisters_refrence, uint16_t size);

    /// @brief Creates a response message for a request to read coils starting from the given address.
    ///
    /// It uses the coils refrence given to the setCoilsRefrence function.
    ///
    /// @param startAddress The address of the first coil to be read.
    /// @param quantity The number of coils to be read following the start address.
    /// @return The size of the response message in number of bytes.
    uint16_t createReadCoilsResponse(uint16_t startAddress, uint16_t quantity)
    {
        return createReadCoilsOrDiscreteInputsResponse(startAddress, quantity, COILS);
    };

    /// @brief Creates a response message for a request to read discrete inputs starting from the given address.
    ///
    /// It uses the discrete inputs refrence given to the setDiscreteInputsRefrence function.
    ///
    /// @param startAddress The address of the first discrete input to be read.
    /// @param quantity The number of discrete inputs to be read following the start address.
    /// @return The size of the response message in number of bytes.
    uint16_t createReadDiscreteInputsResponse(uint16_t startAddress, uint16_t quantity)
    {
        return createReadCoilsOrDiscreteInputsResponse(startAddress, quantity, DISCRETE_INPUTS);
    };

    /// @brief Creates a response message for a request to write a single holding register.
    ///
    /// It uses the Holding registers refrence given to the setHoldingRegistersRefrence function. also sets the new value for the register in memory.
    ///
    /// @param address The address of the holding register to be written.
    /// @param value The value to be written to the holding register.
    /// @return The size of the response message in number of bytes.
    uint16_t createWriteSingleRegisterResponse(uint16_t address, word value);

    /// @brief Creates a response message for a request to read input registers starting from the given address.
    ///
    /// It uses the input registers reference given to the setInputRegistersReference function.
    ///
    /// @param startAddress The address of the first input register to be read.
    /// @param numOfRegisters The number of input registers to be read following the start address.
    /// @return The size of the response message in number of bytes.
    uint16_t createReadInputRegistersResponse(uint16_t startAddress, uint16_t numOfRegisters)
    {
        return createReadHoldingOrInputRegistersResponse(startAddress, numOfRegisters, INPUT_REGISTERS);
    };

    /// @brief Creates a response message for a request to read holding registers starting from the given address.
    ///
    /// It uses the holding registers reference given to the setHoldingRegistersReference function.
    ///
    /// @param startAddress The address of the first holding register to be read.
    /// @param numOfRegisters The number of holding registers to be read following the start address.
    /// @return The size of the response message in number of bytes.
    uint16_t createReadHoldingRegistersResponse(uint16_t startAddress, uint16_t numOfRegisters)
    {
        return createReadHoldingOrInputRegistersResponse(startAddress, numOfRegisters, HOLDING_REGISTERS);
    };

    /// @brief Creates a response message for a request to write a single coil.
    ///
    /// It uses the coil reference given to the setCoilsReference function. also writes the new value for the coil in memory.
    ///
    /// @param address The address of the coil to be written.
    /// @param value The value to be written to the coil.
    /// @return The size of the response message in number of bytes.
    uint16_t createWriteSingleCoilResponse(uint16_t address, boolean value);

    /// @brief Creates a response message for a request to write multiple coils starting from the given address.
    ///
    /// It uses the coil reference given to the setCoilsReference function. also writes the new values for the coils in memory.
    ///
    /// @param startAddress The address of the first coil to be written.
    /// @param values The array of values to be written to the coils.
    /// @param quantity The number of coils to be written following the start address.
    /// @return The size of the response message in number of bytes.
    uint16_t createWriteCoilsResponse(uint16_t startAddress, boolean* values, uint16_t quantity);

    /// @brief Creates a response message for a request to write multiple holding registers starting from the given address.
    ///
    /// It uses the holding registers reference given to the setHoldingRegistersReference function. also writes the new values for the registers in memory.
    ///
    /// @param startAddress The address of the first holding register to be written.
    /// @param values The array of values to be written to the holding registers.
    /// @param quantity The number of holding registers to be written following the start address.
    /// @return The size of the response message in number of bytes.
    uint16_t createWriteRegistersResponse(uint16_t startAddress, word* values, uint16_t quantity);

    /// @brief Creates an exception response message for a given function code and exception code.
    ///
    /// @param functionCode The function code of the original request.
    /// @param exceptionCode The exception code to be sent in the response.
    /// @return The size of the response message in number of bytes.
    uint16_t createExceptionResponse(uint8_t functionCode, uint8_t exceptionCode);

    /// @brief Checks if the given data address is invalid for the given type of memory.
    ///
    /// @param startAddress The address of the first data to be read or written.
    /// @param numberOfCoilsOrRegisters The number of data to be read or written following the start address.
    /// @param typeOfMemory The type of memory (coils, discrete inputs, holding registers, or input registers).
    /// @return True if the data address is invalid, false otherwise.
    boolean isDataAddressWrong(uint16_t startAddress, uint16_t numberOfCoilsOrRegisters, uint8_t typeOfMemory);

    /// @brief Returns a pointer to the message buffer used to store the Modbus response message.
    ///
    /// @return A pointer to the message buffer.
    byte* getMessage() { return message; }
    ~ModbusResponseCreator();
};

#endif