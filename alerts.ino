//Sending email alerts in case of wrong temperature
//We will add the function of sending email in case the temperature is different than 20 ° C. To perform this function we will create a new gmail account which will be the sender. //So, in case we have a different value than 20 ° C in the loop we have created, an email will be sent from the new email alert account through the NodeMCU.

#include <ESP_Mail_Client.h>
#include <ESP_Mail_FS.h>
#include <SDK_Version_Common.h>

#include <ESP8266WiFi.h>
#include <dht.h>
#include <ThingSpeak.h>
//Defining the WiFi credentials
#define WIFI_SSID "Your SSID"
#define WIFI_PASSWORD "Your pass"
//Defining the host we will use for the emails, I use gmail
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials of the new created email */
#define AUTHOR_EMAIL "your new gmail account"
#define AUTHOR_PASSWORD "your new password"

/* Defining recipient's email*/
#define RECIPIENT_EMAIL "your preferred recipient"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Declare the message class */
 SMTP_Message message;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

dht DHT;
#define DHT11_PIN D4

WiFiClient client;

long myChannelNumber = yourChannelID;
const char myWriteAPIKey[] = "yourWriteAPIKey";

void setup() {
  
  Serial.begin(9600);
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("..");
  }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());
  //DHT.begin();
  ThingSpeak.begin(client);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the email session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Set the message headers */
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Alert Email";
  message.addRecipient("Vasileios", RECIPIENT_EMAIL);

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;
}

void loop() {
  
  int chk = DHT.read11(DHT11_PIN);
  float h = DHT.humidity;
  float t = DHT.temperature;
    
  if(t!=20){
    /*Defining the HTML message of the email*/
    String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP board: Temperature needs caution:!</p></div>" + (String)t + "° C";
    message.html.content = htmlMsg.c_str();
    message.html.content = htmlMsg.c_str();
    message.text.charSet = "us-ascii";
    message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

    /* Start sending Email and close the session */
    if (!MailClient.sendMail(&smtp, &message))
      Serial.println("Error sending Email, " + smtp.errorReason());
  }
  
  Serial.println("Temperature: " + (String) t);
  Serial.println("Humidity: " + (String) h);
  
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

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);
      //printing info regarding the email sent
      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}