#include "lcd_display.h"

LcdDisplay::LcdDisplay() : LiquidCrystal_I2C(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS) {}

void LcdDisplay::iniciar() {
  Wire.begin(21, 22); // Pines SDA y SCL por defecto en algunos ESP32
  init();
  backlight();
}

void LcdDisplay::mostrarLinea(uint8_t numeroLinea, const char* mensaje) {
  if (numeroLinea < LCD_ROWS) {
    setCursor(0, numeroLinea);
    print(mensaje);
  }
}

void LcdDisplay::borrarLinea(uint8_t numeroLinea) {
  if (numeroLinea < LCD_ROWS) {
    setCursor(0, numeroLinea);
    for (int i = 0; i < LCD_COLUMNS; i++) {
      print(" ");
    }
  }
}

void LcdDisplay::encenderDisplay() {
  display(); // Enciende el display (sin la luz de fondo)
  backlight(); // Asegura que la luz de fondo también esté encendida
}

void LcdDisplay::apagarDisplay() {
  noDisplay(); // Apaga el display (sin afectar la luz de fondo)
  noBacklight(); // Opcionalmente, también apaga la luz de fondo
}