#include <Arduino.h>

const int mq2Pin = 34;        // Pin analógico para MQ-2
const int ledPin = 13;         // Pin digital para el LED de alerta
const int umbralPPM = 1000;   // Límite de CO2 simulado

// Función para leer el voltaje del sensor
float leerVoltajeSensor() {
  int lectura = analogRead(mq2Pin);
  return lectura * (3.3 / 4095.0);  // Convierte lectura ADC a voltaje
}

// Simula conversión a ppm (partes por millón)
int convertirVoltajeACo2ppm(float voltaje) {
  int mV = voltaje * 1000;
  int ppm = map(mV, 400, 3000, 400, 5000);  // Simula nivel de CO2
  return constrain(ppm, 400, 5000);         // Limita valores
}

void setup() {
  Serial.begin(115200);
  pinMode(mq2Pin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  float voltaje = leerVoltajeSensor();
  int co2ppm = convertirVoltajeACo2ppm(voltaje);

  Serial.print("Voltaje leído: ");
  Serial.print(voltaje, 2);
  Serial.print(" V | CO2 simulado: ");
  Serial.print(co2ppm);
  Serial.print(" ppm");

  if (co2ppm > umbralPPM) {
    Serial.println(" ⚠️ ¡ALERTA! Alta concentración de CO₂");
    digitalWrite(ledPin, HIGH);  // Enciende LED de alerta
  } else {
    Serial.println(" ✅ Aire aceptable");
    digitalWrite(ledPin, LOW);   // Apaga LED
  }

  delay(1000);
}
