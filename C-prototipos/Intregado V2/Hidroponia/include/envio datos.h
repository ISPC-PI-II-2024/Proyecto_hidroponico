#ifndef ENVIO_DATOS_H
#define ENVIO_DATOS_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensores.h"
#include "wifi_async.h"
#include "gsm_async.h"
#include "control.h"

// Tamaño del JSON para envío de datos
#define JSON_BUFFER_SIZE 512

// Configuración de MQTT
#define MQTT_SERVER "mqtt.miservidor.com"
#define MQTT_PORT 1883
#define MQTT_USER "usuario_mqtt"
#define MQTT_PASSWORD "clave_mqtt"
#define MQTT_CLIENT_ID "hidroponico_1"
#define MQTT_TOPIC_BASE "hidroponico/sensores/"

// Configuración de HTTP
#define HTTP_ENDPOINT "https://api.miservidor.com/hidroponico/datos"
#define HTTP_API_KEY "mi_api_key_secreta"

// Tiempos de envío y reintentos (en milisegundos)
#define INTERVALO_ENVIO_DATOS 60000           // 1 minuto entre envíos
#define INTERVALO_RECONEXION_MQTT 5000        // 5 segundos entre intentos de reconexión
#define MAX_REINTENTOS_ENVIO 3                // Máximo de reintentos antes de buffering

// Tamaño del buffer para datos no enviados
#define BUFFER_MAX_MENSAJES 50

// Estados de conexión
typedef enum {
    CONEXION_NINGUNA,
    CONEXION_WIFI,
    CONEXION_GSM
} TipoConexion;

// Estados del envío
typedef enum {
    ENVIO_OK,
    ENVIO_ERROR_CONEXION,
    ENVIO_ERROR_SERVIDOR,
    ENVIO_ERROR_AUTENTICACION,
    ENVIO_ERROR_TIMEOUT
} EstadoEnvio;

// Estructura para mensaje en buffer
typedef struct {
    char jsonData[JSON_BUFFER_SIZE];
    unsigned long timestamp;
    bool enviado;
} MensajeBuffer;

// Variables externas para el estado de la conexión
extern TipoConexion conexionActual;
extern bool envioDatosHabilitado;
extern bool bufferingHabilitado;
extern int numMensajesBuffer;
extern int numErroresConsecutivos;

// Inicialización del módulo de envío de datos
void inicializarEnvioDatos();

// Tarea FreeRTOS para envío periódico de datos
void tareaEnvioDatos(void *pvParameters);

// Funciones principales
bool enviarDatosMQTT();
bool enviarDatosHTTP();
bool enviarDatosMariaDB();

// Funciones auxiliares
void agregarDatosBuffer(const char* jsonData);
bool enviarDatosBuffer();
TipoConexion verificarConexiones();
void recolectarDatosSensores(DynamicJsonDocument& jsonDoc);
void mostrarEstadoConexion();
const char* obtenerEstadoConexionTexto();
const char* obtenerEstadoEnvioTexto(EstadoEnvio estado);

#endif // ENVIO_DATOS_H