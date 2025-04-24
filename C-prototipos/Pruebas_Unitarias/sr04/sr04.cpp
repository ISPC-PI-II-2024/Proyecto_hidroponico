#include "sr04.h"
#include <Arduino.h>

// Máquina de estados
enum Sr04State : uint8_t {
  SR04_IDLE,
  SR04_TRIGGER_PULSE,
  SR04_WAIT_HIGH,
  SR04_WAIT_LOW
};

static Sr04State      _state        = SR04_IDLE;
static unsigned long _lastMs       = 0;     // ms para iniciar medición
static unsigned long _pulseStartUs = 0;     // µs para medir TRIG y eco
static float         _lastDistance = -1;    // cm

static uint8_t       _pinTrig;
static uint8_t       _pinEcho;

// Parámetros ajustables
static const unsigned long INTERVALO_MEDICION = 200UL;   // ms entre mediciones
static const unsigned long TIMEOUT_ECHO       = 30UL;    // ms para HIGH
static const unsigned long TIMEOUT_PULSO      = 30000UL; // µs para LOW

void sr04Begin(uint8_t pinTrig, uint8_t pinEcho) {
  _pinTrig      = pinTrig;
  _pinEcho      = pinEcho;
  pinMode(_pinTrig, OUTPUT);
  pinMode(_pinEcho, INPUT);
  digitalWrite(_pinTrig, LOW);

  _state        = SR04_IDLE;
  _lastMs       = millis();
  _lastDistance = -1;
}

float sr04Read() {
  unsigned long nowMs = millis();
  unsigned long nowUs = micros();

  switch (_state) {
    case SR04_IDLE:
      // Inicia nueva medición cada INTERVALO_MEDICION ms
      if (nowMs - _lastMs >= INTERVALO_MEDICION) {
        digitalWrite(_pinTrig, HIGH);
        _pulseStartUs = nowUs;         // <-- guardo inicio de TRIG
        _state        = SR04_TRIGGER_PULSE;
      }
      break;

    case SR04_TRIGGER_PULSE:
      // Después de 10 µs bajo TRIG
      if (nowUs - _pulseStartUs >= 10UL) {
        digitalWrite(_pinTrig, LOW);
        _lastMs  = nowMs;              // <-- reinicio intervalo
        _state   = SR04_WAIT_HIGH;
      }
      break;

    case SR04_WAIT_HIGH:
      // Espero pulso HIGH en ECHO
      if (digitalRead(_pinEcho) == HIGH) {
        _pulseStartUs = micros();      // <-- guardo inicio de eco
        _state        = SR04_WAIT_LOW;
      }
      // Timeout si no llega eco en TIMEOUT_ECHO ms
      else if (nowMs - _lastMs >= TIMEOUT_ECHO) {
        _lastDistance = -1;
        _state        = SR04_IDLE;
      }
      break;

    case SR04_WAIT_LOW:
      // Fin del eco: calculo distancia
      if (digitalRead(_pinEcho) == LOW) {
        unsigned long durationUs = micros() - _pulseStartUs;
        _lastDistance = (durationUs * 0.0343f) / 2.0f;
        _state        = SR04_IDLE;
      }
      // Timeout si nunca baja en TIMEOUT_PULSO µs
      else if (micros() - _pulseStartUs >= TIMEOUT_PULSO) {
        _lastDistance = -1;
        _state        = SR04_IDLE;
      }
      break;
  }

  return _lastDistance;
}
