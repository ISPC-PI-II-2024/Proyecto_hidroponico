#include "SensorBMP280.h"

SensorBMP280::SensorBMP280(uint8_t sda, uint8_t scl, uint8_t addr)
  : _sda(sda), _scl(scl), _addr(addr), status(false) {}

bool SensorBMP280::begin() {
  Wire.begin(_sda, _scl);
  if (!bmp.begin(_addr)) {
    Serial.println("Error: No se encontró el sensor BMP280. Verifica las conexiones!");
    status = false;
    return false;
  }
  status = true;
  return true;
}

float SensorBMP280::readTemperature() {
  return status ? bmp.readTemperature() : NAN;
}

float SensorBMP280::readPressure() {
  return status ? bmp.readPressure() / 100.0 : NAN;
}

float SensorBMP280::readAltitude(float seaLevelhPa) {
  return status ? bmp.readAltitude(seaLevelhPa) : NAN;
}

bool SensorBMP280::isConnected() {
  return status;
}
