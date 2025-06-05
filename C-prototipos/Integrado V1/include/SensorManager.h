#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Preferences.h>

#include "SensorBMP280.h"
#include "DHT11.h"
#include "GY302_BH1750.h"
#include "SR04.h"
#include "Caudalimetro.h"
#include "DS3231.h"
#include "SensorCO2.h"
#include "Modulo_de_Energia.h"

//===================================
// SensorManager
//===================================
//
// Se encarga de:
//   1) Inicializar y leer periódicamente todos los sensores.
//   2) Detectar fallos de lectura y aislar sensores erráticos.
//   3) Cargar umbrales desde NVS (Preferences) para cada sensor.
//   4) Generar un JSON con todas las lecturas y estados:
//
//     {
//       "timestamp": "2025-06-05T12:34:56Z",
//       "RTC":       { "hora": "2025-06-05 12:34:56", "estado": "OK" },
//       "BMP280":    { "temp": 23.4, "presion": 1013.2, "altura": 120.5, "estado": "OK" },
//       "DHT11":     { "temp": 24.1, "hum": 55.1,   "estado": "OK" },
//       "BH1750":    { "lux": 450.0,              "estado": "OK" },
//       "HC-SR04":   { "dist": 35.7,              "estado": "OK" },
//       "Caudal":    { "caudal":4.2, "volumen":12.3, "estado": "OK" },
//       "CO2":       { "valor": 420.5, "unidad": "ppm", "estado": "OK" },
//       "Energia":   { "voltaje": 5.0, "corriente": 0.12, "potencia":0.60, "estado":"OK" }
//     }
//

// Definición de pines por defecto (ajustar según tu placa)
#define PIN_CO2          35   // Pin analógico para CO2 (ADC1_7 en ESP32)
#define PIN_HC_SR04_TRIG 12
#define PIN_HC_SR04_ECHO 14

class SensorManager {
public:
  //-----------------------------------
  // Estados posibles del SensorManager
  //-----------------------------------
  enum class Estado : uint8_t {
    Inactivo,      // Aún no se llamó a comenzar()
    Leyendo,       // Ciclo normal de lecturas
    Error          // Algún fallo crítico en inicialización
  };

  //-----------------------------------
  SensorManager(                        // Constructor: inicializa instancias de cada sensor
    uint8_t i2c_sda, uint8_t i2c_scl,
    uint8_t pin_dht11,
    uint8_t pin_bh1750_sda, uint8_t pin_bh1750_scl,
    uint8_t pin_sr04_trig, uint8_t pin_sr04_echo,
    uint8_t pin_caudalimetro, float factor_caudal,
    uint8_t pin_co2
  );                      
  
  
  void comenzar();                      // Llamar en setup() para cargar umbrales y comenzar sensores
  void loop();                          // Llamar periódicamente en loop(): hace lecturas si toca
  String obtenerJson();                 // Devuelve un String con el JSON de todas las lecturas
  void setPulsosCaudal(int pulsos);     // caudalímetro: configurar cuántos pulsos detectó el hardware en el intervalo anterior (debe venir de un ISR o contaje externo)
  Estado getEstado() const;             // Obtener el estado general

private:
  // --------------------
  // Métodos internos
  // --------------------
  void cargarUmbralesDesdeNVS();
  void leerSensores();  // Hace la lectura de cada dispositivo
  void registrarLectura(const char* nombre,
                        float valor,
                        const char* unidad,
                        const char* estado,
                        JsonObject& nodo);

  // --------------------
  // Instancias de sensores
  // --------------------
  SensorBMP280    bmp_;
  DHT11Sensor     dht_;
  GY302           bh1750_;
  SR04            sr04_;
  Caudalimetro    caudalimetro_;
  DS3231Clock     rtc_;
  SensorCO2       co2_;
  Modulo_de_Energia moduloEnergia_;

  // --------------------
  // Últimas lecturas
  // --------------------
  // BMP280
  float tempBMP_;
  float presionBMP_;
  float alturaBMP_;

  // DHT11
  float tempDHT_;
  float humDHT_;

  // BH1750
  float luxBH_;

  // HC-SR04
  float distSR_;

  // Caudalímetro (calculado internamente)
  int   pulsosCapturados_;
  float caudal_;
  float volumenAcum_;

  // DS3231 (RTC)
  String horaRTC_;

  // CO2 (MQ)
  float co2PPM_;

  // Módulo de energía
  float voltaje_;
  float corriente_;
  float potencia_;

  // --------------------
  // Umbrales (cargados desde NVS)
  // --------------------
  float umbralTempMax_;     // °C (BMP280 / DHT11)
  float umbralHumMin_;      // % (DHT11)
  float umbralPresionMin_;  // hPa (BMP280)
  float umbralLuxMin_;      // lux (BH1750)
  float umbralDistMax_;     // cm (HC-SR04)
  float umbralCaudalMin_;   // L/min (Caudalímetro)
  float umbralCO2Max_;      // ppm (SensorCO2)
  float umbralVoltMin_;     // V (Módulo de Energía)
  float umbralCorrMin_;     // A (Módulo de Energía)

  // --------------------
  // NVS (Preferences)
  // --------------------
  Preferences prefs_;

  // --------------------
  // Intervalo y temporizador
  // --------------------
  unsigned long intervaloLecturaMs_;
  unsigned long ultimaLecturaMs_;

  // --------------------
  // Estado interno
  // --------------------
  Estado estado_;

  // --------------------
  // JSON buffer (reutilizado)
  // --------------------
  StaticJsonDocument<768> jsonDoc_;  
};

