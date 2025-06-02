#include <Arduino.h>
#include <unity.h>
#include "Caudalimetro.h"

void test_caudal_10_pulsos_10_segundos() {
  float resultado = calcularCaudal(10, 7.5, 10.0);
  TEST_ASSERT_FLOAT_WITHIN(0.01, 1.333, resultado);
}

void test_volumen_10_pulsos() {
  float resultado = calcularVolumen(10, 7.5);
  TEST_ASSERT_FLOAT_WITHIN(0.01, 1.333, resultado);
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_caudal_10_pulsos_10_segundos);
  RUN_TEST(test_volumen_10_pulsos);
  UNITY_END();
}

void loop() {}
