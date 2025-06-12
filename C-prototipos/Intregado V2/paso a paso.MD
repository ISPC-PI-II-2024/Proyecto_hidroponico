
- **Informe 1:** Explica cómo trabaja el programa paso por paso, desde la inicialización hasta la ejecución en el ESP32.  
- **Informe 2:** Explica cómo se pueden hacer ediciones para agregar mejoras al sistema.  

---

# 📝 **Informe 1: Funcionamiento del Programa Paso a Paso**  

## 📌 **1. Inicio y Configuración Inicial**  

Cuando el ESP32 se inicia, se ejecuta la función `setup()`. Esta sección se encarga de:  

1. **Inicializar la comunicación serial** para debug.  
2. **Inicializar la pantalla TFT** para la interfaz gráfica.  
3. **Inicializar sensores** (DHT11, DS18B20, nivel de agua, caudalímetro, pH, etc.).  
4. **Inicializar control de automatización** (bomba de agua, luces, alarmas).  
5. **Conectar a WiFi** y, si es necesario, a MQTT para recibir y enviar datos.  
6. **Activar módulos de comunicación** (BLE Mesh, LoRa Mesh).  
7. **Cargar recetas desde EEPROM** para aplicar parámetros de cultivo preconfigurados.  
8. **Definir si el nodo actúa como transmisor o receptor** (esto se configura en `transmisor.h` y `receptor.h`).  
9. **Inicializar RTC** para manejar fecha y hora correctamente.  

Todo esto ocurre una sola vez al arrancar el ESP32.  

## 📌 **2. Ejecución Cíclica en `loop()`**  

Después de la inicialización, el programa entra en un ciclo de ejecución infinita dentro de `loop()`. En cada ciclo se realizan varias tareas:  

✔ **Actualizar pantalla TFT** con los últimos datos obtenidos.  
✔ **Manejar la entrada de botones** para navegar en los menús.  
✔ **Mantener activa la conexión WiFi y MQTT**.  
✔ **Leer sensores** (temperatura, pH, nivel de agua, flujo, luz, gas, corriente).  
✔ **Ejecutar control automático** según los umbrales configurados:  
   - Si el **nivel de agua es bajo**, activa la bomba.  
   - Si la **luz es insuficiente**, activa la iluminación artificial.  
   - Si el **pH está fuera de rango**, envía una alerta.  
✔ **Verificar alarmas** y enviar notificaciones si se detectan valores peligrosos.  
✔ **Transmitir datos** si el nodo es transmisor.  
✔ **Recibir datos y ejecutar comandos** si el nodo es receptor.  
✔ **Actualizar hora con RTC** y sincronizar si hay conexión MQTT.  

Este ciclo se repite indefinidamente, asegurando que el sistema siempre esté monitoreando y actuando según los datos que recibe.  

## 📌 **3. Modos de Operación**  

El programa admite dos modos de operación:  

1️⃣ **Modo Transmisor:** Recoge datos de sensores y los envía vía MQTT, LoRa o BLE.  
2️⃣ **Modo Receptor:** Procesa los datos recibidos y ejecuta acciones en respuesta (por ejemplo, activando la iluminación o modificando la frecuencia de riego).  

## 📌 **4. Menús y Configuración desde Pantalla**  

El usuario puede usar los botones físicos para:  

✔ **Activar o desactivar sensores** desde el menú de configuración.  
✔ **Seleccionar una receta de cultivo** con parámetros específicos.  
✔ **Sincronizar WiFi** desde una lista de redes disponibles.  
✔ **Visualizar alertas y estados del sistema**.  

## 📌 **5. Comunicación entre Dispositivos**  

El ESP32 puede comunicarse de varias formas:  
✔ **WiFi + MQTT:** Para enviar datos a la nube o recibir comandos remotos.  
✔ **BLE Mesh:** Para conectarse con otros nodos ESP32 cercanos.  
✔ **LoRa Mesh:** Para comunicación a larga distancia sin WiFi.  

Si el sistema detecta una desconexión en WiFi o MQTT, intentará reconectar automáticamente.  

---

# 🛠️ **Informe 2: Cómo Hacer Ediciones y Agregar Mejoras**  

El proyecto está diseñado para ser **modular**, lo que significa que cada funcionalidad está separada en su propio archivo. Esto facilita la edición y la ampliación del sistema sin afectar otros módulos.  

## 📌 **1. Cómo Agregar un Nuevo Sensor**  

Para agregar un nuevo sensor:  

1️⃣ **Define su pin en `sensores.h`**, por ejemplo:  
```cpp
#define SENSOR_CO2_PIN 37
```
2️⃣ **Crea una función para leer el sensor en `sensores.cpp`**, ejemplo:  
```cpp
float leerCO2() {
    return analogRead(SENSOR_CO2_PIN) * 0.1;
}
```
3️⃣ **Actualiza `loop()` en `Hidroponia.ino`** para leer el nuevo sensor periódicamente:  
```cpp
float co2 = leerCO2();
Serial.println("CO2: " + String(co2));
```
4️⃣ **Envía los datos por MQTT** en `transmisor.cpp`:  
```cpp
String datos = "{\"CO2\":" + String(co2) + "}";
client.publish("hidroponia/datos", datos.c_str());
```

---

## 📌 **2. Cómo Agregar un Nuevo Menú en Pantalla**  

Si necesitas agregar un nuevo menú, por ejemplo, para estadísticas avanzadas:  

1️⃣ **Crea una nueva pantalla en `menu.h`**, por ejemplo:  
```cpp
void mostrarMenuEstadisticas();
```
2️⃣ **Define la lógica del menú en `menu.cpp`**, ejemplo:  
```cpp
void mostrarMenuEstadisticas() {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(10, 10);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE);
    tft.println("[ ESTADÍSTICAS ]");
    tft.println("Promedio de pH: " + String(promedioPH));
    tft.println("Flujo medio: " + String(promedioFlujoAgua));
}
```
3️⃣ **Agrega la opción en el menú principal (`menu.cpp`)**:  
```cpp
tft.println(opcionMenu == 4 ? "> Estadísticas" : "  Estadísticas");
```
4️⃣ **Maneja la selección del nuevo menú** en `menu.cpp`:  
```cpp
if (opcionMenu == 4) mostrarMenuEstadisticas();
```

---

## 📌 **3. Cómo Agregar una Nueva Función Automática**  

Si necesitas agregar una automatización nueva, como el control de nutrientes basado en el pH:  

1️⃣ **Agrega la lógica en `control.h`**, por ejemplo:  
```cpp
void ajustarNutrientes();
```
2️⃣ **Define la función en `control.cpp`**, ejemplo:  
```cpp
void ajustarNutrientes() {
    if (phValor < 5.8) {
        Serial.println("Agregando nutrientes alcalinos.");
    } else if (phValor > 7.2) {
        Serial.println("Agregando nutrientes ácidos.");
    } else {
        Serial.println("pH dentro del rango óptimo.");
    }
}
```
3️⃣ **Llama a la función desde `loop()` en `Hidroponia.ino`**:  
```cpp
ajustarNutrientes();
```

---

## 📌 **4. Cómo Modificar el Sistema de Comunicación**  

Si deseas enviar datos no solo por MQTT, sino también por LoRa y BLE simultáneamente:  

1️⃣ **Modifica `transmisor.cpp`** para incluir ambos envíos:  
```cpp
void enviarDatos() {
    if (!esNodoTransmisor) return;
    
    String datos = "{\"tempAgua\":" + String(temperaturaAgua) + "}";

    client.publish("hidroponia/datos", datos.c_str());  // Enviar por MQTT
    enviarDatosLoRa(datos);  // Enviar por LoRa
    enviarDatosBLE(datos);  // Enviar por BLE
}
```
2️⃣ **Asegúrate de que las funciones `enviarDatosLoRa()` y `enviarDatosBLE()` estén activas**.  

---
