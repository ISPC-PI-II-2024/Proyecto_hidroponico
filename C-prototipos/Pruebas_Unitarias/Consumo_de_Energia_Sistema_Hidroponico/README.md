# Presentación del Proyecto: Consumo de Energía en Sistema Hidropónico

Este proyecto utiliza un ESP32-S3 DevKitM-1 para medir el consumo de energía en un sistema hidropónico. Está desarrollado con el framework Arduino y configurado para ser utilizado con PlatformIO.

## Estructura del Proyecto

### Archivos Principales
- **`platformio.ini`**: Archivo de configuración principal de PlatformIO que define el entorno de desarrollo para la placa ESP32-S3 y el framework Arduino.
- **`wokwi.toml`**: Configuración para el simulador Wokwi, especificando el firmware y el archivo ELF generados.
- **`diagram.json`**: Representación del diseño del circuito en el simulador Wokwi, incluyendo un ESP32 y un potenciómetro.
- **`.gitignore`**: Lista de archivos y carpetas ignorados por Git, como `.pio` y `.vscode`.

### Código Fuente
- **`src/main.cpp`**: Contiene el código principal que realiza lecturas analógicas del pin 36 para calcular corriente y voltaje, imprimiendo los resultados en el monitor serial.

### Carpetas
- **`include/`**: Archivos de cabecera del proyecto.  
  - `README`: Explica cómo usar archivos de cabecera en C/C++.
- **`lib/`**: Bibliotecas específicas del proyecto.  
  - `README`: Describe cómo organizar y usar bibliotecas privadas en PlatformIO.
- **`test/`**: Pruebas unitarias del proyecto.  
  - `README`: Explica cómo ejecutar pruebas unitarias con PlatformIO.
- **`.vscode/`**: Configuración para Visual Studio Code.  
  - `c_cpp_properties.json`: Configuración de rutas de inclusión y compilador.  
  - `launch.json`: Configuración para depuración.  
  - `extensions.json`: Recomendaciones de extensiones.

### Otros Archivos
- **`.pio/`**: Carpeta generada automáticamente que contiene archivos de compilación como el firmware (`firmware.bin`) y el ELF (`firmware.elf`).
- **`project.checksum`**: Archivo generado automáticamente con un hash del proyecto.

## Requisitos
- **Hardware**: ESP32-S3 DevKitM-1.  
- **Software**:  
  - PlatformIO IDE (recomendado con Visual Studio Code).  
  - Simulador Wokwi (opcional para pruebas virtuales).

## Configuración y Ejecución
1. Clona el repositorio en tu máquina local.
2. Abre el proyecto en Visual Studio Code con la extensión de PlatformIO instalada.
3. Conecta el ESP32-S3 DevKitM-1 a tu computadora.
4. Compila y sube el firmware al ESP32 ejecutando:
   ```bash
   pio run --target upload
   ```
5. Abre el monitor serial para visualizar los datos:
   ```bash
   pio device monitor
   ```

## Ejecución de Pruebas Unitarias

Para ejecutar las pruebas unitarias del proyecto, sigue los pasos a continuación:

1. Asegúrate de que el sensor esté conectado al pin especificado en el código (`36`).
2. Abre una terminal en la raíz del proyecto.
3. Ejecuta el siguiente comando para compilar y ejecutar las pruebas:
   ```bash
      pio test -e esp32-s3-devkitm-1
      ```
   4. Observa los resultados en el monitor serial. Si el sensor no está presente, se notificará la falla en la consola.

## Simulación en [Wokwi](https://wokwi.com/projects/429326475435077633)
1. Abre el archivo `diagram.json` en el simulador Wokwi.
2. Verifica que el firmware esté configurado correctamente en `wokwi.toml`.



## Documentación Adicional
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Wokwi Simulator](https://wokwi.com/)
- [Arduino Framework](https://www.arduino.cc/)

## Autor
Juan Diego Gonzalez Antoniazzi
```