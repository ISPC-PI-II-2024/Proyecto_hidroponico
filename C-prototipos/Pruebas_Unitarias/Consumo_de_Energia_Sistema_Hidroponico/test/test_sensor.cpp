#include <Arduino.h>
#include <unity.h>

// Declaración de variables globales
float corriente = 0.0;  // Variable para almacenar el valor de la corriente en amperios
float voltaje = 0.0;    // Variable para almacenar el valor del voltaje en voltios
float R = 10000.0;      // Resistencia en ohmios utilizada para calcular la corriente
const int sensorPin = 36; // Pin analógico donde está conectado el sensor

// Función para inicializar el sensor
bool inicializarSensor() {
    pinMode(sensorPin, INPUT);
    return true; // Retorna true si la inicialización es exitosa
}

// Prueba para verificar la inicialización del sensor
void test_inicializacion_sensor() {
    TEST_ASSERT_TRUE(inicializarSensor());
}

// Prueba para verificar la adquisición de datos del sensor
void test_adquisicion_datos() {
    int sensorValue = analogRead(sensorPin);
    corriente = sensorValue * (3.3 / 4095.0) / R;
    voltaje = sensorValue * (3.3 / 4095.0);

    // Verifica que los valores estén dentro de un rango esperado
    TEST_ASSERT_GREATER_OR_EQUAL(0.0, corriente);
    TEST_ASSERT_GREATER_OR_EQUAL(0.0, voltaje);
}

// Prueba para detectar la ausencia del sensor
void test_sensor_no_presente() {
    int lectura = analogRead(sensorPin);
    if (lectura == 0) {
        Serial.println("Falla: Sensor no detectado.");
        TEST_FAIL_MESSAGE("Sensor no detectado.");
    } else {
        Serial.println("Sensor detectado correctamente.");
    }
}

void setup() {
    // Inicializa el monitor serial
    Serial.begin(115200);
    delay(2000); // Espera para que el monitor serial esté listo

    // Inicializa el framework de pruebas unitarias
    UNITY_BEGIN();

    // Ejecuta las pruebas
    RUN_TEST(test_inicializacion_sensor);
    RUN_TEST(test_adquisicion_datos);
    RUN_TEST(test_sensor_no_presente);

    // Finaliza las pruebas
    UNITY_END();
}

void loop() {
    // El loop se deja vacío ya que las pruebas se ejecutan en el setup()
}
