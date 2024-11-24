


#include <IRremote.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2); 


int RECV_PIN = 6;
const int led=13;

IRrecv irrecv(RECV_PIN);
decode_results results;

#include "DHT.h"
#define DHTPIN 2 
#define DHTTYPE DHT11  

DHT dht(DHTPIN, DHTTYPE);

void setup() 
{
  pinMode(led,OUTPUT);
  
  lcd.backlight();
  lcd.init();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Monitoring Systm");
  lcd.setCursor(0,1);
  lcd.print(" for Cold-Chain ");
  delay(3000);
  
  Serial.begin(9600);
  irrecv.enableIRIn();

  dht.begin();
  delay(1000);
}

//1FE50AF --1
//1FED827 --2

void loop() 
{
  if (irrecv.decode(&results)) 
  {
    Serial.println(results.value, HEX);
    delay(100);
     if(results.value==0x1FE50AF)
     {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("LED ON");
      digitalWrite(led, HIGH);
      delay(1000);        
     }
     
     else if(results.value==0x1FED827)
     {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("LED OFF");
      digitalWrite(led, LOW);
      delay(1000);
      }
      
     else
     {
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float f = dht.readTemperature(true);
      delay(100);
  
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
        //Serial.println(F("Failed to read from DHT sensor!"));
        goto ksn;
        }
  
      //Serial.println("Temperature: " + (String) t);
      //Serial.println("Humidity: " + (String) h);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("TEMP: ");lcd.print(t);
      lcd.setCursor(0,1);
      lcd.print("Humi: ");lcd.print(h);
      delay(100);
    } 
    ksn:
    irrecv.resume();
  }

}  
