#include "recetas.h"

Receta recetas[MAX_RECETAS] = {
    {"Lechuga", 20.0, 22.0, 70.0, 10.0, 6.2, true},
    {"Tomate", 22.0, 25.0, 65.0, 15.0, 5.8, true},
    {"Albahaca", 21.0, 23.0, 75.0, 12.0, 6.5, true},
    {"Perejil", 19.0, 21.0, 80.0, 8.0, 6.0, false},
    {"Fresas", 20.0, 24.0, 68.0, 12.0, 5.5, true}
};
int recetaSeleccionada = 0;

void iniciarEEPROM() {
    Serial.println("Inicializando EEPROM...");
    
    // Iniciar EEPROM con un tamaño de 512 bytes
    if (EEPROM.begin(512)) {
        Serial.println("EEPROM iniciada correctamente ");
    } else {
        Serial.println("Error al iniciar EEPROM ");
    }
}


// Cargar recetas desde EEPROM
void cargarRecetas() {
    EEPROM.begin(512);
    EEPROM.get(0, recetas);
}

// Guardar recetas en EEPROM
void guardarRecetas() {
    EEPROM.put(0, recetas);
    EEPROM.commit();
}

// Seleccionar receta de cultivo
void seleccionarReceta(int indice) {
    if (indice >= 0 && indice < MAX_RECETAS) {
        recetaSeleccionada = indice;
        guardarRecetas();
    }
}

// Mostrar receta actual en pantalla
void mostrarRecetaActual() {
    Receta r = recetas[recetaSeleccionada];
    Serial.println("Cultivo seleccionado: " + String(r.nombre));
    Serial.println("Temp Agua: " + String(r.tempAgua) + "C");
    Serial.println("Temp Aire: " + String(r.tempAire) + "C");
    Serial.println("Humedad: " + String(r.humedad) + "%");
    Serial.println("Nivel Agua: " + String(r.nivelAgua) + "cm");
    Serial.println("pH: " + String(r.ph));
    Serial.println("Iluminación: " + String(r.iluminacion ? "Activada" : "Desactivada"));
} 
