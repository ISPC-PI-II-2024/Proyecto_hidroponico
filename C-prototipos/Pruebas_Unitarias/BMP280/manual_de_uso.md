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

|Método| Descripción |
|------|-------------|
|SensorBMP280();| Costructor, no inicializa el sensor.|  
|bool begin();|Inicializa el sensor. Devuelve true si fue exitoso.|
|float getTemperature();| Devuelve la temperatura en °C.|                   
|float readPressure();| Devuelve la presión en hPa (hectopascales).|
|float readAltitude(float seaLevelhPa);	| Devuelve la altitud estimada en metros, basándose en la presión atmosférica al nivel del mar.|
|bool isConnected();| Verifica si el sensor está conectado y funcionando.|  
    
## Uso basico de los metodos 

`bool begin()`  
Inicializa el sensor BMP280. Retorna true si se pudo comunicar correctamente, false si falló.

Ejemplo:

```cpp

if (!bmp.begin()) {
  Serial.println("❌ BMP280 no detectado.");
}  
```  

`float getTemperature()`  
Devuelve la temperatura actual en grados Celsius.

Ejemplo:

```cpp

Serial.println(bmp.getTemperature());  // 23.45  
```  

`float getPressure()`  
Devuelve la presión atmosférica en hectopascales (hPa).

Ejemplo:

```cpp

Serial.println(bmp.getPressure());  // 1013.25  
```  

`float getAltitude(float seaLevelhPa = 1013.25)`    
Calcula y devuelve la altitud estimada en metros, tomando la presión al nivel del mar como parámetro.

Ejemplo:

```cpp

Serial.println(bmp.getAltitude());  // 142.50 (metros)
```

## 📦 Ejemplo de uso básico
```cpp

#include <Arduino.h>
#include "SensorBMP280.h"

SensorBMP280 bmp;
unsigned long Millis_anterior = 0;
const unsigned long intervalo = 5000; // 5 segundos

void setup() {
  Serial.begin(9600);
  if (bmp.begin()) {
    Serial.println("BMP280 conectado correctamente.");
  } else {
    Serial.println("Error al conectar el BMP280. Se intentará de nuevo periódicamente.");
  }
}

void loop() {
  unsigned long Millis_actual = millis();

  // Reintenta conexión si no está conectado
  if (!bmp.isConnected()) {
    static unsigned long ultimo_intento = 0;
    if (Millis_actual - ultimo_intento >= 10000) { // Reintenta cada 10 segundos
      ultimo_intento = Millis_actual;
      if (bmp.begin()) {
        Serial.println("Reconexión exitosa con el BMP280.");
      } else {
        Serial.println("Reintento fallido: BMP280 no conectado.");
      }
    }
    return; // No intentamos leer datos si no está conectado
  }

  if (Millis_actual - Millis_anterior >= intervalo) {
    Millis_anterior = Millis_actual;

    float temp = bmp.readTemperature();
    float presion = bmp.readPressure();
    float altitud = bmp.readAltitude();

    if (isnan(temp) || isnan(presion) || isnan(altitud)) {
      Serial.println("Error al leer datos del BMP280.");
      return;
    }

    Serial.println("-------------");
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println(" °C");

    Serial.print("Presión: ");
    Serial.print(presion);
    Serial.println(" hPa");

    Serial.print("Altitud estimada: ");
    Serial.print(altitud);
    Serial.println(" m");
  }
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