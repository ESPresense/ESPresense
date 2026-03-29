#ifndef ARDUINO_V3

#include "MultiNetwork.h"

#include <HeadlessWiFiSettings.h>
#include <WiFi.h>

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
#include <ETH.h>
#endif

#include "../../include/Logger.h"

namespace {

bool hasAddress(const IPAddress &ip) {
  return ip[0] != 0;
}

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)

constexpr int kNoEthernet = 0;

struct EthernetSettings {
  uint8_t address;
  int power;
  int mdc;
  int mdio;
  eth_phy_type_t type;
  eth_clock_mode_t clockMode;
};

const EthernetSettings ethernetBoards[] = {
  {},
  {1, 16, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN},
  {0, 12, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO17_OUT},
  {0, -1, 16, 17, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN},
  {0, 5, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO17_OUT},
  {0, 5, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO17_OUT},
  {1, -1, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO17_OUT},
  {0, 5, 23, 18, ETH_PHY_IP101, ETH_CLOCK_GPIO0_IN},
  {0, -1, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO17_OUT},
  {1, 5, 23, 18, ETH_PHY_IP101, ETH_CLOCK_GPIO0_IN},
  {0, 12, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO17_OUT},
  {0, 12, 23, 18, ETH_PHY_RTL8201, ETH_CLOCK_GPIO0_IN},
  {1, 12, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO17_OUT},
  {0, -1, 16, 17, ETH_PHY_RTL8201, ETH_CLOCK_GPIO0_IN},
};

constexpr size_t kEthernetBoardCount = sizeof(ethernetBoards) / sizeof(ethernetBoards[0]);

#endif

}  // namespace

bool MultiNetworkManager::supportsEthernet() const {
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  return true;
#else
  return false;
#endif
}

IPAddress MultiNetworkManager::localIP() const {
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  const auto ethIp = ETH.localIP();
  if (hasAddress(ethIp)) {
    return ethIp;
  }
#endif

  const auto wifiIp = WiFi.localIP();
  if (hasAddress(wifiIp)) {
    return wifiIp;
  }

  return IPAddress();
}

IPAddress MultiNetworkManager::dnsIP() const {
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  const auto ethDns = ETH.dnsIP();
  if (hasAddress(ethDns)) {
    return ethDns;
  }
#endif

  const auto wifiDns = WiFi.dnsIP();
  if (hasAddress(wifiDns)) {
    return wifiDns;
  }

  return IPAddress();
}

const char *MultiNetworkManager::getHostname() const {
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  if (hasAddress(ETH.localIP())) {
    return ETH.getHostname();
  }
#endif

  return WiFi.getHostname();
}

bool MultiNetworkManager::isOnline() const {
  const bool wifiOnline = hasAddress(WiFi.localIP()) && WiFi.status() == WL_CONNECTED;
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  return wifiOnline || hasAddress(ETH.localIP());
#else
  return wifiOnline;
#endif
}

bool MultiNetworkManager::initEthernet(int ethernetType) {
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  static bool ethernetInitialized = false;

  if (ethernetInitialized) {
    return true;
  }
  if (ethernetType <= kNoEthernet || ethernetType >= static_cast<int>(kEthernetBoardCount)) {
    return false;
  }

  const auto &settings = ethernetBoards[ethernetType];
  if (!ETH.begin(settings.address, settings.power, settings.mdc, settings.mdio, settings.type, settings.clockMode)) {
    return false;
  }

  ethernetInitialized = true;
  return true;
#else
  (void)ethernetType;
  return false;
#endif
}

bool MultiNetworkManager::connect(int ethernetType, int ethernet_wait_seconds, int wifi_wait_seconds, const char *hostname) {
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  if (ethernetType > kNoEthernet) {
    Log.print(F("Connecting to Ethernet"));

    const unsigned long start = millis();
    if (!initEthernet(ethernetType)) {
      Log.println(F(" init failed."));
    } else {
      ETH.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
      ETH.setHostname(hostname);
      while (!hasAddress(ETH.localIP()) && (ethernet_wait_seconds < 0 || (millis() - start) < static_cast<unsigned long>(ethernet_wait_seconds) * 1000UL)) {
        Log.print(".");
        delay(100);
      }

      if (hasAddress(ETH.localIP())) {
        Log.println(F(" success!"));
        return true;
      }

      Log.println(F(" failed."));
    }
  }
#else
  (void)ethernetType;
  (void)ethernet_wait_seconds;
#endif

  return HeadlessWiFiSettings.connect(true, wifi_wait_seconds);
}

MultiNetworkManager MultiNetwork;

#endif
