#include "ArduinoJson.h"
#include "esp_system.h"
#include <Wire.h>
#include <SPI.h>
#include "configuracion.h"



// Función para obtener el ID del dispositivo basado en su MAC
String obtenerID() {
    uint8_t mac[6];
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK) return "UNKNOWN";
    char buf[13];
    snprintf(buf, sizeof(buf), "%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buf);
}

// Escaneo de dispositivos I2C
void escanearI2C() {
    Serial.println("Escaneando dispositivos I2C...");
    Wire.begin();
    for (uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.print("Dispositivo encontrado en dirección: 0x");
            Serial.println(address, HEX);
        }
    }
}

// Escaneo de dispositivos SPI (requiere configuración específica)
void escanearSPI() {
    Serial.println("Escaneando dispositivos SPI...");
    SPI.begin();
    for (uint8_t csPin = 5; csPin <= 10; csPin++) { // Ajusta según la configuración
        pinMode(csPin, OUTPUT);
        digitalWrite(csPin, LOW);
        delay(10);
        digitalWrite(csPin, HIGH);
        Serial.print("Probando CS en pin: ");
        Serial.println(csPin);
    }
}

// Escaneo de dispositivos UART (requiere configuración específica)
void escanearUART(HardwareSerial &serialPort) {
    Serial.println("Escaneando dispositivos UART...");
    serialPort.begin(9600);
    serialPort.println("Prueba UART");
    delay(500);
    while (serialPort.available()) {
        Serial.write(serialPort.read());
    }
}

void iniciarEscaneo() {
    escanearI2C();
    escanearSPI();
    escanearUART(Serial2); // Puedes usar Serial, Serial1 o Serial2 según tu configuración
}