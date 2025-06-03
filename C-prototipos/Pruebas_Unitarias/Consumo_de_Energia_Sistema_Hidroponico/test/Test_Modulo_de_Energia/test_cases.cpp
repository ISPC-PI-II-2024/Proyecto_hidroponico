#include <unity.h>
#include "Wire.h"         // El mock Wire.h
#include "Modulo_de_Energia.h" // El módulo bajo prueba

Modulo_de_Energia* Modulo_de_Energia = nullptr;

// Constantes para simulación de carga (definidas también en el prompt)
const float V_NOMINAL_SYSTEM = 12.0f;
const float I_LOW_LOAD = 0.1f;    // 100 mA
const float I_MEDIUM_LOAD = 0.5f; // 500 mA
const float I_HIGH_LOAD = 2.0f;   // 2 A
const float V_LOW_ANOMALOUS = 5.0f;
const float V_HIGH_ANOMALOUS = 15.0f;


// Función auxiliar para convertir Volts a valor crudo del ADC
uint16_t voltageToRaw(float volts) {
    if (volts < 0) return 0; // No se pueden representar voltajes negativos
    return (uint16_t)((volts * 1000.0f) / VOLTAGE_SENSOR_LSB_MV);
}

// Función auxiliar para convertir Amperes a valor crudo del ADC
uint16_t currentToRaw(float amps) {
    if (amps < 0) return 0; // No se pueden representar corrientes negativas
    return (uint16_t)((amps * 1000.0f) / CURRENT_SENSOR_LSB_MA);
}

// Función auxiliar para simular la respuesta I2C del sensor
void simulateSensorResponse(uint16_t rawValue) {
    uint8_t msb = (rawValue >> 8) & 0xFF;
    uint8_t lsb = rawValue & 0xFF;
    uint8_t data[2] = {msb, lsb};
    Wire.setRequestFromData(data, 2);
}

// Función que se ejecuta antes de cada test
void setUp(void) {
    Wire.resetMock(); // Reinicia el estado del mock Wire
    // Modulo_de_Energia usa la instancia global 'Wire' que es nuestro mock
    Modulo_de_Energia = new Modulo_de_Energia(Wire); 
}

// Función que se ejecuta después de cada test
void tearDown(void) {
    delete Modulo_de_Energia;
    Modulo_de_Energia = nullptr;
}

// --- Casos de Prueba ---

// Prueba de inicialización exitosa del módulo
void test_module_initialization_ok() {
    Wire.setEndTransmissionStatus(0); // Simula ACK del sensor en begin()
    // Simula lecturas iniciales exitosas durante begin()
    simulateSensorResponse(voltageToRaw(0.0f)); // Para getVoltage() en begin()
    simulateSensorResponse(currentToRaw(0.0f)); // Para getCurrent() en begin()

    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    // Verifica que se intentó comunicar con la dirección correcta durante el endTransmission de begin()
    TEST_ASSERT_EQUAL_UINT8(I2C_ADDRESS_SENSOR, Wire.lastAddress);
}

// Prueba de inicialización fallida del módulo (NACK)
void test_module_initialization_fail() {
    Wire.setEndTransmissionStatus(1); // Simula NACK del sensor en begin()
    Modulo_de_Energia->begin();
    TEST_ASSERT_FALSE(Modulo_de_Energia->isCommunicationOK());
}

// Prueba de lectura de voltaje nominal
void test_voltage_reading_nominal() {
    Wire.setEndTransmissionStatus(0); // Para begin() y para la lectura
    simulateSensorResponse(voltageToRaw(0.0f)); // Lecturas iniciales en begin()
    simulateSensorResponse(currentToRaw(0.0f));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    float testVoltage = 12.0f;
    simulateSensorResponse(voltageToRaw(testVoltage));
    
    float voltage = Modulo_de_Energia->getVoltage();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(testVoltage, voltage);
    TEST_ASSERT_EQUAL_UINT8(VOLTAGE_REGISTER, Wire.writeBuffer[0]); // Verifica que se solicitó el registro correcto
}

// Prueba de lectura de voltaje escalado
void test_voltage_reading_scaled() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(0.0f)); 
    simulateSensorResponse(currentToRaw(0.0f));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    float testVoltage = 5.625f; // 5625mV / 1.25mV/LSB = 4500 LSB
    simulateSensorResponse(voltageToRaw(testVoltage));
    
    float voltage = Modulo_de_Energia->getVoltage();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(testVoltage, voltage);
}

// Prueba de lectura de corriente nominal
void test_current_reading_nominal() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(0.0f)); 
    simulateSensorResponse(currentToRaw(0.0f));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    float testCurrent = 1.5f; // 1500mA / 0.1mA/LSB = 15000 LSB
    simulateSensorResponse(currentToRaw(testCurrent));
    
    float current = Modulo_de_Energia->getCurrent();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(testCurrent, current);
    TEST_ASSERT_EQUAL_UINT8(CURRENT_REGISTER, Wire.writeBuffer[0]);
}

// Prueba de lectura de corriente escalada
void test_current_reading_scaled() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(0.0f)); 
    simulateSensorResponse(currentToRaw(0.0f));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    float testCurrent = 0.255f; // 255mA / 0.1mA/LSB = 2550 LSB
    simulateSensorResponse(currentToRaw(testCurrent));

    float current = Modulo_de_Energia->getCurrent();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(testCurrent, current);
}

// Prueba de cálculo de potencia nominal
void test_power_calculation_nominal() {
    Wire.setEndTransmissionStatus(0); // Todas las comunicaciones I2C serán exitosas
    // Simula lecturas en begin()
    simulateSensorResponse(voltageToRaw(12.0f)); 
    simulateSensorResponse(currentToRaw(1.0f));
    Modulo_de_Energia->begin(); // Esto llama a getVoltage y getCurrent internamente
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    // _lastVoltage y _lastCurrent ya están poblados por begin()
    TEST_ASSERT_EQUAL_FLOAT(12.0f * 1.0f, Modulo_de_Energia->getPower());
}

// Prueba de cálculo de potencia con valores cero
void test_power_calculation_zero_values() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(12.0f)); 
    simulateSensorResponse(currentToRaw(0.0f)); // Corriente cero
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(0.0f, Modulo_de_Energia->getPower());

    Wire.resetMock();
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(0.0f)); // Voltaje cero
    simulateSensorResponse(currentToRaw(1.0f));
    Modulo_de_Energia->begin(); // Re-inicializa con nuevos valores mockeados
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(0.0f, Modulo_de_Energia->getPower());
}

// Prueba de fallo en la lectura de voltaje (fallo en endTransmission)
void test_read_failure_voltage_tx() { // Falla en endTransmission al escribir el registro
    Wire.setEndTransmissionStatus(0); // begin() OK
    simulateSensorResponse(voltageToRaw(0.0f)); 
    simulateSensorResponse(currentToRaw(0.0f));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    Wire.setEndTransmissionStatus(1); // Falla el endTransmission después de write(VOLTAGE_REGISTER)
    // No es necesario simular datos de requestFrom ya que no se llegará a ese punto
    
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, Modulo_de_Energia->getVoltage());
    TEST_ASSERT_FALSE(Modulo_de_Energia->isCommunicationOK());
}

// Prueba de fallo en la lectura de voltaje (fallo en requestFrom)
void test_read_failure_voltage_req() { // Falla en requestFrom
    Wire.setEndTransmissionStatus(0); // begin() y beginTransmission/write(reg) OK
    simulateSensorResponse(voltageToRaw(0.0f)); 
    simulateSensorResponse(currentToRaw(0.0f));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    Wire.setBytesToReturnOnRequest(0); // requestFrom devuelve 0 bytes (falla)
    // readBufferInternal se queda vacío o con menos de 2 bytes
    Wire.setRequestFromData(nullptr, 0); 


    TEST_ASSERT_EQUAL_FLOAT(-1.0f, Modulo_de_Energia->getVoltage());
    TEST_ASSERT_FALSE(Modulo_de_Energia->isCommunicationOK());
}

// Prueba de fallo en la lectura de corriente (fallo en endTransmission)
void test_read_failure_current_tx() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(12.0f)); // Lectura de voltaje en begin OK
    simulateSensorResponse(currentToRaw(0.0f));  // Lectura de corriente en begin OK (para que _lastVoltage sea válido)
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    Wire.setEndTransmissionStatus(1); // Falla el endTransmission después de write(CURRENT_REGISTER)
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, Modulo_de_Energia->getCurrent());
    TEST_ASSERT_FALSE(Modulo_de_Energia->isCommunicationOK());
}

// Prueba de fallo en la lectura de corriente (fallo en requestFrom)
void test_read_failure_current_req() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(12.0f));
    simulateSensorResponse(currentToRaw(0.0f));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    
    // Asegurar que la lectura de voltaje previa (si la hubo) fue OK.
    // Para este test, solo importa el fallo en corriente.
    // Si _lastVoltage no es válido, getPower() podría fallar por eso.
    // Actualizamos _lastVoltage a un valor válido primero.
    simulateSensorResponse(voltageToRaw(V_NOMINAL_SYSTEM));
    Modulo_de_Energia->getVoltage(); // Esto debería ser exitoso
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());


    Wire.setBytesToReturnOnRequest(1); // requestFrom devuelve solo 1 byte
    uint8_t partial_data[1] = {0xAA};
    Wire.setRequestFromData(partial_data, 1);


    TEST_ASSERT_EQUAL_FLOAT(-1.0f, Modulo_de_Energia->getCurrent());
    TEST_ASSERT_FALSE(Modulo_de_Energia->isCommunicationOK());
}

// Prueba de cálculo de potencia después de un fallo de lectura
void test_power_after_read_failure() {
    Wire.setEndTransmissionStatus(0); // begin() OK
    simulateSensorResponse(voltageToRaw(0.0f)); 
    simulateSensorResponse(currentToRaw(0.0f));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    // Falla lectura de voltaje
    Wire.setEndTransmissionStatus(1);
    Modulo_de_Energia->getVoltage();
    TEST_ASSERT_FALSE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, Modulo_de_Energia->getPower());

    // Resetea y prueba falla en corriente
    Wire.resetMock();
    Wire.setEndTransmissionStatus(0); // begin() OK
    simulateSensorResponse(voltageToRaw(12.0f)); // Lectura de voltaje en begin OK
    simulateSensorResponse(currentToRaw(0.0f));  // Lectura de corriente en begin OK
    Modulo_de_Energia->begin(); // Llama a getV y getI.
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK()); // Debería ser true si lecturas en begin fueron bien

    // Ahora, simula que getVoltage es exitoso pero getCurrent falla
    Wire.setEndTransmissionStatus(0); // Para la siguiente getVoltage
    simulateSensorResponse(voltageToRaw(V_NOMINAL_SYSTEM));
    Modulo_de_Energia->getVoltage(); // Actualiza _lastVoltage a valor válido
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());

    Wire.setEndTransmissionStatus(1); // Falla la siguiente lectura de corriente
    Modulo_de_Energia->getCurrent();
    TEST_ASSERT_FALSE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, Modulo_de_Energia->getPower());
}

// --- Simulación de Cargas ---

// Prueba de simulación de carga baja
void test_low_load_simulation() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(V_NOMINAL_SYSTEM)); 
    simulateSensorResponse(currentToRaw(I_LOW_LOAD));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(V_NOMINAL_SYSTEM, Modulo_de_Energia->getVoltage());
    TEST_ASSERT_EQUAL_FLOAT(I_LOW_LOAD, Modulo_de_Energia->getCurrent());
    TEST_ASSERT_EQUAL_FLOAT(V_NOMINAL_SYSTEM * I_LOW_LOAD, Modulo_de_Energia->getPower());
}

// Prueba de simulación de carga media
void test_medium_load_simulation() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(V_NOMINAL_SYSTEM)); 
    simulateSensorResponse(currentToRaw(I_MEDIUM_LOAD));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(V_NOMINAL_SYSTEM, Modulo_de_Energia->getVoltage());
    TEST_ASSERT_EQUAL_FLOAT(I_MEDIUM_LOAD, Modulo_de_Energia->getCurrent());
    TEST_ASSERT_EQUAL_FLOAT(V_NOMINAL_SYSTEM * I_MEDIUM_LOAD, Modulo_de_Energia->getPower());
}

// Prueba de simulación de carga alta
void test_high_load_simulation() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(V_NOMINAL_SYSTEM)); 
    simulateSensorResponse(currentToRaw(I_HIGH_LOAD));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(V_NOMINAL_SYSTEM, Modulo_de_Energia->getVoltage());
    TEST_ASSERT_EQUAL_FLOAT(I_HIGH_LOAD, Modulo_de_Energia->getCurrent());
    TEST_ASSERT_EQUAL_FLOAT(V_NOMINAL_SYSTEM * I_HIGH_LOAD, Modulo_de_Energia->getPower());
}

// Prueba de condición anómala: bajo voltaje y alta corriente
void test_anomalous_low_v_high_i() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(V_LOW_ANOMALOUS)); 
    simulateSensorResponse(currentToRaw(I_HIGH_LOAD));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(V_LOW_ANOMALOUS, Modulo_de_Energia->getVoltage());
    TEST_ASSERT_EQUAL_FLOAT(I_HIGH_LOAD, Modulo_de_Energia->getCurrent());
    TEST_ASSERT_EQUAL_FLOAT(V_LOW_ANOMALOUS * I_HIGH_LOAD, Modulo_de_Energia->getPower());
}

// Prueba de condición anómala: alto voltaje y baja corriente
void test_anomalous_high_v_low_i() {
    Wire.setEndTransmissionStatus(0);
    simulateSensorResponse(voltageToRaw(V_HIGH_ANOMALOUS)); 
    simulateSensorResponse(currentToRaw(I_LOW_LOAD));
    Modulo_de_Energia->begin();
    TEST_ASSERT_TRUE(Modulo_de_Energia->isCommunicationOK());
    TEST_ASSERT_EQUAL_FLOAT(V_HIGH_ANOMALOUS, Modulo_de_Energia->getVoltage());
    TEST_ASSERT_EQUAL_FLOAT(I_LOW_LOAD, Modulo_de_Energia->getCurrent());
    TEST_ASSERT_EQUAL_FLOAT(V_HIGH_ANOMALOUS * I_LOW_LOAD, Modulo_de_Energia->getPower());
}