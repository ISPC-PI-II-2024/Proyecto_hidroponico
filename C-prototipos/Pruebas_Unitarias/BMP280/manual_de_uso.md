# manual de uso Sensor BMP280  

## 📌 Descripción  

El BMP280 es un sensor barométrico fabricado por Bosch que permite medir:

Temperatura ambiente

Presión atmosférica

Altitud estimada (calculada a partir de la presión)

Este sensor se puede comunicar con el microcontrolador mediante los protocolos I2C o SPI. En este ejemplo se usa I2C, que es más sencillo de conectar.

## 🧰 Conexiones típicas (modo I2C)

BMP280 Pin	| Conexión al microcontrolador
VCC	| 3.3V
GND	| GND
SDA	Pin | SDA del micro (ej. GPIO21 en ESP32)
SCL	Pin | SCL del micro (ej. GPIO22 en ESP32)  

💡 La dirección I2C por defecto es 0x76, pero puede ser 0x77 en algunos módulos.

🧠 Métodos disponibles en la clase SensorBMP280

|Método	                                | Descripción                                            |
|----------------------------------------|-------------------------------------------------------|
|bool begin();                          | Inicializa el sensor. Devuelve true si fue exitoso.    |
|float readTemperature();               | Devuelve la temperatura en °C.                         |
|float readPressure();                  | Devuelve la presión en hPa (hectopascales).            |
|float readAltitude(float seaLevelhPa);	| Devuelve la altitud estimada en metros, basándose en la presión atmosférica al nivel del mar.                                                            |
|bool isConnected();                    | Verifica si el sensor está conectado y funcionando.    |

## 📦 Ejemplo de uso básico
```cpp

#include <Wire.h>
#include "SensorBMP280.h"

SensorBMP280 bmp;

void setup() {
  Serial.begin(115200);
  bmp.begin();
}

void loop() {
  Serial.print("Temp: ");
  Serial.print(bmp.readTemperature());
  Serial.print(" °C, ");

  Serial.print("Presión: ");
  Serial.print(bmp.readPressure());
  Serial.print(" hPa, ");

  Serial.print("Altitud: ");
  Serial.print(bmp.readAltitude(1013.25)); // valor típico al nivel del mar
  Serial.println(" m");

  delay(2000);
}
```  

## 🧱 Estructura del código
Se creó una clase llamada SensorBMP280 que permite:

### 🏗️ Inicializar el sensor
```cpp
SensorBME280 bmp;
bmp.begin();  
```
### 🌡️ Obtener lecturas
```cpp

float temp = sensor.getTemperature();      // °C
float hum = sensor.getHumidity();          // %
float pres = sensor.getPressure();         // hPa
float alt = sensor.getAltitude();          // m (estimado)  
```  

🧠 La altitud se calcula usando una presión de referencia a nivel del mar. Por defecto se usa 1013,25 hPa, pero se puede cambiar:

```cpp
sensor.setSeaLevelPressure(1015.0);  
```  

## 📘 Consideraciones a tener en cuenta
- Si se aprecian lecturas erráticas, revisar bien las conexiones.

- Este sensor no mide humedad, a diferencia de su hermano el BME280.

- Si el sensor no responde, probar con la dirección 0x77 en el método bmp.begin(0x77). 

- Evitar colocar el sensor cerca de fuentes de calor o humedad directas (ej: motores, manos respiración).

- Si el valor de altitud es inestable, revisar el valor de presión de referencia (se puede obtener de una estación meteorológica local).