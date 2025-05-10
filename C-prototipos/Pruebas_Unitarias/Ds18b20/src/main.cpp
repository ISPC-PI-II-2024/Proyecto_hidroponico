#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DATOS 4 //  DS18B20

OneWire oneWire(DATOS);  // ConfiOneWire
DallasTemperature sensores(&oneWire);  // Crear instancia del sensor

void setup() {
  Serial.begin(115200); 
  sensores.begin();  // Iniciar el sensor
  
  Serial.println("iniciando test del sensor DS18B20");
}

void loop() {
  sensores.requestTemperatures();  // pedir lectura de temperatura
  
  float temperatura = sensores.getTempCByIndex(0);  // leer temperatura
  
  if (temperatura == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Sensor desconectado");
  } else {
    Serial.print("temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");
  }
  
  delay(2000);  
}
