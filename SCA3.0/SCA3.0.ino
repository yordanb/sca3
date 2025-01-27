/*
 * Program SCA3.0 
 * Hardware : ESP32
 * Library : ESP32-WROOM-DA Module
 * 
 */

#include <WiFi.h>
#include <PubSubClient.h>

// Konfigurasi WiFi
const char* ssid = "Aira alfa";
const char* password = "bismillah";

// Konfigurasi MQTT
const char* mqttServer = "209.182.237.240";
const int mqttPort = 1883;
const char* mqttTopicSend = "home/heating";
const char* mqttTopicControl = "home/control";

WiFiClient espClient;
PubSubClient client(espClient);

String unitSCA = "1";

// Pin untuk alat pemanas
const int heatingPin1 = 2;
const int heatingPin2 = 3;

// Variabel untuk data sensor
float suhu1 = 0;
float arus1 = 0;
float tekanan_udara1 = 0;
String status_heater1 = "off";

float suhu2 = 0;
float arus2 = 0;
float tekanan_udara2 = 0;
String status_heater2 = "off";

float suhu3 = 0;
float arus3 = 0;
float tekanan_udara3 = 0;
String status_heater3 = "off";

float suhu4 = 0;
float arus4 = 0;
float tekanan_udara4 = 0;
String status_heater4 = "off";

void setup() {
  Serial.begin(115200);
  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi...");
  }
  Serial.println("Terkoneksi ke WiFi");

  // Koneksi MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Kontrol alat pemanas
  if (strcmp(topic, mqttTopicControl) == 0) {
    if (payload[0] == '1') {
      digitalWrite(heatingPin1, HIGH);
      status_heater1 = "run";
      Serial.println("Perintah 1 datang");
    } else if (payload[0] == '0') {
      digitalWrite(heatingPin1, LOW);
      status_heater1 = "stop";
      Serial.println("Perintah 0 datang");
    } else if (payload[0] == '3') {
      digitalWrite(heatingPin2, HIGH);
      status_heater2 = "run";
      Serial.println("Perintah 3 datang");
    } else if (payload[0] == '2') {
      digitalWrite(heatingPin2, LOW);
      status_heater2 = "stop";
      Serial.println("Perintah 2 datang");
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Baca data sensor
  suhu1 = random(20, 30);
  arus1 = random(1, 10);
  tekanan_udara1 = random(1000, 2000);

  suhu2 = random(20, 30);
  arus2 = random(1, 10);
  tekanan_udara2 = random(1000, 2000);

  suhu3 = random(20, 30);
  arus3 = random(1, 10);
  tekanan_udara1 = random(1000, 2000);

  suhu4 = random(20, 30);
  arus4 = random(1, 10);
  tekanan_udara2 = random(1000, 2000);

  // Kirim data sensor ke Node-RED
  char json[200];
  sprintf(json, "{\"unitSCA\":\"%s\",\"suhu1\":%.2f,\"arus1\":%.2f,\"tekanan_udara1\":%.2f,\"status_heater1\":\"%s\",\"suhu2\":%.2f,\"arus2\":%.2f,\"tekanan_udara2\":%.2f,\"status_heater2\":\"%s\"}", unitSCA, suhu1, arus1, tekanan_udara1, status_heater1, suhu2, arus2, tekanan_udara2, status_heater2);
  client.publish(mqttTopicSend, json);
  delay(3000);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Terkoneksi ke MQTT");
      client.subscribe(mqttTopicControl);
    } else {
      delay(3000);
    }
  }
}
