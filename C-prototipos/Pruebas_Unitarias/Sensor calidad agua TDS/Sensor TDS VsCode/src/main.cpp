#include <Arduino.h>

// Configuración del sensor TDS
const int sensorPin = 34;          // Pin analógico
const float voltajeReferencia = 3.3; // Voltaje de referencia del ESP32
const int resolucionADC = 4095;    // Resolución ADC 12 bits
const float factorTDS = 0.5;       // Factor de calibración (ejemplo)

// Función para leer el voltaje del sensor
float leerVoltajeTDS() {
  int lecturaADC = analogRead(sensorPin);
  return (lecturaADC * voltajeReferencia) / resolucionADC;
}

// Función para convertir voltaje a TDS (ppm)
// Esta fórmula puede variar según el sensor, este es un ejemplo básico
float calcularTDS(float voltaje) {
  return (voltaje * 1000.0) / factorTDS; // Escalado simple
}

// Función de prueba unitaria
void testSensorTDS(float ppmMin, float ppmMax) {
  float voltaje = leerVoltajeTDS();
  float tds = calcularTDS(voltaje);

  Serial.print("Voltaje leído: ");
  Serial.print(voltaje);
  Serial.print(" V, TDS calculado: ");
  Serial.print(tds);
  Serial.println(" ppm");

  if (isnan(tds) || tds < 0) {
    Serial.println("TEST FAILED ❌ - Lectura inválida");
    return;
  }

  bool tdsOK = (tds >= ppmMin) && (tds <= ppmMax);

  if (tdsOK) {
    Serial.println("TEST PASSED ✅");
  } else {
    Serial.println("TEST FAILED ❌ - TDS fuera de rango");
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  delay(2000);

  Serial.println("Iniciando prueba del sensor TDS...");

  // Ejemplo: esperamos TDS entre 0 y 1000 ppm
  testSensorTDS(0.0, 1000.0);
}

void loop() {
  // Nada en el loop
}
