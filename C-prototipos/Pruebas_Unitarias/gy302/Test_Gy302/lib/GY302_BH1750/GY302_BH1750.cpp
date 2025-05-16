#include "GY302_BH1750.h"

GY302::GY302() : lightMeter() {}

bool GY302::begin() {
  Wire.begin(); // Inicia I2C con pines default (SDA=21, SCL=22 en ESP32)
  return lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
}

float GY302::readLightLevel() {
  return lightMeter.readLightLevel(); // Retorna lux
}