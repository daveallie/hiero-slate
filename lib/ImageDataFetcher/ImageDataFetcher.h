#ifndef IMAGEDATAFETCHER_H
#define IMAGEDATAFETCHER_H

#include <ESP8266WiFi.h>

#define DNS_SERVER_1 IPAddress(8, 8, 8, 8)
#define DNS_SERVER_2 IPAddress(8, 8, 4, 4)
#define IDF_URL "http://10.0.13.67:3000/news"

class ImageDataFetcher {
public:
  ImageDataFetcher(const char* ssid, const char* password, const IPAddress &dns1 = DNS_SERVER_1, const IPAddress &dns2 = DNS_SERVER_2);
  void Init() const;
  void Connect() const;
  void Disconnect() const;
  void PullDataCb(void (*startCb)(), void (*processPartCb)(const uint8 data[], size_t size), void (*finishCb)(size_t size)) const;

private:
  const char* ssid;
  const char* password;
  IPAddress dns1;
  IPAddress dns2;
};

#endif
