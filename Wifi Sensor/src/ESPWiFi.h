#pragma once
//Include needed libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>
#include <string.h>
#include <string>
#include "dhtnew.h"
#include "dhtnew.cpp"
#include <ArduinoJson.h>

//define better names for the IO pins
#define LED_1_PIN LED_BUILTIN
#define BUTTON_CAPTURE_EXT_PULL_UP D0
#define TEMPERATURE_HUMIDITY_SENSOR D1

//define the sensor object
DHTNEW Sensor(TEMPERATURE_HUMIDITY_SENSOR);

//define constant values needed for operation such
//as the WiFi AP name and password
//as well as the URLs needed to query
const char* ssid = "Lyman Family";
const char* password = "7072178282";

String baseURL = "https://josh-lyman-ssu.com/Database-Assignment/insert_data.php";
String timeURL = "https://timeapi.io/api/Time/current/zone?timeZone=";
String Node_ID = "node-6";

//global String to store timezone query
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


  //begin trying to connect to specified network
  WiFi.begin(ssid,password);
  //wait until connection is established
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }

  //print to Serial that connection was successful
  Serial.println();
  Serial.print("Successfully connected to ");
  Serial.println(ssid);
  Serial.println();

  //turn on the LED to show that it is connected
  digitalWrite(LED_1_PIN, LOW);
}

void setTimeZone(){
    //Prompt the user for an input with text on Serial
    Serial.println("Please select a timezone:");
    Serial.println("1) Eastern Time Zone (ET) - New York City, NY");
    Serial.println("2) Central Time Zone (CT) -Chicago, IL");
    Serial.println("3) Mountain Time Zone (MT) - Denver, CO");
    Serial.println("4) Pacific Time Zone (PT) - Los Angeles, CA");
    Serial.println("5) Alaska Time Zone (AKT) - Anchorage, AK");
    Serial.println("6) Hawaii-Aleutian Time Zone (HAT) - Honolulu, HI");
    Serial.println("7) Atlantic Time Zone (AT) - San Juan, Puerto Rico (US territory, not part of the contiguous US).");
    Serial.println("You have 7 seconds to make a choice or PT is chosen automatically.");

    //wait for user input
    delay(7000);

    //this means that a value has been input
    if (Serial.available())
    {
        //read input character
        char timezoneInt = Serial.read();

        //depending on input, set timezoneStr to be string for query
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
            //If input was invalid, select PT
            default:
                Serial.println("Invalid option selected. Using default value of PT");
                timezoneStr = "US/Pacific";
                break;
        }
        Serial.println("You have selected: " + timezoneStr);
    }
    //if no input was given, choose PT
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
    //Recursion check to allow for retrying transmission
    //if it has failed 4 times, just give up
    if (recursionNumber > 3)
    {
        Serial.println("Error: Too many failed requests.");
        Serial.println("Aborting");
        return;
    }



    String TargetURL;

    //if there is a timestamp, add it with the rest of the parameters
	if (timestamp != "")
	{
        //construct the target URL string with given parameters
        TargetURL = baseURL + String("?node_name=") + String(Node_ID) + String("&time_received=") + String(timestamp) + String("&temperature=") + String(temperature) + String("&humidity=") + String(humidity);
	}
	else
    //if no timestamp is given, omit the string for the parameter to let the server
    //handle the time
	{
		//if no valid timestamp, omit and make server add
        //construct the target URL string with given parameters
		TargetURL = baseURL + String("?node_name=") + String(Node_ID) + String("&temperature=") + String(temperature) + String("&humidity=") + String(humidity);
    
	}
	
    //ensure wifi is connected before trying to send
    if (WiFi.status() == WL_CONNECTED)
    {
        //create wifi client and http client
        WiFiClientSecure client;
        client.setInsecure();
        HTTPClient https;

        //print to Serial for monitoring status
        Serial.println("Requesting: -->" + TargetURL);
        Serial.println();

        //ensure that the client can begin the request
        if (https.begin(client,TargetURL))
        {
            //fetch response code from GET query
            int httpCode = https.GET();
            //Print response code to Serial
            Serial.println("Response code <--: " + String(httpCode));

            //if the code is -1, it is a software issue on the ESP.
            //Disconnect wifi, reconnect, and recursively try again to send
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