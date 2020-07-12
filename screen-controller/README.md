# Home cinema screen controller

Controll my home cinema screen with Alexa or any HTTP call

Based on:
- ESP8266-12E with custom firmware
- AWS IoT Core
- Alexa Lambda function

## ESP8266

The C firmware is in the esp-screen-controller directory.

Based on the work of VeeriSubbuAmi: 
https://electronicsinnovation.com/how-to-connect-nodemcu-esp8266-with-aws-iot-core-using-arduino-ide-mqtt/

## Development environment

Used a FT232RL USB serial adapter, some 5k pull-up/pull-down resistor and a 470nf capacitor.
https://arduino-esp8266.readthedocs.io/en/latest/boards.html

ESP8266-12E should be powerful enough to activate by himself an SSR relay, but to be confirmed ;)