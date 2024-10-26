extern "C" {
  #include <ccronexpr.h>
}
#include <Logger.h>
#include "TimeUtils.h"

static const char* TAG = "TIME";

time_t TimeUtils::now;
tm* TimeUtils::nowTimeinfo;
char TimeUtils::timestampBuffer[20];

bool TimeUtils::IsTimeSet() {
  SetInternalNow();
  return nowTimeinfo->tm_year > 0;
}

void TimeUtils::ISOString(const tm* timeinfo, char* buffer) {
  strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
}

String TimeUtils::ISOString(const tm* timeinfo) {
  ISOString(timeinfo, timestampBuffer);
  return {timestampBuffer};
}

void TimeUtils::ISOString(time_t* time, char* buffer) {
  auto timeinfo = localtime(time);
  return ISOString(timeinfo, buffer);
}

String TimeUtils::ISOString(time_t* time) {
  auto timeinfo = localtime(time);
  return ISOString(timeinfo);
}

void TimeUtils::NowISOString(char* buffer) {
  SetInternalNow();
  ISOString(nowTimeinfo, buffer);
}

String TimeUtils::NowISOString() {
  SetInternalNow();
  return ISOString(nowTimeinfo);
}

String TimeUtils::TimeDiffString(const time_t time1, const time_t time2) {
  time_t diff;
  if (time1 < time2) {
    diff = time2 - time1;
  } else {
    diff = time1 - time2;
  }

  String result = "";

  const unsigned long days = diff / 86400;
  if (days > 0) {
    result += String(days) + "d ";
    diff -= days * 86400;
  }

  const unsigned long hours = diff / 3600;
  if (hours > 0) {
    result += String(hours) + "h ";
    diff -= hours * 3600;
  }

  const unsigned long minutes = diff / 60;
  if (minutes > 0) {
    result += String(minutes) + "m ";
    diff -= minutes * 60;
  }

  const unsigned long seconds = diff;
  if (seconds > 0) {
    result += String(seconds) + "s";
  }

  return result;
}

time_t TimeUtils::NextCronTime(const char* cronExpr) {
  SetInternalNow();

  const char* err = nullptr;
  cron_expr* expr = cron_parse_expr(cronExpr, &err);
  if (err) {
    Logger::Log(TAG, "Error parsing cron expression: " + String(err));
    return 0;
  }

  time_t next = cron_next(expr, now);
  cron_expr_free(expr);

  return next;
}

void TimeUtils::SetInternalNow() {
  time(&now);
  nowTimeinfo = localtime(&now);
}