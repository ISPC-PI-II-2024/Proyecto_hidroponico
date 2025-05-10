#ifndef SENSORCO2_H
#define SENSORCO2_H

#include <Arduino.h>
#include "MQUnifiedsensor.h"  // Librería para sensors MQ, nos simplifica la lectura y calibracion

//=============================================
// Configuración del sensor MQ2 para medir CO2
//=============================================

// Este formato estandar viene defindo en la documentacion de la  libreria MQUnifiedsensor
// RS refiere a la resistencia del sensor y RO a la resistencia en aire limpio. Esta ultima es la que se usa de referencia

#define BOARD               F("ESP32")  // Identificador de la placa
#define VCC                 3.3f        // Voltaje de alimentación (V)
#define RL_KOHM             10.0f       // Resistencia de carga en kilo-ohmios (kΩ)
#define ADC_RES             4095        // Resolución ADC del ESP32 (12 bits)
#define RATIO_MQ2_CLEAN_AIR 9.8f        // Rs/Ro en aire limpio según datasheet

class SensorCO2 {
public:

    explicit SensorCO2(int pin);        // PIN a definir segun la planilla de sensores
    void begin();                       // Inicializa el sensor y la librería MQUnifiedsensor
    int readRaw();                      // Lectura cruda del sensor (ADC)  
    float readResistance();             // Lectura de resistencia del sensor (Rs) en KOhms
    float readRatio();                  // Lectura de la relación Rs/Ro
    float readPPM();                    // Lectura de PPM de CO2

private:
    int sensorPin;
    float Ro;                           // Resistencia en aire limpio (kΩ)
    MQUnifiedsensor mq2;                // Objeto de la librería MQUnifiedsensor para el sensor MQ2
};

#endif 