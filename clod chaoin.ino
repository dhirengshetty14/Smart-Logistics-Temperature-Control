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
