# MQTT RC Switch

## What's is good for

I'm using Homebridge on a Raspberry Pi, that is connected to my iOS and AppleTV devices. The Raspberry also runs Mosquitto to send and receive MQTT messages from external sensors and actuators.

One of these actuators is this MQTT RC Switch. It sends RF commands to remote controlled outlets and publishes their status to Homebridge.

## How to get it running

* Install Homebridge on your Raspberry
* Install Mosquitto on your Raspberry
* Make sure you can send and receive MQTT messages

* Configure your homebridge setup with accessoires (I'm using *homebridge-mqttswitch* for my lights, checkout the *sample_config.json* file to see how my setup looks like)
* Flash your ESP8266 board with this code and have it communicate with the raspberry.
(_Note_: This also somewhat works with other boards like an Arduino, but you may have to update the code to use the Ethernet library instead of WiFi)

## Read on

These are excellent resources to get started:

* http://practicalusage.com/apple-homekit-with-ios-with-arduino-esp8266-with-raspberry-pi/ and
* http://practicalusage.com/apple-homekit-on-ios-with-arduino-esp8266-through-raspberry-pi-part-2/

* http://www.instructables.com/id/Installing-MQTT-BrokerMosquitto-on-Raspberry-Pi/

* http://www.instructables.com/id/Using-an-ESP8266-to-Control-Mains-Sockets-Using-43/
