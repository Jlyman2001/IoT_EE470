#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>
#include <string.h>
#include <string>
#include "dhtnew.h"
#include "dhtnew.cpp"
#include <ArduinoJson.h>


#define LED_1_PIN LED_BUILTIN
#define BUTTON_CAPTURE_EXT_PULL_UP D0
#define TEMPERATURE_HUMIDITY_SENSOR D1

DHTNEW Sensor(TEMPERATURE_HUMIDITY_SENSOR);

const char* ssid = "Lyman Family";
const char* password = "7072178282";
String baseURL = "https://josh-lyman-ssu.com/Database-Assignment/insert_data.php";
String timeURL = "https://timeapi.io/api/Time/current/zone?timeZone=";
String Node_ID = "node-6";
String timezoneStr;

using namespace std;

void setTimeZone();
void connectToWiFi();
double captureTemp();
double captureHumidity();
void uploadData(String timestamp, double temperature, double humidity);
String MygetTime();


void connectToWiFi()
{
    
  Serial.println();
  Serial.println();
  Serial.print("Attempting wifi connection to network: ");
  Serial.println(ssid);


  //wifi init
  WiFi.begin(ssid,password);
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

void setTimeZone(){
    Serial.println("Please select a timezone:");
    Serial.println("1) Eastern Time Zone (ET) - New York City, NY");
    Serial.println("2) Central Time Zone (CT) -Chicago, IL");
    Serial.println("3) Mountain Time Zone (MT) - Denver, CO");
    Serial.println("4) Pacific Time Zone (PT) - Los Angeles, CA");
    Serial.println("5) Alaska Time Zone (AKT) - Anchorage, AK");
    Serial.println("6) Hawaii-Aleutian Time Zone (HAT) - Honolulu, HI");
    Serial.println("7) Atlantic Time Zone (AT) - San Juan, Puerto Rico (US territory, not part of the contiguous US).");

    Serial.println("You have 7 seconds to make a choice or PT is chosen automatically.");

    delay(7000);

    

    //this means that a value has been input
    if (Serial.available())
    {
        char timezoneInt = Serial.read();
        switch (timezoneInt)
        {
            case '1':
                timezoneStr = "US/Eastern";
                break;
            case '2':
                timezoneStr = "US/Central";
                break;
            case '3':
                timezoneStr = "US/Mountain";
                break;
            case '4':
                timezoneStr = "US/Pacific";
                break;
            case '5':
                timezoneStr = "US/Alaska";
                break;
            case '6':
                timezoneStr = "US/Hawaii";
                break;
            case '7':
                timezoneStr = "America/Puerto_Rico";
                break;
            default:
                Serial.println("Invalid option selected. Using default value of PT");
                timezoneStr = "US/Pacific";
                break;
        }
        Serial.println("You have selected: " + timezoneStr);
    }
    else
    {
        Serial.println("No input received. Using default value of PT");
        timezoneStr = "US/Pacific";
}

}

double captureTemp()
{
  Sensor.read();
  return Sensor.getTemperature();

}

double captureHumidity()
{
  Sensor.read();
  return Sensor.getHumidity();
}

void uploadData(String timestamp, double temperature, double humidity, int recursionNumber)
{
    if (recursionNumber > 3)
    {
        Serial.println("Error: Too many failed requests.");
        Serial.println("Aborting");
        return;
    }



     String TargetURL;
	if (timestamp != "")
	{
    TargetURL = baseURL + String("?node_name=") + String(Node_ID) + String("&time_received=") + String(timestamp) + String("&temperature=") + String(temperature) + String("&humidity=") + String(humidity);
    //String TargetURL = baseURL  + TimeParameter + NodeParameter + TemperatureParameter + HumidityParameter;
	}
	else
	{
		//if no valid timestamp, omit and make server add
		TargetURL = baseURL + String("?node_name=") + String(Node_ID) + String("&temperature=") + String(temperature) + String("&humidity=") + String(humidity);
    
	}
	
    if (WiFi.status() == WL_CONNECTED)
    {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    Serial.println("Requesting: -->" + TargetURL);
    Serial.println();
    if (https.begin(client,TargetURL))
    {
        int httpCode = https.GET();
        Serial.println("Response code <--: " + String(httpCode));
        if (httpCode == -1)
        {
            Serial.println("Wifi appears to have been disconnected. Retrying.");
            WiFi.disconnect();
            connectToWiFi();
            https.end();
            
            uploadData(timestamp, temperature, humidity, recursionNumber + 1);
        }
        https.end();
        
    }
    else
    {
        Serial.printf("[HTTPS] Unable to connect\n");
    }

    }

}

String MygetTime()
{
  String returnVal =  "";


  if (WiFi.status() == WL_CONNECTED)
    {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    timeURL = timeURL + timezoneStr;

    if (https.begin(client,timeURL))
    {
        int httpCode = https.GET();
        if (httpCode == HTTP_CODE_OK)
        {
            String pageContent =  https.getString();
            //Serial.println(pageContent);
            JsonDocument doc;
            deserializeJson(doc,pageContent);
            returnVal = String(doc["dateTime"]);
        }
        else if (httpCode == -1)
        {
            //error -1 means software issue on ESP side
            //disconnect all
            WiFi.disconnect();
            connectToWiFi();
            client.abort();

            //retry connection
            if (https.begin(client,timeURL))
            {
                int httpCode = https.GET();
                if (httpCode == HTTP_CODE_OK)
                {
                    String pageContent =  https.getString();
                    //Serial.println(pageContent);
                    JsonDocument doc;
                    deserializeJson(doc,pageContent);
                    returnVal = String(doc["dateTime"]);
                }
                //if this doesn't work, just let server handle timestamp    
            }
        }
        else
        {
            Serial.println("Unable to retrieve timestamp.");
            Serial.println("The server will stamp upon receipt.");
        }
        https.end();
        
    }

    }

  return returnVal;
}