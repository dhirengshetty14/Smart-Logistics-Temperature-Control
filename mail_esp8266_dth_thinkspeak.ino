
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#include <ESP_Mail_Client.h>
#include "DHT.h"
#include <ThingSpeak.h>

#define WIFI_SSID "esp32cam"
#define WIFI_PASSWORD "12345678"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "coldchainecs@gmail.com"
#define AUTHOR_PASSWORD "jhcyzrmskvcslyak"

/* Recipient's email*/
#define RECIPIENT_EMAIL "allisadhvik@gmail.com"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Declare the message class */
SMTP_Message message;

/* Declare the session config data */
ESP_Mail_Session session;
  
/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);
void setTextMsg(void);
void sendTextMsg(void);

#define DHTPIN D4 //D2
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;
long myChannelNumber = 1916130;
const char myWriteAPIKey[] = "8HR1WBAS6OOL4M3C";

float t;
int h;
bool trigger_send = true;
float temp_threshold_above = 33.0;
float temp_threshold_below = 31.0;
String textMsg;


void setup()
{
 
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";


  /* Set the message headers */
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Monitoring System for Cold-Chain";
  message.addRecipient("KSN", RECIPIENT_EMAIL);

  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;
  delay(2000);
  /*Send HTML message
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP board</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit; */

  /*
  //Send raw text message 
  textMsg = "Hello World! - Sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;
    */
    
  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config 
  if (!smtp.connect(&session))
    return;       */

  /* Start sending Email and close the session 
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());  */
ThingSpeak.begin(client);
delay(1000);

dht.begin();
delay(1000);
}


void loop()
{
  h = dht.readHumidity();
  t = dht.readTemperature();
  delay(1000);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("Temperature: " + (String) t); Serial.println(" *C");
  Serial.print("Humidity: " + (String) h);Serial.println(" %");

  if(t > temp_threshold_above && trigger_send == true)
  {
    textMsg = textMsg + "Temperature above threshold value: " + String(temp_threshold_above) + " *C" + "\n";
    Serial.print(textMsg);
    Serial.println("Send DTH11 Sensor data vai email");
    setTextMsg();
    sendTextMsg();
    textMsg = "";
    trigger_send == false;
    }
    
  if(t < temp_threshold_below && trigger_send == false)
  {
    textMsg = textMsg + "Temperature below threshold value: " + String(temp_threshold_below) + " *C" + "\n";
    Serial.print(textMsg);
    Serial.println("Send DTH11 Sensor data vai email");
    setTextMsg();
    sendTextMsg();
    textMsg = "";
    trigger_send == true;
    }
 

  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(15000);
 
}/***************MAIN END *********************/

void setTextMsg()
{
  textMsg = textMsg + "Temperature: " + String(t) + " *C" + "\n" + "Humidity: " + String(h) + " %";
  message.text.content = textMsg.c_str();
  }


void sendTextMsg()
{
  
  //message.addHeader("Message-ID:coldchainecs@gmail.com");
  /* Connect to server with the session config */
  if (!smtp.connect(&session))  return;       

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message)) Serial.println("Error sending Email, " + smtp.errorReason());  
  }


   
/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");
  }
}
