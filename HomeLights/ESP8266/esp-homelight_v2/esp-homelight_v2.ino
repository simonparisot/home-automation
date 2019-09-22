/*
  ESP8266/Relay
  parisot.simon (-) gmail.com
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// -------------------------------------------------------- CONFIGURATION
// Wifi
const char* ssid = "PrettyFlyForAWifi";
const char* password = "123tagada";   

// MQTT broker         
const char* mqtt_server = "m23.cloudmqtt.com";
const int mqtt_port = 15067;
const char* mqtt_user = "sqqvsrhi";
const char* mqtt_pwd = "6mvr3arW3WJ0";
const char* outTopic = "state";             
const char* inTopic = "command";

// pins of the relays
const int relay1 = 16;
const int relay2 = 14;
const int relay3 = 12;
const int relay4 = 13;

// pins and state of the physical switches                                      
const int switch1 = 5; int switchState1;  //CUI
const int switch2 = 0; int switchState2;  //PDT
const int switch3 = 4; int switchState3;  //TAB
const int switch4 = 3; int switchState4;  //SAL

WiFiClient espClient;
PubSubClient client(espClient);

// -------------------------------------------------------- WIFI SETUP
void setup_wifi() {

  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// -------------------------------------------------------- TOGGLE RELAY
void toggleRelay(const char *light_id, const char *intent){

  char msg[10];
  msg[9] = '\0';
  int pin;
  
  // find the relay pin associated with the light id
  if      (strcmp(light_id, "CUI") == 0)  { pin = relay1; }
  else if (strcmp(light_id, "PDT") == 0)  { pin = relay2; }
  else if (strcmp(light_id, "TAB") == 0)  { pin = relay3; }
  else if (strcmp(light_id, "SAL") == 0)  { pin = relay4; }
  else { 
    strcpy(msg, "NO-RELAY-");
    Serial.println(msg);
    client.publish(outTopic, msg);
    return;
    }

  // verify state versus intent
  int state = digitalRead(pin);
  if (state & strcmp(intent, "ON") == 0) { 
    strcpy(msg, light_id);
    strcat(msg, "-ALRON");
    Serial.println(msg);
    client.publish(outTopic, msg);
    return;
  };
  if (!state & strcmp(intent, "OF") == 0) {
    strcpy(msg, light_id);
    strcat(msg, "-ALROF");
    Serial.println(msg);
    client.publish(outTopic, msg);
    return;
  };
  
  //toggle relay and verify if toggle was effective
  digitalWrite(pin, !state);
  if (digitalRead(pin) == !state) { 
    if (!state){
      strcpy(msg, light_id);
      strcat(msg, "-IS-ON");
    }else{        
      strcpy(msg, light_id);
      strcat(msg, "-IS-OF");
    }
  }else{ 
    strcpy(msg, "ESP-ERROR");
  }
  Serial.println(msg);
  client.publish(outTopic, msg);
}

// -------------------------------------------------------- MESSAGE ARRIVED CALLBACK
void callback(const char *topic, const byte *payload, const unsigned int length) {

  Serial.print("new message arrived: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  char light_id[4];
  strncpy(light_id, (char*)payload, 3);
  light_id[3] = '\0';
  Serial.print("- light id is: ");
  Serial.println(light_id);

  char intent[3];
  if (length >= 6) {
    strncpy(intent, (char*)payload+4, 2);
    intent[2] = '\0';
    Serial.print("- intent is: ");
    Serial.println(intent);
  }

  toggleRelay(light_id, intent);
}

// -------------------------------------------------------- RECONNECT ROUTINE
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      client.subscribe(inTopic);
      client.publish(outTopic, "Hi, i'm connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// -------------------------------------------------------- SETUP FUNCTION
void setup() {

  //swap TX and RX to GPIO1 ans GPIO3
  //pinMode(1, FUNCTION_3); 
  //pinMode(3, FUNCTION_3); 
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

  // Initialise the input for physical switch
  pinMode(switch1, INPUT_PULLUP); switchState1 = digitalRead(switch1);
  pinMode(switch2, INPUT_PULLUP); switchState2 = digitalRead(switch2);
  pinMode(switch3, INPUT_PULLUP); switchState3 = digitalRead(switch3);
  pinMode(switch4, INPUT_PULLUP); switchState4 = digitalRead(switch4);

  // Initialize the pin which commands the relay
  pinMode(relay1, OUTPUT); digitalWrite(relay1, LOW);
  pinMode(relay2, OUTPUT); digitalWrite(relay2, LOW);
  pinMode(relay3, OUTPUT); digitalWrite(relay3, LOW);
  pinMode(relay4, OUTPUT); digitalWrite(relay4, LOW);
  
  //Serial.begin(74880);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// -------------------------------------------------------- LOOP FUNCTION
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (digitalRead(switch1) != switchState1) { 
    Serial.println("Command received from physical switch CUI");
    switchState1 = digitalRead(switch1);
    toggleRelay("CUI", "NA");
  }
  if (digitalRead(switch2) != switchState2) {
    Serial.println("Command received from physical switch PDT"); 
    switchState2 = digitalRead(switch2);
    toggleRelay("PDT", "NA");
  }
  if (digitalRead(switch3) != switchState3) { 
    Serial.println("Command received from physical switch TAB");
    switchState3 = digitalRead(switch3);
    toggleRelay("TAB", "NA");
  }
  if (digitalRead(switch4) != switchState4) { 
    Serial.println("Command received from physical switch SAL");
    switchState4 = digitalRead(switch4);
    toggleRelay("SAL", "NA");
  }
  
  delay(400);
}
