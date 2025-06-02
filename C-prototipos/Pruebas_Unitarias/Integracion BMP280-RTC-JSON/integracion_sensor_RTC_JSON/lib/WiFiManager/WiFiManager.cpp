#include "WiFiManager.h"
#include "WiFiCred.h"
WiFiManager::WiFiManager() {}

void WiFiManager::conectar() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi");
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Conectado a WiFi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ No se pudo conectar al WiFi.");
  }
}

bool WiFiManager::estaConectado() {
  return WiFi.status() == WL_CONNECTED;
}
