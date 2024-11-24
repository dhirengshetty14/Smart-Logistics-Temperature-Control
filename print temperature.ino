//Printing Temperature and Humidity
//Printing the room Temperature and Humidity values from DHT11 sensor in the Arduino IDE serial monitor.

// The library we need for sensor DHT11
#include <dht.h>

#define dht_apin A0 // The Arduino analog pin in which the DHT11 Sensor is connected to
  
dht DHT; //The object that will handle the values of temperature and humidity
 
void setup(){
 
  Serial.begin(9600);
  delay(500); //Delay 0.5 sec to let system boot
  Serial.println("DHT11 Humidity & temperature Sensor\n\n");
  delay(1000);//Wait 1 sec before accessing Sensor
 
}
 
void loop(){
    //Start of Program 
 
    DHT.read11(dht_apin); //read from the A0 analog pin of Arduino
    
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.print("C and ");
    //We convert the temperature to fahrenheit
    Serial.print(DHT.temperature*1.8 + 32);
    Serial.println("F");
      
    delay(2000);//Wait 2 seconds before accessing the sensor again.
}