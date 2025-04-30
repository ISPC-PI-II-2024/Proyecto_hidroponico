#include <Arduino.h>
#include "SR04.h"

static const uint8_t TRIG_PIN = 5;
static const uint8_t ECHO_PIN = 18;

// Intervalo entre lecturas. En ms. Lo defino en 3 segundos para no saturar el monitor serial
static const unsigned long PRINT_INTERVAL = 3000UL; // ms

SR04 sensorSR04_1; // Instancio el objeto sensorSR04_1 de la clase SR04

void setup() {

  Serial.begin(115200);
  while (!Serial)

 // Arranco el la instancia con los pines definidos arriba
  sensorSR04_1.begin(TRIG_PIN, ECHO_PIN);
  Serial.println("=== Prueba SR04 iniciada ===");
}

void loop() {
  // Llamo a la lectura de la instancia de lecutra, y lo coloco en una variable
  float distancia = sensorSR04_1.read();

  // Presento la variable en consola
  if (distancia < 0) {
    Serial.println("Timeout: sin eco");
  } else {
    Serial.print("Distancia: ");
    Serial.print(distancia, 1);
    Serial.println(" cm");
  }

  // Esperar intervalo deifnido al principio y arranco devuelta.
  delay(PRINT_INTERVAL);
}
