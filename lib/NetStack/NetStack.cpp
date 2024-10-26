#include <ESP8266WiFi.h>
#include <Logger.h>
#include "NetStack.h"

static const char* TAG = "NETWORK";

NetStack::NetStack(const char* ssid, const char* password) {
  this->ssid = ssid;
  this->password = password;
}

bool NetStack::Connect() const {
  dns_setserver(0, IPAddress(8, 8, 8, 8));
  dns_setserver(1, IPAddress(8, 8, 4, 4));

  Logger::Log(TAG, "Connecting to wifi with SSID: " + String(ssid));
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (++retries > 100) {
      Logger::Log(TAG, "Failed to connect to wifi");
      Disconnect();
      return false;
    }
    delay(100);
  }
  Logger::Log(TAG, "Connected to wifi with IP: " + WiFi.localIP().toString());
  return true;
}

void NetStack::SetTime() {
  const char* ntpServer = "pool.ntp.org";

  configTime(0, 0, ntpServer);
  tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Logger::Log(TAG, "Failed to obtain time");
    return;
  }
  setenv("TZ", TIMEZONE, 1);
  tzset();

  Logger::Log(TAG, "Time obtained");
}

void NetStack::Disconnect() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Logger::Log(TAG, "Disconnected and turned off wifi");
}
