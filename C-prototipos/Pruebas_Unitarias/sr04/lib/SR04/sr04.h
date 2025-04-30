#ifndef SR04_H
#define SR04_H

#include <Arduino.h>

class SR04 {
public:
  SR04() = default;
  /// Inicializa el sensor indicando pines TRIG y ECHO
  void begin(uint8_t pinTrig, uint8_t pinEcho);

  /// Lanza la medición y devuelve la última distancia en cm (-1 si timeout)
  float read();

private:
    // Maquina de estados
  enum State : uint8_t {
    IDLE,
    TRIGGER_PULSE,
    WAIT_HIGH,
    WAIT_LOW
  };

  // Parámetros
  static constexpr unsigned long INTERVAL_MS = 200UL;     // ms entre mediciones
  static constexpr unsigned long TIMEOUT_HIGH_MS = 30UL;  // ms para esperar HIGH en echo
  static constexpr unsigned long TIMEOUT_LOW_US = 30000UL; // µs para esperar LOW en echo

  // Pines
  uint8_t   _pinTrig = 0;
  uint8_t   _pinEcho = 0;

  // Variables de estado
  State     _state        = IDLE;
  unsigned long _lastTriggerMs = 0;
  unsigned long _pulseStartUs  = 0;
  float         _lastDistance  = -1;
};

#endif 
