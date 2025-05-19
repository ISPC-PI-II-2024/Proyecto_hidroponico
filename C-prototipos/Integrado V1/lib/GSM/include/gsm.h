#pragma once

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <Preferences.h>

class GSM {
public:
  // Estados principales de la máquina
  enum class State : uint8_t {
    Idle,                  // Nada iniciado
    WaitingCredentials,    // Pidiendo APN/usuario/pass
    Restarting,            // Reiniciando módem
    NetworkRegistering,    // Registrando en red celular
    GprsConnecting,        // Iniciando sesión GPRS
    Connected,             // GPRS activo
    Error                  // Cualquier fallo
  };

  // Constructor: 
  //  - consoleSerial: interfaz para usuario (credenciales, logs) 
  //  - modemSerial:   UART donde está conectado el módem AT
  //  - baud:          velocidad de ese UART
  GSM(HardwareSerial& consoleSerial,
               HardwareSerial& modemSerial,
               uint32_t baud = 115200);

  // Debe llamarse en setup()
  void begin();

  // Debe llamarse en loop()
  void loop();

  // Para que tu sketch sepa cuándo arrancar tu lógica de sensores
  State getState() const;

private:
  // 1) Entrada / persistencia de credenciales
  void solicitarCredenciales();
  bool leerCredentiales();
  void guardarCredentiales();
  bool cargarCredentiales();

  // 2) Pasos de conexión
  void comenzartReinicio();
  void verificarReinicio();

  void comenzarRegistroRed();
  void verificarRegistroRed();

  void comenzartConexionGprs();
  void verificarConexionGprs();

  // --- Datos internos ---
  HardwareSerial& _console;
  HardwareSerial& _modemSerial;
  Preferences     _prefs;
  TinyGsm         _modem;

  State           _state;
  String          _apn, _user, _pass;

  unsigned long   _startMs;
  unsigned long   _timeoutMs;
};
