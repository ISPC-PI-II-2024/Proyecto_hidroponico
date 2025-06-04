#ifndef MODULO_DE_ENERGIA_H
#define MODULO_DE_ENERGIA_H

#include <Arduino.h> // Incluye los tipos estándar de Arduino y Wire.h
// Si Wire.h no es incluido por Arduino.h en tu toolchain de prueba, inclúyelo explícitamente.
// #include <Wire.h> // Será el mock en el entorno de prueba

// Suposiciones de Hardware y Protocolo
#define I2C_ADDRESS_SENSOR 0x40           // Dirección I2C del sensor de energía
#define VOLTAGE_REGISTER 0x02             // Registro donde se lee el voltaje
#define CURRENT_REGISTER 0x04             // Registro donde se lee la corriente
#define VOLTAGE_SENSOR_LSB_MV 1.25f       // 1 LSB equivale a 1.25 mV
#define CURRENT_SENSOR_LSB_MA 0.1f        // 1 LSB equivale a 0.1 mA

// Clase que representa el módulo de energía
class Modulo_de_Energia {
public:
    Modulo_de_Energia(TwoWire &wire); // Constructor con inyección de dependencia para TwoWire
    void begin();                // Inicializa la comunicación con el sensor
    float getVoltage();          // Retorna el voltaje en Volts
    float getCurrent();          // Retorna la corriente en Amperes
    float getPower();            // Retorna la potencia en Watts
    bool isCommunicationOK() const; // Indica si la comunicación I2C es correcta

private:
    TwoWire* _wire;                  // Puntero al bus I2C utilizado
    uint16_t readRawValue(uint8_t reg); // Lee un valor crudo de un registro específico
    float convertRawVoltageToVolts(uint16_t rawValue); // Convierte el valor crudo de voltaje a Volts
    float convertRawCurrentToAmps(uint16_t rawValue);  // Convierte el valor crudo de corriente a Amperes
    
    float _lastVoltage;              // Último voltaje leído
    float _lastCurrent;              // Última corriente leída
    bool _communicationOK;           // Estado de la comunicación I2C
};

#endif