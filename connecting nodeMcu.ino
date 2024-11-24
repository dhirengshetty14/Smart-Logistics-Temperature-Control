//Connecting NodeMcu to WiFi and sending data to the Cloud
//This code first connects NodeMcu to WiFi and then sends the data to the ThingSpeak page. At https://thingspeak.com/ we can create a Channel that will have its own fields, graphs //and API keys so we can do our own updates. So, in the code we need to add the WiFi name and code as well as the Channel ID and the Write API Key of ThingSpeak. Finally, we upload //the code to NodeMcu by selecting "NodeMCU 1.0 (ESP-12E Module)" as a board tool.

#include <ESP8266WiFi.h>
#include <dht.h>
#include <ThingSpeak.h>

dht DHT;
#define DHT11_PIN D4
/* creating object for connecting on WiFi and variables for Channel ID and Write API Key in ThingSpeak */
WiFiClient client;

long myChannelNumber = yourChannelID;
const char myWriteAPIKey[] = "yourAPIKey";

void setup() {
  Serial.begin(9600);
  //connect on WiFi
  WiFi.begin("Your SSID", "Your Pass");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("..");
  }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());
  //Start ThingSpeak with WiFi credentials
  ThingSpeak.begin(client);
}

void loop() {
  int chk = DHT.read11(DHT11_PIN);
  float h = DHT.humidity;
  float t = DHT.temperature;
  Serial.println("Temperature: " + (String) t);
  Serial.println("Humidity: " + (String) h);
  /*sending temperature and humidity values to field1 and field2 of ThingSpeak*/
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(20000);
}