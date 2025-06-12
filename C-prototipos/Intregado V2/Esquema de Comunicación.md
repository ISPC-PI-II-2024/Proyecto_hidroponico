
# 📡 **Esquema de Comunicación del Sistema Hidropónico Inteligente en ESP32**  

## 📌 **1. Introducción**  
El sistema hidroponía inteligente utiliza varios protocolos de comunicación para el envío y recepción de datos en tiempo real. Dependiendo de la configuración, puede utilizar **WiFi, MQTT, BLE Mesh y LoRa Mesh** para conectar múltiples nodos ESP32 o enviar datos a servidores remotos.

### 🔹 **Tipos de Comunicación en el Sistema**
✔ **WiFi + MQTT** → Comunicación remota y almacenamiento de datos en servidores.  
✔ **BLE Mesh** → Comunicación entre nodos cercanos sin necesidad de Internet.  
✔ **LoRa Mesh** → Comunicación a larga distancia sin infraestructura de red.  
✔ **Modo Transmisor/Receptor** → Un nodo envía datos, otro los recibe y ejecuta acciones.  

---

## 📌 **2. Flujo de Datos dentro del Sistema**  

El ESP32 sigue un proceso de comunicación que se divide en **5 etapas:**  

```plaintext
1️⃣ Captura de datos de sensores → (Temperatura, pH, Nivel de agua, Luz, etc.)
2️⃣ Procesamiento de datos → (Verificación de umbrales y lógica de automatización)
3️⃣ Transmisión de datos → (Envía vía MQTT, BLE o LoRa según configuración)
4️⃣ Recepción de datos → (Nodo receptor procesa y ejecuta comandos)
5️⃣ Respuesta y ajustes → (Activación de bomba, ajuste de iluminación, alarmas)
```

---

## 📌 **3. Comunicación WiFi y MQTT**  

El protocolo MQTT permite enviar datos a servidores remotos y recibir comandos de control.  

### **🔹 Proceso de conexión a MQTT (`wifi_manager.h` y `comunicacion.h`)**  
✔ El ESP32 se conecta a WiFi y verifica la señal.  
✔ Se conecta al **servidor MQTT**, enviando credenciales de usuario.  
✔ Se suscribe a tópicos para recibir comandos (`hidroponia/control`).  
✔ Publica datos en **hidroponia/datos** cada vez que toma una nueva medición.  

### **🔹 Ejemplo de envío de datos vía MQTT (`transmisor.cpp`)**  
```cpp
String datos = "{\"tempAgua\":" + String(temperaturaAgua) + ",\"ph\":" + String(phValor) + "}";
client.publish("hidroponia/datos", datos.c_str());
```

### **🔹 Ejemplo de recepción y ejecución de comandos (`receptor.cpp`)**  
```cpp
void procesarDatos(const char* mensaje) {
    if (strstr(mensaje, "\"iluminacion\":true")) {
        activarIluminacion(true);
        Serial.println("Comando recibido: Activar iluminación.");
    }
}
```

---

## 📌 **4. Comunicación BLE Mesh**  

El protocolo **BLE Mesh** permite que múltiples ESP32 intercambien información de manera local, sin necesidad de una red WiFi.  

### **🔹 Inicialización de BLE Mesh (`ble_mesh.h` y `ble_mesh.cpp`)**  
✔ Se define un **servidor BLE** con un servicio y característica.  
✔ Se habilita el envío y recepción de datos entre nodos.  
✔ Cuando otro ESP32 transmite información, se recibe y se procesa el mensaje.  

### **🔹 Ejemplo de envío de datos vía BLE Mesh (`ble_mesh.cpp`)**  
```cpp
void enviarDatosBLE(const String& datos) {
    if (pCharacteristic != nullptr) {
        pCharacteristic->setValue(datos.c_str());
        pCharacteristic->notify();
    }
}
```

### **🔹 Ejemplo de recepción de datos en BLE Mesh (`ble_mesh.cpp`)**  
```cpp
class MyBLECallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
      std::string value = pCharacteristic->getValue();
      if (!value.empty()) {
          Serial.println("Mensaje recibido vía BLE: " + String(value.c_str()));
      }
  }
};
```

---

## 📌 **5. Comunicación LoRa Mesh**  

LoRa Mesh permite que el ESP32 envíe y reciba datos a larga distancia, ideal para ambientes agrícolas sin infraestructura WiFi.  

### **🔹 Configuración de LoRa Mesh (`lora_mesh.h` y `lora_mesh.cpp`)**  
✔ Se inicializa el módulo **LoRa SX1278** con frecuencia 915 MHz (ajustable según región).  
✔ Se establece una función para enviar paquetes de datos a otros nodos.  
✔ Se procesa la recepción de mensajes entrantes y se ejecutan comandos si es necesario.  

### **🔹 Ejemplo de envío de datos en LoRa Mesh (`lora_mesh.cpp`)**  
```cpp
void enviarDatosLoRa(const String& datos) {
    LoRa.beginPacket();
    LoRa.print(datos);
    LoRa.endPacket();
}
```

### **🔹 Ejemplo de recepción de datos en LoRa Mesh (`lora_mesh.cpp`)**  
```cpp
void recibirDatosLoRa() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String mensaje = "";
        while (LoRa.available()) {
            mensaje += (char)LoRa.read();
        }
        Serial.println("Mensaje recibido vía LoRa: " + mensaje);
    }
}
```

---

## 📌 **6. Modos de Operación: Transmisor y Receptor**  

📡 **Modo Transmisor (`transmisor.h`)**  
✔ Recopila datos de los sensores.  
✔ Envía los datos a otros dispositivos.  

🎯 **Modo Receptor (`receptor.h`)**  
✔ Recibe datos y ejecuta comandos.  
✔ Modifica configuraciones según el mensaje recibido.  

✔ Para activar o desactivar estos modos, se modifican las variables:  
```cpp
bool esNodoTransmisor = true;
bool esNodoReceptor = false;
```

---
