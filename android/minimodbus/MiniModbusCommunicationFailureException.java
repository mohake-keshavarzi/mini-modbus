package ir.m.h.keshavarzi.minimodbus;


import java.util.ArrayList;

public class MiniModbusCommunicationFailureException extends Exception{

    MiniModbusCommunicationFailureException(int numOfRetries){
        super("Stable Communication failed After: "+numOfRetries+" Retries");
    }


}
