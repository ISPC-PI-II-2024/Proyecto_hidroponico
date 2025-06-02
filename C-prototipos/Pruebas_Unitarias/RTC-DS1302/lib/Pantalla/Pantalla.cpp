#include "Pantalla.h"

void Pantalla::mostrarHora(const RtcDateTime& dt) {
    char fechaHora[30];
    snprintf(fechaHora, sizeof(fechaHora), "%02u/%02u/%04u %02u:%02u:%02u",
             dt.Day(), dt.Month(), dt.Year(),
             dt.Hour(), dt.Minute(), dt.Second());
    Serial.println(fechaHora);
}

void Pantalla::mensajeInicio() {
    Serial.println(" Sistema de reloj en marcha.");
    Serial.println("Comando disponible:");
    Serial.println("  ver  -> muestra la fecha y hora actual");
    Serial.println("-----------------------------------------------------------");
}

void Pantalla::mensajeHoraInvalida() {
    Serial.println(" Fecha y hora inválidas.");
}

void Pantalla::mensajeComandoNoValido(const String& cmd) {
    Serial.print(" Comando no reconocido: ");
    Serial.println(cmd);
    Serial.println("Solo se admite: 'ver'");
}
