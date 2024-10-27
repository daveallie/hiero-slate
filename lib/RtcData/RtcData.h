#ifndef RTCDATA_H
#define RTCDATA_H

#include <c_types.h>

#include <ctime>

struct RtcObject {
  time_t lastKnownTime;
  time_t expectedWakeTime;
};

struct SignedRtcObject {
  u8 crc32;
  RtcObject rtcObject;
};

class RtcData {
 public:
  bool ReadAndClear();
  bool Write();
  RtcObject* Data();

 private:
  SignedRtcObject signedData{};
  u32 CalculateCRC32() const;
};

#endif
