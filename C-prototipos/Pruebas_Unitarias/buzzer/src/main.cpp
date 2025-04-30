#include <Arduino.h>
#include "buzzer.h"

static const uint8_t BUZZER_PIN = 15;

Buzzer buzzer(BUZZER_PIN);
AlarmLevel currentLevel = A_NONE;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  buzzer.begin();
  buzzer.setLevel(currentLevel);
  Serial.println("=== Prueba Buzzer iniciada ===");
  Serial.println("Ingrese: bajo | medio | alto");
}

void loop() {
  // 1) Actualizo estado del buzzer
  buzzer.update();

  // 2) Verifico si hay datos en el puerto serial
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // Elimina espacios y saltos de línea

    // 3) Actualizo el nivel de alarma según la entrada por consola
    if (input.equalsIgnoreCase("bajo")) {
      currentLevel = A_LOW;
    } else if (input.equalsIgnoreCase("medio")) {
      currentLevel = A_MEDIUM;
    } else if (input.equalsIgnoreCase("alto")) {
      currentLevel = A_HIGH;
    } else {
      currentLevel = A_NONE;
    }

    buzzer.setLevel(currentLevel);
    Serial.print("Nivel de alarma actualizado: ");
    Serial.println(input);
  }
}

