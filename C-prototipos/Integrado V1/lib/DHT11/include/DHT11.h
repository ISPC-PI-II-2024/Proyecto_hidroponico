#ifndef DHT11_H
#define DHT11_H

#include <DHT.h>

#define DHTPIN 4        // Pin de datos del sensor
#define DHTTYPE DHT11   // Tipo de sensor

class DHT11Sensor {
  public:
    DHT11Sensor();
    void begin();
    float readTemperature();
    float readHumidity();

  private:
    DHT dht;
};

#endif
