#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

enum AlarmLevel { A_NONE, A_LOW, A_MEDIUM, A_HIGH };

void buzzerInit(int pin);
void buzzerSetLevel(AlarmLevel lvl);
void buzzerUpdate();

#endif