#include <Arduino.h>
#include "Modulo_de_Energia.h" // Incluye tu librería personalizada para el módulo de energía

// La instancia global de Wire es proporcionada por el framework Arduino
Modulo_de_Energia energyMonitor(Wire); // Crea un objeto para manejar el módulo de energía

void setup() {
  Serial.begin(115200); // Inicializa la comunicación serial a 115200 baudios
  while (!Serial) {
    ; // Espera a que el puerto serial se conecte. Necesario para algunas placas.
  }
  Serial.println("Iniciando Módulo de Consumo de Energía...");
  
  energyMonitor.begin(); // Inicializa el módulo de energía

  if (energyMonitor.isCommunicationOK()) {
    Serial.println("Módulo de energía inicializado correctamente.");
  } else {
    Serial.println("Error al inicializar el módulo de energía.");
  }
}

void loop() {
  if (energyMonitor.isCommunicationOK()) {
    // Si la comunicación es correcta, obtiene y muestra los valores de voltaje, corriente y potencia
    float voltage = energyMonitor.getVoltage();
    float current = energyMonitor.getCurrent();
    float power = energyMonitor.getPower();

    Serial.print("Voltaje: "); Serial.print(voltage, 3); Serial.println(" V");
    Serial.print("Corriente: "); Serial.print(current, 3); Serial.println(" A");
    Serial.print("Potencia: "); Serial.print(power, 3); Serial.println(" W");
  } else {
    // Si hay un error de comunicación, intenta reinicializar el módulo
    Serial.println("Error de comunicación con el sensor de energía. Intentando reinicializar...");
    energyMonitor.begin(); // Intenta reinicializar en caso de error continuo
  }
  
  delay(5000); // Espera 5 segundos entre lecturas
}