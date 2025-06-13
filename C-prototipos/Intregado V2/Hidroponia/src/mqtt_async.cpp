#include <Arduino.h>
#include <ArduinoJson.h>
#include "mqtt_async.h"
#include "comunicacion.h"

#ifdef USE_GSM
  #include "gsm_async.h"
  extern bool gsmIsConnected(); 
  #define APN       "igprs.claro.com.ar"
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
  mqttClient.setCredentials(MQTT_USER, MQTT_PASSWORD);
  mqttClient.setClientId(CLIENT_ID);
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
// *** CORRECCIÓN CRÍTICA AQUÍ: AGREGAR 'const char* topic' A LA FIRMA ***
void publicarLecturas(const char* topic, const String& mensaje) {
#ifdef USE_GSM
  if (!wifiIsConnected() && gsmIsConnected()) {
    mqttClient.publish(topic, mensaje.c_str()); // Usar el 'topic' recibido
    Serial.print(" MQTT por GSM (Topic: ");
    Serial.print(topic);
    Serial.println("):");
    Serial.println(mensaje);
  } else {
    Serial.println(" GSM no conectado.");
  }
#else
  if (_connected) {
    mqttClient.publish(topic, 1, false, mensaje.c_str()); // Usar el 'topic' recibido
    Serial.print(" MQTT por Wi-Fi (Topic: ");
    Serial.print(topic);
    Serial.println("):");
    Serial.println(mensaje);
  } else if (gsmIsConnected()) {
    mqttClient.publish(topic, 1, false, mensaje.c_str()); // Usar el 'topic' recibido
    Serial.print(" MQTT por GSM (fallback) (Topic: ");
    Serial.print(topic);
    Serial.println("):");
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
