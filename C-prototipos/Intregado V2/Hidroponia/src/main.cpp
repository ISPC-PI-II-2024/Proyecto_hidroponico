#include <Arduino.h>

// Módulos del sistema
#include "pantalla.h"
#include "sensores.h"
#include "control.h"
#include "comunicacion.h"
#include "rtc.h"
#include "recetas.h"
#include "menu.h"
#include "menu_recetas.h"
#include "menu_wifi.h"
#include "transmisor.h"
#include "receptor.h"
#include "gsm_async.h"
#include "wifi_manager.h"
#include "mqtt_async.h"

// Variables globales
unsigned long ultimaLectura = 0;
const unsigned long intervaloLectura = 5000;
extern bool pantallaNecesitaActualizacion;
bool menuVisible = false;  // Asegurar inicialización

void setupPantalla() {
  SPI.begin(TFT_CLK, TFT_MISO, TFT_MOSI, TFT_CS);
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.fillRect(0, 150, 128, 10, BROWN);

  for (int altura = 5; altura <= 80; altura += 3) {
    tft.fillRect(55, 140 - altura, 18, altura, DARK_GREEN);
    tft.fillRect(55, 140 - altura, 18, altura - 4, LIGHT_GREEN);

    if (altura > 15) {
      tft.fillCircle(45, 140 - altura, 10, LIGHT_GREEN);
      tft.fillCircle(75, 140 - altura, 10, LIGHT_GREEN);
      tft.fillRect(45, 140 - altura - 5, 30, 10, LIGHT_GREEN);
    }
    if (altura > 30) {
      tft.fillCircle(40, 140 - altura - 10, 12, DARK_GREEN);
      tft.fillCircle(80, 140 - altura - 10, 12, DARK_GREEN);
      tft.fillRect(40, 140 - altura - 15, 40, 12, DARK_GREEN);
    }
    if (altura > 50) {
      tft.fillCircle(55, 140 - altura - 15, 14, LIGHT_GREEN);
      tft.fillCircle(73, 140 - altura - 15, 14, LIGHT_GREEN);
      tft.fillRect(55, 140 - altura - 20, 18, 14, LIGHT_GREEN);
    }

    delay(250);
  }

  tft.fillCircle(64, 40, 8, RED);
  tft.fillCircle(64, 40, 6, ORANGE);
  tft.fillCircle(64, 40, 4, YELLOW);
  delay(5000);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println(" Iniciando sistema hidroponico...");

  // Pantalla bienvenida
  setupPantalla();

  // Espera a que WiFi se establezca completamente
  conectarWiFi();
  
  // Asegura que WiFi esté completamente establecido
  delay(1000);

  // Configuración MQTT
  mqttSetup(MQTT_BROKER, MQTT_PORT);
  mqttSetCallback(recibirMensajeMQTT);
  
  // Intenta conectar directamente
  Serial.println("Conectando directamente a MQTT...");
  getMqttClient().connect();
  
  // Suscripción a temas (solo después de conectar)
  mqttSubscribe("hidroponia/control", 1);


  // Sistema
  inicializarSensores();
  inicializarRTC();
  iniciarEEPROM();
  inicializarDashboard();
  gsmSetup("igprs.claro.com.ar", "", "");

  // Primera lectura para no arrancar vacío
  leerDHT();
  leerLuz();
  leerGas();
  enviarInfoDispositivo();

  Serial.println(" Sistema listo.");
}

void loop() {
  // Interacción con menú
  if (digitalRead(BOTON_CONFIRMAR) == HIGH) {
    delay(200);  // debounce
    menuVisible = !menuVisible;

    if (menuVisible) {
      mostrarMenuPrincipal();
    } else {
      actualizarDashboard();
    }
  }

  // Lectura periódica de sensores
  if (millis() - ultimaLectura >= intervaloLectura) {
    ultimaLectura = millis();

    Serial.println("===== ACTUALIZANDO SENSORES =====");

    if (sensorTempAguaActivo) leerTemperaturaAgua();
    if (sensorAireActivo) leerDHT();
    if (sensorNivelAguaActivo) leerNivelAgua();
    if (sensorFlujoActivo) calcularFlujoAgua();
    if (sensorLuzActivo) leerLuz();
    if (sensorGasActivo) leerGas();
    if (sensorCorrienteActivo) leerCorriente();
    if (sensorPHActivo) leerPH();

    obtenerHora();
    enviarDatos();

    if (!menuVisible) {
      actualizarDashboard();
    }

    Serial.println("===== SENSORES ACTUALIZADOS =====");
    
  }
if (WiFi.status() == WL_CONNECTED && !mqttConectado) {
  static unsigned long ultimoIntentoMQTT = 0;
  unsigned long ahora = millis();
  
  // Intenta reconectar cada 5 segundos
  if (ahora - ultimoIntentoMQTT > 5000) {
    ultimoIntentoMQTT = ahora;
    Serial.println("Intentando reconectar MQTT...");
    getMqttClient().connect();
  }
 }

  delay(10);
}
