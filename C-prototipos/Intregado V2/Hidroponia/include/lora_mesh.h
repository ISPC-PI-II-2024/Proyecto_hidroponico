#ifndef LORA_MESH_H
#define LORA_MESH_H

#include <LoRa.h>
#include <Arduino.h>
extern void recibirDatosLoRa();



#define LORA_CS_PIN    15
#define LORA_RST_PIN   13
#define LORA_DIO0_PIN  34
#define LORA_FREQ      915E6  // Ajusta la frecuencia según tu región

extern bool esNodoLoRaMaestro;

void inicializarLoRaMesh();
void enviarDatosLoRa(const String& datos);
void recibirDatosLoRa();

#endif 
