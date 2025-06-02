#include "GSM.h"

GSM::GSM(HardwareSerial& consoleSerial,
                           HardwareSerial& modemSerial,
                           uint32_t baud)
 : _console(consoleSerial)
 , _modemSerial(modemSerial)
 , _modem(_modemSerial)
 , _state(State::Idle)
{}

void GSM::begin() {
  _console.begin(115200);
  _modemSerial.begin(baudRate_t( _modemSerial.baudRate() )); 
  // Carga o pide credenciales
  if (!cargarCredentiales()) {
    _state = State::WaitingCredentials;
    solicitarCredenciales();
  } else {
    comenzartReinicio();
  }
}

void GSM::loop() {
  switch (_state) {
    case State::WaitingCredentials:
      if (leerCredentiales()) {
        guardarCredentiales();
        comenzartReinicio();
      }
      break;

    case State::Restarting:
      verificarReinicio();
      break;

    case State::NetworkRegistering:
      verificarRegistroRed();
      break;

    case State::GprsConnecting:
      verificarConexionGprs();
      break;

    case State::Connected:
      // Aquí tu lógica de sensores
      break;

    case State::Error:
      // Podrías reintentar o resetear credenciales
      break;

    default:
      break;
  }
}

GSM::State GSM::getState() const {
  return _state;
}

//----------------------
// 1) Credenciales
//----------------------
void GSM::solicitarCredenciales() {
  _console.println("\n--- Configurar GSM ---");
  _console.println("Introduce APN:");
}

bool GSM::leerCredentiales() {
  if (!_console.available()) return false;

  _apn = _console.readStringUntil('\n');  _apn.trim();
  _console.println("Usuario APN (enter para vacío):");
  while (!_console.available()) delay(10);
  _user = _console.readStringUntil('\n'); _user.trim();

  _console.println("Password APN (enter para vacío):");
  while (!_console.available()) delay(10);
  _pass = _console.readStringUntil('\n'); _pass.trim();

  return true;
}

void GSM::guardarCredentiales() {
  _prefs.begin("gsm", false);
  _prefs.putString("apn",  _apn);
  _prefs.putString("user", _user);
  _prefs.putString("pass", _pass);
  _prefs.end();
  _console.println("→ Credenciales guardadas");
}

bool GSM::cargarCredentiales() {
  _prefs.begin("gsm", true);
  bool ok = _prefs.isKey("apn");
  if (ok) {
    _apn  = _prefs.getString("apn");
    _user = _prefs.getString("user");
    _pass = _prefs.getString("pass");
  }
  _prefs.end();
  return ok;
}

//----------------------
// 2) Reinicio módem
//----------------------
void GSM::comenzartReinicio() {
  _console.println("→ Reiniciando módem GSM...");
  _state     = State::Restarting;
  _startMs   = millis();
  _timeoutMs = 10UL * 1000;  // 10 s

  if (! _modem.restart() ) {
    _console.println("Reinicio FALLIDO");
    _state = State::Error;
    return;
  }
  // Al terminar restart (bloqueante), paso directo al registro
  comenzarRegistroRed();
}

void GSM::verificarReinicio() {
  // restart() es bloqueante y llamamos ya a comenzarRegistroRed()
}

//----------------------
// 3) Registro en red
//----------------------
void GSM::comenzarRegistroRed() {
  _console.println("→ Registrando en red celular…");
  _state     = State::NetworkRegistering;
  _startMs   = millis();
  _timeoutMs = 60UL * 1000;  // 60 s
}

void GSM::verificarRegistroRed() {
  if (_modem.isNetworkConnected()) {
    _console.println("Red celular OK");
    comenzartConexionGprs();
  }
  else if (millis() - _startMs > _timeoutMs) {
    _console.println("Timeout red celular");
    _state = State::Error;
  }
}

//----------------------
// 4) Conexión GPRS
//----------------------
void GSM::comenzartConexionGprs() {
  _console.printf("→ Conectando GPRS (APN='%s')…\n", _apn.c_str());
  _state     = State::GprsConnecting;
  _startMs   = millis();
  _timeoutMs = 30UL * 1000;  // 30 s

  // Este método es bloqueante hasta >1 s, pero TinyGSM usa su propio timeout
  if (! _modem.gprsConnect(_apn.c_str(), _user.c_str(), _pass.c_str()) ) {
    _console.println("GPRS FALLIDO");
    _state = State::Error;
  }
}

void GSM::verificarConexionGprs() {
  if (_modem.isGprsConnected()) {
    _console.print("GPRS OK, IP: ");
    _console.println(_modem.getLocalIP());
    _state = State::Connected;
  }
  else if (millis() - _startMs > _timeoutMs) {
    _console.println("Timeout GPRS");
    _state = State::Error;
  }
}
