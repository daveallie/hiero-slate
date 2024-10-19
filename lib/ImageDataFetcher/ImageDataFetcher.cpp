#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ImageDataFetcher.h"

ImageDataFetcher::ImageDataFetcher(const char* ssid, const char* password, const IPAddress &dns1, const IPAddress &dns2) {
  this->ssid = ssid;
  this->password = password;
  this->dns1 = dns1;
  this->dns2 = dns2;
}

void ImageDataFetcher::Init() const {
  dns_setserver(0, dns1);
  dns_setserver(1, dns2);
  WiFi.config(IPAddress(10, 0, 0, 24), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 0, 0), dns1, dns2);
  WiFi.mode(WIFI_STA);
}

void ImageDataFetcher::Connect() const {
  Serial.println("connecting to wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  Serial.println("connected to wifi: " + WiFi.localIP().toString());
}

void ImageDataFetcher::Disconnect() const {
  WiFi.disconnect(true);
}

void ImageDataFetcher::PullDataCb(void (*startCb)(), void (*processPartCb)(const u8 data[], size_t size), void (*finishCb)(size_t size)) const {
  WiFiClient client;
  HTTPClient http;

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
  u8 data[1000] = {}; // Should be a number that divides 192000 and not too large or we'll run out of memory
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
