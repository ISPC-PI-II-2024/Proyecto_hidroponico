#include <Arduino.h>
#include "SensorBMP280.h"

SensorBMP280 bmp;
unsigned long Millis_anterior = 0;
const unsigned long intervalo = 5000; // 5 segundos

void setup() {
  Serial.begin(9600);
  if (bmp.begin()) {
    Serial.println("BMP280 conectado correctamente.");
  } else {
    Serial.println("Error al conectar el BMP280. Se intentará de nuevo periódicamente.");
  }
}

void loop() {
  unsigned long Millis_actual = millis();

  // Reintenta conexión si no está conectado
  if (!bmp.isConnected()) {
    static unsigned long ultimo_intento = 0;
    if (Millis_actual - ultimo_intento >= 10000) { // Reintenta cada 10 segundos
      ultimo_intento = Millis_actual;
      if (bmp.begin()) {
        Serial.println("Reconexión exitosa con el BMP280.");
      } else {
        Serial.println("Reintento fallido: BMP280 no conectado.");
      }
    }
    return; // No intentamos leer datos si no está conectado
  }

  if (Millis_actual - Millis_anterior >= intervalo) {
    Millis_anterior = Millis_actual;

    float temp = bmp.readTemperature();
    float presion = bmp.readPressure();
    float altitud = bmp.readAltitude();

    if (isnan(temp) || isnan(presion) || isnan(altitud)) {
      Serial.println("Error al leer datos del BMP280.");
      return;
    }

    Serial.println("-------------");
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println(" °C");

    Serial.print("Presión: ");
    Serial.print(presion);
    Serial.println(" hPa");

    Serial.print("Altitud estimada: ");
    Serial.print(altitud);
    Serial.println(" m");
  }
}
