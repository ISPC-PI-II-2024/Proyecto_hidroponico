#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT11.h"

const char* ssid = "TU_SSID";
const char* password = "TU_PASSWORD";
const char* mqtt_server = "broker.hivemq.com"; // o el que uses

WiFiClient espClient;
PubSubClient client(espClient);
DHT11Sensor sensor;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado a MQTT");
    } else {
      delay(5000);  // espera 5 segundos e intenta de nuevo
    }
  }
}

void setup() {
  Serial.begin(115200);
  sensor.begin();       // Inicia el sensor
  setup_wifi();         // Conecta a Wi-Fi
  client.setServer(mqtt_server, 1883); // Configura el broker
}

void loop() {
  if (!client.connected()) {
    reconnect(); // reconecta si el broker se cayó
  }
  client.loop(); // mantiene activa la conexión MQTT

  float temp = sensor.readTemperature();
  float hum = sensor.readHumidity();

  if (!isnan(temp) && !isnan(hum)) {
    char payload[100];
    snprintf(payload, sizeof(payload), "{\"temp\":%.2f,\"hum\":%.2f}", temp, hum);
    client.publish("hidroponia/sensor/dht11", payload);
  }

  delay(5000); // espera 5 segundos
}
