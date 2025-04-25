#include <Arduino.h>
#include <unity.h>

// Repetimos la función a testear (o mejor aún, la importamos si está separada en un .h/.cpp)
int convertirVoltajeACo2ppm(float voltaje) {
  int mV = voltaje * 1000;
  int ppm = map(mV, 400, 3000, 400, 5000);
  return constrain(ppm, 400, 5000);
}

void test_voltaje_bajo_deberia_dar_400ppm() {
  TEST_ASSERT_EQUAL(400, convertirVoltajeACo2ppm(0.4));
}

void test_voltaje_medio_deberia_dar_aprox_2700ppm() {
  int ppm = convertirVoltajeACo2ppm(1.7); // valor intermedio
  TEST_ASSERT_TRUE(ppm > 2000 && ppm < 3000);
}

void test_voltaje_alto_deberia_dar_5000ppm() {
  TEST_ASSERT_EQUAL(5000, convertirVoltajeACo2ppm(3.3));
}

void test_valores_fuera_de_rango_limite() {
  TEST_ASSERT_EQUAL(400, convertirVoltajeACo2ppm(0.2)); // Muy bajo
  TEST_ASSERT_EQUAL(5000, convertirVoltajeACo2ppm(4.0)); // Muy alto
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_voltaje_bajo_deberia_dar_400ppm);
  RUN_TEST(test_voltaje_medio_deberia_dar_aprox_2700ppm);
  RUN_TEST(test_voltaje_alto_deberia_dar_5000ppm);
  RUN_TEST(test_valores_fuera_de_rango_limite);
  UNITY_END();
}

void loop() {
  // vacío porque Unity corre en setup()
}
