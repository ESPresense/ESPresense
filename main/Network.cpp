#include "Network.h"

#include <lwip/dns.h>
#include <lwip/sockets.h>
#include <string.h>

#include "HttpWebServer.h"
#include "Logger.h"
#include "Settings.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "dhcpserver/dhcpserver.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "util.h"
#ifdef USE_ETHERNET
#include "esp_eth.h"
#include "esp_eth_mac_esp.h"
#endif

namespace Network {

std::function<int()> onWaitLoop;
std::function<int()> onPortalWaitLoop;

namespace {
esp_netif_t* staNetif = nullptr;
esp_netif_t* apNetif = nullptr;
esp_netif_t* ethNetif = nullptr;
esp_netif_t* activeNetif = nullptr;
EventGroupHandle_t events;
constexpr int GOT_IP = BIT0, LINK_UP = BIT1, WIFI_STARTED = BIT2;
std::string hostName = "esp32";
bool wifiStarted = false;
volatile bool staConnecting = false;  // between esp_wifi_connect() and CONNECTED/DISCONNECTED

void onWifi(void*, esp_event_base_t base, int32_t id, void* data) {
    if (base == WIFI_EVENT) {
        switch (id) {
            case WIFI_EVENT_STA_START:
                xEventGroupSetBits(events, WIFI_STARTED);
                break;
            case WIFI_EVENT_STA_CONNECTED:
                staConnecting = false;
                xEventGroupSetBits(events, LINK_UP);
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                staConnecting = false;
                xEventGroupClearBits(events, LINK_UP | GOT_IP);
                break;
        }
    } else if (base == IP_EVENT && (id == IP_EVENT_STA_GOT_IP || id == IP_EVENT_ETH_GOT_IP)) {
        activeNetif = ((ip_event_got_ip_t*)data)->esp_netif;
        xEventGroupSetBits(events, GOT_IP);
    }
#ifdef USE_ETHERNET
    else if (base == ETH_EVENT) {
        if (id == ETHERNET_EVENT_CONNECTED) xEventGroupSetBits(events, LINK_UP);
        if (id == ETHERNET_EVENT_DISCONNECTED) xEventGroupClearBits(events, LINK_UP | GOT_IP);
    }
#endif
}

std::string ipToStr(const esp_ip4_addr_t& ip) {
    char buf[16];
    esp_ip4addr_ntoa(&ip, buf, sizeof(buf));
    return buf;
}

// Wait for an IP, running the caller's wait loop for LEDs/Improv. -1 = forever.
bool waitForIp(int seconds, std::function<void()> kick, unsigned kickEveryMs) {
    unsigned long start = millis(), lastKick = millis();
    while (!(xEventGroupGetBits(events) & GOT_IP)) {
        if (kick && millis() - lastKick > kickEveryMs) {
            lastKick = millis();
            kick();
        }
        delay(onWaitLoop ? onWaitLoop() : 100);
        if (seconds >= 0 && millis() - start > (unsigned long)seconds * 1000UL) return false;
    }
    return true;
}

// Captive-portal DNS: answer every A query with our AP address.
void dnsTask(void* arg) {
    uint32_t apIp = (uint32_t)(uintptr_t)arg;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(53);
    bind(sock, (sockaddr*)&addr, sizeof(addr));
    uint8_t buf[512];
    while (true) {
        sockaddr_in from;
        socklen_t fromLen = sizeof(from);
        int len = recvfrom(sock, buf, sizeof(buf) - 16, 0, (sockaddr*)&from, &fromLen);
        if (len < 12) continue;
        buf[2] = 0x81; buf[3] = 0x80;          // standard response, no error
        buf[6] = buf[4]; buf[7] = buf[5];      // ancount = qdcount
        buf[8] = buf[9] = buf[10] = buf[11] = 0;
        int i = 12;
        while (i < len && buf[i]) i += buf[i] + 1;  // skip qname
        i += 5;                                     // null + qtype + qclass
        if (i > len) continue;
        uint8_t answer[] = {0xc0, 0x0c, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0};
        memcpy(answer + 12, &apIp, 4);
        memcpy(buf + i, answer, sizeof(answer));
        sendto(sock, buf, i + sizeof(answer), 0, (sockaddr*)&from, fromLen);
    }
}

[[noreturn]] void portal() {
    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_AP);
    wifi_config_t ap = {};
    strncpy((char*)ap.ap.ssid, hostName.c_str(), sizeof(ap.ap.ssid) - 1);
    ap.ap.ssid_len = strlen((char*)ap.ap.ssid);
    ap.ap.max_connection = 4;
    ap.ap.authmode = WIFI_AUTH_OPEN;
    esp_wifi_set_config(WIFI_IF_AP, &ap);
    esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);
    // Hand ourselves out as the DNS server so the phone's captive-portal probe lands on us.
    {
        esp_netif_ip_info_t apInfo;
        esp_netif_get_ip_info(apNetif, &apInfo);
        esp_netif_dhcps_stop(apNetif);
        esp_netif_dns_info_t dns = {};
        dns.ip.type = ESP_IPADDR_TYPE_V4;
        dns.ip.u_addr.ip4.addr = apInfo.ip.addr;
        esp_netif_set_dns_info(apNetif, ESP_NETIF_DNS_MAIN, &dns);
        uint8_t offerDns = OFFER_DNS;
        esp_netif_dhcps_option(apNetif, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &offerDns, sizeof(offerDns));
        // RFC 8910 captive-portal URI (DHCP option 114): phones open the portal without probing.
        static char portalUri[32];
        snprintf(portalUri, sizeof(portalUri), "http://%s/", ipToStr(apInfo.ip).c_str());
        esp_netif_dhcps_option(apNetif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVEPORTAL_URI, portalUri, strlen(portalUri));
        esp_netif_dhcps_start(apNetif);
    }
    Log.println("Starting access point for configuration portal.");
    Log.printf("SSID: '%s'\n", hostName.c_str());
    if (esp_wifi_start() != ESP_OK) Log.println("Failed to start access point!");
    delay(500);

    esp_netif_ip_info_t info;
    esp_netif_get_ip_info(apNetif, &info);
    Log.printf("IP: %s\n", ipToStr(info.ip).c_str());
    xTaskCreate(dnsTask, "dns", 2560, (void*)(uintptr_t)info.ip.addr, 1, nullptr);
    activeNetif = apNetif;
    HttpWebServer::Init(true);

    while (true) {
        delay(onPortalWaitLoop ? onPortalWaitLoop() : 100);
    }
}

#ifdef USE_ETHERNET
struct EthBoard {
    uint8_t addr;
    int power, mdc, mdio;
    int phy;  // 0 LAN8720, 1 IP101, 2 RTL8201
    bool clkOut;  // true: 50MHz out on GPIO17, false: in on GPIO0
};
const EthBoard ethBoards[] = {
    {},
    {1, 16, 23, 18, 0, false},  // WT32-ETH01
    {0, 12, 23, 18, 0, true},   // ESP32-POE
    {0, -1, 16, 17, 0, false},  // WESP32
    {0, 5, 23, 18, 0, true},    // QuinLED-ESP32
    {0, 5, 23, 18, 0, true},    // TwilightLord-ESP32
    {1, -1, 23, 18, 0, true},   // ESP32Deux
    {0, 5, 23, 18, 1, false},   // KIT-VE
    {0, -1, 23, 18, 0, true},   // LilyGO-T-ETH-POE
    {1, 5, 23, 18, 1, false},   // GL-inet GL-S10 v2.1
    {0, 12, 23, 18, 0, true},   // EST-PoE-32
    {0, 12, 23, 18, 2, false},  // LilyGO-T-ETH-Lite (RTL8201)
    {1, 12, 23, 18, 0, true},   // ESP32-POE_A1
    {0, -1, 16, 17, 2, false},  // WESP32 Rev7+ (RTL8201)
};

bool initEthernet(int type) {
    if (type <= 0 || type >= (int)(sizeof(ethBoards) / sizeof(ethBoards[0]))) return false;
    const auto& b = ethBoards[type];
    if (b.power >= 0) {
        pinMode(b.power, OUTPUT);
        digitalWrite(b.power, HIGH);
        delay(100);
    }
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t emac = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    emac.smi_gpio.mdc_num = b.mdc;
    emac.smi_gpio.mdio_num = b.mdio;
    emac.clock_config.rmii.clock_mode = b.clkOut ? EMAC_CLK_OUT : EMAC_CLK_EXT_IN;
    emac.clock_config.rmii.clock_gpio = b.clkOut ? EMAC_APPL_CLK_OUT_GPIO : EMAC_CLK_IN_GPIO;
    esp_eth_mac_t* mac = esp_eth_mac_new_esp32(&emac, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = b.addr;
    phy_config.reset_gpio_num = -1;
    esp_eth_phy_t* phy = b.phy == 1 ? esp_eth_phy_new_ip101(&phy_config)
                       : b.phy == 2 ? esp_eth_phy_new_rtl8201(&phy_config)
                                    : esp_eth_phy_new_lan87xx(&phy_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t handle = nullptr;
    if (esp_eth_driver_install(&config, &handle) != ESP_OK) return false;
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    ethNetif = esp_netif_new(&cfg);
    esp_netif_set_hostname(ethNetif, hostName.c_str());
    esp_netif_attach(ethNetif, esp_eth_new_netif_glue(handle));
    esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, onWifi, nullptr);
    return esp_eth_start(handle) == ESP_OK;
}
#endif
}  // namespace

void Setup() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
    events = xEventGroupCreate();
    esp_netif_init();
    esp_event_loop_create_default();
    staNetif = esp_netif_create_default_wifi_sta();
    apNetif = esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, onWifi, nullptr);
    esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, onWifi, nullptr);
}

bool supportsEthernet() {
#ifdef USE_ETHERNET
    return true;
#else
    return false;
#endif
}

bool connect(int ethernetType, int ethernetWaitSeconds, int wifiWaitSeconds, const std::string& hostname, bool portal) {
    hostName = hostname;
#ifdef USE_ETHERNET
    static bool ethStarted = false;
    if (ethernetType > 0) {
        if (!ethStarted) ethStarted = initEthernet(ethernetType);
        if (ethStarted) {
            Log.print("Connecting to Ethernet");
            if (waitForIp(ethernetWaitSeconds, nullptr, 0)) {
                Log.println(localIP());
                return true;
            }
            Log.println(" failed, trying WiFi");
        }
    }
#endif
    std::string ssid = Settings::slurp("/wifi-ssid");
    std::string pw = Settings::slurp("/wifi-password");
    if (ssid.empty()) {
        Log.println("First contact!\n");
        if (portal) Network::portal();
        return false;
    }

    Log.printf("Connecting to WiFi SSID '%s'", ssid.c_str());
    wifi_config_t sta = {};
    strncpy((char*)sta.sta.ssid, ssid.c_str(), sizeof(sta.sta.ssid) - 1);
    strncpy((char*)sta.sta.password, pw.c_str(), sizeof(sta.sta.password) - 1);
    sta.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
    sta.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
    if (wifiStarted) esp_wifi_disconnect();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &sta);
    esp_netif_set_hostname(staNetif, hostName.c_str());
    if (!wifiStarted) {
        esp_wifi_start();
        wifiStarted = true;
        xEventGroupWaitBits(events, WIFI_STARTED, pdFALSE, pdTRUE, pdMS_TO_TICKS(2000));
    }
    staConnecting = esp_wifi_connect() == ESP_OK;

    // Re-issue connect after each failed attempt; the driver does not retry on its own here.
    bool ok = waitForIp(wifiWaitSeconds, [] {
        if (!(xEventGroupGetBits(events) & LINK_UP) && !staConnecting) {
            Log.print(".");
            staConnecting = esp_wifi_connect() == ESP_OK;
        }
    }, 1000);

    if (!ok) {
        Log.println(" failed.");
        if (portal) Network::portal();
        return false;
    }
    Log.println(localIP());
    return true;
}

bool isOnline() { return (xEventGroupGetBits(events) & GOT_IP) != 0; }

std::string localIP() {
    if (!activeNetif) return "0.0.0.0";
    esp_netif_ip_info_t info;
    esp_netif_get_ip_info(activeNetif, &info);
    return ipToStr(info.ip);
}

std::string dnsIP() {
    if (!activeNetif) return "0.0.0.0";
    esp_netif_dns_info_t dns;
    esp_netif_get_dns_info(activeNetif, ESP_NETIF_DNS_MAIN, &dns);
    return ipToStr(dns.ip.u_addr.ip4);
}

std::string hostname() { return hostName; }

std::string macAddress() {
    uint8_t mac[6] = {0};
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    return Sprintf("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

int rssi() {
    wifi_ap_record_t ap;
    return esp_wifi_sta_get_ap_info(&ap) == ESP_OK ? ap.rssi : 0;
}

int channel() {
    wifi_ap_record_t ap;
    return esp_wifi_sta_get_ap_info(&ap) == ESP_OK ? ap.primary : 0;
}

std::string bssid() {
    wifi_ap_record_t ap;
    if (esp_wifi_sta_get_ap_info(&ap) != ESP_OK) return "00:00:00:00:00:00";
    return Sprintf("%02X:%02X:%02X:%02X:%02X:%02X", ap.bssid[0], ap.bssid[1], ap.bssid[2], ap.bssid[3], ap.bssid[4], ap.bssid[5]);
}

std::vector<std::pair<std::string, int>> scanNetworks() {
    std::vector<std::pair<std::string, int>> out;
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if (mode == WIFI_MODE_AP) esp_wifi_set_mode(WIFI_MODE_APSTA);
    wifi_scan_config_t cfg = {};
    if (esp_wifi_scan_start(&cfg, true) != ESP_OK) return out;
    uint16_t n = 0;
    esp_wifi_scan_get_ap_num(&n);
    if (n == 0) return out;
    std::vector<wifi_ap_record_t> recs(n);
    esp_wifi_scan_get_ap_records(&n, recs.data());
    for (uint16_t i = 0; i < n; i++) {
        std::string ssid = (const char*)recs[i].ssid;
        if (ssid.empty()) continue;
        bool found = false;
        for (auto& e : out)
            if (e.first == ssid) {
                if (recs[i].rssi > e.second) e.second = recs[i].rssi;
                found = true;
                break;
            }
        if (!found) out.emplace_back(ssid, recs[i].rssi);
    }
    return out;
}

}  // namespace Network
