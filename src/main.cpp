#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EPD_7in3e.h>
#include <ImageDataFetcher.h>
#include <NetStack.h>

NetStack ns(WIFI_SSID, WIFI_PASSWORD);
EPD_7in3e epd;
ImageDataFetcher idf;

void renderRequestStart() {
  epd.StartRenderChunks();
}

void renderRequestImagePart(const u8 data[], const size_t size) {
  Serial.print('.');
  epd.SendRenderChunk(data, size);
}

void renderRequestFinish(const size_t total) {
  Serial.println("\nFetched " + String(total) + " bytes");
  epd.FinishRenderChunks();
}

void setup() {
  // Setup serial
  Serial.begin(115200);
  Serial.println("\n");
  Serial.println("Starting");

  if (ESP.getResetInfoPtr()->reason == REASON_DEEP_SLEEP_AWAKE) {
    Serial.println("Woke up from deep sleep");
  } else {
    Serial.println("Woke up from reset / fresh boot");
  }

  // Connect to network
  if (!ns.Connect()) {
    return;
  }

  // Set device time
  ns.SetTime();

  // Initialize display, fetch and display frame
  epd.Init();
  idf.PullDataCb(renderRequestStart, renderRequestImagePart, renderRequestFinish);

  // Sleep the display
  epd.Sleep();

  // Disconnect from network
  ns.Disconnect();
}

void loop() {
  // ESP.deepSleep(30 * 1000000);
}
