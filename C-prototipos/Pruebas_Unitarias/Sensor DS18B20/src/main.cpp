#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4  // Pin GPIO4 para el sensor DS18B20
#define RED_PIN 25      // Pin rojo del LED RGB
#define GREEN_PIN 26    // Pin verde del LED RGB
#define BLUE_PIN 27     // Pin azul del LED RGB

// Configuración de la comunicación OneWire
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensores(&oneWire);

void setup() {
  Serial.begin(9600);  // Inicia la comunicación serial
  sensores.begin();    // Inicializa el sensor DS18B20
  
  // Configuración de los pines del LED RGB
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // Prueba de inicialización
  Serial.println("Iniciando las pruebas...");

  // Comienza la lectura de la temperatura
  sensores.requestTemperatures();
}

void loop() {
  // Leer la temperatura
  float temperatura = sensores.getTempCByIndex(0);
  
  // Verificar si la temperatura es válida
  if (temperatura >= -55 && temperatura <= 125) {
    Serial.print("Temperatura actual: ");
    Serial.print(temperatura);
    Serial.println(" °C");

    // Indicar que la lectura fue exitosa con el LED verde
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
  } else {
    // Si la lectura es inválida, indica el error con el LED rojo
    Serial.println("Error en la lectura de la temperatura.");
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }

  delay(1000);  // Espera 1 segundo antes de hacer una nueva lectura
}