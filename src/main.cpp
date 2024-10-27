#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EPD_7in3e.h>
#include <ImageDataFetcher.h>
#include <NetStack.h>
#include <Logger.h>
#include <RtcData.h>
#include <TimeUtils.h>

bool timeSet = false;

static const char* TAG = "CORE";
NetStack ns(WIFI_SSID, WIFI_PASSWORD);
EPD_7in3e epd;
ImageDataFetcher idf;
RtcData rtcData;

const char* crons[4] = {
  "0 0 0-5,23 * * *", // On the hour from 11pm to 5am
  "0 0/30 6,22 * * *", // Every half-hour for 6am and 10pm
  "0 0/15 7,20,21 * * *", // Every quarter-hour for 7am, 8pm, and 9pm
  "0 0/10 8-19 * * *", // Every 10 minutes for 8am-7:50pm
};

void flashLight(int times = 1, int delayMs = 200) {
  int previousState = digitalRead(0);

  for (int i = 0; i < times; i++) {
    digitalWrite(0, !previousState);
    delay(delayMs);
    digitalWrite(0, previousState);
    delay(delayMs);
  }
}

void renderRequestStart() {
  epd.StartRenderChunks();
}

void renderRequestImagePart(const u8 data[], const size_t size) {
  epd.SendRenderChunk(data, size);
}

void refreshingFn(unsigned long ms) {
  flashLight(1, 50);
}

void renderRequestFinish() {
  epd.RefreshDisplay(refreshingFn);
}

void setup() {
  // Turn on light
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  // Setup serial
  Serial.begin(115200);

  Serial.println("\n");
  Logger::Log(TAG, "Starting");

  if (ESP.getResetInfoPtr()->reason == REASON_DEEP_SLEEP_AWAKE) {
    Logger::Log(TAG, "Woke up from deep sleep");
    if (rtcData.ReadAndClear()) {
      TimeUtils::SetDeviceTime(rtcData.Data()->expectedWakeTime);
    }
  } else {
    Logger::Log(TAG, "Woke up from reset / fresh boot");
  }

  // Connect to network
  if (!ns.Connect()) {
    flashLight(3);
    return;
  }

  // Set device time
  const time_t epochTime = ns.GetEpochTime();
  if (epochTime != 0) {
    TimeUtils::SetDeviceTime(epochTime);
    timeSet = true;
  }

  // Initialize display, fetch and display frame
  epd.Init();
  idf.PullDataCb(renderRequestStart, renderRequestImagePart, renderRequestFinish);

  // Sleep the display
  epd.Sleep();

  // Disconnect from network
  ns.Disconnect();
}

time_t getSleepUntil(time_t minTime, time_t defaultTime) {
  if (!timeSet) {
    Logger::Log(TAG, "Time was not set using network, cron can't be used");
    return defaultTime;
  }

  // Wait at least minTime and get the next cron time
  time_t next = TimeUtils::NextMultiCronTime(crons, 4, minTime);
  if (next == 0) {
    Logger::Log(TAG, "Failed to calculate next cron time");
    return defaultTime;
  }

  return next;
}

void loop() {
  time_t now = time(nullptr);
  time_t next = getSleepUntil(now + 7 * 60, now + 10 * 60);

  rtcData.Data()->lastKnownTime = now;
  rtcData.Data()->expectedWakeTime = next;
  rtcData.Write();

  Logger::Log(TAG, "Going to sleep for until " + TimeUtils::ISOString(&next) + " (" + TimeUtils::TimeDiffString(now, next) + ")");

  // Turn off light
  digitalWrite(0, HIGH);
  ESP.deepSleep((next - now) * 1000000);
}
