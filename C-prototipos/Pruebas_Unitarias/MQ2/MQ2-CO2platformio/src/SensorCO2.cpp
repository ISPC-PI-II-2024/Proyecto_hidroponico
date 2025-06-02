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
