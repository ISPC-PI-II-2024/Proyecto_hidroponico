#include <Arduino.h>
#include "SensorBMP280.h"

SensorBMP280 bmp;

void setup() {
  Serial.begin(9600);
  if (bmp.begin()) {
    Serial.println("BMP280 conectado correctamente.");
  } else {
    Serial.println("Error al conectar el BMP280.");
  }
}

void loop() {
  float temp = bmp.readTemperature();
  float presion = bmp.readPressure();
  float altitud = bmp.readAltitude();

  Serial.println("-------------");
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Presión: ");
  Serial.print(presion);
  Serial.println(" hPa");

  Serial.print("Altitud estimada: ");
  Serial.print(altitud);
  Serial.println(" m");

  delay(5000);
}