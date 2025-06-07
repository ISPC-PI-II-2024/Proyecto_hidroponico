#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include "wifi_personalizado.h"
#include "GSM.h"
#include "MqttManager.h"

// Pin definido para el botón de Wi-Fi (usado en WiFiCtrl). Da acceso al portal web
static constexpr uint8_t WIFI_BUTTON_PIN = 0;

//===================================
// CommunicationManager
//===================================
// Unifica Wi-Fi, GSM y MQTT:
//  1) Intenta primero conectarse por Wi-Fi
//  2) Si Wi-Fi falla (timeout o error), arranca GSM (GPRS)
//  3) Una vez que hay red disponible (Wi-Fi o GPRS), inicia MQTT
//
// El setup de Preferences (lectura de parámetros para umbrales, etc.) 
// se hace en main.cpp. Aquí se recibe todo en el constructor.

class CommunicationManager {
public:
  enum class State : uint8_t {
    Idle,
    ConectandoWiFi,
    ConectandogGSM,
    ConectandogMQTT,
    Conectado,
    Error
  };

  // Constructor:
  //
  //  • consoleSerial: Serial para logs y menús GSM
  //  • modemSerial:   Serial al que está conectado el módem AT
  //  • wifiSerial:    Serial para debug de Wi-Fi (opcional, se pasa Serial)
  //  • wifiSSID, wifiPass: credenciales Wi-Fi
  //  • mqttHost, mqttPort, mqttTopic, mqttUser, mqttPass: parámetros MQTT
  //  • wifiTimeoutMs: cuánto esperar antes de dar fallido Wi-Fi (ms)
  //  • gsmTimeoutMs:  cuánto esperar antes de dar fallido GSM (ms)
  CommunicationManager(HardwareSerial& consoleSerial,
                       HardwareSerial& modemSerial,
                       HardwareSerial& wifiSerial,
                       const char* wifiSSID,
                       const char* wifiPass,
                       const char* mqttHost,
                       uint16_t mqttPort,
                       const char* mqttTopicInfo,
                       const char* mqttTopicLectura,
                       const char* mqttTopicAlarma,
                       const char* mqttUser = nullptr,
                       const char* mqttPass = nullptr,
                       unsigned long wifiTimeoutMs = 15000,
                       unsigned long gsmTimeoutMs  = 30000);

  // Debe llamarse en setup(): arranca el ciclo de conexión
  void begin();

  // Debe llamarse en loop(): mantiene FSM de conexión Wi-Fi, GSM y MQTT
  void loop();

  // Publicar mensaje por MQTT (solo si MQTT está CONECTADO)
  void publicar(const char* topic, const char* payload);

  // Devuelve el estado actual de la máquina de estados
  State obtenerEstado() const;

  // Indica si la conexión de red (Wi-Fi o GPRS) está activa
  bool redHabilitada() const;

private:
  // Inicia la conexión Wi-Fi (usando WiFiCtrl)
  void iniciarWiFi();

  // Inicia la conexión GSM (GPRS) llamando a gsm_.begin()
  void iniciaGSM();

  // Inicia MQTT usando los parámetros recibidos en el constructor
  void iniciaMQTT();

  // Comprueba si Wi-Fi ya excedió su timeout
  bool wifiTimedOut() const;

  // Comprueba si GSM ya excedió su timeout
  bool gsmTimedOut() const;

  // Clientes y managers internos
  WiFiCtrl        wifiCtrl_;     // Controla conexión Wi-Fi (portal cautivo, eventos, etc.)
  GSM             gsm_;          // Controla conexión GSM/GPRS
  WiFiClient      wifiClient_;   // Cliente TCP para PubSubClient sobre Wi-Fi
  TinyGsmClient   gsmClient_;    // Cliente TCP para PubSubClient sobre GSM
  MqttManager     mqttMgr_;      // Cliente MQTT (se conecta cuando hay red)

  // Parámetros de configuración
  const char*     wifiSSID_;
  const char*     wifiPass_;
  const char*     mqttHost_;
  uint16_t        mqttPort_;
  const char*     mqttTopicInfo_;
  const char*     mqttTopicLectura_;
  const char*     mqttTopicAlarma_;
  const char*     mqttUser_;
  const char*     mqttPass_;

  // Tiempos de espera (milisegundos)
  unsigned long   wifiStartMs_;
  unsigned long   wifiTimeoutMs_;
  unsigned long   gsmStartMs_;
  unsigned long   gsmTimeoutMs_;

  // Estado interno de la máquina de estados
  State           state_;
};
