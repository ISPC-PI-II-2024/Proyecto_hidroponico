#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <DHTesp.h>
#include <OneWire.h>
// #include <Adafruit_INA219.h>
#include <Adafruit_INA219.h>

// Definiciones de pines para sensores
#define DHT_PIN 33
#define DHT_TYPE DHT11
#define DS18B20_PIN 25
#define FLOW_SENSOR_PIN 34
#define LDR_PIN 4
#define GAS_SENSOR_PIN 36
#define ULTRASONIC_TRIG_PIN 26
#define ULTRASONIC_ECHO_PIN 32
#define PH_SENSOR_PIN 35

// Variables globales para sensores
extern float temperaturaAgua;
extern float temperaturaAire;
extern float humedadAire;  // Cambiado de humedad a humedadAire
extern float nivelAgua;
extern float flujoAgua;
extern int luz;
extern int gas;
extern float corriente;
extern float voltaje;
extern float potencia;
extern float ph;  // Cambiado de phValor a ph

// Variables para activar/desactivar sensores dinámicamente
extern bool sensorTempAguaActivo;
extern bool sensorAireActivo;
extern bool sensorHumedadActivo;
extern bool sensorNivelAguaActivo;
extern bool sensorFlujoActivo;
extern bool sensorLuzActivo;
extern bool sensorGasActivo;
extern bool sensorCorrienteActivo;
extern bool sensorPHActivo;

// Variables para el sensor de flujo
extern volatile int pulsosFlujo;
extern float factorCalibracion;

// Función para la interrupción del contador de pulsos
void IRAM_ATTR contadorPulsos();

// Inicialización de sensores
void inicializarSensores();

// Funciones de lectura con validación de activación/desactivación
bool leerTemperaturaAgua();
bool leerDHT();
bool leerNivelAgua();
bool calcularFlujoAgua();
bool leerLuz();
bool leerGas();
bool leerCorriente();
bool leerPH();

#endif