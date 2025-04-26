
# 🎯 Integracion sensor, RTC con Salida JSON
En esta integracion se propone un programa que:

- Inicializa el BMP280 y el RTC.  
- Pide la hora actual.  
- Lee presión, temperatura y calcula altura.  
- Crea un JSON con los datos obtenidos.  
- Imprime el JSON formateado (simulando un payload MQTT).  

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

