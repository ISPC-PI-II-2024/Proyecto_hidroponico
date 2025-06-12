#include "menu_recetas.h"

int opcionReceta = 0;

// Menú de selección de recetas
void mostrarMenuRecetas() {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(10, 10);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE);
    tft.println("[ SELECCIONAR CULTIVO ]");

    for (int i = 0; i < MAX_RECETAS; i++) {
        tft.println(opcionReceta == i ? "> " + String(recetas[i].nombre) : "  " + String(recetas[i].nombre));
    }
}

// Manejar selección con botones
void manejarSeleccionReceta() {
    if (digitalRead(BOTON_IZQUIERDA) == LOW) {
        opcionReceta = max(0, opcionReceta - 1);
    }
    if (digitalRead(BOTON_DERECHA) == LOW) {
        opcionReceta = min(MAX_RECETAS - 1, opcionReceta + 1);
    }
    
    if (digitalRead(BOTON_CONFIRMAR) == LOW) {
        seleccionarReceta(opcionReceta);
        mostrarRecetaActual();
    }
} 
