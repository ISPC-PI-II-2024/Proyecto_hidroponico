#include <Arduino.h>
#include <unity.h>

// Función que vamos a probar (un ejemplo, puedes escribir tus propias funciones de prueba)
int leerSensor(int valor) {
    return valor;
}

// Función de configuración para Unity
void setup() {
    Serial.begin(9600);
    UNITY_BEGIN(); // Inicia el framework de pruebas
}

// Función de loop para Unity
void loop() {
    // Prueba un valor de sensor
    TEST_ASSERT_EQUAL(400, leerSensor(400));

    UNITY_END();  // Finaliza el framework de pruebas
}
