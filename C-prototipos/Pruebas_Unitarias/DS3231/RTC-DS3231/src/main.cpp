#include <Arduino.h>
#include "DS3231.h"
#include "WiFiManager.h"

WiFiManager wifi;

DS3231Clock reloj;

void setup() {
  Serial.begin(9600);
  wifi.conectar();
  reloj.begin();
  if (wifi.estaConectado()) {
    reloj.sincronizarConNTP();
  } // Setear hora con servidor NTP
}

void loop() {
  Serial.println(reloj.getTimeString());
  delay(2000);
}




