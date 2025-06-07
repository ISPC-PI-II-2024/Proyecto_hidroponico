#include "ActuadorManager.h"
#include <ArduinoJson.h>
#include <Preferences.h>

ActuadorManager::ActuadorManager(uint8_t pinBuzzer,
                                 uint8_t pinReleBomba,
                                 uint8_t pinLedVerde,
                                 uint8_t pinLedRojo,
                                 SensorManager& sensorMgr,
                                 CommunicationManager& comms,
                                 const char* topicAlarmas)
  : _sensorMgr(sensorMgr),
    _comms(comms),
    _buzzer(pinBuzzer),
    _controlBomba(pinReleBomba, pinLedVerde, pinLedRojo),
    _topicAlarmas(topicAlarmas),
    _hayAlertaActiva(false),
    _nivelAnterior(A_NONE),
    _bombaEncendida(false)
{}

void ActuadorManager::comenzar() {
  _controlBomba.setup();       // Inicializa pines de control de bomba y LEDs
  _buzzer.begin();             // Configura el buzzer en el pin indicado
  _buzzer.setLevel(A_NONE);    // Inicia el buzzer sin alarma
  _hayAlertaActiva = false ;   // No hay alerta activa al inicio
  _nivelAnterior   = A_NONE;   // Nivel de alarma anterior es A_NONE
  _controlBomba.apagarBomba(); // Aseguramos que la bomba esté apagada al inicio
  _bombaEncendida = false;     // Estado de bomba apagada al inicio



  // Cargar umbrales desde NVS (Preferences)
  Preferences prefs;
  prefs.begin("umbrales", true);  // true = solo lectura

  _umbralDistMax   = prefs.getFloat("umbralDistMax",    30.0);
  _umbralTempMax   = prefs.getFloat("umbralTempMax",    35.0);
  _umbralHumMin    = prefs.getFloat("umbralHumMin",     30.0);
  _umbralPresMin   = prefs.getFloat("umbralPresionMin", 950.0);
  _umbralLuxMin    = prefs.getFloat("umbralLuxMin",     200.0);
  _umbralCaudalMin = prefs.getFloat("umbralCaudalMin",  1.0);
  _umbralCO2Max    = prefs.getFloat("umbralCO2Max",     1000.0);
  _umbralVoltMin   = prefs.getFloat("umbralVoltMin",    3.0);
  _umbralCorrMin   = prefs.getFloat("umbralCorrMin",    0.05);

  prefs.end();
}

void ActuadorManager::loop() {
  _buzzer.update();       // Actualizar patrón del buzzer (sinbloqueante)
  evaluarSensores();      // Evaluar sensores y tomar acciones sobre actuadores
}

void ActuadorManager::evaluarSensores() {
  if (_sensorMgr.getEstado() != SensorManager::Estado::Leyendo) { // 1) Verificar que SensorManager ya haya arrancado correctamente
    return;                                                       // Si todavía no está listo o hay error, mantendremos buzzer en A_NONE
  }

  // 2) Obtener los estados de cada nodo JSON como string
  String jsonSensores = _sensorMgr.obtenerJson();                      // Para esto, usamos el JSON que arma SensorManager: obtenerJson()
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, jsonSensores);     // Luego parseamos solo los campos “estado” de nodos críticos.
  if (error) return;
  
  // Detectar error en sensores (Se informan con el LED, no con buzzer)
  bool errorDetectado = false;
  const char* sensores[] = { "BMP280", "DHT11", "BH1750", "HC-SR04", "Caudal", "CO2", "Energia" };
  for (const char* s : sensores) {
    if (strcmp(doc[s]["estado"], "ERR") == 0) {
      errorDetectado = true;
      break;
    }
  }

  if (errorDetectado) {
    _controlBomba.ledRojoOn();
    _controlBomba.ledVerdeOff();
    _buzzer.setLevel(A_NONE);  // Silencio
    return;
  }// No pudimos parsear: abortar evaluación

    // -------------------------------
  // Evaluar distValor para bomba
  float distValor = doc["HC-SR04"]["valor"];
  if (distValor > _umbralDistMax && !_bombaEncendida) {
    _controlBomba.encenderBomba();
    _bombaEncendida = true;
  } else if (distValor <= _umbralDistMax && _bombaEncendida) {
    _controlBomba.apagarBomba();
    _bombaEncendida = false;
  }

  // -------------------------------
  // Evaluar valores para activar buzzer por umbrales
  float temp = doc["DHT11"]["valor"];
  float hum  = doc["DHT11"]["humedad"];
  float pres = doc["BMP280"]["valor"];
  float lux  = doc["BH1750"]["valor"];
  float caud = doc["Caudal"]["valor"];
  float co2  = doc["CO2"]["valor"];
  float volt = doc["Energia"]["voltaje"];
  float corr = doc["Energia"]["corriente"];

  // -------------------------------
  // Evaluar alarmas según umbrales definidos
    bool alarmaAlta =
    (co2 > _umbralCO2Max || volt < _umbralVoltMin || corr < _umbralCorrMin);
  bool alarmaMedia =
    (temp > _umbralTempMax || hum < _umbralHumMin);
  bool alarmaBaja =
    (pres < _umbralPresMin || lux < _umbralLuxMin || caud < _umbralCaudalMin);

  AlarmLevel nivelActual = A_NONE;
  if (alarmaAlta) nivelActual = A_HIGH;
  else if (alarmaMedia) nivelActual = A_MEDIUM;
  else if (alarmaBaja) nivelActual = A_LOW;

  if (nivelActual != _nivelAnterior) {
    _buzzer.setLevel(nivelActual);
    _nivelAnterior = nivelActual;
    _hayAlertaActiva = (nivelActual != A_NONE);
  }

  // -------------------------------
  // LEDS
  if (nivelActual == A_NONE) {
    _controlBomba.ledVerdeOn();
    _controlBomba.ledRojoOff();
  } else {
    _controlBomba.ledVerdeOff();
    _controlBomba.ledRojoOn();
  }
}

AlarmLevel ActuadorManager::getNivelAlarma() const {
  return _nivelAnterior;
}

bool ActuadorManager::isBombaEncendida() const {
  return _bombaEncendida;
}
