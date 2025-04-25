#include <Arduino.h>
#include "MQ135.h"

MQ135 mq135Sensor(A0);  // Instanciamos el sensor en A0

void setup() {
    Serial.begin(9600);
}

void loop() {
    float ppm = mq135Sensor.readPPM();
    Serial.print("CO2 estimado: ");
    Serial.print(ppm);
    Serial.println(" ppm");
    delay(1000);
}
