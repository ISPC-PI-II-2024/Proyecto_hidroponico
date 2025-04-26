#ifndef MY_WEBSERVER_H
#define MY_WEBSERVER_H

#include <WebServer.h>

#define RELAY_PIN 27
#define LED_VERDE 25
#define LED_ROJO 26

extern WebServer server;

void setupWebServer();

#endif
