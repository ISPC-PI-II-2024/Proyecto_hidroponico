#ifndef CAUDALIMETRO_H
#define CAUDALIMETRO_H

#include <Arduino.h>

class Caudalimetro {
public:
    Caudalimetro(uint8_t pin, float factor);
    void begin();
    void reset();
    void isr();  // Llamar desde la ISR
    float calcularCaudal(float intervaloSegundos);
    float calcularVolumen();

private:
    volatile uint32_t pulsos;
    uint8_t _pin;
    float _factor;
};

#endif
