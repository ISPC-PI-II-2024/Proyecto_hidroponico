#include "SensorBMP280.h"

SensorBMP280::SensorBMP280() {
  status = false;
}

bool SensorBMP280::begin() {
  if (!bmp.begin(0x76)) {
      Serial.println("Error: No se encontró el sensor BMP280. Verifica las conexiones!");
      status = false;
      return false;
  }
  status = true;
  return true;
}


float SensorBMP280::readTemperature() {
  if (status) return bmp.readTemperature();
  else return NAN;
}

float SensorBMP280::readPressure() {
  if (status) return bmp.readPressure() / 100.0; // convert to hPa
  else return NAN;
}

float SensorBMP280::readAltitude(float seaLevelhPa) {
  if (status) return bmp.readAltitude(seaLevelhPa);
  else return NAN;
}

bool SensorBMP280::isConnected() {
  return status;
}