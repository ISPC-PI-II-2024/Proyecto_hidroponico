#ifndef SR04_H
#define SR04_H

#include <stdint.h>
/// Se llama en el setup() - Inicializa el SR04 en los pines TRIG y ECHO
void sr04Begin(uint8_t pinTrig, uint8_t pinEcho);
/// Se llama en el loop(), retorna la última medida válida.
float sr04Read();
#endif
