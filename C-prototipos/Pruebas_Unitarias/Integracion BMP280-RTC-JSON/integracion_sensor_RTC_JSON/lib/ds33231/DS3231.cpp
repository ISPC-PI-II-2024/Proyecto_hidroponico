#include "DS3231.h"

DS3231Clock::DS3231Clock() {}

void DS3231Clock::begin() {
  rtc.begin();
}

void DS3231Clock::setDateTime(int year, int month, int day, int hour, int minute, int second) {
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
}

String DS3231Clock::getTimeString() {
  DateTime now = rtc.now();
  char buffer[20];
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
          now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());
  return String(buffer);
}

uint32_t DS3231Clock::getEpochTime() {
  return rtc.now().unixtime();
}

#include <time.h>

void DS3231Clock::sincronizarConNTP() {
  const char* ntpServer = "pool.ntp.org";
  const long gmtOffset_sec = -10800;  // UTC-3 (Argentina)
  const int daylightOffset_sec = 0;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm tiempo;
  if (!getLocalTime(&tiempo)) {
    Serial.println("❌ Error obteniendo hora desde NTP");
    return;
  }

  // Convertimos a DateTime y lo seteamos en el RTC
  DateTime now(tiempo.tm_year + 1900, tiempo.tm_mon + 1, tiempo.tm_mday,
               tiempo.tm_hour, tiempo.tm_min, tiempo.tm_sec);

  rtc.adjust(now);
  Serial.println("✅ RTC actualizado con hora NTP:");
  Serial.println(now.timestamp());
}
