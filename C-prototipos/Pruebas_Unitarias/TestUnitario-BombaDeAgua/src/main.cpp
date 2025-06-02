#include <Arduino.h>
#include <WiFi.h>
#include "my_webserver.h"

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, HIGH);  // Bomba apagada al inicio

  WiFi.begin("NBLink", "159753ba");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("WiFi conectado!");
  Serial.println(WiFi.localIP());

  setupWebServer();  // Arranca el servidor
}

void loop() {
  server.handleClient();  // Atiende las peticiones web
}
