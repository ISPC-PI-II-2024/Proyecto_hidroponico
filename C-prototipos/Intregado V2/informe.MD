
# Informe Global del Proyecto: Hidroponia Inteligente

El proyecto **Hidroponia Inteligente** está diseñado para monitorear y controlar sistemas hidropónicos usando un ESP32. Su arquitectura modular permite una integración flexible de diversos sensores (temperatura, humedad, pH, nivel de agua, flujo, LDR, gas, INA219) y sistemas de comunicación (WiFi+MQTT, BLE Mesh, LoRa Mesh). Adicionalmente, el sistema incorpora un menú interactivo basado en una pantalla TFT para la selección de opciones y recetas de cultivo, así como un módulo RTC para sincronización de fecha y hora. El sistema admite la operación en modo transmisor o receptor para adaptarse a diferentes arquitecturas de red.

## Características Principales

- **Sensores**: Lectura de información ambiental, de agua, flujo y pH.  
- **Control Automático**: Automatización de riego, iluminación (relé para luces) y alertas basadas en umbrales de seguridad.  
- **Comunicación**:  
  - **WiFi y MQTT**: Conexión remota para enviar y recibir datos.  
  - **BLE Mesh**: Comunicación local entre nodos vía Bluetooth.  
  - **LoRa Mesh**: Comunicación a larga distancia ideal para áreas remotas.
- **RTC**: Módulo de reloj (DS1307/DS3231) para gestionar hora y fecha, permitiendo sincronización y programación de eventos.  
- **Recetas de Cultivo**: Permite guardar y aplicar parámetros óptimos para diferentes cultivos (por ejemplo, lechuga, tomate, albahaca).  
- **Menú Interactivo**: Basado en una pantalla TFT que habilita la navegación entre diferentes opciones: sensores, control, recetas, WiFi, entre otros.  
- **Modos de Operación**:  
  - **Transmisor**: Nodo que recopila y envía datos.  
  - **Receptor**: Nodo que recibe datos y ejecuta acciones basadas en comandos recibidos.

## Arquitectura y Escalabilidad

La estructura modular del código permite realizar ampliaciones y modificaciones sin afectar el funcionamiento global. Cada funcionalidad se encuentra separada en módulos, lo que hace al sistema escalable y de fácil mantenimiento. Además, la integración de múltiples protocolos de comunicación garantiza robustez y adaptabilidad al entorno en que se implemente.

---

# Informe por Carpeta del Proyecto Hidroponia

A continuación, se detalla qué encontrarás en cada carpeta y los archivos asociados, junto con una breve descripción de su función.

## 1. **Raíz del Proyecto**

- **Hidroponia.ino**  
  Archivo principal donde se integran y utilizan todos los módulos. Aquí se define el ciclo de trabajo (setup y loop) que gestiona la inicialización de los distintos módulos, la lectura periódica de sensores, y la gestión de comunicaciones y menús.

- **Configuración.h**  
  Contiene definiciones básicas para la configuración global del sistema (por ejemplo, velocidad de comunicación, versión del firmware, constantes generales).

- **Configuración_avanzada.h**  
  Aquí se establecen parámetros más finos y de calibración, como constantes para calibrar el sensor de pH o la temperatura. Permite ajustar valores sin modificar el código base en otros módulos.

## 2. **src/sensores**

Contiene todo lo relacionado con la lectura y gestión de los sensores.

- **sensores.h / sensores.cpp**  
  Funciones para inicializar y leer datos de varios sensores: DHT11 (temperatura y humedad ambiente), DS18B20 (temperatura del agua), sensor ultrasónico (nivel de agua), caudalímetro (flujo de agua), LDR (luz ambiente), MQ7 (sensor de gas) e INA219 (corriente, voltaje y potencia).

- **ph_sensor.h / ph_sensor.cpp**  
  Código específico para la lectura del sensor de pH. Incluye declaraciones para inicializar el sensor y leer el valor de pH, utilizando una conversión lineal básica que se puede calibrar según las características del sensor.

## 3. **src/control**

Contiene la lógica de control y automatización del sistema.

- **control.h / control.cpp**  
  Módulo que define los umbrales para cada sensor (por ejemplo, rangos de temperatura, humedad, nivel de agua) y gestiona la activación/desactivación de dispositivos (como la bomba de agua) según los datos obtenidos. Incluye funciones para verificar alarmas, automatizar riego y notificar estados.

- **iluminacion.h / iluminacion.cpp**  
  Maneja el control de un relé destinado a la iluminación artificial. Permite inicializar el relé y cambiar su estado (encendido/apagado) para complementar la entrada natural de luz.

## 4. **src/pantalla**

Módulo responsable de la interfaz visual y los menús interactivos.

- **pantalla.h / pantalla.cpp**  
  Código de inicialización y manejo de la pantalla TFT, utilizando la librería Adafruit GFX y ST7735. Se incluyen funciones para animaciones (por ejemplo, modo standby con parpadeo de "ojos") y para actualizar la pantalla según el estado del sistema.

- **menu.h / menu.cpp**  
  Funciones para presentar el Menú Principal. Permite navegar entre las diferentes opciones de configuración del sistema (sensores, control, recetas, WiFi).

- **menu_recetas.h / menu_recetas.cpp**  
  Módulo dedicado al manejo y selección de recetas de cultivo. Permite visualizar y seleccionar recetas predefinidas almacenadas en EEPROM para ajustar parámetros óptimos para diferentes cultivos.

- **menu_wifi.h / menu_wifi.cpp**  
  Proporciona una interfaz para el escaneo y selección de redes WiFi disponibles. Permite al usuario elegir una red y guardar la configuración para conexiones futuras.

## 5. **src/comunicacion**

Contiene todos los módulos relacionados con la comunicación, tanto para la conexión remota como para la comunicación en malla.

- **comunicacion.h / comunicacion.cpp**  
  Módulo central que integra las funciones de comunicación: conexión a WiFi, MQTT, y llamadas a funciones de otros módulos (como BLE y LoRa). Se manejan reconexiones y la publicación de datos (por ejemplo, alertas o datos de sensores).

- **wifi_manager.h / wifi_manager.cpp**  
  Funciones para la gestión del WiFi: escaneo de redes, conexión y almacenamiento de credenciales en EEPROM para que el dispositivo se conecte automáticamente a la red configurada.

- **ble_mesh.h / ble_mesh.cpp**  
  Módulo para la comunicación BLE Mesh. Inicia el servidor BLE, define servicios y características para transmitir y recibir mensajes en la red de malla Bluetooth.

- **lora_mesh.h / lora_mesh.cpp**  
  Módulo para la comunicación LoRa Mesh. Configura el módulo LoRa (por ejemplo, el SX1278) para enviar y recibir mensajes en largas distancias en frecuencias específicas.

- **transmisor.h / transmisor.cpp**  
  Define el modo de operación para el nodo transmisor. Incluye funciones para enviar datos (por ejemplo, en formato JSON) a través de MQTT y otras redes, basándose en la lectura de sensores.

- **receptor.h / receptor.cpp**  
  Define el modo de operación para el nodo receptor. Procesa los mensajes entrantes y ejecuta acciones (por ejemplo, activar la iluminación) según comandos específicos recibidos.

## 6. **src/rtc**

Módulo para el manejo del reloj real (RTC).

- **rtc.h / rtc.cpp**  
  Código para inicializar y sincronizar el RTC (como DS1307 o DS3231). Se incluyen funciones para obtener la fecha y hora actual, y para ajustar el reloj a partir de datos externos (por ejemplo, mediante MQTT).

## 7. **src/recetas**

Módulo para la gestión de recetas de cultivo, que permiten guardar parámetros específicos para diferentes plantas.

- **recetas.h / recetas.cpp**  
  Define una estructura llamada `Receta` que guarda parámetros como temperatura del agua, temperatura ambiente, humedad, nivel de agua, pH e información sobre iluminación. Permite cargar y guardar recetas en la EEPROM, seleccionar una receta y mostrar sus parámetros.

## 8. **bibliotecas**

Esta carpeta contendrá las librerías externas que debes instalar (aunque muchas se descargan automáticamente desde el gestor de librerías de Arduino o PlatformIO). Aquí se pueden incluir, por ejemplo:  
- Adafruit_GFX, Adafruit_ST7735  
- PubSubClient  
- LoRa  
- BLEDevice  
- RTClib  
- EEPROM (ya incluida en Arduino)  

## 9. **Errores y manejo de informacion**

- **errores.h / errores.cpp** 

En la carpeta se encuetra el catagolo de categoria y mensaje por indexacion de los posibles errores en diferentes funciones del sistema, Y la funcion encargada de la gestion y producccion de los mensajes

Se adjunta documento **errores.md** con el catagolo en formato de lectura para facilitar acceso a los posibles errores contemplados
---

