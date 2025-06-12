#ifndef MQTT_ASYNC_H
#define MQTT_ASYNC_H

#include <ArduinoJson.h>
#include <AsyncMqttClient.h>

// Tipo de función para recibir mensajes MQTT
typedef void (*MqttMessageCallback)(char* topic,
                                    char* payload,
                                    AsyncMqttClientMessageProperties properties,
                                    size_t len, size_t index, size_t total);

// Configuración y control MQTT
void mqttSetup(const char* server, uint16_t port);
void mqttStart();
void mqttLoop();
void mqttSetCallback(MqttMessageCallback callback);
bool mqttSubscribe(const char* topic, uint8_t qos);
bool mqttPublishJson(const char* topic, const JsonDocument& doc, uint8_t qos = 0, bool retain = false);
void publicarLecturas(const String& mensaje);

// Acceso al cliente MQTT
AsyncMqttClient& getMqttClient();

// Estado de conexión
extern bool mqttConectado;

#endif
