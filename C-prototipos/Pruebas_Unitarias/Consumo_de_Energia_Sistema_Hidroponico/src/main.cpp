#include <Arduino.h> // Incluye la biblioteca principal de Arduino para usar sus funciones

// Declaración de variables globales
float corriente = 0.0;  // Variable para almacenar el valor de la corriente en amperios
float voltaje = 0.0;    // Variable para almacenar el valor del voltaje en voltios
float R = 10000.0;      // Resistencia en ohmios utilizada para calcular la corriente

void setup() {
  Serial.begin(115200); // Inicializa la comunicación serial a 115200 baudios
  Serial.println("Hello, ESP32!"); // Imprime un mensaje inicial en el monitor serial
}

void loop() {
  // Bucle para realizar 100 lecturas del pin analógico y calcular la corriente promedio
  for(int i = 0; i < 100; i++) {
    // Lee el valor analógico del pin 36, lo convierte a voltaje y calcula la corriente
    corriente = (float) analogRead(36) * (3.3 / 4095.0) / R + corriente;
  }

  // Lee el valor analógico del pin 36 y lo convierte a voltaje
  voltaje = analogRead(36) * (3.3 / 4095.0);

  // Imprime la corriente promedio en miliamperios con 4 decimales
  Serial.print(corriente * 1000 / 100, 4);
  Serial.println(" mA.");

  // Imprime el voltaje leído con 3 decimales
  Serial.print(voltaje, 3);
  Serial.println(" V.");

  // Reinicia la variable de corriente para el próximo cálculo
  corriente = 0;
}