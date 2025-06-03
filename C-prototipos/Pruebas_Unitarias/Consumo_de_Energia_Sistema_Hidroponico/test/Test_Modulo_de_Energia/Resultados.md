# Informe de Pruebas Unitarias: Módulo Consumo de Energía

**Fecha:** 2025-06-02
**Entorno:** `esp32_test_energia` (PlatformIO)

## Resumen General

| Total Pruebas | Pasadas ✅ | Falladas ❌ | Ignoradas ➖ |
|---------------|------------|-------------|---------------|
| 18            | 18         | 0           | 0             |

**Estado General: TODAS LAS PRUEBAS PASARON** 🎉

## Detalles de las Pruebas Ejecutadas

| Nombre de la Prueba                           | Estado | Duración (ms) |
|-----------------------------------------------|--------|---------------|
| test_module_initialization_ok                 | PASSED | ~7            |
| test_module_initialization_fail               | PASSED | ~5            |
| test_voltage_reading_nominal                  | PASSED | ~7            |
| test_voltage_reading_scaled                   | PASSED | ~8            |
| test_current_reading_nominal                  | PASSED | ~8            |
| test_current_reading_scaled                   | PASSED | ~7            |
| test_power_calculation_nominal                | PASSED | ~8            |
| test_power_calculation_zero_values            | PASSED | ~10           |
| test_read_failure_voltage_tx                  | PASSED | ~5            |
| test_read_failure_voltage_req                 | PASSED | ~6            |
| test_read_failure_current_tx                  | PASSED | ~7            |
| test_read_failure_current_req                 | PASSED | ~8            |
| test_power_after_read_failure                 | PASSED | ~12           |
| test_low_load_simulation                      | PASSED | ~8            |
| test_medium_load_simulation                   | PASSED | ~9            |
| test_high_load_simulation                     | PASSED | ~11           |
| test_anomalous_low_v_high_i                   | PASSED | ~9            |
| test_anomalous_high_v_low_i                   | PASSED | ~10           |