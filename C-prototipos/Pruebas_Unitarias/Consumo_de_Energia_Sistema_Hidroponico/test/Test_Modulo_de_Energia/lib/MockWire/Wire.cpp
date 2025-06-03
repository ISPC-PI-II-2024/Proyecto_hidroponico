#include "Wire.h" 

// Definición de la instancia global mock Wire
TwoWire Wire; 

// Constructor de la clase TwoWire
TwoWire::TwoWire() {
    resetMock(); // Inicializa el mock al crearse
}

// Reinicia el estado interno del mock
void TwoWire::resetMock() {
    lastAddress = 0;
    writeBuffer.clear();
    while(!readBufferInternal.empty()) readBufferInternal.pop();
    lastTransmitStatus = 0; // Éxito por defecto
    bytesToReturnOnRequest = 0;
    transmissionActive = false;
}

// Inicializa el mock (no hace nada en este caso)
void TwoWire::begin() {
    // No hace nada en el mock, o podría establecer un estado si es necesario
}

// Inicializa el mock con dirección (no implementado en detalle)
void TwoWire::begin(uint8_t address) {
    (void)address; // No implementado en detalle para el mock de maestro
}

// Inicia una transmisión simulada al dispositivo con la dirección dada
void TwoWire::beginTransmission(uint8_t address) {
    lastAddress = address;
    writeBuffer.clear();
    transmissionActive = true;
}

// Finaliza la transmisión simulada, retorna el estado de la transmisión
uint8_t TwoWire::endTransmission(bool stop) {
    (void)stop; 
    transmissionActive = false;
    return lastTransmitStatus;
}

// Sobrecarga de endTransmission sin argumentos
uint8_t TwoWire::endTransmission(void) {
    return endTransmission(true);
}

// Escribe un byte en el buffer de escritura si la transmisión está activa
size_t TwoWire::write(uint8_t data) {
    if (transmissionActive) {
        writeBuffer.push_back(data);
        return 1;
    }
    return 0; 
}

// Escribe varios bytes en el buffer de escritura si la transmisión está activa
size_t TwoWire::write(const uint8_t *data, size_t quantity) {
    if (transmissionActive) {
        for (size_t i = 0; i < quantity; ++i) {
            writeBuffer.push_back(data[i]);
        }
        return quantity;
    }
    return 0;
}

// Devuelve la cantidad de datos disponibles para leer
int TwoWire::available(void) {
    return readBufferInternal.size();
}

// Lee un byte del buffer de lectura, o -1 si está vacío
int TwoWire::read(void) {
    if (readBufferInternal.empty()) {
        return -1; // Comportamiento estándar si no hay datos
    }
    uint8_t val = readBufferInternal.front();
    readBufferInternal.pop();
    return val;
}

// Simula la lectura de datos del esclavo y los pone en readBufferInternal
uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stop) {
    (void)address; 
    (void)stop;    
    // El número de bytes devueltos es el mínimo entre quantity y bytesToReturnOnRequest
    // Los datos ya deberían haber sido puestos en readBufferInternal por setRequestFromData
    size_t actualBytesProvided = readBufferInternal.size();
    return (actualBytesProvided < quantity) ? actualBytesProvided : quantity;
}

// Sobrecarga de requestFrom para compatibilidad
uint8_t TwoWire::requestFrom(int address, int quantity, int stop) {
    return requestFrom((uint8_t)address, (size_t)quantity, (bool)stop);
}

// Sobrecarga de requestFrom con stop por defecto en true
uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity) {
    return requestFrom(address, quantity, true);
}


// --- Métodos de Control del Mock ---

// Permite establecer el estado de la transmisión simulada
void TwoWire::setEndTransmissionStatus(uint8_t status) {
    lastTransmitStatus = status;
}

// Prepara los datos que el mock devolverá en las llamadas a read()
void TwoWire::setRequestFromData(const uint8_t* data, size_t quantity) {
    while(!readBufferInternal.empty()) readBufferInternal.pop(); // Limpiar buffer anterior
    for (size_t i = 0; i < quantity; ++i) {
        readBufferInternal.push(data[i]);
    }
    bytesToReturnOnRequest = quantity; // Guarda cuántos bytes se "recibieron"
}

// Permite establecer cuántos bytes se deben devolver en requestFrom
void TwoWire::setBytesToReturnOnRequest(size_t numBytes) {
    bytesToReturnOnRequest = numBytes;
    // Nota: esto no puebla readBufferInternal, setRequestFromData lo hace.
    // Esto es para controlar el valor de retorno de requestFrom si es diferente
    // de la cantidad de datos realmente preparados. Para este mock,
    // asumimos que setRequestFromData es la fuente de verdad.
}