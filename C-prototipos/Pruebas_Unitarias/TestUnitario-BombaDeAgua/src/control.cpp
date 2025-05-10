#include <Arduino.h>
#include "control.h"

// Definición de pines
#define RELE_PIN 26
#define LED_VERDE 25
#define LED_ROJO 33

void setupControlPins() {
    pinMode(RELE_PIN, OUTPUT);
    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_ROJO, OUTPUT);

    apagarBomba(); // Estado inicial
    Serial.println("Pines configurados.");
}

void encenderBomba() {
    digitalWrite(RELE_PIN, HIGH);
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_ROJO, LOW);
    Serial.println("Bomba encendida. LED verde ON, LED rojo OFF");
}

void apagarBomba() {
    digitalWrite(RELE_PIN, LOW);
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_ROJO, HIGH);
    Serial.println("Bomba apagada. LED verde OFF, LED rojo ON");
}
