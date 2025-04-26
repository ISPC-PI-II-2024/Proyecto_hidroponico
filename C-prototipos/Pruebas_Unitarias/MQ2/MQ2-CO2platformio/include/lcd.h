// lcd.h
#ifndef LCD_H
#define LCD_H

#include <LiquidCrystal_I2C.h>

// Dirección I2C, columnas y filas
extern LiquidCrystal_I2C lcd; 

void setupLCD();
void loopLCD();

#endif
