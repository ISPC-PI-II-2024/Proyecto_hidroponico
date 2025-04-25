#include "MQ135.h"
#include <Arduino.h>

MQ135::MQ135(int pin) {
    _pin = pin;
}

float MQ135::readPPM() {
    int value = analogRead(_pin);  // Valor entre 0 y 1023
    float voltage = value * (5.0 / 1023.0);  // Convierte a voltaje (0 - 5V)

    // Mapeo: suponemos que 0V = 100 ppm y 5V = 1000 ppm
    float ppm = mapFloat(voltage, 0.0, 5.0, 100.0, 1000.0);
    return ppm;
}

// Función auxiliar para mapear floats (como map(), pero con decimales)
float MQ135::mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

