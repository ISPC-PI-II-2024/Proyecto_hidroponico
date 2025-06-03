#include <unity.h>
#include <Arduino.h> // Para setup/loop si se ejecuta en hardware, y tipos básicos
#include "Wire.h"    // Debe ser nuestro MockWire/Wire.h
#include "Modulo_de_Energia.h"

// Declaraciones de funciones de prueba de test_cases.cpp
void test_module_initialization_ok();           // Prueba inicialización exitosa del módulo
void test_module_initialization_fail();         // Prueba fallo en la inicialización del módulo
void test_voltage_reading_nominal();            // Prueba lectura de voltaje nominal
void test_voltage_reading_scaled();             // Prueba lectura de voltaje escalado
void test_current_reading_nominal();            // Prueba lectura de corriente nominal
void test_current_reading_scaled();             // Prueba lectura de corriente escalada
void test_power_calculation_nominal();          // Prueba cálculo de potencia nominal
void test_power_calculation_zero_values();      // Prueba cálculo de potencia con valores cero
void test_read_failure_voltage_tx();            // Prueba fallo de lectura de voltaje en transmisión
void test_read_failure_voltage_req();           // Prueba fallo de lectura de voltaje en solicitud
void test_read_failure_current_tx();            // Prueba fallo de lectura de corriente en transmisión
void test_read_failure_current_req();           // Prueba fallo de lectura de corriente en solicitud
void test_power_after_read_failure();           // Prueba cálculo de potencia tras fallo de lectura
void test_low_load_simulation();                // Prueba simulación de carga baja
void test_medium_load_simulation();             // Prueba simulación de carga media
void test_high_load_simulation();               // Prueba simulación de carga alta
void test_anomalous_low_v_high_i();             // Prueba caso anómalo: bajo voltaje, alta corriente
void test_anomalous_high_v_low_i();             // Prueba caso anómalo: alto voltaje, baja corriente

// setUp y tearDown son globales para Unity por defecto
void setUp(void);      // Función que se ejecuta antes de cada test
void tearDown(void);   // Función que se ejecuta después de cada test

// Función que ejecuta todas las pruebas
void runTests() {
    UNITY_BEGIN(); // Inicializa el framework de pruebas Unity
    RUN_TEST(test_module_initialization_ok);
    RUN_TEST(test_module_initialization_fail);
    RUN_TEST(test_voltage_reading_nominal);
    RUN_TEST(test_voltage_reading_scaled);
    RUN_TEST(test_current_reading_nominal);
    RUN_TEST(test_current_reading_scaled);
    RUN_TEST(test_power_calculation_nominal);
    RUN_TEST(test_power_calculation_zero_values);
    RUN_TEST(test_read_failure_voltage_tx);
    RUN_TEST(test_read_failure_voltage_req);
    RUN_TEST(test_read_failure_current_tx);
    RUN_TEST(test_read_failure_current_req);
    RUN_TEST(test_power_after_read_failure);
    RUN_TEST(test_low_load_simulation);
    RUN_TEST(test_medium_load_simulation);
    RUN_TEST(test_high_load_simulation);
    RUN_TEST(test_anomalous_low_v_high_i);
    RUN_TEST(test_anomalous_high_v_low_i);
    UNITY_END(); // Finaliza la ejecución de pruebas
}

#ifdef ARDUINO
void setup() {
    delay(2000); // Retardo para que el monitor serial se conecte si es necesario
    runTests();  // Ejecuta las pruebas al iniciar
}

void loop() {
    // No hacer nada aquí
}
#else
// Para ejecución nativa (si está configurada)
int main(int argc, char **argv) {
    runTests();  // Ejecuta las pruebas en entorno de PC
    return 0;
}
#endif