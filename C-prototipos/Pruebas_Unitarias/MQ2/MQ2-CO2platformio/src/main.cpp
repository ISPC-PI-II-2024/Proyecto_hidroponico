// main.cpp
#include <Arduino.h>
#include "lcd.h"

// Función de configuración del microcontrolador
void setup() {
    setupLCD();  // Llama a la función de configuración del LCD
}

// Función de bucle que se ejecuta continuamente
void loop() {
    loopLCD();   // Llama al bucle de la pantalla LCD
}
