#include <Arduino.h>
#include "DS3231.h"
#include "WiFiManager.h"

WiFiManager wifi;
DS3231Clock reloj;

unsigned long Ultima_muestra = 0;
const unsigned long Intervalo_muestra = 2000;

void setup() {
  Serial.begin(9600);

  wifi.conectar();
  reloj.begin();

  if (wifi.estaConectado()) {
    reloj.sincronizarConNTP();
  } else {
    Serial.println("No hay conexión WiFi, no se sincronizará con NTP.");
  }
}

void loop() {
  unsigned long Millis_actual = millis();

  if (Millis_actual - Ultima_muestra >= Intervalo_muestra) {
    Ultima_muestra = Millis_actual;

    String hora = reloj.getTimeString();
    Serial.println(hora);
  }
}


