#ifndef RECEPTOR_H
#define RECEPTOR_H

#include "control.h"

extern bool esNodoReceptor; // Variable para indicar si este nodo es receptor

// Inicializa las funciones y parámetros del modo receptor
void inicializarReceptor();

// Procesa el mensaje recibido y ejecuta las acciones necesarias
void procesarDatos(const char* mensaje);

#endif 
