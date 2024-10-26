#ifndef LOGGER_H
#define LOGGER_H

#include <WString.h>

class Logger {
 public:
  static void Log(const char* tag, const char* message);
  static void Log(const char* tag, const String& message);
};

#endif
