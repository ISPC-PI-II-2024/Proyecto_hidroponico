
![alt text](<ChatGPT Image 28 abr 2025, 21_12_27.png>)

 Wokwi

https://wokwi.com/projects/429512446619343873



### *1. ESTADOS DEL LED:*

1 LED verde → Está seco (todo normal).

1 LED rojo → Lluvia detectada (advertencia).

### *Cómo conectar en el circuito*

LED Verde:

Anodo (pata larga) al pin 8 del Arduino.

Cátodo (pata corta) a una resistencia (~220Ω) y luego a GND.

LED Rojo:

Anodo a pin 9 del Arduino.

Cátodo igual: resistencia y GND.


![alt text](image.png)
### *3. Código*


#include <Wire.h>               // Para la comunicación I2C
#include <LiquidCrystal_I2C.h>   // Librería para LCD I2C

const int rainSensorPin = A0;  // Sensor conectado a A0
const int ledVerde = 8;        // LED verde conectado a pin 8
const int ledRojo = 9;         // LED rojo conectado a pin 9

int rainValue = 0;             // Variable para el valor leído

// Configura el LCD: Dirección del I2C, 16 columnas y 2 filas
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando sensor de lluvia...");

  pinMode(ledVerde, OUTPUT);
  pinMode(ledRojo, OUTPUT);

  // Inicia el LCD
  lcd.begin(16, 2);    // Inicializa LCD con 16 columnas y 2 filas
  lcd.backlight();     // Enciende la luz de fondo del LCD
  lcd.setCursor(0, 0); // Establece el cursor en la primera fila, primera columna
  lcd.print("Iniciando..."); // Mensaje de inicio en LCD
  delay(2000);          // Pausa de 2 segundos para que se vea el mensaje
}

void loop() {
  rainValue = analogRead(rainSensorPin);
  
  // Muestra el valor de humedad en el LCD
  lcd.clear();                  // Limpia la pantalla LCD
  lcd.setCursor(0, 0);          // Establece el cursor en la primera fila
  lcd.print("Valor Humedad: ");       
  lcd.print(rainValue);         // Muestra el valor del sensor
  
  // Condición de la humedad
  if (rainValue < 200) {
    lcd.setCursor(0, 1);        // Establece el cursor en la segunda fila
    lcd.print("Muy seco");
    digitalWrite(ledRojo, LOW);    // Apaga LED rojo
    analogWrite(ledVerde, 255);   // Prende LED verde al máximo brillo
  } 
  else if (rainValue >= 200 && rainValue < 400) {
    lcd.setCursor(0, 1);        // Establece el cursor en la segunda fila
    lcd.print("Condicion seca");
    digitalWrite(ledRojo, LOW);    // Apaga LED rojo
    analogWrite(ledVerde, 128);   // Reduce brillo del LED verde
  } 
  else if (rainValue >= 400 && rainValue < 800) {
    lcd.setCursor(0, 1);        // Establece el cursor en la segunda fila
    lcd.print("Humedad detectada");
    digitalWrite(ledRojo, HIGH);   // Prende LED rojo fijo
    analogWrite(ledVerde, 128);   // Reduce brillo del LED verde
  } 
  else {
    lcd.setCursor(0, 1);        // Establece el cursor en la segunda fila
    lcd.print("Lluvia detectada");
    // LED rojo parpadeando de manera dinámica
    for (int i = 0; i < 5; i++) {
      digitalWrite(ledRojo, HIGH);
      delay(300);
      digitalWrite(ledRojo, LOW);
      delay(300);
    }
    analogWrite(ledVerde, 0);   // Apaga LED verde
  }

  delay(3000); // Espera 3 segundos antes de la siguiente lectura
}


Si está seco:
 LED verde prendido → Normal
 LED rojo apagado.

Si llueve:
 LED rojo prendido → Advertencia
 LED verde apagado.




