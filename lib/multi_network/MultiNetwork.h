#pragma once

#include <Arduino.h>

class MultiNetworkManager {
public:
  bool supportsEthernet() const;
  IPAddress localIP() const;
  IPAddress dnsIP() const;
  const char *getHostname() const;
  bool isOnline() const;
  bool connect(int ethernetType, int ethernet_wait_seconds, int wifi_wait_seconds, const char *hostName);

private:
  bool initEthernet(int ethernetType);
};

extern MultiNetworkManager MultiNetwork;
