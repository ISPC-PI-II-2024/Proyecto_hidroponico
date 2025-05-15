# Módulo RTC DS3231 - Manual de Uso

Este módulo está diseñado para facilitar el uso del reloj de tiempo real DS3231 en controladores como el ESP32, y además permite actualizar automáticamente la hora usando servidores NTP a través de internet.

---

## 📦 Archivos del módulo

```swift
/DS3231/
    DS3231.h
    DS3231.cpp    
/WiFiManagerESP32/
    WiFiManagerESP32.h
    WiFiManagerESP32.cpp
/WiFiCred/
    WiFiCred.h
/src/
    main.cpp
/manual_de_uso.md   

```  



## 🔌 Conexión del módulo DS3231

| DS3231     | ESP32        |
|------------|--------------|
| VCC        | 3.3V o 5V    |
| GND        | GND          |
| SDA        | GPIO21 (por defecto I2C) |
| SCL        | GPIO22 (por defecto I2C) |    
   
⚠️ El ESP32 tolera 5V en I2C, pero si querés estar más seguro, usá una versión de DS3231 con regulador a 3.3V.

---

## 📚 Librerías necesarias

Debemos instalar las siguientes librerías:

- `RTClib` (de Adafruit)
- `WiFi` (incluida con ESP32)
- `time` (incluida con ESP32)

---

## 🔧 Inicialización

```cpp
#include "DS3231.h"

DS3231Clock reloj

void setup() {
  Serial.begin(115200);
  reloj.begin();
}  
```  

## 🕐 Obtener la hora actual
```cpp
DateTime actual = reloj.getHora();
Serial.println(actual.timestamp());  
```  

## 🌍 Sincronizar con la hora mundial (NTP)
Este método permite que tu módulo DS3231 se sincronice con la hora oficial de internet. Para ello, es necesario conectarse primero a una red WiFi.

📁 Archivos necesarios:
```swift
/WiFiManager
    WiFiManager.h
    WiFiManager.cpp
/WiFicred
    WiFiCred.h  
```  

## 👣 Pasos:
1. Conectar el ESP32 a la Red WiFi
Primero, debes utilizar la clase WiFiManager para establecer conexión WiFi.

```cpp
#include "WiFiManager.h"

WiFiManager wifi;

void setup() {
  Serial.begin(9600);

  wifi.begin();
}

```    
  
La clase WiFiManager leerá el SSID y la password desde WiFiCred.h, donde estan definidas las credenciales de la siguiente manera:

```cpp
#define WIFI_SSID "TuSSID"
#define WIFI_PASSWORD "TuContraseña"    
```   
Este archivo no sera subido al repositorio con la ayuda de .gitignore.    
  
2. Sincronizar el RTC con la Hora Mundial (NTP)  

Una vez conectado a WiFi, se puede sincronizar la hora utilizando el método sincronizarConNTP() de la clase DS3231Clock.

### Código básico para sincronizar la hora:

```cpp
#include <Arduino.h>
#include "DS3231.h"
#include "WiFiManager.h"

WiFiManager wifi;
DS3231Clock reloj;

unsigned long Ultima_muestra = 0;
const unsigned long Intervalo_muestra = 2000;

void setup() {
  Serial.begin(9600);

  wifi.conectar();
  reloj.begin();

  if (wifi.estaConectado()) {
    reloj.sincronizarConNTP();
  } else {
    Serial.println("No hay conexión WiFi, no se sincronizará con NTP.");
  }
}

void loop() {
  unsigned long Millis_actual = millis();

  if (Millis_actual - Ultima_muestra >= Intervalo_muestra) {
    Ultima_muestra = Millis_actual;

    String hora = reloj.getTimeString();
    Serial.println(hora);
  }
}

```  

Este proceso:

- Se conecta automáticamente al servidor pool.ntp.org.

- Ajusta la hora tomando en cuenta el huso horario de Argentina (UTC-3).

- Actualiza el módulo RTC DS3231 con la hora obtenida de internet.

- Imprime en el monitor serie la nueva hora configurada.  



Con estos pasos el RTC obtiene la hora mundial, y puede usarse incluso sin conexión después.

## ⚙️ Métodos disponibles
`DS3231Clock()`  
Constructor. Crea un objeto RTC sin iniciarlo aún.

`void begin()`  
Inicializa el módulo DS3231. Debe llamarse en setup(). También verifica si el RTC perdió alimentación.

`void setDateTime(int year, int month, int day, int hour, int minute, int second)`  
Establece la fecha y hora manualmente.

Ejemplo:

```cpp
  
reloj.setDateTime(2025, 5, 15, 12, 30, 0);  // 15 de mayo 2025, 12:30:00  
```  

`String getTimeString()`  
Devuelve la hora y fecha actual en formato "YYYY-MM-DD HH:MM:SS" como String.

Ejemplo:

```cpp

Serial.println(reloj.getTimeString());  // "2025-05-15 12:30:00"  
```  

`uint32_t getEpochTime()`  
Devuelve el tiempo actual en formato Unix timestamp (segundos desde 1970).

Ejemplo:

```cpp

uint32_t t = reloj.getEpochTime();  // 1700000000, por ejemplo  
```  

`void sincronizarConNTP()`  
Actualiza la hora del RTC usando un servidor NTP, si el ESP32 está conectado a WiFi.

Requisitos:

Llamar solo si `WiFi.status() == WL_CONNECTED.`

Utiliza `"pool.ntp.org"` como servidor NTP por defecto.



	
## 💡 Consideraciones a tener en cuenta
- Se puede sincronizar con NTP solo una vez al arrancar, o programar una actualización periódica.  

- El ESP32 debe tener acceso a internet en el momento de sincronizar.  
  
- Una vez configurada, el DS3231 mantiene la hora internamente incluso si se corta la energía (gracias a su batería interna).

- Se puede sincronizar periódicamente llamando a sincronizarConNTP() cada cierto tiempo, por ejemplo, una vez por semana.