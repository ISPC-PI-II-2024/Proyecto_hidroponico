#include "Caudalimetro.h"

float calcularCaudal(int pulsos, float factor, float intervaloSegundos) {
    return ((float)pulsos / factor) * (60.0 / intervaloSegundos);
}

float calcularVolumen(int pulsos, float factor) {
    return (float)pulsos / factor;
}