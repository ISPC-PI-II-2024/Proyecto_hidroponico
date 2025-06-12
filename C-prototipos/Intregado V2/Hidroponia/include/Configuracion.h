 
#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include "ArduinoJson.h"
#include "esp_system.h"
String obtenerID();

// Configuración General del Sistema
#define BAUD_RATE 9600

// Puedes agregar aquí constantes globales o ajustes que se usen en varios módulos.
#define VERSION_FIRMWARE "1.0.0"

#endif