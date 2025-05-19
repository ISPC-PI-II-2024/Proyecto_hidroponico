#pragma once
#include <WiFi.h>
#include <Preferences.h>

class wifi {
public:
  enum class State : uint8_t {
    Idle,
    WaitingCredentials,
    Connecting,
    Connected,
    Error
  };

  wifi(HardwareSerial& serial = Serial);
  void begin();        // Inicia la tarea: carga credenciales o pide por Serial
  void loop();         // Avanza la máquina de estados
  State getState() const;

private:
  // Flujo de trabajo
  void solicitarCredentiales();     // Pide SSID por serial
  bool leerCredentiales();        // Lee SSID/pass (bloqueante)
  void guardarCredentiales();        // Guarda en Preferences
  bool cargarCredentiales();        // Carga desde Preferences

  void startConnect();           // Lanza WiFi.begin(...)
  static void onWiFiEvent(WiFiEvent_t event); // Callback estático

  // Datos internos
  HardwareSerial& _serial;
  Preferences     _prefs;
  State           _state;
  String          _ssid, _pass;

  // Instancia global para el callback
  static wifi* _instance;
};
