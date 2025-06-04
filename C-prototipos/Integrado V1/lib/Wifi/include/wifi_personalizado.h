#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

class WiFiCtrl {
public:
    //--------------  
    // Estados posibles del controlador Wi-Fi  
    //--------------  
    enum class State : uint8_t {
        Idle,           // Estado inicial sin acción
        Conectando,     // Intentando conectar en modo STA
        PortalActivo,   // AP + servidor web activo (modo configuración)
        Conectado,      // Conexión STA exitosa
        Error           // Hubo un error (credenciales inválidas o desconexión)
    };

    /**
     * @brief Constructor de WiFiCtrl.  
     * @param serial      Canal Serial donde se mostrarán mensajes de estado.  
     * @param buttonPin   Pin GPIO que dispara el modo configuración (INPUT_PULLUP).  
     * @param apSSID      SSID que usará el ESP32 en modo AP (por defecto "ESP32_Config").  
     * @param apPassword  Password opcional para el AP; si nullptr, el AP quedará abierto.  
     */
    WiFiCtrl(HardwareSerial& serial,
             uint8_t buttonPin,
             const char* apSSID = "ESP32_Config",
             const char* apPassword = nullptr);

    /**
     * @brief Inicializa el controlador.  
     *        - Configura pin de botón.  
     *        - Abre Preferences (“wifi”) para leer SSID/Password guardados.  
     *        - Si no hay credenciales o si el botón está presionado, arranca modo configuración.  
     *        - Si hay credenciales, intenta conectar en modo STA; si falla en <5 s, cae en modo configuración.  
     * @note  Debe llamarse desde setup().  
     */
    void iniciar();

    /**
     * @brief Si estamos en modo configuración (AP + servidor), procesa conexiones HTTP.  
     *        Debe llamarse continuamente desde loop().  
     */
    void handleClient();

    /**
     * @brief Devuelve el estado actual del controlador Wi-Fi.  
     */
    State obtenerEstado() const;

private:
    HardwareSerial& _serial;    // Serial para reportar mensajes
    uint8_t         _buttonPin; // Pin que activa modo configuración (INPUT_PULLUP)
    const char*     _apSSID;    // SSID del AP en modo configuración
    const char*     _apPassword;// Password del AP (o nullptr si abierto)
    WebServer       _server;    // Servidor HTTP en el puerto 80
    Preferences     _prefs;     // Para guardar “ssid” y “pass” en Flash
    State           _estado;    // Estado interno
    bool            _configMode;// true si estamos en modo AP/servidor

    //--------------  
    // Métodos privados  
    //--------------  
    /**
     * @brief Cambia a modo AP (WIFI_AP), arranca el servidor y define rutas.  
     *        A partir de aquí, _configMode = true.  
     */
    void startConfigMode();

    /**
     * @brief Declara las rutas "/" y "/save" en _server, y un onNotFound().  
     */
    void setupRoutes();

    /**
     * @brief Handler para GET "/" → envía la página HTML con formulario SSID/Password.  
     */
    void handleRoot();

    /**
     * @brief Handler para POST "/save" → lee parámetros “ssid” y “pass”, valida,  
     *        guarda en Preferences y reinicia tras responder al cliente.  
     */
    void handleSave();

    /**
     * @brief Intenta conectar en modo STA con las credenciales recibidas.  
     *        Solo hace WiFi.begin(); el timeout y flujo de error/éxito se maneja en iniciar().  
     * @param ssid      Nombre de la red Wi-Fi.  
     * @param password  Contraseña de la red Wi-Fi.  
     */
    void connectToWiFi(const String& ssid, const String& password);

    /**
     * @brief Envía un mensaje por _serial indicando el estado actual.  
     * @param msg  Cadena de texto (C-string) a imprimir.  
     */
    void reportarEstado(const char* msg);
};
