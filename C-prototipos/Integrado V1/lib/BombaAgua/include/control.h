#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>

class ControlBomba {
public:
    ControlBomba(uint8_t relePin, uint8_t ledVerde, uint8_t ledRojo);
    void setup();
    void encenderBomba();
    void apagarBomba();

private:
    uint8_t _relePin, _ledVerde, _ledRojo;
};

#endif
