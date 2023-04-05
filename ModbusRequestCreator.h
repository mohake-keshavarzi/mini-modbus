/// @file ModbusRequestCreator.h
/// @brief This header file is used for creating Request frames in modbus.

#ifndef MODBUS_REQUEST_CREATOR_H
#define MODBUS_REQUEST_CREATOR_H

#include "Arduino.h"
#include "WordFunctions.h"

/// @brief A class with necessery tools to make Request packages to send from Master to an specific Slave.
///
/// Each instance of this class is responible for one slave.
///
class ModbusRequestCreator {
private:
    uint8_t slaveID = 0;
    WordFunctions funcs {};
    byte* message;
    /**
     * Function to add CRC16 code to the ned of the message
     * @param message_size Size of the input message
     * @return The new size of the message 
    */
    uint16_t addCRC(uint16_t message_size);
public:
    /// @brief Creates base instance.
    /// @param slaveID ID of the slave that this instance is responsible to make request frames for.
    ModbusRequestCreator(uint8_t slaveID);

    /// @brief Creates the message for a Read Multiple Coils reqest.
    /// @param startAddress The address of first coil to be read.
    /// @param numberOfCoils Number of coils to be read following the start address.
    /// @return The size of the message in number of bytes.
    uint16_t createReadCoilsRequest(uint16_t startAddress, uint16_t numberOfCoils);

    /// @brief Creates the message for a Read Multiple Discrete Inputs reqest.
    /// @param startAddress The address of first Discrete Input to be read.
    /// @param numberOfCoils Number of discrete inputs to be read.
    /// @return The size of the message in number of bytes.
    uint16_t createReadDiscreteInputsRequest(uint16_t startAddress, uint16_t numberOfCoils);

    /**
     * @brief Creates a message for a Write Single Register request.
     *
     * This function creates a message that requests the value of a single holding register
     * to be written to a specified address.
     *
     * @param address The address of the register to be written.
     * @param value The value to be written to the register.
     *
     * @return The size of the message in number of bytes.
     */
    uint16_t createWriteSingleRegisterRequest(uint16_t address, word value);

    /**
     * @brief Creates a message for a Read Input Registers request.
     *
     * This function creates a message that requests the values of a specified number of input
     * registers to be read starting from a specified address.
     *
     * @param startAddress The address of the first input register to be read.
     * @param numOfRegisters The number of input registers to be read following the start address.
     *
     * @return The size of the message in number of bytes.
     */
    uint16_t createReadInputRegistersRequest(uint16_t startAddress, uint16_t numOfRegisters);

    /**
     * @brief Creates a message for a Read Holding Registers request.
     *
     * This function creates a message that requests the values of a specified number of holding
     * registers to be read starting from a specified address.
     *
     * @param startAddress The address of the first holding register to be read.
     * @param numOfRegisters The number of holding registers to be read following the start address.
     *
     * @return The size of the message in number of bytes.
     */
    uint16_t createReadHoldingRegistersRequest(uint16_t startAddress, uint16_t numOfRegisters);

    /**
     * @brief Creates a message for a Write Single Coil request.
     *
     * This function creates a message that requests the value of a single coil
     * to be written to a specified address.
     *
     * @param address The address of the coil to be written.
     * @param value The value to be written to the coil.
     *
     * @return The size of the message in number of bytes.
     */
    uint16_t createWriteSingleCoilRequest(uint16_t address, boolean value);

    /**
     * @brief Creates a message for a Write Coils request.
     *
     * This function creates a message that requests the values of a specified number of coils
     * to be written starting from a specified address.
     *
     * @param startAddress The address of the first coil to be written.
     * @param values A pointer to an array of boolean values to be written to the coils.
     * @param quantity The number of coils to be written following the start address.
     *
     * @return The size of the message in number of bytes.
     */
    uint16_t createWriteCoilsRequest(uint16_t startAddress, boolean* values, uint16_t quantity);

    /**
     * @brief Creates a message for a Write Registers request.
     *
     * This function creates a message that requests the values of a specified number of holding
     * registers to be written starting from a specified address.
     *
     * @param startAddress The address of the first holding register to be written.
     * @param values A pointer to an array of word values to be written to the registers.
     * @param quantity The number of registers to be written following the start address.
     *
     * @return The size of the message in number of bytes.
     */
    uint16_t createWriteRegistersRequest(uint16_t startAddress, word* values, uint16_t quantity);

    /**
     * @brief Returns the message that has been created.
     * 
     * This function gives you the final message.
     * 
     * @return A pointer to the an array of bytes which represents the message.
    */
    byte* getMessage() { return this->message; }
    ~ModbusRequestCreator();
};

#endif