#include "control.h"
#include "sensores.h"


#define BUZZER_PIN 33;

// Estado del sistema
bool bomba_activa = false;
bool luces_activas = false;
bool alarma_activa = false;
bool modo_automatico = true;

// Inicialización del módulo de control
void inicializarControl() {
    pinMode(26, OUTPUT);  // Pin de la bomba
    pinMode(27, OUTPUT);  // Pin de las luces
    digitalWrite(26, LOW);
    digitalWrite(27, LOW);
}

// Verificación de alarmas en sensores
void verificarAlarmas() {
    alarma_activa = false;
    String mensaje_alarma = "";

    // Temperatura del agua
    if (!leerTemperaturaAgua()) {
        mensaje_alarma += "Error sensor temp agua; ";
        alarma_activa = true;
    } else if (temperaturaAgua < MIN_TEMP_AGUA || temperaturaAgua > MAX_TEMP_AGUA) {
        mensaje_alarma += "Temp agua fuera de rango: " + String(temperaturaAgua) + "°C; ";
        alarma_activa = true;
    }

    // Temperatura y humedad ambiente
    if (!leerDHT()) {
        mensaje_alarma += "Error sensor DHT; ";
        alarma_activa = true;
    } else {
        if (temperaturaAire < MIN_TEMP_AIRE || temperaturaAire > MAX_TEMP_AIRE) {
            mensaje_alarma += "Temp aire fuera de rango: " + String(temperaturaAire) + "°C; ";
            alarma_activa = true;
        }
        if (humedadAire < MIN_HUMEDAD || humedadAire > MAX_HUMEDAD) {
            mensaje_alarma += "Humedad fuera de rango: " + String(humedadAire) + "%; ";
            alarma_activa = true;
        }
    }

    // Nivel de agua
    if (!leerNivelAgua()) {
        mensaje_alarma += "Error sensor nivel agua; ";
        alarma_activa = true;
    } else if (nivelAgua < MIN_NIVEL_AGUA) {
        mensaje_alarma += "Nivel agua bajo: " + String(nivelAgua) + "cm; ";
        alarma_activa = true;
    }

    // Flujo de agua
    if (bomba_activa && !calcularFlujoAgua()) {
        mensaje_alarma += "Error sensor flujo agua; ";
        alarma_activa = true;
    } else if (bomba_activa && flujoAgua < MIN_FLUJO_AGUA) {
        mensaje_alarma += "Flujo agua insuficiente: " + String(flujoAgua) + "L/min; ";
        alarma_activa = true;
    }

    // Gas
    if (!leerGas()) {
        mensaje_alarma += "Error sensor gas; ";
        alarma_activa = true;
    } else if (gas > MAX_GAS) {
        mensaje_alarma += "Nivel de gas peligroso: " + String(gas) + "; ";
        alarma_activa = true;
    }

    // pH fuera de rango
    if (!leerPH()) {
        mensaje_alarma += "Error sensor pH; ";
        alarma_activa = true;
    } else if (ph < MIN_PH || ph > MAX_PH) {
        mensaje_alarma += "pH fuera de rango: " + String(ph) + "; ";
        alarma_activa = true;
    }
}

// Activación de la bomba de agua
void activarBomba(bool estado) {
    bomba_activa = estado;
    digitalWrite(26, estado ? HIGH : LOW);
    Serial.println(estado ? "Bomba activada" : "Bomba desactivada");
}

// Activación de la iluminación artificial
void activarIluminacion(bool estado) {
    luces_activas = estado;
    digitalWrite(27, estado ? HIGH : LOW);
    Serial.println(estado ? "Luces encendidas" : "Luces apagadas");
}

// Automatización del sistema
void controlAutomatico() {
    if (!modo_automatico) return;

    // **Control de la bomba**
    if (nivelAgua < MIN_NIVEL_AGUA) {
        activarBomba(true);
    } else if (nivelAgua > MIN_NIVEL_AGUA + 5.0) {
        activarBomba(false);
    }

    // **Control de iluminación**
    if (luz < MIN_LUZ) {
        activarIluminacion(true);
    } else if (luz > MAX_LUZ) {
        activarIluminacion(false);
    }

    // **Control del pH**
    if (ph < MIN_PH) {
        Serial.println("Agregar solución alcalina para ajustar pH.");
    } else if (ph > MAX_PH) {
        Serial.println("Agregar solución ácida para ajustar pH.");
    }

    // **Acciones en caso de alarma activa**
    if (alarma_activa) {
        Serial.println("⚠️ Sistema en alerta: revisión recomendada!");
    }
}

void inicializarIluminacion() {
    pinMode(RELE_ILUMINACION_PIN, OUTPUT);
    digitalWrite(RELE_ILUMINACION_PIN, LOW);
  }
  
