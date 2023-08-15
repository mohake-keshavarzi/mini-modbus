package ir.m.h.keshavarzi.minimodbus;



public class ModbusResponseCreator {
    private boolean[] coils;
    private int coilsSize;
    private boolean[] discreteInputs;
    private int discreteInputsSize;
    private short[] holdingRegisters;
    private int holdingRegsSize;
    private short[] inputRegisters;
    private int inputRegsSize;
    private byte[] message;
    private int message_size;

    private final static int UINT16_T_SIZE=65536;
    public ModbusResponseCreator() {
        this.message = new byte[8];
        this.message_size = 8;
    }

    private int addCRC(byte[] message,int start, int length){
        short crc = WordFunctions.CRC16.calculate(message,0,length);
        message[length++]=WordFunctions.getLSByte(crc);
        message[length++]=WordFunctions.getMSByte(crc);

        return length;
    }
    public void setCoilsReference(boolean[] coils_reference, int size) {
        if(!isInRangeOfUint16_t(size))
            throw new IllegalArgumentException("The given number for coils reference size " +
                    "is not valid. The number should be at least 0 and at most "+ (UINT16_T_SIZE-1));
        this.coils = coils_reference;
        this.coilsSize = size;
    }

    public void setDiscreteInputsReference(boolean[] discreteInputs_reference, int size) {
        if(!isInRangeOfUint16_t(size))
            throw new IllegalArgumentException("The given number for discrete inputs reference size " +
                    "is not valid. The number should be at least 0 and at most "+ (UINT16_T_SIZE-1));
        this.discreteInputs = discreteInputs_reference;
        this.discreteInputsSize = size;
    }

    public void setHoldingRegistersReference(short[] holdingRegisters_reference, int size) {
        if(!isInRangeOfUint16_t(size))
            throw new IllegalArgumentException("The given number for holding registers reference size " +
                    "is not valid. The number should be at least 0 and at most "+ (UINT16_T_SIZE-1));
        this.holdingRegisters = holdingRegisters_reference;
        this.holdingRegsSize = size;
    }

    public void setInputRegistersReference(short[] inputRegisters_reference, int size) {
        if(!isInRangeOfUint16_t(size))
            throw new IllegalArgumentException("The given number for input registers reference size " +
                    "is not valid. The number should be at least 0 and at most "+ (UINT16_T_SIZE-1));
        this.inputRegisters = inputRegisters_reference;
        this.inputRegsSize = size;
    }

    private int createReadCoilsOrDiscreteInputsResponse(int startAddress, int quantity, FunctionCodes.MemoryType type) {
        boolean[] reference;
        byte functionCode;
        switch (type) {
            case COILS:
                reference = coils;
                functionCode = FunctionCodes.READ_COILS_FUNCTIONCODE;
                break;
            case DISCRETE_INPUTS:
                reference = discreteInputs;
                functionCode = FunctionCodes.READ_DISCRETE_INPUTS_FUNCTIONCODE;
                break;
            default: //Invalid Type
                return 0;
        }

        // INVALID_DATA_ADDRESS_EXCEPTION_CODE;
        if (isDataAddressWrong(startAddress, quantity, type)) {
            return 0;
        }
        message_size = 5; // Master id +function code + byte count + CRC

        byte byteCount = (byte) (quantity / 8);
        if (quantity % 8 != 0)
            byteCount++;

        message_size += byteCount;
        message = new byte[message_size];

        message[0] = FunctionCodes.MASTER_ID;
        message[1] = functionCode;
        message[2] = byteCount;
        int currentPlaceInMessage = 3;
        byte[] myBytes = new byte[byteCount];
        for (short i = 0; i < quantity; i++) {
            // Calculate the index of the coil status byte that contains the current coil
            short byteIndex = (short) (i / 8);

            // Calculate the bit position of the current coil within the current coil status byte
            byte bitPosition = (byte) (i % 8);

            // Set the bit in the coil status byte if the current coil is true
            if (reference[startAddress + i]) {
                myBytes[byteIndex] |= (1 << bitPosition);
            }
        }
        for (int j = 0; j < byteCount; j++) {
            message[currentPlaceInMessage + j] = myBytes[j];
        }


        return addCRC(message,0,message_size-2);
    }


    public int createReadCoilsResponse(int startAddress, int numberOfCoils){
        //INVALID_DATA_ADDRESS_EXCEPTION_CODE;
        if (isDataAddressWrong(startAddress, numberOfCoils, FunctionCodes.MemoryType.COILS)) {
            throw new ModbusException("Incorrect starting address or range",FunctionCodes.INVALID_DATA_ADDRESS_EXCEPTION_CODE);
        }
        return  createReadCoilsOrDiscreteInputsResponse(startAddress,numberOfCoils,FunctionCodes.MemoryType.COILS);
    }

    public int createReadDiscreteInputsResponse(int startAddress, int numberOfCoils){
        //INVALID_DATA_ADDRESS_EXCEPTION_CODE;
        if (isDataAddressWrong(startAddress, numberOfCoils, FunctionCodes.MemoryType.DISCRETE_INPUTS)) {
            throw new ModbusException("Incorrect starting address or range",FunctionCodes.INVALID_DATA_ADDRESS_EXCEPTION_CODE);
        }
        return  createReadCoilsOrDiscreteInputsResponse(startAddress,numberOfCoils,FunctionCodes.MemoryType.DISCRETE_INPUTS);
    }


    public int createWriteSingleRegisterResponse(int address, short value) {
        //INVALID_DATA_ADDRESS_EXCEPTION_CODE;
        if (isDataAddressWrong(address,0, FunctionCodes.MemoryType.HOLDING_REGISTERS)) {
            throw new ModbusException("Incorrect address",FunctionCodes.INVALID_DATA_ADDRESS_EXCEPTION_CODE);
        }
        holdingRegisters[address] = value;
        message_size = 8; //master ID+function code + address + value + CRC = 1+1+2+2+2
        message = new byte[message_size];
        message[0] = FunctionCodes.MASTER_ID;
        message[1] = FunctionCodes.WRITE_SINGLE_REGISTER_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte((short) address);
        message[3] = WordFunctions.getLSByte((short) address);
        message[4] = WordFunctions.getMSByte(value);
        message[5] = WordFunctions.getLSByte(value);

        return addCRC(message,0,message_size-2);
    }

    private int createReadHoldingOrInputRegistersResponse(int startAddress, int numOfRegisters, FunctionCodes.MemoryType type) {
        short[] reference;
        byte functionCode;
        switch (type) {
            case INPUT_REGISTERS:
                reference = inputRegisters;
                functionCode = FunctionCodes.READ_INPUT_REGISTERS_FUNCTIONCODE;
                break;
            case HOLDING_REGISTERS:
                reference = holdingRegisters;
                functionCode = FunctionCodes.READ_HOLDING_REGISTERS_FUNCTIONCODE;
                break;
            default: // Invalid Type
                return 0;
        }
        // INVALID_DATA_ADDRESS_EXCEPTION_CODE;
        if (isDataAddressWrong(startAddress, numOfRegisters, type)) {
            return 0;
        }
        message_size = 5; // master id + function code + byte count + CRC
        int byteCount = numOfRegisters * 2;
        message_size += byteCount;
        message = new byte[message_size];
        message[0] = FunctionCodes.MASTER_ID;
        message[1] = functionCode;
        message[2] = (byte) byteCount;

        int currentPlaceInMessage = 3;
        for (int i = startAddress; i < numOfRegisters + startAddress; i++) {
            message[currentPlaceInMessage] = WordFunctions.getMSByte(reference[i]);
            currentPlaceInMessage++;
            message[currentPlaceInMessage] = WordFunctions.getLSByte(reference[i]);
            currentPlaceInMessage++;
        }

        return addCRC(message,0,message_size - 2);
    }


    public int createReadInputRegistersResponse(int startAddress, int numOfRegisters) {
        if (isDataAddressWrong(startAddress, numOfRegisters, FunctionCodes.MemoryType.INPUT_REGISTERS)) {
            throw new ModbusException("Incorrect starting address or range",FunctionCodes.INVALID_DATA_ADDRESS_EXCEPTION_CODE);
        }

        return createReadHoldingOrInputRegistersResponse(startAddress,numOfRegisters,FunctionCodes.MemoryType.INPUT_REGISTERS);
    }

    public int createReadHoldingRegistersResponse(int startAddress, int numOfRegisters) {
        if (isDataAddressWrong(startAddress, numOfRegisters, FunctionCodes.MemoryType.HOLDING_REGISTERS)) {
            throw new ModbusException("Incorrect starting address or range",FunctionCodes.INVALID_DATA_ADDRESS_EXCEPTION_CODE);
        }

        return createReadHoldingOrInputRegistersResponse(startAddress,numOfRegisters,FunctionCodes.MemoryType.HOLDING_REGISTERS);
    }


    public int createWriteSingleCoilResponse(int address, boolean value) {
        if (isDataAddressWrong(address, 0, FunctionCodes.MemoryType.COILS)) {
            throw new ModbusException("Incorrect address",FunctionCodes.INVALID_DATA_ADDRESS_EXCEPTION_CODE);

        }
        message_size = 8; // master id + function code + address + value + CRC = 1+1+2+2+2=8
        message = new byte[message_size];
        message[0] = FunctionCodes.MASTER_ID;
        message[1] = FunctionCodes.WRITE_SINGLE_COIL_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte((short) address);
        message[3] = WordFunctions.getLSByte((short) address);
        if (value) {
            coils[address] = true;
            message[4] = (byte) 0xFF;
            message[5] = (byte) 0x00;
        } else {
            coils[address] = false;
            message[4] = (byte) 0x00;
            message[5] = (byte) 0x00;
        }
        return addCRC(message,0,message_size-2);
    }

    public int createWriteCoilsResponse(int startAddress, boolean[] values, int quantity) {
        if (isDataAddressWrong(startAddress, quantity, FunctionCodes.MemoryType.COILS)) {
            return 0;
        }
        // size is master id + function code + start address + quantitiy of written coils + CRC = 1+1+2+2+2=8
        message_size = 8;
        message = new byte[message_size];

        message[0] = FunctionCodes.MASTER_ID;
        message[1] = FunctionCodes.WRITE_MULTIPLE_COILS_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte((short)startAddress);
        message[3] = WordFunctions.getLSByte((short)startAddress);
        message[4] = WordFunctions.getMSByte((short)quantity);
        message[5] = WordFunctions.getLSByte((short)quantity);

        for (int i=0; i < quantity ;i++){
            coils[startAddress + i] = values[i];
        }

        return addCRC(message,0,message_size-2);
    }

        public int createWriteRegistersResponse(int startAddress, short[] values, int quantity)
    {
        if (isDataAddressWrong(startAddress, quantity, FunctionCodes.MemoryType.HOLDING_REGISTERS)) {
            throw new ModbusException("Incorrect address",FunctionCodes.INVALID_DATA_ADDRESS_EXCEPTION_CODE);
        }
        // size is master id + function code + start address + quantitiy of written coils + CRC = 1+1+2+2+2=8
        message_size = 8;
        message = new byte[message_size];

        message[0] = FunctionCodes.MASTER_ID;
        message[1] = FunctionCodes.WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte((short)startAddress);
        message[3] = WordFunctions.getLSByte((short)startAddress);
        message[4] = WordFunctions.getMSByte((short)quantity);
        message[5] = WordFunctions.getLSByte((short)quantity);

        for(int i=0;i<quantity;i++){
        holdingRegisters[startAddress + i]=values[i];
    }

        return addCRC(message,0,message_size-2);

    }

    public boolean isDataAddressWrong(int startAddress, int numberOfCoilsOrRegisters, FunctionCodes.MemoryType typeOfMemory) {
        if (!isInRangeOfUint16_t(startAddress) || !isInRangeOfUint16_t(numberOfCoilsOrRegisters)) return true;
        switch (typeOfMemory) {
            case COILS:
                return startAddress > coilsSize - 1 || startAddress < 0 || startAddress + numberOfCoilsOrRegisters > coilsSize;
            case DISCRETE_INPUTS:
                return startAddress > discreteInputsSize - 1 || startAddress < 0 || startAddress + numberOfCoilsOrRegisters > discreteInputsSize;
            case HOLDING_REGISTERS:
                return startAddress > holdingRegsSize - 1 || startAddress < 0 || startAddress + numberOfCoilsOrRegisters > holdingRegsSize;
            case INPUT_REGISTERS:
                return startAddress > inputRegsSize - 1 || startAddress < 0 || startAddress + numberOfCoilsOrRegisters > inputRegsSize;
            default:
                return true;
        }
    }

    public int createExceptionResponse(byte functionCode, byte exceptionCode) {
        // size is master id + function code for error + ExceptionCode = 1+1+1
        message_size = 3;
        message = new byte[message_size];

        message[0] = FunctionCodes.MASTER_ID;
        message[1] = (byte) (functionCode + FunctionCodes.EXCEPTION_OFFSET);
        message[2] = exceptionCode;
        return message_size;
    }


    private boolean isInRangeOfUint16_t(int value){
        if(value<0 || value>UINT16_T_SIZE-1) return false;
        return true;
    }

    public byte[] getMessage() {
        return message;
    }
}