#ifndef SENSORBMP280_H
#define SENSORBMP280_H

#include <Arduino.h>
#include <Adafruit_BMP280.h>

class SensorBMP280 {
  public:
    SensorBMP280();
    bool begin(); // Default I2C
    float readTemperature(); // Celsius
    float readPressure();    // hPa
    float readAltitude(float seaLevelhPa = 1013.25); // metros
    bool isConnected();

  private:
    Adafruit_BMP280 bmp;
    bool status;
};

#endif