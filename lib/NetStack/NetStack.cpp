#include <ESP8266WiFi.h>
#include "NetStack.h"

NetStack::NetStack(const char* ssid, const char* password) {
  this->ssid = ssid;
  this->password = password;
}

bool NetStack::Connect() const {
  dns_setserver(0, IPAddress(8, 8, 8, 8));
  dns_setserver(1, IPAddress(8, 8, 4, 4));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (++retries > 100) {
      Serial.println("failed to connect to wifi");
      Disconnect();
      return false;
    }
    delay(100);
  }
  Serial.println("connected to wifi: " + WiFi.localIP().toString());
  return true;
}

void NetStack::SetTime() {
  const char* ntpServer = "pool.ntp.org";
  // TODO: Make configurable
  const long  gmtOffset_sec = 36000;
  const int   daylightOffset_sec = gmtOffset_sec + 3600;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void NetStack::Disconnect() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
