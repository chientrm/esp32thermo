#include "ds3231_helper.h"
#include <time.h>

RTC_DS3231 rtc;
DateTime lastTime;

void initDS3231()
{
    Wire.begin();
    rtc.begin();
    // Try to read time from RTC
    if (!rtc.lostPower())
    {
        lastTime = rtc.now();
    }
}

void updateDS3231FromNTP()
{
    time_t tnow = time(nullptr);
    struct tm *tm_info = localtime(&tnow);
    if (tnow >= 1609459200 && tm_info)
    {
        rtc.adjust(DateTime(
            tm_info->tm_year + 1900,
            tm_info->tm_mon + 1,
            tm_info->tm_mday,
            tm_info->tm_hour,
            tm_info->tm_min,
            tm_info->tm_sec));
    }
}

String getDS3231DateTime()
{
    DateTime now = rtc.now();
    char buf[32];
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
            now.year(),
            now.month(),
            now.day(),
            now.hour(),
            now.minute(),
            now.second());
    return String(buf);
}
