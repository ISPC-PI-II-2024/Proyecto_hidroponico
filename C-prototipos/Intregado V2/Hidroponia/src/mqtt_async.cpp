#include <Arduino.h>
#include <ArduinoJson.h>
#include "mqtt_async.h"

#ifdef USE_GSM
  #include "gsm_async.h"
  extern bool gsmIsConnected(); 
  #define APN       "tu_apn"
  #define GPRS_USER ""
  #define GPRS_PASS ""
  #include <PubSubClient.h>
#else
  #include "gsm_async.h"
  #include "wifi_async.h"
  #include <freertos/FreeRTOS.h>
  #include <freertos/timers.h>
#endif

// ------------------------------------------------------------------
//                  Instancia del cliente MQTT
// ------------------------------------------------------------------

#ifdef USE_GSM
static PubSubClient mqttClient(gsmGetClient());
#else
static AsyncMqttClient mqttClient;
static TimerHandle_t mqttReconnectTimer;
static bool _connected = false;
#endif

// ------------------------------------------------------------------
//                    Callbacks MQTT
// ------------------------------------------------------------------

#ifndef USE_GSM
static void onMqttConnect(bool sessionPresent) {
  Serial.println("MQTT conectado");
  _connected = true;
  mqttConectado = true;  //  se agregó para que se publique correctamente
}



static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.print(" MQTT desconectado. Código: ");
  Serial.println(static_cast<int>(reason));  // 1: Timeout, 2: DNS, 4: TCP refused
  _connected = false;
  mqttConectado = false;
  if (wifiIsConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}



static void connectToMqtt(TimerHandle_t xTimer) {
  Serial.println(" Ejecutando connectToMqtt(): llamando a mqttClient.connect()");
  mqttClient.connect();
}


#endif

// ------------------------------------------------------------------
//                    API pública de mqtt_async
// ------------------------------------------------------------------

bool mqttConectado = false;

void mqttSetup(const char* server, uint16_t port) {
#ifdef USE_GSM
  mqttClient.setServer(server, port);
#else
  mqttReconnectTimer = xTimerCreate(
    "mqttTimer",
    pdMS_TO_TICKS(5000),
    pdFALSE,
    nullptr,
    connectToMqtt
  );
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.setServer(server, port);
#endif
}

void mqttStart() {
  _connected = false;  // Asegura que empezamos con estado limpio
  
  // Verificación directa del estado de WiFi
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Wi-Fi OK. Iniciando conexión MQTT...");
    
    // Inicia el timer de reconexión MQTT
    xTimerStart(mqttReconnectTimer, 0);
    
    // O alternativamente, conecta directamente sin usar el timer:
    // mqttClient.connect();
  } else {
    Serial.println(" MQTT no iniciado: Wi-Fi desconectado.");
  }
}



void mqttLoop() {
#ifdef USE_GSM
  mqttClient.loop();
#endif
}

void mqttSetCallback(MqttMessageCallback callback) {
#ifdef USE_GSM
  mqttClient.setCallback([callback](char* topic, byte* payload, unsigned int length) {
    AsyncMqttClientMessageProperties props{};
    callback(topic, (char*)payload, props, length, 0, length);
  });
#else
  mqttClient.onMessage(callback);
#endif
}

bool mqttPublishJson(const char* topic,
                     const JsonDocument& doc,
                     uint8_t qos,
                     bool retain) {
  char buf[512];
  size_t len = serializeJson(doc, buf, sizeof(buf));
#ifdef USE_GSM
  return mqttClient.publish(topic, buf, len);
#else
  auto packetId = mqttClient.publish(topic, qos, retain, buf, len);
  return packetId != 0;
#endif
}

bool mqttSubscribe(const char* topic, uint8_t qos) {
#ifdef USE_GSM
  return mqttClient.subscribe(topic, qos);
#else
  auto packetId = mqttClient.subscribe(topic, qos);
  return packetId != 0;
#endif
}

// ------------------------------------------------------------------
//              NUEVA FUNCIÓN para publicar un String JSON
// ------------------------------------------------------------------
void publicarLecturas(const String& mensaje) {
#ifdef USE_GSM
  if (!wifiIsConnected() && gsmIsConnected()) {
    mqttClient.publish("sensores/datos", mensaje.c_str());
    Serial.println(" MQTT por GSM:");
    Serial.println(mensaje);
  } else {
    Serial.println(" GSM no conectado.");
  }
#else
  if (_connected) {
    mqttClient.publish("sensores/datos", 1, false, mensaje.c_str());
    Serial.println(" MQTT por Wi-Fi:");
    Serial.println(mensaje);
  } else if (gsmIsConnected()) {
    mqttClient.publish("sensores/datos", 1, false, mensaje.c_str());
    Serial.println(" MQTT por GSM (fallback):");
    Serial.println(mensaje);
  } else {
    Serial.println(" MQTT no conectado (ni Wi-Fi ni GSM).");
  }
#endif
}

// ------------------------------------------------------------------
//              Función para obtener acceso al cliente
// ------------------------------------------------------------------

AsyncMqttClient& getMqttClient() {
  return mqttClient;
}