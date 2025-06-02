#include "wifi.h"

// Inicializa puntero a nullptr
wifi* wifi::_instance = nullptr;

wifi::wifi(HardwareSerial& serial)
  : _serial(serial), _state(State::Idle)
{
  _instance = this;
}

void wifi::begin() {
  _serial.begin(115200);
  // Registrar el callback de eventos Wi-Fi
  WiFi.onEvent(wifi::onWiFiEvent);

  if (!cargarCredentiales()) {
    _state = State::WaitingCredentials;
    solicitarCredentiales();
  } else {
    startConnect();
  }
}

void wifi::loop() {
  switch (_state) {
    case State::WaitingCredentials:
      if (leerCredentiales()) {
        guardarCredentiales();
        startConnect();
      }
      break;
    case State::Connecting:
      // Esperando callback de evento
      break;
    case State::Connected:
      // Ya conectado; aquí podrías iniciar tus sensores
      break;
    case State::Error:
      // En caso de fallo podrías reintentar o resetear credenciales
      break;
    default:
      break;
  }
}

wifi::State wifi::getState() const {
  return _state;
}

void wifi::solicitarCredentiales() {
  _serial.println("\n--- Configurar WiFi ---");
  _serial.println("Introduce SSID:");
}

bool wifi::leerCredentiales() {
  if (_serial.available()) {
    _ssid = _serial.readStringUntil('\n');
    _ssid.trim();
    _serial.println("Introduce Password:");
    // Bloqueante hasta tener algo
    while (!_serial.available()) { delay(10); }
    _pass = _serial.readStringUntil('\n');
    _pass.trim();
    return true;
  }
  return false;
}

void wifi::guardarCredentiales() {
  _prefs.begin("wifi", false);
  _prefs.putString("ssid", _ssid);
  _prefs.putString("pass", _pass);
  _prefs.end();
  _serial.println("Credenciales guardadas.\n");
}

bool wifi::cargarCredentiales() {
  _prefs.begin("wifi", true);
  bool ok = _prefs.isKey("ssid") && _prefs.isKey("pass");
  if (ok) {
    _ssid = _prefs.getString("ssid");
    _pass = _prefs.getString("pass");
  }
  _prefs.end();
  return ok;
}

void wifi::startConnect() {
  _state = State::Connecting;
  _serial.printf("Conectando a '%s'...\n", _ssid.c_str());
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid.c_str(), _pass.c_str());
}

void wifi::onWiFiEvent(WiFiEvent_t event) {
  if (!_instance) return;
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      _instance->_state = State::Connected;
      _instance->_serial.println("Conectado a WiFi. ¡IP obtenida!");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      _instance->_state = State::Error;
      _instance->_serial.println("Desconectado o fallo de conexión.");
      break;
    default:
      break;
  }
}
