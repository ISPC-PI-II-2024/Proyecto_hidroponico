// lcd.cpp
#include "lcd.h"
#include <Arduino.h>

// Dirección I2C, columnas y filas (16x2 en este caso)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setupLCD() {
    lcd.begin(16, 2);  // Inicializa el LCD con 16 columnas y 2 filas
    lcd.print("Hola Mundo");
}

void loopLCD() {
    delay(1000);  // Espera 1 segundo
}
