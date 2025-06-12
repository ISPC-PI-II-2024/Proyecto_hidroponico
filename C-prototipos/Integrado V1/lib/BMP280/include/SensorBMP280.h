#ifndef SENSORBMP280_H
#define SENSORBMP280_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h> 

class SensorBMP280 {
  public:
    SensorBMP280(uint8_t sda = 21, uint8_t scl = 22, uint8_t addr = 0x76);
    bool begin();
    float readTemperature();
    float readPressure();
    float readAltitude(float seaLevelhPa = 1013.25);
    bool isConnected();

  private:
    Adafruit_BMP280 bmp;
    bool status;
    uint8_t _sda, _scl, _addr;
};

#endif
