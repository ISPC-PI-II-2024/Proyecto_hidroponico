#include "pantalla.h"
#include "sensores.h"
#include "menu.h"

// Objeto para la pantalla
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);


// Variables de tiempo
unsigned long ultimoParpadeo = 0;
unsigned long ultimoAlerta = 0;
bool ojosAbiertos = true;

// Estado de la pantalla
bool modoStandby = false;
bool alertaAgua = false;


// Inicialización de la pantalla
void inicializarPantalla() {
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);

    // Animación dentro de la misma rutina
    for (int altura = 5; altura <= 30; altura += 2) {
        tft.fillRect(63, 80, 2, altura, ST7735_GREEN); // Dibuja el tallo creciendo
        tft.fillCircle(64, 80 - altura, 5, ST7735_GREEN); // Dibuja la hoja
        delay(200); 
        tft.fillRect(63, 80, 2, altura, ST7735_BLACK); // Borra el tallo anterior para la animación
    }

    pinMode(BOTON_IZQUIERDA, INPUT_PULLUP);
    pinMode(BOTON_DERECHA, INPUT_PULLUP);
    pinMode(BOTON_CONFIRMAR, INPUT_PULLUP);
}


// Actualización de pantalla
void actualizarPantalla() {
    if (menuVisible) {  // Solo mostrar menú si está activo
        mostrarMenuPrincipal();
    } else {
        if (modoStandby) {
            mostrarStandby();
        } else if (alertaAgua) {
            mostrarAlertaAgua();
        } else {
            mostrarDatos();
        }
    }
}


// Animación de ojos en standby
// Animación de ojos en standby
void mostrarStandby() {
    unsigned long tiempoActual = millis();
    if (tiempoActual - ultimoParpadeo > STANDBY_INTERVAL) {
        ojosAbiertos = !ojosAbiertos;
        ultimoParpadeo = tiempoActual;
    }

    tft.fillScreen(ST7735_BLACK);
    if (ojosAbiertos) {
        tft.fillCircle(40, 60, 15, ST7735_WHITE);
        tft.fillCircle(80, 60, 15, ST7735_WHITE);
    } else {
        tft.fillRect(30, 55, 60, 10, ST7735_WHITE);
    }
}

// Animación de alerta de agua
void mostrarAlertaAgua() {
    unsigned long tiempoActual = millis();
    if (tiempoActual - ultimoAlerta > ALERTA_INTERVAL) {
        alertaAgua = !alertaAgua;
        ultimoAlerta = tiempoActual;
    }

    tft.fillScreen(alertaAgua ? ST7735_RED : ST7735_BLACK);
    tft.setCursor(20, 50);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(2);
    tft.println("¡AGUA BAJA!");
}

// Manejo de botones para navegación
void manejarBotones() {
    if (digitalRead(BOTON_IZQUIERDA) == LOW) {
        modoStandby = !modoStandby;
        menuVisible = false;  // Volver a activar el dashboard
        delay(200);
    }
    
    if (digitalRead(BOTON_DERECHA) == LOW) {
        alertaAgua = !alertaAgua;
        menuVisible = false;  // Volver a activar el dashboard
        delay(200);
    }
}


void mostrarDatos() {
    tft.setCursor(10, 10);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Temp Agua: " + String(temperaturaAgua) + "°C");
    tft.println("Nivel Agua: " + String(nivelAgua) + "cm");
    tft.println("pH: " + String(ph));
}

void mostrarMensaje(String mensaje) {
    Serial.println(mensaje);
}

void inicializarDashboard(){
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);

    dibujarIconos();   // Llamada correcta
    mostrarValores();  // Llamada correcta
}


void dibujarIconos() {
    tft.fillCircle(15, 30, 5, ST7735_BLUE);    // 🌡️ Termómetro - Temp Agua
    tft.fillCircle(80, 30, 5, ST7735_ORANGE);  // ☀️ Sol - Temp Ambiente
    tft.fillCircle(15, 60, 5, ST7735_CYAN);    // 💧 Gota de Agua - Nivel Agua
    tft.fillCircle(80, 60, 5, ST7735_GREEN);   // 🔄 Grifo - Caudalímetro
    tft.fillCircle(15, 90, 5, GRAY);  // 🌫️ CO₂ - Medición de gas
    tft.fillCircle(80, 90, 5, ST7735_YELLOW);  // 💡 Luz - Sensor de luz
    tft.fillRect(10, 120, 12, 6, ST7735_RED);  // 🔋 Corriente - Medición eléctrica
    tft.fillCircle(80, 120, 5, ST7735_CYAN);   // ⚗️ pH - Medición química
}

void mostrarValores() {
    tft.setCursor(25, 25); tft.print(temperaturaAgua); tft.print(" C");
    tft.setCursor(90, 25); tft.print(temperaturaAire); tft.print(" C");
    tft.setCursor(25, 55); tft.print(nivelAgua); tft.print(" cm");
    tft.setCursor(90, 55); tft.print(flujoAgua); tft.print(" L/min");
    tft.setCursor(25, 85); tft.print(gas); tft.print(" ppm");
    tft.setCursor(90, 85); tft.print(luz > 0 ? "SI" : "NO");
    tft.setCursor(25, 115); tft.print(corriente); tft.print(" A");
    tft.setCursor(90, 115); tft.print(ph); tft.print(" pH");
}


// **Actualizar valores en pantalla**
void actualizarDashboard() {
    tft.fillRect(25, 20, 50, 120, ST7735_BLACK); // Borra valores en lado izquierdo
    tft.fillRect(90, 20, 50, 120, ST7735_BLACK); // Borra valores en lado derecho
    mostrarValores();  // Redibuja los valores actualizados
    Serial.println("Dashboard actualizado con nuevos valores");
}

