/*
  ESP8266/Relay
  parisot.simon (-) gmail.com
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Configuration
const char* ssid = "{{HOME_WIFI_SSID}}";                // wifi configuration
const char* password = "{{HOME_WIFI_PASSWORD}}";            
const char* mqtt_server = "m23.cloudmqtt.com";          // MQTT broker configuration
int mqtt_port = 15067;
const char* mqtt_user = "{{CLOUDMQTT_USER}}";
const char* mqtt_pwd = "{{CLOUDMQTT_PASSWORD}}";
const char* outTopic = "state";             
const char* inTopic = "command";
int relayPin = 12;                                       // pin that commands the relay
int buttonPin = 14;                                      // pin for the physical switch
int buttonPreviousState = HIGH;
int relayState;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Toggle the relay if 2 was received as first characters
  if ((char)payload[0] == '1') { 
    Serial.println("Command received from the internet!");
    toggleRelay();
  }
}

void toggleRelay(){
  // Serial.println("Toggle the relay");
  relayState = digitalRead(relayPin);
  digitalWrite(relayPin, !relayState);
  if (!relayState) {
    client.publish(outTopic, "Relay is on");
  }else{
    client.publish(outTopic, "Relay is off");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Hi, i'm connected");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(relayPin, OUTPUT);     // Initialize the pin which commands the relay
  pinMode(buttonPin, INPUT);     // Initialise the input for physical switch
  digitalWrite(buttonPin, HIGH); 
  Serial.begin(74880);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (digitalRead(buttonPin) != buttonPreviousState) { 
    buttonPreviousState = digitalRead(buttonPin);
    toggleRelay();
    Serial.println("Command received from home switch!");
    delay(400);
  }
}
