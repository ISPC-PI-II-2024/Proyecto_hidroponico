#ifndef MQ135_H
#define MQ135_H

class MQ135 {
public:
    MQ135(int pin);
    float readPPM();  // Devuelve una estimación de ppm

private:
    int _pin;
    float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
};

#endif
