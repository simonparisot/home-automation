# Home cinema screen controller

Control my home cinema screen with Alexa.

## Hardware

I was tired of using bare ESP8266-12 as in previous projects (to unstable, and to annoying to setup). So here I used NodeMCU board, which is a bit larger (by 2cm), a bit more expensive (by 2€) but undeniably easier to use. 

To power the NodeMCU, I used a small 5V power supply module (HLK-PM01).
To switch currents (for the screen motor) I used 2 solid state relays (Omron G3MB-202P). Fortunately, the GPIO of NodeMCU were able to activate directly the relays without requiring extra transistors.
All of that fitted (almost :D) perfectly in a small switch box.

Activating the fish required a low current switch. I used a BC547 transistor.

![Circuit](https://imagizer.imageshack.com/v2/680x540q90/r/923/ABfV7b.png)


## ESP8266 (NodeMCU)

The C++ firmware is in the esp-screen-controller directory.

Based on the work of VeeriSubbuAmi: 
https://electronicsinnovation.com/how-to-connect-nodemcu-esp8266-with-aws-iot-core-using-arduino-ide-mqtt/

It it connected to MQTT topics on AWS IoT Core
- "state" to report state
- "command" to listen for commands

## Lambda + Alexa skill

Alexa custom skill require a lambda to send the MQTT command message.