#include "comunicacion.h"
#include "configuracion.h"
#include "sensores.h"
#include <ArduinoJson.h>
#include "gsm_async.h"
#include "mqtt_async.h"
#include "control.h"
#include "rtc.h"

// Estado de conexión
bool wifiActivo = true;
bool loRaActivo = false;
bool bleActivo = false;
extern bool mqttConectado;

// Instancia del cliente MQTT
// AsyncMqttClient mqttClient;

// Conexión a WiFi
void conectarWiFi() {
    Serial.println("Conectando a WiFi...");
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 20) {
        delay(500);
        Serial.print(".");
        intentos++;
    }
    if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n¡WiFi CONECTADO!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Potencia señal (RSSI): ");
    Serial.println(WiFi.RSSI());
    wifiActivo = true;
  } else {
    Serial.println("\nERROR: No se pudo conectar a WiFi");
    Serial.print("Código estado: ");
    Serial.println(WiFi.status());
    wifiActivo = false;
  }
    Serial.println(WiFi.status() == WL_CONNECTED ? "\nWiFi conectado" : "\nError WiFi");
}

// MQTT: recepción de mensajes
void recibirMensajeMQTT(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    Serial.print(" Mensaje MQTT recibido en el tópico: ");
    Serial.println(topic);

    String mensaje = "";
    for (size_t i = 0; i < len; i++) {
        mensaje += (char)payload[i];
    }
    Serial.print(" Contenido: ");
    Serial.println(mensaje);

    // Ejemplo: controlar bomba o luces
    if (String(topic) == "hidroponia/control") {
        if (mensaje == "bomba_on") {
            bomba_activa = true;
            Serial.println(" BOMBA ACTIVADA");
        } else if (mensaje == "bomba_off") {
            bomba_activa = false;
            Serial.println(" BOMBA DESACTIVADA");
        } else if (mensaje == "luces_on") {
            luces_activas = true;
            Serial.println(" LUCES ENCENDIDAS");
        } else if (mensaje == "luces_off") {
            luces_activas = false;
            Serial.println(" LUCES APAGADAS");
        }
    }
}

// Manejo MQTT loop
void manejarMQTT() {
    mqttLoop();  // para compatibilidad, aunque no hace falta en async
}

// Enviar datos según el medio disponible
void  enviarDatos() {
    if (wifiActivo && WiFi.status() == WL_CONNECTED) {
        Serial.println(" Enviando por Wi-Fi (MQTT)...");
        enviarDatosMQTT();
    } else if (gsmIsConnected()) {
        Serial.println(" Enviando por GSM (MQTT)...");
        enviarDatosMQTT();
    } else if (loRaActivo) {
        Serial.println(" Enviando por LoRa...");
        enviarDatosLoRa();
    } else {
        Serial.println(" No hay método de comunicación activo.");
    }
}

// Enviar datos por MQTT
void enviarDatosMQTT() {
    StaticJsonDocument<512> jsonDoc;

    jsonDoc["gatewayId"] = "esp32-" + obtenerID();    
    jsonDoc["timestamp"] = obtenerHora();             

    JsonArray nodes = jsonDoc.createNestedArray("nodes");
    JsonObject node = nodes.createNestedObject();
    node["deviceId"] = "esp32-" + obtenerID();         
    node["timestamp"] = obtenerHora();                
    node["transmitter"] = true;
    node["reciber"] = true;

    JsonObject sensors = node.createNestedObject("sensors");
    sensors["temperaturaAgua"] = temperaturaAgua;
    sensors["temperaturaAire"] = temperaturaAire;
    sensors["humedad"] = humedadAire;
    sensors["nivelAgua"] = nivelAgua;
    sensors["flujoAgua"] = flujoAgua;
    sensors["luz"] = luz;
    sensors["gas"] = gas;
    sensors["corriente"] = corriente;
    sensors["voltaje"] = voltaje;
    sensors["potencia"] = potencia;
    sensors["phValor"] = ph;
    sensors["hora"] = obtenerHora();                   

    JsonObject controls = node.createNestedObject("controls");
    controls["bomba"] = bomba_activa;
    controls["luces"] = luces_activas;
    controls["alarma"] = alarma_activa;
    controls["modoAutomatico"] = modo_automatico;

    //  Convertimos a string y enviamos con el nuevo método
    String mensaje;
    serializeJson(jsonDoc, mensaje);
    publicarLecturas("sensores/datos", mensaje);
}

// Declaraciones de funciones auxiliares (agregar al inicio del archivo .h o antes de la función)
JsonObject createSensorInfo(String pin, String type);
JsonObject createI2CSensorInfo(String type, String addr);
JsonObject createControlInfo(String pin, String type);

void enviarInfoDispositivo() {
    StaticJsonDocument<512> jsonDoc;
    
    jsonDoc["gatewayId"] = "esp32-" + obtenerID();
    jsonDoc["timestamp"] = obtenerHora();
    
    JsonArray nodes = jsonDoc.createNestedArray("nodes");
    JsonObject node = nodes.createNestedObject();
    node["deviceId"] = "esp32-" + obtenerID();
    node["timestamp"] = obtenerHora();
    node["transmitter"] = true;
    node["receiver"] = true;
    
    // Sensores con información de pines (optimizado)
    JsonObject sensors = node.createNestedObject("sensors");
    sensors["tempAgua"] = createSensorInfo("25", "DS18B20");
    sensors["tempAire"] = createSensorInfo("33", "DHT11");
    sensors["humedad"] = createSensorInfo("33", "DHT11");
    sensors["nivel"] = createSensorInfo("32", "HC-SR04");
    sensors["flujo"] = createSensorInfo("34", "YF-S201");
    sensors["luz"] = createSensorInfo("4", "LDR");
    sensors["gas"] = createSensorInfo("36", "MQ-2");
    sensors["corriente"] = createI2CSensorInfo("INA219", "0x40");
    sensors["voltaje"] = createI2CSensorInfo("INA219", "0x40");
    sensors["potencia"] = createI2CSensorInfo("INA219", "0x40");
    sensors["ph"] = createSensorInfo("35", "pH-analog");
    sensors["hora"] = createI2CSensorInfo("DS1307", "");
    
    // Controles
    JsonObject controls = node.createNestedObject("controls");
    controls["bomba"] = createControlInfo("4", "dout");
    controls["luces"] = createControlInfo("2", "dout");
    controls["alarma"] = createControlInfo("15", "dout");
    controls["modoAuto"] = createControlInfo("", "flag");
    
    // Módulos principales (compacto)
    JsonObject modules = node.createNestedObject("modules");
    modules["tft"] = "SPI-ILI9341";
    modules["lora"] = "915MHz";
    modules["pcf"] = "I2C-0x27";
    modules["gsm"] = "UART-SIM800L";
    
    String mensaje;
    serializeJson(jsonDoc, mensaje);
    publicarLecturas("dispositivo/info", mensaje);
    Serial.println(" Información del dispositivo enviada por MQTT: " + mensaje);
}

// Implementación de funciones auxiliares (agregar después de la función principal)
JsonObject createSensorInfo(String pin, String type) {
    StaticJsonDocument<64> temp;
    temp["pin"] = "GPIO" + pin;
    temp["type"] = type;
    return temp.as<JsonObject>();
}

JsonObject createI2CSensorInfo(String type, String addr) {
    StaticJsonDocument<64> temp;
    temp["bus"] = "I2C";
    temp["type"] = type;
    if (addr != "") temp["addr"] = addr;
    return temp.as<JsonObject>();
}

JsonObject createControlInfo(String pin, String type) {
    StaticJsonDocument<64> temp;
    if (pin != "") temp["pin"] = "GPIO" + pin;
    temp["type"] = type;
    return temp.as<JsonObject>();
}
// Enviar datos por LoRa (vacío por ahora)
void enviarDatosLoRa() {}
void recibirDatosLoRa() {}
void enviarDatosBLE() {}
void recibirDatosBLE() {}
