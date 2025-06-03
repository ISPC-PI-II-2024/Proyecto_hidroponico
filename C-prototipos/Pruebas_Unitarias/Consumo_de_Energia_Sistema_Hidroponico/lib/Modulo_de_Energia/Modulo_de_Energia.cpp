#include "Modulo_de_Energia.h"

// Constructor: inicializa los miembros privados
Modulo_de_Energia::Modulo_de_Energia(TwoWire &wire) 
    : _wire(&wire), _lastVoltage(0.0f), _lastCurrent(0.0f), _communicationOK(false) {}

// Inicializa el módulo de energía y verifica la comunicación con el sensor
void Modulo_de_Energia::begin() {
    _wire->begin(); // Inicializa la comunicación I2C
    // Intenta una comunicación inicial para verificar la conexión con el sensor
    _wire->beginTransmission(I2C_ADDRESS_SENSOR);
    if (_wire->endTransmission() == 0) {
        _communicationOK = true; // Comunicación exitosa
    } else {
        _communicationOK = false; // Fallo en la comunicación
    }
    // Realiza una lectura inicial para poblar _lastVoltage y _lastCurrent
    if (_communicationOK) {
        getVoltage(); // Actualiza _lastVoltage y _communicationOK
        if (_communicationOK) { // Solo lee corriente si el voltaje fue exitoso
             getCurrent(); // Actualiza _lastCurrent y _communicationOK
        }
    } else {
        _lastVoltage = -1.0f; // Indica error en la lectura de voltaje
        _lastCurrent = -1.0f; // Indica error en la lectura de corriente
    }
}

// Devuelve el estado de la comunicación I2C
bool Modulo_de_Energia::isCommunicationOK() const {
    return _communicationOK;
}

// Lee un valor crudo (raw) de un registro específico del sensor
uint16_t Modulo_de_Energia::readRawValue(uint8_t reg) {
    if (!_communicationOK) { 
        // Si la comunicación ya falló, retorna un valor de error inmediatamente.
        return 0;
    }

    _wire->beginTransmission(I2C_ADDRESS_SENSOR); // Inicia transmisión I2C
    _wire->write(reg); // Escribe el registro a leer
    uint8_t txStatus = _wire->endTransmission(false); // false para enviar un restart y mantener la conexión

    if (txStatus != 0) {
        _communicationOK = false; // Error en la transmisión o el sensor no hizo ACK
        return 0; 
    }

    // Solicita 2 bytes del sensor
    uint8_t bytesReceived = _wire->requestFrom((uint8_t)I2C_ADDRESS_SENSOR, (uint8_t)2, (uint8_t)true); // true para enviar stop

    if (bytesReceived != 2) {
        _communicationOK = false; // No se recibieron los 2 bytes esperados
        return 0;
    }

    // Leer los dos bytes (MSB primero)
    uint16_t value = ((uint16_t)_wire->read() << 8) | _wire->read();
    _communicationOK = true; // Si llegamos aquí, la comunicación fue exitosa para esta lectura
    return value;
}

// Convierte el valor crudo de voltaje a voltios
float Modulo_de_Energia::convertRawVoltageToVolts(uint16_t rawValue) {
    return (rawValue * VOLTAGE_SENSOR_LSB_MV) / 1000.0f; // Convertir mV a V
}

// Convierte el valor crudo de corriente a amperios
float Modulo_de_Energia::convertRawCurrentToAmps(uint16_t rawValue) {
    return (rawValue * CURRENT_SENSOR_LSB_MA) / 1000.0f; // Convertir mA a A
}

// Obtiene el voltaje actual del sensor
float Modulo_de_Energia::getVoltage() {
    uint16_t rawVoltage = readRawValue(VOLTAGE_REGISTER);
    if (!_communicationOK) { // readRawValue actualiza _communicationOK
         _lastVoltage = -1.0f; // Indicar error
         return _lastVoltage;
    }
    _lastVoltage = convertRawVoltageToVolts(rawVoltage);
    return _lastVoltage;
}

// Obtiene la corriente actual del sensor
float Modulo_de_Energia::getCurrent() {
    uint16_t rawCurrent = readRawValue(CURRENT_REGISTER);
    if (!_communicationOK) { // readRawValue actualiza _communicationOK
        _lastCurrent = -1.0f; // Indicar error
        return _lastCurrent;
    }
    _lastCurrent = convertRawCurrentToAmps(rawCurrent);
    return _lastCurrent;
}

// Calcula la potencia usando las últimas lecturas válidas de voltaje y corriente
float Modulo_de_Energia::getPower() {
    // Utiliza las últimas lecturas válidas de voltaje y corriente
    // Se asume que getVoltage() y getCurrent() fueron llamadas recientemente
    // o que el usuario entiende que esta es la potencia basada en las últimas lecturas exitosas.
    if (!_communicationOK || _lastVoltage < 0.0f || _lastCurrent < 0.0f) {
        // Si hubo un error en la última lectura de V o I, o la comunicación general falló.
        return -1.0f; // Indicar error
    }
    return _lastVoltage * _lastCurrent;
}