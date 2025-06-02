#include "JsonBuilder.h"

JsonBuilder::JsonBuilder() {
  limpiar();
}

void JsonBuilder::limpiar() {
  documento.clear();
}

void JsonBuilder::agregarCampo(const char* nombre, String valor) {
  if (valor.length() > 0) {
    documento[nombre] = valor;
  }
}

void JsonBuilder::agregarCampo(const char* nombre, float valor) {
  documento[nombre] = valor;
}

void JsonBuilder::agregarCampo(const char* nombre, int valor) {
  documento[nombre] = valor;
}

String JsonBuilder::construir() {
  String salida;
  serializeJsonPretty(documento, salida);
  return salida;
}
