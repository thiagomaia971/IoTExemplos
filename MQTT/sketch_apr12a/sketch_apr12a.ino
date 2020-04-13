#include <PubSubClient.h>
#include "passwords.h"
#include <ESP8266WiFi.h> 

String comando;
long lastMsg = 0;
char msg[10];

WiFiClient espClient;
PubSubClient client(espClient);

String getTopic(char* param) {
  String Topic = "";
  int i = 0;
  while((char)param[i] != 0) {
    Topic += (char)param[i];
    i++;
  }
  return Topic;  
}

int getPayload(byte* payload, unsigned int length) {
  String pay = "";
  for (int i = 0; i < length; i++) {
    pay = pay + (char)payload[i];
  }
  return pay.toInt();  
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
  String callTopic = getTopic(topic);
  int pay = getPayload(payload, length);

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println(); 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão com o servidor MQTT...");
    if (client.connect(mqtt_client, mqtt_user, mqtt_pass)) {
      Serial.println("Conectado");
//      client.subscribe("/teste");
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
  digitalWrite(LED_BUILTIN, LOW);
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

  // Enviando para o topic teste!
  comando = String(random(0, 4096));
  comando.toCharArray(msg, 10);
  client.publish("/teste", msg);
  delay(3000);
}
