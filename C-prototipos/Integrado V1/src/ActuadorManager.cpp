#include "ActuadorManager.h"
#include <ArduinoJson.h> // Útil para construir payload MQTT en formato JSON

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
}

void ActuadorManager::loop() {
  // Actualizar patrón del buzzer (sinbloqueante)
  _buzzer.update();

  // Evaluar sensores y tomar acciones sobre actuadores
  evaluarSensores();
}

void ActuadorManager::evaluarSensores() {
  // 1) Verificar que SensorManager ya haya arrancado correctamente
  if (_sensorMgr.getEstado() != SensorManager::Estado::Leyendo) {
    // Si todavía no está listo o hay error, mantendremos buzzer en A_NONE
    return;
  }

  // 2) Obtener los estados de cada nodo JSON como string
  //    Para esto, usamos el JSON que arma SensorManager: obtenerJson()
  //    Luego parseamos solo los campos “estado” de nodos críticos.
  String jsonSensores = _sensorMgr.obtenerJson();      // :contentReference[oaicite:3]{index=3}
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, jsonSensores);
  if (error) {
    // No pudimos parsear: abortar evaluación
    return;
  }

  // 3) Leer cada “estado” (OK o ERR) desde el JSON
  //    Podemos chequear, por ejemplo: “BMP280.estado”, “DHT11.estado”, “HC-SR04.estado”, etc.
  const char* estadoBMP   = doc["BMP280"]["estado"];
  const char* estadoDHT   = doc["DHT11"]["estado"];
  const char* estadoLux   = doc["BH1750"]["estado"];
  const char* estadoDist  = doc["HC-SR04"]["estado"];
  const char* estadoCaud  = doc["Caudal"]["estado"];
  const char* estadoCO2   = doc["CO2"]["estado"];
  const char* estadoEner  = doc["Energia"]["estado"];
  // (Puedes añadir o quitar nodos según tu configuración)

  // 4) Determinar si hay al menos un sensor en estado “ERR”
  bool hayErrorGlobal = false;
  if (strcmp(estadoBMP, "ERR")  == 0) hayErrorGlobal = true;
  if (strcmp(estadoDHT, "ERR")  == 0) hayErrorGlobal = true;
  if (strcmp(estadoLux, "ERR")  == 0) hayErrorGlobal = true;
  if (strcmp(estadoDist, "ERR") == 0) hayErrorGlobal = true;
  if (strcmp(estadoCaud, "ERR") == 0) hayErrorGlobal = true;
  if (strcmp(estadoCO2, "ERR")  == 0) hayErrorGlobal = true;
  if (strcmp(estadoEner, "ERR") == 0) hayErrorGlobal = true;

  // 5) Definir nivel de alarma según gravedad (por ejemplo, cantidad o tipo de error)
  //    Aquí proponemos:
  //      - A_HIGH: si “CO2” o “Energia” están en ERR (considerados críticos)
  //      - A_MEDIUM: si “BMP280” o “DHT11” están en ERR (temperatura/humedad)
  //      - A_LOW: si cualquier otro sensor (luz, distancia, caudal) está en ERR
  AlarmLevel nivelActual = A_NONE;
  if (strcmp(estadoCO2, "ERR") == 0 || strcmp(estadoEner, "ERR") == 0) {
    nivelActual = A_HIGH;
  }
  else if (strcmp(estadoBMP, "ERR") == 0 || strcmp(estadoDHT, "ERR") == 0) {
    nivelActual = A_MEDIUM;
  }
  else if (strcmp(estadoLux, "ERR") == 0 ||
           strcmp(estadoDist, "ERR") == 0 ||
           strcmp(estadoCaud, "ERR") == 0) {
    nivelActual = A_LOW;
  }
  else {
    nivelActual = A_NONE;
  }

  // 6) Ajustar buzzer si cambió el nivel de alarma
  if (nivelActual != _nivelAnterior) {
    // Cambiar patrón de buzzer
    _buzzer.setLevel(nivelActual);
    _nivelAnterior = nivelActual;

    // Notificar cambio de estado solo cuando pase a nivel distinto de A_NONE
    if (nivelActual != A_NONE) {
      // Construir payload JSON simple:
      StaticJsonDocument<256> payload;
      payload["tipo"]   = "alarma";
      payload["nivel"]  = nivelActual;       // 1=LOW, 2=MEDIUM, 3=HIGH
      payload["mensaje"]= hint("%s", (nivelActual==A_HIGH)
                                ? "Alarma CRITICA"
                                : (nivelActual==A_MEDIUM)
                                  ? "Alarma MEDIA"
                                  : "Alarma BAJA");
      // Serializar y enviar por MQTT
      String out;
      serializeJson(payload, out);
      notificarCambioEstado(out.c_str());
      _hayAlertaActiva = true;
    }
    else {
      // Pasó a A_NONE: quitar buzzer y notificar que todo OK
      StaticJsonDocument<128> payload;
      payload["tipo"]    = "alarma";
      payload["nivel"]   = 0;
      payload["mensaje"] = "Sistema OK";
      String out;
      serializeJson(payload, out);
      notificarCambioEstado(out.c_str());
      _hayAlertaActiva = false;
    }
  }

  // 7) Control de bomba según nivel de agua (“HC-SR04”):
  //    - Si distancia > umbralDistMax_ (en SensorManager), consideramos “agua bajo nivel mínimo” → encender bomba.
  //    - Si distancia <= umbralDistMax_, apagar bomba.
  //    El umbral lo maneja internamente SensorManager. Aquí sólo usamos el estado:
  bool distErr = (strcmp(estadoDist, "ERR") == 0);
  if (distErr && !_bombaEncendida) {
    _controlBomba.encenderBomba();
    _bombaEncendida = true;

    // Notificar cambio de bomba
    StaticJsonDocument<128> payload;
    payload["tipo"]    = "bomba";
    payload["estado"]  = "encendida";
    String out; serializeJson(payload, out);
    notificarCambioEstado(out.c_str());
  }
  else if (!distErr && _bombaEncendida) {
    _controlBomba.apagarBomba();
    _bombaEncendida = false;

    // Notificar cambio de bomba
    StaticJsonDocument<128> payload;
    payload["tipo"]    = "bomba";
    payload["estado"]  = "apagada";
    String out; serializeJson(payload, out);
    notificarCambioEstado(out.c_str());
  }

  // 8) Control de LED VERDE/ROJO:
  //    - Si hay nivelActual == A_NONE → encender LED verde, apagar rojo
  //    - Si nivelActual != A_NONE → encender LED rojo, apagar verde
  if (nivelActual == A_NONE) {
      _controlBomba.ledVerdeOn();
      _controlBomba.ledRojoOff();
  } else {
      _controlBomba.ledVerdeOff();
      _controlBomba.ledRojoOn();
  }
}

void ActuadorManager::notificarCambioEstado(const char* mensaje) {
  // Envía el payload por MQTT solamente si CommunicationManager está conectado
  if (_comms.obtenerEstado() == CommunicationManager::State::Conectado) {
    _comms.publicar(_topicAlarmas, mensaje);
  }
}


AlarmLevel ActuadorManager::getNivelAlarma() const {
  return _nivelAnterior;
}

bool ActuadorManager::isBombaEncendida() const {
  return _bombaEncendida;
}