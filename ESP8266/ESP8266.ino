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

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "-1";
}

void setup() {
  Serial.begin(9600);  // Initialize serial
  Serial.println("Hello! :)");
  WiFi.mode(WIFI_STA);
  ts.begin(client);  // Initialize ThingSpeak

}

void loop() {

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  int incomingByte = 0; // incoming byte from serial input
  char c;
  String output = "";

  while (Serial.available() > 0) 
  {
    // read the incoming byte:
    incomingByte = Serial.read();

    // type cast to a character from the ASCII byte received from Serial
    
    c = (char) incomingByte;
    Serial.print(c);
    output += c; // concatenate each character before sending to ThingSpeak

    // at this point, output is all the data with the delimiting characters in between
    // we need to split up output to get the three numbers: temperature, humidity, pressure
  }

  //output = "123;456;789";
  Serial.println("ENTIRE STRING FROM SERIAL INPUT: ");
  Serial.println(output);

  Serial.println("-----------------------");

    
    String temp = getValue(output, ';', 0);
    String humid = getValue(output, ';', 1);
    String pressure = getValue(output, ';', 2);
    int t = ts.setField(1, temp);
    if (t == 200) { // error code
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem setting Field 1. HTTP error code " + String(t));
    }
    
    int h = ts.setField(2, humid);
    if (h == 200) { // error code
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem setting Field 1. HTTP error code " + String(h));
    }
    
    int p = ts.setField(3, pressure);
     if (p == 200) { // error code
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem setting Field 1. HTTP error code " + String(p));
    }

    //For uploading to thinkspeak
  int x = ts.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  //ESP.deepSleep(20e6);
  delay(15000); // Wait 10 seconds to update the channel again
}
