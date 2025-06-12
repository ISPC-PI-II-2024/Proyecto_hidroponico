#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <functional>
#include <TinyGsmClient.h>   // para TinyGsmClient
#include <WiFi.h>            // para WiFiClient
#include <PubSubClient.h>    // para PubSubClient

//================================
// Métodos de la clase MqttManager
//================================
class MqttManager {
public:
  enum class Estado : uint8_t {
    DESCONECTADO,       
    CONECTANDO_RED,     
    CONECTANDO_MQTT,    
    CONECTADO           
  };

  //-----------------------------------
  // Constructor de MqttManager
  //-----------------------------------
  //  - wifiClient: cliente TCP para Wi-Fi
  //  - gsmClient:  cliente TCP para GSM (TinyGsmClient)
  //  - wifiDisponible: callback que retorna true si Wi-Fi está listo
  //  - gsmDisponible: callback que retorna true si GSM está listo
  //-----------------------------------
  MqttManager(WiFiClient& wifiClient,
              TinyGsmClient& gsmClient,
              std::function<bool()> wifiDisponible,
              std::function<bool()> gsmDisponible);

  //-----------------------------------
  // Configura el cliente MQTT y comienza la conexión
  //-----------------------------------
  //  - host: dirección del broker (IP o nombre)
  //  - port: puerto
  //  - topic: tópico de suscripción
  //  - user/pass: credenciales opcionales
  void begin(const char* host,
             uint16_t port,
             const char* topic,
             const char* user = nullptr,
             const char* pass = nullptr);

  // Debe llamarse en el loop() de Arduino para mantener conexión y procesar incoming
  void loop();

  // Publica un mensaje (en formato JSON) en el topic indicado
  void publish(const char* topic, const char* mensajeJson);

  //----------------------------------------------------------------
  // Devuelve el estado actual (en Español)
  //----------------------------------------------------------------
  Estado getEstado() const;

private:
  //-----------------------------------
  // Rutinas internas
  //-----------------------------------
  // Se encargan de cambiar de cliente según disponibilidad de red
  void conectarMqtt();
  void alConectarMqtt();      // Callback interno cuando se conecta al broker
  void alDesconectarMqtt();   // Callback interno cuando se desconecta
  void alRecibirMensaje(char* topic, unsigned char* payload, unsigned int length);

  // Clientes de red (se inicializan desde el constructor)
  WiFiClient&       wifiClient_;
  TinyGsmClient&    gsmClient_;
  PubSubClient      mqttClient_;

  // Callbacks para saber si la red está lista
  std::function<bool()> wifiDisponible_;
  std::function<bool()> gsmDisponible_;

  // Estado actual (DESCONECTADO, CONECTANDO_RED, CONECTANDO_MQTT, CONECTADO)
  Estado           estado_;

  // Parámetros de broker
  const char*      host_;
  uint16_t         port_;
  const char*      topic_;
  const char*      user_;
  const char*      pass_;

  // Para reintentos de conexión
  unsigned long    ultimoIntentoMs_;
  unsigned long    intervaloReconexionMs_;

  // El espacio de Preferences para lectura/actualización de umbrales/valores
  Preferences      prefs_;
};

#endif
