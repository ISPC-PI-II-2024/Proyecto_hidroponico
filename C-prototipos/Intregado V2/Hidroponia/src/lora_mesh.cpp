#include "lora_mesh.h"
#include <Arduino.h>

bool esNodoLoRaMaestro = false;

void inicializarLoRaMesh() {
  LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_DIO0_PIN);
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Error al iniciar LoRa");
  } else {
    Serial.println("LoRa Mesh iniciado correctamente.");
  }
}

void enviarDatosLoRa(const String& datos) {
  LoRa.beginPacket();
  LoRa.print(datos);
  LoRa.endPacket();
}


