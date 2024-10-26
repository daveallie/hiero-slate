#ifndef RTCDATA_H
#define RTCDATA_H

#include <Arduino.h>

struct RtcObject {
  time_t lastKnownTime;
  time_t expectedWakeTime;
};

struct SignedRtcObject {
  uint32_t crc32;
  RtcObject rtcObject;
};

class RtcData {
public:
  bool ReadAndClear();
  bool Write();
  RtcObject* Data();

private:
  SignedRtcObject signedData {};
  u32 CalculateCRC32() const;
};

#endif
