
#include "ESPWiFi.h"




void setup() {
  // put your setup code here, to run once:

  //set baud rate to 9600
  Serial.begin(9600);         

  //print whitespace before any data for readability
  Serial.println();
  //Serial.println(WiFi.macAddress());
  
  //set button to be input (external pull-up)
  pinMode(BUTTON_CAPTURE_EXT_PULL_UP, INPUT);  
  
  //set pin mode for sensor input
  pinMode(TEMPERATURE_HUMIDITY_SENSOR, INPUT);
  
  //set pin mode for built in LED
  pinMode(LED_1_PIN, OUTPUT);
  
  //LED is inverted logic, this ensures it is off
  digitalWrite(LED_1_PIN, HIGH);
  
  //wait a second 
  delay(1000);

  //set the time zone with input from Serial
  setTimeZone();

  //connect to the specified WiFi AP 
  //as defined in the header
  connectToWiFi();

  


}

void loop() {
  // put your main code here, to run repeatedly:
  //if button pressed
   if (digitalRead(BUTTON_CAPTURE_EXT_PULL_UP) == LOW)
   {
     double temperature = captureTemp();
     double humidity = captureHumidity();
     String timestamp = MygetTime();
     uploadData(timestamp, temperature, humidity, 0);
     
   }


}
