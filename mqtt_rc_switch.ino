#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <RCSwitch.h>
#include <Ticker.h>
 
// WIFI Settings
const char* ssid = "Your SSID";
const char* password = "Your Password";
const char* mqtt_server = "192.168.0.100";

// MQTT Main Topic
const char* topic = "lamp";

// Settings for RF
const int rfProtocol = 1;
const int rfPulseLength = 180;
const int rfRepeatTx = 15;

// Make sure you're using the correct pin
// see http://www.instructables.com/id/Programming-ESP8266-ESP-12E-NodeMCU-Using-Arduino-/
const byte txPin = 0; // D3
const byte rxPin = 4; // D2

const int numberOfLights = 5;
bool lightStatus[] = {false, false, false, false, false };

const int txOn[] = {
    4199731, 
    4199875, 
    4200195, 
    4201731, 
    4207875
  };
const int txOff[] = {
    4199740, 
    4199884, 
    4200204, 
    4201740, 
    4207884
  };

//// You don't need to edit anything below this line ////

bool changesAvailable = false;
 
WiFiClient espClient;
PubSubClient client(espClient);
RCSwitch txSwitch = RCSwitch();
RCSwitch rxSwitch = RCSwitch();
Ticker timer;
  
void callback(char* topic, byte* payload, unsigned int length) {
 // Debug info
// Serial.print("Message arrived [");
// Serial.print(topic);
// Serial.print("], ");
//
// Serial.print("Payload: ");
// for (int i=0;i<length;i++) {
//  char receivedChar = (char)payload[i];
//  Serial.print(receivedChar);
// }

 // calculating the outlet number, based on the last char of the topic
 int topicLength = strlen(topic);
 int lastDigitPos = topicLength - 1;
 int lightID = topic[lastDigitPos] - '0';
// Serial.print(", lightNumber: ");
// Serial.print(lightNumber);
// Serial.println();

 if(strstr(topic, "get")) {
  // not sure how to handle get requests yet
 } else if (strstr(topic, "set")) {
  Serial.print("Set light ");
  Serial.print(lightID);
  Serial.print(" to ");

  int command;

  for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    if (receivedChar == '0') {
      command = txOff[lightID];
      lightStatus[lightID] = false;
    }
    if (receivedChar == '1') {
      command = txOn[lightID];
      lightStatus[lightID] = true;
    }
  }
  txSwitch.send(command, 24);
  Serial.print(lightStatus[lightID]);
  Serial.println();
  changesAvailable = true;
 } else {
  // error, incompatible message found
 }
  Serial.println();
}


void publishStatus() {
  if (changesAvailable) {
    for (int i=0;i < numberOfLights;i++) {
      
      String message = (String)topic + "/status/" + (i);
      // converting int to char
      char statusString = lightStatus[i] + '0';
  
      char publishMessage[message.length()+1];
      // converting string to char*
      message.toCharArray(publishMessage, 65);

      Serial.println("Publishing status:");
      Serial.print(publishMessage);
      Serial.print(": ");
      Serial.println(&statusString);
      client.publish(publishMessage, &statusString); 
    }
    changesAvailable = false;
  }
}
 
 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP8266 Client")) {
  Serial.println("connected");
  // ... and subscribe to topic
  String fullTopic = String(topic) + "/#";
  client.subscribe(string2char(fullTopic));
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}
 
void setup()
{
 Serial.begin(115200);

 setup_wifi();
 
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);

 rxSwitch.enableReceive(rxPin);

 txSwitch.enableTransmit(txPin);
 txSwitch.setProtocol(rfProtocol);
 txSwitch.setPulseLength(rfPulseLength);
 txSwitch.setRepeatTransmit(rfRepeatTx);

 timer.attach(15, publishStatus);
}

void setup_wifi() {
 
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
void loop()
{
  if (rxSwitch.available()) {
    int value = rxSwitch.getReceivedValue();
    if (value != 0) {
      // looping through the off and on commands to determine what command we just received
      for (int i=0;i < numberOfLights;i++) {
        if(txOff[i] == value) {
          Serial.print("Switching off: ");
          Serial.println(i);
          lightStatus[i] = false;
        }
        if(txOn[i] == value) {
          Serial.print("Switching on: ");
          Serial.println(i);
          lightStatus[i] = true;
        }
      }
      // update immediately. Could use debounce (might get called multiple times when button remains pressed)
      changesAvailable = true;
      publishStatus();
    }
    rxSwitch.resetAvailable();
  }
 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// https://coderwall.com/p/zfmwsg/arduino-string-to-char
// by Wenson Smith
char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}
