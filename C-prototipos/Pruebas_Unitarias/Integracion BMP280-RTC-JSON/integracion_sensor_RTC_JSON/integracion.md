
# 🎯 Integracion sensor, RTC con Salida JSON
En esta integracion se propone un programa que:

- Inicializa el BMP280 y el RTC.  
- Pide la hora actual.  
- Lee presión, temperatura y calcula altura.  
- Crea un JSON con los datos obtenidos.  
- Imprime el JSON formateado.
- Envia el JSON por MQTT  

# Clase Jsonificadora
Se trata de una clase que permita crear archivos JSON de diferente longitud de acuerdo a la necesidad

## La clase debería:

- Poder recibir distintas mediciones (timestamp, temperatura, presión, altura, etc).

- No agregar al JSON los valores que estén "vacíos" o "no válidos" o que decidamos no transmitir. (A desarrollar)

- Permitir armar el JSON limpio sin basura o nulos innecesarios, con funciones que solo agregan si el dato es válido. (A desarrollar)  
  
## Estrategia para "solo valores nuevos"
### Dos opciones:

a) Comparar con última medición:
Solo jsonificamos si el valor cambió respecto a la última vez.

b) Flags de "cambio":
Cada sensor podría tener un bool changed que activa si hubo un cambio significativo.

Ejemplo simple:

```cpp
if (abs(nuevaTemperatura - ultimaTemperatura) > umbral) {
    // agregar temperatura al JSON
}
```  
  
# Clase JsonBuilder  

## Descripción  

La clase JsonBuilder permite construir mensajes JSON de manera fácil y ordenada, pensada especialmente para proyectos embebidos (como ESP32, Arduino, etc.).
Ideal para preparar mensajes que luego se envíen por protocolos como MQTT o HTTP.

### Dependencias
```bash
pio lib install "bblanchon/ArduinoJson"  
```  

## Métodos Disponibles  
| Método | Descripcion     |
|--------|------------|
| JsonBuilder()    | Constructor. Inicializa el objeto y limpia el documento JSON interno.      |
| void limpiar()    | Limpia completamente el contenido actual del JSON. Se debe usar para comenzar un nuevo documento. |
| void agregarCampo(const char* nombre, String valor)    |      |
| void agregarCampo(const char* nombre, float valor)    | Agrega un campo al documento JSON.     |
| void agregarCampo(const char* nombre, int valor)    |      |  
| String construir() | Construye el JSON en formato texto bonito (formato "pretty") y lo devuelve como String.    |  


nombre: Nombre que tendrá el campo en el JSON.

valor: Valor a almacenar. Puede ser un String, un float o un int.

Nota: Si el valor tipo String está vacío, no se agregará el campo.

## Ejemplo de Uso Básico
```cpp

#include "JsonBuilder.h"

JsonBuilder creadorJSON;

void setup() {
  Serial.begin(115200);

  creadorJSON.limpiar();
  creadorJSON.agregarCampo("temperatura_C", 24.7);
  creadorJSON.agregarCampo("presion_hPa", 1015.2);
  creadorJSON.agregarCampo("mensaje", "Todo en orden");

  String jsonGenerado = creadorJSON.construir();

  Serial.println("📦 JSON:");
  Serial.println(jsonGenerado);
}

void loop() {
  // No hacemos nada aquí
}  
```  

### Salida esperada:
```json
{
  "temperatura_C": 24.7,
  "presion_hPa": 1015.2,
  "mensaje": "Todo en orden"
}  
```  

## Recomendaciones
- Usar limpiar() antes de construir un nuevo JSON para evitar incluir datos anteriores.

- No saturar el documento con demasiados datos: el tamaño del StaticJsonDocument es fijo (512 bytes por defecto).

- Para campos que no siempre estén disponibles (por ejemplo, sensores desconectados), no llamar a agregarCampo o enviar un String vacío para omitirlos.

# Clase SensorDataMQTT
📂 Archivos involucrados  
SensorDataMQTT.h: declaración de la clase y su interfaz pública.  

SensorDataMQTT.cpp: definición de los métodos de la clase.  

## 📌 Objetivo general
La clase SensorDataMQTT encapsula:

- la conexión WiFi con IP estática,  
- la conexión con un broker MQTT usando la librería PubSubClient,  
- la publicación de un mensaje (en formato JSON) a un topic MQTT.  

Permite un uso limpio desde el main.cpp o setup()/loop() del programa, sin tener que ensuciar el flujo principal con detalles de red.

## 📤 ¿Qué hace exactamente?  
**Métodos públicos (public:)**  
## `SensorDataMQTT(...)`  
Constructor: inicializa la clase con parámetros de red (WiFi y MQTT), y guarda una instancia única (singleton).  

**Parámetros:**

`ssid, password`: credenciales de WiFi.

`broker, port`: datos del broker MQTT.

`ip, gateway, subnet`: configuración de IP estática.

## `void conectarWiFi()`  
Configura y conecta a la red WiFi con IP fija.

`WiFi.config(...)`: fija la IP, gateway, subnet y DNS.

`WiFi.begin(...)`: inicia la conexión.

Espera a que `WiFi.status()` sea `WL_CONNECTED`.

Muestra la IP obtenida por consola.

⚠️ DNS estática fija a 8.8.8.8 (Google DNS).

## `void conectarMQTT()`
Conecta con el broker MQTT.

Usa `client.setServer(...)` para definir IP/hostname y puerto del broker.

Intenta conectar con `client.connect(...)` bajo el nombre ESP32_bmp280.

Reintenta cada 2 segundos si falla.

⚠️ El ID "ESP32_bmp280" es fijo y hardcoded → podría ser configurable o dinámico si se conecta más de un ESP32 al mismo broker.

## `void manejarMQTT()`
Mantiene la conexión activa y llama a client.loop().

Verifica si la conexión MQTT está caída (!client.connected()).

Si es así, vuelve a llamar a conectarMQTT().

Luego, ejecuta `client.loop()` para procesar eventos del cliente.

💡 Se debe llamar en cada ciclo del loop() principal para mantener la conexión viva y recibir mensajes (aunque acá no hay subscripciones activas).

## `void publicarLecturas(String cadena_JSON)`
Publica un mensaje JSON (o cualquier String) al topic "sensores/datos".

Convierte el String a un char[] para usarlo en `client.publish(...)`.

El JSON viene como parámetro externo, y no lo construye internamente.


### Variables privadas (private:)
```cpp
const char* ssid;
const char* password;
const char* broker;
int port;
IPAddress ip, gateway, subnet;
WiFiClient espClient;
PubSubClient client;  
```
Estas variables almacenan:

- credenciales de red,

- configuración de IP fija,

- broker MQTT y puerto,

- los objetos base para comunicación: WiFiClient y PubSubClient.

- Singleton (por si se usa callback en el futuro)
```cpp
static SensorDataMQTT* instancia;  
```  

La clase guarda una instancia estática de sí misma, para manejar callbacks de MQTT a futuro.

Esto se usa en el patrón de "callback wrapper", ya que la función que recibe mensajes en PubSubClient debe ser static.

Aunque actualmente está comentado:

```cpp
//void procesarMensaje(char* topic, byte* payload, unsigned int length);
//static void callbackWrapper(char* topic, byte* payload, unsigned int length);  
```
Y en el .cpp:

```cpp
/*void SensorDataMQTT::callbackWrapper(...) {
   if (instancia) {
       instancia->procesarMensaje(...);
   }
}*/  
```  

💡 Esto permite que una función global (requerida por la lib MQTT) redirija el mensaje a un método de instancia real.

🛠️ Posibles mejoras
ID de cliente MQTT dinámico o configurable.

Callbacks MQTT: recibir y procesar mensajes desde el broker.

Reconnect exponential backoff: evitar reconectar cada 2s linealmente.

Configuración modular externa (por ejemplo desde config.h o SPIFFS).