// Screen controller by Simon (parisot.simon -at- gmail)
// made for ESP8266-12E, AWS IoT Core and a motorized home cinema screen!
// based on the work of VeeriSubbuAmi (https://electronicsinnovation.com)
// and various community libraries

#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// wifi
const char* ssid = "PrettyFlyForAWifi";
const char* password = "123tagada";

// pins of the relays
const int light0pin = 14;
const int light1pin = 12;
const int light2pin = 13;
const int light3pin = 15;

// JSON parsing object in memory
const size_t capacity = JSON_OBJECT_SIZE(3) + 60;
DynamicJsonDocument doc(capacity);

// AWS IoT Core
const char* AWS_endpoint = "a377qra5nhpqt6-ats.iot.eu-west-1.amazonaws.com";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

WiFiClientSecure espClient;
PubSubClient client(espClient);



void setup() {
  
  Serial.begin(115200);

  // initialize digital pin as an output for builtin led and the 2 relays
  pinMode(light0pin, OUTPUT);
  pinMode(light1pin, OUTPUT);
  pinMode(light2pin, OUTPUT);
  pinMode(light3pin, OUTPUT);
  
  digitalWrite(light0pin, LOW);
  digitalWrite(light1pin, LOW);
  digitalWrite(light2pin, LOW);
  digitalWrite(light3pin, LOW);

  setup_wifi();
  client.setServer(AWS_endpoint, 8883);
  client.setCallback(callback);
  delay(1000);

  // filesystem is necessary for loading security certificates
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap()); // not enough heap can be a problem

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r");
  if (!cert) {
    Serial.println("Failed to open cert file");
  }
  else Serial.print("Cert file opened... ");
  delay(1000);

  if (espClient.loadCertificate(cert)) Serial.println("cert loaded");
  else Serial.println("cert not loaded");

  // Load private key file
  File private_key = SPIFFS.open("/private.der", "r");
  if (!private_key) {
    Serial.println("Failed to open private key file");
  }
  else Serial.print("Private key file opened... ");
  delay(1000);

  if (espClient.loadPrivateKey(private_key)) Serial.println("private key loaded");
  else Serial.println("private key not loaded");

  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r");
  if (!ca) {
    Serial.println("Failed to open CA file");
  }
  else Serial.print("CA file opened... ");
  delay(1000);

  if (espClient.loadCACert(ca)) Serial.println("CA loaded");
  else Serial.println("CA not loaded");

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap()); // not enough heap can be a problem
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Run whenever a new message is received from AWS IoT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void callback(char* topic, byte* payload, unsigned int length) {

  String msg = String((char *)payload);
  deserializeJson(doc, msg);

  String thing_type = doc["thing_type"];
  String thing_serial = doc["thing_serial"];
  String command = doc["command"];

  Serial.print("Message arrived on topic [");
  Serial.print(topic);
  Serial.println("] ");
  Serial.println(msg);

  if (thing_type == "light") {
    if (thing_serial == "0") {
          client.publish("state", "{\"thing_type\":\"light\",\"thing_serial\":\"0\",\"response\":\"accepted\"}");
          if (command == "on") digitalWrite(light0pin, HIGH);
          if (command == "off") digitalWrite(light0pin, LOW);
          Serial.print("Light 0 has been switched");
    }
    if (thing_serial == "1") {
          client.publish("state", "{\"thing_type\":\"light\",\"thing_serial\":\"1\",\"response\":\"accepted\"}");
          if (command == "on") digitalWrite(light1pin, HIGH);
          if (command == "off") digitalWrite(light1pin, LOW);
          Serial.print("Light 1 has been switched");
    }
    if (thing_serial == "2") {
          client.publish("state", "{\"thing_type\":\"light\",\"thing_serial\":\"2\",\"response\":\"accepted\"}");
          if (command == "on") digitalWrite(light2pin, HIGH);
          if (command == "off") digitalWrite(light2pin, LOW);
          Serial.print("Light 2 has been switched");
    }
    if (thing_serial == "3") {
          client.publish("state", "{\"thing_type\":\"light\",\"thing_serial\":\"3\",\"response\":\"accepted\"}");
          if (command == "on") digitalWrite(light3pin, HIGH);
          if (command == "off") digitalWrite(light3pin, LOW);
          Serial.print("Light 3 has been switched");
    }
  }
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Function to setup WiFi
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void setup_wifi() {

  // We start by connecting to a WiFi network
  espClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  espClient.setX509Time(timeClient.getEpochTime());

}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Function to connect to AWS IoT MQTT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void reconnect() {
  ESP.wdtDisable();
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("light_livingroom")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("state", "{\"thing_type\":\"light_livingroom\",\"thing_serial\":\"na\",\"state\":\"connected\"}");
      // ... and resubscribe
      delay(10);
      client.subscribe("command");
      Serial.println("Waiting for messages.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      char buf[256];
      espClient.getLastSSLError(buf, 256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  ESP.wdtEnable(0);
}
