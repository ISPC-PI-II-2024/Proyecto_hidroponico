#include <WiFi.h>
#include <WebServer.h>
#include "my_webserver.h"

WebServer server(80);  // Puerto del servidor

void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    String html = R"(
      <!DOCTYPE html>
      <html>
      <head>
        <title>Control de Bomba</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          body { font-family: sans-serif; text-align: center; margin-top: 50px; }
          button { font-size: 20px; padding: 10px 30px; margin: 20px; }
          .on { background-color: #4CAF50; color: white; }
          .off { background-color: #f44336; color: white; }
        </style>
      </head>
      <body>
        <h1>Control de la Bomba de Agua</h1>
        <form action="/encender" method="POST">
          <button class="on">ENCENDER</button>
        </form>
        <form action="/apagar" method="POST">
          <button class="off">APAGAR</button>
        </form>
      </body>
      </html>
    )";
    server.send(200, "text/html", html);
  });

  server.on("/encender", HTTP_POST, []() {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_ROJO, LOW);
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/apagar", HTTP_POST, []() {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_ROJO, HIGH);
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.begin();
}
