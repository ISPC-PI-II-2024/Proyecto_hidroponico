### MANUAL 

## 📌 Descripción

Este manual demuestra cómo utilizar un sensor de distancia ultrasónico **HC-SR04** integrando una librería personalizada. La arquitectura implementa una máquina de estados para optimizar las lecturas.

---

## 📂 Organización del Repositorio

 - 📦sr04
 - ┣ 📂.pio
 - ┃ ┗ 📂build
 - ┣ 📂include
 - ┃ ┗ 📜README
 - ┣ 📂lib
 - ┃ ┣ 📂SR04
 - ┃ ┃ ┣ 📜sr04.cpp
 - ┃ ┃ ┗ 📜sr04.h
 - ┃ ┗ 📜README
 - ┣ 📂src
 - ┃ ┗ 📜main.cpp
 - ┣ 📂test
 - ┃ ┗ 📜README
 - ┣ 📜.gitignore
 - ┣ 📜Manual.md
 - ┗ 📜platformio.ini

 ------------------------------

## Configuración crítica en `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
build_flags = -I${PROJECT_DIR}/lib/SR04
```

---------------------------------

## Libreria SR04.h

```cpp
  #ifndef SR04_H
  #define SR04_H

  #include <Arduino.h>

  class SR04 {
  public:
    SR04() = default;
    void begin(uint8_t pinTrig, uint8_t pinEcho);
    float read();

  private:
    enum State : uint8_t { IDLE, TRIGGER_PULSE, WAIT_HIGH, WAIT_LOW };
    static constexpr unsigned long INTERVAL_MS = 200UL;
    static constexpr unsigned long TIMEOUT_HIGH_MS = 30UL;
    static constexpr unsigned long TIMEOUT_LOW_US = 30000UL;

    uint8_t _pinTrig = 0;
    uint8_t _pinEcho = 0;
    State _state = IDLE;
    unsigned long _lastTriggerMs = 0;
    unsigned long _pulseStartUs = 0;
    float _lastDistance = -1;
  };

  #endif
```

```cpp
#include "SR04.h"

void SR04::begin(uint8_t pinTrig, uint8_t pinEcho) {
  _pinTrig = pinTrig;
  _pinEcho = pinEcho;
  pinMode(_pinTrig, OUTPUT);
  pinMode(_pinEcho, INPUT);
  digitalWrite(_pinTrig, LOW);

  _state = IDLE;
  _lastTriggerMs = millis();
  _lastDistance = -1;
}

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
```

---------------------

## Conexiones Físicas

| **Pin SR04** | **ESP32**        |
|--------------|------------------|
| VCC          | 5V               |
| GND          | GND              |
| TRIG         | GPIO5            |
| ECHO         | GPIO18           |

---------------------

## Código de Ejemplo (main.cpp)

```cpp
#include <Arduino.h>
#include "SR04.h"

static const uint8_t TRIG_PIN = 5;
static const uint8_t ECHO_PIN = 18;
static const unsigned long PRINT_INTERVAL = 3000UL;

SR04 sensorSR04_1;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  sensorSR04_1.begin(TRIG_PIN, ECHO_PIN);
  Serial.println("=== Prueba SR04 iniciada ===");
}

void loop() {
  float distancia = sensorSR04_1.read();

  if (distancia < 0) {
    Serial.println("Timeout: sin eco");
  } else {
    Serial.print("Distancia: ");
    Serial.print(distancia, 1);
    Serial.println(" cm");
  }

  delay(PRINT_INTERVAL);
}
```

| **Error**                             | **Causa**                                   | **Solución**                                                                 |
|---------------------------------------|---------------------------------------------|------------------------------------------------------------------------------|
| `sr04.h: No such file`                | Ruta incorrecta                             | Asegurarse de incluir el flag `-I${PROJECT_DIR}/lib/SR04` en `platformio.ini` |
| Distancia negativa constante          | Timeout del pulso ECHO                      | Verificar conexiones y alimentación del sensor                              |
| Valores erráticos o inestables       | Interferencias o eco múltiple               | Aumentar intervalo o filtrar múltiples lecturas                             |
| Sensor sin respuesta                  | Error físico o incorrecta inicialización    | Revisar pines, soldaduras y estado del sensor                               |


