#include "Network.h"
#include "../../include/Logger.h"

/**
 * @brief Returns the device's current local IP address, preferring Ethernet when configured.
 *
 * @return IPAddress The local IP address: the Ethernet IP if Ethernet is enabled and non-zero, otherwise the WiFi IP if non-zero, otherwise INADDR_NONE.
 */
IPAddress NetworkClass::localIP()
{
  IPAddress localIP;
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  localIP = ETH.localIP();
  if (localIP[0] != 0) {
    return localIP;
  }
#endif
  localIP = WiFi.localIP();
  if (localIP[0] != 0) {
    return localIP;
  }

  return INADDR_NONE;
}

IPAddress NetworkClass::subnetMask()
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  if (ETH.localIP()[0] != 0) {
    return ETH.subnetMask();
  }
#endif
  if (WiFi.localIP()[0] != 0) {
    return WiFi.subnetMask();
  }
  return IPAddress(255, 255, 255, 0);
}

IPAddress NetworkClass::gatewayIP()
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  if (ETH.localIP()[0] != 0) {
      return ETH.gatewayIP();
  }
#endif
  if (WiFi.localIP()[0] != 0) {
      return WiFi.gatewayIP();
  }
  return INADDR_NONE;
}

IPAddress NetworkClass::dnsIP()
{
  IPAddress dnsIP;
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  dnsIP = ETH.dnsIP();
  if (dnsIP[0] != 0) {
    return dnsIP;
  }
#endif
  dnsIP = WiFi.dnsIP();
  if (dnsIP[0] != 0) {
    return dnsIP;
  }

  return INADDR_NONE;
}

const char* NetworkClass::getHostname()
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  if (ETH.localIP()[0] != 0) {
    return ETH.getHostname();
  }
#endif
  return WiFi.getHostname();
}

bool NetworkClass::isConnected()
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  return (WiFi.localIP()[0] != 0 && WiFi.status() == WL_CONNECTED) || ETH.localIP()[0] != 0;
#else
  return (WiFi.localIP()[0] != 0 && WiFi.status() == WL_CONNECTED);
#endif
}

bool NetworkClass::isEthernet()
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  return (ETH.localIP()[0] != 0);
#endif
  return false;
}

bool NetworkClass::initEthernet(int ethernetType)
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)

  static bool successfullyConfiguredEthernet = false;

  if (successfullyConfiguredEthernet) {
    return false;
  }
  if (ethernetType == CONFIG_ETH_NONE) {
    return false;
  }
  if (ethernetType >= CONFIG_NUM_ETH_TYPES) {
    return false;
  }

  ethernet_settings es = ethernetBoards[ethernetType];
  if (!ETH.begin(
                (uint8_t) es.eth_address,
                (int)     es.eth_power,
                (int)     es.eth_mdc,
                (int)     es.eth_mdio,
                (eth_phy_type_t)   es.eth_type,
                (eth_clock_mode_t) es.eth_clk_mode
                )) {
    return false;
  }

  successfullyConfiguredEthernet = true;
  return true;
#else
  return false; // Ethernet not enabled for build
#endif
}

/**
 * @brief Attempt to bring up the Ethernet interface, set its hostname, and wait for an assigned IP address.
 *
 * Configures the Ethernet interface and blocks until a non-zero local IP is obtained or the timeout elapses.
 *
 * @param ethernetType Index identifying which Ethernet board configuration to initialize.
 * @param wait_seconds Maximum time to wait for an IP address in seconds; a negative value means wait indefinitely.
 * @param hostname NUL-terminated hostname to assign to the Ethernet interface.
 * @return true if the interface obtained a non-zero local IP address within the timeout, false otherwise.
 */
bool NetworkClass::connect(int ethernetType, int wait_seconds, const char* hostname, IPAddress staticIP, IPAddress gateway, IPAddress subnet, IPAddress dns)
{
    Log.print(F("Connecting to Ethernet"));

    unsigned long starttime = millis();
    initEthernet(ethernetType);
    if (staticIP[0] != 0 && gateway[0] != 0 && subnet[0] != 0) {
      if (dns[0] == 0) {
        dns = gateway;
      }
      ETH.config(staticIP, gateway, subnet, dns);
    } else {
      ETH.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    }
    ETH.setHostname(hostname);
    while (ETH.localIP()[0] == 0 && (wait_seconds < 0 || (millis() - starttime) < (unsigned)wait_seconds * 1000)) {
        Log.print(".");
        delay(100);
    }

    if (ETH.localIP()[0] == 0) {
        Log.println(F(" failed."));
        return false;
    }

    Log.println(F(" success!"));
    return true;
}

NetworkClass Network;