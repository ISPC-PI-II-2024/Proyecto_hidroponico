#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

// Tres niveles de alarma
enum AlarmLevel : uint8_t {
  A_NONE = 0,
  A_LOW,
  A_MEDIUM,
  A_HIGH
};

class Buzzer {
public:
  Buzzer(uint8_t pin);
  void begin();                    // Configura el pin
  void setLevel(AlarmLevel lvl);   // Cambia el patrón de alarma
  void update();                   // Llamar en loop(): sin bloqueo

private:
  static const struct {
  uint16_t freq;
  uint16_t onDur;
  uint16_t offDur;
  } params[4];


  uint8_t       _pin;
  AlarmLevel    _level;
  bool          _isOn;
  unsigned long _lastTs;
};

#endif
