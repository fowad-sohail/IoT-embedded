#include "ThingSpeak.h"
#include "secrets.h"
#include "ESP8266WiFi.h"
#include <Wire.h>
#include <SPI.h>

//Pull the secrets from that other file
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
WiFiClient  client;
ThingSpeakClass ts;


//This function takes an input of data in this format 123;456;7891 and then breaks it into sections 
//based on where the seperator charecter is (in this case ';'). Index is which piece of the string to you 
//want to return. 
//for instance:             123            ;          456               ;            7891
//for instance: [index = 0 to return this] ; [index = 1 to return this] ; [index = 2 to return this]
//If it doesnt find any semicolins in your data, it returns a blank string
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
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

//Initilizes the UART to 115200, says Hello to the user, and does some wifi initilization
void setup() {
  Serial.begin(115200);  // Initialize serial
  Serial.println("Hello! :)"); //Gotta say hi
  WiFi.mode(WIFI_STA);
  ts.begin(client);  // Initialize ThingSpeak
}

//We stay in this loop indefinitely
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

    //Arduino's Serial saves incoming data (From the MSP) in a buffer when you run
  //Serial.avaliable you check to see if there is anything in that buffer
  //once you read data from the buffer, the data you read is removed from the buffer
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
  
    //We just got sent data in the format [temperature] ; [humidity] ; [pressure]
    //We need to parse that into three seperate values. That is what getValue does
    String temp = getValue(output, ';', 0);
    String humid = getValue(output, ';', 1);
    String pressure = getValue(output, ';', 2);
    int t = ts.setField(1, temp); //Upload field 1
    if (t == 200) { // error code
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem setting Field 1. HTTP error code " + String(t));
    }
    
    int h = ts.setField(2, humid); //Upload field 2
    if (h == 200) { // error code
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem setting Field 1. HTTP error code " + String(h));
    }
    
    int p = ts.setField(3, pressure); //Upload field 3
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