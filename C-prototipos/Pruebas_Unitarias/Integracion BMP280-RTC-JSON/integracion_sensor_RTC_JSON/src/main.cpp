#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <SensorBMP280.h>
#include "DS3231.h"
#include "JsonBuilder.h"
#include "WiFiManager.h"


SensorBMP280 sensorBMP; // Objeto del sensor BMP280
DS3231Clock relojRTC;      // Objeto del reloj DS3231
JsonBuilder creadorJSON;   // Objeto para construir el JSON
WiFiManager wifi; //Objeto conexion wifi

unsigned long Ultima_muestra = 0;
const unsigned long Intervalo_muestra = 5000;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Inicializar el sensor BMP280
  if (!sensorBMP.begin()) { // Asegurate que sea la dirección correcta (0x76 o 0x77)
    Serial.println("Error al inicializar BMP280, Se intentara de nuevo periodicamente");
    } else {
      Serial.println("BMP280 conectado correctamente.");
    }
  

  wifi.conectar();

  // Inicializar el reloj RTC
  relojRTC.begin();
  if (wifi.estaConectado()) {
    relojRTC.sincronizarConNTP(); // Sincronizar con servidor NTP
  } else {
    Serial.println("No hay conexión WiFi, no se sincronizará con NTP.");
  } 
}

void loop() {

  unsigned long Millis_actual = millis();

  if (Millis_actual - Ultima_muestra >= Intervalo_muestra) {
    Ultima_muestra = Millis_actual;

    // Leer variables del sensor
    float temperatura = sensorBMP.readTemperature();
    float presion = sensorBMP.readPressure() / 100.0F; // hPa
    float altura = sensorBMP.readAltitude(1013.25); // Ajustar presión al nivel del mar si se desea

    // Obtener fecha y hora actual
    String fechaHora = relojRTC.getTimeString();

    // Crear el JSON
    creadorJSON.limpiar();
    creadorJSON.agregarCampo("fecha_hora", fechaHora);
    creadorJSON.agregarCampo("temperatura_C", temperatura);
    creadorJSON.agregarCampo("presion_hPa", presion);
    creadorJSON.agregarCampo("altura_m", altura);

    String mensajeJSON = creadorJSON.construir();
    Serial.println("JSON generado:");
    Serial.println(mensajeJSON);
  }

}
