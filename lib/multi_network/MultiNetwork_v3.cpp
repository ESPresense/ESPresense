#ifdef ARDUINO_V3

#include "MultiNetwork.h"

#include <HeadlessWiFiSettings.h>
#include <Network.h>

namespace {

NetworkInterface *defaultInterface() {
  return Network.getDefaultInterface();
}

}  // namespace

bool MultiNetworkManager::supportsEthernet() const {
  return false;
}

IPAddress MultiNetworkManager::localIP() const {
  auto *iface = defaultInterface();
  return iface ? iface->localIP() : IPAddress();
}

IPAddress MultiNetworkManager::dnsIP() const {
  auto *iface = defaultInterface();
  return iface ? iface->dnsIP() : IPAddress();
}

const char *MultiNetworkManager::getHostname() const {
  return Network.getHostname();
}

bool MultiNetworkManager::isOnline() const {
  auto *iface = defaultInterface();
  return iface && iface->connected() && iface->hasIP();
}

bool MultiNetworkManager::initEthernet(int ethernetType) {
  (void)ethernetType;
  return false;
}

bool MultiNetworkManager::connect(int ethernetType, int ethernet_wait_seconds, int wifi_wait_seconds, const char *hostname) {
  (void)ethernetType;
  (void)ethernet_wait_seconds;
  if (hostname && hostname[0] != '\0') {
    Network.setHostname(hostname);
  }
  return HeadlessWiFiSettings.connect(true, wifi_wait_seconds);
}

MultiNetworkManager MultiNetwork;

#endif
