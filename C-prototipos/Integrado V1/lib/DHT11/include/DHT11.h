#ifndef DHT11_H
#define DHT11_H

#include <DHT.h>

class DHT11Sensor {
  public:
    DHT11Sensor(uint8_t pin, uint8_t type = DHT11); // Pin y tipo opcional
    void begin();
    float readTemperature();
    float readHumidity();

  private:
    DHT dht;
};

#endif
