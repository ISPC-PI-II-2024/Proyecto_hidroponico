#include "wifi_manager.h"

char redSeleccionada[MAX_SSID_LENGTH] = "";
char claveWiFi[MAX_PASS_LENGTH] = "";

// Inicializar WiFi
void inicializarWiFi() {
    EEPROM.begin(128);
    EEPROM.get(0, redSeleccionada);
    EEPROM.get(32, claveWiFi);

    if (strlen(redSeleccionada) > 0) {
        Serial.print("Intentando conectar a: ");
        Serial.println(redSeleccionada);
        conectarWiFi(redSeleccionada, claveWiFi);
    }
}

// Escaneo de redes disponibles
void escanearRedes() {
    Serial.println("Escaneando redes WiFi...");
    int numRedes = WiFi.scanNetworks();
    for (int i = 0; i < numRedes; i++) {
        Serial.println(WiFi.SSID(i));
    }
}

// Conectar a red seleccionada
bool conectarWiFi(const char* ssid, const char* password) {
    Serial.println("Conectando...");
    WiFi.begin(ssid, password);
    int intentos = 0;
    
    while (WiFi.status() != WL_CONNECTED && intentos < 10) {
        delay(1000);
        Serial.print(".");
        intentos++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("¡Conectado!");
        return true;
    } else {
        Serial.println("Error de conexión.");
        return false;
    }
}

// Guardar credenciales en EEPROM
void guardarCredenciales() {
    EEPROM.put(0, redSeleccionada);
    EEPROM.put(32, claveWiFi);
    EEPROM.commit();
} 
