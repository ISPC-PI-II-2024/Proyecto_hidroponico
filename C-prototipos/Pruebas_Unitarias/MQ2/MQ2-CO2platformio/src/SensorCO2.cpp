#include "SensorCO2.h"
#include <Arduino.h>  // Asegúrate de que las funciones de Arduino estén disponibles

SensorCO2::SensorCO2(int pin) {
    sensorPin = pin;
}

int SensorCO2::readCO2() {
    int potValue = analogRead(sensorPin);  // Usamos el potenciómetro para simular el CO2
    return map(potValue, 0, 4095, 0, 1500);  // Escalamos a 0-1500 ppm
}
