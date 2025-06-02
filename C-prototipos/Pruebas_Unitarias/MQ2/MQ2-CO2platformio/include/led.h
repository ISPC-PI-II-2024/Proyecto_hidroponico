#ifndef LED_H
#define LED_H

class LED {
public:
    LED(int pin);
    void setState(bool state);
    void blink();

private:
    int ledPin;
    bool state;
};

#endif
