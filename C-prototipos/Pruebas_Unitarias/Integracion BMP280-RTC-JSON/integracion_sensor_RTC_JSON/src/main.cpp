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

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Inicializar el sensor BMP280
  if (!sensorBMP.begin()) { // Asegurate que sea la dirección correcta (0x76 o 0x77)
    Serial.println("❌ Error al inicializar BMP280");
    while (1);
  }
  Serial.println("✅ BMP280 listo");

  wifi.conectar();

  // Inicializar el reloj RTC
  relojRTC.begin();
  relojRTC.sincronizarConNTP(); // Sincronizar con servidor NTP
}

void loop() {
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
  Serial.println("📦 JSON generado:");
  Serial.println(mensajeJSON);

  delay(5000); // Esperar 5 segundos
}
