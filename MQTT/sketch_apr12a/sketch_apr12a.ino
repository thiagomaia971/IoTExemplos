#include <PubSubClient.h>
#include "passwords.h"
#include <ESP8266WiFi.h> 

String comando;
long lastMsg = 0;
char msg[10];

WiFiClient espClient;
PubSubClient client(espClient);

String getPayload(byte* payload, unsigned int length) {
  String messagePayload = "";
  for (int i = 0; i < length; i++) {
    messagePayload += (char)payload[i];
  }
  return messagePayload;
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando com ");
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
  String messagePayload = getPayload(payload, length);
  Serial.print("[");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(messagePayload);

  if (String(topic) == "/O8Rg5f638G/led") {
    if (messagePayload == "true") {
        digitalWrite(LED_BUILTIN, LOW);
      }
      else if (messagePayload == "false") {
        digitalWrite(LED_BUILTIN, HIGH);
      }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão com o servidor MQTT...");
    if (client.connect(mqtt_client, mqtt_user, mqtt_pass)) {
      Serial.println("Conectado");
      client.subscribe("/O8Rg5f638G/led");
    } 
    else {
      Serial.print("falhou, rc = ");
      Serial.print(client.state());
      Serial.println("Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void myConcat(String& dest,char c) {
 dest.concat(c);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  //reconectando caso caia
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sendTopic("/O8Rg5f638G/velocimetro");
  sendTopic("/O8Rg5f638G/peso");
  delay(500);
}

void sendTopic(char* topic) {
  comando = String(random(0, 200));
  comando.toCharArray(msg, 10);
  
  if (true) {
    Serial.print("[");
    Serial.print(topic);
    Serial.print("] = ");
    Serial.print(msg);
    Serial.println("");
  }
  client.publish(topic, msg);
}
