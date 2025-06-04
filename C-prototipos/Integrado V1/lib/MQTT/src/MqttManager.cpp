#include "MqttManager.h"
#include <ArduinoJson.h>

//================================
// Implementación de MqttManager
//================================

//-----------------------------------
// Constructor: inicializa estado en DESCONECTADO
//-----------------------------------
MqttManager::MqttManager(WiFiClient& wifiClient,
                         TinyGsmClient& gsmClient,
                         std::function<bool()> wifiDisponible,
                         std::function<bool()> gsmDisponible)
 : wifiClient_(wifiClient)
 , gsmClient_(gsmClient)
 , wifiDisponible_(wifiDisponible)
 , gsmDisponible_(gsmDisponible)
 , mqttClient_(wifiClient_)           // inicialmente apuntamos a WiFiClient
 , estado_(Estado::DESCONECTADO)
 , host_(nullptr)
 , port_(0)
 , topic_(nullptr)
 , user_(nullptr)
 , pass_(nullptr)
 , ultimoIntentoMs_(0)
 , intervaloReconexionMs_(5000)
{
  // Configuramos callback para cuando llegue mensaje MQTT
  mqttClient_.setCallback([this](char* t, unsigned char* p, unsigned int len){
    alRecibirMensaje(t, p, len);
  });
}

//----------------------------------------------------------------
// Inicialización y parámetros de broker (se llama en setup())
//----------------------------------------------------------------
void MqttManager::begin(const char* host,
                        uint16_t port,
                        const char* topic,
                        const char* user,
                        const char* pass) {
  host_  = host;
  port_  = port;
  topic_ = topic;
  user_  = user;
  pass_  = pass;

  // Abrimos el espacio "config" en Preferences (read/write)
  // Si falla, prefs_.begin() retorna false, pero por simplicidad no lo chequeamos aquí
  prefs_.begin("config", false);

  estado_ = Estado::CONECTANDO_RED;   // Pasamos a estado de intentar conexión de red
  ultimoIntentoMs_ = 0;               // Forzar intento inmediato en loop()
}

//----------------------------------------------------------------
// Mantiene la lógica de conexión y procesa JSON entrante
//----------------------------------------------------------------
void MqttManager::loop() {
  switch (estado_) {
    case Estado::DESCONECTADO:
      estado_ = Estado::CONECTANDO_RED;
      ultimoIntentoMs_ = 0;
      break;

    case Estado::CONECTANDO_RED:
      if (wifiDisponible_()) {
        // Si WiFi está listo, usamos WiFiClient
        mqttClient_.setClient(wifiClient_);
        estado_ = Estado::CONECTANDO_MQTT;
      } else if (gsmDisponible_()) {
        // Si GSM está listo, usamos TinyGsmClient
        mqttClient_.setClient(gsmClient_);
        estado_ = Estado::CONECTANDO_MQTT;
      }
      break;

    case Estado::CONECTANDO_MQTT:
      mqttClient_.setServer(host_, port_);
      if (user_ && pass_) {
        if (mqttClient_.connect("clientID", user_, pass_)) {
          estado_ = Estado::CONECTADO;
          alConectarMqtt();
        }
      } else {
        if (mqttClient_.connect("clientID")) {
          estado_ = Estado::CONECTADO;
          alConectarMqtt();
        }
      }
      if (estado_ != Estado::CONECTADO) {
        // No conectado al broker, esperamos y reintentar
        unsigned long ahora = millis();
        if (ahora - ultimoIntentoMs_ > intervaloReconexionMs_) {
          ultimoIntentoMs_ = ahora;
          // Se quedará en este estado hasta reconectar
        }
      }
      break;

    case Estado::CONECTADO:
      mqttClient_.loop();
      break;
  }
}

//----------------------------------------------------------------
// Callback interno: al conectar al broker
//----------------------------------------------------------------
void MqttManager::alConectarMqtt() {
  Serial.println("[INFO] [MQTT] Conectado al broker");
  // Nos suscribimos al tópico configurado
  if (topic_) {
    mqttClient_.subscribe(topic_);
  }
}

//----------------------------------------------------------------
// Callback interno: al desconectar del broker
//----------------------------------------------------------------
void MqttManager::alDesconectarMqtt() {
  Serial.println("[INFO] [MQTT] Desconectado del broker");
  estado_ = Estado::DESCONECTADO;
}

//----------------------------------------------------------------
// Envía un mensaje JSON (char*) a un topic
//----------------------------------------------------------------
void MqttManager::publish(const char* topic, const char* mensajeJson) {
  if (estado_ == Estado::CONECTADO) {
    mqttClient_.publish(topic, mensajeJson);
  } else {
    Serial.println("[ERROR] [MQTT] No conectado, no se puede publicar");
  }
}

//----------------------------------------------------------------
// Devuelve el estado actual (en Español)
//----------------------------------------------------------------
MqttManager::Estado MqttManager::getEstado() const {
  return estado_;
}

//----------------------------------------------------------------
// Callback interno: al recibir un mensaje en el tópico suscrito
//----------------------------------------------------------------
void MqttManager::alRecibirMensaje(char* topic, unsigned char* payload, unsigned int length) {
  // 1) Convertir payload a String
  String mensaje;
  mensaje.reserve(length + 1);
  for (unsigned int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  Serial.printf("[INFO] [MQTT] Mensaje recibido en %s: %s\n", topic, mensaje.c_str());

  // 2) Parsear JSON
  StaticJsonDocument<512> doc;  // Se aumentó a 512 en caso de muchos pares clave/valor
  DeserializationError error = deserializeJson(doc, mensaje);
  if (error) {
    Serial.println("[INFO] [MQTT] Error al parsear JSON");
    return;  // Salimos si el JSON no es válido
  }

  // 3) Iterar sobre cada clave en el JSON y actualizar Preferences
  JsonObject obj = doc.as<JsonObject>();
  bool huboError = false;

  for (JsonPair kv : obj) {
    const char* clave = kv.key().c_str();
    // 3.a) Verificar si la clave existe en Preferences
    size_t longitud = prefs_.getBytesLength(clave);
    if (longitud == 0) {
      Serial.printf("[ERROR] [MQTT] ERROR: clave '%s' no encontrada en Preferences\n", clave);
      huboError = true;
      continue;
    }

    // 3.b) Actualizar según el tipo de valor en JSON
    if (kv.value().is<int>()) {
      int nuevoInt = kv.value().as<int>();
      prefs_.putInt(clave, nuevoInt);
      Serial.printf("[INFO] [MQTT] Se actualizó '%s' = %d\n", clave, nuevoInt);

    } else if (kv.value().is<float>()) {
      float nuevoFloat = kv.value().as<float>();
      prefs_.putFloat(clave, nuevoFloat);
      Serial.printf("[INFO] [MQTT] Se actualizó '%s' = %f\n", clave, nuevoFloat);

    } else if (kv.value().is<const char*>()) {
      const char* nuevoStr = kv.value().as<const char*>();
      prefs_.putString(clave, nuevoStr);
      Serial.printf("[INFO] [MQTT] Se actualizó '%s' = \"%s\"\n", clave, nuevoStr);

    } else {
      Serial.printf("[ERROR] [MQTT] ERROR: tipo de valor no soportado para clave '%s'\n", clave);
      huboError = true;
    }
  }

  // 4) Informar resultado global por Serial (u opcionalmente por MQTT si se desea)
  if (!huboError) {
    Serial.println("[INFO] [MQTT] Preferencias actualizadas correctamente");
  } else {
    Serial.println("[ERROR] [MQTT] Hubo errores al actualizar algunas preferencias");
  }
}
