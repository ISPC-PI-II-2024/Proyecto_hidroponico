### Donde y que colocar los elementos dentro del main.cpp

### Definiciones pre setup

  #include "buzzer/buzzer.h"
  #define BUZZER_PIN 5

### Setup

    buzzerInit(BUZZER_PIN);

### Loop

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();       
    // mensajes de referencia: 100, 200, 300. Los cambiaremos proximametne
    if      (cmd == "100") buzzerSetLevel(A_LOW);
    else if (cmd == "200") buzzerSetLevel(A_MEDIUM);
    else if (cmd == "300") buzzerSetLevel(A_HIGH);
    else                   buzzerSetLevel(A_NONE);
  }
  buzzerUpdate();