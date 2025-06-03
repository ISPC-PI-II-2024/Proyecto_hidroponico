#ifndef MOCK_ARDUINO_WIRE_H
#define MOCK_ARDUINO_WIRE_H

#include <stdint.h>
#include <stddef.h> 
#include <vector>   
#include <queue>    
#include <string>   

// Declaración mínima de la clase Print si es necesaria (la TwoWire real hereda de Print)
class Print {
public:
    // Método virtual puro para escribir un solo byte
    virtual size_t write(uint8_t) = 0;
    // Método virtual puro para escribir un buffer de bytes
    virtual size_t write(const uint8_t *buffer, size_t size) = 0;
    // Puedes añadir más stubs de Print si son utilizados a través de la instancia Wire
};

// Clase mock que simula el comportamiento de TwoWire (Wire)
class TwoWire : public Print { 
public:
    // Miembros para rastrear el estado y controlar el mock
    uint8_t lastAddress; // Última dirección utilizada
    std::vector<uint8_t> writeBuffer; // Buffer para almacenar los datos escritos
    std::queue<uint8_t> readBufferInternal; // Buffer interno para los datos que read() debe devolver
    uint8_t lastTransmitStatus; // Último estado de transmisión
    size_t bytesToReturnOnRequest; // Cuántos bytes debe simular requestFrom que leyó
    bool transmissionActive; // Indica si hay una transmisión activa

    TwoWire(); // Constructor

    // Métodos estándar de la API de Wire
    void begin(); // Inicializa el bus I2C en modo maestro
    void begin(uint8_t address); // Inicializa el bus I2C en modo esclavo
    void beginTransmission(uint8_t address); // Inicia una transmisión a una dirección específica
    uint8_t endTransmission(bool stop); // Finaliza la transmisión, con opción de enviar condición de parada
    uint8_t endTransmission(void); // Finaliza la transmisión (por defecto con stop)
    
    // Métodos para escribir datos (sobrecargas)
    virtual size_t write(uint8_t data) override; // Escribe un byte
    virtual size_t write(const uint8_t *data, size_t quantity) override; // Escribe varios bytes
    
    int available(void); // Devuelve cuántos bytes hay disponibles para leer
    int read(void); // Lee un byte del buffer interno
    uint8_t requestFrom(uint8_t address, size_t quantity, bool stop); // Solicita datos a un dispositivo
    uint8_t requestFrom(uint8_t address, size_t quantity); // Solicita datos (sin especificar stop)
    uint8_t requestFrom(int address, int quantity, int stop); // Sobrecarga común para compatibilidad

    // Métodos de control del Mock (no parte de la API estándar de Wire)
    void resetMock(); // Reinicia el estado del mock
    void setEndTransmissionStatus(uint8_t status); // Define el estado que devolverá endTransmission
    void setRequestFromData(const uint8_t* data, size_t quantity); // Carga datos que devolverá read()
    void setBytesToReturnOnRequest(size_t numBytes); // Define cuántos bytes simulará que están disponibles tras requestFrom
};

// Declaración del objeto global Wire (que será nuestra instancia mock)
extern TwoWire Wire; 

#endif // MOCK_ARDUINO_WIRE_H