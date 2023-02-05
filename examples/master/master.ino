#include "MiniModbus.h"
#include <WiFi.h>
#include <PubSubClient.h>

char* ssid = "mrez";
const char* password = "moes1379";
const char* mqttServer = "mbr.atro1.com";
const int mqttPort = 1883;
const char* mqttUser = "Gateway";
const char* mqttPassword = "0559";
 
WiFiClient espClient;
PubSubClient client(espClient);

int responseSize;
byte response[256];

boolean relay_status=false;

ModbusRequestCreator actuator(2);
ModbusRequestCreator controller(1);
void setup() {
  Serial2.begin(19200);
  Serial.begin(19200);
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    }
     
    Serial.println("Connected to the WiFi network");
     
    client.setServer(mqttServer, mqttPort);
     
    while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
     
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
     
    Serial.println("connected");
     
    } else {
     
    Serial.print("failed with state ");
    Serial.print(client.state());
    delay(2000);
     
    }
}

client.setCallback(callback);
client.subscribe("esp32/esp32req");
}
void loop() {
 client.loop();
  
  
  uint16_t messagesize= actuator.createReadInputRegistersRequest(0x0000,2);
    Serial.println (messagesize);

  Serial2.write(actuator.getMessage(),messagesize);
//  Serial.write(actuator.getMessage(),messagesize);
  

  delay(200);
  readResponse();
  delay(5000);
  messagesize= controller.createWriteSingleCoilRequest(0x0000,relay_status);
  Serial2.write(controller.getMessage(),messagesize);
//  Serial.write(actuator.getMessage(),messagesize);
  delay(200);
  readResponse();
  delay(5000);
}


void readResponse(){
    char temp_hum[80]={};
  responseSize=Serial2.available();
  if (responseSize>0){
    
    Serial2.readBytes(response,responseSize);
    ModbusRequestResponseParser parser(0,response);
    
    if (parser.getSlaveID()==0){
      if(parser.isItException()){
        Serial.println("An Error have been happend");
        Serial.print("Error is on this function code:");
        Serial.println(parser.getFunctionCode()-0x80);
        Serial.print("Error code is:");
        Serial.println(parser.getExceptionCode());
        
      }
      else if(parser.getFunctionCode() == READ_INPUT_REGISTERS_FUNCTIONCODE){  
        Serial.print("actuator's temp is");
        sprintf (temp_hum, "the temperture is :%s \n the humidity is :%s",String( parser.getInputRegistersValues(0)),String (parser.getInputRegistersValues(1)));
        client.publish("esp32/esp32test",temp_hum );
        Serial.println(parser.getInputRegistersValues(1));
      }
      else if (parser.getFunctionCode()==WRITE_SINGLE_COIL_FUNCTIONCODE){
        Serial.print("controller's coil is set to ");
        Serial.println(parser.getWriteSingleCoilValue());
      }
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  if (message[0]=='1'){
    relay_status=true;}
    else if(message[0]=='0'){
    relay_status=false;
   }
}
