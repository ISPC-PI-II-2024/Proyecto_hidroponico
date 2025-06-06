#include "GY302_BH1750.h"

GY302::GY302(uint8_t sda, uint8_t scl, TwoWire &wirePort)
  : _sda(sda), _scl(scl), _wire(&wirePort), lightMeter() {}

bool GY302::begin() {
  _wire->begin(_sda, _scl);
  return lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, _wire);
}

float GY302::readLightLevel() {
  if (lightMeter.measurementReady()) {
    return lightMeter.readLightLevel();
  } else {
    return -1.0f; // Valor de error, sin nueva medición
  }
}