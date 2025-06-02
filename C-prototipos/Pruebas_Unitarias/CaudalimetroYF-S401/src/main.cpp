// Profesor: Mario González 05-05-2025
// Tecnicatura: TS Telecomunicaciones
// Materia: Proyecto integrado II

#include <Arduino.h>

// Definición del pin conectado a la salida del sensor de flujo
const int pinSensorFlujo = 4;  // Pin donde está conectado el botón / sensor

// Variables para el caudalímetro
volatile int contadorPulsos = 0;  // Contador de pulsos
unsigned long tiempoAnterior = 0;    // Tiempo anterior
float tasaFlujo = 0.0;         // Caudal en litros/minuto
float totalLitros = 0.0;      // Volumen total en litros
// Factor de calibración para el YF-S401 (aproximadamente 7.5 pulsos por litro)
const float factorCalibracion = 7.5;

// Variables para control de rebote del botón
volatile unsigned long ultimoTiempoRebote = 0;
unsigned long retraseRebote = 50;  // Tiempo de debounce en ms (Si se conecta un YF-S401 bajar el valor a 15 o 20)

// Función de interrupción que se ejecuta cuando hay un pulso
void IRAM_ATTR contarPulso() {
  // Debounce para evitar lecturas falsas
  unsigned long tiempoActual = millis();
  if ((tiempoActual - ultimoTiempoRebote) > retraseRebote) {
    contadorPulsos++;
    ultimoTiempoRebote = tiempoActual;
  }
}

void setup() {
  Serial.begin(9600);
  
  Serial.println("Lectura del Sensor de Flujo YF-S401");
  Serial.println("-----------------------------------------");
  
  // Inicializa el contador de pulsos
  contadorPulsos = 0;
  
  // Configura el pin del sensor como entrada con resistencia pull-up interna
  pinMode(pinSensorFlujo, INPUT_PULLUP);  // Usamos PULLUP para evitar lecturas flotantes
  
  // Adjunta la interrupción al pin del sensor.
  // RISING significa que se detectará el flanco de subida del pulso.
  attachInterrupt(digitalPinToInterrupt(pinSensorFlujo), contarPulso, RISING);
  
  tiempoAnterior = millis();
}

void loop() {
  unsigned long tiempoActual = millis();
  
  // Calcula el flujo cada segundo (1000 milisegundos)
  if ((tiempoActual - tiempoAnterior) >= 1000) {
    // Desactiva temporalmente las interrupciones para leer el contador de forma segura
    detachInterrupt(digitalPinToInterrupt(pinSensorFlujo));
    
    // Calcula la tasa de flujo en litros por minuto
    // Usando una fórmula más precisa que considera el tiempo real transcurrido
    tasaFlujo = ((float)contadorPulsos / factorCalibracion) * 60.0 / ((float)(tiempoActual - tiempoAnterior) / 1000.0);
    
    // Cálculo del volumen total
    totalLitros += (float)contadorPulsos / factorCalibracion;
    
    // Publica los resultados por monitor serial
    Serial.print("Pulsos: ");
    Serial.print(contadorPulsos);
    Serial.print(" | Tasa de Flujo: ");
    Serial.print(tasaFlujo, 2);  // Con 2 decimales
    Serial.print(" L/min | Volumen Total: ");
    Serial.print(totalLitros, 2);  // Con 2 decimales
    Serial.println(" L");
    
    // Reinicia el contador de pulsos
    contadorPulsos = 0;
    
    // Reactiva las interrupciones
    attachInterrupt(digitalPinToInterrupt(pinSensorFlujo), contarPulso, RISING);
    
    tiempoAnterior = tiempoActual;
  }
}