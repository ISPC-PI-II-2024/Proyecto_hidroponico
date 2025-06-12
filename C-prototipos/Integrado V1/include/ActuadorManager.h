#ifndef ACTUADOR_MANAGER_H
#define ACTUADOR_MANAGER_H

#include <Arduino.h>
#include "SensorManager.h"
#include "CommunicationManager.h"
#include "buzzer.h"
#include "control.h"

// Niveles de alarma definidos en buzzer.h
enum AlarmLevel : uint8_t { A_NONE = 0, A_LOW, A_MEDIUM, A_HIGH };

class ActuadorManager {
public:
  /**
   * @brief                 Constructor de ActuadorManager
   * @param pinBuzzer       : número de pin donde está conectado el buzzer
   * @param sensorMgr       : referencia a la instancia de SensorManager
   * @param comms           : referencia a la instancia de CommunicationManager
   * @param topicAlarmas    : topic MQTT donde se publican mensajes de alarma
   */
  ActuadorManager(uint8_t pinBuzzer,
                  uint8_t pinReleBomba,
                  uint8_t pinLedVerde,
                  uint8_t pinLedRojo,
                  SensorManager& sensorMgr,
                  CommunicationManager& comms,
                  const char* topicAlarmas);
                  


  void comenzar();          // Inicializa pines de actuadores y buzzer. Debe llamarse una sola vez en setup().
  void loop();              // Loop de ejecución de ActuadorManager. Llama internamente a evaluate() en cada ciclo rápido.

  AlarmLevel getNivelAlarma() const;
  bool       isBombaEncendida() const;
   String    getOrigenAlarma() const;

private:
  // Métodos auxiliares
  void evaluarSensores();
  void notificarCambioEstado(const char* mensaje);

  // Referencias a otros managers
  SensorManager& _sensorMgr;
  CommunicationManager& _comms;

  // Buzzer y Bomba
  Buzzer _buzzer;
  ControlBomba _controlBomba;

  // Topic MQTT para notificaciones de alarmas
  const char* _topicAlarmas;

  // Últimos estados que llevamos controlados, para detectar cambios
  bool _hayAlertaActiva;
  AlarmLevel _nivelAnterior;

  // Variables internas para tomar acciones en bomba/LED
  bool _bombaEncendida;

  // Umbrales cargados desde Preferences
  float _umbralDistMax;
  float _umbralTempMax;
  float _umbralHumMin;
  float _umbralPresMin;
  float _umbralLuxMin;
  float _umbralCaudalMin;
  float _umbralCO2Max;
  float _umbralVoltMin;
  float _umbralCorrMin;
  String _origenAlarma;
};

#endif 
