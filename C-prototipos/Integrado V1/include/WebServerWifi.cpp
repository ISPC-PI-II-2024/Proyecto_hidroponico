#include "WebServerWifi.h"
#include <WiFi.h>

WebServerWifi::WebServerWifi(HardwareSerial& serial,
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
}

void WebServerWifi::iniciar() {
  pinMode(_buttonPin, INPUT_PULLUP);  // 1) Configuración del pin del botón
  _prefs.begin("config", false);      // 2) Abrimos Preferences en modo lectura/escritura
  
  // 3) Leemos SSID y PASS guardados (si existen)
  String savedSSID = _prefs.getString("ssid", ""); 
  String savedPASS = _prefs.getString("pass", "");

  // 4) Si el botón está presionado (LOW) o no hay credenciales guardadas → modo Configuración
  if (digitalRead(_buttonPin) == LOW || savedSSID.length() == 0 || savedPASS.length() == 0) {
    _estado = State::Config;
    _configMode = true;

    // Arrancamos AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_apSSID, _apPassword);
    IPAddress IP = WiFi.softAPIP();
    char buf[128];
    snprintf(buf, sizeof(buf),
             "[INFO] AP arrancado con SSID='%s', IP='%s'",
             _apSSID, IP.toString().c_str());
    reportarEstado(buf);

    // Definimos rutas del WebServer
    _server.on("/", HTTP_GET, [&]() {
      handleRoot();
    });
    _server.on("/save", HTTP_POST, [&]() {
      handleSave();
    });
    _server.onNotFound([&]() {
      _server.send(404, "text/plain", "404: Página no encontrada");
    });
    
    _server.begin();                    // Arrancamos el servidor
    reportarEstado("[INFO] Servidor HTTP iniciado en modo configuración");

  } else {
    
    _estado = State::Connect;           // 5) Modo estación: conectamos a Wi-Fi con credenciales almacenadas
    _configMode = false; 

    reportarEstado(("[INFO] Conectando a Wi-Fi: '" + savedSSID + "'").c_str());
    connectToWiFi(savedSSID, savedPASS);


    unsigned long inicio = millis();     // Esperamos conexión (hasta 10 segundos)
    while (WiFi.status() != WL_CONNECTED && (millis() - inicio) < 10000) {
      delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
      char buf[128];
      snprintf(buf, sizeof(buf),
               "Conectado a Wi-Fi. IP: %s",
               WiFi.localIP().toString().c_str());
      reportarEstado(buf);
      // Aca leemos el resto de valores de configuración guardados en Preferences
      // Se puede no leer, y lo llamamos en otro lado, como en MQTT o el main
      String broker       = _prefs.getString("broker", "test.mosquitto.org");
      uint32_t port       = _prefs.getUInt("port", 1883);
      String topic        = _prefs.getString("topic", "prueba/sensor");
      String deviceName   = _prefs.getString("deviceName", "ESP32");
     
    } else {
      reportarEstado("[Error] No se pudo conectar a Wi-Fi");
    }
  }

  // 6) Cerramos Preferences. Solo se volverá a abrir en handleSave() si se necesita escribir.
  _prefs.end();
}

void WebServerWifi::procesarServidor() {
  // solo en modo Config procesamos peticiones
  if (_configMode) {
    _server.handleClient();
  }
}

void WebServerWifi::handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <title>Configurar Wi-Fi y MQTT</title>
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
      max-width: 400px;
      width: 100%;
    }
    input[type=text], input[type=password], input[type=number] {
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
  <script>
    function validarFormulario() {
      var ssid = document.forms["formConfig"]["ssid"].value.trim();
      var pass = document.forms["formConfig"]["pass"].value.trim();
      var broker = document.forms["formConfig"]["broker"].value.trim();
      var port = document.forms["formConfig"]["port"].value.trim();
      var topic = document.forms["formConfig"]["topic"].value.trim();
      var deviceName = document.forms["formConfig"]["nombreDispositivo"].value.trim();

      if (ssid.length === 0) {
        alert("El campo SSID no puede estar vacío.");
        return false;
      }
      if (pass.length === 0) {
        alert("El campo Contraseña Wi-Fi no puede estar vacío.");
        return false;
      }

      // Validar broker: solo letras, números, puntos y guiones
      var reBroker = /^[A-Za-z0-9\.-]+$/;
      if (!reBroker.test(broker)) {
        alert("Formato de Broker MQTT inválido. Solo se permiten letras, números, puntos y guiones.");
        return false;
      }

      // Validar puerto: 1–65535
      var numPort = parseInt(port, 10);
      if (isNaN(numPort) || numPort < 1 || numPort > 65535) {
        alert("Formato de Puerto MQTT inválido. Debe ser un número entre 1 y 65535.");
        return false;
      }

      // Validar topic: no espacios ni '+' ni '#'
      var reTopic = /^[^+#\s]+$/;
      if (!reTopic.test(topic)) {
        alert("Formato de Topic MQTT inválido. No se permiten espacios, '+' ni '#'.");
        return false;
      }

      // Validar deviceName: no espacios
      if (deviceName.length === 0 || deviceName.indexOf(' ') !== -1) {
        alert("Formato de Nombre de Dispositivo inválido. No puede estar vacío ni contener espacios.");
        return false;
      }

      return true;
    }
  </script>
</head>
<body>
  <div class="container">
    <h2>Configurar Wi-Fi y MQTT</h2>
    <form name="formConfig" action="/save" method="POST" onsubmit="return validarFormulario();">
      <input type="text"   name="ssid"              placeholder="SSID"                      required><br>
      <input type="password" name="pass"             placeholder="Contraseña Wi-Fi"          required><br>
      <input type="text"   name="broker"            placeholder="Broker MQTT (IP o URL)"     required><br>
      <input type="number" name="port"              placeholder="Puerto MQTT (ej. 1883)"     required><br>
      <input type="text"   name="topic"             placeholder="Topic MQTT (ej. sensor/1)"  required><br>
      <input type="text"   name="nombreDispositivo" placeholder="Nombre Dispositivo"         required><br>
      <input type="submit" value="Guardar y Conectar">
    </form>
  </div>
</body>
</html>
  )rawliteral";

  _server.send(200, "text/html", html);
}

void WebServerWifi::handleSave() {
  // 1) Leemos parámetros del POST
  String newSSID             = _server.arg("ssid");
  String newPASS             = _server.arg("pass");
  String newBroker           = _server.arg("broker");
  String newPortStr          = _server.arg("port");
  String newTopic            = _server.arg("topic");
  String newNombreDispositivo = _server.arg("nombreDispositivo");

  // 2) Validamos que no estén vacíos
  if (newSSID.length() == 0 || newPASS.length() == 0 ||
      newBroker.length() == 0 || newPortStr.length() == 0 ||
      newTopic.length() == 0 || newNombreDispositivo.length() == 0) {
    _server.send(400, "text/plain", "[ERROR]: Faltan parámetros obligatorios.");
    return;
  }

  // 3) Validamos puerto en servidor
  int newPort = newPortStr.toInt();
  if (newPort <= 0 || newPort > 65535) {
    _server.send(400, "text/plain", "[ERROR]: Puerto MQTT inválido.");
    return;
  }

  // 4) Validamos formato de broker (solo letras, números, puntos y guiones)
  bool brokerValido = true;
  for (size_t i = 0; i < newBroker.length(); i++) {
    char c = newBroker.charAt(i);
    if (!((c >= 'A' && c <= 'Z') ||
          (c >= 'a' && c <= 'z') ||
          (c >= '0' && c <= '9') ||
          c == '.' || c == '-')) {
      brokerValido = false;
      break;
    }
  }
  if (!brokerValido) {
    _server.send(400, "text/plain", "[ERROR]: Formato de Broker MQTT inválido.");
    return;
  }

  // 5) Validamos formato de topic (no espacios, ni '+' ni '#')
  if (newTopic.indexOf(' ') != -1 || newTopic.indexOf('+') != -1 || newTopic.indexOf('#') != -1) {
    _server.send(400, "text/plain", "[ERROR]: Formato de Topic MQTT inválido.");
    return;
  }

  // 6) Validamos nombreDispositivo (no espacios)
  if (newNombreDispositivo.indexOf(' ') != -1) {
    _server.send(400, "text/plain", "[ERROR]: Formato de Nombre de Dispositivo inválido.");
    return;
  }

  // 7) Reportamos en Serial y guardamos en Preferences
  char buf[256];
  snprintf(buf, sizeof(buf),
           "[INFO] Guardando → SSID:'%s' PASS:'%s' BROKER:'%s' PORT:%d TOPIC:'%s' DEVICE:'%s'",
           newSSID.c_str(),
           newPASS.c_str(),
           newBroker.c_str(),
           newPort,
           newTopic.c_str(),
           newNombreDispositivo.c_str());
  reportarEstado(buf);

  // Abrimos Preferences en modo escritura
  _prefs.begin("config", false);
  _prefs.putString("ssid", newSSID);
  _prefs.putString("pass", newPASS);
  _prefs.putString("broker", newBroker);
  _prefs.putUInt("port", static_cast<uint32_t>(newPort));
  _prefs.putString("topic", newTopic);
  _prefs.putString("deviceName", newNombreDispositivo);
  _prefs.end();

  // 8) Enviamos página de confirmación y reiniciamos
  String respuesta = R"rawliteral(
<!DOCTYPE html>
<html>
  <body>
    <h3>Configuración guardada con éxito.</h3>
    <p>Reiniciando dispositivo para aplicar todos los cambios...</p>
  </body>
</html>
  )rawliteral";

  _server.send(200, "text/html", respuesta);
  delay(1500);
  ESP.restart();
}

void WebServerWifi::connectToWiFi(const String& ssid, const String& password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  // El caller (iniciar()) hará un timeout para verificar si conecta o no
}

void WebServerWifi::reportarEstado(const char* msg) {
  _serial.println(String("[INFO] [WebServerWifi] -> ") + msg);
}
