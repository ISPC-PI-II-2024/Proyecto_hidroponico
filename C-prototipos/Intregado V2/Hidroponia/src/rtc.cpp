#include "rtc.h"
#include <ArduinoJson.h>
#include "esp_system.h"

RTC_DS1307 rtc;
DateTime horaActual;

// Inicialización del RTC
void inicializarRTC() {
    if (!rtc.begin()) {
        Serial.println("Error al iniciar RTC");
        return;
    }

    if (!rtc.isrunning()) {
        Serial.println("RTC no tiene hora válida, configurando...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

// Obtener hora actual


String obtenerHora() {
    DateTime horaActual = rtc.now();
    String hora = String(horaActual.year()) + "-" + 
                  String(horaActual.month()) + "-" + 
                  String(horaActual.day()) + " " + 
                  String(horaActual.hour()) + ":" + 
                  String(horaActual.minute()) + ":" + 
                  String(horaActual.second());

    Serial.println("Hora actual: " + hora);  // Imprimir en el monitor serial
    return hora;
}


// Sincronizar hora desde MQTT
void sincronizarHora(String nuevaHora) {
    int year, month, day, hour, minute;
    sscanf(nuevaHora.c_str(), "%d-%d-%d %d:%d", &year, &month, &day, &hour, &minute);
    rtc.adjust(DateTime(year, month, day, hour, minute, 0));
    Serial.println("Hora sincronizada desde MQTT: " + nuevaHora);
} 

