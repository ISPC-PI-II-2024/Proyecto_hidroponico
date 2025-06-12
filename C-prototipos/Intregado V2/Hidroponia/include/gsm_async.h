#ifndef GSM_ASYNC_H
#define GSM_ASYNC_H

// Define el modelo de módem antes de incluir TinyGsmClient
#ifndef TINY_GSM_MODEM_SIM800
  #define TINY_GSM_MODEM_SIM800
#endif
#include <TinyGsmClient.h>

/// Inicializa y gestiona GSM/GPRS (attach, APN…)
void gsmSetup(const char* apn, const char* user, const char* pass);
/// Llamar en loop para mantener conexión
void gsmLoop();
/// Indica si la sesión GPRS está activa
bool gsmIsConnected();
/// Proporciona el cliente TCP para MQTT
TinyGsmClient& gsmGetClient();

#endif // GSM_ASYNC_H
