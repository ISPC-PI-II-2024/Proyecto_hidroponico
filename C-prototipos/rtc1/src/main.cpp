/*#include <Arduino.h>

#include <ThreeWire.h>  
#include <RtcDS1302.h>

#define PIN_CLK 5
#define PIN_DAT 18
#define PIN_RST 19

ThreeWire myWire(PIN_DAT, PIN_CLK, PIN_RST); // IO, SCLK, CE
RtcDS1302<ThreeWire> rtc(myWire);

void setup() {
  Serial.begin(9600);
  rtc.Begin();

  // Descomentar la siguiente línea para establecer la hora (solo una vez):
  rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));

  if (rtc.GetIsWriteProtected()) {
    rtc.SetIsWriteProtected(false);
  }

  if (!rtc.GetIsRunning()) {
    rtc.SetIsRunning(true);
  }

  Serial.println("RTC DS1302 iniciado");
}

void loop() {
  RtcDateTime now = rtc.GetDateTime();

  char dateTimeStr[20];
  snprintf(dateTimeStr, sizeof(dateTimeStr), "%02u/%02u/%04u %02u:%02u:%02u",
           now.Day(), now.Month(), now.Year(),
           now.Hour(), now.Minute(), now.Second());

  Serial.println(dateTimeStr);
  delay(1000);
}
*/


/*
#include <Arduino.h>
#include <ThreeWire.h>    
#include <RtcDS1302.h>
#include <SPI.h>

const int IO = 18;  // DAT
const int SCLK = 5; // CLK
const int CE = 19;  // RST

ThreeWire myWire(IO, SCLK, CE);
RtcDS1302<ThreeWire> Rtc(myWire);

void printDateTime(const RtcDateTime& dt);
void resetRtcToCompileTime();

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.print("Compiled: ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);

  Rtc.Begin();

  resetRtcToCompileTime();
  Serial.println("Presioná ENTER para reiniciar RTC con fecha/hora de compilación.");
}

void loop() {
  // Si presionás Enter (en blanco), reinicia el RTC
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      resetRtcToCompileTime();
    }
  }

  delay(10);
}

void resetRtcToCompileTime() {
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);
  Serial.print("RTC reiniciado con fecha y hora de compilación: ");
  printDateTime(compiled);
  Serial.println();
}

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];
  snprintf_P(datestring,
             sizeof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}

*/
#include <Arduino.h>
#include <ThreeWire.h>    
#include <RtcDS1302.h>
#include <SPI.h>

const int IO = 18;  // DAT
const int SCLK = 5; // CLK
const int CE = 19;  // RST

ThreeWire myWire(IO, SCLK, CE);
RtcDS1302<ThreeWire> Rtc(myWire);

void printDateTime(const RtcDateTime& dt);
void resetRtcIfInvalid();

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Esperar al monitor serial (opcional en algunos boards)

  Serial.print("Compiled: ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);

  Rtc.Begin();

  // Desbloquear y arrancar el RTC si es necesario
  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC estaba protegido contra escritura, se desprotege...");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC no estaba en marcha, se arranca...");
    Rtc.SetIsRunning(true);
  }

  resetRtcIfInvalid();  // Comprobar y forzar fecha válida
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();

  if (!now.IsValid()) {
    Serial.println("RTC devolvió una hora inválida. Reiniciando...");
    resetRtcIfInvalid();  // Reintentar
  } else {
    printDateTime(now);
    Serial.println();
  }

  delay(5000);  // 5 segundos
}

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring,
             sizeof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}

void resetRtcIfInvalid() {
  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC no tiene una hora válida. Se restaura a fecha de compilación.");
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Rtc.SetDateTime(compiled);
  }

  // Seguridad: comprobar si la hora actual es basura (fuera de rango)
  RtcDateTime now = Rtc.GetDateTime();
  if (!now.IsValid() || now.Year() > 2100 || now.Month() > 12 || now.Day() > 31) {
    Serial.println("La hora actual no es válida (datos fuera de rango). Se fuerza nueva fecha.");
    Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  }
}