#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>
#include <string.h>
#include <string>
#include <ArduinoJson.h>

#define LED_1_PIN LED_BUILTIN
#define Fetch_Settings_Button_EXT_PU D0
#define Red_LED_Output D1
#define Green_LED_Output D2
#define Blue_LED_Output D3
#define Single_LED_Output D4


int Red_Duty_Cycle = 0;
int Green_Duty_Cycle = 0;
int Blue_Duty_Cycle = 0;
bool LED_Status = false;

unsigned long currentMillis;
unsigned long startMillis;

// five minutes in ms
const unsigned long period = 300000;

// WiFi settings
const char *ssid = "Lyman Family";
const char *password = "7072178282";
const String PATH_TO_SETTINGS_FILE = "https://josh-lyman-ssu.com/RGB-LED-Assignment/results.txt";

// put function declarations here:
void FetchLEDSettings(String URL);
void connectToWiFi();
void writeToLEDs();
void sendMessageToSlack();

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);

  pinMode(Fetch_Settings_Button_EXT_PU, INPUT);
  pinMode(Red_LED_Output, OUTPUT);
  pinMode(Green_LED_Output, OUTPUT);
  pinMode(Blue_LED_Output, OUTPUT);
  pinMode(Single_LED_Output, OUTPUT);
  //turn off builtin LED
  digitalWrite(LED_BUILTIN, HIGH);

  connectToWiFi();

  startMillis = millis();
}

void loop()
{
  // put your main code here, to run repeatedly:

  //if wifi disconnected, turn off builtin LED
  if (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  // update timer
  currentMillis = millis();

  // if it has been the set delay of 5min or if the button is pressed, get info from server
  if (
    currentMillis - startMillis >= period ||
    digitalRead(Fetch_Settings_Button_EXT_PU) == LOW)
  {
    //sendMessageToSlack();
    FetchLEDSettings(PATH_TO_SETTINGS_FILE);

    // reset timer for next delay if the timer was what triggered the update
    if (currentMillis - startMillis >= period)
    {
      //currentMillis tracks time since program start, referenced to startMillis.
      //increasing the startmillis count by the period makes it act as if the program has
      //been running for 5 minutes less
      startMillis += period;
    }
  }

  writeToLEDs();
}

// put function definitions here:

void connectToWiFi()
{

  Serial.println();
  Serial.println();
  Serial.print("Attempting wifi connection to network: ");
  Serial.println(ssid);

  // wifi init
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Successfully connected to ");
  Serial.println(ssid);
  Serial.println();
  digitalWrite(LED_1_PIN, LOW);
}

void writeToLEDs()
{
  analogWrite(Red_LED_Output, Red_Duty_Cycle);
  analogWrite(Green_LED_Output, Green_Duty_Cycle);
  analogWrite(Blue_LED_Output, Blue_Duty_Cycle);
  digitalWrite(Single_LED_Output, LED_Status);
}

void FetchLEDSettings(String URL)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    if (https.begin(client, URL))
    {

      int httpCode = https.GET();
      if (httpCode == HTTP_CODE_OK)
      {
        String pageContent = https.getString();
        JsonDocument doc;
        deserializeJson(doc, pageContent);
        // Code here to parse string
        Red_Duty_Cycle = int(doc["Red"]);
        Green_Duty_Cycle = int(doc["Green"]);
        Blue_Duty_Cycle = int(doc["Blue"]);
        LED_Status = bool(doc["LED"]);

        Serial.println("Duty Cycles Set To: (RGB):");
        Serial.println(Red_Duty_Cycle);
        Serial.println(Green_Duty_Cycle);
        Serial.println(Blue_Duty_Cycle);
        Serial.println();
        Serial.print("The LED status has been set to ");
        if (LED_Status)
        {
          Serial.println("On.");
        }
        else
        {
          Serial.println("Off.");
        }
        Serial.println();
      }
      else
      {
        Serial.println("Unable to connect to settings file.");
      }
      https.end();
    }
  }
}

void sendMessageToSlack()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    if (https.begin(client, "https://maker.ifttt.com/trigger/ESP_Button_Pressed/with/key/dXFemypsBN5xGJpFthY2e4"))
    {
      Serial.println("Using GET method on URL: ");
      Serial.println("https://maker.ifttt.com/trigger/ESP_Button_Pressed/with/key/dXFemypsBN5xGJpFthY2e4");
      int httpCode = https.GET();
      if (httpCode == HTTP_CODE_OK)
      {
        Serial.println(https.getString());
      }
    }
    https.end();
  }
}