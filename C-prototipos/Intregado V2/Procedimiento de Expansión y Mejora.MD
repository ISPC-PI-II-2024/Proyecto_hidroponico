
# 🏗️ **Procedimiento de Expansión y Mejora: Hidroponia Inteligente en ESP32**  

## 📌 **1. Introducción**  
Este documento describe cómo se pueden modificar y mejorar los diferentes módulos del sistema hidropónico, permitiendo agregar nuevos sensores, ampliar la comunicación y optimizar la automatización.  

El sistema está diseñado en **arquitectura modular**, lo que significa que cada funcionalidad es independiente y puede ser modificada sin afectar el resto del código.  

---

## 📌 **2. Agregar un Nuevo Sensor al Sistema**  

Para agregar un nuevo sensor, sigue estos pasos:  

### **1️⃣ Definir el nuevo sensor en `sensores.h`**  
Agrega el pin en el archivo `sensores.h`, por ejemplo, si deseas incluir un sensor de **CO2**:  
```cpp
#define SENSOR_CO2_PIN 37
```

### **2️⃣ Crear la función de lectura en `sensores.cpp`**  
Define la función para leer los datos del sensor:  
```cpp
float leerCO2() {
    return analogRead(SENSOR_CO2_PIN) * 0.1; // Ajustar según el sensor específico
}
```

### **3️⃣ Modificar `loop()` en `Hidroponia.ino`**  
Agrega la lectura del sensor en cada ciclo de ejecución del ESP32:  
```cpp
float co2 = leerCO2();
Serial.println("CO2: " + String(co2));
```

### **4️⃣ Enviar los datos vía MQTT en `transmisor.cpp`**  
Si el sistema está conectado a la nube, puedes enviar los datos del nuevo sensor por MQTT:  
```cpp
String datos = "{\"CO2\":" + String(co2) + "}";
client.publish("hidroponia/datos", datos.c_str());
```

✔ **Con estos pasos, el nuevo sensor se integrará al sistema sin afectar las demás funciones.**  

---

## 📌 **3. Agregar un Nuevo Menú en la Pantalla TFT**  

Si necesitas un nuevo menú para mostrar información específica, sigue estos pasos:  

### **1️⃣ Definir el nuevo menú en `menu.h`**  
```cpp
void mostrarMenuEstadisticas();
```

### **2️⃣ Implementar el menú en `menu.cpp`**  
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

### **3️⃣ Agregar la opción en el menú principal (`menu.cpp`)**  
```cpp
tft.println(opcionMenu == 4 ? "> Estadísticas" : "  Estadísticas");
```

### **4️⃣ Manejar la selección del nuevo menú**  
```cpp
if (opcionMenu == 4) mostrarMenuEstadisticas();
```

✔ **Este método permite expandir el menú sin modificar otros módulos del sistema.**  

---

## 📌 **4. Agregar una Nueva Automatización**  

Si deseas mejorar el control del sistema agregando una función para **ajustar nutrientes según el pH**, sigue estos pasos:  

### **1️⃣ Definir la nueva lógica en `control.h`**  
```cpp
void ajustarNutrientes();
```

### **2️⃣ Implementar la función en `control.cpp`**  
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

### **3️⃣ Llamar a la función en `loop()` en `Hidroponia.ino`**  
```cpp
ajustarNutrientes();
```

✔ **Este método permite agregar funciones de automatización sin cambiar la estructura base del código.**  

---

## 📌 **5. Optimización de la Comunicación**  

Si deseas enviar datos no solo por MQTT, sino también por **LoRa Mesh y BLE Mesh simultáneamente**, puedes hacerlo modificando `transmisor.cpp`:  

```cpp
void enviarDatos() {
    if (!esNodoTransmisor) return;
    
    String datos = "{\"tempAgua\":" + String(temperaturaAgua) + "}";

    client.publish("hidroponia/datos", datos.c_str());  // MQTT
    enviarDatosLoRa(datos);  // LoRa Mesh
    enviarDatosBLE(datos);  // BLE Mesh
}
```

✔ **Este método permite que el ESP32 comunique los datos por múltiples canales sin modificar el resto del sistema.**  

---

## 📌 **6. Agregar Más Recetas de Cultivo**  

Si deseas ampliar la cantidad de recetas almacenadas en **EEPROM**, sigue estos pasos:  

### **1️⃣ Modificar `recetas.h` para aumentar el límite**  
```cpp
#define MAX_RECETAS 10 // Cambia el número según lo necesario
```

### **2️⃣ Agregar nuevas recetas en `recetas.cpp`**  
```cpp
Receta recetas[MAX_RECETAS] = {
    {"Lechuga", 20.0, 22.0, 70.0, 10.0, 6.2, true},
    {"Tomate", 22.0, 25.0, 65.0, 15.0, 5.8, true},
    {"Albahaca", 21.0, 23.0, 75.0, 12.0, 6.5, true},
    {"Perejil", 19.0, 21.0, 80.0, 8.0, 6.0, false},
    {"Fresas", 20.0, 24.0, 68.0, 12.0, 5.5, true},
    {"Menta", 18.5, 21.0, 77.0, 9.0, 6.3, true},
    {"Cilantro", 20.0, 22.5, 79.0, 10.5, 6.1, false},
    {"Pimiento", 23.0, 26.0, 60.0, 14.0, 5.7, true},
    {"Espinaca", 19.0, 22.0, 75.0, 10.0, 6.4, false},
    {"Frambuesas", 20.5, 23.5, 69.0, 11.0, 5.6, true}
};
```

✔ **Este método permite expandir la lista de cultivos sin afectar la lógica del sistema.**  

---

