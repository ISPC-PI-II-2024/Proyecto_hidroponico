// MqttManager.cpp
#include "MqttManager.h"
#include <Arduino.h>

MqttManager::MqttManager(AsyncMqttClient& mqttClient,
                         std::function<bool()> wifiAvailable,
                         std::function<bool()> gsmAvailable)
  : mqtt_(mqttClient)
  , wifiAvailable_(wifiAvailable)
  , gsmAvailable_(gsmAvailable)
  , state_(State::DISCONNECTED)
  , host_(nullptr)
  , port_(0)
  , user_(nullptr)
  , pass_(nullptr)
{
  // Callbacks asíncronos
  mqtt_.onConnect(   [this](bool session) { onMqttConnect(session); });
  mqtt_.onDisconnect([this](AsyncMqttClientDisconnectReason r) { onMqttDisconnect(r); });
  mqtt_.onMessage(   [this](char* t, char* p, AsyncMqttClientMessageProperties props,
                             size_t len, size_t idx, size_t total){
    onMqttMessage(t, p, props, len, idx, total);
  });
}

void MqttManager::begin(const char* host, uint16_t port,
                         const char* user, const char* pass) {
  host_ = host;
  port_ = port;
  user_ = user;
  pass_ = pass;
  state_ = State::CONNECT_NETWORK;
}

void MqttManager::loop() {
  switch (state_) {
    case State::DISCONNECTED:
      state_ = State::CONNECT_NETWORK;
      break;

    case State::CONNECT_NETWORK:
      // Espera a que haya red (Wi-Fi o GSM)
      if (wifiAvailable_() || gsmAvailable_()) {
        state_ = State::CONNECT_MQTT;
        connectMqtt();
      }
      break;

    case State::CONNECT_MQTT:
      // AsyncMqttClient maneja conexión internamente
      break;

    case State::CONNECTED:
      mqtt_.loop(); // keep-alive y callbacks
      break;
  }
}

void MqttManager::connectMqtt() {
  mqtt_.setServer(host_, port_);
  if (user_) mqtt_.setCredentials(user_, pass_);
  mqtt_.connect();
}

void MqttManager::onMqttConnect(bool sessionPresent) {
  Serial.println("[MQTT] Conectado");
  state_ = State::CONNECTED;
  // Ejemplo de suscripción:
  mqtt_.subscribe("home/sensor/data", 1);
}

void MqttManager::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.printf("[MQTT] Desconectado (razón=%d)\n", (int)reason);
  state_ = State::DISCONNECTED;
}

void MqttManager::onMqttMessage(char* topic,
                                char* payload,
                                AsyncMqttClientMessageProperties props,
                                size_t len,
                                size_t index,
                                size_t total) {
  Serial.printf("[MQTT] Mensaje en %s: %.*s\n", topic, len, payload);
}

void MqttManager::publish(const char* topic,
                           const char* payload,
                           uint8_t qos,
                           bool retain) {
  if (state_ == State::CONNECTED) {
    mqtt_.publish(topic, qos, retain, payload);
  }
}
