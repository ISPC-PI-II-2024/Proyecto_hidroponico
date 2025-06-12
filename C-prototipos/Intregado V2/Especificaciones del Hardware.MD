
# 🛠️ **Especificaciones del Hardware: Hidroponia Inteligente en ESP32**  

## 📌 **1. Microcontrolador Principal: ESP32**  
El sistema está basado en el **ESP32**, un microcontrolador con conectividad WiFi y Bluetooth, múltiples entradas analógicas/digitales y capacidad de procesamiento eficiente para proyectos IoT y automatización.

### **🔹 Características Clave del ESP32:**  
✔ 240 MHz de frecuencia de CPU.  
✔ 520 KB de SRAM.  
✔ WiFi integrado para conexión remota.  
✔ Bluetooth BLE para comunicación de corto alcance.  
✔ Múltiples puertos ADC y PWM para sensores y actuadores.  
✔ Bajo consumo energético para funcionamiento eficiente.  

### **🔹 Conexión del ESP32:**  
✔ **Alimentación:** Se puede alimentar con **5V desde un adaptador USB** o con una **batería externa**.  
✔ **Pines digitales:** Se usan para controlar el relé de la bomba y la iluminación.  
✔ **Pines analógicos:** Se usan para sensores como pH, flujo y nivel de agua.  

---

## 📌 **2. Sensores Integrados en el Sistema**  

El sistema cuenta con varios sensores para monitoreo en tiempo real:  

### **🔹 Sensor de Temperatura y Humedad (DHT11 / DHT22)**  
✔ Conectado a un **pin digital** del ESP32.  
✔ Mide temperatura y humedad ambiente.  

### **🔹 Sensor de Temperatura del Agua (DS18B20)**  
✔ Conectado mediante **OneWire** a un pin del ESP32.  
✔ Mide la temperatura del agua para optimizar el cultivo.  

### **🔹 Sensor de Nivel de Agua (Ultrasónico HC-SR04)**  
✔ Utiliza **dos pines digitales**: `Trigger` y `Echo`.  
✔ Mide la distancia hasta la superficie del agua.  

### **🔹 Sensor de Flujo de Agua (Caudalímetro YF-S201)**  
✔ Conectado a un **pin de interrupción** del ESP32.  
✔ Mide el flujo de agua en litros por minuto.  

### **🔹 Sensor de pH (Electrodo + Amplificador PH-4502C)**  
✔ Conectado a un **pin analógico** del ESP32.  
✔ Proporciona medición del pH del agua en tiempo real.  

### **🔹 Sensor de Luz (LDR)**  
✔ Conectado a un **pin analógico**.  
✔ Detecta la cantidad de luz ambiental para activar la iluminación artificial.  

### **🔹 Sensor de Gas (MQ7)**  
✔ Conectado a un **pin analógico**.  
✔ Detecta concentraciones de gases que puedan afectar el cultivo.  

### **🔹 Sensor de Corriente y Voltaje (INA219)**  
✔ Conectado vía **I2C** para medir corriente y voltaje en el sistema.  
✔ Permite monitorear el consumo de energía del sistema hidropónico.  

---

## 📌 **3. Actuadores del Sistema**  

El sistema controla dispositivos físicos a través de relés y bombas:  

### **🔹 Relé para la Bomba de Agua**  
✔ Conectado a un **pin digital** del ESP32.  
✔ Activa o desactiva el bombeo según el nivel de agua.  

### **🔹 Relé para la Iluminación Artificial**  
✔ Conectado a un **pin digital** del ESP32.  
✔ Enciende o apaga luces LED para mejorar el crecimiento de las plantas.  

---

## 📌 **4. Módulos de Comunicación**  

### **🔹 Conexión WiFi + MQTT**  
✔ Se configura en `wifi_manager.h` y `comunicacion.h`.  
✔ Permite enviar datos a servidores remotos y recibir comandos.  

### **🔹 BLE Mesh (Bluetooth de corto alcance entre nodos)**  
✔ Configurado en `ble_mesh.h` y `ble_mesh.cpp`.  
✔ Permite comunicación entre múltiples ESP32 sin requerir WiFi.  

### **🔹 LoRa Mesh (Comunicación de larga distancia sin WiFi)**  
✔ Se configura en `lora_mesh.h` y `lora_mesh.cpp`.  
✔ Permite enviar datos a largas distancias en entornos sin Internet.  

---

## 📌 **5. Diagrama de Conexión**  

```
ESP32
 ├── Sensores:
 │   ├── DHT11 (Temp/Humedad) → Pin Digital
 │   ├── DS18B20 (Temp Agua) → Pin OneWire
 │   ├── HC-SR04 (Nivel Agua) → Pines Trigger/Echo
 │   ├── YF-S201 (Flujo Agua) → Pin de interrupción
 │   ├── PH-4502C (Sensor pH) → Pin Analógico
 │   ├── LDR (Sensor de Luz) → Pin Analógico
 │   ├── MQ7 (Sensor de Gas) → Pin Analógico
 │   ├── INA219 (Corriente/Voltaje) → I2C
 ├── Actuadores:
 │   ├── Relé Bomba Agua → Pin Digital
 │   ├── Relé Iluminación → Pin Digital
 ├── Comunicación:
 │   ├── WiFi (MQTT) → `wifi_manager.h`
 │   ├── BLE Mesh → `ble_mesh.h`
 │   ├── LoRa Mesh → `lora_mesh.h`
```

---
