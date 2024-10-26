#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <Arduino.h>

class TimeUtils {
public:
  static void ISOString(const tm* timeinfo, char* buffer);
  static String ISOString(const tm* timeinfo);
  static void ISOString(time_t* time, char* buffer);
  static String ISOString(time_t* time);
  static void NowISOString(char* buffer);
  static String NowISOString();
  static String TimeDiffString(time_t time1, time_t time2);
  static time_t NextCronTime(const char* cronExpr, time_t from);
  static time_t NextCronTime(const char* cronExpr);
  static void SetDeviceTime(time_t time);

private:
  static time_t now;
  static tm* nowTimeinfo;
  static char timestampBuffer[20];
  static void SetInternalNow();
};

#endif
