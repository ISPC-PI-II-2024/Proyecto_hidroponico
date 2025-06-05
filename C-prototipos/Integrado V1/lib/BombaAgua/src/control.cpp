#include "Control.h"

ControlBomba::ControlBomba(uint8_t relePin, uint8_t ledVerde, uint8_t ledRojo)
    : _relePin(relePin), _ledVerde(ledVerde), _ledRojo(ledRojo) {}

void ControlBomba::setup() {
    pinMode(_relePin, OUTPUT);
    pinMode(_ledVerde, OUTPUT);
    pinMode(_ledRojo, OUTPUT);

    apagarBomba(); // Estado inicial
    Serial.println("Pines de control configurados.");
}

void ControlBomba::encenderBomba() {
    digitalWrite(_relePin, HIGH);
    digitalWrite(_ledVerde, HIGH);
    digitalWrite(_ledRojo, LOW);
    Serial.println("Bomba encendida. LED verde ON, LED rojo OFF");
}

void ControlBomba::apagarBomba() {
    digitalWrite(_relePin, LOW);
    digitalWrite(_ledVerde, LOW);
    digitalWrite(_ledRojo, HIGH);
    Serial.println("Bomba apagada. LED verde OFF, LED rojo ON");
}
