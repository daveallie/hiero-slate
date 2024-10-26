#include <Arduino.h>
#include "Logger.h"

time_t now;
tm * timeinfo;
char timestampBuffer[20]; // "YYYY-MM-DD HH:MM:SS" + null terminator

void Logger::Log(const char* tag, const char* message) {
  time(&now);
  timeinfo = localtime(&now);
  strftime(timestampBuffer, sizeof(timestampBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);

  Serial.print("[");
  Serial.print(timestampBuffer);
  Serial.print("]");
  Serial.print("[");
  Serial.print(tag);
  Serial.print("] ");
  Serial.println(message);
}

void Logger::Log(const char *tag, const String &message) {
  Log(tag, message.c_str());
}
