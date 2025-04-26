#ifndef DS3231_H
#define DS3231_H

#include <RTClib.h>

class DS3231Clock {
  public:
    DS3231Clock();
    void begin();
    void setDateTime(int year, int month, int day, int hour, int minute, int second);
    String getTimeString();
    uint32_t getEpochTime();
    void sincronizarConNTP();  // nueva función para sincronizar RTC con hora NTP


  private:
    RTC_DS3231 rtc;
};

#endif