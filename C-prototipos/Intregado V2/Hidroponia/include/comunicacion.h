#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <WiFi.h>
#include <AsyncMqttClient.h>
#include <LoRa.h>
#include <BLEDevice.h>

// Variables para activar/desactivar comunicación
extern bool wifiActivo;
extern bool loRaActivo;
extern bool bleActivo;

// Definiciones de conexión WiFi
#define WIFI_SSID "DZS_5380"
#define WIFI_PASSWORD "dzsi123456789"

//#define WIFI_SSID "redmi"
//#define WIFI_PASSWORD "12345678"

/// Configuración MQTT
#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_PORT 1883

//#define MQTT_BROKER "test.mosquitto.org"
//#define MQTT_PORT 1883
#define CLIENT_ID "ESP32_Hidroponico"

// Cliente MQTT
extern AsyncMqttClient mqttClient;
extern bool mqttConectado;

// Inicialización de comunicación
void conectarWiFi();
void conectarLoRa();
void conectarBLEMesh();
void manejarMQTT();
void enviarDatosMQTT();
void enviarDatosLoRa();
void recibirDatosLoRa();
void enviarDatosBLE();
void recibirDatosBLE();
void enviarDatos();

// Callback de recepción MQTT
void recibirMensajeMQTT(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

#endif
