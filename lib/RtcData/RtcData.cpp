#include <Arduino.h>
#include <Logger.h>
#include "RtcData.h"

static const char* TAG = "RTCDATA";

bool RtcData::ReadAndClear() {
  bool loaded = ESP.rtcUserMemoryRead(0, reinterpret_cast<u32 *>(&signedData), sizeof(signedData));

  if (!loaded) {
    Logger::Log(TAG, "Failed to read RTC data");
    signedData.rtcObject = {};
  } else {
    const u32 crcOfData = CalculateCRC32();
    if (crcOfData != signedData.crc32) {
      Logger::Log(TAG, "CRC32 in RTC memory doesn't match CRC32 of data. Discarding data.");
      signedData.rtcObject = {};
      loaded = false;
    } else {
      Logger::Log(TAG, "Successfully read RTC data");
    }
  }

  SignedRtcObject loadedData = signedData;
  this->signedData = {};
  Write();
  this->signedData = loadedData;

  return loaded;
}

bool RtcData::Write() {
  signedData.crc32 = CalculateCRC32();
  if (!ESP.rtcUserMemoryWrite(0, reinterpret_cast<u32 *>(&signedData), sizeof(signedData))) {
    return false;
  }

  Logger::Log(TAG, "Wrote RTC data");
  return false;
}

RtcObject* RtcData::Data() {
  return &signedData.rtcObject;
}

u32 RtcData::CalculateCRC32() const {
  u32 crc = 0xffffffff;
  const u8 *data = reinterpret_cast<const u8 *>(&signedData.rtcObject);
  size_t length = sizeof(signedData.rtcObject);

  while (length--) {
    const u8 c = *data++;
    for (u32 i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) { bit = !bit; }
      crc <<= 1;
      if (bit) { crc ^= 0x04c11db7; }
    }
  }

  return crc;
}
