# Fish controller

Control a singind fish with an HTTP API. Because why not!

## Hardware

I used a NodeMCU ESP8266 as in the screen controller project (see there why).

Activating the fish required a low current switch. I used a BC547 transistor.
![Circuit](https://imagizer.imageshack.com/v2/400x220q90/r/924/r3D0oD.png)


## ESP8266 (NodeMCU)

The C++ firmware is in the esp-fish-controller directory.

Based on the work of VeeriSubbuAmi: 
https://electronicsinnovation.com/how-to-connect-nodemcu-esp8266-with-aws-iot-core-using-arduino-ide-mqtt/

It it connected to MQTT topics on AWS IoT Core
- "state" to report state
- "command" to listen for commands

## Lambda

A simple lambda + API Gateway to expose the MQTT IoT Core as an HTTP API
