#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Logger.h>
#include "NetStack.h"

static const char* TAG = "NETWORK";
WiFiUDP udp;
NTPClient ntp(udp);

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
    // Timeout after 10 seconds
    if (++retries > 100) {
      Logger::Log(TAG, "Failed to connect to wifi");
      Disconnect();
      return false;
    }
    delay(100);
  }
  Logger::Log(TAG, "Connected to wifi with IP: " + WiFi.localIP().toString());

  ntp.begin();

  return true;
}

time_t NetStack::GetEpochTime() {
  if (!ntp.update()) {
    Logger::Log(TAG, "Failed to obtain time from NTP");
    return 0;
  }

  Logger::Log(TAG, "Time obtained from NTP");
  return ntp.getEpochTime();
}

void NetStack::Disconnect() {
  ntp.end();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Logger::Log(TAG, "Disconnected and turned off wifi");
}
