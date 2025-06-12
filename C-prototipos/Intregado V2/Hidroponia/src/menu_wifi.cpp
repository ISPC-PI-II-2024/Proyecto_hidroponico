#include "menu_wifi.h"

int redSeleccionadaIndex = 0;
int numRedes = 0;
String redesDisponibles[10];

// Mostrar menú WiFi
void mostrarMenuWiFi() {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(10, 10);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE);
    tft.println("[ CONFIGURAR WIFI ]");
    
    numRedes = WiFi.scanNetworks();
    for (int i = 0; i < numRedes && i < 10; i++) {
        redesDisponibles[i] = WiFi.SSID(i);
        tft.println(redSeleccionadaIndex == i ? "> " + redesDisponibles[i] : "  " + redesDisponibles[i]);
    }
}

// Manejar selección de red
void manejarSeleccionWiFi() {
    if (digitalRead(BOTON_IZQUIERDA) == LOW) {
        redSeleccionadaIndex = max(0, redSeleccionadaIndex - 1);
    }
    if (digitalRead(BOTON_DERECHA) == LOW) {
        redSeleccionadaIndex = min(numRedes - 1, redSeleccionadaIndex + 1);
    }

    if (digitalRead(BOTON_CONFIRMAR) == LOW) {
        strncpy(redSeleccionada, redesDisponibles[redSeleccionadaIndex].c_str(), MAX_SSID_LENGTH);
        Serial.print("Red seleccionada: ");
        Serial.println(redSeleccionada);
    }
} 
