//Handling the room temperature by sending IR signal to turn on/off the air-conditioner

//We set specific temperature values and check the ambient conditions through the DHT temperature and humidity sensor. We have set as desired room temperature the 20 ° C and the //signal is sent per minute. If the conditions are not what we need, the Arduino board via IR Transmitter sends the appropriate signal either to turn on or off the air //conditioning.

//The libraries we need for handling IR singals and DHT11 sensor
#include <IRremote.hpp>
#include <dht.h>

//defining the Arduino pin where we connect DHT11 sensor
#define dht_apin A0
//defining the Arduino pin where we connect IR Transmitter
#define IRledPin 13
#define NumIRsignals 200

dht DHT;

int IRsignalON[] = {

//put your own signal here
/*
456, 454,
58, 164,
58, 52,
58, 166,
56, 164,
58, 54,
58, 52,
58, 164,
58, 54,
58, 52,
58, 164,
58, 54,
56, 54,
58, 164,
58, 164,
58, 54,
56, 166,
58, 164,
58, 52,
58, 164,
58, 164,
58, 164,
58, 164,
58, 164,
58, 164,
58, 54,
58, 164,
58, 52,
58, 54,
58, 52,
58, 54,
56, 54,
58, 52,
58, 166,
56, 54,
58, 164,
58, 164,
58, 164,
58, 164,
58, 54,
56, 54,
58, 54,
56, 164,
58, 54,
58, 52,
58, 54,
58, 52,
58, 164,
58, 164,
58, 536,
456, 456,
56, 166,
56, 54,
58, 164,
58, 164,
58, 52,
58, 54,
58, 164,
58, 52,
58, 54,
56, 166,
58, 52,
58, 54,
56, 166,
58, 164,
58, 52,
58, 164,
58, 164,
58, 54,
58, 164,
58, 164,
58, 164,
58, 164,
58, 164,
58, 164,
58, 54,
56, 164,
58, 54,
58, 52,
58, 54,
58, 52,
58, 54,
58, 52,
58, 164,
58, 54,
56, 166,
56, 166,
56, 166,
56, 166,
56, 54,
58, 52,
58, 54,
58, 164,
58, 52,
58, 54,
58, 52,
58, 54,
56, 166,
58, 164,
58, 0
*/
};

int IRsignalOFF[] = {

//put your own signal here
/*
458, 454,
58, 164,
58, 52,
58, 164,
58, 164,
58, 54,
58, 52,
58, 164,
58, 54,
56, 54,
58, 164,
58, 54,
56, 54,
58, 164,
58, 164,
58, 52,
58, 164,
58, 54,
58, 164,
58, 164,
58, 164,
58, 164,
58, 54,
56, 166,
56, 166,
58, 164,
58, 52,
58, 54,
58, 52,
58, 54,
56, 166,
58, 52,
58, 54,
56, 166,
56, 166,
56, 166,
56, 54,
58, 54,
56, 54,
58, 52,
58, 54,
58, 52,
58, 54,
58, 52,
58, 164,
58, 164,
58, 164,
58, 164,
58, 164,
58, 536,
456, 454,
58, 164,
58, 54,
58, 164,
58, 164,
58, 52,
58, 54,
58, 164,
58, 52,
58, 54,
58, 164,
58, 52,
58, 54,
58, 164,
58, 164,
58, 52,
58, 164,
58, 54,
58, 164,
56, 166,
56, 166,
58, 164,
58, 52,
58, 164,
58, 164,
58, 164,
58, 54,
58, 52,
58, 54,
58, 52,
58, 164,
58, 54,
56, 54,
58, 164,
58, 164,
58, 164,
58, 52,
58, 54,
58, 52,
58, 54,
58, 52,
58, 54,
58, 52,
58, 54,
58, 164,
56, 166,
58, 164,
58, 164,
58, 164,
58, 0
*/
};

void setup() {
  //Confirming that IR is off in the beginning
  digitalWrite(IRledPin, LOW); 
  pinMode(IRledPin, OUTPUT); 
  Serial.begin(9600);        
}

void loop() {
    //reading from sensor
    DHT.read11(dht_apin);
    //printing values from sensor
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.print("C ~ ");
    Serial.print(DHT.temperature*1.8 + 32);
    Serial.println("F");
    /*If temperature is less than 19°C --> send IR signal for turning on the air-conditioner*/
    if(DHT.temperature < 19)
    {
      Serial.println("Temperature low --> AC powered on");
      for (int i = 0; i < NumIRsignals; i+=2) 
      {
        //send IR signal of turning on the air-conditioner
        pulseIR(IRsignalON[i]*10);
        //turning off the IR Transmitter
        delayMicroseconds(IRsignalON[i+1]*10);  
      }
    }
    
    /*If temperature is more than 19°C --> send IR signal for turning off the air-conditioner*/
    if(DHT.temperature > 19)
    {
      Serial.println("Temperature high --> AC powered off");
      for (int i = 0; i < NumIRsignals; i+=2) 
      {
        //send IR signal of turning off the air-conditioner
        pulseIR(IRsignalOFF[i]*10);  
        //turning off the IR Transmitter
        delayMicroseconds(IRsignalOFF[i+1]*10); 
      }
    }
    //wait for 60 secs and repeat
    delay(60000);
}

//method for sending the IR signal
void pulseIR(long microsecs) {
  
  //turns off any background interrupts
  cli();  
 
  while (microsecs > 0) {
    
   digitalWrite(IRledPin, HIGH); 
   delayMicroseconds(10);         
   digitalWrite(IRledPin, LOW);   
   delayMicroseconds(10);         
 
   microsecs -= 26;
  }
  //turns on background interrupts
  sei();  
}