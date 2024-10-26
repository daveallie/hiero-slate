#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ImageDataFetcher.h"
#include "rootca.h"

void ImageDataFetcher::PullDataCb(void (*startCb)(), void (*processPartCb)(const u8 data[], size_t size), void (*finishCb)(size_t size)) const {
  WiFiClientSecure client;
  HTTPClient http;

  // TODO - Make this configurable?
  client.setTrustAnchors(new X509List(rootca_der, rootca_der_len));

  if (!http.begin(client, IDF_URL)) {
    Serial.println("[HTTP] Unable to connect");
    return;
  }

  const int httpCode = http.GET();
  if (httpCode <= 0) {
    Serial.println("[HTTP] GET... failed, error: " + HTTPClient::errorToString(httpCode));
    return;
  }

  if (httpCode != HTTP_CODE_OK) {
    Serial.println("[HTTP] Got non-200 status code: " + String(httpCode));
    return;
  }

  size_t len = http.getSize();
  if (len != 192000) {
    Serial.println("[HTTP] Was expecting response of 192000 bytes, got: " + String(len));
    return;
  }

  // Mark the start of the data transfer
  startCb();

  WiFiClient* stream = &http.getStream();
  u8 data[1000] = {}; // Should not be too large or we'll run out of memory
  size_t read = 0;

  while (http.connected() && len > 0) {
    const size_t size = stream->readBytes(data, std::min(len, sizeof(data)));
    if (size) {
      len -= size;
      read += size;
      processPartCb(data, size);
    } else {
      Serial.println("[HTTP] Read timeout");
    }
  }

  http.end();

  finishCb(read);
}
