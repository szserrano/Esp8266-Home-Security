/*********
  Modified by Sydney Serrano
  Original authored by Rui Santos
  Complete project details at https://RandomNerdTutorials.com/door-status-monitor-using-the-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <ESP8266WiFi.h>

// Set GPIOs for LED and reedswitch
const int reedSwitch1 = 5;
const int reedSwitch2 = 4;
const int led = 2; //optional

// which door
String whichDoor = "";

// Detects whenever the door changed state
bool changeState1 = false;
bool changeState2 = false;

// Holds reedswitch state (1=opened, 0=close)
bool state1;
bool state2;
char* doorState1;
char* doorState2;

// holds values for POST req
char* doorState;
char* door;

// Auxiliary variables (it will only detect changes that are 60000 milliseconds apart)
unsigned long previousMillis = 0; 
const long interval = 60000; // 60000 milliseconds (1 minute)

const char* ssid = "me_irl"; //iPhone Hotspot
const char* password = "whyphy22"; //iPhone Hotspot pass
const char* host = "webhook.site";
const char* apiKey = "007347fc-f34d-4286-88d8-a10bbb8b2292";

// Runs whenever the reedswitch changes state
ICACHE_RAM_ATTR void changeDoorStatus1() {
  Serial.println("State1 changed");
  changeState1 = true;
}

// Runs whenever the reedswitch changes state
ICACHE_RAM_ATTR void changeDoorStatus2() {
  Serial.println("State2 changed");
  changeState2 = true;
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Read the current door state
  pinMode(reedSwitch1, INPUT_PULLUP);
  state1 = digitalRead(reedSwitch1);

  pinMode(reedSwitch2, INPUT_PULLUP);
  state2 = digitalRead(reedSwitch2);

  // Set LED state to match door state
  //pinMode(led, OUTPUT);
  //digitalWrite(led, state);
  
  // Set the reedswitch pin as interrupt, assign interrupt function and set CHANGE mode
  attachInterrupt(digitalPinToInterrupt(reedSwitch1), changeDoorStatus1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(reedSwitch2), changeDoorStatus2, CHANGE);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected"); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if (changeState1 || changeState2){
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= interval) {
      Serial.print("Difference in time: ");
      Serial.println(currentMillis - previousMillis);
      previousMillis = currentMillis;
      // If a state change has occured, invert the current door state
      if(changeState1){
        state1 = !state1;
        if(state1) { // If state == 1, the door is closed
          doorState1 = "closed";
        }
        else{ // Otherwise, the door is open
          doorState1 = "open";
        }    
        changeState1 = false;
        Serial.print("state1: ");
        Serial.println(state1);
        Serial.println(doorState1);
        whichDoor = "front door";
      } else {
        state2 = !state2;
        if(state2) { // If state == 1, the door is closed
          doorState2 = "closed";
        }
        else{ // Otherwise, the door is open
          doorState2 = "open";
        }
        changeState2 = false;
        Serial.print("state2: ");
        Serial.println(state2);
        Serial.println(doorState2);
        whichDoor = "Altoid can";
      }
      //digitalWrite(led, state);
        
      //Update google sheet
      Serial.print("connecting to ");
      Serial.println(host);
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
    
      String url = "/";
      url += apiKey;
          
      Serial.print("Requesting URL: ");
      Serial.println(url);
      
      if(whichDoor == "front door"){
        door = "front door";
        doorState = doorState1;
      } else if (whichDoor == "Altoid can"){
        door = "Altoid can"; 
        doorState = doorState2;
      }

      int length = 25 + strlen(door) + strlen(doorState); // 33 is number of extra characters in body of json payload not including doorState and door lengths
      if(doorState == "closed"){
        Serial.print(String("POST ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" + 
                          "Content-Type: application/json\r\n" + 
                          "Content-Length: " + String(length) + "\r\n\r\n" +
                          "{\"value1\":\"" + doorState + "\",\"value2\":\"" + door + "\"}" + "\r\n");
        client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" + 
                          "Content-Type: application/json\r\n" + 
                          "Content-Length: " + String(length) + "\r\n\r\n" +
                          "{\"value1\":\"" + doorState + "\",\"value2\":\"" + door + "\"}" + "\r\n");
      } else {
        Serial.print(String("POST ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" + 
                          "Content-Type: application/json\r\n" + 
                          "Content-Length: " + String(length) + "\r\n\r\n" +
                          "{\"value1\":\"" + doorState + "\",\"value2\":\"" + door + "\"}" + "\r\n");
        client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" + 
                          "Content-Type: application/json\r\n" + 
                          "Content-Length: " + String(length) + "\r\n\r\n" +
                          "{\"value1\":\"" + doorState + "\",\"value2\":\"" + door + "\"}" + "\r\n");
      }
    }
  }  
  else {
    changeState1 = true; // Testing if it works
  }
}
