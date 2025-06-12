#include "transmisor.h"

bool esNodoTransmisor = true; // Modifica este valor para activar o desactivar el modo transmisor

void inicializarTransmisor() {
    Serial.println("Modo: TRANSMISOR inicializado");
    // Aquí podrías agregar inicializaciones específicas para la transmisión (por ejemplo, configurar BLE Mesh o LoRa si fuera necesario)
}

