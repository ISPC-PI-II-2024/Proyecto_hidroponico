#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <stdint.h>
#include <string>

// Define la dirección I2C y las dimensiones del LCD (puedes modificarlas aquí)
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 20
#define LCD_ROWS 4

/// Se llama en el setup() - Inicializa el LCD
void lcdBegin();

/// Se llama para escribir texto en una posición específica (columna, fila).
void lcdSetCursor(uint8_t col, uint8_t row);
void lcdPrint(const char* text);
void lcdPrint(const std::string& text);

/// Se llama para escribir una línea completa (comenzando desde la columna 0).
void lcdWriteLine(uint8_t row, const char* text);
void lcdWriteLine(uint8_t row, const std::string& text);

/// Se llama para borrar completamente la pantalla LCD.
void lcdClear();

/// Se llama para encender la luz de fondo.
void lcdBacklight();

/// Se llama para apagar la luz de fondo.
void lcdNoBacklight();

#endif // LCD_DISPLAY_H