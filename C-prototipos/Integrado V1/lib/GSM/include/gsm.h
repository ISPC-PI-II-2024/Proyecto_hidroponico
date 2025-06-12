#pragma once

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <Preferences.h>

//================================
// Clase GSM
//================================
class GSM {
public:
  //-----------------------------------
  // Estados posibles
  //-----------------------------------
  enum class State : uint8_t {
    Inactivo,               // Antes era Idle
    EsperandoCredenciales,  // Pidiendo APN/usuario/pass
    Reiniciando,            // Reiniciando módem
    RegistrandoEnRed,       // Registrando en red celular
    GprsConectando,         // Iniciando sesión GPRS
    Conectado,              // GPRS activo
    Error                   // Cualquier fallo
  };

  // Constructor:
  //  - consoleSerial: Serial para interacción/menús
  //  - modemSerial:   UART donde está el módem AT
  //  - baudRate:      velocidad para el UART del módem
  GSM(HardwareSerial& consoleSerial,
      HardwareSerial& modemSerial,
      uint32_t baudRate = 115200);

  //-----------------------------------
  // Métodos públicos
  //-----------------------------------
  void begin();             // Debe llamarse en setup()
  void loop();              // Debe llamarse en loop()
  State getState() const;   // Devuelve el estado actual
  TinyGsm& getModem();      // Permite a otros acceder al módem (TinyGsm) para crear, por ejemplo, un TinyGsmClient


//-----------------------------------
// Métodos privados de gestión interna
//-----------------------------------
private:
  // 1) Entrada / persistencia de credenciales
  void solicitarCredenciales();
  bool leerCredenciales();
  void guardarCredenciales();
  bool cargarCredenciales();

  // 2) Pasos de conexión
  void comenzarReinicio();
  void verificarReinicio();

  void comenzarRegistroRed();
  void verificarRegistroRed();

  void comenzarConexionGprs();
  void verificarConexionGprs();

  // --- Datos internos ---
  HardwareSerial& _console;
  HardwareSerial& _modemSerial;
  uint32_t        _baud;     
  Preferences     _prefs;
  TinyGsm         _modem;

  State           _state;
  String          _apn, _user, _pass;

  unsigned long   _startMs;
  unsigned long   _timeoutMs;
};

