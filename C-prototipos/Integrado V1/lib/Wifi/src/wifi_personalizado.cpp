#include "wifi_personalizado.h"

static WiFiCtrl* _instance = nullptr;  // Puntero a la instancia para usar en el callback

WiFiCtrl::WiFiCtrl(HardwareSerial& serial)
  : _serial(serial), _estado(State::Idle), _eventoId(0)
{
    _instance = this;
}

//================================
// Métodos de la clase WiFiCtrl
//================================

//----------------------------------------------------------------
// Inicializa WiFiManager y registra el callback de eventos WiFi
//----------------------------------------------------------------
void WiFiCtrl::iniciar() {
    _eventoId = WiFi.onEvent(WiFiCtrl::onWiFiEvent);     // Registrar callback de eventos WiFi
    _estado = State::Conectando;                         // Intentar conectar en modo STA mediante WiFiManager
    reportarEstado("Conectando a WiFi (modo STA)...");

    // Configuración de portal cautivo:
    _PortalCautivo.setTimeout(10);                       // Timeout de 10 segundos para el portal cautivo
    _PortalCautivo.setAPCallback([](WiFiManager* wm) {        
        if (_instance) {
            _instance->_estado = State::PortalActivo;
            _instance->reportarEstado("Portal cautivo activo. Accede a 192.168.4.1 para configurar.");
        }
    });

    // Si ya hay credenciales guardadas, se conecta en STA, Si falla o no hay credenciales, levanta portal AP con SSID="ESP32_Config"
    bool conectado = _PortalCautivo.autoConnect("ESP32_Config");
    if (!conectado) {
        _estado = State::Error;                          // Si no se pudo conectar en modo STA, se asume que no hay credenciales válidas
        reportarEstado("Error: Timeout o fallo al conectar en modo STA. Portal cerrado.");
        return;
    }
}

//----------------------------------------------------------------
// Callback estático para manejar eventos de WiFi 
//----------------------------------------------------------------
void WiFiCtrl::onWiFiEvent(WiFiEvent_t event) {
    if (!_instance) return;

    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP: {
            _instance->_estado = State::Conectado;
            IPAddress ip = WiFi.localIP();
            char buf[128];
            snprintf(buf, sizeof(buf),
                     "WiFi OK — SSID: %s, IP: %s",
                     WiFi.SSID().c_str(),
                     ip.toString().c_str());
            _instance->reportarEstado(buf);
            break;
        }
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
            // Si se desconecta tras haber estado conectado
            _instance->_estado = State::Error;
            _instance->reportarEstado("Error: desconectado o credenciales inválidas.");
            break;
        }
        default:
            // Otros eventos no se manejan explícitamente
            break;
    }
}

//----------------------------------------------------------------
// Devuelve el estado actual del controlador WiFi
//----------------------------------------------------------------
WiFiCtrl::State WiFiCtrl::obtenerEstado() const {
    return _estado;
}

//----------------------------------------------------------------
// Inicia la conexión en modo estación (STA) manualmente
//----------------------------------------------------------------
void WiFiCtrl::iniciarSTA() {
    _estado = State::Conectando;
    reportarEstado("Iniciando conexión STA manualmente...");
    WiFi.mode(WIFI_STA);  // Asegura que estamos en modo estación
    WiFi.begin();         // Intenta conectar a la red WiFi con las credenciales guardadas  
}

//----------------------------------------------------------------
// Levanta el portal cautivo en modo AP directamente
//----------------------------------------------------------------
void WiFiCtrl::iniciarSTAPortal() {
    _estado = State::PortalActivo;                // Levanta AP + portal cautivo directamente
    reportarEstado("Levantando portal cautivo en modo AP...");
    _PortalCautivo.startConfigPortal("ESP32_Config");
    // startConfigPortal bloquea hasta que se configure o se cierre el portal, luego de config se distara el autoconnect
}

//----------------------------------------------------------------
// Reporta el estado actual del controlador WiFi por Serial
//----------------------------------------------------------------
void WiFiCtrl::reportarEstado(const char* msg) {
    _serial.println(String("[WiFiCtrl] ") + msg);
}
