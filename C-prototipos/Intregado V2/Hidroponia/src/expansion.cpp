

 #include "expansion.h"

 // Variables externas para acceso desde otras rutinas
 bool pulsador1_presionado = false;
 bool pulsador2_presionado = false;
 bool pulsador3_presionado = false;
 bool sensor1_activado = false;
 bool sensor2_activado = false;
 bool sensor3_activado = false;
 bool rele_activado = false;
 
 // Variables privadas
 static uint8_t _estado_actual = 0xFF; // Estado actual de los pines (inicialmente todos HIGH)
 static bool _inicializado = false;
 
 
 static bool _escribir_estado() {
     Wire.beginTransmission((uint8_t)PCF8574_ADDR);
     Wire.write(_estado_actual);
     uint8_t resultado = Wire.endTransmission();
     return (resultado == 0);
 }
 

 static bool _leer_estado() {
     // Evitar ambigüedad especificando tipos exactos para requestFrom
     uint8_t bytes_leidos = Wire.requestFrom((uint8_t)PCF8574_ADDR, (uint8_t)1);
     if (bytes_leidos != 1) {
         return false;
     }
     
     if (Wire.available()) {
         _estado_actual = Wire.read();
         return true;
     }
     
     return false;
 }
 
 bool expansion_init(int sda_pin, int scl_pin) {
     // Inicializar I2C con pines personalizados si se especifican
     if (sda_pin != SDA || scl_pin != SCL) {
         Wire.begin(sda_pin, scl_pin);
     } else {
         Wire.begin();
     }
     
     // Verificar comunicación con el dispositivo
     Wire.beginTransmission((uint8_t)PCF8574_ADDR);
     uint8_t resultado = Wire.endTransmission();
     
     if (resultado != 0) {
         Serial.println("Error: No se pudo inicializar el expansor PCF8574T");
         return false;
     }
     
     // Configuración inicial: todos los pines en HIGH (1)
     // En PCF8574, escribir 1 a un pin lo configura como entrada con pull-up
     // o salida en HIGH, por lo que esta configuración es segura para iniciar
     _estado_actual = 0xFF;
     
     // Activamos todos los pines como entradas con pull-up
     if (!_escribir_estado()) {
         Serial.println("Error: No se pudo escribir la configuración inicial");
         return false;
     }
     
     // Configuración específica para el relé (inicialmente desactivado)
     // Para relés optoacopladores, normalmente LOW activa el relé
     expansion_desactivar_rele();
     
     _inicializado = true;
     Serial.println("Expansor PCF8574T inicializado correctamente");
     
     // Lectura inicial para actualizar estados
     expansion_leer_entradas();
     
     return true;
 }
 
 bool expansion_leer_entradas() {
     if (!_inicializado) return false;
     
     // Leer estado actual del expansor
     if (!_leer_estado()) {
         return false;
     }
     
     // Actualizar estados de pulsadores (activo bajo, invertimos la lógica)
     pulsador1_presionado = !bitRead(_estado_actual, PIN_PULSADOR1);
     pulsador2_presionado = !bitRead(_estado_actual, PIN_PULSADOR2);
     pulsador3_presionado = !bitRead(_estado_actual, PIN_PULSADOR3);
     
     // Actualizar estados de sensores
     sensor1_activado = !bitRead(_estado_actual, PIN_SENSOR1);
     sensor2_activado = !bitRead(_estado_actual, PIN_SENSOR2);
     sensor3_activado = !bitRead(_estado_actual, PIN_SENSOR3);
     
     // El estado del relé se conserva desde la última escritura
     
     return true;
 }
 
 bool expansion_activar_rele() {
     return expansion_set_rele(true);
 }
 
 bool expansion_desactivar_rele() {
     return expansion_set_rele(false);
 }
 
 bool expansion_set_rele(bool estado) {
     if (!_inicializado) return false;
     
     // La lógica puede ser inversa dependiendo del tipo de relé
     // Para un relé optoacoplador activado por LOW:
     // true (activar) = poner pin en LOW (0)
     // false (desactivar) = poner pin en HIGH (1)
     
     if (estado) {
         bitClear(_estado_actual, PIN_RELE); // Activar relé (LOW)
     } else {
         bitSet(_estado_actual, PIN_RELE); // Desactivar relé (HIGH)
     }
     
     if (_escribir_estado()) {
         rele_activado = estado;
         return true;
     }
     
     return false;
 }
 
 bool expansion_set_pin(uint8_t pin, bool estado) {
     if (!_inicializado || pin > 7) return false;
     
     if (estado) {
         bitSet(_estado_actual, pin);
     } else {
         bitClear(_estado_actual, pin);
     }
     
     return _escribir_estado();
 }
 
 bool expansion_leer_pin(uint8_t pin) {
     if (!_inicializado || pin > 7) return true; // Valor por defecto en caso de error
     
     if (!_leer_estado()) {
         return true; // Valor por defecto en caso de error
     }
     
     return bitRead(_estado_actual, pin);
 }
 
 void expansion_actualizar() {
     if (!_inicializado) return;
     
     // Leer el estado actual de las entradas
     expansion_leer_entradas();
     
     // Aquí puedes añadir lógica adicional para el debounce de los pulsadores
     // o filtrado de señales de los sensores si es necesario
 }
 
 bool expansion_diagnostico() {
     if (!_inicializado) {
         expansion_init(); // Intentar inicializar si no se ha hecho
     }
     
     // Verificar comunicación con el dispositivo
     Wire.beginTransmission((uint8_t)PCF8574_ADDR);
     uint8_t resultado = Wire.endTransmission();
     
     if (resultado != 0) {
         Serial.println("Diagnóstico: Error de comunicación con PCF8574T");
         return false;
     }
     
     // Prueba de lectura
     if (!_leer_estado()) {
         Serial.println("Diagnóstico: Error al leer estado del PCF8574T");
         return false;
     }
     
     Serial.println("Diagnóstico: PCF8574T operativo");
     Serial.print("Estado actual de los pines: 0b");
     for (int i = 7; i >= 0; i--) {
         Serial.print(bitRead(_estado_actual, i));
     }
     Serial.println();
     
     return true;
 }