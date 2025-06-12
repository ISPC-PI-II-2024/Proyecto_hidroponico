
# 🛠️ **Manual Técnico: Hidroponia Inteligente en ESP32**  

## 📌 **1. Arquitectura del Sistema**  

El sistema hidroponía inteligente está basado en **ESP32**, y se organiza en módulos independientes para cada funcionalidad.  
La estructura del código se encuentra organizada en las siguientes carpetas:  

```plaintext
Hidroponia/
├── Hidroponia.ino               # Código principal
├── Configuración.h              # Configuración general
├── Configuración_avanzada.h     # Configuración avanzada
├── src/
│   ├── sensores/                # Módulos de sensores (pH, temperatura, nivel de agua, flujo)
│   ├── control/                 # Control automático de riego e iluminación
│   ├── pantalla/                # Interfaz de pantalla TFT y menús
│   ├── comunicacion/            # Conexiones WiFi, MQTT, BLE Mesh y LoRa Mesh
│   ├── rtc/                     # Manejo del reloj RTC
│   ├── recetas/                 # Gestión de recetas de cultivo
└── bibliotecas/                  # Librerías externas necesarias
```

Cada módulo es independiente, lo que permite modificar el código sin afectar otras partes del sistema.  

---

## 📌 **2. Inicialización del Sistema (`setup()`)**  

Cuando el ESP32 se enciende, ejecuta la función `setup()` en `Hidroponia.ino`, donde se realizan las siguientes acciones:  

✔ **Inicializar pantalla TFT** para mostrar información visual.  
✔ **Inicializar sensores** y realizar la primera medición.  
✔ **Conectar a WiFi y MQTT**, asegurando la comunicación con el servidor.  
✔ **Activar módulos de comunicación** (BLE Mesh y LoRa Mesh).  
✔ **Cargar recetas almacenadas en EEPROM** y aplicar los parámetros.  
✔ **Definir si el nodo funcionará como transmisor o receptor**, según `transmisor.h` y `receptor.h`.  
✔ **Inicializar RTC** para manejar la hora y permitir sincronización.  

---

## 📌 **3. Ciclo de Operación (`loop()`)**  

El código ejecuta un ciclo continuo en la función `loop()`, donde se realiza:  

✔ **Actualización de la pantalla TFT** con los datos más recientes.  
✔ **Gestión de botones físicos** para navegar entre los menús.  
✔ **Lectura de sensores** cada cierto intervalo (ejemplo: 5 segundos).  
✔ **Verificación de alarmas** si algún valor está fuera de los rangos seguros.  
✔ **Control automático** de bomba y luces según condiciones ambientales.  
✔ **Envío de datos** por MQTT si el nodo es transmisor.  
✔ **Recepción y ejecución de comandos** si el nodo es receptor.  
✔ **Sincronización de fecha y hora con RTC**.  

---

## 📌 **4. Comunicación del Sistema**  

### **🔹 Conexión WiFi y MQTT (`wifi_manager.h` y `comunicacion.h`)**  
- Se inicia WiFi con las credenciales guardadas en EEPROM.  
- Se conecta a un servidor MQTT.  
- Se suscribe a tópicos MQTT para recibir comandos.  
- Si hay desconexión, el sistema intenta reconectar automáticamente.  

### **🔹 Comunicación BLE Mesh (`ble_mesh.h`)**  
- Se configura un servidor BLE.  
- Se define un servicio y característica para enviar y recibir datos.  
- Cuando otro ESP32 envía datos, la función `onWrite()` los procesa y ejecuta acciones.  

### **🔹 Comunicación LoRa Mesh (`lora_mesh.h`)**  
- Se inicializa el módulo LoRa.  
- Se define una función para **enviar paquetes de datos LoRa** a otros nodos.  
- Se procesa la recepción de datos entrantes para ejecutar acciones en el nodo receptor.  

---

## 📌 **5. Modo Transmisor y Modo Receptor**  

📡 **Modo Transmisor (`transmisor.h`)**  
✔ Recoge datos de los sensores.  
✔ Envía los datos a otros dispositivos vía MQTT, LoRa o BLE.  

🎯 **Modo Receptor (`receptor.h`)**  
✔ Recibe datos de otros nodos.  
✔ Procesa los mensajes y ejecuta comandos (por ejemplo, activar iluminación).  

Estos modos pueden activarse o desactivarse con la variable `esNodoTransmisor` o `esNodoReceptor`.  

---

## 📌 **6. Almacenamiento de Recetas de Cultivo en EEPROM**  

Las recetas de cultivo se guardan en EEPROM y permiten modificar los parámetros sin necesidad de reprogramar el código.  

✔ **Estructura de receta (`recetas.h`)**  
```cpp
struct Receta {
    char nombre[20];
    float tempAgua;
    float tempAire;
    float humedad;
    float nivelAgua;
    float ph;
    bool iluminacion;
};
```
✔ **Carga y guardado en EEPROM (`recetas.cpp`)**  
```cpp
void cargarRecetas() {
    EEPROM.begin(512);
    EEPROM.get(0, recetas);
}
void guardarRecetas() {
    EEPROM.put(0, recetas);
    EEPROM.commit();
}
```

---

## 📌 **7. Expansión del Sistema (Agregar Sensores y Mejoras)**  

Para **agregar un sensor nuevo**, sigue estos pasos:  
1️⃣ **Define el pin en `sensores.h`**  
2️⃣ **Crea una función de lectura en `sensores.cpp`**  
3️⃣ **Modifica `loop()` en `Hidroponia.ino`** para llamar a la función periódicamente.  
4️⃣ **Si es necesario, envía los datos por MQTT en `transmisor.cpp`**  

Ejemplo para un sensor de CO2:  

### **Archivo: `sensores.h`**  
```cpp
#define SENSOR_CO2_PIN 37
```

### **Archivo: `sensores.cpp`**  
```cpp
float leerCO2() {
    return analogRead(SENSOR_CO2_PIN) * 0.1;
}
```

### **Modificación en `loop()` (`Hidroponia.ino`)**  
```cpp
float co2 = leerCO2();
Serial.println("CO2: " + String(co2));
```

### **Envío de datos MQTT en `transmisor.cpp`**  
```cpp
String datos = "{\"CO2\":" + String(co2) + "}";
client.publish("hidroponia/datos", datos.c_str());
```

---

## 📌 **8. Agregar Nuevas Automatizaciones**  

Para **agregar una automatización**, por ejemplo, ajuste de nutrientes según el pH:  

### **Archivo: `control.h`**  
```cpp
void ajustarNutrientes();
```

### **Archivo: `control.cpp`**  
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

### **Llamada en `loop()` (`Hidroponia.ino`)**  
```cpp
ajustarNutrientes();
```

---

