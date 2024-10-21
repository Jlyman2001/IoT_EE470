
#include "ESPWiFi.h"




void setup() {
  // put your setup code here, to run once:

  //set baud rate to 9600
  Serial.begin(9600);             
  Serial.println();
  Serial.println(WiFi.macAddress());
  //set button to be input (external pull-up)
  pinMode(BUTTON_CAPTURE_EXT_PULL_UP, INPUT);  
  pinMode(TEMPERATURE_HUMIDITY_SENSOR, INPUT);
  pinMode(LED_1_PIN, OUTPUT);
  //LED is inverted logic, this turns it off
  digitalWrite(LED_1_PIN, HIGH);
  
  delay(1000);

  setTimeZone();
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
