#include "wifi_personalizado.h"

// ================================
// Constructor
// ================================
WiFiCtrl::WiFiCtrl(HardwareSerial& serial,
                   uint8_t buttonPin,
                   const char* apSSID,
                   const char* apPassword)
  : _serial(serial),
    _buttonPin(buttonPin),
    _apSSID(apSSID),
    _apPassword(apPassword),
    _server(80),
    _estado(State::Idle),
    _configMode(false) {
    // No hacemos nada más en el constructor; la inicialización completa queda en iniciar()
}

// ================================
// iniciar(): configura pin de botón, Preferences y decide modo
// ================================
void WiFiCtrl::iniciar() {
    // 1) Configuramos el botón como INPUT_PULLUP
    pinMode(_buttonPin, INPUT_PULLUP);

    // 2) Abrimos Preferences en el namespace "wifi" (lectura/escritura)
    _prefs.begin("wifi", false);

    // 3) Intentamos leer SSID y PASS guardados
    String savedSSID = _prefs.getString("ssid", "");
    String savedPASS = _prefs.getString("pass", "");

    // 4) Verificamos si el botón está presionado (activo LOW) 
    bool buttonPressed = (digitalRead(_buttonPin) == LOW);

    // 5) Decidimos:
    if (buttonPressed || savedSSID.length() == 0) {
        // No hay credenciales guardadas O el usuario pidió modo configuración
        reportarEstado("Entrando en MODO CONFIGURACIÓN (AP + servidor web)...");
        startConfigMode();
    } else {
        // Ya tenemos credenciales y NO se presionó el botón → intentamos conectar en STA
        _estado = State::Conectando;
        reportarEstado("Intentando conectar a Wi-Fi en modo STA...");

        connectToWiFi(savedSSID, savedPASS);

        // Esperamos hasta 5 000 ms para ver si conecta
        unsigned long inicio = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - inicio < 5000) {
            delay(200);
        }
        if (WiFi.status() == WL_CONNECTED) {
            _estado = State::Conectado;
            char buf[128];
            snprintf(buf, sizeof(buf),
                     "Conectado a WiFi '%s', IP: %s",
                     savedSSID.c_str(),
                     WiFi.localIP().toString().c_str());
            reportarEstado(buf);
            // Quedamos en modo “normal” (sin iniciar servidor web)
        } else {
            // No pudo conectar: credenciales inválidas o falla de red
            _estado = State::Error;
            reportarEstado("[ERROR] Error: no se pudo conectar en modo STA. Activando modo configuración...");
            startConfigMode();
        }
    }
}

// ================================
// handleClient(): despacha peticiones HTTP en modo configuración
// ================================
void WiFiCtrl::handleClient() {
    if (_configMode) {
        _server.handleClient();
    }
}

// ================================
// obtenerEstado(): devuelve el estado actual
// ================================
WiFiCtrl::State WiFiCtrl::obtenerEstado() const {
    return _estado;
}

// ================================
// startConfigMode(): arranca AP + servidor y define rutas
// ================================
void WiFiCtrl::startConfigMode() {
    _configMode = true;
    _estado = State::PortalActivo;

    // 1) Cambiamos a modo AP
    WiFi.mode(WIFI_AP);
    if (_apPassword && strlen(_apPassword) > 0) {   // AP protegido con contraseña
        WiFi.softAP(_apSSID, _apPassword);
    } else {
        WiFi.softAP(_apSSID);                       // AP abierto
    }

    // 2) Imprimimos IP del AP
    IPAddress apIP = WiFi.softAPIP(); // suele ser 192.168.4.1
    char buf[64];
    snprintf(buf, sizeof(buf), "[INFO] AP iniciado: SSID='%s', IP: %s", 
             _apSSID, apIP.toString().c_str());
    reportarEstado(buf);

    // 3) Configuramos rutas y arrancamos servidor HTTP
    setupRoutes();
    _server.begin();
    reportarEstado("[INFO] Servidor HTTP listo en el puerto 80. Accede a '/' para ingresar SSID/Pass.");
}

// ================================
// setupRoutes(): define "/" (GET) y "/save" (POST) y fallback 404
// ================================
void WiFiCtrl::setupRoutes() {
    // GET "/" → formulario HTML
    _server.on("/", HTTP_GET, [&]() {
        handleRoot();
    });

    // POST "/save" → procesa credenciales
    _server.on("/save", HTTP_POST, [&]() {
        handleSave();
    });

    // Si solicitan algo no definido, devolvemos 404
    _server.onNotFound([&]() {
        _server.send(404, "text/plain", "404: Página no encontrada");
    });
}

// ================================
// handleRoot(): envía el HTML con el formulario SSID/Password
// ================================
void WiFiCtrl::handleRoot() {
    // Construimos la página HTML (puedes modificar estilos a tu gusto)
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <title>Configurar Wi-Fi</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f2f2f2;
      text-align: center;
      margin: 0;
      padding: 20px;
    }
    .container {
      background: white;
      padding: 20px;
      border-radius: 8px;
      display: inline-block;
      margin-top: 50px;
      box-shadow: 0 2px 6px rgba(0,0,0,0.3);
    }
    input[type=text], input[type=password] {
      width: 100%;
      padding: 8px 12px;
      margin: 8px 0;
      box-sizing: border-box;
      border: 1px solid #ccc;
      border-radius: 4px;
    }
    input[type=submit] {
      background-color: #4CAF50;
      color: white;
      padding: 10px 24px;
      border: none;
      border-radius: 4px;
      cursor: pointer;
      font-size: 16px;
    }
    input[type=submit]:hover {
      background-color: #45a049;
    }
    h2 {
      margin-bottom: 20px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>Ingrese credenciales Wi-Fi</h2>
    <form action="/save" method="POST">
      <input type="text" name="ssid" placeholder="SSID" required><br>
      <input type="password" name="pass" placeholder="Contraseña" required><br>
      <input type="submit" value="Guardar y Conectar">
    </form>
  </div>
</body>
</html>
    )rawliteral";

    _server.send(200, "text/html", html);
}

// ================================
// handleSave(): lee "ssid" y "pass", valida, guarda en Preferences y reinicia
// ================================
void WiFiCtrl::handleSave() {
    // 1) Leemos parámetros del POST
    String newSSID = _server.arg("ssid");
    String newPASS = _server.arg("pass");

    // 2) Validamos que no estén vacíos
    if (newSSID.length() == 0 || newPASS.length() == 0) {
        _server.send(400, "text/plain", "[ERROR]: SSID o Password vacios");
        return;
    }

    // 3) Mostramos mensaje en Serial y guardamos en Flash
    char buf[128];
    snprintf(buf, sizeof(buf),
             "[INFO] Guardando credenciales → SSID: '%s', PASS: '%s'",
             newSSID.c_str(), newPASS.c_str());
    reportarEstado(buf);

    _prefs.putString("ssid", newSSID);
    _prefs.putString("pass", newPASS);

    // 4) Respondemos al cliente y esperamos un instante antes de reiniciar
    String respuesta = R"rawliteral(
<!DOCTYPE html>
<html>
  <body>
    <h3>Credenciales guardadas con exito.</h3>
    <p>Reiniciando dispositivo para aplicar la nueva configuracion...</p>
  </body>
</html>
    )rawliteral";
    _server.send(200, "text/html", respuesta);

    delay(1500);      // Esperamos 1.5 s para que el navegador reciba la página
    ESP.restart();    // Reiniciamos para que, al arrancar, lea las credenciales y se conecte
}

// ================================
// connectToWiFi(): configuración de WiFi.begin() en modo estación
// ================================
void WiFiCtrl::connectToWiFi(const String& ssid, const String& password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    // No esperamos aquí: el caller (iniciar()) maneja el timeout/éxito.
}

// ================================
// reportarEstado(): imprime msg por Serial
// ================================
void WiFiCtrl::reportarEstado(const char* msg) {
    _serial.println(String("[INFO] [WiFiCtrl] -> ") + msg);
}
