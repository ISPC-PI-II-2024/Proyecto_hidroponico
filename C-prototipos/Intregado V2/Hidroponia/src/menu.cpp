#include "menu.h"
#include <Arduino.h>
#include "pantalla.h"


// Opciones del menú
int opcionMenu = 0;
bool pantallaNecesitaActualizacion = true;  // Inicialización global
extern bool menuVisible;


void mostrarMenuPrincipal() {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(10, 10);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE);
    tft.println("Menú Principal");
    tft.println(opcionMenu == 0 ? "> Sensores" : "  Sensores");
    tft.println(opcionMenu == 1 ? "> Control" : "  Control");
    tft.println(opcionMenu == 2 ? "> Recetas" : "  Recetas");
    tft.println(opcionMenu == 3 ? "> WiFi" : "  WiFi");
    tft.println(opcionMenu == 4 ? "> Activar/Desactivar WiFi" : "  Activar/Desactivar WiFi");
    tft.println(opcionMenu == 5 ? "> Activar/Desactivar LoRa" : "  Activar/Desactivar LoRa");
    tft.println(opcionMenu == 6 ? "> Activar/Desactivar BLE Mesh" : "  Activar/Desactivar BLE Mesh");
    tft.println(opcionMenu == 7 ? "> Activar/Desactivar Sensor de pH" : "  Activar/Desactivar Sensor de pH");
    tft.println(opcionMenu == 8 ? "> Activar/Desactivar Sensor de Flujo de Agua" : "  Activar/Desactivar Sensor de Flujo de Agua");
    tft.println(opcionMenu == 9 ? "> Activar/Desactivar Sensor de Temperatura del Agua" : "  Activar/Desactivar Sensor de Temperatura del Agua");
    menuVisible = true;  // Indicar que el menú está en pantalla

}

/*
void manejarSeleccionMenu() {
    if (digitalRead(BOTON_IZQUIERDA) == LOW) {
        opcionMenu = max(0, opcionMenu - 1);
        pantallaNecesitaActualizacion = true;
        delay(200);
    }
    if (digitalRead(BOTON_DERECHA) == LOW) {
        opcionMenu = min(9, opcionMenu + 1);
        pantallaNecesitaActualizacion = true;
        delay(200);
    }

    if (digitalRead(BOTON_CONFIRMAR) == LOW) {
        Serial.println("Opción seleccionada: " + String(opcionMenu));

        switch (opcionMenu) {
            case 2:
                mostrarMenuRecetas();
                break;
            case 3:
                mostrarMenuWiFi();
                break;
            case 4:
                wifiActivo = !wifiActivo;
                mostrarMensaje("WiFi " + String(wifiActivo ? "Activado" : "Desactivado"));
                break;
            case 5:
                loRaActivo = !loRaActivo;
                mostrarMensaje("LoRa " + String(loRaActivo ? "Activado" : "Desactivado"));
                break;
            case 6:
                bleActivo = !bleActivo;
                mostrarMensaje("BLE " + String(bleActivo ? "Activado" : "Desactivado"));
                break;
            case 7:
                sensorPHActivo = !sensorPHActivo;
                mostrarMensaje("Sensor pH " + String(sensorPHActivo ? "Activado" : "Desactivado"));
                break;
            case 8:
                sensorFlujoActivo = !sensorFlujoActivo;
                mostrarMensaje("Sensor Flujo " + String(sensorFlujoActivo ? "Activado" : "Desactivado"));
                break;
            case 9:
                sensorTempAguaActivo = !sensorTempAguaActivo;
                mostrarMensaje("Sensor Temp Agua " + String(sensorTempAguaActivo ? "Activado" : "Desactivado"));
                break;
        }
        
        pantallaNecesitaActualizacion = true;  // Indica que hay que actualizar la pantalla
        menuVisible = true;
        delay(500);
    }

}
*/