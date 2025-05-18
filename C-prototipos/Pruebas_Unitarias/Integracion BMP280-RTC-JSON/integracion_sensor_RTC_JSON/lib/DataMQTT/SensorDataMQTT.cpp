#include "SensorDataMQTT.h"

SensorDataMQTT* SensorDataMQTT::instancia = nullptr;

SensorDataMQTT::SensorDataMQTT(const char* ssid, const char* password, const char* broker, int port,
                               IPAddress ip, IPAddress gateway, IPAddress subnet)
    : ssid(ssid), password(password), broker(broker), port(port),
      ip(ip), gateway(gateway), subnet(subnet), client(espClient) {
    instancia = this;
}

void SensorDataMQTT::conectarWiFi() {
    WiFi.config(ip, gateway, subnet, IPAddress(8,8,8,8)); //configuro la conexion WiFi con IP fija y un servidor DNS
    
    WiFi.begin(ssid, password);  //arranco la conexion WiFi
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi con IP: " + WiFi.localIP().toString());
}

void SensorDataMQTT::conectarMQTT() {
    client.setServer(broker, port);   //Configuro direccion del broker MQTT
    

    while (!client.connected()) {
        Serial.println("Conectando al broker MQTT...");
        if (client.connect("ESP32_bmp280")) {
            Serial.println("Conectado!");
            
        } else {
            Serial.print("No se pudo conectar: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

void SensorDataMQTT::manejarMQTT() {
    if (!client.connected()) {
        conectarMQTT();
    }
    client.loop();
}

void SensorDataMQTT::publicarLecturas(String cadena_JSON/*float co2, float temperatura, float humedad, float nivelBateria*/) {
    int n = cadena_JSON.length();
    char buffer[n + 1];
    auto primero = cadena_JSON.begin();
    auto ultimo = cadena_JSON.end();

    std::copy(primero, ultimo, buffer);
   
    client.publish("sensores/datos", buffer);
}

/*void SensorDataMQTT::callbackWrapper(char* topic, byte* payload, unsigned int length) {
    if (instancia) {
        instancia->procesarMensaje(topic, payload, length);
    }
}*/

