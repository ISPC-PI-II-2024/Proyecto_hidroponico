# 🚿 Proyecto: Control de Bomba de Agua con ESP32 vía WiFi

---

## 📋 Descripción general

Este proyecto permite **encender** y **apagar** una **bomba de agua de 5V** de forma remota mediante una **página web** servida desde un **ESP32**.\
También muestra el **estado actual** (encendida o apagada) utilizando **LEDs indicadores**.

El ESP32 crea un **servidor web** que puede ser accedido desde cualquier dispositivo conectado a la **misma red WiFi**.

---

## ⚙️ Componentes utilizados

| Componente          | Descripción               |
| ------------------- | ------------------------- |
| ESP32 Dev Module    | Microcontrolador WiFi     |
| Relé de 5V          | Controla la bomba de agua |
| Bomba de agua 5V    | Actuador principal        |
| LED Verde           | Indica "Bomba Encendida"  |
| LED Rojo            | Indica "Bomba Apagada"    |
| Resistencias (220Ω) | Para los LEDs             |
| Cables de conexión  |                           |

---

## 🧹 Organización del Código

El proyecto está organizado en los siguientes archivos:

| Archivo          | Propósito                                                                  |
| ---------------- | -------------------------------------------------------------------------- |
| `main.cpp`       | Inicializa el ESP32, configura pines, conecta WiFi e inicia el servidor.   |
| `webserver.cpp`  | Define las rutas web (`/`, `/encender`, `/apagar`) y genera la página web. |
| `my_webserver.h` | Declara constantes, pines y funciones relacionadas con el servidor.        |

---

## 🛠️ Instalación y Uso

1. **Modificar credenciales WiFi** en `main.cpp`:
   ```cpp
   WiFi.begin("TU_SSID", "TU_PASSWORD");
   ```
2. **Subir el código** al ESP32 mediante PlatformIO o Arduino IDE.
3. **Conectar la bomba** de agua a través del módulo relé.
4. **Abrir el Monitor Serie** y tomar nota de la **IP asignada** al ESP32.
5. **Ingresar esa IP** desde cualquier navegador web conectado a la misma red.
6. **Controlar la bomba** usando los botones "ENCENDER" y "APAGAR".

---

## 🌐 Funcionamiento del servidor web

- Al ingresar a la página, verás dos botones:

  - **ENCENDER**: Activa la bomba, enciende LED verde.
  - **APAGAR**: Desactiva la bomba, enciende LED rojo.

- El servidor utiliza rutas:

  - `/` → Página principal con los botones.
  - `/encender` → Activa el relé y el LED verde.
  - `/apagar` → Desactiva el relé y enciende el LED rojo.

---

---

## 🔌 Diagrama de conexiones

| ESP32 Pin | Conexión                                | Descripción                 |
| --------- | --------------------------------------- | --------------------------- |
| 27        | IN del módulo Relé                      | Controla el estado del relé |
| 25        | Ánodo LED Verde (con resistencia 220Ω)  | Indica bomba encendida      |
| 26        | Ánodo LED Rojo (con resistencia 220Ω)   | Indica bomba apagada        |
| GND       | GND Relé y GND LEDs                     | Tierra común                |
| 5V        | VCC Relé (opcional si el relé es de 5V) | Alimentación                |

- El relé cierra o abre el circuito de la bomba, permitiendo su encendido o apagado.
- Los LEDs informan visualmente el estado actual.

---

## 📜 Código fuente resumido

### `main.cpp`

```cpp
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, HIGH);

  WiFi.begin("TU_SSID", "TU_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) { delay(1000); }
  setupWebServer();
}

void loop() {
  server.handleClient();
}
```

### `webserver.cpp`

```cpp
server.on("/", HTTP_GET, []() {
  server.send(200, "text/html", "<html>...</html>");
});

server.on("/encender", HTTP_POST, []() {
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_ROJO, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
});

server.on("/apagar", HTTP_POST, []() {
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
});
```

### `my_webserver.h`

```cpp
#define RELAY_PIN 27
#define LED_VERDE 25
#define LED_ROJO 26

extern WebServer server;
void setupWebServer();
```

---

## ⚡ Recomendaciones

- Verificar las conexiones del relé (IN, GND, VCC).
- Asegurar que la bomba no sobrepase la capacidad del relé.
- Usar fuentes de alimentación separadas si se maneja carga alta.
- Tener precaución al trabajar con agua cerca de dispositivos electrónicos.

---

## 👨‍💻 Alumno del ISPC

Proyecto realizado NicolasB-27&#x20;

---

---
