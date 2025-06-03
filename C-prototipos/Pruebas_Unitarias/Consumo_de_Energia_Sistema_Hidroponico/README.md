# Consumo de Energía - Sistema Hidropónico

Este proyecto implementa y prueba un **módulo de medición de consumo de energía** para sistemas hidropónicos, utilizando un sensor I2C para obtener voltaje, corriente y calcular potencia. Incluye pruebas unitarias exhaustivas con mocks de hardware para asegurar la robustez del código.

---

## Estructura del Proyecto

```
Consumo_de_Energia_Sistema_Hidroponico/
├── lib/
│   └── Modulo_de_Energia/
│       ├── Modulo_de_Energia.h
│       ├── Modulo_de_Energia.cpp
│       └── library.json
├── src/
│   └── main.cpp
├── test/
│   └── Test_Modulo_de_Energia/
│       ├── test_main.cpp
│       ├── test_cases.cpp
│       ├── Resultados.md
│       └── lib/
│           └── MockWire/
│               ├── Wire.h
│               ├── Wire.cpp
│               └── library.json
├── platformio.ini
└── README.md
```

---

## Descripción de Carpetas y Archivos

### `lib/Modulo_de_Energia/`
- **Modulo_de_Energia.h / .cpp**: Implementación de la clase `Modulo_de_Energia`, que abstrae la lectura de voltaje, corriente y cálculo de potencia desde un sensor I2C.
- **library.json**: Metadatos de la librería para PlatformIO.

### `src/`
- **main.cpp**: Ejemplo de uso en un entorno real (ESP32), mostrando cómo inicializar el módulo y leer valores periódicamente.

### `test/Test_Modulo_de_Energia/`
- **test_main.cpp**: Entrada principal para ejecutar las pruebas unitarias con Unity.
- **test_cases.cpp**: Implementación de todos los casos de prueba, cubriendo inicialización, lecturas correctas, fallos de comunicación, simulaciones de carga y condiciones anómalas.
- **Resultados.md**: Informe de resultados de las pruebas unitarias ejecutadas.
- **lib/MockWire/**: Implementación mock de la librería `Wire` de Arduino para simular el bus I2C en pruebas.
  - **Wire.h / Wire.cpp**: Mock completo de la clase `TwoWire` (Wire), permitiendo simular lecturas, escrituras y errores de comunicación.
  - **library.json**: Metadatos del mock para PlatformIO.

### Raíz del Proyecto
- **platformio.ini**: Configuración de entornos PlatformIO para pruebas (`esp32_test_energia`) y firmware real (`esp32_firmware`).
- **README.md**: Este archivo.

---

## Principales Características

- **Lectura de voltaje y corriente** vía I2C, con conversión a unidades físicas.
- **Cálculo de potencia** en tiempo real.
- **Manejo robusto de errores de comunicación**.
- **Pruebas unitarias exhaustivas** usando Unity y mocks de hardware.
- **Simulación de condiciones reales y anómalas** (baja/alta carga, fallos de sensor).

---

## Ejemplo de Uso (main.cpp)

```cpp
#include <Arduino.h>
#include "Modulo_de_Energia.h"

Modulo_de_Energia energyMonitor(Wire);

void setup() {
  Serial.begin(115200);
  energyMonitor.begin();
  if (energyMonitor.isCommunicationOK()) {
    Serial.println("Módulo de energía inicializado correctamente.");
  }
}

void loop() {
  if (energyMonitor.isCommunicationOK()) {
    float voltage = energyMonitor.getVoltage();
    float current = energyMonitor.getCurrent();
    float power = energyMonitor.getPower();
    Serial.print("Voltaje: "); Serial.println(voltage);
    Serial.print("Corriente: "); Serial.println(current);
    Serial.print("Potencia: "); Serial.println(power);
  }
  delay(5000);
}
```

---

## Pruebas Unitarias

- **Framework:** Unity (integrado con PlatformIO).
- **MockWire:** Simula el bus I2C y el sensor para pruebas determinísticas.
- **Cobertura:** Inicialización, lecturas correctas, fallos de comunicación, simulaciones de carga, condiciones anómalas.
- **Resultados:** Ver `test/Test_Modulo_de_Energia/Resultados.md`.

---

## Cómo Ejecutar

1. **Instalar PlatformIO** en VSCode.
2. **Seleccionar entorno**:  
   - Para pruebas: `esp32_test_energia`
   - Para firmware: `esp32_firmware`
3. **Compilar y ejecutar pruebas**:
   ```
   pio test -e esp32_test_energia
   ```
4. **Subir firmware real**:
   ```
   pio run -e esp32_firmware -t upload
   ```

---

## Créditos

- Autor: Juan Diego Gonzalez Antoniazzi
- Contacto: juandi19972008@gmail.com

---
