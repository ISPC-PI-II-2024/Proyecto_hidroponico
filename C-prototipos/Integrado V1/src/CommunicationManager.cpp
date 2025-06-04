#include "CommunicationManager.h"
#include <Arduino.h>
#include <WiFi.h>

//===================================
// Implementación de CommunicationManager
//===================================

//------------------------------------------------------------------------------
// Constructor: guarda referencias y parámetros
//------------------------------------------------------------------------------
CommunicationManager::CommunicationManager(HardwareSerial& consoleSerial,
                                           HardwareSerial& modemSerial,
                                           HardwareSerial& wifiSerial,
                                           const char* wifiSSID,
                                           const char* wifiPass,
                                           const char* mqttHost,
                                           uint16_t mqttPort,
                                           const char* mqttTopic,
                                           const char* mqttUser,
                                           const char* mqttPass,
                                           unsigned long wifiTimeoutMs,
                                           unsigned long gsmTimeoutMs)
  : wifiCtrl_(wifiSerial,        // Serial para debug de Wi-Fi
              WIFI_BUTTON_PIN,   // Pin que dispara “modo configuración” (INPUT_PULLUP)
              "ESP32_Config",    // SSID elegido para el AP de configuración
              nullptr)           // (opcional) password del AP; si es nullptr, AP queda abierto
  , gsm_(consoleSerial, modemSerial, 115200)
  , wifiClient_()
  , gsmClient_(gsm_.getModem())
  , mqttMgr_(wifiClient_,
             gsmClient_,
             [this](){ return this->wifiCtrl_.obtenerEstado() == WiFiCtrl::State::Conectado; },
             [this](){ return this->gsm_.getState()            == GSM::State::Conectado; })
  , wifiSSID_(wifiSSID)
  , wifiPass_(wifiPass)
  , mqttHost_(mqttHost)
  , mqttPort_(mqttPort)
  , mqttTopic_(mqttTopic)
  , mqttUser_(mqttUser)
  , mqttPass_(mqttPass)
  , wifiStartMs_(0)
  , wifiTimeoutMs_(wifiTimeoutMs)
  , gsmStartMs_(0)
  , gsmTimeoutMs_(gsmTimeoutMs)
  , state_(State::Idle)
{}

//------------------------------------------------------------------------------
// begin(): arranca el proceso de conexión por Wi-Fi
//------------------------------------------------------------------------------
void CommunicationManager::begin() {
  state_ = State::ConectandoWiFi;
  wifiStartMs_ = millis();
  iniciarWiFi();    // Lanza autoConnect con portal cautivo
}

//------------------------------------------------------------------------------
// loop(): máquina de estados para Wi-Fi → GSM → MQTT
//------------------------------------------------------------------------------
void CommunicationManager::loop() {
  // -----------------------------
  // 0) Si WiFiCtrl está en “modo configuración”, hay que despachar HTTP:
  // -----------------------------
  if (wifiCtrl_.obtenerEstado() == WiFiCtrl::State::PortalActivo) {  // Atiende peticiones al servidor web (GET "/" y POST "/save")
    wifiCtrl_.handleClient();                                        // Mientras siga en PortalActivo, NO hacemos nada más en la FSM:
    return;
  }
  
  // -----------------------------
  // 1) Máquina de estados Wi-Fi → GSM → MQTT
  // -----------------------------
  switch (state_) {

    //------------------------------------
    case State::ConectandoWiFi:
      // 1) Revisamos si Wi-Fi ya está conectado (WiFiCtrl es event-driven)
      if (wifiCtrl_.obtenerEstado() == WiFiCtrl::State::Conectado) {
        Serial.println("[INFO] Wi-Fi conectado. Iniciando MQTT...");
        state_ = State::ConectandogMQTT;
        iniciaMQTT();
      }
      // 2) Si Wi-Fi falla o excede su timeout, pasamos a GSM
      else if (wifiTimedOut() ||
               wifiCtrl_.obtenerEstado() == WiFiCtrl::State::Error) {
        Serial.println("[INFO] Wi-Fi FALLÓ. Iniciando GSM...");
        state_ = State::ConectandogGSM;
        gsmStartMs_ = millis();
        iniciaGSM();
      }
      break;

    //------------------------------------
    case State::ConectandogGSM:
      // 1) Llamamos a gsm_.loop() para avanzar su FSM
      gsm_.loop();

      // 2) Si GPRS ya está conectado, iniciamos MQTT
      if (gsm_.getState() == GSM::State::Conectado) {
        Serial.println("[INFO] GPRS conectado. Iniciando MQTT...");
        state_ = State::ConectandogMQTT;
        iniciaMQTT();
      }
      // 3) Si GSM falla o excede su timeout, marcamos error general
      else if (gsmTimedOut() || gsm_.getState() == GSM::State::Error) {
        Serial.println("[INFO] GPRS FALLÓ. Estado: ERROR.");
        state_ = State::Error;
      }
      break;

    //------------------------------------
    case State::ConectandogMQTT:
      // 1) Dejamos que mqttMgr_.loop() maneje su FSM interna (conexión, reconexión, callbacks)
      mqttMgr_.loop();

      // 2) Si MQTT ya está conectado al broker, pasamos a Conectado
      if (mqttMgr_.getEstado() == MqttManager::Estado::CONECTADO) {
        Serial.println("[INFO] MQTT conectado (broker). Sistema listo.");
        state_ = State::Conectado;
      }
      // 3) Si en cualquier punto se pierde toda la red (ni Wi-Fi ni GPRS), reintentar Wi-Fi
      else if (!redHabilitada()) {
        Serial.println("[INFO] Se perdió conexión de red; reintentando Wi-Fi...");
        state_ = State::ConectandoWiFi;
        wifiStartMs_ = millis();
        iniciarWiFi();
      }
      break;

    //------------------------------------
    case State::Conectado:
      // 1) Mantenemos Wi-Fi y GPRS (WiFiCtrl es event-driven, gsm_.loop() chequea GPRS)
      //    además, verificamos si la red sigue disponible:
      if (!redHabilitada()) {
        Serial.println("[INFO] Se perdió red. Reiniciando ciclo de conexión...");
        state_ = State::ConectandoWiFi;
        wifiStartMs_ = millis();
        iniciarWiFi();
      }
      // 2) Si todo OK, mantenemos la conexión MQTT viva
      else {
        mqttMgr_.loop();
      }
      break;

    //------------------------------------
    case State::Error:
      // Estado de error terminal. Se queda aquí hasta reinicio manual externo
      break;

    default:
      break;
  }
}

//------------------------------------------------------------------------------
// publicar(): envía un mensaje por MQTT solo si estamos en estado Conectado
//------------------------------------------------------------------------------
void CommunicationManager::publicar(const char* topic, const char* payload) {
  if (state_ == State::Conectado) {
    mqttMgr_.publish(topic, payload);
  }
}

//------------------------------------------------------------------------------
// obtenerEstado(): devuelve el estado de la FSM de CommunicationManager
//------------------------------------------------------------------------------
CommunicationManager::State CommunicationManager::obtenerEstado() const {
  return state_;
}

//------------------------------------------------------------------------------
// redHabilitada(): true si Wi-Fi o GPRS está conectado
//------------------------------------------------------------------------------
bool CommunicationManager::redHabilitada() const {
  return (wifiCtrl_.obtenerEstado() == WiFiCtrl::State::Conectado)
      || (gsm_.getState()            == GSM::State::Conectado);
}

//------------------------------------------------------------------------------
// iniciarWiFi(): lanza la conexión Wi-Fi usando WiFiCtrl (portal cautivo incluido)
//------------------------------------------------------------------------------
void CommunicationManager::iniciarWiFi() {
  Serial.println("[INFO] Intentando conectar por Wi-Fi.");
  wifiCtrl_.iniciar();
}

//------------------------------------------------------------------------------
// iniciaGSM(): arranca el FSM de GSM/GPRS
//------------------------------------------------------------------------------
void CommunicationManager::iniciaGSM() {
  Serial.println("[INFO] Intentando conectar por GSM (GPRS).");
  gsm_.begin();
}

//------------------------------------------------------------------------------
// iniciaMQTT(): llama a mqttMgr_.begin(...) con los parámetros recibidos
//------------------------------------------------------------------------------
void CommunicationManager::iniciaMQTT() {
  mqttMgr_.begin(mqttHost_, mqttPort_, mqttTopic_, mqttUser_, mqttPass_);
}

//------------------------------------------------------------------------------
// wifiTimedOut(): true si excedimos el timeout para Wi-Fi
//------------------------------------------------------------------------------
bool CommunicationManager::wifiTimedOut() const {
  return (millis() - wifiStartMs_) > wifiTimeoutMs_;
}

//------------------------------------------------------------------------------
// gsmTimedOut(): true si excedimos el timeout para GSM/GPRS
//------------------------------------------------------------------------------
bool CommunicationManager::gsmTimedOut() const {
  return (millis() - gsmStartMs_) > gsmTimeoutMs_;
}
