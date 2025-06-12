#ifndef WIFI_ASYNC_H
#define WIFI_ASYNC_H

#include <WiFi.h>

/// Inicializa y gestiona WiFi vía eventos
void wifiSetup(const char* ssid, const char* password);
/// Llamar en loop (vacío, todo va por eventos)
void wifiLoop();
/// Indica si hay conexión
bool wifiIsConnected();

#endif // WIFI_ASYNC_H