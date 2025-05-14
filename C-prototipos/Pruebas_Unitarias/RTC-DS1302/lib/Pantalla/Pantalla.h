#ifndef PANTALLA_H
#define PANTALLA_H

#include <Arduino.h>
#include <RtcDS1302.h>

class Pantalla {
public:
    void mostrarHora(const RtcDateTime& dt);
    void mensajeInicio();
    void mensajeHoraInvalida();
    void mensajeComandoNoValido(const String& cmd);
};

#endif
