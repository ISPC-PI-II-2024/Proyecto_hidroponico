#ifndef GY302_BH1750_H
#define GY302_BH1750_H

#include <Wire.h>
#include <BH1750.h>

class GY302 {
  public:
    GY302();
    bool begin();
    float readLightLevel();
  private:
    BH1750 lightMeter;
};

#endif