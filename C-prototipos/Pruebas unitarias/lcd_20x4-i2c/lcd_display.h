#ifndef lcd_display_h
#define lcd_display_h

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Definiciones para la dirección I2C y las dimensiones del LCD
#define LCD_ADDRESS 0x27 // O puedes usar 0x3F si tu módulo es diferente
#define LCD_COLUMNS 20
#define LCD_ROWS 4

class LcdDisplay : public LiquidCrystal_I2C {
public:
  LcdDisplay();
  void iniciar();
  void mostrarLinea(uint8_t numeroLinea, const char* mensaje);
  void borrarLinea(uint8_t numeroLinea);
  void encenderDisplay();
  void apagarDisplay();
};

#endif
