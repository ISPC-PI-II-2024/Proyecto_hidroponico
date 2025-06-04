#include "GSM.h"

//================================
// Clase GSM
//================================

GSM::GSM(HardwareSerial& consoleSerial,
         HardwareSerial& modemSerial,
         uint32_t baudRate)
 : _console(consoleSerial)
 , _modemSerial(modemSerial)
 , _baud(baudRate)
 , _modem(_modemSerial)
 , _state(State::Inactivo)
{}

//-----------------------------------
// Métodos públicos inicio
//-----------------------------------
void GSM::begin() {
  
  _modemSerial.begin(_baud);                // Iniciar el Serial del módem a la velocidad correcta
  if (!cargarCredenciales()) {              // Carga o pide credenciales
    _state = State::EsperandoCredenciales;
    solicitarCredenciales();
  } else {
    comenzarReinicio();
  }
}

//-----------------------------------
// Métodos públicos loop
//-----------------------------------
void GSM::loop() {
  switch (_state) {
    case State::EsperandoCredenciales:
      if (leerCredenciales()) {
        guardarCredenciales();
        comenzarReinicio();
      }
      break;

    case State::Reiniciando:
      verificarReinicio();
      break;

    case State::RegistrandoEnRed:
      verificarRegistroRed();
      break;

    case State::GprsConectando:
      verificarConexionGprs();
      break;

    case State::Conectado:  // Aquí podríamos verificar si la GPRS se cae, reintentos, etc.      
      break;

    case State::Error:      // Queda en estado de error hasta un reinicio externo
      break;

    default:
      break;
  }
}

//-----------------------------------
// Métodos públicos de acceso
//-----------------------------------
GSM::State GSM::getState() const {
  return _state;
}

//-----------------------------------
// Devuelve la referencia al módem TinyGsm
//-----------------------------------
TinyGsm& GSM::getModem() {    // Devuelve la referencia al módem TinyGsm
  return _modem;
}

//----------------------------------------------------------------
// 1) Entrada de credenciales
//----------------------------------------------------------------
void GSM::solicitarCredenciales() {
  _console.println("\n--- Configurar GSM ---");
  _console.println("Introduce APN:");
}

bool GSM::leerCredenciales() {
  if (!_console.available()) return false; // Espera a que haya datos en consola

  _apn = _console.readStringUntil('\n');
  _apn.trim();
  _console.println("Usuario APN (enter para vacío):");
  while (!_console.available()) delay(10);
  _user = _console.readStringUntil('\n');
  _user.trim();
  _console.println("Password APN (enter para vacío):");
  while (!_console.available()) delay(10);
  _pass = _console.readStringUntil('\n');
  _pass.trim();
  return true;
}

//----------------------------------------------------------------
// Persistencia de credenciales
//----------------------------------------------------------------
void GSM::guardarCredenciales() {
  _prefs.begin("gsm", false);
  _prefs.putString("apn",  _apn);
  _prefs.putString("user", _user);
  _prefs.putString("pass", _pass);
  _prefs.end();
  _console.println("→ Credenciales guardadas");
}

bool GSM::cargarCredenciales() {
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

//----------------------------------------------------------------
// 2) Pasos de conexión
//----------------------------------------------------------------
void GSM::comenzarReinicio() {
  _console.println("→ Reiniciando módem GSM...");
  _state     = State::Reiniciando;
  _startMs   = millis();
  _timeoutMs = 10UL * 1000;     // 10 s

  if (!_modem.restart()) {
    _console.println("Reinicio FALLIDO");
    _state = State::Error;
    return;
  }
  
  comenzarRegistroRed();      // restart() es bloqueante y, cuando termina, ya podemos registrar
}

void GSM::verificarReinicio() {
  // Dado que restart() bloqueó y ya llamó a comenzarRegistroRed(),
  // aca no hace falta nada, solo queda el apartado vacio.
}

void GSM::comenzarRegistroRed() {
  _console.println("→ Registrando en red celular…");
  _state     = State::RegistrandoEnRed;
  _startMs   = millis();
  _timeoutMs = 60UL * 1000;  // 60 s
}

void GSM::verificarRegistroRed() {
  if (_modem.isNetworkConnected()) {
    _console.println("Red celular OK");
    comenzarConexionGprs();
  }
  else if (millis() - _startMs > _timeoutMs) {
    _console.println("Timeout red celular");
    _state = State::Error;
  }
}

void GSM::comenzarConexionGprs() {
  _console.printf("→ Conectando GPRS (APN='%s')…\n", _apn.c_str());
  _state     = State::GprsConectando;
  _startMs   = millis();
  _timeoutMs = 30UL * 1000;  // 30 s

  // Llama a gprsConnect (bloqueante hasta ~1 s normalmente)
  if (!_modem.gprsConnect(_apn.c_str(), _user.c_str(), _pass.c_str())) {
    _console.println("GPRS FALLIDO");
    _state = State::Error;
  }
}

void GSM::verificarConexionGprs() {
  if (_modem.isGprsConnected()) {
    _console.print("GPRS OK, IP: ");
    _console.println(_modem.getLocalIP());
    _state = State::Conectado;
  }
  else if (millis() - _startMs > _timeoutMs) {
    _console.println("Timeout GPRS");
    _state = State::Error;
  }
}
