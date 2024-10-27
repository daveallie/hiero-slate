#include "Logger.h"

#include <HardwareSerial.h>
#include <TimeUtils.h>

void Logger::Log(const char* tag, const char* message) {
  Serial.print("[");
  Serial.print(TimeUtils::NowISOString());
  Serial.print("]");
  Serial.print("[");
  Serial.print(tag);
  Serial.print("] ");
  Serial.println(message);
}

void Logger::Log(const char* tag, const String& message) {
  Log(tag, message.c_str());
}
