#include "Network.h"
#include "../../include/Logger.h"
#include <SPI.h>

/**
 * @brief Returns the device's current local IP address, preferring Ethernet when configured.
 *
 * @return IPAddress The local IP address: the Ethernet IP if Ethernet is enabled and non-zero, otherwise the WiFi IP if non-zero, otherwise INADDR_NONE.
 */
IPAddress NetworkManager::localIP()
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

IPAddress NetworkManager::dnsIP()
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

const char* NetworkManager::getHostname()
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  if (ETH.localIP()[0] != 0) {
    return ETH.getHostname();
  }
#endif
  return WiFi.getHostname();
}

void NetworkManager::setHostname(const char* hostname)
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  ETH.setHostname(hostname);
#endif
  WiFi.setHostname(hostname);
}

void NetworkManager::hostname(const String& hostname)
{
  setHostname(hostname.c_str());
}

bool NetworkManager::isOnline()
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)
  return (WiFi.localIP()[0] != 0 && WiFi.status() == WL_CONNECTED) || ETH.localIP()[0] != 0;
#else
  return (WiFi.localIP()[0] != 0 && WiFi.status() == WL_CONNECTED);
#endif
}

bool NetworkManager::initEthernet(int ethernetType)
{
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_USE_ETHERNET)

  static bool successfullyConfiguredEthernet = false;

  if (successfullyConfiguredEthernet) {
    return true;
  }
  if (ethernetType == CONFIG_ETH_NONE) {
    return false;
  }
  if (ethernetType >= CONFIG_NUM_ETH_TYPES) {
    return false;
  }

  ethernet_settings es = ethernetBoards[ethernetType];
  
  // Handle SPI Ethernet (W5500)
  if (es.use_spi) {
    // For W5500, the SPI pins (MISO, MOSI, SCLK) need to be initialized first
    // Initialize SPI with custom pins for Waveshare ESP32-S3-ETH
    SPI.begin(es.spi_sclk, es.spi_miso, es.spi_mosi, es.spi_cs);
    
    // For SPI Ethernet, use the begin method with SPI parameters
    // ETH.begin(eth_phy_type_t type, int32_t phy_addr, int cs, int irq, int rst, SPIClass &spi, uint8_t spi_freq_mhz)
    if (!ETH.begin(
                (eth_phy_type_t) es.eth_type,
                0,              // phy_addr (auto-detect for SPI)
                es.spi_cs,      // cs
                es.spi_int,     // irq
                es.eth_power,   // rst
                SPI,            // SPI instance
                20              // spi_freq_mhz (20 MHz)
                )) {
      return false;
    }
  } else {
    // Handle RMII Ethernet (LAN8720, RTL8201, etc.)
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
bool NetworkManager::connect(int ethernetType, int wait_seconds, const char* hostname)
{
    Log.print(F("Connecting to Ethernet"));

    unsigned long starttime = millis();
    if (!initEthernet(ethernetType)) {
        Log.println(F(" init failed."));
        return false;
    }
    ETH.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
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

NetworkManager Network;
