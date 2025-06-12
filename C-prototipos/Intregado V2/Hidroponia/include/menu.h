#ifndef MENU_H
#define MENU_H

#include "pantalla.h"
#include "comunicacion.h"
#include "sensores.h"

extern bool menuVisible;  // Declaración, pero no inicialización

// Funciones para mostrar y procesar la navegación del menú general
void mostrarMenuPrincipal();
void procesarEntradaMenu();
//void manejarSeleccionMenu();
void mostrarMenuRecetas();
void mostrarMenuWiFi();
void mostrarMensaje(String mensaje);


#endif 

