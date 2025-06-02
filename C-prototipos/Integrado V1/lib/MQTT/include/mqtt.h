// MqttManager.h
#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <AsyncMqttClient.h>
#include <WiFi.h>
#include <TinyGsmClient.h>
#include <functional>

class MqttManager {
public:
  enum class State { DISCONNECTED, CONNECT_NETWORK, CONNECT_MQTT, CONNECTED };

  MqttManager(AsyncMqttClient& mqttClient,
              std::function<bool()> wifiAvailable,
              std::function<bool()> gsmAvailable);

  // Inicializa parámetros de broker y pasa a estado de conexión
  void begin(const char* host, uint16_t port,
             const char* user = nullptr, const char* pass = nullptr);

  // Debe llamarse desde loop()
  void loop();

  // Publicar mensaje (no bloqueante)
  void publish(const char* topic, const char* payload,
               uint8_t qos = 0, bool retain = false);

private:
  AsyncMqttClient&            mqtt_;
  std::function<bool()>       wifiAvailable_;
  std::function<bool()>       gsmAvailable_;
  State                       state_;

  // Broker params
  const char* host_;
  uint16_t    port_;
  const char* user_;
  const char* pass_;

  void connectMqtt();
  void onMqttConnect(bool sessionPresent);
  void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
  void onMqttMessage(char* topic,
                     char* payload,
                     AsyncMqttClientMessageProperties props,
                     size_t len,
                     size_t index,
                     size_t total);
};

#endif // MQTTMANAGER_H