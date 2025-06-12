#ifndef WEB_SERVER_WIFI_H
#define WEB_SERVER_WIFI_H

#include <Arduino.h>
#include <WebServer.h>
#include <Preferences.h>

/*
  Esto hace lo siguiente:
   - Conexión Wi-Fi (modo STA o AP para configurar). Tenes que buscar y conectar a la red Wi-Fi del ESP32
   - Entrar en la direccion: 192.168.4.1 (entiendo que igual se debe poder cambiar)
   - Servidor HTTP que muestrar formulario y parámetros (SSID, pass, broker, port, topic, deviceName).
   - Validación de cada parámetro (cliente y servidor) para no mandar moco colocando otra cosa.
   - Almacenamiento único en Preferences bajo namespace "config".
   - Reinicio automático tras guardar. (tambien se puede cambiar,y que conecte al vuelo pero no le agarre la mano)
*/

class WebServerWifi {
public:
  // Estados internos
  enum class State {
    Idle,
    Config,
    Connect
  };

  /**
   * @param serial        Referencia al Serial (para reportar logs).
   * @param buttonPin     Pin donde hay un botón (INPUT_PULLUP) para forzar modo config.
   * @param apSSID        SSID que usará el ESP32 cuando arranque en modo AP.
   * @param apPassword    Contraseña que usará el ESP32 en modo AP.
   */
  WebServerWifi(HardwareSerial& serial, uint8_t buttonPin,
           const char* apSSID, const char* apPassword);

  /**
   * Debe llamarse desde setup(). 
   * - Si el botón está presionado o no hay credenciales guardadas, arranca en modo AP+portal.
   * - En otro caso, lee SSID/PASS desde Preferences y conecta en modo STA.
   */
  void iniciar();

  /**
   * Debe llamarse en loop().
   * Cuando está en modo Config, procesa las conexiones entrantes del WebServer.
   */
  void procesarServidor();

  /** Estado actual interno (Idle, Config o Connect). */
  State getState() const { return _estado; }

private:
  void handleRoot(); /** Envía el formulario HTML con los 6 campos y validación en cliente (JS). */
  /**
   * Recibe los POST, valida cada campo en servidor, almacena en Preferences y reinicia.
   * Si algún campo no cumple, devuelve 400 con mensaje de error.
   */
  void handleSave();
  void connectToWiFi(const String& ssid, const String& password); /** Llama a WiFi.begin(ssid, pass). */
  void reportarEstado(const char* msg); /** Imprime por Serial un mensaje formateado. */

  // -----------------------------------------------------------------------
  // Miembros
  // -----------------------------------------------------------------------

  HardwareSerial& _serial;
  uint8_t         _buttonPin;
  const char*     _apSSID;
  const char*     _apPassword;

  WebServer       _server{80};
  Preferences     _prefs;

  State           _estado;
  bool            _configMode;
};

#endif
