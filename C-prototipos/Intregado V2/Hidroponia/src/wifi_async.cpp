#include "wifi_async.h"
#include "mqtt_async.h"    
#include <WiFi.h>

static const char* _ssid;
static const char* _password;
static bool        _ready = false;

// Handler de eventos WiFi
void onWifiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.printf("WiFi conectado, IP: %s\n", WiFi.localIP().toString().c_str());
      _ready = true;
      mqttStart();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi desconectado, reconectando...");
      _ready = false;
      WiFi.begin(_ssid, _password);
      break;
    default:
      break;
  }
}

void wifiSetup(const char* ssid, const char* password) {
  _ssid     = ssid;
  _password = password;
  WiFi.onEvent(onWifiEvent);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);
}

void wifiLoop() {
  // Vacío: gestionado por eventos
}

bool wifiIsConnected() {
  return _ready;
}