#ifndef SENSORCO2_H
#define SENSORCO2_H

#include <Arduino.h>  // Para reconocer las funciones de Arduino como analogRead

class SensorCO2 {
public:
    SensorCO2(int pin);  // Constructor para el pin del sensor
    int readCO2();       // Función para leer el CO2
private:
    int sensorPin;       // Pin del sensor
};

#endif
