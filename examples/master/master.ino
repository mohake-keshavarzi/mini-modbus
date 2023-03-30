#include "C:\Users\AEA\AppData\Local\Arduino15\libraries\MiniModbus\examples\Configs.h"
#include "MiniModbus.h"
#define CONTROLLER_ID 0x01
#define ACTUATOR_ID 0x02

Master miniModbusMaster{Serial2,PARSER_DIGITALS_BUFFER_SIZE,PARSER_REGISTERS_BUFFER_SIZE};
boolean taskIsDone{false};

void setupTimer1Interrupt(unsigned long delayTime) {
  cli(); // Disable global interrupts

  TCCR1A = 0; // Clear Timer1 control registers
  TCCR1B = 0;

  // Set Timer1 to CTC (Clear Timer on Compare Match) mode
  TCCR1B |= (1 << WGM12);

  // Set Timer1 prescaler to 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // Calculate value for Timer1 Compare Match register based on delay time
  unsigned long compareMatchValue = (delayTime * 1000) / (1000000 / (F_CPU / 1024));
  OCR1A = compareMatchValue;

  // Enable Timer1 Compare Match interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); // Enable global interrupts
}
// Timer1 interrupt service routine (ISR)
ISR(TIMER1_COMPA_vect) {
  // This code will run when the Timer1 interrupt is triggered
  digitalWrite(13,!digitalRead(13));

}

// int working{0};
void setup() {
    Serial2.begin(19200);
    Serial.begin(19200);
    pinMode(13, OUTPUT);
    digitalWrite(13,false);
//    pinMode(20, INPUT_PULLUP);
    setupTimer1Interrupt(500);
    
}
void loop() {
    int quantity=9;
  
    if(miniModbusMaster.readDiscreteInputs(CONTROLLER_ID,0x0005,quantity)){
        // digitalWrite(13,true);
        Serial.println("Seccessful");
        Serial.print("DATA:");
        for(int i{0};i<quantity;i++){
          Serial.print("  ");
          Serial.print(miniModbusMaster.getDigitalsBuffer()[i]);    
        }
        Serial.println();
    }
    else{
        // digitalWrite(13,false);
        Serial.print("Not seccessful: ");
        Serial.println(miniModbusMaster.getExceptionCode());
    }
    printInHex(miniModbusMaster.getResponseMessage(),20);
    delay(500);

    // Serial.println(working++);
    
}

void printInHex(byte* message ,int length){
    for (int i = 0; i < length; i++) {
        Serial.print(message[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}
