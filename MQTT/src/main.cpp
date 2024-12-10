//-----------------------------
// Title: MQTT
//-----------------------------
// Program Details:
//-----------------------------
// Purpose: Conenct to broker.mqtt-dashboard.com, Publish and subscribe
// Dependencies: Make sure you have installed PubSubClient.h
// Compiler: PIO Version 1.72.0
// Atuhor: Originally an example called ESP8266-mqtt / slightly modified and cleaned up by Farid Farahmand
// OUTPUT: publishes 1,2,3,.... on outTopic every publishTimeInterval
// INPUT: Received value from the broker on inTopic  
// SETUP: To see the published values go to http://www.hivemq.com/demos/websocket-client/ 
//        subscribe to inTopic and outTopic. You can also create an APP using MQTT Dash
// Versions: 
//  v1: Nov-24-2022 - Cleaned up version 
//-----------------------------

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>



// WiFi variables
//REDACTED
//const char* ssid = "XXXXXXXXXXXX";
const char* ssid = "XXXXXXXXXXXXXXXX";  // Enter your WiFi name
//const char* password = "XXXXXXXXXXXXXX";  // Enter WiFi password
const char* password = "XXXXXXXXXXXXXXXX";

// MQTT variables
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* publishTopic = "testtopic/temp/outTopic/lymanj";   // outTopic where ESP publishes
const char* subscribeTopic = "testtopic/temp/inTopic/lymanj";  // inTopic where ESP has subscribed to
#define publishTimeInterval 10000 // in seconds 

// Definitions 
unsigned long lastMsg = 0;
unsigned long lastMeasure = 0;
#define MSG_BUFFER_SIZE	(50)
#define BUILTIN_LED 2 // built-in LED
#define BUTTON_EXT_PU D0
#define POTENTIOMETER A0
char msg[MSG_BUFFER_SIZE];
int value = 0;
int ledStatus = 0;
bool sendData = false;
WiFiClient espClient;
PubSubClient client(espClient); // define MQTTClient 


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
  //------------------------------------------

void callback(char* topic, byte* payload, int length) {
  Serial.print("Message arrived ["); // Received the incoming message
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  // the received value is in (char)payload[i]
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  // add your code here

  if (char(payload[0]) == '0')
  {
    Serial.println("Message Received: 0");
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (char(payload[0]) == '1')
  {
    Serial.println("Message Received: 1");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    Serial.print("Message Received: ");
    Serial.println(char(payload[0]));
  }

}
  //------------------------------------------

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "asdvjoiasdbjaESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 //------------------------------------------

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(BUTTON_EXT_PU, INPUT);
  pinMode(POTENTIOMETER, INPUT);
  Serial.begin(9600);
  setup_wifi();










  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}
//------------------------------------------
void loop() {
  unsigned long now = millis(); 
   if (!digitalRead(BUTTON_EXT_PU))
   {
     sendData = true;
   }
  if(WiFi.status() != WL_CONNECTED)
   {
     WiFi.disconnect();
     Serial.println("WiFi disconnected, Reconnecting.");
     setup_wifi();
   }

  if (!client.connected()) {
    //client.disconnect();
    reconnect(); // check for the latest value in inTopic 
  }
  client.loop();


  
//test analog read to see it break
//analogRead(POTENTIOMETER);

// Publish to outTopic 
  if ( sendData && now - lastMsg > publishTimeInterval) {
    lastMsg = now;
    float voltage = analogRead(POTENTIOMETER);
    voltage = 3.3 * voltage / float(1024);
    snprintf(msg, MSG_BUFFER_SIZE, "Voltage: %f", voltage); // prints Number # 1, Number # 2, .....
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publishTopic, msg);
    sendData = false;
  }

}