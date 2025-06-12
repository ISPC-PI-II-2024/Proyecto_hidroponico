#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <EEPROM.h>

#define MAX_SSID_LENGTH 32
#define MAX_PASS_LENGTH 64

// Variables globales
extern char redSeleccionada[MAX_SSID_LENGTH];
extern char claveWiFi[MAX_PASS_LENGTH];

// Inicialización WiFi
void inicializarWiFi();

// Escaneo de redes disponibles
void escanearRedes();

// Conectar a red seleccionada
bool conectarWiFi(const char* ssid, const char* password);

// Guardar credenciales en EEPROM
void guardarCredenciales();

#endif 
