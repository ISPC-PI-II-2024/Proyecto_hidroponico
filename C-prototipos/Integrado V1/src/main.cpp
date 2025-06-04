// main.cpp

#include <Arduino.h>
#include "CommunicationManager.h"

// Instanciamos CommunicationManager con:
//  • consoleSerial  = Serial       (para logs)
//  • modemSerial    = Serial1      (para el módem GSM; aunque no se use en test Wi-Fi basta con declarar)
//  • wifiSerial     = Serial       (para debug de Wi-Fi)
//  • SSID/Pass Wi-Fi     = "Vitto", "vittorio10"
//  • Broker MQTT         = "test.mosquitto.org", puerto 1883
//  • Topic MQTT          = "prueba"
//  • Usuario/Pass MQTT   = nullptr (sin credenciales)
CommunicationManager comm(
  Serial,      // consoleSerial
  Serial1,     // modemSerial
  Serial,      // wifiSerial (para WiFiCtrl)
  "Vitto",     // SSID
  "vittorio10",// Pass
  "test.mosquitto.org", // MQTT broker
  1883,        // MQTT puerto
  "prueba",    // MQTT tópico
  nullptr,     // MQTT user
  nullptr      // MQTT pass
);

bool mensajeEnviado = false;

void setup() {
  Serial.begin(115200);
  // Inicializamos Serial1 para el módem GSM (aunque no se use en este test)
  Serial1.begin(115200);

  // Arrancamos el ciclo de conexión (Wi-Fi → MQTT)
  comm.begin();
}

void loop() {
  // Mantenemos la FSM de conexión
  comm.loop();

  // Cuando ya estemos CONECTADOS, enviamos "probando" una sola vez
  if (!mensajeEnviado &&
      comm.obtenerEstado() == CommunicationManager::State::Conectado) {
    comm.publicar("prueba", "probando");
    Serial.println("[Main] Mensaje 'probando' enviado al tópico 'prueba'.");
    mensajeEnviado = true;
  }
}
