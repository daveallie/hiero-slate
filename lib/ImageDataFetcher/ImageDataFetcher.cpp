#include "ImageDataFetcher.h"

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Logger.h>
#include <TimeUtils.h>

#include "rootca.h"

static const char* TAG = "IDF";

String buildHeaderValue() { return "time=" + TimeUtils::NowISOString(); }

void ImageDataFetcher::PullDataCb(void (*startCb)(),
                                  void (*processPartCb)(const u8 data[],
                                                        size_t size),
                                  void (*finishCb)()) const {
  WiFiClientSecure client;
  HTTPClient http;

  http.setTimeout(15000);

  // TODO - Make this configurable?
  client.setTrustAnchors(new X509List(rootca_der, rootca_der_len));

  Logger::Log(TAG, "Begin fetching image data from: " + String(IDF_URL));
  if (!http.begin(client, IDF_URL)) {
    Logger::Log(TAG, "Unable to connect");
    return;
  }

  http.addHeader("X-Device-Data", buildHeaderValue());

  const int httpCode = http.GET();
  if (httpCode <= 0) {
    Logger::Log(TAG,
                "GET failed, error: " + HTTPClient::errorToString(httpCode));
    return;
  }

  if (httpCode != HTTP_CODE_OK) {
    Logger::Log(TAG, "Got non-200 status code: " + String(httpCode));
    return;
  }

  size_t len = http.getSize();
  if (len != 192000) {
    Logger::Log(TAG,
                "Was expecting response of 192000 bytes, got: " + String(len));
    return;
  }

  // Mark the start of the data transfer
  startCb();

  WiFiClient* stream = &http.getStream();
  u8 data[1000] = {};  // Should not be too large or we'll run out of memory

  while (http.connected() && len > 0) {
    const size_t size = stream->readBytes(data, std::min(len, sizeof(data)));
    if (size) {
      len -= size;
      processPartCb(data, size);
    } else {
      Logger::Log(TAG, "Read timeout");
    }
  }

  http.end();
  Logger::Log(TAG, "Finished fetching response");

  finishCb();
}
