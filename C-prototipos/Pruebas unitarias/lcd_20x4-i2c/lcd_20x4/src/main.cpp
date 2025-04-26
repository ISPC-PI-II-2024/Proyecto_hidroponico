#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <lcd_display.h>

LcdDisplay lcd;

void setup() {
  Serial.begin(115200);
  lcd.iniciar();
  lcd.mostrarLinea(0, "Wokwi Prueba 1");
  lcd.mostrarLinea(1, "Linea Dos");
  lcd.mostrarLinea(2, "Tercera Linea");
  lcd.mostrarLinea(3, "Final Linea");
}

void loop() {
  // Tu lógica aquí
  delay(1000);
}