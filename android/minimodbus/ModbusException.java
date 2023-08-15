package ir.m.h.keshavarzi.minimodbus;

public class ModbusException extends RuntimeException{
    private final byte exceptionCode;
    public ModbusException(String message,byte exceptionCode){
        super(message);
        this.exceptionCode=exceptionCode;
    }
    public int getExceptionCode(){
        return exceptionCode;
    }
}
