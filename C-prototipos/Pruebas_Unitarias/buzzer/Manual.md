### MANUAL

## 📌 Descripción

Este manual demuestra cómo utilizar un **buzzer pasivo** con una librería personalizada en **ESP32**, implementando un modelo no bloqueante basado en `millis()` y tres niveles de alarma definidos por frecuencia y tiempos de encendido/apagado.

------------------------

## 📂 Organización del Repositorio

- 📦buzzer
-  ┣ 📂.pio
-  ┃ ┗ 📂build
-  ┣ 📂include
-  ┃ ┗ 📜README
-  ┣ 📂lib
-  ┃ ┗ 📜README
-  ┣ 📂src
-  ┃ ┗ 📜main.cpp
-  ┣ 📂test
-  ┃ ┗ 📜README
-  ┣ 📜.gitignore
-  ┣ 📜buzzer.cpp
-  ┣ 📜buzzer.h
-  ┣ 📜Manual.md
-  ┗ 📜platformio.ini


------------------------

## 🔧 Configuración crítica en `platformio.ini`

```ini
[env:esp32dev]
platform    = espressif32
board       = esp32dev
framework   = arduino
monitor_speed = 115200
build_flags  = -I${PROJECT_DIR}/lib/Buzzer
```

## LIbreria Personalizada

```cpp
#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

/// Niveles de alarma
enum AlarmLevel : uint8_t {
  A_NONE = 0,
  A_LOW,
  A_MEDIUM,
  A_HIGH
};

class Buzzer {
public:
  Buzzer(uint8_t pin);
  void begin();                    // Configura el pin y estado inicial
  void setLevel(AlarmLevel lvl);   // Cambia el patrón de alarma
  void update();                   // Debe llamarse en loop(): no bloqueante

private:
  struct AlarmParams { uint16_t freq; uint16_t onDur; uint16_t offDur; };
  static const AlarmParams params[4];

  uint8_t       _pin;
  AlarmLevel    _current;
  bool          _isOn;
  unsigned long _lastTs;
};

#endif
```

```cpp
#include "buzzer.h"

// {frecuencia (Hz), tiempo ON (ms), tiempo OFF (ms)}
const Buzzer::AlarmParams Buzzer::params[4] = {
  {   0,   0,   0 },  // A_NONE
  {1000, 100, 900},  // A_LOW    100 ms ON / 900 ms OFF
  {1500, 200, 800},  // A_MEDIUM 200 ms ON / 800 ms OFF
  {2000, 300, 700}   // A_HIGH   300 ms ON / 700 ms OFF
};

Buzzer::Buzzer(uint8_t pin)
  : _pin(pin), _current(A_NONE), _isOn(false), _lastTs(0) {}

void Buzzer::begin() {
  pinMode(_pin, OUTPUT);
  noTone(_pin);
  _lastTs = millis();
}

void Buzzer::setLevel(AlarmLevel lvl) {
  _current = lvl;
  _isOn    = false;
  _lastTs  = millis();
}

void Buzzer::update() {
  if (_current == A_NONE) {
    noTone(_pin);
    return;
  }
  unsigned long now = millis();
  const auto &p = params[_current];

  if (_isOn) {
    if (now - _lastTs >= p.onDur) {
      noTone(_pin);
      _isOn   = false;
      _lastTs = now;
    }
  } else {
    if (now - _lastTs >= p.offDur) {
      tone(_pin, p.freq);
      _isOn   = true;
      _lastTs = now;
    }
  }
}
```

------------------------
## Conexiones Físicas

| **Buzzer**     | **ESP32**   |
| -------------- | ----------- |
| Señal (positivo) | GPIO 15   |
| GND (negativo)   | GND       |

------------------------

## Código de Ejemplo (src/main.cpp)

```cpp
#include <Arduino.h>
#include "buzzer.h"

static const uint8_t BUZZER_PIN     = 15;
static const unsigned long INTERVAL = 5000; // Cambiar nivel cada 5 s

Buzzer buzzer(BUZZER_PIN);
AlarmLevel levels[] = { A_LOW, A_MEDIUM, A_HIGH, A_NONE };
size_t      idx       = 0;
unsigned long lastChange = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  buzzer.begin();
  buzzer.setLevel(levels[idx]);
  Serial.println("=== Prueba Buzzer iniciada ===");
}

void loop() {
  // 1) Ejecuta la lógica no bloqueante del buzzer
  buzzer.update();

  // 2) Cada INTERVAL cambia nivel y lo reporta
  unsigned long now = millis();
  if (now - lastChange >= INTERVAL) {
    idx = (idx + 1) % (sizeof(levels)/sizeof(levels[0]));
    buzzer.setLevel(levels[idx]);
    Serial.print("Nivel de alarma: ");
    Serial.println(idx); // 0=Low,1=Med,2=High,3=None
    lastChange = now;
  }
}
```

------------------------

## Solucion de Problemas

| **Síntoma**                        | **Causa Probable**                       | **Solución**                                                   |
|------------------------------------|-------------------------------------------|----------------------------------------------------------------|
| No suena el buzzer                 | Pin mal configurado o incorrecto          | Verificar `BUZZER_PIN` y `pinMode()` en `begin()`              |
| Patrón de sonido constante         | `_isOn` nunca cambia porque `update()` no se llama | Asegurarse de llamar a `buzzer.update()` en cada `loop()`      |
| Alertas no varían                  | `setLevel()` no invocado o nivel inválido | Revisar la lógica de cambio de niveles en `loop()`            |
| Ruido o clics no deseados          | Frecuencias altas o duraciones muy cortas | Ajustar valores en `params[]` según el buzzer utilizado        |