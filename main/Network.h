#pragma once
// WiFi station / captive-portal AP / (ESP32) RMII Ethernet on esp_netif + esp_wifi + esp_eth.
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace Network {
void Setup();  // nvs, netif, event loop, wifi driver
bool supportsEthernet();
// Bring up Ethernet (type > 0) or WiFi from the stored ssid/password. Falls into the captive
// portal (never returns; Improv or the web form saves credentials and restarts) when there is
// no ssid or the connection fails and portal is true.
bool connect(int ethernetType, int ethernetWaitSeconds, int wifiWaitSeconds, const std::string& hostname, bool portal = true);
bool isOnline();
std::string localIP();
std::string dnsIP();
std::string hostname();
std::string macAddress();  // "AA:BB:CC:DD:EE:FF"
int rssi();
int channel();
std::string bssid();
std::vector<std::pair<std::string, int>> scanNetworks();

// Called while waiting for a connection / while the portal is up; returns ms to sleep.
extern std::function<int()> onWaitLoop;
extern std::function<int()> onPortalWaitLoop;
}  // namespace Network
