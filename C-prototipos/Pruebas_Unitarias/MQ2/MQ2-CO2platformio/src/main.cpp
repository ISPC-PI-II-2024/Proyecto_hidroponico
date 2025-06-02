#include <Arduino.h>
#include "SensorCO2.h"

#ifndef SENSORGAS_PIN
#define SENSORGAS_PIN 15 
#endif

SensorCO2 co2Sensor(SENSORGAS_PIN);

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("=== Iniciando lectura de CO2 con MQ ===");
    co2Sensor.begin();
}

void loop() {
    int raw    = co2Sensor.readRaw();
    float rs   = co2Sensor.readResistance();
    float ratio= co2Sensor.readRatio();
    float ppm  = co2Sensor.readPPM();

    Serial.print("Lectura analogica: ");     Serial.print(raw);
    Serial.print("  Resistencia Sensor: ");  Serial.print(rs, 2);    Serial.print(" kΩ");
    Serial.print("  Relacion Rs/Ro: ");      Serial.print(ratio, 3);
    Serial.print("  CO2 PPM: ");             Serial.println(ppm, 1);

    delay(1000);
}