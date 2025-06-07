#include "buzzer.h"

// Definición de parámetros por nivel: {frecuencia, ON ms, OFF ms}
constexpr decltype(Buzzer::params) Buzzer::params = {
  {   0,    0,    0 },  // A_NONE
  {1000,  100,  900},  // A_LOW
  {1500,  200,  800},  // A_MEDIUM
  {2000,  300,  700}   // A_HIGH
};

Buzzer::Buzzer(uint8_t pin)
  : _pin(pin), _level(A_NONE), _isOn(false), _lastTs(0) {}

void Buzzer::begin() {
  pinMode(_pin, OUTPUT);
  noTone(_pin);                 // Apaga cualquier tono previo
  ledcSetup(0, 2000, 8);        // Canal 0, 2 kHz, resolución 8 bits
  ledcAttachPin(_pin, 0);       // Asocia canal 0 al pin
  _lastTs = millis();
}

void Buzzer::setLevel(AlarmLevel lvl) {
  _level = lvl;
  _isOn   = false;
  _lastTs = millis();
}

void Buzzer::update() {
  if (_level == A_NONE) {
    ledcWriteTone(0, 0);
    return;
  }
  auto now = millis();
  auto &p  = params[_level];

  if (_isOn) {
    if (now - _lastTs >= p.onDur) {
      ledcWriteTone(0, 0);
      _isOn    = false;
      _lastTs  = now;
    }
  } else {
    if (now - _lastTs >= p.offDur) {
      ledcWriteTone(0, p.freq);
      _isOn    = true;
      _lastTs  = now;
    }
  }
}