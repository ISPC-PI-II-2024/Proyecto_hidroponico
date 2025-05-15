#include <Arduino.h>

// Configuración del sensor TDS
const int sensorPin = 34;           // Pin analógico donde conectas el TDS
const float voltajeReferencia = 3.3; // Voltaje de referencia del ESP32
const int resolucionADC = 4095;     // Resolución ADC de 12 bits
const float factorTDS = 0.5;        // Factor de calibración (ajustable)

unsigned long tiempoAnterior = 0;   // Para control de intervalo de lectura
const unsigned long intervaloLectura = 5000; // Intervalo de 5 segundos (5000 ms)

// Función para leer voltaje
float leerVoltajeTDS() {
  int lecturaADC = analogRead(sensorPin);
  return (lecturaADC * voltajeReferencia) / resolucionADC;
}

// Función para calcular TDS (ppm)
float calcularTDS(float voltaje) {
  return (voltaje * 1000.0) / factorTDS;
}

// Función para realizar y mostrar la prueba
void testSensorTDS(float ppmMin, float ppmMax) {
  float voltaje = leerVoltajeTDS();
  float tds = calcularTDS(voltaje);

  Serial.print("Voltaje leído: ");
  Serial.print(voltaje, 3);
  Serial.print(" V | TDS: ");
  Serial.print(tds, 1);
  Serial.println(" ppm");

  if (isnan(tds) || tds < 0) {
    Serial.println("TEST FAILED ❌ - Lectura inválida");
    return;
  }

  bool tdsOK = (tds >= ppmMin) && (tds <= ppmMax);

  if (tdsOK) {
    Serial.println("TEST PASSED ✅");
  } else {
    Serial.println("TEST FAILED ❌ - TDS fuera de rango");
  }
  Serial.println("------------------------------");
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  delay(2000);

  Serial.println("Iniciando lectura de sensor TDS cada 5 segundos...");
}

void loop() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervaloLectura) {
    tiempoAnterior = tiempoActual;

    // Esperamos TDS en rango 0 - 1000 ppm para agua potable
    testSensorTDS(0.0, 1000.0);
  }
}


