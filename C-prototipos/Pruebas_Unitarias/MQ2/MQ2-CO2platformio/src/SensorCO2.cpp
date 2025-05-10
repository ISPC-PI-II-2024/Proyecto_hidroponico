#include "SensorCO2.h"


SensorCO2::SensorCO2(int pin)                                   // Inicializa el sensor MQ2, usando la funcion mq2 de la libreria MQUnifiedsensor
  : sensorPin(pin)
  , mq2(BOARD, VCC, ADC_RES, sensorPin, F("MQ-2"))         // placa, voltaje, resolución ADC, pin, tipo de sensor    
{
}
        
//=============================================
// Funciones de la clase para llamar en el SETUP
//=============================================

void SensorCO2::begin() {
    Serial.println("Inicializando MQ2 y calibrando Ro...");
    mq2.setRegressionMethod(1);                                 // regresión exponencial (curva log-log)
    mq2.setRL(RL_KOHM);                                 
    mq2.init();                                                 // inicializa ADC interno del sensor
    Ro = mq2.calibrate(RATIO_MQ2_CLEAN_AIR);                    // Calibración usando el valor de Rs/Ro en aire limpio
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
    mq2.update();                               // Actualiza medición interna
    float voltage = mq2.getVoltage();           // Obtiene voltaje del sensor                  
    float rs = (VCC - voltage) / voltage * mq2.getRL();  // Calcula Rs(res del sensor)
    return rs;
}
float SensorCO2::readRatio() {                  // Calcula la relación Rs/Ro (res sensor/ res aire limpio)
    return readResistance() / Ro;
}
float SensorCO2::readPPM() {
    mq2.update();                               // refrescar medición
    return mq2.readSensor();                    // devuelve PPM para MQ2
}
