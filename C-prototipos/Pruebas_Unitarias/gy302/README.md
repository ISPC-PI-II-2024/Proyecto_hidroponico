# Proyecto_hidroponico<p align="center">
  <img src="E-Assets/logo.png" width="200" alt="ISPC Hydroponics logo"/>
</p>

<h1 align="center">ISPC Hydroponics</h1>

<p align="center">
  🌱 Cultivando el futuro con tecnología inteligente 💧
</p>

---

## 📌 Descripción

**ISPC Hydroponics** es un proyecto IoT colaborativo desarrollado por estudiantes del Instituto Superior Politecnico Cordoba, correspondientes a la materia Proyecto Integrador 2 de la Tecnicatura Superior en Telecomunicaciones.
En el mismo se busca aplicar tecnología para monitorear y automatizar un sistema de cultivo hidropónico. 
El objetivo es construir una solución inteligente, ecológica y accesible que combine sensores, microcontroladores y visualización en tiempo real.

---

## 📂 Organización

```
📁 /Requisitos         → los requisitos del proyecto  
📁 /Investigacion      → Archivos de investigacion, procedimientos, apuntes, datasheets 
📁 /Prototipos         → Prototipos, pruebas unitarios, integraciones, etc 
📁 /Presentacion       → Documentacion de presentacion, videos, diapositivas, manuales de uso
📁 /assets       → Imágenes, logo, multimedia  
📄 README.md     → Este archivo  
📄 LICENSE       → Licencia del proyecto
```


# Manual de Usuario: GY-302 (BH1750) con ESP32 en PlatformIO

##  📌 Requisitos
- **Hardware**:
  - Placa ESP32
  - Sensor GY-302 (BH1750)
  - Conexiones I2C (cables dupont)

- **Software**:
  - PlatformIO (extensión en VSCode)  

## 🔧 Instalación
1. **Estructura del proyecto**:
   ```bash
   /lib
     /GY302_BH1750
       GY302_BH1750.h
       GY302_BH1750.cpp
   /src
     main.cpp
   platformio.ini
---

##  Configuración crítica en platformio.ini:  
```ini  
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 9600
lib_deps = claws/BH1750@^1.3.0
build_flags = -I${PROJECT_DIR}/lib/GY302_BH1750  
```    
   


## 📦 Librería Personalizada

### GY302_BH1750.h
```cpp

#ifndef GY302_BH1750_H
#define GY302_BH1750_H

#include <BH1750.h>

class GY302 {
  public:
    bool begin();
    float readLightLevel();
  private:
    BH1750 lightMeter;
};
#endif

```

### GY302_BH1750.cpp

```cpp  

#include "GY302_BH1750.h"
#include <Wire.h>

bool GY302::begin() {
  Wire.begin();
  return lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
}

float GY302::readLightLevel() {
  return lightMeter.readLightLevel();
} 


```   



##  🔌 Conexiones Físicas

| **Sensor GY-302** | **ESP32**       |
|-------------------|-----------------|
| VCC (rojo)        | 3.3V (alimentación) |
| GND (negro)       | GND             |
| SDA (azul)        | GPIO21          |
| SCL (verde)       | GPIO22          |



##  💻 Código de Ejemplo (main.cpp)


```cpp

#include <Arduino.h>
#include <GY302_BH1750.h>

GY302 sensor;
const int LED_PIN = 2; // LED interno

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  
  if (!sensor.begin()) {
    Serial.println("¡Error en sensor!");
    while (1);
  }
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  float lux = sensor.readLightLevel();
  Serial.print("Luz: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(9900); // Total 10 segundos
}



```   

## 🛠 Solución de Problemas

### Errores Comunes y Soluciones

| **Error**                           | **Causa Probable**                  | **Solución**                                                                 |
|-------------------------------------|--------------------------------------|------------------------------------------------------------------------------|
| `GY302_BH1750.h: No such file`      | Ruta incorrecta del header           | Verificar: <br>- `build_flags = -I${PROJECT_DIR}/lib/GY302_BH1750` en `platformio.ini` <br>- El archivo debe estar en `/lib/GY302_BH1750/` |
| `undefined reference to GY302::...` | El `.cpp` no se compila              | Mover `GY302_BH1750.cpp` a `/lib/GY302_BH1750/` (no dentro de `src/`)       |
| Valores incorrectos (ej: "kx")      | Error tipográfico en `Serial.print()` | Revisar: <br>```cpp <br>Serial.println(" lx"); // Debe ser 'lx' (lux) <br>``` |
| Sensor no detectado                 | Problemas de conexión I2C            | Verificar: <br>- Cableado (SDA=GPIO21, SCL=GPIO22) <br>- Alimentación (3.3V) |
| Lecturas inconsistentes             | Ruido eléctrico o luz ambiental      | Promediar múltiples lecturas: <br>```cpp <br>lux = (sensor.read() + sensor.read()) / 2; <br>``` |

### Pasos para Debugging Avanzado

1. **Verificar conexiones I2C**:
   ```cpp
   void setup() {
     Wire.begin();
     Serial.begin(9600);
     while (!Serial);
     Wire.beginTransmission(0x23); // Dirección del BH1750
     if (Wire.endTransmission() == 0) {
       Serial.println("Sensor detectado!");
     } else {
       Serial.println("Error: Revisar conexiones");
     }
   }  
    ```




---

## 📄 Licencia

Distribuido bajo la Licencia Creative Commons.

---