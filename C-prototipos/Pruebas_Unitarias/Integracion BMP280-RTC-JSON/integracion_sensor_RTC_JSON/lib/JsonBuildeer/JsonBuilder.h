#ifndef JSONBUILDER_H
#define JSONBUILDER_H

#include <ArduinoJson.h>

class JsonBuilder {
  public:
    JsonBuilder();
    void limpiar();
    void agregarCampo(const char* nombre, String valor);
    void agregarCampo(const char* nombre, float valor);
    void agregarCampo(const char* nombre, int valor);
    String construir();

  private:
    StaticJsonDocument<512> documento;
};

#endif
