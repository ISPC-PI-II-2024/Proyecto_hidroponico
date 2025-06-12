#ifndef PANTALLA_H
#define PANTALLA_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>



#define TFT_DC 12    // A0
#define TFT_CS 13    // CS
#define TFT_MOSI 14  // SDA
#define TFT_CLK 27   // SCK
#define TFT_RST 0    // RST
#define TFT_MISO 0   // MISO (no siempre necesario)


// Definiciones para botones de navegación
#define BOTON_IZQUIERDA 42
#define BOTON_DERECHA   41
#define BOTON_CONFIRMAR 4

// Definiciones de animaciones
#define STANDBY_INTERVAL 5000    // Tiempo entre parpadeo de ojos
#define ALERTA_INTERVAL  500     // Tiempo entre parpadeo de alerta de agua

#define DARK_GREEN 0x03E0  // Verde oscuro
#define LIGHT_GREEN 0x07E0 // Verde claro
#define BROWN 0x79E0       // Marrón para el suelo
#define YELLOW 0xFFE0      // Amarillo para el centro de la flor
#define RED 0xF800         // Rojo para los pétalos
#define ORANGE 0xFC00      // Naranja para los detalles de la flor
#define BLUE 0x001F       // Azul para el fondo
#define GRAY 0x8410  // Un gris neutral basado en RGB


extern Adafruit_ST7735 tft;

void inicializarPantalla();
void actualizarPantalla();
void mostrarStandby();
void mostrarAlertaAgua();
void manejarBotones();
void mostrarDatos();
void mostrarMensaje(String mensaje);
void dibujarIconos();
void mostrarValores();
void actualizarDashboard();
void inicializarDashboard();


#endif 
