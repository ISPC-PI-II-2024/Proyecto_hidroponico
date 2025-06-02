# Manual de uso de la librería lcd_display para ESP32 y LCD 20x4 I2C

Este manual describe cómo utilizar la librería `lcd_display` para controlar una pantalla LCD 20x4 que se comunica a través del protocolo I2C con una placa ESP32.

## Archivos Incluidos

* `lcd_display.h`: Archivo de encabezado que declara las funciones para interactuar con el LCD.
* `lcd_display.cpp`: Archivo de implementación que contiene el código fuente de las funciones.

## Instalación

1.  Guarda los archivos `lcd_display.h` y `lcd_display.cpp` en la misma carpeta de tu proyecto de Arduino (o PlatformIO) junto con tu archivo principal (`.ino` o `.cpp`).

## Uso en tu Código Principal (`main.cpp` o `.ino`)

### 1. Incluir la Librería

En la parte superior de tu archivo principal, incluye el archivo de encabezado de las librerias:

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "lcd_display.h"
