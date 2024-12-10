# --------------------------------------------------
# Title: Accessing The Things Network
# --------------------------------------------------
# Program Details:
#     The purpose of this program is to access the TTN network
#     and retrieve the data from a device.
# Inputs:
#     - TTN server information
#     - Data format
# Outputs:
#     - Data received by TTN
# Date: October 11, 2024
# File Dependencies / Libraries: make sure you have installed: pip install paho-mqtt
# Compiler: Python 3
# Author: Farid Farahmand
# Versions:
#     - V1.0: Original
# Useful Links:
#     - https://nam1.cloud.thethings.network/console/applications/cubecell-bc1/api-keys
# --------------------------------------------------

#####  Used for platformIO
#    #include "LoRaWan_APP.h"
#    #include "Arduino.h"
#    #include <Wire.h>
#    #include <Adafruit_BMP280.h>
#    #include <Adafruit_AHTX0.h>
#    #include <AHT10.h>

import paho.mqtt.client as mqtt
import json
import requests

MQTT_SERVER = 'broker.mqtt-dashboard.com'
MQTT_PORT = 1883
MQTT_TOPIC = 'testtopic/temp/outTopic/lymanj'

# Examples of BASE64 data transmitted: 1234567803EA1722091C
# Use this here to check:
# in https://nam1.cloud.thethings.network/console/applications/cubecell-bc1/devices/faridcubecell/payload-formatters/uplink

# MQTT topic format for TTN uplinks
global counter
counter = 0

# Callback when message is received
def on_message(client, userdata, message):
    print("Received message from MQTT")
    global counter  # Declare that we are using the global variable
    try:
        #print(message)
        payload = str(message.payload.decode("utf-8"))
        counter += 1
        print(payload)
        url = "https://josh-lyman-ssu.com/mqtt_data.php"
        
        voltage = payload.split()[1]
        print(voltage)
        
        params = {
            "pot": voltage
        }
        
        try:
            response = requests.get(url, params=params)
            print("Response status code:", response.status_code)
            #print("Response content:", response.text)
        except requests.RequestException as e:
            print("An error occurred while sending the GET request:", e)
        
    except Exception as e:
        print(f"Error processing message: {e}")

# Callback when connected to TTN
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
        client.subscribe(MQTT_TOPIC)
    else:
        print(f"Failed to connect, return code {rc}")

# Setup MQTT client
client = mqtt.Client()

# Attach callback functions
client.on_connect = on_connect
client.on_message = on_message

# Optional: Enable logging for debugging
client.enable_logger()

# Connect to TTN (Consider using TLS if needed)
client.connect(MQTT_SERVER, MQTT_PORT, 60)

# Blocking loop to process messages
client.loop_forever()
