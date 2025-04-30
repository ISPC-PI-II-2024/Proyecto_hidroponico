#include "buzzer.h"

static int       _pin;
static AlarmLevel _currentAlarm = A_NONE;
static unsigned long _lastToggle = 0;
static bool      _buzzerOn = false;

// Parámetros segun el nivel: {frecuencia, onTime, offTime}
struct AlarmParams { unsigned freq, onDur, offDur; };
static const AlarmParams _params[] = {
  {0,    0,   0},   // A_NONE
  {1000,100, 900},  // A_LOW        100 ms ON, 900 ms OFF
  {1500,200, 800},  // A_MEDIUM     200 ms ON, 800 ms OFF
  {2000,300, 700}   // A_HIGH       300 ms ON, 700 ms OFF
};

void buzzerInit(int pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
}

void buzzerSetLevel(AlarmLevel lvl) {
  _currentAlarm = lvl;
  _buzzerOn     = false;
  _lastToggle   = millis();
}

void buzzerUpdate() {
  if (_currentAlarm == A_NONE) {
    noTone(_pin);
    return;
  }
  unsigned long now = millis();
  const auto &p = _params[_currentAlarm];
  if (_buzzerOn) {
    if (now - _lastToggle >= p.onDur) {
      noTone(_pin);
      _buzzerOn    = false;
      _lastToggle  = now;
    }
  } else {
    if (now - _lastToggle >= p.offDur) {
      tone(_pin, p.freq);
      _buzzerOn    = true;
      _lastToggle  = now;
    }
  }
}
