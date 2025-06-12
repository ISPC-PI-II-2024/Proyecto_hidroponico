#ifndef RECETAS_H
#define RECETAS_H

#include <EEPROM.h>

#define MAX_RECETAS 5  // Número máximo de recetas almacenadas

// Estructura de receta
struct Receta {
    char nombre[20];
    float tempAgua;
    float tempAire;
    float humedad;
    float nivelAgua;
    float ph;
    bool iluminacion;
};

// Recetas guardadas
extern Receta recetas[MAX_RECETAS];
extern int recetaSeleccionada;

// Funciones para gestionar recetas
void cargarRecetas();
void guardarRecetas();
void seleccionarReceta(int indice);
void mostrarRecetaActual();
void iniciarEEPROM(); // Inicializa la EEPROM al inicio del programa

#endif 
