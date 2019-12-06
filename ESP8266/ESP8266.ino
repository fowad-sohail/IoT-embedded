#include "ThingSpeak.h"
#include "secrets.h"
#include "ESP8266WiFi.h"
#include <Wire.h>
#include <SPI.h>

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
WiFiClient  client;
ThingSpeakClass ts;

void setup() {
  Serial.begin(115200);  // Initialize serial

  WiFi.mode(WIFI_STA); 
  ts.begin(client);  // Initialize ThingSpeak

  Serial.println(F("BME280 test"));
 
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }


  // hardcoded input for testing
  int input = 37;
  
  // Write to ThingSpeak.

  int x = ts.setField(1, input);
  if(x == 200){ // error code
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem setting Field 1. HTTP error code " + String(x));
  }

  x = ts.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  //ESP.deepSleep(20e6);
  delay(15000); // Wait 20 seconds to update the channel again
}
