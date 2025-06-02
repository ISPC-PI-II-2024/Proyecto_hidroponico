#include "DHT11.h"

DHT11Sensor::DHT11Sensor() : dht(DHTPIN, DHTTYPE) {}

void DHT11Sensor::begin() {
  dht.begin();
}

float DHT11Sensor::readTemperature() {
  return dht.readTemperature();
}

float DHT11Sensor::readHumidity() {
  return dht.readHumidity();
}
