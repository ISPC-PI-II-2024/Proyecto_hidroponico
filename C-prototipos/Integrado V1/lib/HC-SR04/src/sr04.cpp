#include "SR04.h"

// Defino el metodo "begin", arranca en estado IDLE y espera 200ms entre mediciones
void SR04::begin(uint8_t pinTrig, uint8_t pinEcho) {
  _pinTrig = pinTrig;
  _pinEcho = pinEcho;
  pinMode(_pinTrig, OUTPUT);
  pinMode(_pinEcho, INPUT);
  digitalWrite(_pinTrig, LOW);

  _state          = IDLE;
  _lastTriggerMs  = millis();
  _lastDistance   = -1;
}

// Defino el metodo "read", que lanza la medicion y devuelve la distancia en cm o -1 si hubo timeout
float SR04::read() {
  unsigned long nowMs = millis();
  unsigned long nowUs = micros();

  switch (_state) {
    case IDLE:
      if (nowMs - _lastTriggerMs >= INTERVAL_MS) {
        digitalWrite(_pinTrig, HIGH);
        _pulseStartUs = nowUs;
        _state = TRIGGER_PULSE;
      }
      break;

    case TRIGGER_PULSE:
      if (nowUs - _pulseStartUs >= 10UL) {
        digitalWrite(_pinTrig, LOW);
        _lastTriggerMs = nowMs;
        _state = WAIT_HIGH;
      }
      break;

    case WAIT_HIGH:
      if (digitalRead(_pinEcho) == HIGH) {
        _pulseStartUs = micros();
        _state = WAIT_LOW;
      } else if (nowMs - _lastTriggerMs >= TIMEOUT_HIGH_MS) {
        _lastDistance = -1;
        _state = IDLE;
      }
      break;

    case WAIT_LOW:
      if (digitalRead(_pinEcho) == LOW) {
        unsigned long durationUs = micros() - _pulseStartUs;
        _lastDistance = (durationUs * 0.0343f) / 2.0f;
        _state = IDLE;
      } else if (micros() - _pulseStartUs >= TIMEOUT_LOW_US) {
        _lastDistance = -1;
        _state = IDLE;
      }
      break;
  }

  return _lastDistance;
}
