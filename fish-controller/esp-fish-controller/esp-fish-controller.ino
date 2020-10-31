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
const char* ssid = "{{HOME_WIFI_SSID}}";
const char* password = "{{HOME_WIFI_PASSWORD}}";

// pins of the relays
const int rollPin = 13;

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
  pinMode(rollPin, OUTPUT);
  digitalWrite(rollPin, LOW);

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

  if (thing_type == "fish" && thing_serial == "0") {
    if (command == "sing") {
      
      client.publish("state", "{\"thing_type\":\"fish\",\"thing_serial\":\"0\",\"response\":\"accepted\"}");
      digitalWrite(rollPin, HIGH);
      Serial.print("Fish is singing... ");
      delay(100);
      digitalWrite(rollPin, LOW);
      
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
    if (client.connect("fish")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("state", "{\"thing_type\":\"fish\",\"thing_serial\":\"0\",\"state\":\"connected\"}");
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
