#ifndef TRANSMISOR_H
#define TRANSMISOR_H

#include "sensores.h"
#include "comunicacion.h"

extern bool esNodoTransmisor; // Variable para indicar si este nodo es transmisor

// Inicializa las funciones y parámetros del modo transmisor
void inicializarTransmisor();

// Envía la información de los sensores mediante MQTT (o el canal elegido)
void enviaDats();

#endif 
