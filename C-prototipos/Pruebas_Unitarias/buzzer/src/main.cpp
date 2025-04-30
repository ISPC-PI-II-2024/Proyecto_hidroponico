#include <Arduino.h>
#include "buzzer.h"

static const uint8_t BUZZER_PIN       = 15;
static const unsigned long INTERVAL   = 5000; // Cambia nivel cada 5 s

Buzzer buzzer(BUZZER_PIN);
AlarmLevel levels[] = { A_LOW, A_MEDIUM, A_HIGH, A_NONE };
size_t      idx     = 0;
unsigned long lastChange = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  buzzer.begin();
  buzzer.setLevel(levels[idx]);
  Serial.println("=== Prueba Buzzer iniciada ===");
}

void loop() {
  // 1) Actualizo estado del buzzer
  buzzer.update();

  // 2) Cada INTERVAL cambio nivel de alarma y lo muestro
  unsigned long now = millis();
  if (now - lastChange >= INTERVAL) {
    idx = (idx + 1) % (sizeof(levels)/sizeof(levels[0]));
    buzzer.setLevel(levels[idx]);

    Serial.print("Nivel de alarma: ");
    Serial.println(idx);  // 0=None,1=Low,2=Med,3=High

    lastChange = now;
  }
}
