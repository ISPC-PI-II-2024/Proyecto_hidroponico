##codigo minnimo para sensor de lluvia analogico:

```cpp

#include <Arduino.h>

const int analogPin = 4;  // Puedes cambiar esto a 5 si prefieres usar ese pin
unsigned long previousMillis = 0;
const unsigned long interval = 500; // Intervalo de lectura en milisegundos

void setup() {
  Serial.begin(9600);  // Inicializa la comunicación serial
  Serial.println("Sensor de agua listo");
}

void loop() {
  unsigned long currentMillis = millis(); // Obtiene el tiempo actual

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Actualiza el último tiempo de ejecución

    int sensorValue = analogRead(analogPin); // Lee el valor analógico del sensor

    Serial.print("Nivel de agua: ");
    Serial.println(sensorValue); // Imprime el valor en el monitor serial

    if (sensorValue > 700) {  // Si el nivel de agua supera el umbral
      Serial.println("¡Agua detectada!");
    } else {
      Serial.println("No hay agua.");
    }
  }
}

``
