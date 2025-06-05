#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "SensorManager.h"
#include "ActuadorManager.h"
#include "CommunicationManager.h"

//----------------------------------------
// GLOSARIO DE PINES ESP32 (centralizado)
//----------------------------------------

// Sensores analógicos
#define PIN_LM35           36
#define PIN_PH             34
#define PIN_EC             35
#define PIN_CORRIENTE      32
#define PIN_GAS            39

// Sensores digitales / 1-Wire / PWM / IN
#define PIN_DHT11          4
#define PIN_DS18B20        5
#define PIN_FLOTADOR       14
#define PIN_HCSR04_TRIG    12
#define PIN_HCSR04_ECHO    13
#define PIN_RELE_BOMBA     25
#define PIN_CAUDALIMETRO   26
#define PIN_ENERGIA        33
#define PIN_BUZZER         27
#define PIN_LLUVIA         18

// Sensores I2C
#define PIN_I2C_SDA        21
#define PIN_I2C_SCL        22

#define PIN_BH1750_SDA     PIN_I2C_SDA
#define PIN_BH1750_SCL     PIN_I2C_SCL
#define PIN_BME280_SDA     PIN_I2C_SDA
#define PIN_BME280_SCL     PIN_I2C_SCL
#define PIN_RTC_SDA        PIN_I2C_SDA
#define PIN_RTC_SCL        PIN_I2C_SCL

#define PIN_LED_VERDE      2
#define PIN_LED_ROJO       15

//----------------------------------------
// FIN GLOSARIO | INICIO VARIABLES
//----------------------------------------

// --- Variables de configuración ---
Preferences prefs;
String ssid, pass, broker, topic, deviceName, mqttUser, mqttPass;
uint16_t port;
uint32_t intervaloPublicacion = 60000; // Default: 1 min, modificable via pref.

// --- Instancias globales (ajustar si tu constructor requiere más/menos argumentos) ---
SensorManager sensorMgr(
    PIN_I2C_SDA, PIN_I2C_SCL,
    PIN_DHT11,
    PIN_BH1750_SDA, PIN_BH1750_SCL,
    PIN_HCSR04_TRIG, PIN_HCSR04_ECHO,
    PIN_CAUDALIMETRO, 450.0,            // factor caudal default
    PIN_GAS                             // Sensor de gas (MQ-2, MQ-7, etc.)
);

// ActuadorManager(pinBuzzer, refSensorMgr, refCommMgr, topic:Alarmas)
ActuadorManager actuadorMgr(
    PIN_BUZZER,
    sensorMgr,
    "alarmas/dispositivo"
);

// 
// Ajusta según el hardware. Tengo que verificarlo en el ESP
CommunicationManager commMgr(
    Serial, Serial2, Serial1,  // Consola, GSM, WiFi debug
    nullptr, nullptr,          // ssid, pass: se cargarán en begin()
    nullptr, 0, nullptr,       // broker, port, topic
    nullptr, nullptr,          // user, pass
    10000, 10000               // Timeouts WiFi, GSM
);

// --- Estado para detectar cambios de alarma ---
int nivelAlarmaAnterior = 0;  // Arranca por defecto sin alarmas.

// -----------------------------------
// SETUP
// -----------------------------------
void setup() {
    Serial.begin(115200);

    // --- Cargar configuración desde Preferences ---
    prefs.begin("config", true);
    ssid                 = prefs.getString("ssid", "");
    pass                 = prefs.getString("pass", "");
    broker               = prefs.getString("broker", "");
    port                 = prefs.getUInt("port", 1883);
    topic                = prefs.getString("topic", "mediciones/dispositivo");
    deviceName           = prefs.getString("deviceName", "ESP32");
    intervaloPublicacion = prefs.getUInt("intervaloPublicacion", 60000);
    // // De momento no se usa, pero se va a sumar al portal cautivo luego de testearlo
    mqttUser             = prefs.getString("mqttUser", "");
    mqttPass             = prefs.getString("mqttPass", "");
    prefs.end();

    // --- Inicializar bus I2C SOLO UNA VEZ --- (Esto para que todo lo que utiliza I2C va por este tramo.)
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    // --- Inicializar managers ---
    sensorMgr.comenzar();
    actuadorMgr.comenzar();

    // --- Inicializar comunicación (ahora con config cargada) ---
    commMgr.setConfig(
        ssid.c_str(), pass.c_str(),
        broker.c_str(), port,
        topic.c_str(), mqttUser.c_str(), mqttPass.c_str()
    );
    commMgr.begin();

    // Inicializar estado anterior de alarma
    nivelAlarmaAnterior = actuadorMgr.getNivelAlarma();

    Serial.println("[INFO] Setup completo.");
}

// -----------------------------------
// LOOP
// -----------------------------------
void loop() {
    static unsigned long lastPub = 0;

    // Ejecutar lógica de cada manager
    sensorMgr.loop();
    actuadorMgr.loop();
    commMgr.loop();

    // --- Publicar medición completa cada intervalo ---
    unsigned long now = millis();
    if (now - lastPub > intervaloPublicacion) {
        lastPub = now;
        enviarJsonCompuesto();
    }

    // --- Enviar notificación inmediata ante cambio de alarma/umbral crítico ---
    int nivelAlarmaActual = actuadorMgr.getNivelAlarma();
    if (nivelAlarmaActual != nivelAlarmaAnterior && nivelAlarmaActual != 0) {
        enviarAlarma(nivelAlarmaActual);
        nivelAlarmaAnterior = nivelAlarmaActual;
    }
}

// -----------------------------------
// Armado y envío del JSON compuesto por MQTT
// -----------------------------------
void enviarJsonCompuesto() {
    StaticJsonDocument<1536> docFinal;
    // --- Obtener JSON sensores y parsear ---
    String sensoresStr = sensorMgr.obtenerJson();
    StaticJsonDocument<1024> docSensores;
    DeserializationError error = deserializeJson(docSensores, sensoresStr);
    if (error) {
        docFinal["device"] = deviceName;
        docFinal["timestamp"] = millis();
        docFinal["sensores"]["error"] = "No data";
    } else {
        // Timestamp en root
        docFinal["timestamp"] = docSensores["timestamp"];
        docFinal["device"]    = deviceName;
        // Pasar todos los sensores como subobjeto
        docFinal["sensores"]  = docSensores;
        docFinal["sensores"].remove("timestamp");
    }

    // --- Estado de actuadores (puedes ampliar con más getters) ---
    JsonObject actuadores = docFinal.createNestedObject("actuadores");
    actuadores["bomba"]  = actuadorMgr.isBombaEncendida();
    actuadores["alarma"] = actuadorMgr.getNivelAlarma();
   // Si agregamos algun otro metdodo como luces, o  similar, lo podemos poner aca.

    // --- Serializar y enviar por MQTT ---
    String payload;
    serializeJson(docFinal, payload);
    commMgr.publicar(topic.c_str(), payload.c_str());

    Serial.println("[INFO] Payload publicado:");
    Serial.println(payload);
}

// -----------------------------------
// Envío inmediato de mensaje de alarma por MQTT
// -----------------------------------
void enviarAlarma(int nivel) {
    StaticJsonDocument<256> payload;
    payload["device"]  = deviceName;
    payload["tipo"]    = "alarma";
    payload["nivel"]   = nivel;
    switch (nivel) {
        case 3: payload["mensaje"] = "Alarma CRITICA"; break;
        case 2: payload["mensaje"] = "Alarma MEDIA";   break;
        case 1: payload["mensaje"] = "Alarma BAJA";    break;
        default: payload["mensaje"] = "Alarma desconocida"; break;
    }
    String out;
    serializeJson(payload, out);
    commMgr.publicar("alarmas/dispositivo", out.c_str());
    Serial.println("[INFO] Alarma publicada:");
    Serial.println(out);
}
