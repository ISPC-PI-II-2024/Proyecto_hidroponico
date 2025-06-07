#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "SensorManager.h"
#include "ActuadorManager.h"
#include "CommunicationManager.h"
#include "ConfigHardware.h"

// --- Pines traidos desde ConfigHardware.h ---

// ANSI colors (para seriales compatibles como PlatformIO)
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"

// --- Topicos ---
const char* TOPIC_TELEMETRIA = "mediciones/dispositivo";
const char* TOPIC_ALARMAS    = "alarmas/dispositivo";

// --- Variables de configuración ---
String topicInfo, topicLectura, topicAlarma;
Preferences prefs;
String ssid, pass, broker, topic, deviceName, mqttUser, mqttPass;
uint16_t port;
uint32_t intervaloPublicacion = 60000; // Default: 1 min, modificable via pref.

// --- Instancias globales (como punteros) ---
CommunicationManager* commMgr = nullptr;
SensorManager* sensorMgr = nullptr;
ActuadorManager* actuadorMgr = nullptr;

// --- Estado para detectar cambios de alarma ---
int nivelAlarmaAnterior = 0;

// --- declaración de funciones ---
void enviarJsonCompuesto();
void enviarAlarma(int nivel);
void imprimirEstadoDispositivo();

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
    topic                = prefs.getString("topic", TOPIC_TELEMETRIA);
    deviceName           = prefs.getString("deviceName", "ESP32");
    intervaloPublicacion = prefs.getUInt("intervaloPublicacion", 60000);
    mqttUser             = prefs.getString("mqttUser", "");
    mqttPass             = prefs.getString("mqttPass", "");
    topicInfo            = prefs.getString("topicInfo", "info/default");
    topicLectura         = prefs.getString("topicLectura", "lecturas/default");
    topicAlarma          = prefs.getString("topicAlarma", "alarmas/default");
    prefs.end();

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    // --- crear instancias de managers --- 
    commMgr = new CommunicationManager(
        Serial, Serial2, Serial1,
        ssid.c_str(), pass.c_str(),
        broker.c_str(), port,
        topic.c_str(), mqttUser.c_str(), mqttPass.c_str(),
        10000, 10000
    );

    sensorMgr = new SensorManager(
        PIN_I2C_SDA, PIN_I2C_SCL,
        PIN_DHT11,
        PIN_BH1750_SDA, PIN_BH1750_SCL,
        PIN_HCSR04_TRIG, PIN_HCSR04_ECHO,
        PIN_CAUDALIMETRO, 450.0,
        PIN_GAS
    );

    actuadorMgr = new ActuadorManager(
        PIN_BUZZER, PIN_RELE_BOMBA, PIN_LED_VERDE, PIN_LED_ROJO,
        *sensorMgr, *commMgr, TOPIC_ALARMAS
    );

    // --- Inicializar managers ---
    sensorMgr->comenzar();
    actuadorMgr->comenzar();
    commMgr->begin();

    // --- setear estado inicial de alarma ---
    nivelAlarmaAnterior = actuadorMgr->getNivelAlarma();

    enviarInformacionDispositivo();
    Serial.println("[INFO] Setup completo.");
}


// -----------------------------------
// LOOP
// -----------------------------------
void loop() {
    static unsigned long lastPub = 0;

    sensorMgr->loop();
    actuadorMgr->loop();
    commMgr->loop();

    unsigned long now = millis();
    if (now - lastPub > intervaloPublicacion) {
        lastPub = now;
        enviarJsonCompuesto();
        imprimirEstadoDispositivo();
    }

    int nivelAlarmaActual = actuadorMgr->getNivelAlarma();
    if (nivelAlarmaActual != nivelAlarmaAnterior && nivelAlarmaActual != 0) {
        enviarAlarma(nivelAlarmaActual);
        nivelAlarmaAnterior = nivelAlarmaActual;
    }
}

// -----------------------------------
// Armado y envío del JSON compuesto por MQTT
// -----------------------------------
void enviarJsonCompuesto() {
    DynamicJsonDocument docFinal(1536);
    String sensoresStr = sensorMgr->obtenerJson();
    DynamicJsonDocument docSensores(1024);
    DeserializationError error = deserializeJson(docSensores, sensoresStr);

    if (error) {
        docFinal["device"] = deviceName;
        docFinal["timestamp"] = millis();
        docFinal["sensores"]["error"] = "No data";
    } else {
        docFinal["timestamp"] = docSensores["timestamp"];
        docFinal["device"]    = deviceName;
        docFinal["sensores"]  = docSensores.as<JsonObject>();
        docFinal["sensores"].remove("timestamp");
    }

    JsonObject actuadores = docFinal["actuadores"].to<JsonObject>();
    actuadores["bomba"]  = actuadorMgr->isBombaEncendida();
    actuadores["alarma"] = actuadorMgr->getNivelAlarma();

    String payload;
    serializeJson(docFinal, payload);
    commMgr->publicar(TOPIC_TELEMETRIA, payload.c_str());

    Serial.println("[INFO] Payload publicado:");
    Serial.println(payload);
}

// -----------------------------------
// Envío inmediato de mensaje de alarma por MQTT
// -----------------------------------
void enviarAlarma(int nivel) {
    DynamicJsonDocument payload(256);
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
    commMgr->publicar(TOPIC_ALARMAS, out.c_str());
    Serial.println("[INFO] Alarma publicada:");
    Serial.println(out);
}

// -----------------------------------
void imprimirEstadoDispositivo() {
    Serial.println(YELLOW "\n========= ESTADO DEL DISPOSITIVO =========" RESET);

    String jsonSensores = sensorMgr->obtenerJson();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, jsonSensores);

    const char* sensores[] = { "BMP280", "DHT11", "BH1750", "HC-SR04", "Caudal", "CO2", "Energia" };
    for (const char* s : sensores) {
        const char* estado = doc[s]["estado"] | "desconocido";
        const char* color  = strcmp(estado, "OK") == 0 ? GREEN : RED;
        Serial.printf("%s[SENSOR]  %-9s: %s%s\n", color, s, estado, RESET);
    }

    Serial.printf("%s[ACTUADOR] Bomba      : %s%s\n", 
        actuadorMgr->isBombaEncendida() ? GREEN : RED,
        actuadorMgr->isBombaEncendida() ? "ON" : "OFF",
        RESET);

    Serial.printf("%s[ACTUADOR] Alarma     : Nivel %d%s\n", 
        actuadorMgr->getNivelAlarma() > 0 ? RED : GREEN,
        actuadorMgr->getNivelAlarma(),
        RESET);

    Serial.printf("%s[COMMS]    Wi-Fi      : %s%s\n",
        commMgr->redHabilitada() ? GREEN : RED,
        commMgr->redHabilitada() ? "CONECTADO" : "DESCONECTADO",
        RESET);

    // MQTT y GSM son privados → podés expandir CommunicationManager con funciones públicas si querés:
    // Por ahora asumimos solo `redHabilitada()` como resumen de ambos.

    Serial.println(YELLOW "============================================\n" RESET);
    }
void enviarInformacionDispositivo() {
    DynamicJsonDocument doc(1024);
    doc["device"] = deviceName;
    doc["evento"] = "inicio";

    JsonArray sensores = doc.createNestedArray("sensores");
    JsonObject s1 = sensores.createNestedObject();
    s1["nombre"] = "DHT11";
    s1["pin"] = PIN_DHT11;
    s1["umbralTempMax"] = prefs.getFloat("umbralTempMax", 35.0);
    s1["umbralHumMin"] = prefs.getFloat("umbralHumMin", 30.0);

    JsonObject s2 = sensores.createNestedObject();
    s2["nombre"] = "BMP280";
    s2["pin"] = -1;
    s2["umbralPresionMin"] = prefs.getFloat("umbralPresionMin", 950.0);

    JsonArray actuadores = doc.createNestedArray("actuadores");
    JsonObject a1 = actuadores.createNestedObject();
    a1["nombre"] = "BOMBA";
    a1["pin"] = PIN_RELE_BOMBA;
    a1["estado"] = actuadorMgr->isBombaEncendida() ? "ON" : "OFF";

    String out;
    serializeJson(doc, out);
    commMgr->publicar(topicInfo.c_str(), out.c_str());

    Serial.println("[INFO] Mensaje de inicio publicado:");
    Serial.println(out);
}
