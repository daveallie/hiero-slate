#ifndef NETSTACK_H
#define NETSTACK_H

class NetStack {
public:
  NetStack(const char* ssid, const char* password);
  bool Connect() const;
  static time_t GetEpochTime();
  static void Disconnect();

private:
  const char* ssid;
  const char* password;
};

#endif
