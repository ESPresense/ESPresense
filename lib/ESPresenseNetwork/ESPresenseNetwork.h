#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else // ESP32
  #include <WiFi.h>
  #include <ETH.h>
#endif

#if !defined(SPI2_HOST)
  #define SPI2_HOST 1
#endif

#ifndef ETH_PHY_LAN8720
  #define ETH_PHY_LAN8720 ETH_PHY_MAX
#endif
#ifndef ETH_PHY_IP101
  #define ETH_PHY_IP101 ETH_PHY_MAX
#endif
#ifndef ETH_PHY_RTL8201
  #define ETH_PHY_RTL8201 ETH_PHY_MAX
#endif
#ifndef ETH_CLOCK_GPIO0_IN
  #define ETH_CLOCK_GPIO0_IN 0
#endif
#ifndef ETH_CLOCK_GPIO17_OUT
  #define ETH_CLOCK_GPIO17_OUT 0
#endif

#ifndef Network_h
#define Network_h

class ESPresenseNetworkClass
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

#define CONFIG_NUM_ETH_TYPES        15

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
#define CONFIG_ETH_LILYGO_LITE_RTL  11
#define CONFIG_ETH_ESP32_POE_A1     12
#define CONFIG_ETH_WESP32_RTL8201   13
#define CONFIG_ETH_ETH01_EVO_DM9051 14

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


typedef enum EthernetBusType {
  ETH_BUS_RMII,
  ETH_BUS_SPI
} ethernet_bus_type_t;

typedef struct EthernetSettings {
  ethernet_bus_type_t bus_type;
  uint8_t             eth_address;
  int                 eth_power;
  int                 eth_mdc;
  int                 eth_mdio;
  eth_phy_type_t      eth_type;
  int                 eth_clk_mode;
  int                 eth_cs;
  int                 eth_irq;
  int                 eth_rst;
  int                 eth_spi_host;
  int                 eth_sck;
  int                 eth_miso;
  int                 eth_mosi;
  uint8_t             eth_spi_freq_mhz;
} ethernet_settings;

const ethernet_settings ethernetBoards[] = {
  // None
  {
    ETH_BUS_RMII
  },

  // WT32-EHT01
  {
    ETH_BUS_RMII,      // bus_type
    1,                 // eth_address,
    16,                // eth_power,
    23,                // eth_mdc,
    18,                // eth_mdio,
    ETH_PHY_LAN8720,   // eth_type,
    ETH_CLOCK_GPIO0_IN // eth_clk_mode
  },

  // ESP32-POE
  {
    ETH_BUS_RMII,      // bus_type
    0,                  // eth_address,
    12,                  // eth_power,
    23,                  // eth_mdc,
    18,                  // eth_mdio,
    ETH_PHY_LAN8720,     // eth_type,
    ETH_CLOCK_GPIO17_OUT // eth_clk_mode
  },

   // WESP32
  {
    ETH_BUS_RMII,      // bus_type
    0,			              // eth_address,
    -1,			              // eth_power,
    16,			              // eth_mdc,
    17,			              // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO0_IN	  // eth_clk_mode
  },

  // QuinLed-ESP32-Ethernet
  {
    ETH_BUS_RMII,      // bus_type
    0,			              // eth_address,
    5,			              // eth_power,
    23,			              // eth_mdc,
    18,			              // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT	// eth_clk_mode
  },

  // TwilightLord-ESP32 Ethernet Shield
  {
    ETH_BUS_RMII,      // bus_type
    0,			              // eth_address,
    5,			              // eth_power,
    23,			              // eth_mdc,
    18,			              // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT	// eth_clk_mode
  },

  // ESP3DEUXQuattro
  {
    ETH_BUS_RMII,      // bus_type
    1,                    // eth_address,
    -1,                   // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT  // eth_clk_mode
  },

  // ESP32-ETHERNET-KIT-VE
  {
    ETH_BUS_RMII,      // bus_type
    0,                    // eth_address,
    5,                    // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_IP101,        // eth_type,
    ETH_CLOCK_GPIO0_IN    // eth_clk_mode
  },

  // LilyGO-T-ETH-POE
  {
    ETH_BUS_RMII,      // bus_type
    0,                    // eth_address,
    -1,                   // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT  // eth_clk_mode
  },
  
  // GL-inet GL-S10 v2.1 Ethernet
  {
    ETH_BUS_RMII,      // bus_type
    1,                    // eth_address,
    5,                    // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_IP101,        // eth_type,
    ETH_CLOCK_GPIO0_IN    // eth_clk_mode
  },

  // EST-PoE-32
  {
    ETH_BUS_RMII,      // bus_type
    0,                  // eth_address,
    12,                  // eth_power,
    23,                  // eth_mdc,
    18,                  // eth_mdio,
    ETH_PHY_LAN8720,     // eth_type,
    ETH_CLOCK_GPIO17_OUT // eth_clk_mode
  },

  // LilyGO-T-ETH-Lite
  {
    ETH_BUS_RMII,      // bus_type
    0,                    // eth_address,
    12,                   // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_RTL8201,      // eth_type,
    ETH_CLOCK_GPIO0_IN    // eth_clk_mode
  },

  // ESP32-POE_A1
  {
    ETH_BUS_RMII,      // bus_type
    1,                  // eth_address,
   12,                  // eth_power,
   23,                  // eth_mdc,
   18,                  // eth_mdio,
   ETH_PHY_LAN8720,     // eth_type,
   ETH_CLOCK_GPIO17_OUT // eth_clk_mode
  },

  // WESP32 Rev7+ (RTL8201)
  {
    ETH_BUS_RMII,      // bus_type
    0,                  // eth_address,
    -1,                 // eth_power,
    16,                 // eth_mdc,
    17,                 // eth_mdio,
    ETH_PHY_RTL8201,    // eth_type,
    ETH_CLOCK_GPIO0_IN  // eth_clk_mode
  },

  // ETH01-EVO (DM9051)
  {
    ETH_BUS_SPI,         // bus_type
    1,                   // eth_address (DM9051 default)
    -1,                  // eth_power (unused)
    -1,                  // eth_mdc (unused)
    -1,                  // eth_mdio (unused)
#if defined(ETH_PHY_DM9051)
    ETH_PHY_DM9051,      // eth_type
#else
    ETH_PHY_MAX,         // eth_type placeholder when DM9051 is unavailable
#endif
    ETH_CLOCK_GPIO0_IN,  // eth_clk_mode (unused)
    9,                   // eth_cs
    8,                   // eth_irq
    6,                   // eth_rst
    SPI2_HOST,           // eth_spi_host
    7,                   // eth_sck
    3,                   // eth_miso
    10,                  // eth_mosi
    8                    // eth_spi_freq_mhz
  }

};

extern ESPresenseNetworkClass ESPresenseNetwork;

#endif
