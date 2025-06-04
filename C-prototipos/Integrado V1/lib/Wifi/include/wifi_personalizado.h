#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>  // Librería para portal cautivo en ESP32


//================================
// Clase WiFiCtrl
//================================
class WiFiCtrl {
public:
  //-----------------------------------
  // Enumeración de estados de WiFiCtrl
  //-----------------------------------
    enum class State : uint8_t {
      Idle,           // Estado inicial, sin conexión
      Conectando,     // Intentando conectar a la red WiFi
      PortalActivo,   // Portal cautivo activo
      Conectado,      // Conexión exitosa a la red WiFi
      Error           // Error en la conexión o configuración
    };

  //-----------------------------------
  // Constructor de WiFiCtrl
  //-----------------------------------
    WiFiCtrl(HardwareSerial& serial = Serial);
    void iniciar();                                 // Inicializa WiFiManager / Configura callback y arranca conexión
    static void onWiFiEvent(WiFiEvent_t event);     // Método estático para eventos de WiFi (GOT_IP, DISCONNECTED, etc.)
    State obtenerEstado() const;                    // Devuelve el estado actual


//-----------------------------------
// Métodos de utilidad
//-----------------------------------
private:
    HardwareSerial& _serial;
    State           _estado;
    WiFiEventId_t   _eventoId;          // ID del evento registrado (por si queremos desconectarnos)
    WiFiManager     _PortalCautivo;     // Objeto para manejar portal cautivo

    void iniciarSTA();                  // Intenta conectar en modo estación
    void iniciarSTAPortal();            // Levanta AP + portal cautivo
    void reportarEstado(const char* msg);
};