#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>


class WiFiManager {
  public:
    WiFiManager();
    void conectar();
    bool estaConectado();
};

#endif
