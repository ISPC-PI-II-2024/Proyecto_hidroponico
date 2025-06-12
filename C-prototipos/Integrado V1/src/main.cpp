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

// --- Función global para obtener ID corto del dispositivo ---
String idCorto() {
    uint64_t chipid = ESP.getEfuseMac();            // Obtiene el valor único de 64 bits
    char id[7];
    snprintf(id, sizeof(id), "%02X%02X%02X",        // Solo los últimos 3 bytes 
             (uint8_t)(chipid >> 16),
             (uint8_t)(chipid >> 8),
             (uint8_t)chipid);
    return String(id);
}

// ----------------------------------------------------
// ----------------------------------------------------
void cargarConfiguracion() {
    prefs.begin("config", true);
    ssid                 = prefs.getString("ssid", "");
    pass                 = prefs.getString("pass", "");
    broker               = prefs.getString("broker", "test.mosquitto.org");
    port                 = prefs.getUInt("port", 1883);
    deviceName           = prefs.getString("deviceName", "ESP32");
    intervaloPublicacion = prefs.getUInt("intervaloPublicacion", 60000);
    mqttUser             = prefs.getString("mqttUser", "");
    mqttPass             = prefs.getString("mqttPass", "");
    topicInfo            = prefs.getString("topicInfo", "info/default");
    topicLectura         = prefs.getString("topicLectura", "lecturas/default");
    topicAlarma          = prefs.getString("topicAlarma", "alarmas/default");

    umbralTempMax     = prefs.getFloat("umbralTempMax", 35.0);
    umbralHumMin      = prefs.getFloat("umbralHumMin", 30.0);
    umbralPresionMin  = prefs.getFloat("umbralPresionMin", 950.0);
    umbralLuxMin      = prefs.getFloat("umbralLuxMin", 200.0);
    umbralDistMax     = prefs.getFloat("umbralDistMax", 30.0);
    umbralCaudalMin   = prefs.getFloat("umbralCaudalMin", 1.0);
    umbralCO2Max      = prefs.getFloat("umbralCO2Max", 1000.0);
    umbralVoltMin     = prefs.getFloat("umbralVoltMin", 3.0);
    umbralCorrMin     = prefs.getFloat("umbralCorrMin", 0.05);
    prefs.end();
}

// -----------------------------------
// SETUP
// -----------------------------------
void setup() {
    Serial.begin(115200);
    void cargarConfiguracion();
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    // --- crear instancias de managers --- 
    commMgr = new CommunicationManager(
        Serial, Serial2, Serial1,
        ssid.c_str(), pass.c_str(),
        broker.c_str(), port,
        topicInfo.c_str(), topicLectura.c_str(), topicAlarma.c_str(),
        mqttUser.c_str(), mqttPass.c_str(),
        10000, 10000
    );

    sensorMgr = new SensorManager(
        PIN_I2C_SDA, PIN_I2C_SCL,
        PIN_DHT11,
        PIN_BH1750_SDA, PIN_BH1750_SCL,
        PIN_HCSR04_TRIG, PIN_HCSR04_ECHO,
        PIN_CAUDALIMETRO, FACTOR_CAUDAL,
        PIN_GAS
    );

    actuadorMgr = new ActuadorManager(
        PIN_BUZZER, PIN_RELE_BOMBA, PIN_LED_VERDE, PIN_LED_ROJO,
        *sensorMgr, *commMgr, topicAlarma.c_str()
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
        docFinal["id"]     = idCorto();
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
    commMgr->publicar(topicLectura.c_str(), payload.c_str());

    Serial.println("[INFO] Payload publicado:");
    Serial.println(payload);
}

// -----------------------------------
// Envío inmediato de mensaje de alarma por MQTT
// -----------------------------------
void enviarAlarma(int nivel) {
    DynamicJsonDocument payload(256);
    payload["device"]  = deviceName;
    payload["id"]      = idCorto();
    payload["tipo"]    = "alarma";
    payload["nivel"]   = nivel;
    payload["origen"]  = actuadorMgr->getOrigenAlarma();
    switch (nivel) {
        case 3: payload["mensaje"] = "Alarma CRITICA"; break;
        case 2: payload["mensaje"] = "Alarma MEDIA";   break;
        case 1: payload["mensaje"] = "Alarma BAJA";    break;
        default: payload["mensaje"] = "Alarma desconocida"; break;
    }
    String out;
    serializeJson(payload, out);
    commMgr->publicar(topicAlarma.c_str(), out.c_str());
    Serial.println("[INFO] Alarma publicada:");
    Serial.println(out);
}

// -----------------------------------

void enviarInformacionDispositivo() {
    DynamicJsonDocument doc(2048);
    doc["device"] = deviceName;
    doc["id"]     = idCorto();
    doc["evento"] = "inicio";

    JsonArray sensores = doc.createNestedArray("sensores");

    JsonObject dht = sensores.createNestedObject();
    dht["nombre"] = "DHT11";
    dht["pin"] = PIN_DHT11;
    dht["umbralTempMax"] = prefs.getFloat("umbralTempMax", 35.0);
    dht["umbralHumMin"] = prefs.getFloat("umbralHumMin", 30.0);

    JsonObject bmp = sensores.createNestedObject();
    bmp["nombre"] = "BMP280";
    bmp["pin"] = -1;
    bmp["umbralPresionMin"] = prefs.getFloat("umbralPresionMin", 950.0);

    JsonObject bh = sensores.createNestedObject();
    bh["nombre"] = "BH1750";
    bh["pin"] = PIN_BH1750_SDA;
    bh["umbralLuxMin"] = prefs.getFloat("umbralLuxMin", 200.0);

    JsonObject sr = sensores.createNestedObject();
    sr["nombre"] = "HC-SR04";
    sr["pinTrig"] = PIN_HCSR04_TRIG;
    sr["pinEcho"] = PIN_HCSR04_ECHO;
    sr["umbralDistMax"] = prefs.getFloat("umbralDistMax", 30.0);

    JsonObject caudal = sensores.createNestedObject();
    caudal["nombre"] = "Caudalimetro";
    caudal["pin"] = PIN_CAUDALIMETRO;
    caudal["umbralCaudalMin"] = prefs.getFloat("umbralCaudalMin", 1.0);

    JsonObject co2 = sensores.createNestedObject();
    co2["nombre"] = "CO2";
    co2["pin"] = PIN_CO2;
    co2["umbralCO2Max"] = prefs.getFloat("umbralCO2Max", 1000.0);

    JsonObject energia = sensores.createNestedObject();
    energia["nombre"] = "Energia";
    energia["pin"] = PIN_ENERGIA;
    energia["umbralVoltMin"] = prefs.getFloat("umbralVoltMin", 3.0);
    energia["umbralCorrMin"] = prefs.getFloat("umbralCorrMin", 0.05);

    JsonArray actuadores = doc.createNestedArray("actuadores");

    JsonObject bomba = actuadores.createNestedObject();
    bomba["nombre"] = "BOMBA";
    bomba["pin"] = PIN_RELE_BOMBA;
    bomba["estado"] = actuadorMgr->isBombaEncendida() ? "ON" : "OFF";

    JsonObject buzzer = actuadores.createNestedObject();
    buzzer["nombre"] = "BUZZER";
    buzzer["pin"] = PIN_BUZZER;

    JsonObject ledV = actuadores.createNestedObject();
    ledV["nombre"] = "LED_VERDE";
    ledV["pin"] = PIN_LED_VERDE;

    JsonObject ledR = actuadores.createNestedObject();
    ledR["nombre"] = "LED_ROJO";
    ledR["pin"] = PIN_LED_ROJO;

    String out;
    serializeJson(doc, out);
    commMgr->publicar(topicInfo.c_str(), out.c_str());

    Serial.println("[INFO] Mensaje de inicio publicado:");
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