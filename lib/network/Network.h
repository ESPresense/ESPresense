#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else // ESP32
  #include <WiFi.h>
  #include <ETH.h>
#endif

#ifndef Network_h
#define Network_h

class NetworkClass
{
public:
  IPAddress localIP();
  IPAddress subnetMask();
  IPAddress gatewayIP();
  IPAddress dnsIP();
  const char *getHostname();
  bool isConnected();
  bool isEthernet();
  bool initEthernet(int ethernetType);
  bool connect(int ethernetType, int wait_seconds, const char *hostName);
};

#define CONFIG_NUM_ETH_TYPES        11

#define CONFIG_ETH_NONE             0
#define CONFIG_ETH_WT32_ETH01       1
#define CONFIG_ETH_ESP32_POE        2
#define CONFIG_ETH_WESP32           3
#define CONFIG_ETH_QUINLED          4
#define CONFIG_ETH_TWILIGHTLORD     5
#define CONFIG_ETH_ESP32DEUX        6
#define CONFIG_ETH_ETHERNET_KIT_VE  7
#define CONFIG_ETH_LILYGO           8
#define CONFIG_ETH_GLINET_S10_V21   9
#define CONFIG_ETH_EST_POE_32       10

// For ESP32, the remaining five pins are at least somewhat configurable.
// eth_address  is in range [0..31], indicates which PHY (MAC?) address should be allocated to the interface
// eth_power    is an output GPIO pin used to enable/disable the ethernet port (and/or external oscillator)
// eth_mdc      is an output GPIO pin used to provide the clock for the management data
// eth_mdio     is an input/output GPIO pin used to transfer management data
// eth_type     is the physical ethernet module's type (ETH_PHY_LAN8720, ETH_PHY_TLK110)
// eth_clk_mode defines the GPIO pin and GPIO mode for the clock signal
//              However, there are really only four configurable options on ESP32:
//              ETH_CLOCK_GPIO0_IN    == External oscillator, clock input  via GPIO0
//              ETH_CLOCK_GPIO0_OUT   == ESP32 provides 50MHz clock output via GPIO0
//              ETH_CLOCK_GPIO16_OUT  == ESP32 provides 50MHz clock output via GPIO16
//              ETH_CLOCK_GPIO17_OUT  == ESP32 provides 50MHz clock output via GPIO17
typedef struct EthernetSettings {
  uint8_t        eth_address;
  int            eth_power;
  int            eth_mdc;
  int            eth_mdio;
  eth_phy_type_t eth_type;
  eth_clock_mode_t eth_clk_mode;
} ethernet_settings;

const ethernet_settings ethernetBoards[] = {
  // None
  {
  },

  // WT32-EHT01
  {
    1,                 // eth_address,
    16,                // eth_power,
    23,                // eth_mdc,
    18,                // eth_mdio,
    ETH_PHY_LAN8720,   // eth_type,
    ETH_CLOCK_GPIO0_IN // eth_clk_mode
  },

  // ESP32-POE
  {
     0,                  // eth_address,
    12,                  // eth_power,
    23,                  // eth_mdc,
    18,                  // eth_mdio,
    ETH_PHY_LAN8720,     // eth_type,
    ETH_CLOCK_GPIO17_OUT // eth_clk_mode
  },

   // WESP32
  {
    0,			              // eth_address,
    -1,			              // eth_power,
    16,			              // eth_mdc,
    17,			              // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO0_IN	  // eth_clk_mode
  },

  // QuinLed-ESP32-Ethernet
  {
    0,			              // eth_address,
    5,			              // eth_power,
    23,			              // eth_mdc,
    18,			              // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT	// eth_clk_mode
  },

  // TwilightLord-ESP32 Ethernet Shield
  {
    0,			              // eth_address,
    5,			              // eth_power,
    23,			              // eth_mdc,
    18,			              // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT	// eth_clk_mode
  },

  // ESP3DEUXQuattro
  {
    1,                    // eth_address,
    -1,                   // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT  // eth_clk_mode
  },

  // ESP32-ETHERNET-KIT-VE
  {
    0,                    // eth_address,
    5,                    // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_IP101,        // eth_type,
    ETH_CLOCK_GPIO0_IN    // eth_clk_mode
  },

  // LilyGO-T-ETH-POE
  {
    0,                    // eth_address,
    -1,                   // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT  // eth_clk_mode
  },
  
  // GL-inet GL-S10 v2.1 Ethernet
  {
    1,                    // eth_address,
    5,                    // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_IP101,        // eth_type,
    ETH_CLOCK_GPIO0_IN    // eth_clk_mode
  },

  // EST-PoE-32
  {
    0,                  // eth_address,
    12,                  // eth_power,
    23,                  // eth_mdc,
    18,                  // eth_mdio,
    ETH_PHY_LAN8720,     // eth_type,
    ETH_CLOCK_GPIO17_OUT // eth_clk_mode
  }
};

extern NetworkClass Network;

#endif
