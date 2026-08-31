#ifdef ARDUINO_V3

#include "MultiNetwork.h"

#include <HeadlessWiFiSettings.h>
#include <Network.h>
#include <WiFi.h>

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
#include <ETH.h>
#endif

#include "../../include/Logger.h"

namespace {

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)

constexpr int kNoEthernet = 0;

struct EthernetSettings {
  eth_phy_type_t type;
  int32_t address;
  int mdc;
  int mdio;
  int power;
  eth_clock_mode_t clockMode;
};

// Arduino v3 ETH.begin() parameter order: type, phy_addr, mdc, mdio, power, clk_mode
const EthernetSettings ethernetBoards[] = {
  {},
  {ETH_PHY_LAN8720, 1, 23, 18, 16, ETH_CLOCK_GPIO0_IN},      // WT32-ETH01
  {ETH_PHY_LAN8720, 0, 23, 18, 12, ETH_CLOCK_GPIO17_OUT},     // ESP32-POE
  {ETH_PHY_LAN8720, 0, 16, 17, -1, ETH_CLOCK_GPIO0_IN},       // WESP32
  {ETH_PHY_LAN8720, 0, 23, 18, 5, ETH_CLOCK_GPIO17_OUT},      // QuinLED-ESP32
  {ETH_PHY_LAN8720, 0, 23, 18, 5, ETH_CLOCK_GPIO17_OUT},      // TwilightLord-ESP32
  {ETH_PHY_LAN8720, 1, 23, 18, -1, ETH_CLOCK_GPIO17_OUT},     // ESP3DEUXQuattro
  {ETH_PHY_IP101, 0, 23, 18, 5, ETH_CLOCK_GPIO0_IN},          // ESP32-ETHERNET-KIT-VE
  {ETH_PHY_LAN8720, 0, 23, 18, -1, ETH_CLOCK_GPIO17_OUT},     // LilyGO-T-ETH-POE
  {ETH_PHY_IP101, 1, 23, 18, 5, ETH_CLOCK_GPIO0_IN},          // GL-inet GL-S10 v2.1
  {ETH_PHY_LAN8720, 0, 23, 18, 12, ETH_CLOCK_GPIO17_OUT},     // EST-PoE-32
  {ETH_PHY_RTL8201, 0, 23, 18, 12, ETH_CLOCK_GPIO0_IN},       // LilyGO-T-ETH-Lite
  {ETH_PHY_LAN8720, 1, 23, 18, 12, ETH_CLOCK_GPIO17_OUT},     // ESP32-POE_A1
  {ETH_PHY_RTL8201, 0, 16, 17, -1, ETH_CLOCK_GPIO0_IN},       // WESP32 Rev7+ (RTL8201)
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
  auto *iface = Network.getDefaultInterface();
  return iface ? iface->localIP() : IPAddress();
}

IPAddress MultiNetworkManager::dnsIP() const {
  auto *iface = Network.getDefaultInterface();
  return iface ? iface->dnsIP() : IPAddress();
}

const char *MultiNetworkManager::getHostname() const {
  return Network.getHostname();
}

bool MultiNetworkManager::isOnline() const {
  auto *iface = Network.getDefaultInterface();
  return iface && iface->connected() && iface->hasIP();
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

  const auto &s = ethernetBoards[ethernetType];
  if (!ETH.begin(s.type, s.address, s.mdc, s.mdio, s.power, s.clockMode)) {
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
  if (hostname && hostname[0] != '\0') {
    Network.setHostname(hostname);
  }

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  if (ethernetType > kNoEthernet) {
    Log.print(F("Connecting to Ethernet"));

    const unsigned long start = millis();
    if (!initEthernet(ethernetType)) {
      Log.println(F(" init failed."));
    } else {
      ETH.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
      while (!ETH.hasIP() && (ethernet_wait_seconds < 0 || (millis() - start) < static_cast<unsigned long>(ethernet_wait_seconds) * 1000UL)) {
        Log.print(".");
        delay(100);
      }

      if (ETH.hasIP()) {
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
