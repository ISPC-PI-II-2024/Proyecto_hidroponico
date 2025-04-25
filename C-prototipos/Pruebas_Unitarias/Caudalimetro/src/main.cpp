#include <Arduino.h>

// Definición de pines
const int SENSOR_PIN = 12;  // Pin donde está conectado el botón

// Variables para el caudalímetro
volatile int pulseCount = 0;  // Contador de pulsos
unsigned long oldTime = 0;    // Tiempo anterior
float flowRate = 0.0;         // Caudal en litros/minuto
float totalLitros = 0.0;      // Volumen total en litros
float factorCalibracion = 7.5; // Factor de calibración (pulsos por litro)
                              // Para el YF-S201 son: 7.5 pulsos por litro

// Variables para control de rebote del botón
volatile unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // Tiempo de debounce en ms

// Función de interrupción que se ejecuta cuando hay un pulso
void IRAM_ATTR pulseCounter() {
  // Debounce para evitar lecturas falsas
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    pulseCount++;
    lastDebounceTime = currentTime;
  }
}

void setup() {
  Serial.begin(9600);
  
  // Configurar el pin del sensor como entrada con resistencia pull-up interna
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  
  // Adjuntar la interrupción al pin del sensor, activando en flanco DESCENDENTE
  // (cuando el botón se presiona, la entrada va de HIGH a LOW)
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
  
  Serial.println("Simulador de Caudalímetro de Efecto Hall");
  Serial.println("-----------------------------------------");
}

void loop() {
  // Actualizar cada segundo
  if ((millis() - oldTime) > 1000) {
    // Desactivar interrupciones temporalmente mientras se calcula
    detachInterrupt(digitalPinToInterrupt(SENSOR_PIN));
    
    // Cálculo del caudal (L/min) = (pulsos / factor_calibración) * 60
    // El factor de calibración es pulsos por litro
    // Multiplicamos por 60 para convertir de L/s a L/min
    flowRate = ((float)pulseCount / factorCalibracion) * 60;
    
    // Cálculo del volumen total
    totalLitros += (float)pulseCount / factorCalibracion;
    
    // Imprimimos los resultados
    Serial.print("Pulsos: ");
    Serial.print(pulseCount);
    Serial.print(" | Caudal: ");
    Serial.print(flowRate, 2);  // Con 2 decimales
    Serial.print(" L/min | Volumen Total: ");
    Serial.print(totalLitros, 2);  // Con 2 decimales
    Serial.println(" L");
    
    // Reiniciamos el contador y actualizamos el tiempo
    pulseCount = 0;
    oldTime = millis();
    
    // Reactivamos las interrupciones
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
  }
}