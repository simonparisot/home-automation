// Screen controller by Simon (parisot.simon -at- gmail)
// made for ESP8266-12E, AWS IoT Core and a motorized home cinema screen!
// based on the work of VeeriSubbuAmi (https://electronicsinnovation.com)
// and various community libraries

#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// wifi
const char* ssid = "PrettyFlyForAWifi";
const char* password = "xxxxxxxxx";

// pins of the relays
const int rollPin = 16;
const int unrollPin = 14;

// exact duration of rolling and unrolling of the screen
const int rollDelay = 2000;
const int unrollDelay = 2000;

// AWS IoT Core
const char* AWS_endpoint = "xxxxxxxxxxxxxx-ats.iot.eu-west-3.amazonaws.com";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  char screen = (char)payload[65]; // Extracting the controlling command from the Payload to Controlling screen from AWS
  Serial.print("screen command=");
  Serial.println(screen);
  if(screen==49) // 49 is the ASCI value of 1
  {
    digitalWrite(unrollPin, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Screen is unrolling... ");
    delay(unrollDelay);
    digitalWrite(unrollPin, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Done.");
  }
  else if(screen==48) // 48 is the ASCI value of 0
  {
    digitalWrite(rollPin, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Screen is rolling up... ");
    delay(rollDelay);
    digitalWrite(rollPin, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Done.");
   }          
  Serial.println();
}


WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883, callback, espClient); //set MQTT port number to 8883 as per //standard
long lastMsg = 0;
char msg[50];
int value = 0;


void setup_wifi() {

  delay(10);
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
    while(!timeClient.update()){
    timeClient.forceUpdate();
  }
  
  espClient.setX509Time(timeClient.getEpochTime());

}

void reconnect() {
  while (!client.connected()) {
    
    Serial.print("Attempting MQTT connection...");
      if (client.connect("ESPthing")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "Hello world, I am the screen controller.");
      // ... and resubscribe
      client.subscribe("inTopic");
      Serial.println("Waiting for messages.");
    } else {  
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      char buf[256];
      espClient.getLastSSLError(buf,256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  
  Serial.begin(74880);
  Serial.setDebugOutput(false);
  
  // initialize digital pin as an output for builtin led and the 2 relays
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(rollPin, OUTPUT);
  pinMode(unrollPin, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(rollPin, LOW);
  digitalWrite(unrollPin, LOW);
  
  setup_wifi();
  delay(1000);
  
  // filesystem is necessary for loading security certificates
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap()); // not enough heap can be a problem

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r");
  if (!cert) { Serial.println("Failed to open cert file"); }
  else Serial.print("Cert file opened... ");
  delay(1000);
  
  if (espClient.loadCertificate(cert)) Serial.println("cert loaded");
  else Serial.println("cert not loaded");
  
  // Load private key file
  File private_key = SPIFFS.open("/private.der", "r");
  if (!private_key) { Serial.println("Failed to open private key file"); }
  else Serial.print("Private key file opened... ");
  delay(1000);
  
  if (espClient.loadPrivateKey(private_key)) Serial.println("private key loaded");
  else Serial.println("private key not loaded");
  
  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r");
  if (!ca) { Serial.println("Failed to open CA file"); }
  else Serial.print("CA file opened... ");
  delay(1000);
  
  if(espClient.loadCACert(ca)) Serial.println("CA loaded");
  else Serial.println("CA not loaded");
  
  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap()); // not enough heap can be a problem
}



void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
}
