# Home lights

Basé sur :
- un ESP8266 avec firmware custom
- une fonction Alexa
- une plateforme MQTT (CloudMQTT)

## ESP8266

Ressources intéressantes :
https://www.esp8266.com/wiki/doku.php?id=start
https://f-leb.developpez.com/tutoriels/arduino/esp8266/debuter/
https://www.fais-le-toi-meme.fr/fr/electronique/tutoriel/programmes-arduino-executes-sur-esp8266-arduino-ide
https://github.com/espressif/esptool/wiki/ESP8266-Boot-Mode-Selection

### Installer l'environnement Arduino pour développer le firmware

https://arduino-esp8266.readthedocs.io/en/latest/

### Utilisation de l'ESP8266 - 12e
- GPIO2 pullup resistor à VCC
- GPIO15 à GND (pulldown pas nécessaire)

bootmode(1,7) = chargement serial d'un nouveau firmware
bootmode(3,7) = exécution normale

A priori assez puissant pour activer tout seul un relay SSR, d'après les expériences précédentes :)

### Utilisation de l'ESP8266 - 01

Pour le flasher :
- GPIO0 à GND
- CH_PD à VCC
- GND-GND, RX-TX et TX-RX sur le USBtoSerial
- pas besoin de connecter VCC au USBtoSerial



## CloudMQTT

https://customer.cloudmqtt.com/instance
Login : parisot.simon+cloudmqtt@gmail.com
