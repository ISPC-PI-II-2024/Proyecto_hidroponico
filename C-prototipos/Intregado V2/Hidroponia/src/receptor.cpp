#include "receptor.h"

bool esNodoReceptor = false; // Cambia este valor a true para que este nodo funcione como receptor

void inicializarReceptor() {
    Serial.println("Modo: RECEPTOR inicializado");
    // Inicializaciones adicionales para el receptor pueden ir aquí.
}

void procesarDatos(const char* mensaje) {
    if (!esNodoReceptor)
        return;

    Serial.println("Procesando datos recibidos:");
    Serial.println(mensaje);

    // Ejemplo: si el mensaje contiene una instrucción de encender la iluminación
    if (strstr(mensaje, "\"iluminacion\":true")) {
        activarIluminacion(true);
        Serial.println("Comando recibido: Activar iluminación.");
    }
    
    // Aquí puedes agregar más condiciones para procesar otros comandos o ajustes.
} 
