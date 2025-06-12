#ifndef BLE_MESH_H
#define BLE_MESH_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>


#define BLE_MESH_SERVICE_UUID "0000FF00-0000-1000-8000-00805F9B34FB"
#define BLE_MESH_CHAR_UUID    "0000FF01-0000-1000-8000-00805F9B34FB"

extern bool esNodoBLEMaestro; // Define si este nodo actúa como maestro en la malla BLE

// Funciones para inicializar y utilizar BLE Mesh
void inicializarBLEMesh();
void enviarDatosBLE(const String & datos);

#endif 
