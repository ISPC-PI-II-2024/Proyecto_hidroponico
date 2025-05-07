#include <Arduino.h>  

class LED {
public:
    LED(int pin);
    void setState(bool state);
    void blink();
private:
    int ledPin;
};

LED::LED(int pin) {
    ledPin = pin;
    pinMode(ledPin, OUTPUT);  // Configura el pin como salida
}

void LED::setState(bool state) {
    digitalWrite(ledPin, state ? HIGH : LOW);  // Enciende o apaga el LED
}

void LED::blink() {
    setState(true);  // Enciende el LED
    delay(300);     // Espera 300 ms
    setState(false); // Apaga el LED
    delay(300);     // Espera 300 ms
}
