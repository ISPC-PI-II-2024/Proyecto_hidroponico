#include <Arduino.h>
#include <GY302_BH1750.h>

GY302 sensor;
const int LED_PIN = 2; // LED interno

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  
  if (!sensor.begin()) {
    Serial.println("¡Error en sensor!");
    while (1);
  }
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  float lux = sensor.readLightLevel();
  Serial.print("Luz: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(9500); // Total 10 segundos
}