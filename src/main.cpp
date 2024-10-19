#include <Arduino.h>
#include <EPD_7in3e.h>
#include <ImageDataFetcher.h>

EPD_7in3e epd;
ImageDataFetcher idf("my-ssid", "my-password");

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

  if (EspClass::getResetInfoPtr()->reason == REASON_DEEP_SLEEP_AWAKE) {
    Serial.println("Woke up from deep sleep");
  } else {
    Serial.println("Woke up from reset / fresh boot");
  }

  // Setup e-paper and image data fetcher
  epd.Init();
  idf.Init();

  idf.Connect();
  idf.PullDataCb(renderRequestStart, renderRequestImagePart, renderRequestFinish);
  idf.Disconnect();

  epd.Sleep();
}

void loop() {
  // ESP.deepSleep(30 * 1000000);
}
