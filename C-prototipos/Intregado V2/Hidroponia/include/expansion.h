 #ifndef EXPANSION_H
 #define EXPANSION_H
 
 #include <Arduino.h>
 #include <Wire.h>
 
 // Dirección I2C del PCF8574T (típicamente 0x20-0x27, configurable con pines A0-A2)
 #define PCF8574_ADDR 0x27//direccion
 
 // Definición de pines en el expansor
 #define PIN_PULSADOR1    0   // P0 del PCF8574T
 #define PIN_PULSADOR2    1   // P1 del PCF8574T
 #define PIN_PULSADOR3    2   // P2 del PCF8574T
 #define PIN_SENSOR1      3   // P3 del PCF8574T
 #define PIN_SENSOR2      4   // P4 del PCF8574T
 #define PIN_SENSOR3      5   // P5 del PCF8574T
 #define PIN_RELE         6   // P6 del PCF8574T
 #define PIN_EXTRA        7   // P7 del PCF8574T, reservado para futuro uso
 
 // Estados de los pulsadores (ACTIVO BAJO: 0 = presionado, 1 = no presionado)
 extern bool pulsador1_presionado;
 extern bool pulsador2_presionado;
 extern bool pulsador3_presionado;
 
 // Estados de los sensores
 extern bool sensor1_activado;
 extern bool sensor2_activado;
 extern bool sensor3_activado;
 // Estado del relé
 extern bool rele_activado;
 bool expansion_init(int sda_pin = SDA, int scl_pin = SCL);
 bool expansion_leer_entradas();
 bool expansion_activar_rele();
 bool expansion_desactivar_rele();
 bool expansion_set_rele(bool estado);
 bool expansion_set_pin(uint8_t pin, bool estado);
 bool expansion_leer_pin(uint8_t pin);
  void expansion_actualizar();
 bool expansion_diagnostico();
 
 #endif // EXPANSION_H