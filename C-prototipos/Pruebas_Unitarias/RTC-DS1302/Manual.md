### MANUAL 

## 📌 Descripción
    El manual muestra como utilizar un RTC DS1302. Logrando visualizar la fecha y hora en tiempo real de forma automatica

## 📂 Organización del Repositorio

 - 📦RTC DS1302
 - ┣ 📂.pio
 - ┃ ┗ 📂build
 - ┣ 📂include
 - ┃ ┗ 📜README
 - ┣ 📂lib
 - ┃ ┣ 📂Pantalla
 - ┃ ┃ ┣ 📜Pantalla.cpp
 - ┃ ┃ ┗ 📜Pantalla.h
 - ┃ ┗ 📜README
 - ┣ 📂src
 - ┃ ┗ 📜main.cpp
 - ┣ 📂test
 - ┃ ┗ 📜README
 - ┣ 📜.gitignore
 - ┣ 📜Manual.md
 - ┗ 📜platformio.ini


  ------------------------------

## Configuración  en `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
	adafruit/RTClib@^2.1.4
	adafruit/Adafruit BusIO@^1.17.0
	makuna/RTC@^2.5.0
    3304
monitor_speed = 115200
```

---------------------------------



## Conexiones Físicas

|**Pin DS1302**| **ESP32**       |
|--------------|------------------|
| VCC          | 3.3V             |
| GND          | GND              |
| DAT          | GPIO18           |
| CLK          | GPIO5            |
| RST          | GPIO19           |
---------------------

## Libreria Pantalla

### 📁 lib/pantalla/Pantalla.cpp

```cpp

#include "Pantalla.h"

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


### 📁 lib/pantalla/Pantalla.h
```h

#ifndef PANTALLA_H
#define PANTALLA_H

#include <Arduino.h>
#include <RtcDS1302.h>

class Pantalla {
public:
    void mostrarHora(const RtcDateTime& dt);
    void mensajeInicio();
    void mensajeHoraInvalida();
    void mensajeComandoNoValido(const String& cmd);
};

#endif



```

## Código utilizado
### src/main.cpp

```cpp

include <ThreeWire.h>
#include <RtcDS1302.h>
#include "Pantalla.h"

// Conexiones de RTC DS1302
#define PIN_DAT 18 
#define PIN_CLK 5
#define PIN_RST 19

ThreeWire myWire(PIN_DAT, PIN_CLK, PIN_RST);
RtcDS1302<ThreeWire> Rtc(myWire);
Pantalla pantalla;

unsigned long tiempoAnterior = 0;
const unsigned long intervalo = 10000;  // 10 segundos

void setup() {
    Serial.begin(115200);
    while (!Serial);

    Rtc.Begin();

    if (Rtc.GetIsWriteProtected()) Rtc.SetIsWriteProtected(false);
    if (!Rtc.GetIsRunning()) Rtc.SetIsRunning(true);

    if (!Rtc.IsDateTimeValid()) {
        pantalla.mensajeHoraInvalida();
    }

    pantalla.mensajeInicio();
}

void loop() {
    if (millis() - tiempoAnterior >= intervalo) {
        tiempoAnterior = millis();
        RtcDateTime now = Rtc.GetDateTime();
        if (now.IsValid()) {
            Serial.print("[Auto] ");
            pantalla.mostrarHora(now);
        }
    }

    if (Serial.available()) {
        String comando = Serial.readStringUntil('\n');
        comando.trim();

        Serial.print("> ");
        Serial.println(comando);

        if (comando.equalsIgnoreCase("ver")) {
            RtcDateTime now = Rtc.GetDateTime();
            if (now.IsValid()) {
                pantalla.mostrarHora(now);
            } else {
                pantalla.mensajeHoraInvalida();
            }
        } else {
            pantalla.mensajeComandoNoValido(comando);
        }
    }
}
```
