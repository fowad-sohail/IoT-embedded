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
  Serial.begin(9600);  // Initialize serial

  WiFi.mode(WIFI_STA); 
  ts.begin(client);  // Initialize ThingSpeak

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

  int incomingByte = 0; // incoming byte from serial input
  char c;
  String output = "";
  int temperature;
  int humidity;
  int pressure;
  
  while (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // type cast to a character from the ASCII byte received from Serial
    c = (char) incomingByte;
    output += c; // concatenate each character before sending to ThingSpeak

    // at this point, output is all the data with the delimiting characters in between
    // we need to split up output to get the three numbers: temperature, humidity, pressure
  }
  
    if(output.charAt(0) === 'T') { // character for Temp
      // get the temperature data
      temperature = output.substring(1,3);
    }
    // delete the temperature data
    output = output.substring(4, output.length());
    
    if(output.charAt(0) === 'H') { // character for Hum
      humidity = output.substring(1,3);
    }
    // delete the humidity data
    output = output.substring(4, output.length());
    
    if(output.charAt(0) === 'P') { // character for Press
      pressure = output.substring(1, output.length());
    }

  Serial.print("DATA SENT TO THINGSPEAK: ");
  Serial.println(output);



  
  // Write to ThingSpeak.

  int x = ts.setField(1, temperature); // SEND TEMPERATURE
  if(x == 200){ // error code
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem setting Field 1. HTTP error code " + String(x));
  }


  int x = ts.setField(2, humidity); // SEND HUMIDITY
  if(x == 200){ // error code
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem setting Field 1. HTTP error code " + String(x));
  }
  
  int x = ts.setField(3, pressure); // SEND PRESSURE
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
  delay(15000); // Wait 10 seconds to update the channel again
}
