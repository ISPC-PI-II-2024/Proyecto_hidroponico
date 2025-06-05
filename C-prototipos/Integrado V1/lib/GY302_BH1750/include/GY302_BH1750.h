#ifndef GY302_BH1750_H
#define GY302_BH1750_H

#include <Wire.h>
#include <BH1750.h>

class GY302 {
  public:
    GY302(uint8_t sda = 21, uint8_t scl = 22, TwoWire &wirePort = Wire);
    bool begin();
    float readLightLevel();

  private:
    BH1750 lightMeter;
    uint8_t _sda, _scl;
    TwoWire* _wire;
};

#endif
