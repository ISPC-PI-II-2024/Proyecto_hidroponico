# SENSOR: MQ-2 (o MQ-135)

La siguiente prueba unitaria esta destinada a la familia de sensores MQ-N para la medicion de CO2, mediante la utilizacion de la libreria MQUnifiedsensor@^3.0.0 para facilitacion de la calibracion y las cuentas de evaluacion de PPM del gas indicado

**a considerar**
El sensor MQ-2 no esta destinado a la medicion de CO2. La mayoria de los sensores de la familia MQ tienen como fin la deteccion de gases inflamables y humo. Una excepcion es, por ejemplo el sensor MQ-135 que posee una gama de medicion de gases mas amplia.

### Caracteristicas de sensores MQ-N
Son sensores analogicos semiconductores resistivos, que mediante la interaccion con determinados elementos del ambiente varian su resistencia. Es mediante la comparacion de la variacion de resistencia en presencia de aire limpio, y el aire con presencia del gas a medir que podemos calcular la concentracion de dicho gas en el ambiente

Esto requiere que el sensor en cuestion se calibre. Segun la documentacion se recomienda entre 10 y 24hs la primera vez que se encienda (sea nuevo, o por que tiene varios meses de NO uso). En uso regular el mismo levanta temperatura en unos 5/10 minutos.

La calibracion ante la presencia de gases se logra mediante la documentacion de cada modelo del sensor en el que se indican curvas de sensibilidad con la carga resistiva del modelo en cuestion.

### Libreria: MQUnifiedsensor@^3.0.0

- Documentacion de la libreria: [link-a-la-libreria](https://github.com/miguel5612/MQSensorsLib)
- En el "REAMDE.md" de la libreria se encuentran los links a las datasheets de los diferentes sensores que permite gestion la libreria. Se agregan los dos posibles a utilizar para evitar futuras descargas.

El usuario creador de la misma deja bien documentado ejemplo de uso de las funciones planteadas en el link previo.
La libreria tiene la intencion de facilitar la configuracion y calibracion de varios modelos de la flia MQ-N en proyectos que utilizan microcontroladores como Arduino, o ESP.

**A considerar** Los sensores MQ requieren un voltage de 5V. Casos como en el ESP8266 se requieren dos fuentes. Pero para con el ESP32 se indica lo siguiente
ESP32 WROOM 32D
-- The ESP32 WROOM 32D does not need an external power supply. A0 goes to PIN36, Vcc to 3v3 and GND to any GND port on the board. Check the ESP2/ESP32_WROOM_32 folder to fixing the measuring issue when connecting to wifi.--

### Codigo

** SensorCO2.h** 
```cpp
//=============================================
// Librerias y condicional
//=============================================
#ifndef SENSORCO2_H
#define SENSORCO2_H
#include <Arduino.h>
#include "MQUnifiedsensor.h"  // Librería para sensors MQ, nos simplifica la lectura y calibracion
//=============================================
// Configuración del sensor MQ2 para medir CO2
//=============================================
// Este formato estandar viene defindo en la documentacion de la  libreria MQUnifiedsensor
// Son los parametros que luego llamamos en la funcion [SensorCO2::SensorCO2(int pin)]
// RS refiere a la resistencia del sensor y RO a la resistencia en aire limpio. Esta ultima es la que se usa de referencia
#define BOARD               F("ESP32")  // Identificador de la placa
#define VCC                 3.3f        // Voltaje de alimentación (V)
#define RL_KOHM             10.0f       // Resistencia de carga en kilo-ohmios (kΩ)
#define ADC_RES             12          // Resolución ADC del ESP32 (12 bits = 4095)
#define RATIO_CLEAN_AIR     9.8f        // Rs/Ro en aire limpio según datasheet
#define CALIB_DELAY_MS      2000          // Tiempo de espera para estabilizar el sensor (ms)
// Por suerte la configuracion para el MQ135 es simple, comentar y descomentar lo siguiente
//#define RATIO_CLEAN_AIR     3.8f 
//#define RL_KOHM             20.0f
class SensorCO2 {
public:
    explicit SensorCO2(int pin);        // PIN a definir segun la planilla de sensores
    void begin();                       // Inicializa el sensor y la librería MQUnifiedsensor
    int readRaw();                      // Lectura cruda del sensor (ADC)  
    float readResistance();             // Lectura de resistencia del sensor (Rs) en KOhms
    float readRatio();                  // Lectura de la relación Rs/Ro
    float readPPM();                    // Lectura de PPM de CO2
private:
    int sensorPin;
    float Ro;                           // Resistencia en aire limpio (kΩ)
    MQUnifiedsensor mq;                // Objeto de la librería MQUnifiedsensor para el sensor MQ sea el 2, o el 135
};
#endif 
```

**SensorCO2.cpp** 
```cpp
#include "SensorCO2.h"
SensorCO2::SensorCO2(int pin)                                   // Inicializa el sensor mq, usando la funcion mq de la libreria MQUnifiedsensor
  : sensorPin(pin)
  , mq(BOARD, VCC, ADC_RES, sensorPin, F("MQ-2")){}             // placa, voltaje, resolución ADC, pin, tipo de sensor    
//, mq(BOARD, VCC, ADC_RES, sensorPin, F("MQ-135")) {} 
        
//=============================================
// Funciones de la clase para llamar en el SETUP
//=============================================
void SensorCO2::begin() {
    Serial.println("Inicializando mq y calibrando Ro...");
    mq.setRegressionMethod(1);                                 // regresión exponencial (curva log-log)
    mq.setRL(RL_KOHM);                                 
    mq.init();                                                 // inicializa ADC interno del sensor
    mq.update();                                               // Actualiza medición interna
    delay(CALIB_DELAY_MS);                                                 
    Ro = mq.calibrate(RATIO_CLEAN_AIR);                    // Calibración usando el valor de Rs/Ro en aire limpio
    Serial.print("Calibración completa. Ro = ");
    Serial.print(Ro, 2);
    Serial.println(" kΩ\n");
}
//=============================================
// Funciones de la clase para llamar en el LOOP
//=============================================
int SensorCO2::readRaw() {
    return analogRead(sensorPin);               // Lectura del valor analogico
}
float SensorCO2::readResistance() {
    mq.update();                               // Actualiza medición interna
    float voltage = mq.getVoltage();           // Obtiene voltaje del sensor                  
    float rs = (VCC - voltage) / voltage * mq.getRL();  // Calcula Rs(res del sensor)
    return rs;
}
float SensorCO2::readRatio() {                  // Calcula la relación Rs/Ro (res sensor/ res aire limpio)
    return readResistance() / Ro;
}
float SensorCO2::readPPM() {
    mq.update();                               // refrescar medición
    return mq.readSensor();                    // devuelve PPM para mq
}
```

**main.cpp**
```cpp
//=============================================
// Librerias y condicional
//=============================================
#include <Arduino.h>
#include "SensorCO2.h"

#ifndef SENSORGAS_PIN
#define SENSORGAS_PIN 15 
#endif
//=============================================
// INCIALIZACION/DEFINICIONES
//=============================================
SensorCO2 co2Sensor(SENSORGAS_PIN);
//=============================================
// INICIALIZACION del MONITOR SERIAL + SENSOR
//=============================================
void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("=== Iniciando lectura de CO2 con MQ ===");
    co2Sensor.begin();
}
//=============================================
// CICLO DE LECTURA DEL SENSOR
//=============================================
// 1- co2Sensor.readRaw(); lectura analogica sin mas
// 2- co2Sensor.readResistance(); lee la resistencia del sensor en ese determinado instante
// 3- co2Sensor.readRatio(); hace la relacion entre la res anterior, y la resistencia de referencia
// 4- co2Sensor.readPPM(); hace la lectura y devuelve la PPM del gas en cuestion
//=============================================
void loop() {
    int raw    = co2Sensor.readRaw();
    float rs   = co2Sensor.readResistance();
    float ratio= co2Sensor.readRatio();
    float ppm  = co2Sensor.readPPM();

    Serial.print("Lectura analogica: ");     Serial.print(raw);
    Serial.print("  Resistencia Sensor: ");  Serial.print(rs, 2);    Serial.print(" kΩ");
    Serial.print("  Relacion Rs/Ro: ");      Serial.print(ratio, 3);
    Serial.print("  CO2 PPM: ");             Serial.println(ppm, 1);

    delay(1000);
}
```