
#include "gsm_async.h"
#include <TinyGsmClient.h>
#include <Arduino.h>

// Pines UART del módem
static const uint8_t MODEM_RX = 16;
static const uint8_t MODEM_TX = 17;
// Instancias TinyGSM
static HardwareSerial SerialGSM(1);
static TinyGsm       modem(SerialGSM);
static TinyGsmClient client(modem);

// Parámetros APN (guardados en setup)
static const char* _apn;
static const char* _user;
static const char* _pass;

static bool        _connected   = false;
static unsigned long _lastTry   = 0;
static const unsigned long RETRY_MS = 10000;

void gsmSetup(const char* apn, const char* user, const char* pass) {
  _apn  = apn; 
  _user = user;
  _pass = pass;
  SerialGSM.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

 Serial.println("GSM: comprobando comunicación...");

 if (!modem.testAT()) {
  Serial.println(" ERROR: No hay comunicación con el SIM800L");
  _connected = false;
  return;  // Salir si no hay respuesta AT
}

Serial.println(" Comunicación establecida con SIM800L");
Serial.println("GSM: reiniciando módem...");
modem.restart();  // Soft-reset AT
}

void gsmLoop() {
  unsigned long now = millis();
  // 1) Registro en red
  if (!modem.isNetworkConnected()) {
    if (now - _lastTry > RETRY_MS) {
      _lastTry = now;
      Serial.println("GSM: esperando red...");
      if (modem.waitForNetwork(60000L)) {
        Serial.println("GSM: registrado en red");
      }
    }
    return;
  }
  // 2) Conexión GPRS
  if (!modem.isGprsConnected()) {
    if (now - _lastTry > RETRY_MS) {
      _lastTry = now;
      Serial.println("GSM: conectando GPRS...");
      if (modem.gprsConnect(_apn, _user, _pass)) {
        Serial.println("GSM: GPRS activo");
        _connected = true;
      } else {
        Serial.println("GSM: fallo GPRS");
      }
    }
    return;
  }
  // 3) Ya conectado: permanece listo para usar client()
}

bool gsmIsConnected() {
  return _connected;
}

TinyGsmClient& gsmGetClient() {
  return client;
}
