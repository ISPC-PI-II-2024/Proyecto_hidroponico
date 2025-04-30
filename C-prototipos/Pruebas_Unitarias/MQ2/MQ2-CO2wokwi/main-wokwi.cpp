#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int sensorPin = A0;    // MQ-2 (o simulador de CO2)
int ledPin = 9;        // LED
int potPin = A1;       // Potenciómetro para simular CO2
int sensorValue = 0;
int potValue = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C típica

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("MQ-2 Sensor");
  delay(2000);
}

void loop() {
  potValue = analogRead(potPin);  // Usamos el potenciómetro para simular el CO2
  sensorValue = map(potValue, 0, 1023, 0, 1500);  // Escalamos a 0-1500 ppm

  // Mostrar en Serial
  Serial.print("Simulado CO2: ");
  Serial.print(sensorValue);
  Serial.print(" ppm | Pot: ");
  Serial.println(potValue);

  // Mostrar en LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2: ");
  lcd.print(sensorValue);
  lcd.print("ppm");

  // Comportamiento del LED
  if (sensorValue < 400) {
    digitalWrite(ledPin, LOW);  // LED apagado
  } else if (sensorValue < 1000) {
    digitalWrite(ledPin, HIGH); // LED encendido fijo
  } else {
    // Parpadeo
    digitalWrite(ledPin, HIGH);
    delay(300);
    digitalWrite(ledPin, LOW);
    delay(300);
    return; // Salimos del loop temporal para no sobreescribir el parpadeo
  }

  delay(200);  // Pequeño delay de estabilización
}
