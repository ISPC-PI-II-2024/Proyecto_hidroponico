#ifndef RTC_H
#define RTC_H

#include <Wire.h>
#include <RTClib.h>

// Variables globales
extern RTC_DS1307 rtc;
extern DateTime horaActual;

// Inicialización del RTC
void inicializarRTC();

// Obtener fecha y hora actual
String obtenerHora();

// Sincronizar hora desde MQTT
void sincronizarHora(String nuevaHora);



#endif 
