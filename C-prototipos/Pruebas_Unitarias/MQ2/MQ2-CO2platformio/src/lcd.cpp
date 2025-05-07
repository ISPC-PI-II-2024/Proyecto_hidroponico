// lcd.cpp
#include "lcd.h"
#include <Arduino.h>

// Dirección I2C, columnas y filas (16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setupLCD() {
    lcd.begin(16, 2);  // Inicializa el LCD con 16 columnas y 2 filas
    lcd.print("CO2_ISPC");
}

void loopLCD() {
    delay(2000);  // Espera 2 segundo
}
