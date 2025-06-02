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
#define ADC_RES             12          // Resolución ADC del ESP32 (12 bits = 4095)
#define RATIO_CLEAN_AIR     9.8f        // Rs/Ro en aire limpio según datasheet
#define CALIB_DELAY_MS      2000          // Tiempo de espera para estabilizar el sensor (ms)

// Por suerte la configuracion para el MQ135 es simple, comentar y descomentar lo siguiente
//#define RATIO_CLEAN_AIR     3.8f 
//#define RL_KOHM             20.0f

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
    MQUnifiedsensor mq;                // Objeto de la librería MQUnifiedsensor para el sensor MQ sea el 2, o el 135
};

#endif 