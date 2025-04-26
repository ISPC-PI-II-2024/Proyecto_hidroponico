### Donde y que colocar los elementos dentro del main.cpp

### Definiciones pre setup

  #include "sr04/sr04.h"
  #define TRIG_PIN 5
  #define ECHO_PIN 18

### Setup

    sr04Begin(TRIG_PIN, ECHO_PIN);

### Loop

    float distancia = sr04Read();
    if (distancia >= 0) {
      Serial.printf("Distancia SR04: %.1f cm\n", distancia);
      // Publicacion MQTT - Topic sensores/sr04
        StaticJsonDocument<64> j;
        j["distancia"] = distancia;
        mqttPublishJson("sensores/sr04", j, 1, false);
    } 
    else {
      Serial.println("Lectura SR04 inválida");
    }