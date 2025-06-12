#include "Caudalimetro.h"

Caudalimetro::Caudalimetro(uint8_t pin, float factor)
    : _pin(pin), _factor(factor), pulsos(0) {}

void Caudalimetro::begin() {
    pinMode(_pin, INPUT_PULLUP);
    pulsos = 0;

}

void Caudalimetro::reset() {
    pulsos = 0;
}

void Caudalimetro::isr() {
    pulsos++;
}

float Caudalimetro::calcularCaudal(float intervaloSegundos) {
    noInterrupts();
    uint32_t p = pulsos;
    interrupts();
    return ((float)p / _factor) * (60.0 / intervaloSegundos);
}

float Caudalimetro::calcularVolumen() {
    noInterrupts();
    uint32_t p = pulsos;
    interrupts();
    return (float)p / _factor;
}
