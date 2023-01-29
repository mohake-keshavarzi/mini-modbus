#ifndef WORD_FUNCTIONS_H
#define WORD_FUNCTIONS_H

class WordFunctions
{
private:
    /* data */
public:
    WordFunctions(){}
    byte getLSByte(word inWord){return  (byte)(inWord & 0x00FF);}
    byte getMSByte(word inWord){return (byte)((inWord & 0xFF00) >>8);}
    word MSBLSBJoin(byte MSB, byte LSB)
    {
        word wMSB= (0x0000 | MSB)<<8;
        wMSB = wMSB & 0xFF00;
        word wLSB= 0x0000 | LSB;
        return wMSB | wLSB;
    }

};




#endif