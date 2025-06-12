#include "ble_mesh.h"
#include <Arduino.h>

bool esNodoBLEMaestro = false;

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;

class MyBLECallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    std::string value = pCharacteristic->getValue();
    if (!value.empty()) {
      Serial.println("Mensaje recibido vía BLE:");
      Serial.println(value.c_str());
      // Aquí se puede procesar el mensaje recibido; por ejemplo,
      // si es nodo maestro, redirigirlo vía MQTT.
    }
  }
};

void inicializarBLEMesh() {
  BLEDevice::init("ESP32-Hidroponia-BLE");
  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(BLE_MESH_SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                        BLE_MESH_CHAR_UUID,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
  pCharacteristic->setCallbacks(new MyBLECallbacks());
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_MESH_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
  Serial.println("BLE Mesh iniciado.");
}

void enviarDatosBLE(const String& datos) {
  if (pCharacteristic != nullptr) {
    pCharacteristic->setValue(datos.c_str());
    pCharacteristic->notify();
  }
} 
