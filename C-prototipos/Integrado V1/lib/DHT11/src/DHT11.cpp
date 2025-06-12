#include "DHT11.h"

DHT11Sensor::DHT11Sensor(uint8_t pin, uint8_t type)
  : dht(pin, type) {}

void DHT11Sensor::begin() {
  dht.begin();
}

float DHT11Sensor::readTemperature() {
  return dht.readTemperature();
}

float DHT11Sensor::readHumidity() {
  return dht.readHumidity();
}
